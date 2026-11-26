#include "z2s_lua_web_gui.h"

#if Z2S_ENABLE_LUA

#include <Arduino.h>
#include <cstdio>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "Z2S_ESPUI.h"
#include "z2s_lua_engine.h"

namespace {

/*****************************************************************************/
// Static GUI IDs, Style Literals & State (0 Arduino String, 0 runtime DRAM)
/*****************************************************************************/

constexpr uint32_t LUA_GUI_REFRESH_MS = 1000;
constexpr uint32_t DELETE_CONFIRM_MS  = 5000;
constexpr size_t   CONSOLE_JSON_CAP   = 960;  // Safe inside Z2S_ESPUI s_batch_buf[1360]

// Split literals prevent HTML renderers from decoding entities on copy/paste;
// static_assert guarantees memcpy lengths can never go out of bounds.
static constexpr char HTML_ENT_AMP[] = "&" "amp;";
static constexpr char HTML_ENT_LT[]  = "&" "lt;";
static constexpr char HTML_ENT_GT[]  = "&" "gt;";

static_assert(sizeof(HTML_ENT_AMP) == 6, "HTML_ENT_AMP was mangled on copy/paste");
static_assert(sizeof(HTML_ENT_LT)  == 5, "HTML_ENT_LT was mangled on copy/paste");
static_assert(sizeof(HTML_ENT_GT)  == 5, "HTML_ENT_GT was mangled on copy/paste");

// Static-lifetime style literals required by Z2S_ESPUIClass::setElementStyle().
// Every variant explicitly sets both background and border-left so appending to
// wrap.style.cssText cleanly overrides previous states.
static constexpr const char* STYLE_SAFE_OK =
    "text-align:left;font-family:monospace;white-space:pre-wrap;"
    "border-left:3px solid #10b981;background:transparent;";
static constexpr const char* STYLE_SAFE_TRIP =
    "text-align:left;font-family:monospace;white-space:pre-wrap;"
    "border-left:3px solid #e74c3c;background:rgba(231,76,60,.14);";

static constexpr const char* STYLE_TEL_INFO =
    "text-align:left;font-family:monospace;font-size:12px;white-space:pre-wrap;"
    "border-left:3px solid #3b82f6;background:transparent;";
static constexpr const char* STYLE_TEL_OK =
    "text-align:left;font-family:monospace;font-size:12px;white-space:pre-wrap;"
    "border-left:3px solid #10b981;background:transparent;";
static constexpr const char* STYLE_TEL_ERR =
    "text-align:left;font-family:monospace;font-size:12px;white-space:pre-wrap;"
    "border-left:3px solid #e74c3c;background:rgba(231,76,60,.14);";

static constexpr const char* STYLE_EDITOR_WRAP =
    "background-color: transparent;";
static constexpr const char* STYLE_CONSOLE =
    "text-align:left;font-family:monospace;font-size:11.5px;white-space:pre-wrap;"
    "max-height:220px;overflow-y:auto;";

// Single-quoted HTML (0 double quotes / 0 newlines => 0 JSON escape expansion).
// Size: ~1,064 B -> ~1,210 B with /api/ui JSON envelope (< 1,360 B s_batch_buf).
static const char LUA_EDITOR_HTML[] PROGMEM =
    "<textarea id='le' rows='14' maxlength='16384' spellcheck='false' "
    "placeholder='Lua script...' "
    "onfocus='if(!this.dataset.l){this.dataset.l=1;if(!this.value)document.getElementById(`ll`).click()}' "
    "oninput='document.getElementById(`ls`).textContent=this.value.length+`/16384 B`'>"
    "</textarea>"
    "<div style='display:flex;gap:8px;align-items:center;margin-top:8px'>"
    "<button type='button' class='z2s-btn emerald' style='width:auto' "
    "onclick='let g=i=>document.getElementById(i),s=g(`ls`);s.textContent=`Saving...`;"
    "fetch(`/api/lua/script`,{method:`POST`,headers:{[`Content-Type`]:`text/x-lua`},body:g(`le`).value})"
    ".then(r=>r.text()).then(t=>s.textContent=t).catch(()=>s.textContent=`Err`)'>"
    "Save &" "amp; Reload</button>"
    "<button id='ll' type='button' class='z2s-btn' style='width:auto' "
    "onclick='let g=i=>document.getElementById(i),s=g(`ls`);s.textContent=`Loading...`;"
    "fetch(`/api/lua/script`).then(r=>r.text()).then(t=>{let e=g(`le`);"
    "e.value=t;e.dataset.l=1;s.textContent=t.length+` B`}).catch(()=>s.textContent=`Err`)'>"
    "Load</button>"
    "<span id='ls' style='font:12px monospace'>Ready</span></div>";

static_assert(sizeof(LUA_EDITOR_HTML) < 1120,
              "LUA_EDITOR_HTML must fit inside Z2S_ESPUI s_batch_buf[1360] with JSON envelope");

uint16_t s_tab_lua            = Control::noParent;
uint16_t s_lbl_safemode       = Control::noParent;
uint16_t s_btn_clear_safemode = Control::noParent;
uint16_t s_lbl_telemetry      = Control::noParent;
uint16_t s_lbl_editor         = Control::noParent;
uint16_t s_btn_reload_fs      = Control::noParent;
uint16_t s_btn_stop_vm        = Control::noParent;
uint16_t s_btn_delete_script  = Control::noParent;
uint16_t s_lbl_console        = Control::noParent;
uint16_t s_btn_clear_console  = Control::noParent;

bool     s_gui_built          = false;
bool     s_routes_registered  = false;
uint32_t s_last_refresh_ms    = 0;
uint32_t s_delete_armed_ms    = 0;

uint32_t s_last_status_hash   = 0;
uint32_t s_last_console_hash  = 0;
bool     s_last_safemode_ui   = false;

// Dedicated PSRAM buffers for /api/lua/script HTTP GET/POST (allocated once)
char*             s_http_get_buf  = nullptr;
char*             s_http_post_buf = nullptr;
size_t            s_http_get_len  = 0;
size_t            s_http_post_len = 0;
bool              s_post_overflow = false;
StaticSemaphore_t s_get_mtx_buf   = {};
StaticSemaphore_t s_post_mtx_buf  = {};
SemaphoreHandle_t s_get_mtx       = nullptr;
SemaphoreHandle_t s_post_mtx      = nullptr;

/*****************************************************************************/
// Helpers (zero heap)
/*****************************************************************************/

uint32_t fnv1a32(const char* s, size_t len) {
  uint32_t h = 2166136261UL;
  for (size_t i = 0; i < len; ++i) {
    h ^= static_cast<uint8_t>(s[i]);
    h *= 16777619UL;
  }
  return h;
}

const char* luaStateName(Z2S_LuaState st, bool autostart_enabled) {
  switch (st) {
    case Z2S_LuaState::UNINITIALIZED: return "UNINITIALIZED";
    case Z2S_LuaState::NO_SCRIPT:     return "NO SCRIPT";
    case Z2S_LuaState::RUNNING:       return "RUNNING";
    case Z2S_LuaState::ERROR:         return "ERROR";
    case Z2S_LuaState::SAFE_MODE:     return "SAFE MODE";
    case Z2S_LuaState::STOPPED:
      return autostart_enabled ? "STOPPED" : "STOPPED (autostart OFF)";
    default:                          return "UNKNOWN";
  }
}

size_t escapeHtml(const char* src, char* dst, size_t dst_cap) {
  if (!dst || dst_cap == 0) return 0;
  size_t w = 0;
  if (!src) {
    dst[0] = '\0';
    return 0;
  }
  for (size_t i = 0; src[i] != '\0'; ++i) {
    const char* rep = nullptr;
    size_t rlen = 0;
    switch (src[i]) {
      case '&': rep = HTML_ENT_AMP; rlen = sizeof(HTML_ENT_AMP) - 1; break;
      case '<': rep = HTML_ENT_LT;  rlen = sizeof(HTML_ENT_LT)  - 1; break;
      case '>': rep = HTML_ENT_GT;  rlen = sizeof(HTML_ENT_GT)  - 1; break;
      default:  break;
    }
    if (rep) {
      if (w + rlen >= dst_cap) break;
      memcpy(dst + w, rep, rlen);
      w += rlen;
    } else {
      if (w + 1 >= dst_cap) break;
      dst[w++] = src[i];
    }
  }
  dst[w] = '\0';
  return w;
}

// Formats the newest tail of the console ring buffer, HTML-escaped and guaranteed
// to fit inside max_json_bytes after Z2S_ESPUI's appendEscapedJson() pass.
// Drops any clipped leading line caused by truncation or ring-buffer wrap.
void formatConsoleTailForBatch(
    const char* raw, size_t raw_len,
    char* dst, size_t dst_cap, size_t max_json_bytes,
    bool ring_wrapped = false) {
  if (!dst || dst_cap == 0) return;
  if (!raw || raw_len == 0) {
    snprintf(dst, dst_cap, "(empty)");
    return;
  }

  constexpr size_t kAmpLen = sizeof(HTML_ENT_AMP) - 1;
  constexpr size_t kLtLen  = sizeof(HTML_ENT_LT)  - 1;
  constexpr size_t kGtLen  = sizeof(HTML_ENT_GT)  - 1;

  size_t json_cost = 0;
  size_t html_cost = 0;
  size_t start     = raw_len;

  while (start > 0) {
    char c = raw[start - 1];
    size_t h_add = 1;
    size_t j_add = 1;

    if (c == '&')      { h_add = kAmpLen; j_add = kAmpLen; }
    else if (c == '<') { h_add = kLtLen;  j_add = kLtLen;  }
    else if (c == '>') { h_add = kGtLen;  j_add = kGtLen;  }
    else if (c == '"' || c == '\\' || c == '\n' || c == '\r' || c == '\t') {
      h_add = 1; j_add = 2;
    } else if (static_cast<uint8_t>(c) < 0x20) {
      h_add = 0; j_add = 0;
    }

    if ((json_cost + j_add) > max_json_bytes || (html_cost + h_add + 1) >= dst_cap) {
      break;
    }
    json_cost += j_add;
    html_cost += h_add;
    start--;
  }

  // Advance forward past any clipped leading fragment (only shrinks output)
  if ((start > 0 && raw[start - 1] != '\n') || (start == 0 && ring_wrapped)) {
    const void* nl_ptr = memchr(raw + start, '\n', raw_len - start);
    if (nl_ptr != nullptr) {
      size_t nl_idx = static_cast<size_t>(
          static_cast<const char*>(nl_ptr) - raw);
      if (nl_idx + 1 < raw_len) {
        start = nl_idx + 1;
      }
    }
  }

  size_t w = 0;
  for (size_t i = start; i < raw_len; ++i) {
    char c = raw[i];
    if (c == '&') {
      memcpy(dst + w, HTML_ENT_AMP, kAmpLen); w += kAmpLen;
    } else if (c == '<') {
      memcpy(dst + w, HTML_ENT_LT,  kLtLen);  w += kLtLen;
    } else if (c == '>') {
      memcpy(dst + w, HTML_ENT_GT,  kGtLen);  w += kGtLen;
    } else if (static_cast<uint8_t>(c) >= 0x20 || c == '\n' || c == '\r' || c == '\t') {
      dst[w++] = c;
    }
  }
  dst[w] = '\0';
}

/*****************************************************************************/
// Z2S_ESPUI Button Callbacks (dispatched on loopTask by ESPUI.loop())
/*****************************************************************************/

void onClearSafeModeBtn(BasicControl* /*sender*/, int type, void* /*param*/) {
  if (type != B_UP) return;
  Z2S_LuaEngine::instance().clearSafeMode(true);
  Z2S_loopLuaWebGUI(true);
}

void onReloadFsBtn(BasicControl* /*sender*/, int type, void* /*param*/) {
  if (type != B_UP) return;
  Z2S_LuaEngine::instance().requestReloadFromFS(false, false);
  Z2S_loopLuaWebGUI(true);
}

void onStopVmBtn(BasicControl* /*sender*/, int type, void* /*param*/) {
  if (type != B_UP) return;
  Z2S_LuaEngine::instance().stopVM();
  Z2S_loopLuaWebGUI(true);
}

void onDeleteScriptBtn(BasicControl* /*sender*/, int type, void* /*param*/) {
  if (type != B_UP) return;

  uint32_t now = millis();
  if (s_delete_armed_ms != 0 && (now - s_delete_armed_ms) <= DELETE_CONFIRM_MS) {
    s_delete_armed_ms = 0;
    ESPUI.updateControlValue(s_btn_delete_script, "Delete Script");
    Z2S_LuaEngine::instance().deleteScriptAndStop();
    Z2S_loopLuaWebGUI(true);
  } else {
    s_delete_armed_ms = now ? now : 1;
    ESPUI.updateControlValue(s_btn_delete_script, "Confirm Delete?");
  }
}

void onClearConsoleBtn(BasicControl* /*sender*/, int type, void* /*param*/) {
  if (type != B_UP) return;
  Z2S_LuaEngine::instance().clearConsoleLog();
  Z2S_loopLuaWebGUI(true);
}

}  // namespace

