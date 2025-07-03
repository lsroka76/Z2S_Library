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

#ifndef SRC_SUPLA_CONTROL_Z2S_DIMMER_INTERFACE_H_
#define SRC_SUPLA_CONTROL_Z2S_DIMMER_INTERFACE_H_

#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>
#include "ZigbeeGateway.h"

#define Z2S_SEND_TO_LEVEL_DIMMER          0x00
#define Z2S_COLOR_TEMPERATURE_DIMMER      0x01
#define Z2S_TUYA_COLOR_TEMPERATURE_DIMMER 0x02
#define Z2S_TUYA_F0_CMD_DIMMER            0x03 //LEVEL CONTROL 0 - 1000
#define Z2S_TUYA_E0_CMD_DIMMER            0x04 //COLOR CLUSTER 0 - 1000

namespace Supla {
namespace Control {
class Z2S_DimmerInterface : public ChannelElement, public ActionHandler {

public:

  Z2S_DimmerInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t dimmer_mode = Z2S_SEND_TO_LEVEL_DIMMER);

  void setRGBChannel(uint8_t rgb_channel) {
    _rgb_channel = rgb_channel;
    if (_rgb_channel < 0xFF) {
      _rgb = Supla::Element::getElementByChannelNumber(_rgb_channel);
    }
    else _rgb = nullptr;
  }

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;
  
  //void onInit() override;
  void iterateAlways() override;

  virtual void handleAction(int event, int action) override;

  virtual void sendValueToDevice(uint8_t brightness); //= 0;

  virtual void setValueOnServer(uint16_t value);

  //virtual void setStateOnServer(bool state);

  virtual void ping();

  void setDimmerMode(uint8_t dimmer_mode);
  uint8_t getDimmerMode();

  void setKeepAliveSecs(uint32_t keep_alive_secs);
  void setTimeoutSecs(uint32_t timeout_secs);

  uint32_t getKeepAliveSecs();
  uint32_t getTimeoutSecs();


protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;
  uint8_t _dimmer_mode = Z2S_SEND_TO_LEVEL_DIMMER;

  bool _fresh_start = true;

  bool _keep_alive_enabled = true;
  bool _timeout_enabled    = true;

  uint32_t _keep_alive_ms = 45000;
  uint32_t _timeout_ms    = 60000;
  uint32_t _last_ping_ms  = 0;
  uint32_t _last_seen_ms  = 0;

  uint8_t _rgb_channel = 0xFF;
  bool _turn_rgb_off = false;
  Element *_rgb = nullptr;

  uint8_t _last_brightness = 0;
  uint8_t _brightness = 0;

  uint32_t _lastMsgReceivedMs = 0;

  void increaseBrightness(int8_t add_to_brightness);

}; //Z2S_DimmerInterface

} //Control
}//Supla

#endif
