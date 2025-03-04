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

#ifndef SRC_SUPLA_CONTROL_Z2S_DIMMER_BASE_H_
#define SRC_SUPLA_CONTROL_Z2S_DIMMER_BASE_H_

#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>


namespace Supla {
namespace Control {
class Z2S_DimmerBase : public ChannelElement, public ActionHandler {

public:

  Z2S_DimmerBase();

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;
  virtual void turnOn() = 0;
  virtual void turnOff() = 0;;
  virtual void toggle() = 0;
  virtual bool isOn() = 0;

  virtual bool ping() = 0;
  
  //void onInit() override;
  void iterateAlways() override;

  virtual void sendValueToDevice(uint32_t brightness) = 0;
  virtual void setValueOnServer(uint32_t brightness);
  virtual void setStateOnServer(bool state);
  virtual void handleAction(int event, int action) override;

protected:

  uint8_t _current_brightness = 0;            // 0 - 100
  uint8_t _last_brightness = 100;

  uint32_t _last_channel_update_ms = 0;
  uint32_t _last_ping_ms = 0;

}; //Z2S_DimmerBase

} //Control
}//Supla

#endif