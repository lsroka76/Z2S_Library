#include "z2s_lua_bindings.h"

#if Z2S_ENABLE_LUA

#include <Arduino.h>
#include <cmath>
#include <cstring>
#include <ctime>

#include <ZigbeeGateway.h>
#include <SuplaDevice.h>
#include <supla/auto_lock.h>
#include <supla/control/hvac_base.h>
#include <supla/control/roller_shutter_interface.h>
#include <supla/sensor/electricity_meter.h>
#include <supla/sensor/general_purpose_channel_base.h>

#include <Z2S_common.h>
#include <Z2S_custom_actions_events.h>
#include <Z2S_control/Z2S_local_action_handlers.h>
#include <Z2S_control/Z2S_dimmer_interface.h>
#include <Z2S_control/Z2S_rgbcct_interface.h>

#include "z2s_devices_table.h"

/*****************************************************************************/
// Pure C++ snapshot types & native enum tables (ZERO Lua calls under mutex)
/*****************************************************************************/

namespace {

// 2024-01-01 00:00:00 UTC — anything older means RTC/clock has not synced yet
static constexpr time_t Z2S_LUA_MIN_VALID_TIME = 1704067200;

enum class LuaValKind : uint8_t { NIL, BOOL, INT, NUMBER };

struct LuaValSnapshot {
  LuaValKind kind = LuaValKind::NIL;
  union {
    bool    b;
    int32_t i;
    float   f;
  } u{};

