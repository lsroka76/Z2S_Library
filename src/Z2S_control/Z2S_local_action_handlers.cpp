/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "Z2S_local_action_handlers.h"

#define SSLCLIENT_INSECURE_ONLY
#define SSLCLIENT_HALF_DUPLEX

#include <ESP_SSLClient.h>
#include <WiFiClient.h>
//#include <HTTPClient.h>

ESP_SSLClient ssl_client;
WiFiClient basic_client;

//HTTPClient https;

//WiFiClientSecure clientSec;


extern bool force_leave_global_flag;

extern bool Z2S_sendPushoverMessage(uint16_t message_index);
extern bool Z2S_fillPushoverMessage(
  uint16_t message_index, char *message_buffer);

extern void GUI_onChannelDataChanged(int16_t channel_slot);


//using Supla::LocalActionHandler;
//using Supla::LocalActionHandlerWithTrigger;
using namespace Supla;
/*****************************************************************************/

LocalActionHandler::LocalActionHandler() {

};

/*****************************************************************************/

LocalActionHandler::~LocalActionHandler() {

};

/*****************************************************************************/

/*****************************************************************************/

LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(
  uint8_t pin_logic_operator, uint8_t logic_pins_number)
  : Z2S_Core(this) {

  Z2S_Core::setLogicOperator(pin_logic_operator, false);
  setLogicPinsNumber(logic_pins_number, false);
  resetLogicRuntimeState(true);

  log_i(
    "logic_operator %u, logic_pins_number %u, logic_mask 0x%08lX", 
    getLogicOperator(), getLogicPinsNumber(), getLogicMask());
};

/*****************************************************************************/

LocalActionHandlerWithTrigger::~LocalActionHandlerWithTrigger() {

};

/*****************************************************************************/

void LocalActionHandlerWithTrigger::onInit() {

  if ((getLogicOperator() == PIN_LOGIC_OPERATOR_NONE) &&
      (getZ2SLocalChannelFunc() != PIN_LOGIC_OPERATOR_NONE)) {
    Z2S_Core::setLogicOperator(getZ2SLocalChannelFunc(), false);
  }

  setLogicPinsNumber(getLogicPinsNumber(), false);
  resetLogicRuntimeState(true);
}

/*****************************************************************************/

