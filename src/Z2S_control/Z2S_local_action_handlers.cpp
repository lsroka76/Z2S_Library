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


using Supla::LocalActionHandlerWithTrigger;


LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(uint8_t pin_logic_operator) 
  : _pin_logic_operator(pin_logic_operator) {

    log_i("_pin_logic_operator %u, pin_logic_operator %u", _pin_logic_operator, pin_logic_operator);
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

LocalActionHandlerWithTrigger::~LocalActionHandlerWithTrigger() {};

void LocalActionHandlerWithTrigger::handleAction(int event, int action) {

  //if (actionhandler_callback != nullptr)  actionhandler_callback(event, action);  
  log_i("event %u, action %u", event, action);
  log_i("_pin_a %u, _pin_b %u, _pin_logic_operator %u", 
        _pin_a, _pin_b, _pin_logic_operator);

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

      log_i("TURN_ON _pin_a %u, _pin_b %u, _pin_c %u, _pin_logic_operator %u, pins_changed %s", 
            _pin_a, _pin_b, _pin_c, _pin_logic_operator, pins_changed ? "TRUE" : "FALSE");
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

      log_i("TURN_OFF _pin_a %u, _pin_b %u, _pin_c %u, _pin_logic_operator %u, pins_changed %s", 
            _pin_a, _pin_b, _pin_c, _pin_logic_operator, pins_changed ? "TRUE" : "FALSE");
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

      logic_operation_result = (_pin_a | _pin_b & _pin_c); break;
  }
  
  log_i("logic_operation_result %s", logic_operation_result ? "TRUE" : "FALSE");
  
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

void LocalActionHandlerWithTrigger::iterateAlways() {

  uint32_t millis_ms = millis();

  if ((_pending_postponed_turn_on_ms) && 
      ((millis_ms - _pending_postponed_turn_on_ms) > _postponed_turn_on_ms)) {

        log_i("millis_ms %lu, _postponed_turn_on_ms %lu, _pending_postponed_turn_on_ms %lu", 
              millis_ms, _postponed_turn_on_ms, _pending_postponed_turn_on_ms);

        _pending_postponed_turn_on_ms = 0;
        runAction(ON_TURN_ON);
  }
}