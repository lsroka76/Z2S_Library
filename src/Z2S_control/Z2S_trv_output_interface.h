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

#ifndef SRC_SUPLA_CONTROL_Z2S_TRV_OUTPUT_INTERFACE_H_
#define SRC_SUPLA_CONTROL_Z2S_TRV_OUTPUT_INTERFACE_H_

#include <supla/element.h>
#include <supla/control/output_interface.h>
#include "ZigbeeGateway.h"



namespace Supla {
namespace Control {
class Z2S_TRVOutputInterface : public OutputInterface, public Element {
 public:
  Z2S_TRVOutputInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_mode);

  int getOutputValue() const override;
  void setOutputValue(int value) override;
  bool isOnOffOnly() const override;

  void iterateAlways() override;


protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t _device;
  uint8_t _Tuya_dp_data[10];
  uint8_t _trv_mode = 0;
  int _trv_state = 0;

  uint32_t _refresh_ms      = 60000;
  uint32_t _last_refresh_ms = 0;

  void sendOnOff(bool state);
  //void sendTempPing();
};

};  // namespace Control
};  // namespace Supla


#endif //SRC_SUPLA_CONTROL_Z2S_TRV_OUTPUT_INTERFACE_H_