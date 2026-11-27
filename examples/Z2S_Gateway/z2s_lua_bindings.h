#pragma once

#include <Z2S_Lua_Config.h>

#if Z2S_ENABLE_LUA

#include <cstddef>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <supla/action_handler.h>
#include <supla/local_action.h>
#include <Z2S_Lua_API.h>

/*****************************************************************************/

struct Z2S_LuaEvent {
  uint8_t  channel;
  uint8_t  watch_slot;
  uint16_t event_id;
};

enum class Z2S_LuaCmdType : uint8_t {
  ACTION = 0,
  SET_PIN,
  SET_BRIGHTNESS,
  SET_POSITION,
  WATCH,
  UNWATCH_ALL
};

struct Z2S_LuaCmd {
  Z2S_LuaCmdType type;
  uint8_t        channel;
  uint8_t        arg_u8;   // pin (0..31) or pct (0..100)
  uint8_t        sub_op;   // 0=clear, 1=set, 2=toggle
  uint32_t       arg_u32;  // action_id or event_id
};

/*****************************************************************************/

class Z2S_LuaBridge {
 public:
  static constexpr size_t CMD_QUEUE_DEPTH = 32;
  static constexpr size_t MAX_WATCHES     = 32;

  static Z2S_LuaBridge& instance() {
    static Z2S_LuaBridge inst;
    return inst;
  }

  // Call once from setup() before starting Z2S_LuaEngine
  void begin(void (*event_sink)(const Z2S_LuaEvent& ev));

  // Register global `z2s` table inside a fresh lua_State (worker task only)
  static void registerBindings(lua_State* L);

  static const char* eventAlias(uint16_t supla_evt);

  // Queue an UNWATCH_ALL command (called by worker before reload/stop)
  bool enqueueUnwatchAll();

  // MUST be called only from Arduino loop() right after SuplaDevice.iterate()
  void drainCommands();

 private:
  Z2S_LuaBridge() = default;

  bool enqueueCmd(const Z2S_LuaCmd& cmd);

  // Main-loop execution helpers (never called from Lua worker task)
  void execAction(uint8_t ch, uint32_t raw_action_id);
  void execSetPin(uint8_t gate_ch, uint8_t pin, uint8_t sub_op);
  void execSetBrightness(uint8_t ch, uint8_t pct);
  void execSetPosition(uint8_t ch, uint8_t pct);
  void execWatch(uint8_t ch, uint32_t raw_event_id);
  void execUnwatchAll();

  static bool isClientAlive(Supla::ActionHandlerClient* target);

  // Single static receiver shared by all z2s.watch() subscriptions
  class WatchReceiver : public Supla::ActionHandler {
   public:
    void handleAction(int event, int action) override;
  };

  struct WatchSlot {
    uint8_t                     channel    = 0xFF;
    uint16_t                    supla_evt  = 0;
    bool                        active     = false;
    Supla::ActionHandlerClient* client_ptr = nullptr;
  };

  // Lua C bindings (called from Core 1 Lua worker task)
  static int l_get(lua_State* L);
  static int l_get_pin(lua_State* L);
  static int l_set_pin(lua_State* L);
  static int l_action(lua_State* L);
  static int l_set_brightness(lua_State* L);
  static int l_set_position(lua_State* L);
  static int l_watch(lua_State* L);
  static int l_uptime_s(lua_State* L);
  static int l_time(lua_State* L);
  static int l_sun(lua_State* L);
  static int l_set_location(lua_State* L);
  static int l_tick_ms(lua_State* L);
  static int l_elapsed_ms(lua_State* L);

  QueueHandle_t _cmd_queue = nullptr;
  StaticQueue_t _cmd_queue_struct = {};
  uint8_t       _cmd_queue_storage[CMD_QUEUE_DEPTH * sizeof(Z2S_LuaCmd)] = {};

  WatchReceiver _watch_receiver;
  WatchSlot     _watches[MAX_WATCHES] = {};
  void (*_event_sink)(const Z2S_LuaEvent& ev) = nullptr;
};

#endif  // Z2S_ENABLE_LUA