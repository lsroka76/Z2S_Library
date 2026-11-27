#include "z2s_lua_engine.h"

#if Z2S_ENABLE_LUA

#include <Arduino.h>
#include <Preferences.h>
#include <cstdio>
#include <cstring>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <esp_timer.h>

#include <Z2S_solar.h>

#include "z2s_little_fs.h"

/*****************************************************************************/
// RTC_NOINIT_ATTR Boot-Loop Guard, Syntax Validator & Dispatch Trampoline
/*****************************************************************************/

namespace {

constexpr uint32_t Z2S_LUA_RTC_MAGIC = 0x5A324C55UL;  // "Z2LU"

struct Z2S_LuaRtcGuard {
  uint32_t magic;
  uint32_t magic_inv;
  uint8_t  in_lua_exec;
  uint8_t  crash_count;
  uint8_t  safe_mode_latched;
  uint8_t  reserved;
};

RTC_NOINIT_ATTR Z2S_LuaRtcGuard s_lua_rtc_guard;

static bool isAbnormalReset(esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_PANIC:
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT:
    case ESP_RST_BROWNOUT:
      return true;
    default:
      return false;
  }
}

struct DispatchArgs {
  const Z2S_LuaEvent* ev;
  bool                invoked;
};

static void pushEventArgs(lua_State* L, const Z2S_LuaEvent* ev) {
  lua_pushinteger(L, ev->channel);
  if (const char* evt_name = Z2S_LuaBridge::eventAlias(ev->event_id)) {
    lua_pushstring(L, evt_name);
  } else {
    lua_pushinteger(L, ev->event_id);
  }
}

