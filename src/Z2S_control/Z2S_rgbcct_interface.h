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

#ifndef SRC_SUPLA_CONTROL_Z2S_RGBCCT_INTERFACE_H_
#define SRC_SUPLA_CONTROL_Z2S_RGBCCT_INTERFACE_H_

#include <supla/action_handler.h>
#include <Z2S_custom_actions_events.h>
#include <supla/channel_element.h>
#include "ZigbeeGateway.h"
#include "TuyaDatapoints.h"

#include "Z2S_common.h"

#define Z2S_SEND_TO_LEVEL_DIMMER              0x00
#define Z2S_COLOR_TEMPERATURE_DIMMER          0x01
#define Z2S_TUYA_COLOR_TEMPERATURE_DIMMER     0x02
#define Z2S_TUYA_F0_CMD_DIMMER                0x03 //LEVEL CONTROL 0 - 1000
#define Z2S_TUYA_E0_CMD_DIMMER                0x04 //COLOR CLUSTER 0 - 1000
#define Z2S_PHILIPS_COLOR_TEMPERATURE_DIMMER  0x05
#define Z2S_TUYA_BRIGHTNESS_DP_DIMMER         0x06 //DP 0-1000
#define Z2S_TUYA_COLOR_TEMPERATURE_DP_DIMMER  0x07 //DP 0-1000


#define Z2S_COLOR_HS_RGB                      0x01
#define Z2S_COLOR_HS_XY_RGB                   0x02
#define Z2S_COLOR_XY_RGB                      0x03

#define Z2S_TUYA_COLOR_HS_RGB                 0x11
#define Z2S_TUYA_COLOR_HS_XY_RGB              0x12
#define Z2S_TUYA_COLOR_XY_RGB                 0x13

#define Z2S_PHILIPS_COLOR_HS_RGB              0x21
#define Z2S_PHILIPS_COLOR_HS_XY_RGB           0x22
#define Z2S_PHILIPS_COLOR_XY_RGB              0x23

#define Z2S_TUYA_DP_COLOR_HS_RGB              0x30

#define RGBCCT_STATE_OFF                      0x00
#define RGBCCT_STATE_ON                       0x01
#define RGBCCT_STATE_UNKNOWN                  0xFF

#define RGBCCT_COLOR_MODE_HS                  0x00
#define RGBCCT_COLOR_MODE_XY                  0x01
#define RGBCCT_COLOR_MODE_CT                  0x02
#define RGBCCT_COLOR_MODE_UNKNOWN             0xFF

enum class RGBCCTMessage {

  LEGACY_MSG,
  ON_OFF_STATE_MSG,
  LEVEL_CONTROL_MSG,
  COLOR_TEMPERATURE_MSG,
  HUE_MSG,
  SATURATION_MSG,
  COLOR_MODE_MSG
};


namespace Supla {
namespace Control {
class Z2S_RGBCCTInterface : public ChannelElement, public ActionHandler, 
  public Z2S_Core {

public:

  Z2S_RGBCCTInterface(zbg_device_params_t *device, 
    uint8_t rgb_mode = Z2S_COLOR_HS_RGB);

  void onLoadState() override;
  void onSaveState() override;

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;

  void onInit() override;
  void iterateAlways() override;

  virtual void handleAction(int event, int action) override {};

  virtual void sendValueToDevice(
    uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness, 
    uint8_t brightness, uint8_t whiteTemperature);

  virtual void setStateOnServer(bool state);
  virtual void setDeviceColorMode(uint8_t device_color_mode) {
    
    _device_color_mode = device_color_mode;
  }

  virtual void setValueOnServer(
    uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness,
    uint8_t brightness, uint8_t whiteTemperature);

  void syncDevice();

  void setRGBMode(uint8_t rgb_mode);
  uint8_t getRGBMode();
  
  virtual void ping();

  void setKeepAliveSecs(uint32_t keep_alive_secs);
  void setTimeoutSecs(uint32_t timeout_secs);

  uint32_t getKeepAliveSecs();
  uint32_t getTimeoutSecs();
  

protected:

  uint8_t _rgb_mode = Z2S_COLOR_HS_RGB;

  bool _fresh_start = true;

  uint8_t _device_state = RGBCCT_STATE_UNKNOWN;
  uint8_t _device_color_mode = RGBCCT_COLOR_MODE_UNKNOWN;

  uint32_t _keep_alive_ms = 0;
  uint32_t _timeout_ms    = 0;
  uint32_t _last_ping_ms  = 0;
  uint32_t _last_seen_ms  = 0;
  uint32_t _last_sync_ms  = 0; 

  uint8_t turnOnOff = 0x00;

  uint8_t _last_red = 0;
  uint8_t _last_green = 0;
  uint8_t _last_blue = 0;
  uint8_t _last_colorBrightness = 0;
  uint8_t _last_brightness= 0;
  uint8_t _last_whiteTemperature = 0;
  uint8_t _red = 0;
  uint8_t _green = 255;
  uint8_t _blue = 0;
  uint8_t _colorBrightness = 0;
  uint8_t _brightness = 0;
  uint8_t _whiteTemperature = 0;
  
  uint32_t _lastServerMsgReceivedMs = 0;
  uint32_t _lastDeviceMsgReceivedMs = 0;

}; //Z2S_RGBCCTInterface

} //Control
}//Supla

#endif