/*****************************************************************************/
// Public Build / HTTP Endpoints / Loop Refresh
/*****************************************************************************/

void Z2S_buildLuaWebGUI() {
  if (s_gui_built) return;

  if (!s_get_mtx)  s_get_mtx  = xSemaphoreCreateMutexStatic(&s_get_mtx_buf);
  if (!s_post_mtx) s_post_mtx = xSemaphoreCreateMutexStatic(&s_post_mtx_buf);

  if (!s_http_get_buf) {
    s_http_get_buf = static_cast<char*>(
        z2s_gui_malloc(Z2S_LuaEngine::MAX_SCRIPT_BYTES + 1));
  }
  if (!s_http_post_buf) {
    s_http_post_buf = static_cast<char*>(
        z2s_gui_malloc(Z2S_LuaEngine::MAX_SCRIPT_BYTES + 1));
  }

  s_tab_lua = ESPUI.addControl(
      Control::Type::Tab, "Lua", "Lua", Control::Color::Dark);

  // 1. Safe-Mode Status Card (wide root Label + inline recovery Button)
  s_lbl_safemode = ESPUI.addControl(
      Control::Type::Label, "Safe Mode Status",
      "Normal mode (boot crash guard active)",
      Control::Color::Emerald, s_tab_lua);
  ESPUI.setPanelWide(s_lbl_safemode, true);
  ESPUI.setElementStyle(s_lbl_safemode, STYLE_SAFE_OK);

  s_btn_clear_safemode = ESPUI.addControl(
      Control::Type::Button, "Recovery", "Clear Safe Mode & Reload",
      Control::Color::Alizarin, s_lbl_safemode, onClearSafeModeBtn);
  ESPUI.setEnabled(s_btn_clear_safemode, false);

  // 2. Live VM Telemetry Card
  s_lbl_telemetry = ESPUI.addControl(
      Control::Type::Label, "Lua VM Telemetry", "Initializing...",
      Control::Color::Peterriver, s_tab_lua);
  ESPUI.setPanelWide(s_lbl_telemetry, true);
  ESPUI.setElementStyle(s_lbl_telemetry, STYLE_TEL_INFO);

  // 3. Script Editor Card (static Flash DROM pointer -> 0 RAM copy)
  s_lbl_editor = ESPUI.addControl(
      Control::Type::Label, "Lua Script Editor", LUA_EDITOR_HTML,
      Control::Color::Dark, s_tab_lua);
  ESPUI.setPanelWide(s_lbl_editor, true);
  ESPUI.setElementStyle(s_lbl_editor, STYLE_EDITOR_WRAP);

  s_btn_reload_fs = ESPUI.addControl(
      Control::Type::Button, "Reload", "Reload from Flash",
      Control::Color::Peterriver, s_lbl_editor, onReloadFsBtn);

  s_btn_stop_vm = ESPUI.addControl(
      Control::Type::Button, "Stop", "Stop VM",
      Control::Color::Carrot, s_lbl_editor, onStopVmBtn);

  s_btn_delete_script = ESPUI.addControl(
      Control::Type::Button, "Delete", "Delete Script",
      Control::Color::Alizarin, s_lbl_editor, onDeleteScriptBtn);

  // 4. Live Console Output Card (tail-capped for s_batch_buf)
  s_lbl_console = ESPUI.addControl(
      Control::Type::Label, "Lua Console Output", "(empty)",
      Control::Color::Dark, s_tab_lua);
  ESPUI.setPanelWide(s_lbl_console, true);
  ESPUI.setElementStyle(s_lbl_console, STYLE_CONSOLE);

  s_btn_clear_console = ESPUI.addControl(
      Control::Type::Button, "Console", "Clear Console",
      Control::Color::Wetasphalt, s_lbl_console, onClearConsoleBtn);

  s_gui_built = true;
  Z2S_loopLuaWebGUI(true);
}