  static LuaValSnapshot makeNil()          { return {}; }
  static LuaValSnapshot makeBool(bool v)   { LuaValSnapshot s; s.kind = LuaValKind::BOOL;   s.u.b = v; return s; }
  static LuaValSnapshot makeInt(int32_t v) { LuaValSnapshot s; s.kind = LuaValKind::INT;    s.u.i = v; return s; }
  static LuaValSnapshot makeNum(double v)  {
    if (std::isnan(v) || std::isinf(v)) return makeNil();
    LuaValSnapshot s; s.kind = LuaValKind::NUMBER; s.u.f = static_cast<float>(v); return s;
  }
};

struct NameIdEntry {
  const char* name;
  uint32_t    id;
};

static constexpr NameIdEntry ACTION_ALIASES[] = {
  {"on",            static_cast<uint32_t>(Supla::Action::TURN_ON)},
  {"off",           static_cast<uint32_t>(Supla::Action::TURN_OFF)},
  {"toggle",        static_cast<uint32_t>(Supla::Action::TOGGLE)},
  {"set",           static_cast<uint32_t>(Supla::Action::SET)},
  {"clear",         static_cast<uint32_t>(Supla::Action::CLEAR)},
  {"open",          static_cast<uint32_t>(Supla::Action::OPEN)},
  {"close",         static_cast<uint32_t>(Supla::Action::CLOSE)},
  {"stop",          static_cast<uint32_t>(Supla::Action::STOP)},
  {"open_or_stop",  static_cast<uint32_t>(Supla::Action::OPEN_OR_STOP)},
  {"close_or_stop", static_cast<uint32_t>(Supla::Action::CLOSE_OR_STOP)},
  {"brighten",      static_cast<uint32_t>(Supla::Action::BRIGHTEN_W)},
  {"dim",           static_cast<uint32_t>(Supla::Action::DIM_W)},
  {"enable",        static_cast<uint32_t>(Supla::Action::ENABLE)},
  {"disable",       static_cast<uint32_t>(Supla::Action::DISABLE)},
  {"reset",         static_cast<uint32_t>(Supla::Action::RESET)},
  {"temp_up",       static_cast<uint32_t>(Supla::Action::INCREASE_TEMPERATURE)},
  {"temp_down",     static_cast<uint32_t>(Supla::Action::DECREASE_TEMPERATURE)},
  {"manual",        static_cast<uint32_t>(Supla::Action::SWITCH_TO_MANUAL_MODE)},
  {"heat",          static_cast<uint32_t>(Supla::Action::SWITCH_TO_MANUAL_MODE_HEAT)},
  {"cool",          static_cast<uint32_t>(Supla::Action::SWITCH_TO_MANUAL_MODE_COOL)},
  {"heat_cool",     static_cast<uint32_t>(Supla::Action::SWITCH_TO_MANUAL_MODE_HEAT_COOL)},
  {"schedule",      static_cast<uint32_t>(Supla::Action::SWITCH_TO_WEEKLY_SCHEDULE_MODE)},
};

static constexpr NameIdEntry EVENT_ALIASES[] = {
  {"change",        static_cast<uint32_t>(Supla::Event::ON_CHANGE)},
  {"on",            static_cast<uint32_t>(Supla::Event::ON_TURN_ON)},
  {"off",           static_cast<uint32_t>(Supla::Event::ON_TURN_OFF)},
  {"open",          static_cast<uint32_t>(Supla::Event::ON_OPEN)},
  {"close",         static_cast<uint32_t>(Supla::Event::ON_CLOSE)},
  {"press",         static_cast<uint32_t>(Supla::Event::ON_PRESS)},
  {"release",       static_cast<uint32_t>(Supla::Event::ON_RELEASE)},
  {"hold",          static_cast<uint32_t>(Supla::Event::ON_HOLD)},
  {"hold_release",  static_cast<uint32_t>(Supla::Event::ON_HOLD_RELEASE)},
  {"click1",        static_cast<uint32_t>(Supla::Event::ON_CLICK_1)},
  {"click2",        static_cast<uint32_t>(Supla::Event::ON_CLICK_2)},
  {"click3",        static_cast<uint32_t>(Supla::Event::ON_CLICK_3)},
  {"click4",        static_cast<uint32_t>(Supla::Event::ON_CLICK_4)},
  {"click5",        static_cast<uint32_t>(Supla::Event::ON_CLICK_5)},
  {"online",        static_cast<uint32_t>(Supla::Event::ON_CHANNEL_ONLINE)},
  {"offline",       static_cast<uint32_t>(Supla::Event::ON_CHANNEL_OFFLINE)},
  {"hvac_heating",  static_cast<uint32_t>(Supla::Event::ON_HVAC_HEATING)},
  {"hvac_cooling",  static_cast<uint32_t>(Supla::Event::ON_HVAC_COOLING)},
  {"hvac_standby",  static_cast<uint32_t>(Supla::Event::ON_HVAC_STANDBY)},
  {"tick5",         Z2S_SUPLA_EVENT_ON_EVERY_5_SECONDS},
  {"tick30",        Z2S_SUPLA_EVENT_ON_EVERY_30_SECONDS},
  {"tick60",        Z2S_SUPLA_EVENT_ON_EVERY_60_SECONDS},
  {"tick24h",       Z2S_SUPLA_EVENT_ON_EVERY_24_HOURS},
};

static bool lookupAlias(const NameIdEntry* table, size_t count,
                        const char* key, uint32_t& out_id) {
  if (!key) return false;
  for (size_t i = 0; i < count; ++i) {
    if (strcmp(table[i].name, key) == 0) {
      out_id = table[i].id;
      return true;
    }
  }
  return false;
}

// Numeric IDs are restricted to stable Z2S custom ranges (0x4000..0x5FFF, 0x8000..0xFFFF),
// excluding raw <0x4000 Supla enums and 0x6000..0x6065 LittleFS persistence IDs.
static bool isCustomZ2SActionId(uint32_t id) {
  return (id >= Z2S_SUPLA_ACTION_FIRST_ACTION) &&
         (id <= 0xFFFFUL) &&
         !(id >= Z2S_SUPLA_ACTION_CORE_ACTIONS_FIRST_ACTION &&
           id <= Z2S_SUPLA_ACTION_CORE_ACTIONS_LAST_ACTION);
}

static bool isCustomZ2SEventId(uint32_t id) {
  return (id >= Z2S_SUPLA_EVENT_FIRST_EVENT) &&
         (id <= 0xFFFFUL) &&
         !(id >= Z2S_SUPLA_EVENT_CORE_EVENT_FIRST_EVENT &&
           id <= Z2S_SUPLA_EVENT_CORE_EVENTS_LAST_EVENT);
}

static bool fieldEq(const char* field, const char* literal) {
  return field && (strcmp(field, literal) == 0);
}

static inline void luaSetIntField(lua_State* L, const char* key, lua_Integer val) {
  lua_pushinteger(L, val);
  lua_setfield(L, -2, key);
}

// Pure C++ channel reader: holds TimerAccessMutex strictly inside this scope
static LuaValSnapshot readChannelSnapshot(
    uint8_t ch_num, const char* field, int32_t phase_arg) {

  Supla::AutoLock lock(SuplaDevice.getTimerAccessMutex());

  if (ch_num == GATEWAY_EVENTS_CHANNEL_NUMBER) {
    return fieldEq(field, "online") ? LuaValSnapshot::makeBool(true)
                                    : LuaValSnapshot::makeNil();
  }

  Supla::Element* el   = Z2S_Core::getSuplaElementByChannelNumber(ch_num);
  Z2S_Core*       core = Z2S_Core::getZ2SCoreByChannelNumber(ch_num);
  if (!el) {
    return LuaValSnapshot::makeNil();
  }

  Supla::Channel* ch = el->getChannel();

  // Universal diagnostic fields
  if (fieldEq(field, "online")) {
    return LuaValSnapshot::makeBool(ch ? ch->isStateOnline() : true);
  }
  if (fieldEq(field, "battery")) {
    uint8_t bat = core ? core->getZbDeviceLastBatteryLevelPercentage() : 0xFF;
    if (bat > 100 && ch) bat = ch->getBatteryLevel();
    return (bat <= 100) ? LuaValSnapshot::makeInt(bat) : LuaValSnapshot::makeNil();
  }
  if (fieldEq(field, "rssi")) {
    return core ? LuaValSnapshot::makeInt(core->getZbDeviceLastRSSI())
                : LuaValSnapshot::makeNil();
  }

  // Non-Supla-channel local element (Logic Gate)
  if (!ch) {
    if (core && core->getZ2SLocalChannelType() == LOCAL_CHANNEL_TYPE_ACTION_HANDLER) {
      if (fieldEq(field, "mask") || fieldEq(field, "value")) {
        return LuaValSnapshot::makeInt(
            static_cast<int32_t>(core->getLogicValue() & core->getLogicMask()));
      }
      if (fieldEq(field, "pins")) {
        return LuaValSnapshot::makeInt(core->getLogicPinsNumber());
      }
      uint8_t res = core->getLastLogicResult();
      return (res == 0xFF) ? LuaValSnapshot::makeNil()
                           : LuaValSnapshot::makeBool(res == 1);
    }
    return LuaValSnapshot::makeNil();
  }

  switch (ch->getChannelType()) {
    case SUPLA_CHANNELTYPE_BINARYSENSOR:
      return LuaValSnapshot::makeBool(ch->getValueBool());

    case SUPLA_CHANNELTYPE_RELAY:
      if (ch->isRollerShutterRelayType()) {
        auto* rs = static_cast<Supla::Control::RollerShutterInterface*>(el);
        int pos = rs->getCurrentPosition();
        return (pos >= 0 && pos <= 100) ? LuaValSnapshot::makeInt(pos)
                                        : LuaValSnapshot::makeNil();
      }
      return LuaValSnapshot::makeBool(ch->getValueBool());

    case SUPLA_CHANNELTYPE_VALVE_OPENCLOSE: {
      // TValve_Value byte 0: 0 = open, non-zero = closed (same byte read by getValueBool)
      bool closed = ch->getValueBool();
      return LuaValSnapshot::makeBool(fieldEq(field, "closed") ? closed : !closed);
    }

    case SUPLA_CHANNELTYPE_THERMOMETER: {
      double t = ch->getValueDouble();
      return (t <= -272.0) ? LuaValSnapshot::makeNil() : LuaValSnapshot::makeNum(t);
    }

    case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {
      if (fieldEq(field, "humi") || fieldEq(field, "humidity")) {
        double h = ch->getValueDoubleSecond();
        return (h < 0.0) ? LuaValSnapshot::makeNil() : LuaValSnapshot::makeNum(h);
      }
      double t = ch->getValueDoubleFirst();
      return (t <= -272.0) ? LuaValSnapshot::makeNil() : LuaValSnapshot::makeNum(t);
    }

    case SUPLA_CHANNELTYPE_PRESSURESENSOR: {
      double p = ch->getValueDouble();
      return (p < 0.0) ? LuaValSnapshot::makeNil() : LuaValSnapshot::makeNum(p);
    }

    case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT:
    case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_METER: {
      auto* gpm = static_cast<Supla::Sensor::GeneralPurposeChannelBase*>(el);
      return LuaValSnapshot::makeNum(gpm->getCalculatedValue());
    }

    case SUPLA_CHANNELTYPE_DIMMER: {
      if (core && core->getZ2SLocalChannelType() == LOCAL_CHANNEL_TYPE_GPIO_PWM) {
        auto* pwm = static_cast<Supla::Control::LocalPWM*>(el);
        return LuaValSnapshot::makeInt(pwm->getPWM());
      }
      if (fieldEq(field, "cct")) {
        return LuaValSnapshot::makeInt(ch->getValueWhiteTemperature());
      }
      return LuaValSnapshot::makeInt(ch->getValueBrightness());
    }

    case SUPLA_CHANNELTYPE_RGBLEDCONTROLLER: {
      if (fieldEq(field, "r")) return LuaValSnapshot::makeInt(ch->getValueRed());
      if (fieldEq(field, "g")) return LuaValSnapshot::makeInt(ch->getValueGreen());
      if (fieldEq(field, "b")) return LuaValSnapshot::makeInt(ch->getValueBlue());
      return LuaValSnapshot::makeInt(ch->getValueColorBrightness());
    }

    case SUPLA_CHANNELTYPE_DIMMERANDRGBLED: {
      if (fieldEq(field, "color")) return LuaValSnapshot::makeInt(ch->getValueColorBrightness());
      if (fieldEq(field, "cct"))   return LuaValSnapshot::makeInt(ch->getValueWhiteTemperature());
      if (fieldEq(field, "r"))     return LuaValSnapshot::makeInt(ch->getValueRed());
      if (fieldEq(field, "g"))     return LuaValSnapshot::makeInt(ch->getValueGreen());
      if (fieldEq(field, "b"))     return LuaValSnapshot::makeInt(ch->getValueBlue());
      return LuaValSnapshot::makeInt(ch->getValueBrightness());
    }

    case SUPLA_CHANNELTYPE_HVAC: {
      auto* hvac = static_cast<Supla::Control::HvacBase*>(el);
      if (fieldEq(field, "mode")) {
        return LuaValSnapshot::makeInt(hvac->getMode());
      }
      if (fieldEq(field, "cool")) {
        int sp = hvac->getTemperatureSetpointCool();
        return (sp == INT16_MIN) ? LuaValSnapshot::makeNil()
                                 : LuaValSnapshot::makeNum(sp * 0.01);
      }
      int sp = hvac->getTemperatureSetpointHeat();
      return (sp == INT16_MIN) ? LuaValSnapshot::makeNil()
                               : LuaValSnapshot::makeNum(sp * 0.01);
    }

    case SUPLA_CHANNELTYPE_ELECTRICITY_METER: {
      auto* em = static_cast<Supla::Sensor::ElectricityMeter*>(el);
      int p = (phase_arg >= 1 && phase_arg <= MAX_PHASES) ? (phase_arg - 1) : 0;

      if (fieldEq(field, "voltage")) {
        return LuaValSnapshot::makeNum(em->getVoltage(p) * 0.01);
      }
      if (fieldEq(field, "current")) {
        return LuaValSnapshot::makeNum(em->getCurrent(p) * 0.001);
      }
      if (fieldEq(field, "freq")) {
        return LuaValSnapshot::makeNum(em->getFreq() * 0.01);
      }
      if (fieldEq(field, "energy") || fieldEq(field, "fwd_energy")) {
        if (phase_arg == 0) {
          uint64_t sum = em->getFwdActEnergy(0) + em->getFwdActEnergy(1) + em->getFwdActEnergy(2);
          return LuaValSnapshot::makeNum(static_cast<double>(sum) * 0.00001);
        }
        return LuaValSnapshot::makeNum(static_cast<double>(em->getFwdActEnergy(p)) * 0.00001);
      }
      if (fieldEq(field, "rvr_energy")) {
        if (phase_arg == 0) {
          uint64_t sum = em->getRvrActEnergy(0) + em->getRvrActEnergy(1) + em->getRvrActEnergy(2);
          return LuaValSnapshot::makeNum(static_cast<double>(sum) * 0.00001);
        }
        return LuaValSnapshot::makeNum(static_cast<double>(em->getRvrActEnergy(p)) * 0.00001);
      }
      // Default / "power": active power in Watts (3-phase sum if phase == 0)
      if (phase_arg == 0) {
        int64_t sum = em->getPowerActive(0) + em->getPowerActive(1) + em->getPowerActive(2);
        return LuaValSnapshot::makeNum(static_cast<double>(sum) * 0.00001);
      }
      return LuaValSnapshot::makeNum(static_cast<double>(em->getPowerActive(p)) * 0.00001);
    }

    default:
      return LuaValSnapshot::makeNil();
  }
}

static LuaValSnapshot readGatePinSnapshot(uint8_t gate_ch, uint8_t pin) {
  Supla::AutoLock lock(SuplaDevice.getTimerAccessMutex());

  Z2S_Core* core = Z2S_Core::getZ2SCoreByChannelNumber(gate_ch);
  if (!core || core->getZ2SLocalChannelType() != LOCAL_CHANNEL_TYPE_ACTION_HANDLER) {
    return LuaValSnapshot::makeNil();
  }
  uint32_t bit = (1UL << pin);
  if ((core->getLogicMask() & bit) == 0) {
    return LuaValSnapshot::makeNil();
  }
  return LuaValSnapshot::makeBool((core->getLogicValue() & bit) != 0);
}

static void pushSnapshot(lua_State* L, const LuaValSnapshot& s) {
  switch (s.kind) {
    case LuaValKind::BOOL:   lua_pushboolean(L, s.u.b ? 1 : 0); break;
    case LuaValKind::INT:    lua_pushinteger(L, s.u.i); break;
    case LuaValKind::NUMBER: lua_pushnumber(L, s.u.f); break;
    default:                 lua_pushnil(L); break;
  }
}

}  // namespace

