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

//#ifdef Z2S_GATEWAY

#ifndef SRC_SUPLA_CONTROL_Z2S_DIMMER_LIGHT_SOURCE_H_
#define SRC_SUPLA_CONTROL_Z2S_DIMMER_LIGHT_SOURCE_H_

#include <supla/control/dimmer_base.h>
#include "ZigbeeGateway.h"

namespace Supla {
namespace Control {
class Z2S_DimmerLightSource : public DimmerBase {
 public:
  Z2S_DimmerLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device);

  void onInit() override;
  void turnOn() override;
  void turnOff() override;
    
//  bool isOn() override {
  //  return _state;
 // }

  void setStateOnOff(bool state) {
    _state = state;
  }

  //int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;
  //void handleAction(int event, int action) override;
  
  void setRGBWValueOnDevice(uint32_t red,
                            uint32_t green,
                            uint32_t blue,
                            uint32_t colorBrightness,
                            uint32_t brightness) override;
  
  void setRGBWValueOnServer(uint32_t red,
                           uint32_t green,
                           uint32_t blue,
                           uint32_t colorBrightness,
                           uint32_t brightness);

 protected:
  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;
  bool _state;
  uint8_t _write_mask;
  uint8_t _Tuya_mode = 0;
  uint8_t _Tuya_dp_data[20];
  uint8_t hue_start = 1;
};

};  // namespace Control
};  // namespace Supla

#endif  // SRC_SUPLA_CONTROL_Z2S_RGBW_LIGHT_SOURCE_H_
//#endif  // #ifdef Z2S_GATEWAY