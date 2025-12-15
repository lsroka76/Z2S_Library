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

#ifndef SRC_SUPLA_CONTROL_Z2S_RGB_INTERFACE_H_
#define SRC_SUPLA_CONTROL_Z2S_RGB_INTERFACE_H_

#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>
#include "ZigbeeGateway.h"
#include "TuyaDatapoints.h"

#define Z2S_COLOR_HS_RGB            0x01
#define Z2S_COLOR_HS_XY_RGB         0x02
#define Z2S_COLOR_XY_RGB            0x03

#define Z2S_TUYA_COLOR_HS_RGB       0x11
#define Z2S_TUYA_COLOR_HS_XY_RGB    0x12
#define Z2S_TUYA_COLOR_XY_RGB       0x13

#define Z2S_PHILIPS_COLOR_HS_RGB    0x21
#define Z2S_PHILIPS_COLOR_HS_XY_RGB 0x22
#define Z2S_PHILIPS_COLOR_XY_RGB    0x23

#define Z2S_TUYA_DP_COLOR_HS_RGB    0x30


namespace Supla {
namespace Control {
class Z2S_RGBInterface : public ChannelElement, public ActionHandler {

public:

  Z2S_RGBInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t rgb_mode = Z2S_COLOR_HS_RGB);

  void setDimmerChannel(uint8_t dimmer_channel) {
    /*_dimmer_channel = dimmer_channel;
    if (_dimmer_channel < 0xFF) {
      _dimmer = Supla::Element::getElementByChannelNumber(_dimmer_channel);
    }
    else _dimmer = nullptr;*/
  }

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;

  //void onInit() override;
  void iterateAlways() override;

  virtual void handleAction(int event, int action) override {};

  virtual void sendValueToDevice(
    uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness);

  virtual void setValueOnServer(
    uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness);

  void setRGBMode(uint8_t rgb_mode);
  uint8_t getRGBMode();
  
  virtual void ping();

  void setKeepAliveSecs(uint32_t keep_alive_secs);
  void setTimeoutSecs(uint32_t timeout_secs);

  uint32_t getKeepAliveSecs();
  uint32_t getTimeoutSecs();
  

protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;
  uint8_t _rgb_mode = Z2S_COLOR_HS_RGB;

  bool _fresh_start = true;

  bool _keep_alive_enabled = true;
  bool _timeout_enabled    = true;

  uint32_t _keep_alive_ms = 45000;
  uint32_t _timeout_ms    = 60000;
  uint32_t _last_ping_ms  = 0;
  uint32_t _last_seen_ms  = 0;

  //uint8_t _dimmer_channel = 0xFF;
  uint8_t turnOnOff = 0xFF;
  bool _turn_dimmer_off = false;
  Element *_dimmer = nullptr;

  uint8_t _last_red = 0;
  uint8_t _last_green = 0;
  uint8_t _last_blue = 0;
  uint8_t _last_colorBrightness = 0;
  uint8_t _red = 0;
  uint8_t _green = 255;
  uint8_t _blue = 0;
  uint8_t _colorBrightness = 0;
  

  uint32_t _lastMsgReceivedMs = 0;

}; //Z2S_RGBInterface

} //Control
}//Supla

#endif