/*****************************************************************************/
// Lifecycle & Queue Plumbing
/*****************************************************************************/

const char* Z2S_LuaBridge::eventAlias(uint16_t supla_evt) {
  for (const auto& entry : EVENT_ALIASES) {
    if (entry.id == supla_evt) {
      return entry.name;
    }
  }
  return nullptr;
}

void Z2S_LuaBridge::begin(void (*event_sink)(const Z2S_LuaEvent& ev)) {
  _event_sink = event_sink;
  if (!_cmd_queue) {
    _cmd_queue = xQueueCreateStatic(
        CMD_QUEUE_DEPTH, sizeof(Z2S_LuaCmd),
        _cmd_queue_storage, &_cmd_queue_struct);
  }
}

bool Z2S_LuaBridge::enqueueCmd(const Z2S_LuaCmd& cmd) {
  if (!_cmd_queue) return false;
  return xQueueSend(_cmd_queue, &cmd, 0) == pdTRUE;
}

bool Z2S_LuaBridge::enqueueUnwatchAll() {
  Z2S_LuaCmd cmd{};
  cmd.type = Z2S_LuaCmdType::UNWATCH_ALL;
  return enqueueCmd(cmd);
}

void Z2S_LuaBridge::WatchReceiver::handleAction(int event, int action) {
  auto& br = Z2S_LuaBridge::instance();
  if (!br._event_sink) return;
  if (action < 0 || static_cast<size_t>(action) >= MAX_WATCHES) return;

  const auto& slot = br._watches[action];
  if (!slot.active) return;

  Z2S_LuaEvent ev{};
  ev.channel    = slot.channel;
  ev.watch_slot = static_cast<uint8_t>(action);
  ev.event_id   = static_cast<uint16_t>(event);
  br._event_sink(ev);
}