void Z2S_registerLuaHttpEndpoints() {
  AsyncWebServer* srv = ESPUI.WebServer();
  if (s_routes_registered || !srv) {
    return;
  }

  // GET /api/lua/script — chunked PSRAM stream (zero Arduino String)
  srv->on(
      "/api/lua/script", HTTP_GET,
      [](AsyncWebServerRequest* request) {
        if (!s_http_get_buf || !s_get_mtx) {
          request->send(503, "text/plain", "Lua buffer unavailable");
          return;
        }
        if (xSemaphoreTake(s_get_mtx, pdMS_TO_TICKS(200)) != pdTRUE) {
          request->send(503, "text/plain", "Busy");
          return;
        }

        size_t len = 0;
        Z2S_LuaEngine::instance().readStoredScript(
            s_http_get_buf, Z2S_LuaEngine::MAX_SCRIPT_BYTES + 1, len);
        s_http_get_len = len;
        xSemaphoreGive(s_get_mtx);

        AsyncWebServerResponse* resp = request->beginChunkedResponse(
            "text/plain; charset=utf-8",
            [len](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
              if (index >= len || !s_http_get_buf) return 0;
              if (xSemaphoreTake(s_get_mtx, pdMS_TO_TICKS(50)) != pdTRUE) return 0;

              size_t avail = (index < s_http_get_len) ? (s_http_get_len - index) : 0;
              size_t chunk = (avail < maxLen) ? avail : maxLen;
              if (chunk > 0) {
                memcpy(buffer, s_http_get_buf + index, chunk);
              }
              xSemaphoreGive(s_get_mtx);
              return chunk;
            });
        resp->addHeader("Cache-Control", "no-store");
        resp->addHeader("Connection", "close");
        request->send(resp);
      });

  // POST /api/lua/script — raw body into PSRAM staging buffer, validate, then save & reload
  srv->on(
      "/api/lua/script", HTTP_POST,
      [](AsyncWebServerRequest* request) {
        if (!s_http_post_buf || !s_post_mtx) {
          request->send(503, "text/plain", "Lua buffer unavailable");
          return;
        }
        if (xSemaphoreTake(s_post_mtx, pdMS_TO_TICKS(500)) != pdTRUE) {
          request->send(503, "text/plain", "Busy");
          return;
        }

        bool overflow   = s_post_overflow;
        size_t body_len = s_http_post_len;
        s_post_overflow = false;
        s_http_post_len = 0;

        if (overflow) {
          xSemaphoreGive(s_post_mtx);
          request->send(413, "text/plain", "Error: script exceeds 16384 B");
          return;
        }

        if (body_len == 0) {
          xSemaphoreGive(s_post_mtx);
          if (request->contentLength() > 0) {
            log_e("[LUA] POST body missing (Content-Length=%u, Content-Type=%s)",
                  static_cast<unsigned>(request->contentLength()),
                  request->contentType().c_str());
            request->send(400, "text/plain", "Error: body not received");
            return;
          }
          bool ok = Z2S_LuaEngine::instance().deleteScriptAndStop();
          request->send(ok ? 200 : 500, "text/plain",
                        ok ? "Script cleared" : "Clear failed");
          return;
        }

        // Pre-save syntax gate: rejects broken Lua without touching LittleFS or running VM
        char err_buf[192];
        if (!Z2S_LuaEngine::instance().validateSyntax(
                s_http_post_buf, body_len, err_buf, sizeof(err_buf))) {
          xSemaphoreGive(s_post_mtx);
          request->send(400, "text/plain", err_buf);
          return;
        }

        bool ok = Z2S_LuaEngine::instance().saveAndReload(s_http_post_buf, body_len);
        xSemaphoreGive(s_post_mtx);

        if (ok) {
          char reply[64];
          snprintf(reply, sizeof(reply), "Saved & reloading (%u B)",
                   static_cast<unsigned>(body_len));
          request->send(200, "text/plain", reply);
        } else {
          request->send(500, "text/plain", "Save failed (check telemetry)");
        }
      },
      nullptr,
      [](AsyncWebServerRequest* /*request*/, uint8_t* data, size_t len,
         size_t index, size_t total) {
        if (!s_http_post_buf || !s_post_mtx) return;
        if (xSemaphoreTake(s_post_mtx, pdMS_TO_TICKS(200)) != pdTRUE) return;

        if (index == 0) {
          s_http_post_len = 0;
          s_post_overflow = (total > Z2S_LuaEngine::MAX_SCRIPT_BYTES);
        }

        if (s_post_overflow ||
            (index + len) > Z2S_LuaEngine::MAX_SCRIPT_BYTES) {
          s_post_overflow = true;
          xSemaphoreGive(s_post_mtx);
          return;
        }

        if (len > 0 && data) {
          memcpy(s_http_post_buf + index, data, len);
        }
        size_t end_pos = index + len;
        if (end_pos > s_http_post_len) {
          s_http_post_len = end_pos;
        }
        if (end_pos == total) {
          s_http_post_buf[end_pos] = '\0';
        }
        xSemaphoreGive(s_post_mtx);
      });

  s_routes_registered = true;
  log_i("[LUA] Registered /api/lua/script endpoints on Z2S_ESPUI");
}

