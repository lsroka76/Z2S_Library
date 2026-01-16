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

LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(
  uint8_t pin_logic_operator) : _pin_logic_operator(pin_logic_operator) {

    log_i(
      "_pin_logic_operator %u, pin_logic_operator %u", _pin_logic_operator, 
      pin_logic_operator);
    
    switch (_pin_logic_operator) {


      case PIN_LOGIC_OPERATOR_AND3:
      case PIN_LOGIC_OPERATOR_OR3:

        _pin_c = 0;
      break;

      default:

        _pin_c = 0xFF;
      break;
    }
  };

/*****************************************************************************/

LocalActionHandlerWithTrigger::~LocalActionHandlerWithTrigger() {

};

/*****************************************************************************/

void LocalActionHandlerWithTrigger::handleAction(int event, int action) {
  
  log_i("event %u, action %u", event, action);
  log_i(
    "_pin_a %u, _pin_b %u, _pin_logic_operator %u", _pin_a, _pin_b, 
    _pin_logic_operator);

  bool logic_operation_result = false;

  bool pins_changed = true;

  switch (action) {

    case TURN_ON: {
      
      (_pin_a == 0) ? 
        _pin_a = 1 : (_pin_b == 0) ? 
          _pin_b = 1 : (_pin_c == 0) ? 
            _pin_c = 1 : pins_changed = false;

      /*if (_pin_a == 0) {
        
        _pin_a = 1;
        pins_changed = true;
      } else {

        if (_pin_b == 0) {

          _pin_b = 1;
          pins_changed = true;
        }
      }*/

      log_i(
        "TURN_ON _pin_a %u, _pin_b %u, _pin_c %u, _pin_logic_operator %u, "
        "pins_changed %s", _pin_a, _pin_b, _pin_c, _pin_logic_operator, 
        pins_changed ? "TRUE" : "FALSE");
    } break;

    case TURN_OFF: {

      (_pin_a == 1) ? 
        _pin_a = 0 : (_pin_b == 1) ? 
          _pin_b = 0 : (_pin_c == 1) ? 
            _pin_c = 0 : pins_changed = false;
    
      /*if (_pin_a == 1) {

        _pin_a = 0;
        pins_changed = true;
      } else {

        if (_pin_b == 1) {

          _pin_b = 0;
          pins_changed = true;
        }
      }*/

      log_i(
        "TURN_OFF _pin_a %u, _pin_b %u, _pin_c %u, _pin_logic_operator %u, "
        "pins_changed %s", _pin_a, _pin_b, _pin_c, _pin_logic_operator, 
        pins_changed ? "TRUE" : "FALSE");
    } break;
  }
  
  if  (pins_changed == false)
    return;

  switch (_pin_logic_operator) {


    case PIN_LOGIC_OPERATOR_AND:

      logic_operation_result = _pin_a && _pin_b; break;


    case PIN_LOGIC_OPERATOR_OR:

      logic_operation_result = _pin_a || _pin_b; break;


    case PIN_LOGIC_OPERATOR_XOR:

      logic_operation_result = _pin_a ^ _pin_b; break;

    
    case PIN_LOGIC_OPERATOR_NOT:

      logic_operation_result = !_pin_a; break;

    case PIN_LOGIC_OPERATOR_NAND:

      logic_operation_result = !(_pin_a & _pin_b); break;

    case PIN_LOGIC_OPERATOR_NOR:

      logic_operation_result = !(_pin_a | _pin_b); break;

    case PIN_LOGIC_OPERATOR_AND3:

      logic_operation_result = (_pin_a & _pin_b & _pin_c); break;

    case PIN_LOGIC_OPERATOR_OR3:

      logic_operation_result = (_pin_a | _pin_b | _pin_c); break;
  }
  
  log_i(
    "logic_operation_result %s", logic_operation_result ? "TRUE" : "FALSE");
  
  if (logic_operation_result) {

    if (_postponed_turn_on_ms)
      _pending_postponed_turn_on_ms = millis();
    else
      runAction(ON_TURN_ON);
  }
  else {

    if (_pending_postponed_turn_on_ms)
      _pending_postponed_turn_on_ms = 0;
   runAction(ON_TURN_OFF);
  }
  
  log_i("_postponed_turn_on_ms %lu, _pending_postponed_turn_on_ms %lu", 
        _postponed_turn_on_ms, _pending_postponed_turn_on_ms);
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::iterateAlways() {

  uint32_t millis_ms = millis();

  if ((_pending_postponed_turn_on_ms) && 
      ((millis_ms - _pending_postponed_turn_on_ms) > _postponed_turn_on_ms)) {

        log_i(
          "millis_ms %lu, _postponed_turn_on_ms %lu, "
          "_pending_postponed_turn_on_ms %lu", millis_ms, 
          _postponed_turn_on_ms, _pending_postponed_turn_on_ms);

        _pending_postponed_turn_on_ms = 0;
        runAction(ON_TURN_ON);
  }
}

/*****************************************************************************/

LocalActionVirtualButton::LocalActionVirtualButton() {

}

/*****************************************************************************/

LocalActionVirtualButton::~LocalActionVirtualButton() {

}

/*****************************************************************************/

void LocalActionVirtualButton::registerFunction(uint32_t function) {

  _function_flags |= function;
}

/*****************************************************************************/

void LocalActionVirtualButton::unregisterFunction(uint32_t function) {

  _function_flags &= ~function;
}

/*****************************************************************************/

bool LocalActionVirtualButton::hasFunction(uint32_t function) {

  return (_function_flags & function);
}

/*****************************************************************************/

void LocalActionVirtualButton::handleAction(int event, int action) {

}
/*****************************************************************************/

void GatewayEvents::onInit() {

  cyclic_event_ms = millis();
}

/*****************************************************************************/

void GatewayEvents::setActionHandlerCallback(
  _actionhandler_callback actionhandler_callback) {

    this->actionhandler_callback = actionhandler_callback;
}


/*****************************************************************************/

void GatewayEvents::handleAction(int event, int action) {

  log_i(
    "&&&&&&&&&&&&&&&&&&&&&&&&&&"
    "event %u, action %u", event, action);

  if (action) {

    switch (action) {

      case Z2S_SUPLA_ACTION_START_TIMER: {

        cyclic_event_enabled = true;
        cyclic_event_ms = millis();
        cyclic_event_counter = 0;
      } break;


      case Z2S_SUPLA_ACTION_STOP_TIMER:

        cyclic_event_enabled = false;
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


      case Z2S_SUPLA_ACTION_START_GUI_MINIMAL:
      case Z2S_SUPLA_ACTION_START_GUI_STANDARD:

        if (actionhandler_callback != nullptr)  
          actionhandler_callback(event, action);  
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

  if (!cyclic_event_enabled)
    return;

  if (millis_ms - cyclic_event_ms < 5000)
    return;

  cyclic_event_ms = millis_ms;

  cyclic_event_counter = (cyclic_event_counter + 1) % 17280; //24H

  if ((cyclic_event_counter % 1) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_5_SECONDS);

  if ((cyclic_event_counter % 6) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_30_SECONDS);

  if ((cyclic_event_counter % 12) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_60_SECONDS);

  if (cyclic_event_counter == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_24_HOURS);
}


/*****************************************************************************/


Supla::Control::LocalActionTrigger::LocalActionTrigger() {

}

/*****************************************************************************/

Supla::Control::LocalActionTrigger::~LocalActionTrigger() {

}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::handleAction(int event, int action) {

  Supla::Control::ActionTrigger::handleAction(event, action);

  uint32_t local_event = actionTriggerCapToButtonEvent(
    getActionTriggerCap(action));

  log_i("event %u, action %u, local event %u", event, action, local_event);

  //Supla::LocalAction::
  runAction(local_event);
}

/*****************************************************************************/


Supla::Control::LocalVirtualRelay::LocalVirtualRelay(_supla_int_t functions):
 VirtualRelay(functions) {

}

/*****************************************************************************/

Supla::Control::LocalVirtualRelay::~LocalVirtualRelay() {

}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::handleAction(int event, int action) {

  Supla::Control::VirtualRelay::handleAction(event, action);

  log_i("event %u, action %u", event, action);

  switch (action) {

    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:

      if (state) 
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
    break;


    case Z2S_SUPLA_ACTION_SET_RELAY_STATE_DISABLED:

      channel.setStateOffline();
    break;
  }
}