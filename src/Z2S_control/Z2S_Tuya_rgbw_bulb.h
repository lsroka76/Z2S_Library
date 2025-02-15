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

#ifndef SRC_SUPLA_CONTROL_Z2S_TUYA_RGBW_BULB_H_
#define SRC_SUPLA_CONTROL_Z2S_TUYA_RGBW_BULB_H_

#include "Z2S_rgbw_base.h"
#include "ZigbeeGateway.h"


#define TUYA_RGBW_MODE_DIMMER 0x00
#define TUYA_RGBW_MODE_RGB    0x01

namespace Supla {
namespace Control {
class Z2S_TuyaRGBWBulb: public Z2S_RGBWBase {

public:

  Z2S_TuyaRGBWBulb(ZigbeeGateway *gateway, zbg_device_params_t *device);

  void turnOn() override;
  void turnOff() override;
  void toggle() override;
  bool isOn() override;

  virtual void turnOnDimmer() override;
  virtual void turnOffDimmer() override;

  virtual void turnOnRGB() override;
  virtual void turnOffRGB() override;

  virtual void toggleDimmerRGB() override;

  
  virtual bool isDimmerOn() override;
  virtual bool isRGBOn() override;
  
  void onInit() override;
  void iterateAlways() override;

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;  

  virtual void sendValueToDevice(uint8_t red,
                                 uint8_t green,
                                 uint8_t blue,
                                 uint8_t colorBrightness,
                                 uint8_t brightness) override;

  virtual void setValueOnServer(uint8_t hue, uint8_t saturation, uint16_t level);

protected:

  uint8_t _Tuya_bulb_mode = 0;
  uint8_t _on_off_state = 0;
  bool    _zed_offline = true;
  bool    _toggle_offline = false;

  uint8_t _last_hue;
  uint8_t _last_saturation;
  uint8_t _last_level;

  uint8_t _last_svr_red = 0;
  uint8_t _last_svr_green = 0;
  uint8_t _last_svr_blue = 0;
  uint8_t _last_svr_colorBrightness = 0;
  uint8_t _last_svr_brightness = 0;



  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;

private:
  
  void setTuyaMode(uint8_t mode);
  bool getTuyaMode();

  uint8_t getTuyaBulbState();
  bool setTuyaBulbState(uint8_t Tuya_bulb_state);
};

} //Control
}//Supla

#endif