/*****************************************************************************/
// Main-Loop Command Drainer (runs inside loop() right after SuplaDevice.iterate())
/*****************************************************************************/

void Z2S_LuaBridge::drainCommands() {
  if (!_cmd_queue) return;

  Z2S_LuaCmd cmd{};
  uint8_t processed = 0;
  while (processed < 16 && xQueueReceive(_cmd_queue, &cmd, 0) == pdTRUE) {
    ++processed;
    switch (cmd.type) {
      case Z2S_LuaCmdType::ACTION:
        execAction(cmd.channel, cmd.arg_u32);
        break;
      case Z2S_LuaCmdType::SET_PIN:
        execSetPin(cmd.channel, cmd.arg_u8, cmd.sub_op);
        break;
      case Z2S_LuaCmdType::SET_BRIGHTNESS:
        execSetBrightness(cmd.channel, cmd.arg_u8);
        break;
      case Z2S_LuaCmdType::SET_POSITION:
        execSetPosition(cmd.channel, cmd.arg_u8);
        break;
      case Z2S_LuaCmdType::WATCH:
        execWatch(cmd.channel, cmd.arg_u32);
        break;
      case Z2S_LuaCmdType::UNWATCH_ALL:
        execUnwatchAll();
        break;
    }
  }
}

void Z2S_LuaBridge::execAction(uint8_t ch, uint32_t native_action_id) {
  Supla::ActionHandler* handler = getActionHandlerPtr(ch);
  if (!handler) {
    log_w("[LUA] action ignored: no ActionHandler for ch %u", ch);
    return;
  }
  handler->handleAction(0, static_cast<int>(native_action_id));
}

