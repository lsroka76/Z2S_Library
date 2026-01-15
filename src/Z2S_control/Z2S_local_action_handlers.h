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

#ifndef LOCAL_ACTION_HANDLERS_H
#define LOCAL_ACTION_HANDLERS_H

#include <Arduino.h>
//#include <SuplaDevice.h>
//#include <supla/storage/storage.h>
#include <supla/actions.h>
#include <supla/action_handler.h>
#include <supla/element.h>
#include <supla/local_action.h>
#include <supla/events.h>
#include <supla/element_with_channel_actions.h>
#include <supla/control/action_trigger.h>
#include <supla/control/virtual_relay.h>

#include "Z2S_custom_actions_events.h"

#define PIN_LOGIC_OPERATOR_NONE       0x00
#define PIN_LOGIC_OPERATOR_AND        0x01
#define PIN_LOGIC_OPERATOR_OR         0x02
#define PIN_LOGIC_OPERATOR_NOT        0x03
#define PIN_LOGIC_OPERATOR_XOR        0x04
#define PIN_LOGIC_OPERATOR_NAND       0x05
#define PIN_LOGIC_OPERATOR_NOR        0x06
#define PIN_LOGIC_OPERATOR_AND3       0x07
#define PIN_LOGIC_OPERATOR_OR3        0x08

#define LAVB_SINGLE_PRESS_FUNC        0x01
#define LAVB_DOUBLE_PRESS_FUNC        0x02
#define LAVB_HELD_FUNC                0x03
#define LAVB_ROTATE_RIGHT_FUNC        0x04
#define LAVB_ROTATE_LEFT_FUNC         0x05


//typedef void (*_actionhandler_callback)(int event, int action);

namespace Supla {

class LocalActionHandler : public Element, public LocalAction, 
  public ActionHandler {

  public:
    LocalActionHandler();
    virtual ~LocalActionHandler();

};

class LocalActionHandlerWithTrigger : public LocalActionHandler {
  //public Element, public LocalAction, public ActionHandler {
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
    void iterateAlways() override;
    void setPostponedTurnOnSecs(uint32_t _postponed_turn_on_secs) {

      _postponed_turn_on_ms = _postponed_turn_on_secs * 1000;
    }
    uint32_t getPostponedTurnOnSecs() {

      return _postponed_turn_on_ms / 1000;
    }
  protected:
    uint8_t _pin_a = 0;
    uint8_t _pin_b = 0;
    uint8_t _pin_c = 0xFF;

    uint8_t _pin_logic_operator = PIN_LOGIC_OPERATOR_AND;

    uint32_t _pending_postponed_turn_on_ms = 0;
    uint32_t _postponed_turn_on_ms = 0;
};

class LocalActionVirtualButton : public LocalActionHandler {

  public:

    LocalActionVirtualButton();
    virtual ~LocalActionVirtualButton();

    void registerFunction(uint32_t function);
    void unregisterFunction(uint32_t function);
    bool hasFunction(uint32_t function);
    void handleAction(int event, int action);

  private:

  uint32_t  _function_flags = 0;
};

class GatewayEvents: public LocalActionHandler {

  public:

    GatewayEvents() {};
    virtual ~GatewayEvents() {};

    void onInit();
    void handleAction(int event, int action);
    void iterateAlways();

  private:

    uint32_t  cyclic_event_ms = 0;
    uint32_t  cyclic_event_counter = 0;
};

namespace Control {
class LocalActionTrigger: public ActionTrigger, public LocalAction {

  public:

    LocalActionTrigger();
    virtual ~LocalActionTrigger();
    void handleAction(int event, int action) override;

};

class LocalVirtualRelay: public VirtualRelay {

  public:

    LocalVirtualRelay(
      _supla_int_t functions =
       (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

    virtual ~LocalVirtualRelay();
    void handleAction(int event, int action) override;

    
};
}; //namespace Control
};  // namespace Supla

#endif