void Z2S_loopLuaWebGUI(bool force) {
  if (!s_gui_built) return;

  uint32_t now = millis();

  if (s_delete_armed_ms != 0 && (now - s_delete_armed_ms) > DELETE_CONFIRM_MS) {
    s_delete_armed_ms = 0;
    ESPUI.updateControlValue(s_btn_delete_script, "Delete Script");
  }

  if (!force && (now - s_last_refresh_ms) < LUA_GUI_REFRESH_MS) {
    return;
  }
  s_last_refresh_ms = now;

  Z2S_LuaStatus st{};
  Z2S_LuaEngine::instance().getStatus(st);

  // 1. Safe-Mode banner
  if (force || st.safe_mode != s_last_safemode_ui) {
    s_last_safemode_ui = st.safe_mode;
    if (st.safe_mode) {
      char banner[160];
      snprintf(banner, sizeof(banner),
               "SAFE MODE ACTIVE: Script disabled after %u crash resets.\n"
               "Save a fixed script or click Clear Safe Mode & Reload.",
               static_cast<unsigned>(st.crash_count));
      ESPUI.setElementStyle(s_lbl_safemode, STYLE_SAFE_TRIP);
      ESPUI.updateControlValue(s_lbl_safemode, banner);
      ESPUI.setEnabled(s_btn_clear_safemode, true);
    } else {
      char banner[96];
      snprintf(banner, sizeof(banner),
               "Normal mode (boot crash counter: %u / %u)",
               static_cast<unsigned>(st.crash_count),
               static_cast<unsigned>(Z2S_LuaEngine::MAX_BOOT_CRASHES));
      ESPUI.setElementStyle(s_lbl_safemode, STYLE_SAFE_OK);
      ESPUI.updateControlValue(s_lbl_safemode, banner);
      ESPUI.setEnabled(s_btn_clear_safemode, false);
    }
  }

  // 2. Telemetry (hash-gated, zero String, guaranteed < 550 B)
  char err_esc[192];
  escapeHtml(st.last_error[0] ? st.last_error : "none",
             err_esc, sizeof(err_esc));

  char tel_buf[448];
  int tel_len = snprintf(
      tel_buf, sizeof(tel_buf),
      "State:   %s\n"
      "Script:  %u / %u B\n"
      "PSRAM:   %u B used | %u B peak | %u B arena (largest free: %u B)\n"
      "Events:  %lu processed | %lu dropped | %lu watchdog trips\n"
      "Error:   %s",
      luaStateName(st.state, st.autostart_enabled),
      static_cast<unsigned>(st.script_size),
      static_cast<unsigned>(Z2S_LuaEngine::MAX_SCRIPT_BYTES),
      static_cast<unsigned>(st.used_bytes),
      static_cast<unsigned>(st.peak_bytes),
      static_cast<unsigned>(st.arena_bytes),
      static_cast<unsigned>(st.largest_free_block),
      static_cast<unsigned long>(st.events_processed),
      static_cast<unsigned long>(st.events_dropped),
      static_cast<unsigned long>(st.watchdog_trips),
      err_esc);

  if (tel_len > 0) {
    size_t n = (static_cast<size_t>(tel_len) < sizeof(tel_buf))
                   ? static_cast<size_t>(tel_len)
                   : (sizeof(tel_buf) - 1);
    uint32_t h = fnv1a32(tel_buf, n);
    if (force || h != s_last_status_hash) {
      s_last_status_hash = h;
      const char* style = STYLE_TEL_INFO;
      if (st.state == Z2S_LuaState::RUNNING) {
        style = STYLE_TEL_OK;
      } else if (st.state == Z2S_LuaState::ERROR ||
                 st.state == Z2S_LuaState::SAFE_MODE) {
        style = STYLE_TEL_ERR;
      }
      ESPUI.setElementStyle(s_lbl_telemetry, style);
      ESPUI.updateControlValue(s_lbl_telemetry, tel_buf);
    }
  }

  // 3. Console output (newest tail capped to <= 960 JSON bytes, clean top line)
  char raw_log[Z2S_LuaEngine::CONSOLE_BUF_SIZE];
  bool console_wrapped = false;
  size_t raw_len = Z2S_LuaEngine::instance().copyConsoleLog(
      raw_log, sizeof(raw_log), &console_wrapped);
  uint32_t chash = fnv1a32(raw_log, raw_len);

  if (force || chash != s_last_console_hash) {
    s_last_console_hash = chash;
    char esc_log[1024];
    formatConsoleTailForBatch(
        raw_log, raw_len, esc_log, sizeof(esc_log),
        CONSOLE_JSON_CAP, console_wrapped);
    ESPUI.updateControlValue(s_lbl_console, esc_log);
  }
}

#else
typedef int z2s_lua_web_gui_disabled_stub_t;
#endif  // Z2S_ENABLE_LUA