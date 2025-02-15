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

#ifndef SRC_SUPLA_CONTROL_Z2S_RGBW_BASE_H_
#define SRC_SUPLA_CONTROL_Z2S_RGBW_BASE_H_

#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>


namespace Supla {
namespace Control {
class Z2S_RGBWBase : public ChannelElement, public ActionHandler {

public:

  Z2S_RGBWBase();

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;
  virtual void turnOn() = 0;
  virtual void turnOff() = 0;
  virtual void toggle() = 0;
  virtual bool isOn() = 0;

  virtual void turnOnDimmer() = 0;
  virtual void turnOffDimmer() = 0;

  virtual void turnOnRGB() = 0;
  virtual void turnOffRGB() = 0;

  virtual void toggleDimmerRGB() = 0;

  
  virtual bool isDimmerOn() = 0;
  virtual bool isRGBOn() = 0;

  
  //void onInit() override;
  void iterateAlways() override;

  virtual void handleAction(int event, int action) override {};

  virtual void sendValueToDevice(uint8_t red,
                                 uint8_t green,
                                 uint8_t blue,
                                 uint8_t colorBrightness,
				 uint8_t brightness) = 0;

  virtual void setValueOnServer(uint8_t red,
                                uint8_t green,
                                uint8_t blue,
                                uint8_t colorBrightness,
				uint8_t brightness);

  virtual void setStateOnServer(bool state);


protected:

  bool    _state;
  bool    _toggle = false;

  uint8_t _last_red = 0;
  uint8_t _last_green = 0;
  uint8_t _last_blue = 0;
  uint8_t _last_colorBrightness = 0;
  uint8_t _last_brightness = 0;

  uint32_t _lastMsgReceivedMs = 0;

}; //Z2S_RGBBase

} //Control
}//Supla

#endif