#pragma once

#include <Z2S_Lua_Config.h>

#if Z2S_ENABLE_LUA

#include <cstddef>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <multi_heap.h>

#include <Z2S_Lua_API.h>
#include "z2s_lua_bindings.h"

/*****************************************************************************/

enum class Z2S_LuaState : uint8_t {
  UNINITIALIZED = 0,
  NO_SCRIPT,
  RUNNING,
  ERROR,
  SAFE_MODE,
  STOPPED
};

struct Z2S_LuaStatus {
  Z2S_LuaState state;
  bool         safe_mode;
  uint8_t      crash_count;
  size_t       arena_bytes;
  size_t       used_bytes;
  size_t       peak_bytes;
  size_t       largest_free_block;
  size_t       script_size;
  uint32_t     events_processed;
  uint32_t     events_dropped;
  uint32_t     watchdog_trips;
  char         last_error[128];
};

/*****************************************************************************/

class Z2S_LuaEngine {
 public:
  static constexpr size_t   ARENA_BYTES      = 128 * 1024;
  static constexpr size_t   SOFT_LIMIT_BYTES = 112 * 1024;
  static constexpr size_t   MAX_SCRIPT_BYTES = 16 * 1024;
  static constexpr size_t   MSG_QUEUE_DEPTH  = 32;
  static constexpr size_t   CONSOLE_BUF_SIZE = 1024;
  static constexpr uint32_t TASK_STACK_BYTES = 8192;
  static constexpr uint32_t TASK_STACK_DEPTH = TASK_STACK_BYTES / sizeof(StackType_t);

  static constexpr uint8_t  MAX_BOOT_CRASHES  = 2;
  static constexpr uint32_t STABILITY_HEAL_MS = 60000;
  static constexpr int      HOOK_STEP_INSTR   = 512;
  static constexpr uint32_t INIT_MAX_INSTR    = 40000;
  static constexpr int64_t  INIT_MAX_WALL_US  = 30000;
  static constexpr uint32_t EVENT_MAX_INSTR   = 25000;
  static constexpr int64_t  EVENT_MAX_WALL_US = 20000;

  static constexpr const char* SCRIPT_FILE     = "lua_script_v1.z2s";
  static constexpr const char* SCRIPT_BAK_FILE = "lua_script_v1.bak";

  static Z2S_LuaEngine& instance() {
    static Z2S_LuaEngine inst;
    return inst;
  }

  // Call once from setup() after Z2S_initSuplaActions() and Z2S_LuaBridge::begin()
  bool begin();

  // Fast-path event ingress called from Z2S_LuaBridge::WatchReceiver (non-blocking)
  bool enqueueEvent(const Z2S_LuaEvent& ev);

  // Web GUI / user API (safe to call from AsyncWebServer or loopTask)
  bool saveAndReload(const char* src, size_t len);
  bool requestReloadFromFS(bool clear_safe_mode = false);
  bool deleteScriptAndStop();
  bool clearSafeMode(bool reload_now = false);

  // Read stored script from LittleFS into caller buffer for Web GUI editor
  bool readStoredScript(char* dst, size_t dst_cap, size_t& out_len);

  // Call on STATUS_SOFTWARE_RESET before Z2S_endLittleFs(true)
  void requestStop(uint32_t wait_ms = 200);

  // Thread-safe snapshots for Web GUI diagnostics (zero heap, zero multi_heap races)
  void   getStatus(Z2S_LuaStatus& out);
  size_t copyConsoleLog(char* dst, size_t dst_cap);
  void   clearConsoleLog();

  // Internal hook called by extern "C" z2s_lua_write()
  void appendConsole(const char* s, size_t len);

 private:
  Z2S_LuaEngine() = default;

  enum class MsgType : uint8_t {
    EVENT = 0,
    RELOAD_FS,
    CLEAR_SCRIPT,
    STOP
  };

  struct WorkerMsg {
    MsgType      type;
    uint8_t      reserved[3];
    Z2S_LuaEvent ev;
  };

  struct SandboxCtx {
    multi_heap_handle_t heap             = nullptr;
    void*               slab_ptr         = nullptr;
    size_t              slab_bytes       = 0;
    size_t              soft_limit_bytes = 0;
    size_t              used_bytes       = 0;
    size_t              peak_bytes       = 0;

    int64_t       deadline_us      = 0;
    uint32_t      instr_executed   = 0;
    uint32_t      instr_limit      = 0;
    volatile bool force_abort      = false;
    bool          watchdog_tripped = false;
  };

  // Boot-loop crash guard helpers (RTC_NOINIT_ATTR)
  void evaluateRtcGuardOnBoot();
  void markExecEnter();
  void markExecExit();
  void maybeHealCrashCounter();

  // Worker-task-only methods (single owner of _L and _ctx.heap)
  static void workerEntry(void* arg);
  void        workerLoop();
  bool        resetVM();
  void        closeVM();
  void        handleReloadFromFS();
  void        handleClearScript();
  void        handleEvent(const Z2S_LuaEvent& ev);
  void        flushStaleEvents();
  void        publishHeapSnapshot();

  bool        loadScriptChunk(const char* src, size_t len, const char* name);
  void        captureLuaError(const char* stage);
  void        setErrorStatus(Z2S_LuaState st, const char* msg);
  void        publishState(Z2S_LuaState st);

  static void* luaArenaAlloc(void* ud, void* ptr, size_t osize, size_t nsize);
  static void  luaWatchdogHook(lua_State* L, lua_Debug* ar);
  static void  armWatchdog(lua_State* L, SandboxCtx& ctx,
                           uint32_t max_instr, int64_t max_us);
  static void  openSafeStdlib(lua_State* L);

  // Worker-owned VM state
  lua_State* _L          = nullptr;
  SandboxCtx _ctx        = {};
  char*      _script_buf = nullptr;  // 16 KB + 1 in PSRAM, allocated once

  // Synchronized status & console telemetry
  portMUX_TYPE  _stats_mux          = portMUX_INITIALIZER_UNLOCKED;
  Z2S_LuaState  _state              = Z2S_LuaState::UNINITIALIZED;
  bool          _safe_mode          = false;
  uint8_t       _crash_count        = 0;
  size_t        _used_bytes_pub     = 0;
  size_t        _peak_bytes_pub     = 0;
  size_t        _largest_free_pub   = 0;
  size_t        _script_size        = 0;
  uint32_t      _script_loaded_ms   = 0;
  uint32_t      _events_processed   = 0;
  uint32_t      _events_dropped     = 0;
  uint32_t      _watchdog_trips     = 0;
  char          _last_error[128]    = {};

  char          _console_buf[CONSOLE_BUF_SIZE] = {};
  size_t        _console_head       = 0;
  size_t        _console_len        = 0;

  volatile bool _worker_stopped     = false;
  volatile bool _started            = false;

  // Static FreeRTOS primitives (internal DRAM)
  QueueHandle_t _msg_queue          = nullptr;
  StaticQueue_t _msg_queue_struct   = {};
  uint8_t       _msg_queue_storage[MSG_QUEUE_DEPTH * sizeof(WorkerMsg)] = {};

  TaskHandle_t  _task               = nullptr;
  StaticTask_t  _task_tcb           = {};
  StackType_t   _task_stack[TASK_STACK_DEPTH] = {};
};

#endif  // Z2S_ENABLE_LUA