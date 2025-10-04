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
  };

LocalActionHandlerWithTrigger::~LocalActionHandlerWithTrigger() {};

void LocalActionHandlerWithTrigger::handleAction(int event, int action) {

  //if (actionhandler_callback != nullptr)  actionhandler_callback(event, action);  
  log_i("event %u, action %u", event, action);
  log_i("_pin_a %u, _pin_b %u, _pin_logic_operator %u", 
        _pin_a, _pin_b, _pin_logic_operator);

  bool logic_operation_result = false;

  switch (action) {

    case TURN_ON: {
      
      if (_pin_a == 0) {
        _pin_a = 1;
      } else {
      if (_pin_b == 0)
        _pin_b = 1;
      }

      log_i("TURN ON _pin_a %u, _pin_b %u, _pin_logic_operator %u", 
        _pin_a, _pin_b, _pin_logic_operator);

      switch (_pin_logic_operator) {


        case PIN_LOGIC_OPERATOR_AND:

          logic_operation_result = (_pin_a && _pin_b); break;


        case PIN_LOGIC_OPERATOR_OR:

          logic_operation_result = (_pin_a || _pin_b); break;


        case PIN_LOGIC_OPERATOR_XOR:

          logic_operation_result = (_pin_a != _pin_b); break;

        
        case PIN_LOGIC_OPERATOR_NOT:

          runAction(ON_TURN_OFF); break;
      }
      log_i("logic_operation_result %s", logic_operation_result ? "TRUE" : "FALSE");
      if (logic_operation_result)
        runAction(ON_TURN_ON);
    } break;

    case TURN_OFF: {

      if (_pin_a == 1)
        _pin_a = 0;
      else
      if (_pin_b == 1)
        _pin_b = 0;
      
      log_i("TURN OFF _pin_a %u, _pin_b %u, _pin_logic_operator %u", 
        _pin_a, _trigger_value, _pin_logic_operator);


      switch (_pin_logic_operator) {


        case PIN_LOGIC_OPERATOR_AND:

          logic_operation_result = !(_pin_a && _pin_b); break;


        case PIN_LOGIC_OPERATOR_OR:

          logic_operation_result = !(_pin_a || _pin_b); break;


        case PIN_LOGIC_OPERATOR_XOR:

          logic_operation_result = !(_pin_a != _pin_b); break;

        
        case PIN_LOGIC_OPERATOR_NOT:

          runAction(ON_TURN_ON); break;
      }
      log_i("logic_operation_result %s", logic_operation_result ? "TRUE" : "FALSE");
      if (logic_operation_result)
        runAction(ON_TURN_OFF);
    } break;
  } 
}

/*void LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(_actionhandler_callback actionhandler_callback)
{
    this->actionhandler_callback = actionhandler_callback;
}*/
//void iterateAlways();