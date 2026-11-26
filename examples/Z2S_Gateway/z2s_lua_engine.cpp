#include "z2s_lua_engine.h"

#if Z2S_ENABLE_LUA

#include <Arduino.h>
#include <cstdio>
#include <cstring>
#include <esp_heap_caps.h>
#include <esp_system.h>
#include <esp_timer.h>

#include "z2s_little_fs.h"

/*****************************************************************************/
// RTC_NOINIT_ATTR Boot-Loop Guard & Protected Dispatch Trampoline
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

// Runs inside lua_pcall() so lua_getglobal() and lua_pushstring() can never
// trigger an unprotected LUA_ERRMEM panic if the PSRAM arena is full.
static int dispatchEventTrampoline(lua_State* L) {
  auto* args = static_cast<DispatchArgs*>(lua_touserdata(L, 1));
  if (!args || !args->ev) return 0;

  lua_getglobal(L, "on_event");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 1);
    return 0;
  }

  args->invoked = true;
  lua_pushinteger(L, args->ev->channel);
  if (const char* evt_name = Z2S_LuaBridge::eventAlias(args->ev->event_id)) {
    lua_pushstring(L, evt_name);
  } else {
    lua_pushinteger(L, args->ev->event_id);
  }

  lua_call(L, 2, 0);
  return 0;
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
// RTC Guard Lifecycle
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
      return nullptr;  // Triggers Lua 5.4 emergency GC, then LUA_ERRMEM
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

  requestReloadFromFS(false);
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

  return requestReloadFromFS(true);
}

bool Z2S_LuaEngine::requestReloadFromFS(bool clear_safe_mode) {
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
  msg.type = MsgType::RELOAD_FS;
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

  return reload_now ? requestReloadFromFS(false) : true;
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
    }
  }
  taskEXIT_CRITICAL(&_stats_mux);
}

size_t Z2S_LuaEngine::copyConsoleLog(char* dst, size_t dst_cap) {
  if (!dst || dst_cap == 0) return 0;

  taskENTER_CRITICAL(&_stats_mux);
  size_t to_copy = (_console_len < (dst_cap - 1)) ? _console_len : (dst_cap - 1);
  size_t start   = (_console_head + CONSOLE_BUF_SIZE - to_copy) % CONSOLE_BUF_SIZE;

  for (size_t i = 0; i < to_copy; ++i) {
    dst[i] = _console_buf[(start + i) % CONSOLE_BUF_SIZE];
  }
  dst[to_copy] = '\0';
  taskEXIT_CRITICAL(&_stats_mux);
  return to_copy;
}

void Z2S_LuaEngine::clearConsoleLog() {
  taskENTER_CRITICAL(&_stats_mux);
  _console_head = 0;
  _console_len  = 0;
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
        handleReloadFromFS();
        publishHeapSnapshot();
        break;

      case MsgType::CLEAR_SCRIPT:
        handleClearScript();
        publishHeapSnapshot();
        break;

      case MsgType::STOP:
        Z2S_LuaBridge::instance().enqueueUnwatchAll();
        closeVM();
        publishHeapSnapshot();
        publishState(Z2S_LuaState::STOPPED);
        _worker_stopped = true;
        return;
    }
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

void Z2S_LuaEngine::handleReloadFromFS() {
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
    setErrorStatus(Z2S_LuaState::ERROR, "failed to load script from LittleFS");
    return;
  }

  // 4. Rebuild pristine VM and execute chunk in text-only mode ("t")
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

  armWatchdog(_lua, _ctx, INIT_MAX_INSTR, INIT_MAX_WALL_US);
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