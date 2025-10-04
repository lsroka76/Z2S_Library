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

#ifndef LOCAL_ACTION_HANDLER_WITH_TRIGGER_H
#define LOCAL_ACTION_HANDLER_WITH_TRIGGER_H

#include <Arduino.h>
//#include <SuplaDevice.h>
//#include <supla/storage/storage.h>
#include <supla/actions.h>
#include <supla/action_handler.h>
#include <supla/element.h>
#include <supla/local_action.h>
#include <supla/events.h>
#include <supla/element_with_channel_actions.h>

#define PIN_LOGIC_OPERATOR_NONE 0x00
#define PIN_LOGIC_OPERATOR_AND  0x01
#define PIN_LOGIC_OPERATOR_OR   0x02
#define PIN_LOGIC_OPERATOR_NOT  0x03
#define PIN_LOGIC_OPERATOR_XOR  0x04

//typedef void (*_actionhandler_callback)(int event, int action);

namespace Supla {

class LocalActionHandlerWithTrigger : public Element, public LocalAction, public ActionHandler {
  public:
    LocalActionHandlerWithTrigger(uint8_t pin_logic_operator);
    virtual ~LocalActionHandlerWithTrigger();
    void handleAction(int event, int action);
    /*void addAction(uint16_t action,
                   ActionHandler &client,   // NOLINT(runtime/references)
                   uint16_t event,
                   bool alwaysEnabled = false) override;
    void addAction(uint16_t action, 
                   ActionHandler *client, 
                   uint16_t event,
                   bool alwaysEnabled = false) override;*/
    //void  setActionHandlerCallback(_actionhandler_callback actionhandler_callback);
    //void iterateAlways();
  protected:
    //_actionhandler_callback actionhandler_callback = nullptr;
    uint8_t _trigger_value = 0;
    uint8_t _trigger_counter = 0;

    uint8_t _pin_a = 0;
    uint8_t _pin_b = 0;
    uint8_t _pin_logic_operator = PIN_LOGIC_OPERATOR_AND;
};

};  // namespace Supla

#endif