void Z2S_LuaBridge::execSetPin(uint8_t gate_ch, uint8_t pin, uint8_t sub_op) {
  if (pin > 31) return;

  Z2S_Core* core = Z2S_Core::getZ2SCoreByChannelNumber(gate_ch);
  if (!core || core->getZ2SLocalChannelType() != LOCAL_CHANNEL_TYPE_ACTION_HANDLER) {
    log_w("[LUA] set_pin ignored: ch %u is not a Logic Gate", gate_ch);
    return;
  }

  uint32_t base = Z2S_SUPLA_ACTION_CLEAR_LOGIC_GATE_PIN_FIRST_ACTION;
  if (sub_op == 1)      base = Z2S_SUPLA_ACTION_SET_LOGIC_GATE_PIN_FIRST_ACTION;
  else if (sub_op == 2) base = Z2S_SUPLA_ACTION_TOGGLE_LOGIC_GATE_PIN_FIRST_ACTION;

  if (auto* handler = getActionHandlerPtr(gate_ch)) {
    handler->handleAction(0, static_cast<int>(base + pin));
  }
}

void Z2S_LuaBridge::execSetBrightness(uint8_t ch, uint8_t pct) {
  if (pct > 100) pct = 100;

  Supla::Element* el   = Z2S_Core::getSuplaElementByChannelNumber(ch);
  Z2S_Core*       core = Z2S_Core::getZ2SCoreByChannelNumber(ch);
  if (!el || !el->getChannel()) return;

  Supla::Channel* sch = el->getChannel();

  if (core && core->getZ2SLocalChannelType() == LOCAL_CHANNEL_TYPE_GPIO_PWM) {
    static_cast<Supla::Control::LocalPWM*>(el)->setBrightness(pct);
    return;
  }

  if (sch->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {
    if (core && core->getChannelModelId() ==
                    Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH) {
      TSD_SuplaChannelNewValue nv = {};
      nv.ChannelNumber = ch;
      nv.value[0]      = static_cast<char>(pct);
      el->handleNewValueFromServer(&nv);
      return;
    }
    static_cast<Supla::Control::Z2S_DimmerInterface*>(el)->sendValueToDimmer(pct);
    return;
  }

  if (sch->getChannelType() == SUPLA_CHANNELTYPE_DIMMERANDRGBLED) {
    auto* rgbcct = static_cast<Supla::Control::Z2S_RGBCCTInterface*>(el);
    rgbcct->sendValueToDevice(
        sch->getValueRed(), sch->getValueGreen(), sch->getValueBlue(),
        sch->getValueColorBrightness(), pct, sch->getValueWhiteTemperature());
  }
}

void Z2S_LuaBridge::execSetPosition(uint8_t ch, uint8_t pct) {
  if (pct > 100) pct = 100;

  Supla::Element* el = Z2S_Core::getSuplaElementByChannelNumber(ch);
  if (!el || !el->getChannel()) return;

  if (el->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY &&
      el->getChannel()->isRollerShutterRelayType()) {
    static_cast<Supla::Control::RollerShutterInterface*>(el)->setTargetPosition(pct);
  }
}

bool Z2S_LuaBridge::isClientAlive(Supla::ActionHandlerClient* target) {
  if (!target) return false;
  for (auto* p = Supla::LocalAction::getClientListPtr(); p != nullptr; p = p->next) {
    if (p == target) return true;
  }
  return false;
}

void Z2S_LuaBridge::execWatch(uint8_t ch, uint32_t native_event_id) {
  Supla::LocalAction* la = getLocalActionPtr(ch);
  if (!la) {
    log_w("[LUA] watch ignored: ch %u has no LocalAction", ch);
    return;
  }

  uint16_t evt = static_cast<uint16_t>(native_event_id);

  // Auto-start GatewayEvents cyclic timer when watching tick5/30/60/24h
  if (ch == GATEWAY_EVENTS_CHANNEL_NUMBER &&
      (evt == Z2S_SUPLA_EVENT_ON_EVERY_5_SECONDS ||
       evt == Z2S_SUPLA_EVENT_ON_EVERY_30_SECONDS ||
       evt == Z2S_SUPLA_EVENT_ON_EVERY_60_SECONDS ||
       evt == Z2S_SUPLA_EVENT_ON_EVERY_24_HOURS)) {
    if (!GatewayEventsInstance.isCyclicEventEnabled()) {
      GatewayEventsInstance.handleAction(0, Z2S_SUPLA_ACTION_START_TIMER);
      log_i("[LUA] Auto-started GatewayEvents cyclic timer (evt=0x%04X)", evt);
    }
  }

  // 1. Reuse existing slot for identical (ch, evt) across script reloads
  int free_idx = -1;
  for (size_t i = 0; i < MAX_WATCHES; ++i) {
    if (_watches[i].client_ptr && !isClientAlive(_watches[i].client_ptr)) {
      _watches[i] = {};  // Target channel was deleted at runtime
    }
    if (_watches[i].client_ptr &&
        _watches[i].channel == ch &&
        _watches[i].supla_evt == evt) {
      _watches[i].active = true;
      _watches[i].client_ptr->enable();
      return;
    }
    if (free_idx < 0 && !_watches[i].client_ptr) {
      free_idx = static_cast<int>(i);
    }
  }

  // 2. If full, reclaim an inactive slot
  if (free_idx < 0) {
    for (size_t i = 0; i < MAX_WATCHES; ++i) {
      if (!_watches[i].active) {
        if (isClientAlive(_watches[i].client_ptr)) {
          delete _watches[i].client_ptr;
        }
        _watches[i] = {};
        free_idx = static_cast<int>(i);
        break;
      }
    }
  }

  if (free_idx < 0) {
    log_e("[LUA] watch table full (max %u)", static_cast<unsigned>(MAX_WATCHES));
    return;
  }

  la->addAction(free_idx, &_watch_receiver, evt);

  Supla::ActionHandlerClient* found = nullptr;
  for (auto* p = Supla::LocalAction::getClientListPtr(); p != nullptr; p = p->next) {
    if (p->trigger == la && p->client == &_watch_receiver &&
        p->onEvent == evt && p->action == free_idx) {
      found = p;
      break;
    }
  }

  if (found) {
    _watches[free_idx].channel    = ch;
    _watches[free_idx].supla_evt  = evt;
    _watches[free_idx].active     = true;
    _watches[free_idx].client_ptr = found;
  }
}

void Z2S_LuaBridge::execUnwatchAll() {
  for (size_t i = 0; i < MAX_WATCHES; ++i) {
    if (_watches[i].client_ptr) {
      if (isClientAlive(_watches[i].client_ptr)) {
        _watches[i].client_ptr->disable();
      } else {
        _watches[i].client_ptr = nullptr;
      }
    }
    _watches[i].active = false;
  }
}

/*****************************************************************************/
// Lua C Bindings (NO mutex held while calling any lua_* / luaL_* function)
/*****************************************************************************/

int Z2S_LuaBridge::l_get(lua_State* L) {
  lua_Integer ch = luaL_checkinteger(L, 1);
  if (ch < 0 || ch > 254) {
    lua_pushnil(L);
    return 1;
  }

  const char* field = luaL_optstring(L, 2, nullptr);
  lua_Integer phase = luaL_optinteger(L, 3, 0);

  LuaValSnapshot snap = readChannelSnapshot(
      static_cast<uint8_t>(ch), field, static_cast<int32_t>(phase));
  pushSnapshot(L, snap);
  return 1;
}

int Z2S_LuaBridge::l_get_pin(lua_State* L) {
  lua_Integer gate_ch = luaL_checkinteger(L, 1);
  lua_Integer pin     = luaL_checkinteger(L, 2);
  if (gate_ch < 0 || gate_ch > 254 || pin < 0 || pin > 31) {
    lua_pushnil(L);
    return 1;
  }

  LuaValSnapshot snap = readGatePinSnapshot(
      static_cast<uint8_t>(gate_ch), static_cast<uint8_t>(pin));
  pushSnapshot(L, snap);
  return 1;
}

int Z2S_LuaBridge::l_set_pin(lua_State* L) {
  lua_Integer gate_ch = luaL_checkinteger(L, 1);
  lua_Integer pin     = luaL_checkinteger(L, 2);
  luaL_argcheck(L, gate_ch >= 0 && gate_ch <= 254, 1, "channel out of range");
  luaL_argcheck(L, pin >= 0 && pin <= 31, 2, "pin must be 0..31");

  uint8_t sub_op = 2;  // toggle
  if (lua_isboolean(L, 3)) {
    sub_op = lua_toboolean(L, 3) ? 1 : 0;
  } else if (lua_isinteger(L, 3)) {
    sub_op = (lua_tointeger(L, 3) != 0) ? 1 : 0;
  } else if (lua_type(L, 3) == LUA_TSTRING) {
    const char* s = lua_tostring(L, 3);
    if (strcmp(s, "set") == 0 || strcmp(s, "on") == 0)         sub_op = 1;
    else if (strcmp(s, "clear") == 0 || strcmp(s, "off") == 0) sub_op = 0;
    else if (strcmp(s, "toggle") == 0)                         sub_op = 2;
    else return luaL_argerror(L, 3, "expected bool, 0/1, or 'set'/'clear'/'toggle'");
  } else if (!lua_isnoneornil(L, 3)) {
    return luaL_argerror(L, 3, "expected bool, 0/1, or 'toggle'");
  }

  Z2S_LuaCmd cmd{};
  cmd.type    = Z2S_LuaCmdType::SET_PIN;
  cmd.channel = static_cast<uint8_t>(gate_ch);
  cmd.arg_u8  = static_cast<uint8_t>(pin);
  cmd.sub_op  = sub_op;

  lua_pushboolean(L, instance().enqueueCmd(cmd) ? 1 : 0);
  return 1;
}

int Z2S_LuaBridge::l_action(lua_State* L) {
  lua_Integer ch = luaL_checkinteger(L, 1);
  luaL_argcheck(L, ch >= 0 && ch <= 254, 1, "channel out of range");

  uint32_t act_id = 0;
  if (lua_type(L, 2) == LUA_TSTRING) {
    const char* name = lua_tostring(L, 2);
    if (!lookupAlias(ACTION_ALIASES, sizeof(ACTION_ALIASES) / sizeof(ACTION_ALIASES[0]),
                     name, act_id)) {
      return luaL_argerror(L, 2, "unknown action alias");
    }
  } else if (lua_isinteger(L, 2)) {
    act_id = static_cast<uint32_t>(lua_tointeger(L, 2));
    if (!isCustomZ2SActionId(act_id)) {
      return luaL_argerror(
          L, 2, "use string alias for Supla actions; numbers are for Z2S 0x4000+/0x8000+ IDs");
    }
  } else {
    return luaL_argerror(L, 2, "expected action alias string or Z2S custom ID");
  }

  Z2S_LuaCmd cmd{};
  cmd.type    = Z2S_LuaCmdType::ACTION;
  cmd.channel = static_cast<uint8_t>(ch);
  cmd.arg_u32 = act_id;

  lua_pushboolean(L, instance().enqueueCmd(cmd) ? 1 : 0);
  return 1;
}

int Z2S_LuaBridge::l_set_brightness(lua_State* L) {
  lua_Integer ch  = luaL_checkinteger(L, 1);
  lua_Integer pct = luaL_checkinteger(L, 2);
  luaL_argcheck(L, ch >= 0 && ch <= 254, 1, "channel out of range");
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;

  Z2S_LuaCmd cmd{};
  cmd.type    = Z2S_LuaCmdType::SET_BRIGHTNESS;
  cmd.channel = static_cast<uint8_t>(ch);
  cmd.arg_u8  = static_cast<uint8_t>(pct);

  lua_pushboolean(L, instance().enqueueCmd(cmd) ? 1 : 0);
  return 1;
}

int Z2S_LuaBridge::l_set_position(lua_State* L) {
  lua_Integer ch  = luaL_checkinteger(L, 1);
  lua_Integer pct = luaL_checkinteger(L, 2);
  luaL_argcheck(L, ch >= 0 && ch <= 254, 1, "channel out of range");
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;

  Z2S_LuaCmd cmd{};
  cmd.type    = Z2S_LuaCmdType::SET_POSITION;
  cmd.channel = static_cast<uint8_t>(ch);
  cmd.arg_u8  = static_cast<uint8_t>(pct);

  lua_pushboolean(L, instance().enqueueCmd(cmd) ? 1 : 0);
  return 1;
}

int Z2S_LuaBridge::l_watch(lua_State* L) {
  lua_Integer ch = luaL_checkinteger(L, 1);
  luaL_argcheck(L, ch >= 0 && ch <= 254, 1, "channel out of range");

  uint32_t evt_id = static_cast<uint32_t>(Supla::Event::ON_CHANGE);
  if (lua_type(L, 2) == LUA_TSTRING) {
    const char* name = lua_tostring(L, 2);
    if (!lookupAlias(EVENT_ALIASES, sizeof(EVENT_ALIASES) / sizeof(EVENT_ALIASES[0]),
                     name, evt_id)) {
      return luaL_argerror(L, 2, "unknown event alias");
    }
  } else if (lua_isinteger(L, 2)) {
    evt_id = static_cast<uint32_t>(lua_tointeger(L, 2));
    if (!isCustomZ2SEventId(evt_id)) {
      return luaL_argerror(
          L, 2, "use string alias for Supla events; numbers are for Z2S 0x4000+ custom IDs");
    }
  } else if (!lua_isnoneornil(L, 2)) {
    return luaL_argerror(L, 2, "expected event alias string or Z2S custom ID");
  }

  Z2S_LuaCmd cmd{};
  cmd.type    = Z2S_LuaCmdType::WATCH;
  cmd.channel = static_cast<uint8_t>(ch);
  cmd.arg_u32 = evt_id;

  lua_pushboolean(L, instance().enqueueCmd(cmd) ? 1 : 0);
  return 1;
}

int Z2S_LuaBridge::l_uptime_s(lua_State* L) {
  lua_pushinteger(L, static_cast<lua_Integer>(SuplaDevice.uptime.getUptime()));
  return 1;
}

int Z2S_LuaBridge::l_time(lua_State* L) {
  time_t now = time(nullptr);
  if (now < Z2S_LUA_MIN_VALID_TIME) {
    lua_pushnil(L);
    return 1;
  }

  struct tm tm_local{};
  if (localtime_r(&now, &tm_local) == nullptr) {
    lua_pushnil(L);
    return 1;
  }

  // ISO-8601 weekday: 1 = Monday .. 7 = Sunday
  const int iso_wday = (tm_local.tm_wday == 0) ? 7 : tm_local.tm_wday;

  // Fast path: z2s.time("hour") allocates 0 bytes in Lua PSRAM
  if (!lua_isnoneornil(L, 1)) {
    const char* field = luaL_checkstring(L, 1);
    if (fieldEq(field, "hour")) {
      lua_pushinteger(L, tm_local.tm_hour);
    } else if (fieldEq(field, "minute")) {
      lua_pushinteger(L, tm_local.tm_min);
    } else if (fieldEq(field, "second")) {
      lua_pushinteger(L, tm_local.tm_sec);
    } else if (fieldEq(field, "weekday")) {
      lua_pushinteger(L, iso_wday);
    } else if (fieldEq(field, "day")) {
      lua_pushinteger(L, tm_local.tm_mday);
    } else if (fieldEq(field, "month")) {
      lua_pushinteger(L, tm_local.tm_mon + 1);
    } else if (fieldEq(field, "year")) {
      lua_pushinteger(L, tm_local.tm_year + 1900);
    } else if (fieldEq(field, "unix")) {
      lua_pushinteger(L, static_cast<lua_Integer>(now));
    } else {
      lua_pushnil(L);
    }
    return 1;
  }

  lua_createtable(L, 0, 8);
  luaSetIntField(L, "hour",    tm_local.tm_hour);
  luaSetIntField(L, "minute",  tm_local.tm_min);
  luaSetIntField(L, "second",  tm_local.tm_sec);
  luaSetIntField(L, "weekday", iso_wday);
  luaSetIntField(L, "day",     tm_local.tm_mday);
  luaSetIntField(L, "month",   tm_local.tm_mon + 1);
  luaSetIntField(L, "year",    tm_local.tm_year + 1900);
  luaSetIntField(L, "unix",    static_cast<lua_Integer>(now));
  return 1;
}

int Z2S_LuaBridge::l_tick_ms(lua_State* L) {
  lua_pushinteger(L, static_cast<lua_Integer>(static_cast<int32_t>(millis())));
  return 1;
}

int Z2S_LuaBridge::l_elapsed_ms(lua_State* L) {
  uint32_t ref  = static_cast<uint32_t>(static_cast<int32_t>(luaL_checkinteger(L, 1)));
  uint32_t diff = static_cast<uint32_t>(millis()) - ref;
  if (diff > 0x7FFFFFFFUL) diff = 0x7FFFFFFFUL;
  lua_pushinteger(L, static_cast<lua_Integer>(diff));
  return 1;
}

void Z2S_LuaBridge::registerBindings(lua_State* L) {
  static constexpr luaL_Reg funcs[] = {
    {"get",            l_get},
    {"get_pin",        l_get_pin},
    {"set_pin",        l_set_pin},
    {"action",         l_action},
    {"set_brightness", l_set_brightness},
    {"set_position",   l_set_position},
    {"watch",          l_watch},
    {"uptime_s",       l_uptime_s},
    {"time",           l_time},
    {"tick_ms",        l_tick_ms},
    {"elapsed_ms",     l_elapsed_ms},
    {nullptr,          nullptr}
  };
  luaL_newlib(L, funcs);

  lua_pushinteger(L, GATEWAY_EVENTS_CHANNEL_NUMBER);
  lua_setfield(L, -2, "GATEWAY");

  lua_setglobal(L, "z2s");
}

#else
typedef int z2s_lua_bindings_disabled_stub_t;
#endif  // Z2S_ENABLE_LUA