// Runs inside lua_pcall() so lua_getglobal() and lua_pushstring() can never
// trigger an unprotected LUA_ERRMEM panic if the PSRAM arena is full.
static int dispatchEventTrampoline(lua_State* L) {
  auto* args = static_cast<DispatchArgs*>(lua_touserdata(L, 1));
  if (!args || !args->ev) return 0;

  // 1. Built-in prelude router (z2s.on / z2s.window / z2s.at)
  lua_getglobal(L, "z2s");
  if (lua_istable(L, -1)) {
    lua_getfield(L, -1, "_dispatch");
    if (lua_isfunction(L, -1)) {
      pushEventArgs(L, args->ev);
      lua_call(L, 2, 1);
      if (lua_toboolean(L, -1)) {
        args->invoked = true;
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }
  }
  lua_pop(L, 1);

  // 2. Optional raw user callback function on_event(ch, evt)
  lua_getglobal(L, "on_event");
  if (lua_isfunction(L, -1)) {
    args->invoked = true;
    pushEventArgs(L, args->ev);
    lua_call(L, 2, 0);
  } else {
    lua_pop(L, 1);
  }
  return 0;
}

static int syncWindowsTrampoline(lua_State* L) {
  lua_getglobal(L, "z2s");
  if (lua_istable(L, -1)) {
    lua_getfield(L, -1, "_sync_windows");
    if (lua_isfunction(L, -1)) {
      lua_call(L, 0, 0);
    } else {
      lua_pop(L, 1);
    }
  }
  lua_pop(L, 1);
  return 0;
}

// Auto-loaded into every fresh VM from Flash DROM (0 B user script budget, 0 B DRAM).
// Exposes z2s.on(), z2s.window(), and z2s.at() backed by C++ z2s.sun() & "minute".
static const char Z2S_LUA_PRELUDE[] PROGMEM = R"z2slua(
local routes, windows, events, watched = {}, {}, {}, {}
local sched_armed = false

local function watch_once(ch, evt)
  local k = tostring(ch) .. ":" .. tostring(evt)
  if not watched[k] then
    watched[k] = true
    z2s.watch(ch, evt)
  end
end

local function ensure_sched()
  if not sched_armed then
    sched_armed = true
    watch_once(z2s.GATEWAY, "minute")
  end
end

local function clamp(v, lo, hi)
  if v < lo then return lo end
  if v > hi then return hi end
  return v
end

local function parse_hhmm(s)
  local h, m = string.match(s, "^(%d+):(%d+)$")
  h, m = tonumber(h), tonumber(m)
  if not h or not m then return nil end
  return clamp(h * 60 + m, 0, 1439)
end

local function resolve_min(spec, t, sun)
  local tp = type(spec)
  if tp == "number" then return clamp(spec, 0, 1439) end
  if tp == "function" then
    local v = spec(t, sun)
    return v and clamp(v, 0, 1439) or nil
  end
  if tp == "string" then
    if spec == "sunrise" then return sun and sun.sunrise or nil end
    if spec == "sunset"  then return sun and sun.sunset  or nil end
    return parse_hhmm(spec)
  end
  if tp == "table" then
    local base = resolve_min(spec.ref, t, sun)
    if not base then return nil end
    local v = base + (spec.offset or 0)
    if spec.min then
      local lo = resolve_min(spec.min, t, sun)
      if lo then v = math.max(v, lo) end
    end
    if spec.max then
      local hi = resolve_min(spec.max, t, sun)
      if hi then v = math.min(v, hi) end
    end
    return clamp(v, 0, 1439)
  end
  return nil
end

local function spec_kind(s)
  local tp = type(s)
  if tp == "number" then return "fixed" end
  if tp == "string" then
    if s == "sunrise" or s == "sunset" then return s end
    return "fixed"
  end
  if tp == "table" then
    if s.ref == "sunrise" or s.ref == "sunset" then return s.ref end
    return "fixed"
  end
  return "other"
end

local function auto_overnight(s1, s2)
  local k1, k2 = spec_kind(s1), spec_kind(s2)
  return (k1 == "fixed" and k2 == "fixed") or (k1 == "sunset" and k2 == "sunrise")
end

local function match_wday(f, val)
  if f == nil then return true end
  if f == "workdays" then return val >= 1 and val <= 5 end
  if f == "weekend"  then return val >= 6 end
  if type(f) == "table" then
    for i = 1, #f do if f[i] == val then return true end end
    return false
  end
  return f == val
end

local function match_num(f, val)
  if f == nil then return true end
  if type(f) == "table" then
    for i = 1, #f do if f[i] == val then return true end end
    return false
  end
  return f == val
end

local function match_date(r, t)
  if not match_num(r.year, t.year) then return false end
  if not match_num(r.month, t.month) then return false end
  if not match_num(r.day, t.day) then return false end
  if not match_wday(r.weekday, t.weekday) then return false end
  if r.date_from and r.date_to then
    local mmdd = t.month * 100 + t.day
    if r.date_from <= r.date_to then
      if mmdd < r.date_from or mmdd > r.date_to then return false end
    else
      if mmdd < r.date_from and mmdd > r.date_to then return false end
    end
  end
  return true
end

local function in_window(now_m, s_m, e_m, ov)
  if s_m < e_m then return now_m >= s_m and now_m < e_m end
  if ov and s_m > e_m then return now_m >= s_m or now_m < e_m end
  return false
end

local function eval_windows(t, sun, now_m)
  for i = 1, #windows do
    local w = windows[i]
    local s_m = resolve_min(w.start, t, sun)
    local e_m = resolve_min(w.stop, t, sun)
    if s_m and e_m then
      local ov = w.overnight
      if ov == nil then ov = auto_overnight(w.start, w.stop) end
      local active  = match_date(w, t) and in_window(now_m, s_m, e_m, ov)
      local changed = (w._active ~= active)
      w._active = active

      if w.ch then
        local cur = z2s.get(w.ch)
        if cur ~= nil and cur ~= active then
          z2s.action(w.ch, active and "on" or "off")
        end
      elseif w.apply then
        local ok, err = pcall(w.apply, active, changed, t, sun)
        if not ok then print("[ERR " .. w.name .. "] " .. tostring(err)) end
      end
    end
  end
end

function z2s.on(ch, evt, fn, name)
  if type(evt) == "function" then
    name, fn, evt = fn, evt, "change"
  end
  if type(fn) ~= "function" then error("z2s.on: callback must be a function") end
  watch_once(ch, evt)
  routes[#routes + 1] = {
    ch = ch, evt = evt, fn = fn,
    name = name or (tostring(ch) .. ":" .. tostring(evt)),
  }
end

function z2s.window(target, start_spec, stop_spec, opts)
  local tp = type(target)
  if tp ~= "number" and tp ~= "function" then
    error("z2s.window: target must be channel # or function")
  end
  opts = opts or {}
  ensure_sched()
  windows[#windows + 1] = {
    ch        = (tp == "number") and target or nil,
    apply     = (tp == "function") and target or nil,
    start     = start_spec,
    stop      = stop_spec,
    overnight = opts.overnight,
    name      = opts.name or (tp == "number" and ("win_ch" .. target) or ("win#" .. (#windows + 1))),
    year      = opts.year, month = opts.month, day = opts.day, weekday = opts.weekday,
    date_from = opts.date_from, date_to = opts.date_to,
  }
end

function z2s.at(spec, a, b)
  local opts, fn
  if type(a) == "function" then opts, fn = {}, a else opts, fn = (a or {}), b end
  if type(fn) ~= "function" then error("z2s.at: callback must be a function") end
  ensure_sched()
  events[#events + 1] = {
    at = spec, fn = fn, name = opts.name or ("at#" .. (#events + 1)),
    year = opts.year, month = opts.month, day = opts.day, weekday = opts.weekday,
    date_from = opts.date_from, date_to = opts.date_to,
  }
end

function z2s._sync_windows()
  if #windows == 0 then return end
  local t = z2s.time()
  if not t then return end
  eval_windows(t, z2s.sun(), t.hour * 60 + t.minute)
end

function z2s._dispatch(ch, evt)
  local handled = false
  for i = 1, #routes do
    local r = routes[i]
    if r.ch == ch and r.evt == evt then
      handled = true
      local ok, err = pcall(r.fn, ch, evt)
      if not ok then print("[ERR " .. r.name .. "] " .. tostring(err)) end
    end
  end
  if sched_armed and ch == z2s.GATEWAY and evt == "minute" then
    local t = z2s.time()
    if t then
      handled = true
      local sun   = z2s.sun()
      local today = t.year * 10000 + t.month * 100 + t.day
      local now_m = t.hour * 60 + t.minute
      eval_windows(t, sun, now_m)
      for i = 1, #events do
        local e = events[i]
        if e._last_day ~= today and match_date(e, t) then
          local target_m = resolve_min(e.at, t, sun)
          if target_m and now_m >= target_m and now_m <= (target_m + 1) then
            e._last_day = today
            local ok, err = pcall(e.fn, t, sun)
            if not ok then print("[ERR " .. e.name .. "] " .. tostring(err)) end
          end
        end
      end
    end
  end
  return handled
end
)z2slua";

static void* luaSyntaxPsramAlloc(
    void* ud, void* ptr, size_t osize, size_t nsize) {
  auto heap = static_cast<multi_heap_handle_t>(ud);
  if (!heap) return nullptr;

  const size_t old_bytes = (ptr != nullptr) ? osize : 0;

  if (nsize == 0) {
    if (ptr != nullptr) {
      multi_heap_free(heap, ptr);
    }
    return nullptr;
  }

  void* new_ptr = (ptr == nullptr)
                      ? multi_heap_malloc(heap, nsize)
                      : multi_heap_realloc(heap, ptr, nsize);
  if (new_ptr == nullptr && ptr != nullptr && nsize <= old_bytes) {
    return ptr;
  }
  return new_ptr;
}

static void formatSyntaxError(
    const char* lua_err, char* out, size_t out_cap) {
  if (!out || out_cap == 0) return;
  if (!lua_err || lua_err[0] == '\0') {
    snprintf(out, out_cap, "Syntax error");
    return;
  }

  static constexpr char   kPrefix[]  = "z2s_main:";
  static constexpr size_t kPrefixLen = sizeof(kPrefix) - 1;

  if (strncmp(lua_err, kPrefix, kPrefixLen) == 0) {
    const char* rest  = lua_err + kPrefixLen;
    const char* colon = strchr(rest, ':');
    if (colon && colon > rest) {
      int line_len    = static_cast<int>(colon - rest);
      const char* msg = colon + 1;
      while (*msg == ' ') {
        ++msg;
      }
      snprintf(out, out_cap, "Syntax error (line %.*s): %s",
               line_len, rest, msg);
      return;
    }
  }
  snprintf(out, out_cap, "Syntax error: %s", lua_err);
}

}  // namespace

/*****************************************************************************/
// extern "C" print() sink referenced by luaconf.h
/*****************************************************************************/

extern "C" void z2s_lua_write(const char* s, size_t l) {
  if (!s || l == 0) return;
  Z2S_LuaEngine::instance().appendConsole(s, l);
}

/*****************************************************************************/
// RTC Guard & NVS Autostart Lifecycle
/*****************************************************************************/

void Z2S_LuaEngine::evaluateRtcGuardOnBoot() {
  esp_reset_reason_t reason = esp_reset_reason();
  bool magic_valid =
      (s_lua_rtc_guard.magic == Z2S_LUA_RTC_MAGIC) &&
      (s_lua_rtc_guard.magic_inv == ~Z2S_LUA_RTC_MAGIC);

  if (reason == ESP_RST_POWERON || !magic_valid) {
    s_lua_rtc_guard.magic             = Z2S_LUA_RTC_MAGIC;
    s_lua_rtc_guard.magic_inv         = ~Z2S_LUA_RTC_MAGIC;
    s_lua_rtc_guard.in_lua_exec       = 0;
    s_lua_rtc_guard.crash_count       = 0;
    s_lua_rtc_guard.safe_mode_latched = 0;
    s_lua_rtc_guard.reserved          = 0;
  } else {
    if (isAbnormalReset(reason) && s_lua_rtc_guard.in_lua_exec != 0) {
      if (s_lua_rtc_guard.crash_count < 255) {
        s_lua_rtc_guard.crash_count++;
      }
      log_e("[LUA] Detected crash inside Lua VM (reset=%d, count=%u/%u)",
            static_cast<int>(reason), s_lua_rtc_guard.crash_count,
            MAX_BOOT_CRASHES);

      if (s_lua_rtc_guard.crash_count >= MAX_BOOT_CRASHES) {
        s_lua_rtc_guard.safe_mode_latched = 1;
      }
    }
    s_lua_rtc_guard.in_lua_exec = 0;
  }

  taskENTER_CRITICAL(&_stats_mux);
  _crash_count = s_lua_rtc_guard.crash_count;
  _safe_mode   = (s_lua_rtc_guard.safe_mode_latched != 0);
  taskEXIT_CRITICAL(&_stats_mux);
}

void Z2S_LuaEngine::markExecEnter() {
  s_lua_rtc_guard.in_lua_exec = 1;
}

void Z2S_LuaEngine::markExecExit() {
  s_lua_rtc_guard.in_lua_exec = 0;
}

void Z2S_LuaEngine::maybeHealCrashCounter() {
  if (_crash_count > 0 && !_safe_mode && _script_loaded_ms != 0 &&
      (millis() - _script_loaded_ms) >= STABILITY_HEAL_MS) {
    s_lua_rtc_guard.crash_count = 0;
    taskENTER_CRITICAL(&_stats_mux);
    _crash_count = 0;
    taskEXIT_CRITICAL(&_stats_mux);
    log_i("[LUA] Script stable for %lu s -> RTC crash counter cleared",
          static_cast<unsigned long>(STABILITY_HEAL_MS / 1000));
  }
}

bool Z2S_LuaEngine::readAutostartPref() {
  Preferences prefs;
  if (!prefs.begin(NVS_AUTOSTART_NS, /*readOnly=*/true)) {
    return true;
  }
  bool enabled = prefs.getBool(NVS_AUTOSTART_KEY, true);
  prefs.end();
  return enabled;
}

void Z2S_LuaEngine::writeAutostartPref(bool enabled) {
  Preferences prefs;
  if (!prefs.begin(NVS_AUTOSTART_NS, /*readOnly=*/false)) {
    log_e("[LUA] Failed to open NVS '%s' for autostart write", NVS_AUTOSTART_NS);
    return;
  }
  if (prefs.getBool(NVS_AUTOSTART_KEY, true) != enabled) {
    prefs.putBool(NVS_AUTOSTART_KEY, enabled);
  }
  prefs.end();

  taskENTER_CRITICAL(&_stats_mux);
  _autostart_enabled = enabled;
  taskEXIT_CRITICAL(&_stats_mux);
}

/*****************************************************************************/
// Private PSRAM TLSF Allocator & Uncatchable Watchdog
/*****************************************************************************/

void* Z2S_LuaEngine::luaArenaAlloc(
    void* ud, void* ptr, size_t osize, size_t nsize) {
  auto* ctx = static_cast<SandboxCtx*>(ud);
  if (!ctx || !ctx->heap) {
    return nullptr;
  }

  // CRITICAL: When ptr == nullptr, Lua passes a type tag in osize, not byte count
  const size_t old_bytes = (ptr != nullptr) ? osize : 0;

  if (nsize == 0) {
    if (ptr != nullptr) {
      multi_heap_free(ctx->heap, ptr);
      ctx->used_bytes = (ctx->used_bytes >= old_bytes)
                            ? (ctx->used_bytes - old_bytes)
                            : 0;
    }
    return nullptr;
  }

  if (nsize > old_bytes) {
    const size_t delta = nsize - old_bytes;
    if (delta > (ctx->soft_limit_bytes - ctx->used_bytes)) {
      return nullptr;  // Triggers Lua emergency GC, then LUA_ERRMEM
    }
  }

  void* new_ptr = (ptr == nullptr)
                      ? multi_heap_malloc(ctx->heap, nsize)
                      : multi_heap_realloc(ctx->heap, ptr, nsize);

  if (new_ptr == nullptr) {
    if (ptr != nullptr && nsize <= old_bytes) {
      return ptr;  // Lua contract: shrinking realloc must not fail
    }
    return nullptr;
  }

  ctx->used_bytes = (ctx->used_bytes - old_bytes) + nsize;
  if (ctx->used_bytes > ctx->peak_bytes) {
    ctx->peak_bytes = ctx->used_bytes;
  }
  return new_ptr;
}

void Z2S_LuaEngine::luaWatchdogHook(lua_State* L, lua_Debug* /*ar*/) {
  void* ud = nullptr;
  lua_getallocf(L, &ud);
  auto* ctx = static_cast<SandboxCtx*>(ud);
  if (!ctx) return;

  if (ctx->watchdog_tripped) {
    luaL_error(L, "script aborted (watchdog)");
  }

  ctx->instr_executed += HOOK_STEP_INSTR;

  const bool over_instr = (ctx->instr_executed >= ctx->instr_limit);
  const bool over_time  = (esp_timer_get_time() >= ctx->deadline_us);

  if (ctx->force_abort || over_instr || over_time) {
    ctx->watchdog_tripped = true;
    // Sticky 1-instruction + return hook defeats pcall() catch loops
    lua_sethook(L, luaWatchdogHook, LUA_MASKCOUNT | LUA_MASKRET, 1);

    if (ctx->force_abort) {
      luaL_error(L, "script aborted (reload/stop)");
    } else if (over_time) {
      luaL_error(L, "script timeout (wall-clock limit)");
    } else {
      luaL_error(L, "script timeout (instruction budget)");
    }
  }
}

void Z2S_LuaEngine::armWatchdog(
    lua_State* L, SandboxCtx& ctx, uint32_t max_instr, int64_t max_us) {
  ctx.instr_executed   = 0;
  ctx.instr_limit      = max_instr;
  ctx.deadline_us      = esp_timer_get_time() + max_us;
  ctx.watchdog_tripped = false;
  lua_sethook(L, luaWatchdogHook, LUA_MASKCOUNT, HOOK_STEP_INSTR);
}

void Z2S_LuaEngine::openSafeStdlib(lua_State* L) {
  static constexpr luaL_Reg safe_libs[] = {
    {LUA_GNAME,       luaopen_base},
    {LUA_TABLIBNAME,  luaopen_table},
    {LUA_STRLIBNAME,  luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {nullptr,         nullptr}
  };

  for (const luaL_Reg* lib = safe_libs; lib->func != nullptr; ++lib) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);
  }

  static constexpr const char* blocked_globals[] = {
    "dofile", "loadfile", "load", "collectgarbage"
  };
  for (const char* name : blocked_globals) {
    lua_pushnil(L);
    lua_setglobal(L, name);
  }
}

/*****************************************************************************/
// Public Startup & Cross-Task Control API
/*****************************************************************************/

bool Z2S_LuaEngine::begin() {
  if (_started) return true;

  evaluateRtcGuardOnBoot();
  bool autostart = readAutostartPref();
  taskENTER_CRITICAL(&_stats_mux);
  _autostart_enabled = autostart;
  taskEXIT_CRITICAL(&_stats_mux);

  _ctx.slab_ptr = heap_caps_aligned_alloc(
      8, ARENA_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  _script_buf = static_cast<char*>(heap_caps_malloc(
      MAX_SCRIPT_BYTES + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));

  if (!_ctx.slab_ptr || !_script_buf) {
    log_e("[LUA] PSRAM allocation failed (arena=%p, buf=%p)",
          _ctx.slab_ptr, _script_buf);
    setErrorStatus(Z2S_LuaState::ERROR, "PSRAM arena allocation failed");
    return false;
  }

  _ctx.slab_bytes       = ARENA_BYTES;
  _ctx.soft_limit_bytes = SOFT_LIMIT_BYTES;

  _msg_queue = xQueueCreateStatic(
      MSG_QUEUE_DEPTH, sizeof(WorkerMsg),
      _msg_queue_storage, &_msg_queue_struct);
  if (!_msg_queue) {
    setErrorStatus(Z2S_LuaState::ERROR, "message queue init failed");
    return false;
  }

  _syntax_mtx      = xSemaphoreCreateMutexStatic(&_syntax_mtx_buf);
  _syntax_done_sem = xSemaphoreCreateBinaryStatic(&_syntax_done_buf);
  if (!_syntax_mtx || !_syntax_done_sem) {
    setErrorStatus(Z2S_LuaState::ERROR, "syntax semaphore init failed");
    return false;
  }

  _worker_stopped = false;
  _task = xTaskCreateStaticPinnedToCore(
      workerEntry, "z2s_lua", TASK_STACK_DEPTH, this,
      1,  // Low priority: yields to Wi-Fi, Zigbee, and Supla loop
      _task_stack, &_task_tcb, 1);
  if (!_task) {
    setErrorStatus(Z2S_LuaState::ERROR, "worker task init failed");
    return false;
  }

  _started = true;

  if (_safe_mode) {
    setErrorStatus(Z2S_LuaState::SAFE_MODE,
                   "Safe Mode: script disabled after repeated crash resets");
    log_w("[LUA] Safe Mode active (crashes=%u) - skipping boot auto-load",
          _crash_count);
    return true;
  }

  requestReloadFromFS(/*clear_safe_mode=*/false, /*is_boot=*/true);
  return true;
}

bool Z2S_LuaEngine::enqueueEvent(const Z2S_LuaEvent& ev) {
  if (!_started || !_msg_queue || _worker_stopped) {
    return false;
  }

  WorkerMsg msg{};
  msg.type = MsgType::EVENT;
  msg.ev   = ev;

  if (xQueueSend(_msg_queue, &msg, 0) != pdTRUE) {
    taskENTER_CRITICAL(&_stats_mux);
    _events_dropped++;
    taskEXIT_CRITICAL(&_stats_mux);
    return false;
  }
  return true;
}

bool Z2S_LuaEngine::validateSyntax(
    const char* src, size_t len,
    char* err_out, size_t err_cap, bool log_to_console) {
  if (err_out && err_cap > 0) {
    err_out[0] = '\0';
  }
  if (!src || len == 0) {
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Empty script");
    return false;
  }
  if (len > MAX_SCRIPT_BYTES) {
    if (err_out && err_cap > 0) {
      snprintf(err_out, err_cap, "Script too large (%u / %u B)",
               static_cast<unsigned>(len),
               static_cast<unsigned>(MAX_SCRIPT_BYTES));
    }
    return false;
  }
  if (!_started || !_msg_queue || _worker_stopped ||
      !_syntax_mtx || !_syntax_done_sem) {
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Lua engine not ready");
    return false;
  }

  // Direct execution if already running on the z2s_lua worker task
  if (xTaskGetCurrentTaskHandle() == _task) {
    return runValidateSyntax(src, len, err_out, err_cap, log_to_console);
  }

  // Offload parsing to z2s_lua (8 KB stack) so async_tcp (4 KB stack) never overflows
  if (xSemaphoreTake(_syntax_mtx, pdMS_TO_TICKS(1500)) != pdTRUE) {
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Syntax check busy");
    return false;
  }

  taskENTER_CRITICAL(&_stats_mux);
  bool already_busy = _syntax_busy;
  taskEXIT_CRITICAL(&_stats_mux);
  if (already_busy) {
    xSemaphoreGive(_syntax_mtx);
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Syntax check busy");
    return false;
  }

  char* src_copy = static_cast<char*>(
      heap_caps_malloc(len + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  if (!src_copy) {
    xSemaphoreGive(_syntax_mtx);
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Syntax check OOM (PSRAM)");
    return false;
  }
  memcpy(src_copy, src, len);
  src_copy[len] = '\0';

  // Drain any stale completion signal before queueing
  xSemaphoreTake(_syntax_done_sem, 0);

  taskENTER_CRITICAL(&_stats_mux);
  _syntax_req.src_copy       = src_copy;
  _syntax_req.len            = len;
  _syntax_req.log_to_console = log_to_console;
  _syntax_ok                 = false;
  _syntax_err[0]             = '\0';
  _syntax_busy               = true;
  taskEXIT_CRITICAL(&_stats_mux);

  WorkerMsg msg{};
  msg.type = MsgType::VALIDATE_SYNTAX;
  if (xQueueSendToFront(_msg_queue, &msg, pdMS_TO_TICKS(100)) != pdTRUE) {
    taskENTER_CRITICAL(&_stats_mux);
    _syntax_req.src_copy = nullptr;
    _syntax_busy         = false;
    taskEXIT_CRITICAL(&_stats_mux);
    heap_caps_free(src_copy);
    xSemaphoreGive(_syntax_mtx);
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Lua queue busy");
    return false;
  }

  if (xSemaphoreTake(_syntax_done_sem, pdMS_TO_TICKS(2000)) != pdTRUE) {
    // Do NOT free src_copy here — worker owns it until _syntax_busy clears
    xSemaphoreGive(_syntax_mtx);
    if (err_out && err_cap > 0) snprintf(err_out, err_cap, "Syntax check timeout");
    return false;
  }

  bool ok = false;
  taskENTER_CRITICAL(&_stats_mux);
  ok = _syntax_ok;
  if (!ok && err_out && err_cap > 0) {
    snprintf(err_out, err_cap, "%s", _syntax_err);
  }
  taskEXIT_CRITICAL(&_stats_mux);

  xSemaphoreGive(_syntax_mtx);
  return ok;
}

bool Z2S_LuaEngine::runValidateSyntax(
    const char* src, size_t len,
    char* err_out, size_t err_cap, bool log_to_console) {
  if (err_out && err_cap > 0) {
    err_out[0] = '\0';
  }

  void* arena = heap_caps_aligned_alloc(
      8, SYNTAX_CHECK_ARENA_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!arena) {
    if (err_out && err_cap > 0) {
      snprintf(err_out, err_cap, "Syntax check OOM (PSRAM)");
    }
    return false;
  }

  multi_heap_handle_t heap = multi_heap_register(arena, SYNTAX_CHECK_ARENA_BYTES);
  if (!heap) {
    heap_caps_free(arena);
    if (err_out && err_cap > 0) {
      snprintf(err_out, err_cap, "Syntax check heap init failed");
    }
    return false;
  }

  lua_State* checkL = lua_newstate(luaSyntaxPsramAlloc, heap, luaL_makeseed(nullptr));
  if (!checkL) {
    heap_caps_free(arena);
    if (err_out && err_cap > 0) {
      snprintf(err_out, err_cap, "Syntax check VM init failed");
    }
    return false;
  }

  int status = luaL_loadbufferx(checkL, src, len, "@z2s_main", "t");
  bool ok    = (status == LUA_OK);

  if (!ok) {
    const char* raw_err = lua_tostring(checkL, -1);
    char local_err[192];
    formatSyntaxError(raw_err, local_err, sizeof(local_err));

    if (err_out && err_cap > 0) {
      snprintf(err_out, err_cap, "%s", local_err);
    }
    if (log_to_console) {
      char console_msg[224];
      int n = snprintf(console_msg, sizeof(console_msg),
                       "[LUA] Save rejected: %s\n", local_err);
      if (n > 0) {
        size_t clen = (static_cast<size_t>(n) < sizeof(console_msg))
                          ? static_cast<size_t>(n)
                          : (sizeof(console_msg) - 1);
        appendConsole(console_msg, clen);
      }
    }
    log_w("[LUA] Pre-save syntax check failed: %s", local_err);
  }

  lua_close(checkL);
  heap_caps_free(arena);
  return ok;
}

void Z2S_LuaEngine::handleValidateSyntax() {
  taskENTER_CRITICAL(&_stats_mux);
  char*  src_copy = _syntax_req.src_copy;
  size_t len      = _syntax_req.len;
  bool   log_cons = _syntax_req.log_to_console;
  _syntax_req.src_copy = nullptr;
  taskEXIT_CRITICAL(&_stats_mux);

  if (!src_copy) {
    taskENTER_CRITICAL(&_stats_mux);
    _syntax_busy = false;
    taskEXIT_CRITICAL(&_stats_mux);
    if (_syntax_done_sem) xSemaphoreGive(_syntax_done_sem);
    return;
  }

  char err_buf[sizeof(_syntax_err)];
  bool ok = runValidateSyntax(src_copy, len, err_buf, sizeof(err_buf), log_cons);
  heap_caps_free(src_copy);

  taskENTER_CRITICAL(&_stats_mux);
  _syntax_ok = ok;
  if (ok) {
    _syntax_err[0] = '\0';
  } else {
    snprintf(_syntax_err, sizeof(_syntax_err), "%s", err_buf);
  }
  _syntax_busy = false;
  taskEXIT_CRITICAL(&_stats_mux);

  if (_syntax_done_sem) {
    xSemaphoreGive(_syntax_done_sem);
  }
}

bool Z2S_LuaEngine::saveAndReload(const char* src, size_t len) {
  if (!_started || !_msg_queue) return false;
  if (!src && len > 0) return false;
  if (len > MAX_SCRIPT_BYTES) {
    setErrorStatus(Z2S_LuaState::ERROR, "script exceeds 16 KB limit");
    return false;
  }

  if (len == 0) {
    return deleteScriptAndStop();
  }

  if (!Z2S_saveFileWithCRCAndBackup(
          SCRIPT_FILE, reinterpret_cast<const uint8_t*>(src),
          len, SCRIPT_BAK_FILE)) {
    setErrorStatus(Z2S_LuaState::ERROR, "LittleFS script save failed");
    return false;
  }

  return requestReloadFromFS(/*clear_safe_mode=*/true, /*is_boot=*/false);
}

bool Z2S_LuaEngine::requestReloadFromFS(bool clear_safe_mode, bool is_boot) {
  if (!_started || !_msg_queue || _worker_stopped) return false;

  if (clear_safe_mode) {
    s_lua_rtc_guard.crash_count       = 0;
    s_lua_rtc_guard.safe_mode_latched = 0;
    taskENTER_CRITICAL(&_stats_mux);
    _crash_count = 0;
    _safe_mode   = false;
    taskEXIT_CRITICAL(&_stats_mux);
  }

  _ctx.force_abort = true;

  WorkerMsg msg{};
  msg.type  = MsgType::RELOAD_FS;
  msg.flags = is_boot ? 1 : 0;
  return xQueueSend(_msg_queue, &msg, pdMS_TO_TICKS(100)) == pdTRUE;
}

bool Z2S_LuaEngine::stopVM() {
  if (!_started || !_msg_queue || _worker_stopped) return false;

  _ctx.force_abort = true;

  WorkerMsg msg{};
  msg.type = MsgType::STOP_VM;
  return xQueueSendToFront(_msg_queue, &msg, pdMS_TO_TICKS(100)) == pdTRUE;
}

bool Z2S_LuaEngine::setAutostart(bool enabled) {
  if (!_started || !_msg_queue || _worker_stopped) return false;

  taskENTER_CRITICAL(&_stats_mux);
  _autostart_enabled = enabled;
  taskEXIT_CRITICAL(&_stats_mux);

  WorkerMsg msg{};
  msg.type  = MsgType::SET_AUTOSTART;
  msg.flags = enabled ? 1 : 0;
  return xQueueSend(_msg_queue, &msg, pdMS_TO_TICKS(100)) == pdTRUE;
}

bool Z2S_LuaEngine::deleteScriptAndStop() {
  if (!_started || !_msg_queue) return false;

  Z2S_deleteFile(SCRIPT_FILE);
  Z2S_deleteFile(SCRIPT_BAK_FILE);

  s_lua_rtc_guard.crash_count       = 0;
  s_lua_rtc_guard.safe_mode_latched = 0;
  taskENTER_CRITICAL(&_stats_mux);
  _crash_count = 0;
  _safe_mode   = false;
  taskEXIT_CRITICAL(&_stats_mux);

  _ctx.force_abort = true;

  WorkerMsg msg{};
  msg.type = MsgType::CLEAR_SCRIPT;
  return xQueueSend(_msg_queue, &msg, pdMS_TO_TICKS(100)) == pdTRUE;
}

bool Z2S_LuaEngine::clearSafeMode(bool reload_now) {
  s_lua_rtc_guard.crash_count       = 0;
  s_lua_rtc_guard.safe_mode_latched = 0;

  taskENTER_CRITICAL(&_stats_mux);
  _crash_count = 0;
  _safe_mode   = false;
  if (_state == Z2S_LuaState::SAFE_MODE) {
    _state         = Z2S_LuaState::NO_SCRIPT;
    _last_error[0] = '\0';
  }
  taskEXIT_CRITICAL(&_stats_mux);

  return reload_now ? requestReloadFromFS(false, false) : true;
}

bool Z2S_LuaEngine::readStoredScript(
    char* dst, size_t dst_cap, size_t& out_len) {
  out_len = 0;
  if (!dst || dst_cap == 0) return false;
  dst[0] = '\0';

  Z2S_FsLoadResult primary_st = Z2S_FsLoadResult::IO_ERROR;
  size_t sz = Z2S_getFileSizeWithCRC(SCRIPT_FILE, &primary_st);

  if (primary_st == Z2S_FsLoadResult::OK && sz > 0 && sz < dst_cap &&
      sz <= MAX_SCRIPT_BYTES) {
    primary_st = Z2S_loadFileWithCRCEx(
        SCRIPT_FILE, reinterpret_cast<uint8_t*>(dst), sz);
    if (primary_st == Z2S_FsLoadResult::OK) {
      dst[sz] = '\0';
      out_len = sz;
      return true;
    }
  }

  if (primary_st != Z2S_FsLoadResult::MUTEX_TIMEOUT) {
    Z2S_FsLoadResult bak_st = Z2S_FsLoadResult::NOT_FOUND;
    size_t bak_sz = Z2S_getFileSizeWithCRC(SCRIPT_BAK_FILE, &bak_st);
    if (bak_st == Z2S_FsLoadResult::OK && bak_sz > 0 && bak_sz < dst_cap &&
        bak_sz <= MAX_SCRIPT_BYTES) {
      bak_st = Z2S_loadFileWithCRCEx(
          SCRIPT_BAK_FILE, reinterpret_cast<uint8_t*>(dst), bak_sz);
      if (bak_st == Z2S_FsLoadResult::OK) {
        dst[bak_sz] = '\0';
        out_len = bak_sz;
        log_w("[LUA] Recovered script from backup (%u bytes)",
              static_cast<unsigned>(bak_sz));
        Z2S_saveFileWithCRC(
            SCRIPT_FILE, reinterpret_cast<const uint8_t*>(dst), bak_sz);
        return true;
      }
    }
  }

  return false;
}

void Z2S_LuaEngine::requestStop(uint32_t wait_ms) {
  if (!_started || _worker_stopped) return;

  _ctx.force_abort = true;

  if (_msg_queue) {
    WorkerMsg msg{};
    msg.type = MsgType::STOP;
    xQueueSendToFront(_msg_queue, &msg, pdMS_TO_TICKS(50));
  }

  uint32_t start = millis();
  while (!_worker_stopped && (millis() - start) < wait_ms) {
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

/*****************************************************************************/
// Telemetry & Console Log (zero heap, spinlock-protected)
/*****************************************************************************/

void Z2S_LuaEngine::publishHeapSnapshot() {
  size_t largest = 0;
  if (_ctx.heap) {
    multi_heap_info_t info = {};
    multi_heap_get_info(_ctx.heap, &info);
    largest = info.largest_free_block;
  }

  taskENTER_CRITICAL(&_stats_mux);
  _used_bytes_pub   = _ctx.used_bytes;
  _peak_bytes_pub   = _ctx.peak_bytes;
  _largest_free_pub = largest;
  taskEXIT_CRITICAL(&_stats_mux);
}

void Z2S_LuaEngine::getStatus(Z2S_LuaStatus& out) {
  taskENTER_CRITICAL(&_stats_mux);
  out.state              = _state;
  out.safe_mode          = _safe_mode;
  out.autostart_enabled  = _autostart_enabled;
  out.crash_count        = _crash_count;
  out.arena_bytes        = _ctx.slab_bytes;
  out.used_bytes         = _used_bytes_pub;
  out.peak_bytes         = _peak_bytes_pub;
  out.largest_free_block = _largest_free_pub;
  out.script_size        = _script_size;
  out.events_processed   = _events_processed;
  out.events_dropped     = _events_dropped;
  out.watchdog_trips     = _watchdog_trips;
  memcpy(out.last_error, _last_error, sizeof(out.last_error));
  taskEXIT_CRITICAL(&_stats_mux);
}

void Z2S_LuaEngine::appendConsole(const char* s, size_t len) {
  if (!s || len == 0) return;

  log_i("[LUA] %.*s", static_cast<int>(len), s);

  taskENTER_CRITICAL(&_stats_mux);
  for (size_t i = 0; i < len; ++i) {
    _console_buf[_console_head] = s[i];
    _console_head = (_console_head + 1) % CONSOLE_BUF_SIZE;
    if (_console_len < CONSOLE_BUF_SIZE) {
      _console_len++;
    } else {
      _console_wrapped = true;
    }
  }
  taskEXIT_CRITICAL(&_stats_mux);
}

size_t Z2S_LuaEngine::copyConsoleLog(
    char* dst, size_t dst_cap, bool* out_wrapped) {
  if (!dst || dst_cap == 0) {
    if (out_wrapped) *out_wrapped = false;
    return 0;
  }

  taskENTER_CRITICAL(&_stats_mux);
  size_t to_copy = (_console_len < (dst_cap - 1)) ? _console_len : (dst_cap - 1);
  size_t start   = (_console_head + CONSOLE_BUF_SIZE - to_copy) % CONSOLE_BUF_SIZE;

  for (size_t i = 0; i < to_copy; ++i) {
    dst[i] = _console_buf[(start + i) % CONSOLE_BUF_SIZE];
  }
  dst[to_copy] = '\0';
  if (out_wrapped) {
    *out_wrapped = _console_wrapped;
  }
  taskEXIT_CRITICAL(&_stats_mux);
  return to_copy;
}

void Z2S_LuaEngine::clearConsoleLog() {
  taskENTER_CRITICAL(&_stats_mux);
  _console_head    = 0;
  _console_len     = 0;
  _console_wrapped = false;
  taskEXIT_CRITICAL(&_stats_mux);
}

/*****************************************************************************/
// Worker Task (Single Owner of lua_State* and _ctx.heap)
/*****************************************************************************/

void Z2S_LuaEngine::workerEntry(void* arg) {
  static_cast<Z2S_LuaEngine*>(arg)->workerLoop();
  vTaskDelete(nullptr);
}

void Z2S_LuaEngine::workerLoop() {
  WorkerMsg msg{};

  for (;;) {
    if (xQueueReceive(_msg_queue, &msg, pdMS_TO_TICKS(1000)) != pdTRUE) {
      maybeHealCrashCounter();
      publishHeapSnapshot();
      continue;
    }

    switch (msg.type) {
      case MsgType::EVENT:
        handleEvent(msg.ev);
        maybeHealCrashCounter();
        publishHeapSnapshot();
        break;

      case MsgType::RELOAD_FS:
        handleReloadFromFS((msg.flags & 0x01) != 0);
        publishHeapSnapshot();
        break;

      case MsgType::STOP_VM:
        handleStopVM();
        publishHeapSnapshot();
        break;

      case MsgType::SET_AUTOSTART:
        handleSetAutostart((msg.flags & 0x01) != 0);
        break;

      case MsgType::VALIDATE_SYNTAX:
        handleValidateSyntax();
        break;

      case MsgType::CLEAR_SCRIPT:
        handleClearScript();
        publishHeapSnapshot();
        break;

      case MsgType::STOP: {
        // Unblock any pending HTTP syntax-check waiter before shutting down
        char* orphan_src = nullptr;
        bool  had_syntax = false;
        taskENTER_CRITICAL(&_stats_mux);
        if (_syntax_busy) {
          orphan_src           = _syntax_req.src_copy;
          _syntax_req.src_copy = nullptr;
          _syntax_ok           = false;
          snprintf(_syntax_err, sizeof(_syntax_err), "Engine stopping");
          _syntax_busy         = false;
          had_syntax           = true;
        }
        taskEXIT_CRITICAL(&_stats_mux);
        if (orphan_src) heap_caps_free(orphan_src);
        if (had_syntax && _syntax_done_sem) xSemaphoreGive(_syntax_done_sem);

        Z2S_LuaBridge::instance().enqueueUnwatchAll();
        closeVM();
        publishHeapSnapshot();
        publishState(Z2S_LuaState::STOPPED);
        _worker_stopped = true;
        return;
      }
    } //msg.type
  }
}

void Z2S_LuaEngine::closeVM() {
  if (_lua) {
    lua_close(_lua);
    _lua = nullptr;
  }
  _ctx.heap       = nullptr;
  _ctx.used_bytes = 0;
}

bool Z2S_LuaEngine::resetVM() {
  closeVM();
  Z2S_Solar::clearLuaOverride();

  _ctx.used_bytes = 0;
  _ctx.peak_bytes = 0;
  _ctx.heap = multi_heap_register(_ctx.slab_ptr, _ctx.slab_bytes);
  if (!_ctx.heap) {
    setErrorStatus(Z2S_LuaState::ERROR, "multi_heap_register failed");
    return false;
  }

  _lua = lua_newstate(luaArenaAlloc, &_ctx, luaL_makeseed(nullptr));
  if (!_lua) {
    setErrorStatus(Z2S_LuaState::ERROR, "lua_newstate OOM");
    return false;
  }
  lua_gc(_lua, LUA_GCGEN);

  openSafeStdlib(_lua);
  Z2S_LuaBridge::registerBindings(_lua);

  // Load built-in Flash prelude (z2s.on / z2s.window / z2s.at)
  int st = luaL_loadbufferx(
      _lua, Z2S_LUA_PRELUDE, sizeof(Z2S_LUA_PRELUDE) - 1, "@z2s_prelude", "t");
  if (st == LUA_OK) {
    st = lua_pcall(_lua, 0, 0, 0);
  }
  if (st != LUA_OK) {
    captureLuaError("prelude");
    closeVM();
    return false;
  }

  return true;
}

void Z2S_LuaEngine::flushStaleEvents() {
  WorkerMsg tmp{};
  size_t count = uxQueueMessagesWaiting(_msg_queue);
  for (size_t i = 0; i < count; ++i) {
    if (xQueueReceive(_msg_queue, &tmp, 0) != pdTRUE) break;
    if (tmp.type != MsgType::EVENT) {
      xQueueSendToBack(_msg_queue, &tmp, 0);
    }
  }
}

void Z2S_LuaEngine::handleStopVM() {
  Z2S_LuaBridge::instance().enqueueUnwatchAll();
  flushStaleEvents();
  _ctx.force_abort = false;
  closeVM();

  taskENTER_CRITICAL(&_stats_mux);
  _script_loaded_ms = 0;
  _last_error[0]    = '\0';
  if (_state != Z2S_LuaState::NO_SCRIPT && _state != Z2S_LuaState::SAFE_MODE) {
    _state = Z2S_LuaState::STOPPED;
  }
  taskEXIT_CRITICAL(&_stats_mux);

  static constexpr char kMsg[] =
      "[LUA] VM stopped (script kept on flash)\n";
  appendConsole(kMsg, sizeof(kMsg) - 1);
}

void Z2S_LuaEngine::handleSetAutostart(bool enabled) {
  writeAutostartPref(enabled);
  log_i("[LUA] Boot autostart %s (saved to NVS)",
        enabled ? "ENABLED" : "DISABLED");
}

void Z2S_LuaEngine::handleClearScript() {
  Z2S_LuaBridge::instance().enqueueUnwatchAll();
  flushStaleEvents();
  _ctx.force_abort = false;
  closeVM();

  taskENTER_CRITICAL(&_stats_mux);
  _script_size      = 0;
  _script_loaded_ms = 0;
  _last_error[0]    = '\0';
  _state            = Z2S_LuaState::NO_SCRIPT;
  taskEXIT_CRITICAL(&_stats_mux);

  log_i("[LUA] Script cleared and VM stopped");
}

void Z2S_LuaEngine::handleReloadFromFS(bool is_boot) {
  // 1. Disable old watches FIRST (FIFO precedes any z2s.watch() in new script)
  Z2S_LuaBridge::instance().enqueueUnwatchAll();
  flushStaleEvents();
  _ctx.force_abort = false;

  // 2. Check if any script exists before spinning up VM
  int8_t pri_exists = Z2S_fileExists(SCRIPT_FILE);
  int8_t bak_exists = Z2S_fileExists(SCRIPT_BAK_FILE);
  if (pri_exists == 0 && bak_exists == 0) {
    closeVM();
    taskENTER_CRITICAL(&_stats_mux);
    _script_size      = 0;
    _script_loaded_ms = 0;
    _last_error[0]    = '\0';
    _state            = Z2S_LuaState::NO_SCRIPT;
    taskEXIT_CRITICAL(&_stats_mux);
    log_i("[LUA] No saved script found on LittleFS");
    return;
  }

  // 3. Load script into PSRAM staging buffer
  size_t script_len = 0;
  if (!readStoredScript(_script_buf, MAX_SCRIPT_BYTES + 1, script_len) ||
      script_len == 0) {
    closeVM();
    taskENTER_CRITICAL(&_stats_mux);
    _script_size      = 0;
    _script_loaded_ms = 0;
    taskEXIT_CRITICAL(&_stats_mux);
    setErrorStatus(Z2S_LuaState::ERROR, "failed to load script from LittleFS");
    return;
  }

  // Publish script size immediately so compile/watchdog errors show accurate byte count
  taskENTER_CRITICAL(&_stats_mux);
  _script_size = script_len;
  taskEXIT_CRITICAL(&_stats_mux);

  // 4. Boot-time autostart guard: keep script on flash, leave VM stopped
  if (is_boot && !readAutostartPref()) {
    closeVM();
    taskENTER_CRITICAL(&_stats_mux);
    _autostart_enabled = false;
    _script_loaded_ms  = 0;
    _last_error[0]     = '\0';
    _state             = Z2S_LuaState::STOPPED;
    taskEXIT_CRITICAL(&_stats_mux);
    static constexpr char kMsg[] =
        "[LUA] Autostart OFF - VM kept stopped (click Reload from Flash to start)\n";
    appendConsole(kMsg, sizeof(kMsg) - 1);
    return;
  }

  // 5. Rebuild pristine VM and execute chunk in text-only mode ("t")
  if (!resetVM()) {
    return;
  }

  if (!loadScriptChunk(_script_buf, script_len, "@z2s_main")) {
    Z2S_LuaBridge::instance().enqueueUnwatchAll();
    closeVM();
    return;
  }

  taskENTER_CRITICAL(&_stats_mux);
  _script_size      = script_len;
  _script_loaded_ms = millis() ? millis() : 1;
  _last_error[0]    = '\0';
  _state            = Z2S_LuaState::RUNNING;
  taskEXIT_CRITICAL(&_stats_mux);

  log_i("[LUA] Script loaded OK (%u bytes, VM heap=%u B)",
        static_cast<unsigned>(script_len),
        static_cast<unsigned>(_ctx.used_bytes));
}

bool Z2S_LuaEngine::loadScriptChunk(
    const char* src, size_t len, const char* name) {
  if (!_lua || !src || len == 0) return false;

  int status = luaL_loadbufferx(_lua, src, len, name, "t");
  if (status != LUA_OK) {
    captureLuaError("compile");
    return false;
  }

  lua_pushcfunction(_lua, syncWindowsTrampoline);
  armWatchdog(_lua, _ctx, EVENT_MAX_INSTR, EVENT_MAX_WALL_US);
  markExecEnter();
  status = lua_pcall(_lua, 0, 0, 0);
  markExecExit();
  lua_sethook(_lua, nullptr, 0, 0);

  if (status != LUA_OK) {
    if (_ctx.watchdog_tripped && !_ctx.force_abort) {
      taskENTER_CRITICAL(&_stats_mux);
      _watchdog_trips++;
      taskEXIT_CRITICAL(&_stats_mux);
    }
    captureLuaError("init");
    return false;
  }
  return true;
}

void Z2S_LuaEngine::handleEvent(const Z2S_LuaEvent& ev) {
  if (!_lua || _state != Z2S_LuaState::RUNNING || _ctx.force_abort) {
    return;
  }

  DispatchArgs args{&ev, false};

  lua_pushcfunction(_lua, dispatchEventTrampoline);
  lua_pushlightuserdata(_lua, &args);

  armWatchdog(_lua, _ctx, EVENT_MAX_INSTR, EVENT_MAX_WALL_US);
  markExecEnter();
  int status = lua_pcall(_lua, 1, 0, 0);
  markExecExit();
  lua_sethook(_lua, nullptr, 0, 0);

  if (status != LUA_OK) {
    if (_ctx.watchdog_tripped && !_ctx.force_abort) {
      taskENTER_CRITICAL(&_stats_mux);
      _watchdog_trips++;
      taskEXIT_CRITICAL(&_stats_mux);
    }
    captureLuaError("runtime");

    if (status == LUA_ERRMEM) {
      lua_gc(_lua, LUA_GCCOLLECT, 0);
    }
    return;
  }

  if (args.invoked) {
    taskENTER_CRITICAL(&_stats_mux);
    _events_processed++;
    taskEXIT_CRITICAL(&_stats_mux);
  }
}

void Z2S_LuaEngine::captureLuaError(const char* stage) {
  const char* msg = lua_tostring(_lua, -1);
  char buf[128];
  snprintf(buf, sizeof(buf), "[%s] %s", stage, msg ? msg : "unknown");
  lua_pop(_lua, 1);

  log_e("[LUA] %s", buf);

  taskENTER_CRITICAL(&_stats_mux);
  memcpy(_last_error, buf, sizeof(_last_error));
  if (strcmp(stage, "compile") == 0 || strcmp(stage, "init") == 0) {
    _state = Z2S_LuaState::ERROR;
  }
  taskEXIT_CRITICAL(&_stats_mux);
}

void Z2S_LuaEngine::setErrorStatus(Z2S_LuaState st, const char* msg) {
  taskENTER_CRITICAL(&_stats_mux);
  _state = st;
  snprintf(_last_error, sizeof(_last_error), "%s", msg ? msg : "");
  taskEXIT_CRITICAL(&_stats_mux);
}

void Z2S_LuaEngine::publishState(Z2S_LuaState st) {
  taskENTER_CRITICAL(&_stats_mux);
  _state = st;
  taskEXIT_CRITICAL(&_stats_mux);
}

#else
typedef int z2s_lua_engine_disabled_stub_t;
#endif  // Z2S_ENABLE_LUA