uint8_t LocalActionHandlerWithTrigger::validateLogicPinsNumber(
  uint8_t logic_operator, uint8_t pins) {

  switch (logic_operator) {


    case PIN_LOGIC_OPERATOR_NONE:
      return 0;


    case PIN_LOGIC_OPERATOR_NOP:
    case PIN_LOGIC_OPERATOR_NOT:
      
      return 1;
    
    case PIN_LOGIC_OPERATOR_JK_LATCH:
    case PIN_LOGIC_OPERATOR_T_LATCH:

      return 2;


    case PIN_LOGIC_OPERATOR_AND3:
    case PIN_LOGIC_OPERATOR_OR3:
      
      return 3;


    case PIN_LOGIC_OPERATOR_XOR:
    case PIN_LOGIC_OPERATOR_AND:
    case PIN_LOGIC_OPERATOR_OR:
    case PIN_LOGIC_OPERATOR_NAND:
    case PIN_LOGIC_OPERATOR_NOR:
    case PIN_LOGIC_OPERATOR_ONE_HOT:
    case PIN_LOGIC_OPERATOR_AT_MOST_ONE:
    case PIN_LOGIC_OPERATOR_INHIBIT:
    case PIN_LOGIC_OPERATOR_MAJORITY:
    case PIN_LOGIC_OPERATOR_SR_LATCH:

      return (pins < 2) ? 2 : ((pins > 32) ? 32 : pins);


    default:
      
      return (pins > 32) ? 32 : pins;
  }
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::updateLogicMask() {

  uint8_t pins = getLogicPinsNumber();

  uint32_t mask = (pins == 0) ? 0UL :
                  (pins >= 32) ? 0xFFFFFFFFUL : ((1UL << pins) - 1UL);

  setLogicMask(mask);
  setLogicValue(getLogicValue() & mask);
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::setLogicOperator(
  uint8_t logic_operator, bool save) {

  uint8_t prev_op   = getLogicOperator();
  uint8_t prev_pins = getLogicPinsNumber();

  Z2S_Core::setLogicOperator(logic_operator, false);

  uint8_t valid_pins = validateLogicPinsNumber(
    logic_operator, prev_pins);

  Z2S_Core::setLogicPinsNumber(valid_pins, false);
  updateLogicMask();

  if ((prev_op != logic_operator) || (prev_pins != valid_pins))
    resetLogicRuntimeState(false);

  if (save)
    saveChannelData();
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::setLogicPinsNumber(
  uint8_t logic_pins_number, bool save) {

  uint8_t prev_pins = getLogicPinsNumber();

  uint8_t valid_pins = validateLogicPinsNumber(
    getLogicOperator(), logic_pins_number);

  Z2S_Core::setLogicPinsNumber(valid_pins, false);
  updateLogicMask();

  if (prev_pins != valid_pins)
    resetLogicRuntimeState(false);

  if (save)
    saveChannelData();
}

/*****************************************************************************/

const char* LocalActionHandlerWithTrigger::getOperatorName() {

  uint8_t op = getLogicOperator();

  if (op < (sizeof(pin_logic_operator_names) / 
      sizeof(pin_logic_operator_names[0]))) {
        
    return pin_logic_operator_names[op];
  }
  return "UNKNOWN";
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::resetLogicRuntimeState(
  bool clear_pin_values) {

  _pending_postponed_turn_on_ms  = 0;
  _pending_postponed_turn_off_ms = 0;

  setLogicPinsChanged(false);
  setLastLogicResult(0xFF);

  if (clear_pin_values)
    setLogicValue(0);
  else
    setLogicValue(getLogicValue() & getLogicMask());
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::handleAction(int event, int action) {
  
  uint32_t mask        = getLogicMask();
  uint32_t val         = getLogicValue() & mask;
  uint8_t  op          = getLogicOperator();
  uint8_t  last_result = getLastLogicResult();

  log_i(
    "event %u, action 0x%04X, logic_value 0x%08lX, logic_mask 0x%08lX, "
    "logic_operator %u, last_result %u", event, action, val, mask, op, 
    last_result);

  if ((mask == 0) || (op == PIN_LOGIC_OPERATOR_NONE))
    return;

  setLogicPinsChanged(false);

  switch (action) {

    case Z2S_SUPLA_ACTION_SET_LOGIC_GATE_PIN_FIRST_ACTION ... 
         Z2S_SUPLA_ACTION_SET_LOGIC_GATE_PIN_LAST_ACTION: {

      uint8_t bit_idx = 
        action - Z2S_SUPLA_ACTION_SET_LOGIC_GATE_PIN_FIRST_ACTION;
      uint32_t pin_bit = (1UL << bit_idx) & mask;

      if (pin_bit != 0) {
        if (((val & pin_bit) == 0) || (last_result == 0xFF)) {
          val |= pin_bit;
          setLogicValue(val);
          setLogicPinsChanged(true);
        }
      }

      log_i(
        "SET_LOGIC_GATE_PIN[%u] logic_value 0x%08lX, logic_mask 0x%08lX, "
        "logic_operator %u, pins_changed %s", bit_idx, val, mask, op, 
        hasLogicPinsChanged() ? "TRUE" : "FALSE");
    } break;

    case Z2S_SUPLA_ACTION_CLEAR_LOGIC_GATE_PIN_FIRST_ACTION ... 
         Z2S_SUPLA_ACTION_CLEAR_LOGIC_GATE_PIN_LAST_ACTION: {

      uint8_t bit_idx = 
        action - Z2S_SUPLA_ACTION_CLEAR_LOGIC_GATE_PIN_FIRST_ACTION;
      uint32_t pin_bit = (1UL << bit_idx) & mask;

      if (pin_bit != 0) {
        if (((val & pin_bit) != 0) || (last_result == 0xFF)) {
          val &= ~pin_bit;
          setLogicValue(val);
          setLogicPinsChanged(true);
        }
      }

      log_i(
        "CLEAR_LOGIC_GATE_PIN[%u] logic_value 0x%08lX, logic_mask 0x%08lX, "
        "logic_operator %u, pins_changed %s", bit_idx, val, mask, op, 
        hasLogicPinsChanged() ? "TRUE" : "FALSE");
    } break;

    case Z2S_SUPLA_ACTION_TOGGLE_LOGIC_GATE_PIN_FIRST_ACTION ... 
         Z2S_SUPLA_ACTION_TOGGLE_LOGIC_GATE_PIN_LAST_ACTION: {

      uint8_t bit_idx = 
        action - Z2S_SUPLA_ACTION_TOGGLE_LOGIC_GATE_PIN_FIRST_ACTION;
      uint32_t pin_bit = (1UL << bit_idx) & mask;

      if (pin_bit != 0) {
        val ^= pin_bit;
        setLogicValue(val);
        setLogicPinsChanged(true);
      }

      log_i(
        "TOGGLE_LOGIC_GATE_PIN[%u] logic_value 0x%08lX, logic_mask 0x%08lX, "
        "logic_operator %u, pins_changed %s", bit_idx, val, mask, op, 
        hasLogicPinsChanged() ? "TRUE" : "FALSE");
    } break;

    case TURN_ON: {
      
      uint32_t zero_bits = (~val) & mask;

      if (zero_bits != 0) {
        uint8_t bit_idx = __builtin_ctz(zero_bits);
        val |= (1UL << bit_idx);
        setLogicValue(val);
        setLogicPinsChanged(true);
      } else if (last_result == 0xFF) {
        setLogicPinsChanged(true);
      }

      log_i(
        "TURN_ON logic_value 0x%08lX, logic_mask 0x%08lX, "
        "logic_operator %u, pins_changed %s", val, mask, op, 
        hasLogicPinsChanged() ? "TRUE" : "FALSE");
    } break;

    case TURN_OFF: {

      uint32_t one_bits = val & mask;

      if (one_bits != 0) {
        uint8_t bit_idx = __builtin_ctz(one_bits);
        val &= ~(1UL << bit_idx);
        setLogicValue(val);
        setLogicPinsChanged(true);
      } else if (last_result == 0xFF) {
        setLogicPinsChanged(true);
      }
     
      log_i(
        "TURN_OFF logic_value 0x%08lX, logic_mask 0x%08lX, "
        "logic_operator %u, pins_changed %s", val, mask, op, 
        hasLogicPinsChanged() ? "TRUE" : "FALSE");
    } break;
  }
  
  if (!hasLogicPinsChanged())
    return;

  uint32_t active = val & mask;
  bool logic_operation_result = false;

  switch (op) {


    case PIN_LOGIC_OPERATOR_NOP:
    case PIN_LOGIC_OPERATOR_OR:
    case PIN_LOGIC_OPERATOR_OR3:

      logic_operation_result = (active != 0); 
    break;


    case PIN_LOGIC_OPERATOR_AND:
    case PIN_LOGIC_OPERATOR_AND3:

      logic_operation_result = (active == mask);
    break;


    case PIN_LOGIC_OPERATOR_XOR:

      logic_operation_result = (__builtin_parity(active) == 1); 
    break;
    

    case PIN_LOGIC_OPERATOR_NOT:
    case PIN_LOGIC_OPERATOR_NOR:

      logic_operation_result = (active == 0); 
    break;


    case PIN_LOGIC_OPERATOR_NAND:

      logic_operation_result = (active != mask); 
    break;


    case PIN_LOGIC_OPERATOR_ONE_HOT:

      logic_operation_result = (active != 0) && ((active & (active - 1)) == 0);
    break;


    case PIN_LOGIC_OPERATOR_AT_MOST_ONE:

      logic_operation_result = ((active & (active - 1)) == 0);
    break;


    case PIN_LOGIC_OPERATOR_INHIBIT: {

      uint32_t trigger_bit = mask & (~mask + 1);
      logic_operation_result = (active == trigger_bit);
    } break;


    case PIN_LOGIC_OPERATOR_MAJORITY:

      logic_operation_result = 
        (__builtin_popcount(active) * 2) > __builtin_popcount(mask);
    break;


    case PIN_LOGIC_OPERATOR_SR_LATCH: {

      uint32_t set_bit    = mask & (~mask + 1);
      uint32_t reset_mask = mask & ~set_bit;

      if ((active & reset_mask) != 0)
        logic_operation_result = false;
      else if ((active & set_bit) != 0)
        logic_operation_result = true;
      else
        logic_operation_result = (last_result == 1);
    } break;


    case PIN_LOGIC_OPERATOR_JK_LATCH: {

      uint32_t set_bit    = mask & (~mask + 1);
      uint32_t reset_mask = mask & ~set_bit;

      bool j = (active & set_bit) != 0;
      bool k = (active & reset_mask) != 0;

      if (j && k)
        logic_operation_result = (last_result != 1);
      else if (k)
        logic_operation_result = false;
      else if (j)
        logic_operation_result = true;
      else
        logic_operation_result = (last_result == 1);
    } break;


    case PIN_LOGIC_OPERATOR_D_LATCH: {
  
      uint32_t set_bit    = mask & (~mask + 1);
      uint32_t reset_mask = mask & ~set_bit;

      bool d = (active & set_bit) != 0;
      bool e = (active & reset_mask) != 0;

      if (e)
        logic_operation_result = d;       // Transparent when enabled
      else
      logic_operation_result = (last_result == 1); // Hold state when disabled
    } break;


    case PIN_LOGIC_OPERATOR_T_LATCH: {
      
      uint32_t set_bit    = mask & (~mask + 1);
      uint32_t reset_mask = mask & ~set_bit;

      bool t = (active & set_bit) != 0;
      bool e = (active & reset_mask) != 0;

      if (e && t)
        logic_operation_result = (last_result != 1); // Toggle when T and E are active
      else
      logic_operation_result = (last_result == 1); // Otherwise hold state
    } break;
  }
  
  log_i(
    "logic_operation_result %s (committed last_result %u)", 
    logic_operation_result ? "TRUE" : "FALSE", last_result);
  
  if (logic_operation_result) {

    _pending_postponed_turn_off_ms = 0;

    if (last_result != 1) {
      if (getPostponedTurnOnMs()) {
        if (_pending_postponed_turn_on_ms == 0) {
          uint32_t now = millis();
          _pending_postponed_turn_on_ms = now ? now : 1;
        }
      } 
      else {
        _pending_postponed_turn_on_ms = 0;
        setLastLogicResult(1);
        runAction(ON_TURN_ON);
      }
    }
  }
  else {

    _pending_postponed_turn_on_ms = 0;

    if (last_result != 0) {
      if (getPostponedTurnOffMs()) {
        if (_pending_postponed_turn_off_ms == 0) {
          uint32_t now = millis();
          _pending_postponed_turn_off_ms = now ? now : 1;
        }
      } else {
        _pending_postponed_turn_off_ms = 0;
        setLastLogicResult(0);
        runAction(ON_TURN_OFF);
      }
    }
  }
  
  log_i(
    "_postponed_turn_on_ms %lu, _pending_postponed_turn_on_ms %lu", 
    getPostponedTurnOnMs(), _pending_postponed_turn_on_ms);

  log_i(
    "_postponed_turn_off_ms %lu, _pending_postponed_turn_off_ms %lu", 
    getPostponedTurnOffMs(), _pending_postponed_turn_off_ms);
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::iterateAlways() {

  uint32_t millis_ms = millis();

  if ((_pending_postponed_turn_on_ms) && 
      ((millis_ms - _pending_postponed_turn_on_ms) > getPostponedTurnOnMs())) {

    log_i(
      "millis_ms %lu, _postponed_turn_on_ms %lu, "
      "_pending_postponed_turn_on_ms %lu", millis_ms, 
      getPostponedTurnOnMs(), _pending_postponed_turn_on_ms);

    _pending_postponed_turn_on_ms = 0;
    setLastLogicResult(1);
    runAction(ON_TURN_ON);
  }

  if ((_pending_postponed_turn_off_ms) && 
      ((millis_ms - _pending_postponed_turn_off_ms) > 
       getPostponedTurnOffMs())) {

    log_i(
      "millis_ms %lu, _postponed_turn_off_ms %lu, "
      "_pending_postponed_turn_off_ms %lu", millis_ms, 
      getPostponedTurnOffMs(), _pending_postponed_turn_off_ms);

    _pending_postponed_turn_off_ms = 0;
    setLastLogicResult(0);
    runAction(ON_TURN_OFF);
  }
}

/*****************************************************************************/

void GatewayEvents::onInit() {

  _cyclic_event_ms = millis();

  ssl_client.setInsecure();
  ssl_client.setBufferSizes(1024 /* rx */, 512 /* tx */);
  ssl_client.setDebugLevel(3);
  ssl_client.setSessionTimeout(120);
  ssl_client.setClient(&basic_client);
  //ssl_client.setTimeout(5000);
}

/*****************************************************************************/

void GatewayEvents::setActionHandlerCallback(
  _actionhandler_callback actionhandler_callback) {

    this->actionhandler_callback = actionhandler_callback;
}

/*****************************************************************************/

void GatewayEvents::sendPushoverMessage(uint32_t action) {

  static const char *pushover_host = "api.pushover.net";
	char pushover_message[1200] = {};
		
  if (Z2S_fillPushoverMessage(
        action - Z2S_SUPLA_ACTION_PUSHOVER_FIRST_ACTION, pushover_message)) {

	  int16_t connection_code = ssl_client.connect(pushover_host, 443);
			
    log_i("connection_code = %d", connection_code);
			
    if (!connection_code) {
        
	    log_e("Connection failed!");
    } 
    else {
        
    ssl_client.print(pushover_message);
  
    log_i("Pushover notification sent!");
	  }
	  ssl_client.stop();
  }
}

/*****************************************************************************/

void GatewayEvents::handleAction(int event, int action) {

  log_i(
    "&&&&&&&&&&&&&&&&&&&&&&&&&& event %u, action %u", event, action);

  if (_disable_actions)
    return;

  if (action) {

    if ((action >= Z2S_SUPLA_ACTION_PUSHOVER_FIRST_ACTION) &&
        (action <= Z2S_SUPLA_ACTION_PUSHOVER_LAST_ACTION))
      _action = action; 

    switch (action) {

      case Z2S_SUPLA_ACTION_START_TIMER: {

        _cyclic_event_enabled = true;
        _cyclic_event_ms = millis();
        _cyclic_event_counter = 0;
      } break;


      case Z2S_SUPLA_ACTION_STOP_TIMER:

        _cyclic_event_enabled = false;
      break;


      case Z2S_SUPLA_ACTION_TURN_ON_LED_WHITE:

        rgbLedWrite(RGB_BUILTIN, 255, 255, 255);
      break;


      case Z2S_SUPLA_ACTION_TURN_OFF_LED:

        rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
      break;


      case Supla::SOFT_RESTART:

        SuplaDevice.scheduleSoftRestart(0);
      break;


      case Supla::ENTER_CONFIG_MODE:

        SuplaDevice.enterConfigMode();
      break;


      case Z2S_SUPLA_ACTION_OPEN_ZIGBEE_NETWORK:

        if (!Zigbee.isNetworkOpen())
          Zigbee.openNetwork(180);
      break;


      case Z2S_SUPLA_ACTION_CLOSE_ZIGBEE_NETWORK:

        if (Zigbee.isNetworkOpen())
          Zigbee.openNetwork(0);
      break;


      case Z2S_SUPLA_ACTION_SET_FORCE_BIND_DEVICE:

        force_leave_global_flag = true;
      break;


      case Z2S_SUPLA_ACTION_CLEAR_FORCE_BIND_DEVICE:

        force_leave_global_flag = false;
      break;


      case Z2S_SUPLA_ACTION_START_GUI_MINIMAL:
      case Z2S_SUPLA_ACTION_START_GUI_STANDARD:

        if (actionhandler_callback != nullptr)  
          actionhandler_callback(event, action);  
      break;


      case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON:

        if (toggleNotifications)
          toggleNotifications->turnOn();
      break;


      case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF:

        if (toggleNotifications)
          toggleNotifications->turnOff();
      break;
    } 
  return;
  }
  
  switch (event) {


    case Supla::ON_DEVICE_STATUS_CHANGE:
    case Z2S_SUPLA_EVENT_ON_SUPLA_INITIALIZED:
    case Z2S_SUPLA_EVENT_ON_SUPLA_REGISTERED_AND_READY:
    case Z2S_SUPLA_EVENT_ON_ZIGBEE_STARTED:
    case Z2S_SUPLA_EVENT_ON_ZIGBEE_OPEN_NETWORK:
    case Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK:
    case Z2S_SUPLA_EVENT_ON_GUI_STARTED:
    case Z2S_SUPLA_EVENT_ON_GUI_NOT_STARTED:
      
      runAction(event);
    break;
  }
}

/*****************************************************************************/

void GatewayEvents::iterateAlways() {
  
  uint32_t millis_ms = millis();

  if ((_action >= 0) && Supla::Network::IsReady()) {
    sendPushoverMessage(_action);
    _action = -1;
  }

  if (!_cyclic_event_enabled)
    return;

  if (millis_ms - _cyclic_event_ms < 5000)
    return;


  if (millis_ms - _cyclic_event_ms >= 10000) {
    
    _cyclic_event_ms = millis_ms;
  } 
  else {
    
    _cyclic_event_ms += 5000;
  }

  _cyclic_event_counter = (_cyclic_event_counter + 1) % 17280; // 24H

  runAction(Z2S_SUPLA_EVENT_ON_EVERY_5_SECONDS);

  if ((_cyclic_event_counter % 6) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_30_SECONDS);

  if ((_cyclic_event_counter % 12) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_60_SECONDS);

  if (_cyclic_event_counter == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_24_HOURS);
}

/*****************************************************************************/


Supla::Control::LocalActionTrigger::LocalActionTrigger() : Z2S_Core(this) {

}


Supla::Control::LocalActionTrigger::LocalActionTrigger(
  int16_t channel_index, z2s_device_params_t *z2s_channel) : Z2S_Core(this) {

    setZ2SChannel(channel_index, z2s_channel);
}
/*****************************************************************************/

Supla::Control::LocalActionTrigger::~LocalActionTrigger() {

}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::setHoldMs(uint32_t hold_ms) {

  _hold_ms = hold_ms;
}

/*****************************************************************************/

uint32_t Supla::Control::LocalActionTrigger::getHoldMs() {

  return _hold_ms;
}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::handleAction(int event, int action) {

  Supla::Control::ActionTrigger::handleAction(event, action);

  uint32_t local_event = actionTriggerCapToButtonEvent(
    getActionTriggerCap(action));

  switch (getActionTriggerCap(action)) {

    case SUPLA_ACTION_CAP_TOGGLE_x1: {
      
      local_event = Supla::ON_LONG_CLICK_1;
    } break;
  

    case SUPLA_ACTION_CAP_TOGGLE_x2: {
      
      local_event = Supla::ON_LONG_CLICK_2;
    } break;

  
    case SUPLA_ACTION_CAP_TOGGLE_x3: {
      
      local_event = Supla::ON_LONG_CLICK_3;
    } break;
    

    case SUPLA_ACTION_CAP_TOGGLE_x4: {
      
      local_event = Supla::ON_LONG_CLICK_4;
    } break;
    

    case SUPLA_ACTION_CAP_TOGGLE_x5: {
      local_event = Supla::ON_LONG_CLICK_5;
    }  break;
  }

  log_i("event %u, action %u, local event %u", event, action, local_event);
  log_i("Supla::ON_LONG_CLICK_1 % u", Supla::ON_LONG_CLICK_1);

  //Supla::LocalAction::
  channel.runAction(local_event);
}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::iterateAlways() {

  if (_hold_ms && ((millis() - _last_hold_ms) > _hold_ms)) {

    log_i("hold repeat");
    _last_hold_ms = millis();
    channel.runAction(Supla::ON_HOLD);
  }

  if (_action >= 0) {
    handleAction(0, _action);
    _action = -1;
  }
}

/*****************************************************************************/

Supla::Control::LocalVirtualRelay::LocalVirtualRelay(_supla_int_t functions)
  : VirtualRelay(functions), Z2S_Core(this) {

}

/*****************************************************************************/

Supla::Control::LocalVirtualRelay::~LocalVirtualRelay() {

}

/*****************************************************************************/

const char *Supla::Control::LocalVirtualRelay::getFunctionName() {

  uint8_t fn = getZ2SLocalChannelFunc();
  constexpr uint8_t count = sizeof(local_virtual_relay_functions_names) /
                            sizeof(local_virtual_relay_functions_names[0]);
  if (fn >= 1 && fn <= count) {
    return local_virtual_relay_functions_names[fn - 1];
  }
  return local_virtual_relay_functions_names[0];
}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::configureRelayFunction() {

  if (getZ2SLocalChannelFunc() == 0) {
    setZ2SLocalChannelFunc(LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_RESTORE, false);
  }

  _astable_running = false;
  _astable_phase_on = false;
  _astable_last_switch_ms = 0;
  _astable_cycles_done = 0;

  switch (getZ2SLocalChannelFunc()) {

    case LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_RESTORE:
      setDefaultStateRestore();
      break;

    case LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_ON:
      setDefaultStateOn();
      break;

    case LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_OFF:
      setDefaultStateOff();
      break;

    case LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY:
    case LOCAL_VIRTUAL_RELAY_FUNC_ASTABLE:
      setDefaultStateOff();
      durationMs = 0;
      state = false;
      channel.setNewValue(false);
      break;

    default:
      setDefaultStateRestore();
      break;
  }
}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::turnOn(_supla_int_t duration) {

  switch (getZ2SLocalChannelFunc()) {

    case LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY: {
      durationMs = 0;
      state = false;
      channel.setNewValue(true);
      channel.setNewValue(false);
      runAction(Supla::ON_TURN_ON);
      runAction(Supla::ON_CHANGE);
    } break;

    case LOCAL_VIRTUAL_RELAY_FUNC_ASTABLE: {
      _astable_running = true;
      _astable_phase_on = true;
      _astable_last_switch_ms = millis();
      _astable_cycles_done = 0;
      Supla::Control::VirtualRelay::turnOn(0);
    } break;

    default: {
      Supla::Control::VirtualRelay::turnOn(duration);
    } break;
  }
}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::turnOff(_supla_int_t duration) {

  switch (getZ2SLocalChannelFunc()) {

    case LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY: {
      durationMs = 0;
      state = false;
      channel.setNewValue(true);
      channel.setNewValue(false);
      runAction(Supla::ON_TURN_OFF);
      runAction(Supla::ON_CHANGE);
    } break;

    case LOCAL_VIRTUAL_RELAY_FUNC_ASTABLE: {
      _astable_running = false;
      _astable_phase_on = false;
      Supla::Control::VirtualRelay::turnOff(0);
    } break;

    default: {
      Supla::Control::VirtualRelay::turnOff(duration);
    } break;
  }
}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::handleAction(int event, int action) {

  Supla::Control::VirtualRelay::handleAction(event, action);

  log_i("event %u, action %u", event, action);

  uint8_t func = getZ2SLocalChannelFunc();

  switch (action) {

    case Z2S_SUPLA_ACTION_TURN_OFF_RELAY_5_SECONDS:
      if (func != LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY &&
          func != LOCAL_VIRTUAL_RELAY_FUNC_ASTABLE) {
        turnOff(5000);
      }
      break;

    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:
      if (func == LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY)
        break;
      if (state) 
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
      break;

    case Z2S_SUPLA_ACTION_COPY_ON_STATE:
      if (func == LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY)
        break;
      if (state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
      break;

    case Z2S_SUPLA_ACTION_COPY_OFF_STATE:
      if (func == LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY)
        break;
      if (!state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
      break;

    case Z2S_SUPLA_ACTION_SET_RELAY_STATE_DISABLED:
      channel.setStateOffline();
      break;
  }
}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::iterateAlways() {

  Supla::Control::VirtualRelay::iterateAlways();

  if ((getZ2SLocalChannelFunc() == LOCAL_VIRTUAL_RELAY_FUNC_ASTABLE) &&
      _astable_running) {

    uint32_t t_on  = getLocalVirtualRelayTOnMs();
    uint32_t t_off = getLocalVirtualRelayTOffMs();
    uint32_t limit = getLocalVirtualRelayCycleLimit();

    if (t_on == 0 || t_off == 0) {
      return;
    }

    uint32_t now = millis();

    if (_astable_phase_on) {
      if ((now - _astable_last_switch_ms) >= t_on) {
        _astable_phase_on = false;
        _astable_last_switch_ms = now;
        state = false;
        channel.setNewValue(false);
        runAction(Supla::ON_TURN_OFF);
        runAction(Supla::ON_CHANGE);

        _astable_cycles_done++;
        if (limit > 0 && _astable_cycles_done >= limit) {
          _astable_running = false;
        }
      }
    } else {
      if ((now - _astable_last_switch_ms) >= t_off) {
        _astable_phase_on = true;
        _astable_last_switch_ms = now;
        state = true;
        channel.setNewValue(true);
        runAction(Supla::ON_TURN_ON);
        runAction(Supla::ON_CHANGE);
      }
    }
  }
}

/*****************************************************************************/
/*****************************************************************************/


Supla::Control::LocalRelay::LocalRelay(
  int pin, bool highIsOn, _supla_int_t functions) 
  : Relay(pin, highIsOn, functions), Z2S_Core(this) {

}

/*****************************************************************************/

Supla::Control::LocalRelay::~LocalRelay() {

}

/*****************************************************************************/

void Supla::Control::LocalRelay::setPin(int pin) {

  outputPin.setPin(pin);
  outputPin.setMode(OUTPUT);
  outputPin.pinMode();
}

/*****************************************************************************/

void Supla::Control::LocalRelay::setHighIsOn(bool highIsOn) {

  outputPin.setActiveHigh(highIsOn);
}

/*****************************************************************************/

int Supla::Control::LocalRelay::getPin() {

  return outputPin.getPin();
}

/*****************************************************************************/

bool Supla::Control::LocalRelay::getHighIsOn() {

  return outputPin.isActiveHigh();
}

/*****************************************************************************/

void Supla::Control::LocalRelay::handleAction(int event, int action) {

  Supla::Control::Relay::handleAction(event, action);

  log_i("event %u, action %u", event, action);

  switch (action) {

    
    case Z2S_SUPLA_ACTION_TURN_OFF_RELAY_5_SECONDS:

      turnOff(5000);
    break;


    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:

      if (isOn()) 
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
    break;


    case Z2S_SUPLA_ACTION_COPY_ON_STATE:

      if (isOn()) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
    break;


    case Z2S_SUPLA_ACTION_COPY_OFF_STATE:

      if (!isOn()) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
    break;


    case Z2S_SUPLA_ACTION_SET_RELAY_STATE_DISABLED:

      channel.setStateOffline();
    break;
  }
}
/*****************************************************************************/

void Supla::Control::LocalRelay::iterateAlways() {

  Supla::Control::Relay::iterateAlways();
}

/*****************************************************************************/
/*****************************************************************************/


Supla::Control::LocalHvac::LocalHvac(int pin, bool highIsOn, int channelNumber) 
  : HvacBase(&_local_hvac_out_pin, nullptr, channelNumber), Z2S_Core(this) {

  _local_hvac_out_pin.getIoPin()->setPin(pin);
  _local_hvac_out_pin.getIoPin()->setMode(OUTPUT);
  _local_hvac_out_pin.getIoPin()->pinMode();
    
  _local_hvac_out_pin.getIoPin()->setActiveHigh(highIsOn);
}

/*****************************************************************************/

Supla::Control::LocalHvac::~LocalHvac() {

}

/*****************************************************************************/

void Supla::Control::LocalHvac::setPin(int pin) {

  _local_hvac_out_pin.getIoPin()->setPin(pin);
  _local_hvac_out_pin.getIoPin()->setMode(OUTPUT);
  _local_hvac_out_pin.getIoPin()->pinMode(channel.getChannelNumber());
}

/*****************************************************************************/

void Supla::Control::LocalHvac::setHighIsOn(bool highIsOn) {

  _local_hvac_out_pin.getIoPin()->setActiveHigh(highIsOn);
}

/*****************************************************************************/

int Supla::Control::LocalHvac::getPin() {

  return _local_hvac_out_pin.getIoPin()->getPin();
}

/*****************************************************************************/

bool Supla::Control::LocalHvac::getHighIsOn() {

  return _local_hvac_out_pin.getIoPin()->isActiveHigh();
}

/*****************************************************************************/

void Supla::Control::LocalHvac::handleAction(int event, int action) {

  Supla::Control::HvacBase::handleAction(event, action);
}
/*****************************************************************************/

void Supla::Control::LocalHvac::iterateAlways() {

  Supla::Control::HvacBase::iterateAlways();
}

/*****************************************************************************/

Supla::Control::LocalPWM::LocalPWM() : Z2S_Core(this) {

  channel.setType(SUPLA_CHANNELTYPE_DIMMER);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER);
}

/*****************************************************************************/

Supla::Control::LocalPWM::~LocalPWM() {

  if (_attached_pin >= 0) {
    ledcDetach(_attached_pin);
  }
}

/*****************************************************************************/

void Supla::Control::LocalPWM::onLoadState() {

  Supla::Storage::ReadState(
    (unsigned char *)&_brightness, sizeof(_brightness));
  Supla::Storage::ReadState(
    (unsigned char *)&_last_brightness, sizeof(_last_brightness));

  if (_brightness > 100)
    _brightness = 0;
  if (_last_brightness == 0 || _last_brightness > 100)
    _last_brightness = 100;
}

/*****************************************************************************/

void Supla::Control::LocalPWM::onSaveState() {

  Supla::Storage::WriteState(
    (unsigned char *)&_brightness, sizeof(_brightness));
  Supla::Storage::WriteState(
    (unsigned char *)&_last_brightness, sizeof(_last_brightness));
}

/*****************************************************************************/

void Supla::Control::LocalPWM::onInit() {

  setupPWM();
  channel.setNewValue(0, 0, 0, 0, _brightness, 0);
}

/*****************************************************************************/

void Supla::Control::LocalPWM::setupPWM() {

  int8_t   pin  = getLocalPwmGpioPin();
  uint32_t freq = getLocalPwmFreqHz();
  uint8_t  res  = getLocalPwmResolutionBits();

  if (freq == 0) freq = 5000;
  if (res < 8 || res > 12) res = 10;

  if (_attached_pin >= 0 &&
      (_attached_pin != pin || _attached_freq != freq || 
       _attached_res != res)) {
    ledcDetach(_attached_pin);
    _attached_pin = -1;
  }

  if (pin >= 0 && _attached_pin < 0) {
    if (ledcAttach(pin, freq, res)) {
      _attached_pin  = pin;
      _attached_freq = freq;
      _attached_res  = res;
    } else {
      log_e("[LocalPWM] ledcAttach failed on pin %d (freq=%lu, res=%u)",
            pin, freq, res);
    }
  }

  applyHardwarePWM();
}

/*****************************************************************************/

void Supla::Control::LocalPWM::applyHardwarePWM() {

  if (_attached_pin < 0)
    return;

  uint32_t max_raw = (1UL << _attached_res) - 1UL;
  uint32_t duty_raw = 0;

  if (_brightness > 0) {
    uint8_t min_d = getLocalPwmMinDuty();
    uint8_t max_d = getLocalPwmMaxDuty();

    if (min_d > 100) min_d = 100;
    if (max_d > 100 || max_d == 0) max_d = 100;
    if (min_d > max_d) min_d = max_d;

    uint32_t min_raw = (max_raw * min_d) / 100UL;
    uint32_t max_limit_raw = (max_raw * max_d) / 100UL;

    if (_brightness >= 100) {
      duty_raw = max_limit_raw;
    } else if (_brightness == 1 && min_d > 0) {
      duty_raw = min_raw;
    } else if (min_d > 0) {
      duty_raw = min_raw +
        ((max_limit_raw - min_raw) * (_brightness - 1UL)) / 99UL;
    } else {
      duty_raw = (max_limit_raw * _brightness) / 100UL;
    }
  }

  if (getLocalPwmInvertLogic()) {
    duty_raw = max_raw - duty_raw;
  }

  ledcWrite(_attached_pin, duty_raw);

  log_i("[LocalPWM] pin=%d, brightness=%u%%, duty_raw=%lu/%lu (invert=%u)",
        _attached_pin, _brightness, duty_raw, max_raw,
        getLocalPwmInvertLogic());
}

/*****************************************************************************/

void Supla::Control::LocalPWM::setBrightness(uint8_t brightness) {

  if (brightness > 100)
    brightness = 100;

  bool was_on = (_brightness > 0);

  if (brightness != _brightness) {
    if (brightness == 0) {
      _last_brightness = _brightness;
      _brightness = 0;
    } else if (_brightness == 0) {
      _brightness = brightness;
    } else {
      _last_brightness = _brightness;
      _brightness = brightness;
    }
  }

  applyHardwarePWM();
  _last_msg_received_ms = millis();
  Supla::Storage::ScheduleSave(5000, 2000);

  bool is_now_on = (_brightness > 0);
  if (!was_on && is_now_on) {
    runAction(Supla::ON_TURN_ON);
  } else if (was_on && !is_now_on) {
    runAction(Supla::ON_TURN_OFF);
  }
  runAction(Supla::ON_CHANGE);
}

/*****************************************************************************/

int32_t Supla::Control::LocalPWM::handleNewValueFromServer(
  TSD_SuplaChannelNewValue *newValue) {

  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  if (brightness > 100)
    brightness = 100;

  if (brightness == 0) {
    setBrightness(0);
  } else if (_brightness == 0 && brightness == 100) {
    uint8_t restore = (_last_brightness > 0 && _last_brightness <= 100)
                        ? _last_brightness : 100;
    setBrightness(restore);
  } else {
    setBrightness(brightness);
  }

  return -1;
}

/*****************************************************************************/

void Supla::Control::LocalPWM::turnOff() {

  if (_brightness > 0) {
    setBrightness(0);
  }
}

/*****************************************************************************/

void Supla::Control::LocalPWM::turnOn() {

  uint8_t restore = (_last_brightness > 0 && _last_brightness <= 100)
                      ? _last_brightness : 100;
  setBrightness(restore);
}

/*****************************************************************************/

void Supla::Control::LocalPWM::toggle() {

  if (isOn())
    turnOff();
  else
    turnOn();
}

/*****************************************************************************/

void Supla::Control::LocalPWM::increaseBrightness(int8_t step) {

  int16_t target = static_cast<int16_t>(_brightness) + step;
  if (target < 0) target = 0;
  if (target > 100) target = 100;

  setBrightness(static_cast<uint8_t>(target));
}

/*****************************************************************************/

void Supla::Control::LocalPWM::handleAction(int event, int action) {

  (void)(event);

  switch (action) {

    case TURN_ON:
      if (!isOn())
        turnOn();
      break;

    case TURN_OFF:
      if (isOn())
        turnOff();
      break;

    case TOGGLE:
      toggle();
      break;

    case DIM_W:
      increaseBrightness(-10);
      break;

    case BRIGHTEN_W:
      increaseBrightness(10);
      break;

    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:
      if (isOn())
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
      break;

    case Z2S_SUPLA_ACTION_COPY_ON_STATE:
      if (isOn())
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
      break;

    case Z2S_SUPLA_ACTION_COPY_OFF_STATE:
      if (!isOn())
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
      break;
  }

  if ((action >= Z2S_SUPLA_ACTION_BRIGHTEN_W_WITH_STEP_FIRST_ACTION) &&
      (action <= Z2S_SUPLA_ACTION_BRIGHTEN_W_WITH_STEP_LAST_ACTION)) {

    int16_t step = action - Z2S_SUPLA_ACTION_BRIGHTEN_W_WITH_STEP_FIRST_ACTION;
    increaseBrightness(step);
  }

  if ((action >= Z2S_SUPLA_ACTION_DIM_W_WITH_STEP_FIRST_ACTION) &&
      (action <= Z2S_SUPLA_ACTION_DIM_W_WITH_STEP_LAST_ACTION)) {

    int16_t step = -1 * (action - 
      Z2S_SUPLA_ACTION_DIM_W_WITH_STEP_FIRST_ACTION);
    increaseBrightness(step);
  }

  /*if ((action >= Z2S_SUPLA_ACTION_SET_PWM_DUTY_PERCENTAGE_FIRST_ACTION) &&
      (action <= Z2S_SUPLA_ACTION_SET_PWM_DUTY_PERCENTAGE_LAST_ACTION)) {

    uint8_t duty =
      action - Z2S_SUPLA_ACTION_SET_PWM_DUTY_PERCENTAGE_FIRST_ACTION;
    if (duty != _brightness) {
      setBrightness(duty);
    }
  }*/
}

/*****************************************************************************/

void Supla::Control::LocalPWM::iterateAlways() {

  if (_last_msg_received_ms != 0 && millis() - _last_msg_received_ms >= 200) {
    _last_msg_received_ms = 0;
    channel.setNewValue(0, 0, 0, 0, _brightness, 0);
  }
}

/*****************************************************************************/

Supla::Control::SwitchBotRelay::SwitchBotRelay(uint8_t device_type_id)
  : Relay(-1, true, 0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER), 
  Z2S_Core(this) {

  _device_type_id = device_type_id;
}

/*****************************************************************************/

void Supla::Control::SwitchBotRelay::onInit() {

  runtimeFlags.initDone = true;
}

/*****************************************************************************/

void Supla::Control::SwitchBotRelay::turnOn(_supla_int_t duration) {
  
  char sb_url[128];
      
  sprintf(sb_url, sb_device_path, _sb_device_id.c_str());
  
  if (ssl_client.connect(sb_host, 443)) {
        
    ssl_client.print(sb_url);
    ssl_client.print("Host: api.switch-bot.com\r\n");
    ssl_client.print("Content-Type: application/json\r\n");
    ssl_client.print("Authorization: ");
    ssl_client.print(_sb_token);
    ssl_client.print("\r\n");
    ssl_client.print("Content-Length: ");
    ssl_client.print(_json_payload.length());
    ssl_client.print("\r\n\r\n");
    ssl_client.print(_json_payload);
  }
  ssl_client.stop();

  if (_device_type_id == SB_DEVICE_TYPE_ON_OFF_ID) {

    _state = true;
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    Supla::Storage::ScheduleSave(5000);
  }
}

/*****************************************************************************/

void Supla::Control::SwitchBotRelay::turnOff(_supla_int_t duration) {

  if (_device_type_id == SB_DEVICE_TYPE_ON_OFF_ID) {

  char sb_url[128];

  sprintf(sb_url, sb_device_path, _sb_device_id.c_str());
  
  if (ssl_client.connect(sb_host, 443)) {
        
    ssl_client.print(sb_url);
    ssl_client.print("Host: api.switch-bot.com\r\n");
    ssl_client.print("Content-Type: application/json\r\n");
    ssl_client.print("Authorization: ");
    ssl_client.print(_sb_token);
    ssl_client.print("\r\n");
    ssl_client.print("Content-Length: ");
    ssl_client.print(_json_payload_2.length());
    ssl_client.print("\r\n\r\n");
    ssl_client.print(_json_payload_2);
  }
  ssl_client.stop();  
    
    _state = false;
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    Supla::Storage::ScheduleSave(5000);
  }
}

/*****************************************************************************/

bool Supla::Control::SwitchBotRelay::updateSwitchBotData(
  channel_extended_data_sb_t &channel_extended_data_sb, uint8_t direction) {

    if (direction == SB_UPDATE_DATA_LOAD_DIR) {

      _sb_device_id = channel_extended_data_sb.ble_mac_address;
      _sb_token = channel_extended_data_sb.token;
      _json_payload = channel_extended_data_sb.json_payload;
      _json_payload_2 = channel_extended_data_sb.json_payload_2;
      return true;
    }
    if (direction == SB_UPDATE_DATA_SAVE_DIR) {

      strncpy(
        channel_extended_data_sb.ble_mac_address, _sb_device_id.c_str(),
        sizeof(channel_extended_data_sb.ble_mac_address));
      strncpy(
        channel_extended_data_sb.token, _sb_token.c_str(),
        sizeof(channel_extended_data_sb.token));
      strncpy(
        channel_extended_data_sb.json_payload, _json_payload.c_str(),
        sizeof(channel_extended_data_sb.json_payload));
      strncpy(
        channel_extended_data_sb.json_payload_2, _json_payload_2.c_str(),
        sizeof(channel_extended_data_sb.json_payload_2));
      return true;
    }
    return false;
  }
/*****************************************************************************/

const char *Supla::Control::SwitchBotRelay::getSwitchBotChannelName(
  uint8_t device_type_id) {

  switch (device_type_id) {


    case SB_DEVICE_TYPE_PRESS_ID:
      return "SWITCH BOT (1X)";


    case SB_DEVICE_TYPE_ON_OFF_ID:
      return "SWITCH BOT (2X)";
  }
}

/*****************************************************************************/

Supla::Sensor::LocalVirtualBinary::LocalVirtualBinary(
  bool keepStateInStorage) 
  : VirtualBinary(keepStateInStorage), Z2S_Core(this) {

};

/*****************************************************************************/

Supla::Sensor::LocalVirtualBinary::~LocalVirtualBinary() {

};

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::handleAction(int event, int action) {

  Supla::Sensor::VirtualBinary::handleAction(event, action);

  /*if (!_notification_registered)
    return;*/
  log_i("event %u, action %u", event, action);

  switch (action) {
    
    
    case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON:

      if (_notification_registered)
        enableNotifications();
    break;


    case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF:

      if (_notification_registered)
        disableNotifications();
    break;
    
    
    case SET:

      if (_auto_clear_ms)
        _last_clear_ms = millis();
    [[fallthrough]];
    case CLEAR:
    case TOGGLE: 
      
      if (_notification_registered)
        sendNotification();
    break;


    case Z2S_SUPLA_ACTION_COPY_ON_STATE:

      if (state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
    break;


    case Z2S_SUPLA_ACTION_COPY_OFF_STATE:

      if (!state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
    break;

  }
}  

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::iterateAlways() {

  Supla::Sensor::VirtualBinary::iterateAlways();

  if (_auto_clear_ms && _last_clear_ms && 
      ((millis() - _last_clear_ms) > _auto_clear_ms)) {

    _last_clear_ms = 0;
    clear();
  }
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::registerNotification() {

  _notification_registered = true;
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::enableNotifications() {

  _notifications_enabled = true;
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::disableNotifications() {

  _notifications_enabled = false;
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::sendNotification() {

  if (!sendIASNotifications)
    return;

  Supla::Notification::SendF(
    channel.getChannelNumber(), "","State changed - now is %s", 
    getValue() ? "ON" : "OFF");
}

/*****************************************************************************/

Supla::Sensor::CalculatedThermometer::CalculatedThermometer() 
  : VirtualThermometerWithActions(), Z2S_Core(this) {
}

/*****************************************************************************/

double calculateDewPoint(double temperature, double humidity) {

  if (humidity <= 0.0) return -273.15; 

    
  const double a = 17.27;
  const double b = 237.3;


  double gamma = (a * temperature) / (b + temperature) + 
    std::log(humidity / 100.0);

  double dewPoint = (b * gamma) / (a - gamma);

  return dewPoint;
}

/*****************************************************************************/

double calculateHeatIndex(double temperatureC, double humidity) {
    
  double T = (temperatureC * 1.8) + 32.0;
    
  double HI = 0.5 * (T + 61.0 + ((T - 68.0) * 1.2) + (humidity * 0.094));

  
  if (HI >= 80.0) {
        
    HI = -42.379 + (2.04901523 * T) + (10.14333127 * humidity) - 
      (0.22475541 * T * humidity) - (0.00683783 * T * T) - 
      (0.05481717 * humidity * humidity) + (0.00122874 * T * T * humidity) + 
      (0.00085282 * T * humidity * humidity) - 
      (0.00000199 * T * T * humidity * humidity);

        
    if (humidity < 13.0 && T >= 80.0 && T <= 112.0) {
    
      double adjustment = ((13.0 - humidity) / 4.0) * 
        std::sqrt((17.0 - std::abs(T - 95.0)) / 17.0);
      HI -= adjustment;
    } 
    else if (humidity > 85.0 && T >= 80.0 && T <= 87.0) {

      double adjustment = ((humidity - 85.0) / 10.0) * ((87.0 - T) / 5.0);
      HI += adjustment;
    }
  }
  return (HI - 32.0) / 1.8;
}

/*****************************************************************************/

void Supla::Sensor::CalculatedThermometer::validateSourceChannel() {

  if ((_last_source_channel < 0xFF) && 
      (_last_source_channel != getSourceChannel()))
    _source_Supla_element = nullptr;

  if (_source_Supla_element == nullptr) {

    switch (getZ2SLocalChannelFunc()) {


      case CALCULATED_THERMOMETER_FUNC_DEW_POINT: 
      case CALCULATED_THERMOMETER_FUNC_HEAT_INDEX: {

        _source_Supla_element = Z2S_Core::getSuplaElementByChannelNumberAndType(
          getSourceChannel(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);

        if (_source_Supla_element) {

          _last_source_channel = getSourceChannel();          
        }
      } break;


      case CALCULATED_THERMOMETER_FUNC_READ_HVAC_TEMP_SETPOINT_HEAT: {

        _source_Supla_element = Z2S_Core::getSuplaElementByChannelNumberAndType(
          getSourceChannel(), SUPLA_CHANNELTYPE_HVAC);

        if (_source_Supla_element) {

          _last_source_channel = getSourceChannel();
        }        
      } break;
    }
  }
}

/*****************************************************************************/

void Supla::Sensor::CalculatedThermometer::iterateAlways() {

  Supla::Sensor::VirtualThermometerWithActions::iterateAlways();

  if (getRefreshMs() && ((millis() - _last_refresh_ms) > getRefreshMs())) {

    _last_refresh_ms = millis();

    validateSourceChannel();

    if (_source_Supla_element) {

      switch (getZ2SLocalChannelFunc()) {


        case CALCULATED_THERMOMETER_FUNC_DEW_POINT: {

          VirtualThermHygroMeter *_source_virtual_therm_hygro_meter = 
            static_cast<Supla::Sensor::VirtualThermHygroMeter *>
              (_source_Supla_element);

          double dew_point = calculateDewPoint(
            _source_virtual_therm_hygro_meter->getChannel()->
              getValueDoubleFirst(),
            _source_virtual_therm_hygro_meter->getChannel()->
              getValueDoubleSecond());

          setValue(dew_point);
        } break;


        case CALCULATED_THERMOMETER_FUNC_HEAT_INDEX: {

          VirtualThermHygroMeter *_source_virtual_therm_hygro_meter = 
            static_cast<Supla::Sensor::VirtualThermHygroMeter *>
              (_source_Supla_element);

          double heat_index = calculateHeatIndex(
            _source_virtual_therm_hygro_meter->getChannel()->
              getValueDoubleFirst(),
            _source_virtual_therm_hygro_meter->getChannel()->
              getValueDoubleSecond());

          setValue(heat_index);
        } break;
      }

      if (checkChannelUserDataFlags(
            USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE)) {

        resendTemperatureHumidityValue(
        RTH_VALUE_TYPE_TEMPERATURE, channel.getValueDouble() * 100);
      }
    }       
  }
}

/*****************************************************************************/

void Supla::Sensor::CalculatedThermometer::readHvacTemperatureSetpointHeat() {

  if (getZ2SLocalChannelFunc() != 
        CALCULATED_THERMOMETER_FUNC_READ_HVAC_TEMP_SETPOINT_HEAT)
    return;

  validateSourceChannel();

  if (_source_Supla_element) {

    Supla::Control::HvacBase *_source_hvac = 
      static_cast<Supla::Control::HvacBase *>(_source_Supla_element);

    float temperature_setpoint_heat = 
      _source_hvac->getTemperatureSetpointHeat();

    log_i("temperature_setpoint_heat %f", temperature_setpoint_heat);

    setValue(temperature_setpoint_heat / 100);

    _last_refresh_ms = 0;
  }
  else
    log_i("_source_Supla_element empty");     
}

/*****************************************************************************/

void Supla::Sensor::CalculatedThermometer::handleAction(
  int event, int action) {

  log_i("event %u, action %u", event, action);
  
  switch (action) {


    case SET:
    
      readHvacTemperatureSetpointHeat();
    break;


    case CLEAR:

      setValue(0);
    break;
  }
}

/*****************************************************************************/

Supla::Sensor::CalculatedGPM::CalculatedGPM() 
  : VirtualGeneralPurposeMeasurement(), Z2S_Core(this) {
}

/*****************************************************************************/

bool Supla::Sensor::CalculatedGPM::isControlAlgorithmFunc() const {

  uint8_t fn = const_cast<CalculatedGPM *>(this)->getZ2SLocalChannelFunc();
  return (fn == CALCULATED_GPM_FUNC_LINEAR_MAP ||
          fn == CALCULATED_GPM_FUNC_PI_CONTROL ||
          fn == CALCULATED_GPM_FUNC_PID_CONTROL);
}

/*****************************************************************************/

const char *Supla::Sensor::CalculatedGPM::getFunctionName() {

  uint8_t fn = getZ2SLocalChannelFunc();
  constexpr uint8_t count = sizeof(calculated_gpm_functions_names) /
                            sizeof(calculated_gpm_functions_names[0]);
  if (fn >= 1 && fn <= count) {
    return calculated_gpm_functions_names[fn - 1];
  }
  return "UNKNOWN";
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::configureFunctionDefaults() {

  switch (getZ2SLocalChannelFunc()) {

    case CALCULATED_GPM_FUNC_VPD:
      setDefaultUnitAfterValue(" kPa");
      setDefaultValuePrecision(2);
      break;

    case CALCULATED_GPM_FUNC_ABSOLUTE_HUMIDITY:
      setDefaultUnitAfterValue(" g/m³");
      setDefaultValuePrecision(2);
      break;

    case CALCULATED_GPM_FUNC_SPECIFIC_ENTHALPY:
      setDefaultUnitAfterValue(" kJ/kg");
      setDefaultValuePrecision(1);
      break;

    case CALCULATED_GPM_FUNC_DEW_POINT_SPREAD:
      setDefaultUnitAfterValue(" K");
      setDefaultValuePrecision(1);
      break;

    case CALCULATED_GPM_FUNC_WOOD_EMC:
    case CALCULATED_GPM_FUNC_LINEAR_MAP:
    case CALCULATED_GPM_FUNC_PI_CONTROL:
    case CALCULATED_GPM_FUNC_PID_CONTROL:
      setDefaultUnitAfterValue(" %");
      setDefaultValuePrecision(1);
      break;
  }
}

/*****************************************************************************/

double calculateVPD(double temperature, double humidity) {

  if ((temperature <= -273.0) || (humidity < 0.0))
    return NAN;

  if (humidity > 100.0)
    humidity = 100.0;

  const double a = 17.27;
  const double b = 237.3;

  // Saturation Vapor Pressure (SVP) in kPa
  double svp = 0.61078 * std::exp((a * temperature) / (b + temperature));

  // Vapor Pressure Deficit (VPD) in kPa
  return svp * (1.0 - (humidity / 100.0));
}

/*****************************************************************************/

double calculateAbsoluteHumidity(double temperature, double humidity) {

  if ((temperature <= -273.0) || (humidity < 0.0))
    return NAN;

  //Magnus-Tetens saturation vapor pressure (hPa) -> mass concentration (g/m^3)
  double es = 6.112 * std::exp((17.67 * temperature) / (temperature + 243.5));
  return (es * humidity * 2.1674) / (273.15 + temperature);
}

/*****************************************************************************/

double calculateSpecificEnthalpy(double temperature, double humidity) {

  if ((temperature <= -273.0) || (humidity < 0.0))
    return NAN;

  const double p_atm = 101.325; // Standard atmospheric pressure in kPa
  double svp = 0.61078 * std::exp((17.27 * temperature) / 
    (237.3 + temperature));
  double pv = svp * (humidity / 100.0);

  if (pv >= p_atm)
    pv = p_atm - 0.01;

  // Humidity ratio (kg water / kg dry air)
  double w = 0.62198 * (pv / (p_atm - pv));

  // Specific enthalpy in kJ/kg
  return (1.006 * temperature) + w * (2501.0 + (1.86 * temperature));
}

/*****************************************************************************/

double calculateDewPointSpread(double temperature, double humidity) {

  if ((temperature <= -273.0) || (humidity <= 0.0))
    return NAN;

  double dp = calculateDewPoint(temperature, humidity);
  return temperature - dp;
}

/*****************************************************************************/

double calculateWoodEMC(double temperature, double humidity) {

  if ((temperature <= -273.0) || (humidity <= 0.0))
    return 0.0;

  double h = (humidity > 100.0) ? 1.0 : (humidity / 100.0);
  double w = 349.0 + (1.29 * temperature) + 
    (0.0135 * temperature * temperature);
  double k = 0.805 + (0.000736 * temperature) - 
    (0.00000273 * temperature * temperature);
  double k1 = 6.27 - (0.00938 * temperature) - 
    (0.000303 * temperature * temperature);
  double k2 = 1.91 + (0.0407 * temperature) - 
    (0.000293 * temperature * temperature);

  double kh = k * h;
  return (1800.0 / w) * (
    (kh / (1.0 - kh)) +
    (((k1 * kh) + (2.0 * k1 * k2 * kh * kh)) /
     (1.0 + (k1 * kh) + (k1 * k2 * kh * kh)))
  );
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::resetControllerState() {

  _integral = 0.0f;
  _prev_temp = NAN;
  _d_filtered = 0.0f;
  _last_pid_calc_ms = 0;
  _last_pwm_duty = 0xFF;
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::setControllerEnabled(bool enabled) {

  if (_controller_enabled == enabled)
    return;

  _controller_enabled = enabled;
  resetControllerState();

  if (!_controller_enabled) {
    commitPwmOutput(0.0);
  } else {
    _last_refresh_ms = 0;
  }
}

/*****************************************************************************/

double Supla::Sensor::CalculatedGPM::computeLinearMap(double temp) {

  float t_min    = getCalculatedGpmParam1();
  float t_max    = getCalculatedGpmParam2();
  float duty_min = constrain(getCalculatedGpmParam3(), 0.0f, 100.0f);
  float duty_max = constrain(getCalculatedGpmParam4(), 0.0f, 100.0f);

  float pv = static_cast<float>(temp);
  if (fabsf(t_max - t_min) < 1e-4f) {
    return (pv >= t_max) ? duty_max : duty_min;
  }

  float ratio = (pv - t_min) / (t_max - t_min);
  ratio = constrain(ratio, 0.0f, 1.0f);

  return static_cast<double>(duty_min + ratio * (duty_max - duty_min));
}

/*****************************************************************************/

double Supla::Sensor::CalculatedGPM::computePID(
    double temp, float dt, bool use_derivative) {

  float setpoint = getCalculatedGpmParam1();
  float kp_raw   = getCalculatedGpmParam2();
  float ki_raw   = getCalculatedGpmParam3();
  float kd_raw   = use_derivative ? getCalculatedGpmParam4() : 0.0f;

  // Kp > 0 => Heating (output rises when temp < setpoint)
  // Kp < 0 => Cooling (output rises when temp > setpoint)
  bool is_cooling = (kp_raw < 0.0f);
  float kp = fabsf(kp_raw);
  float ki = fabsf(ki_raw);
  float kd = fabsf(kd_raw);

  float pv = static_cast<float>(temp);
  float error = is_cooling ? (pv - setpoint) : (setpoint - pv);

  // 1. Proportional term
  float p_term = kp * error;

  // 2. Integral term with dynamic anti-windup clamping to [0..100]
  if (ki > 0.0f && dt > 0.0f) {
    _integral += ki * error * dt;
    float i_min = fminf(0.0f, -p_term);
    float i_max = fmaxf(0.0f, 100.0f - p_term);
    _integral = constrain(_integral, i_min, i_max);
  } else {
    _integral = 0.0f;
  }

  // 3. Derivative-on-Measurement with EMA smoothing (alpha = 0.25)
  float d_term = 0.0f;
  if (use_derivative && kd > 0.0f && !isnan(_prev_temp) && dt > 0.0f) {
    float dpv_dt = (pv - _prev_temp) / dt;
    float raw_d = is_cooling ? (kd * dpv_dt) : (-kd * dpv_dt);
    _d_filtered = (0.25f * raw_d) + (0.75f * _d_filtered);
    d_term = _d_filtered;
  } else {
    _d_filtered = 0.0f;
  }
  _prev_temp = pv;

  float out = constrain(p_term + _integral + d_term, 0.0f, 100.0f);
  return static_cast<double>(out);
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::validateDestinationChannel() {

  uint8_t dst_ch = getDestinationChannel();

  if (dst_ch == 0xFF) {
    _destination_Supla_element = nullptr;
    _last_destination_channel = 0xFF;
    return;
  }

  if ((_last_destination_channel != dst_ch) ||
      (_destination_Supla_element == nullptr)) {

    _destination_Supla_element = nullptr;

    Z2S_Core *dst_core = Z2S_Core::getZ2SCoreByChannelNumber(dst_ch);
    if (dst_core && dst_core->getZ2SElementPtr()) {
      
      _destination_Supla_element = dst_core->getZ2SElementPtr();
      _last_destination_channel = dst_ch;
      _last_pwm_duty = 0xFF; 
    }
  }
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::commitPwmOutput(double out_pct) {

  if (isnan(out_pct))
    return;

  out_pct = constrain(out_pct, 0.0, 100.0);
  setValue(out_pct);

  validateDestinationChannel();

  uint8_t rounded_duty = static_cast<uint8_t>(lround(out_pct));
  
  if (_destination_Supla_element && (rounded_duty != _last_pwm_duty)) {

    _last_pwm_duty = rounded_duty;

    Z2S_Core *dst_core = Z2S_Core::getZ2SCoreByChannelNumber(
      _last_destination_channel);
    
    if (dst_core) {
      
      
      switch (dst_core->getZ2SLocalChannelType()) {
        

        case LOCAL_CHANNEL_TYPE_GPIO_PWM: {
          
          auto *pwm = static_cast<Supla::Control::LocalPWM *>(
            _destination_Supla_element);
          pwm->setPWM(rounded_duty);
        } break;
          
        // Future expansion:
        // case LOCAL_CHANNEL_TYPE_GPIO_RELAY:
        // case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {
        //   if (rounded_duty > 0) _destination_Supla_element->handleAction(0, Supla::TURN_ON);
        //   else _destination_Supla_element->handleAction(0, Supla::TURN_OFF);
        // } break;
      }
    }
  }
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::validateSourceChannel() {

  if (((_last_source_channel < 0xFF) && 
       (_last_source_channel != getSourceChannel())) ||
      ((_last_channel_func < 0xFF) &&
       (_last_channel_func != getZ2SLocalChannelFunc()))) {

    _source_Supla_element = nullptr;
    if (_last_channel_func != getZ2SLocalChannelFunc()) {
      resetControllerState();
    }
  }

  if (_source_Supla_element == nullptr) {

    switch (getZ2SLocalChannelFunc()) {

      case CALCULATED_GPM_FUNC_VPD:
      case CALCULATED_GPM_FUNC_ABSOLUTE_HUMIDITY:
      case CALCULATED_GPM_FUNC_SPECIFIC_ENTHALPY:
      case CALCULATED_GPM_FUNC_DEW_POINT_SPREAD:
      case CALCULATED_GPM_FUNC_WOOD_EMC: {
        
        _source_Supla_element = Z2S_Core::getSuplaElementByChannelNumberAndType(
          getSourceChannel(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);

        if (_source_Supla_element) {
          _last_source_channel = getSourceChannel();
          _last_channel_func = getZ2SLocalChannelFunc();
        }
      } break;

      case CALCULATED_GPM_FUNC_LINEAR_MAP:
      case CALCULATED_GPM_FUNC_PI_CONTROL:
      case CALCULATED_GPM_FUNC_PID_CONTROL: {

        _source_Supla_element = Z2S_Core::getSuplaElementByChannelNumberAndType(
          getSourceChannel(), SUPLA_CHANNELTYPE_THERMOMETER);

        if (!_source_Supla_element) {
          _source_Supla_element =
            Z2S_Core::getSuplaElementByChannelNumberAndType(
              getSourceChannel(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);
        }

        if (_source_Supla_element) {
          _last_source_channel = getSourceChannel();
          _last_channel_func = getZ2SLocalChannelFunc();
        }
      } break;
    }
  }
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::iterateAlways() {

  Supla::Sensor::VirtualGeneralPurposeMeasurement::iterateAlways();

  uint32_t interval_ms = getRefreshMs();
  if (interval_ms == 0 && isControlAlgorithmFunc()) {
    interval_ms = 5000;
  }

  if (interval_ms && ((millis() - _last_refresh_ms) > interval_ms)) {

    uint32_t now_ms = millis();
    float dt = (_last_pid_calc_ms > 0)
                 ? ((now_ms - _last_pid_calc_ms) * 0.001f)
                 : (interval_ms * 0.001f);
    if (dt <= 0.0f || dt > 600.0f) {
      dt = interval_ms * 0.001f;
    }

    _last_refresh_ms = now_ms;
    _last_pid_calc_ms = now_ms;

    validateSourceChannel();

    if (isControlAlgorithmFunc()) {

      if (!_controller_enabled)
        return;

      double temp = NAN;
      bool sensor_valid = false;

      if (_source_Supla_element && _source_Supla_element->getChannel()) {
        auto *src_ch = _source_Supla_element->getChannel();
        if (src_ch->isStateOnline()) {
          if (src_ch->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
            temp = src_ch->getValueDouble();
          } else if (src_ch->getChannelType() ==
                     SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {
            temp = src_ch->getValueDoubleFirst();
          }
          if (!isnan(temp) && (temp > -272.0)) {
            sensor_valid = true;
          }
        }
      }

      if (!sensor_valid) {
        _prev_temp = NAN;
        int8_t failsafe = getCalculatedGpmFailsafeDuty();
        if (failsafe >= 0 && failsafe <= 100) {
          commitPwmOutput(static_cast<double>(failsafe));
        }
        return;
      }

      double out_pct = 0.0;
      switch (getZ2SLocalChannelFunc()) {

        case CALCULATED_GPM_FUNC_LINEAR_MAP:
          out_pct = computeLinearMap(temp);
          break;

        case CALCULATED_GPM_FUNC_PI_CONTROL:
          out_pct = computePID(temp, dt, false);
          break;

        case CALCULATED_GPM_FUNC_PID_CONTROL:
          out_pct = computePID(temp, dt, true);
          break;
      }

      commitPwmOutput(out_pct);
      return;
    }

    if (_source_Supla_element) {

      auto *src_ch = _source_Supla_element->getChannel();

      if (src_ch && src_ch->getChannelType() == 
            SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {

        double temp = src_ch->getValueDoubleFirst();
        double humi = src_ch->getValueDoubleSecond();

        switch (getZ2SLocalChannelFunc()) {

          case CALCULATED_GPM_FUNC_VPD:
            setValue(calculateVPD(temp, humi));
            break;

          case CALCULATED_GPM_FUNC_ABSOLUTE_HUMIDITY:
            setValue(calculateAbsoluteHumidity(temp, humi));
            break;

          case CALCULATED_GPM_FUNC_SPECIFIC_ENTHALPY:
            setValue(calculateSpecificEnthalpy(temp, humi));
            break;

          case CALCULATED_GPM_FUNC_DEW_POINT_SPREAD:
            setValue(calculateDewPointSpread(temp, humi));
            break;

          case CALCULATED_GPM_FUNC_WOOD_EMC:
            setValue(calculateWoodEMC(temp, humi));
            break;
        }
      }
    }       
  }
}

/*****************************************************************************/

void Supla::Sensor::CalculatedGPM::handleAction(int event, int action) {

  log_i("event %u, action %u", event, action);

  uint8_t fn = getZ2SLocalChannelFunc();
  bool is_linear_map = (fn == CALCULATED_GPM_FUNC_LINEAR_MAP);
  bool is_pi_or_pid  = (fn == CALCULATED_GPM_FUNC_PI_CONTROL ||
                        fn == CALCULATED_GPM_FUNC_PID_CONTROL);

  auto adjustParam1 = [&](float delta) {
    setCalculatedGpmParam1(getCalculatedGpmParam1() + delta, true);
    _last_refresh_ms = 0;
    GUI_onChannelDataChanged(getZ2SChannelIndex());
  };

  auto adjustParam2 = [&](float delta) {
    setCalculatedGpmParam2(getCalculatedGpmParam2() + delta, true);
    _last_refresh_ms = 0;
    GUI_onChannelDataChanged(getZ2SChannelIndex());
  };
  
  switch (action) {

    case TURN_ON:
    case SET:
      if (isControlAlgorithmFunc()) {
        setControllerEnabled(true);
      }
      break;

    case TURN_OFF:
    case CLEAR:
      if (isControlAlgorithmFunc()) {
        setControllerEnabled(false);
      } else {
        setValue(0);
      }
      break;

    case TOGGLE:
      if (isControlAlgorithmFunc()) {
        setControllerEnabled(!_controller_enabled);
      }
      break;

    case Z2S_SUPLA_ACTION_INC_TMIN_VALUE_1:
      if (is_linear_map) adjustParam1(0.1f);
      break;

    case Z2S_SUPLA_ACTION_DEC_TMIN_VALUE_1:
      if (is_linear_map) adjustParam1(-0.1f);
      break;

    case Z2S_SUPLA_ACTION_INC_TMIN_VALUE_5:
      if (is_linear_map) adjustParam1(0.5f);
      break;

    case Z2S_SUPLA_ACTION_DEC_TMIN_VALUE_5:
      if (is_linear_map) adjustParam1(-0.5f);
      break;

    case Z2S_SUPLA_ACTION_INC_TMAX_VALUE_1:
      if (is_linear_map) adjustParam2(0.1f);
      break;

    case Z2S_SUPLA_ACTION_DEC_TMAX_VALUE_1:
      if (is_linear_map) adjustParam2(-0.1f);
      break;

    case Z2S_SUPLA_ACTION_INC_TMAX_VALUE_5:
      if (is_linear_map) adjustParam2(0.5f);
      break;

    case Z2S_SUPLA_ACTION_DEC_TMAX_VALUE_5:
      if (is_linear_map) adjustParam2(-0.5f);
      break;

    case Z2S_SUPLA_ACTION_INC_SETPOINT_VALUE_1:
      if (is_pi_or_pid) adjustParam1(0.1f);
      break;

    case Z2S_SUPLA_ACTION_DEC_SETPOINT_VALUE_1:
      if (is_pi_or_pid) adjustParam1(-0.1f);
      break;

    case Z2S_SUPLA_ACTION_INC_SETPOINT_VALUE_5:
      if (is_pi_or_pid) adjustParam1(0.5f);
      break;

    case Z2S_SUPLA_ACTION_DEC_SETPOINT_VALUE_5:
      if (is_pi_or_pid) adjustParam1(-0.5f);
      break;
  }
}

/*****************************************************************************/

Supla::Sensor::LocalVirtualThermHygroMeter::LocalVirtualThermHygroMeter() 
  : VirtualThermHygroMeter(), Z2S_Core(this) {
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualThermHygroMeter::iterateAlways() {

  Supla::Sensor::VirtualThermHygroMeter::iterateAlways();

  if (getRefreshMs() && ((millis() - _last_refresh_ms) > getRefreshMs())) {

    _last_refresh_ms = millis();

    if ((_last_source_channel < 0xFF) && 
        (_last_source_channel != getSourceChannel()))
      _source_virtual_therm_hygro_meter = nullptr;

    if (_source_virtual_therm_hygro_meter == nullptr) {

      Supla::Element *element = getSuplaElementByChannelNumberAndType(
        getSourceChannel(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);

      if (element) {
        _source_virtual_therm_hygro_meter = static_cast<
          Supla::Sensor::VirtualThermHygroMeter *>(element);
      
        _last_source_channel = getSourceChannel();
      }
    }

    if (_source_virtual_therm_hygro_meter) {

      setTemp(
        _source_virtual_therm_hygro_meter->getChannel()->getValueDoubleFirst(
          ));
      setHumi(
        _source_virtual_therm_hygro_meter->getChannel()->getValueDoubleSecond(
          ));
      

      if (checkChannelUserDataFlags(
            USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE)) {

        resendTemperatureHumidityValue(
          RTH_VALUE_TYPE_TEMPERATURE, channel.getValueDoubleFirst() * 100);

        resendTemperatureHumidityValue(
          RTH_VALUE_TYPE_HUMIDITY, channel.getValueDoubleSecond() * 100);
      }
    }       
  }
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualGPM::handleAction(int event, int action) {

  log_i("event %u, action %u", event, action);
  
  switch (action) {


    case TURN_ON:
    case SET:

      _value = 1000;
    break;


    case TURN_OFF:
    case CLEAR:

      _value = 0;
    break;
  }
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualGPM::iterateAlways() {
    
  VirtualGeneralPurposeMeasurement::iterateAlways();
}

/*****************************************************************************/

Supla::Sensor::LocalDS18B20::LocalDS18B20(
  uint8_t gpio_pin, uint8_t *sensor_address) 
  : DS18B20(gpio_pin, sensor_address), Z2S_Core(this) {

} 

/*****************************************************************************/
/*****************************************************************************/

Supla::Sensor::LocalBinary::LocalBinary() 
  : Binary(-1, false, false), Z2S_Core(this) {

}

/*****************************************************************************/

void Supla::Sensor::LocalBinary::setPin(int pin) {

  inputPin.setPin(pin);
  inputPin.setMode(INPUT);
  inputPin.pinMode();
}

/*****************************************************************************/

void Supla::Sensor::LocalBinary::setInvertLogic(bool invertLogic) {

  inputPin.setActiveHigh(!invertLogic);
}
void Supla::Sensor::LocalBinary::setPullUp(bool pullUp) {

  inputPin.setPullUp(pullUp);
}

/*****************************************************************************/

int Supla::Sensor::LocalBinary::getPin() {

  return inputPin.getPin();
}

/*****************************************************************************/

bool Supla::Sensor::LocalBinary::getInvertLogic() {
  
  return !inputPin.isActiveHigh();
}

/*****************************************************************************/

bool Supla::Sensor::LocalBinary::getPullUp() {

  return inputPin.isPullUp();
}

/*****************************************************************************/

/*****************************************************************************/

Supla::Sensor::HttpBinary::HttpBinary(bool keepStateInStorage)
  : VirtualBinary(keepStateInStorage), Z2S_Core(this) {
}

/*****************************************************************************/

Supla::Sensor::HttpBinary::~HttpBinary() {
}

/*****************************************************************************/

bool Supla::Sensor::HttpBinary::sendHttpTrigger() {
  
  if (!Supla::Network::IsReady()) {
    return false;
  }

  channel_extended_data_http_binary_t hb_data = {};

  if (!loadChannelExtendedData(
        CHANNEL_EXTENDED_DATA_TYPE_HTTP_BINARY,(uint8_t *)&hb_data) || 
      hb_data.url[0] == '\0') {

    log_e(
      "[HttpBinary] Missing or empty extended data URL for slot %d",
      getZ2SChannelIndex());
    
    return false;
  }

  const char *hostStart = hb_data.url;
  if (strncmp(hb_data.url, "http://", 7) == 0) {
    hostStart += 7;
  } else if (strncmp(hb_data.url, "https://", 8) == 0) {
    hostStart += 8;
  }

  const char *pathStart = strchr(hostStart, '/');
  if (pathStart == nullptr) {
    log_e("[HttpBinary] Invalid URL (missing path): %s", hb_data.url);
    return false;
  }

  char host[64] = {0};
  uint16_t port = 80;

  const char *colonPos = strchr(hostStart, ':');
  if (colonPos != nullptr && colonPos < pathStart) {
    size_t hostLen = colonPos - hostStart;
    if (hostLen >= sizeof(host)) return false;
    memcpy(host, hostStart, hostLen);
    host[hostLen] = '\0';
    port = atoi(colonPos + 1);
  } else {
    size_t hostLen = pathStart - hostStart;
    if (hostLen >= sizeof(host)) return false;
    memcpy(host, hostStart, hostLen);
    host[hostLen] = '\0';
  }

  if (!basic_client.connect(host, port, 1000)) {
    
    log_e("[HttpBinary] Connection failed to %s:%u", host, port);
    
    return false;
  }

  basic_client.printf(
    "GET %s HTTP/1.1\r\nHost: %s:%u\r\nConnection: close\r\n\r\n",
    pathStart, host, port);

  basic_client.flush();
  delay(20);
  basic_client.stop();

  log_i("[HttpBinary] Trigger sent to %s:%u", host, port);
  return true;
}

/*****************************************************************************/

void Supla::Sensor::HttpBinary::handleAction(int event, int action) {
  
  bool prevState = state;
  Supla::Sensor::VirtualBinary::handleAction(event, action);

  switch (action) {
    case SET:
    case TURN_ON:
    case TOGGLE:
      if (state && !prevState) {
        sendHttpTrigger();
        _last_http_send_ms = millis();
      }
      break;

    case CLEAR:
    case TURN_OFF:
      _last_http_send_ms = 0;
      break;

    case Z2S_SUPLA_ACTION_COPY_ON_STATE:
      if (state)
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
      break;

    case Z2S_SUPLA_ACTION_COPY_OFF_STATE:
      if (!state)
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
      break;
  }
}

/*****************************************************************************/

void Supla::Sensor::HttpBinary::iterateAlways() {

  Supla::Sensor::VirtualBinary::iterateAlways();

  if (state) {
    uint32_t interval_ms = getRefreshMs();
    if (interval_ms > 0 && (millis() - _last_http_send_ms >= interval_ms)) {
      
      _last_http_send_ms = millis();
      sendHttpTrigger();
    }
  }
}

/*****************************************************************************/

Supla::GatewayEvents GatewayEventsInstance;