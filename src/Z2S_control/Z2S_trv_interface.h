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

#ifndef SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_
#define SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_

#include <supla/element.h>
#include <supla/actions.h>
#include <supla/action_handler.h>
#include <supla/control/output_interface.h>
#include "ZigbeeGateway.h"
#include "hvac_base_ee.h"


namespace Supla {
namespace Control {
class Z2S_TRVInterface : public OutputInterface, public ActionHandler, public Element {
 public:
  Z2S_TRVInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_commands_set, uint8_t trv_thermometer_channel_no);

  Supla::Control::HvacBaseEE *getTRVHvac();
  void setTRVHvac(Supla::Control::HvacBaseEE *trv_hvac);

  int getOutputValue() const override;
  void setOutputValue(int value) override;
  bool isOnOffOnly() const override;
  bool isControlledInternally() const override { return false; }

  void iterateAlways() override;
  void handleAction(int event, int action) override;

  bool isOutputEnabled();
  void setOutputEnabled(bool output_enabled);

  void setTRVTemperatureSetpoint(int16_t trv_temperature_setpoint);
  void setTRVMode(uint8_t trv_mode);

protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t _device;
  uint8_t _Tuya_dp_data[10];
  uint8_t _trv_commands_set;

  HvacBaseEE *_trv_hvac = nullptr;

  uint8_t _trv_thermometer_channel_no = 0xFF;

  int _trv_state = 0;
  bool _output_enabled = false;

  uint8_t _trv_mode = 0;
  int16_t _trv_temperature_setpoint = 0;

  uint32_t _refresh_ms      = 5000;
  uint32_t _last_refresh_ms = 0;

  void sendOnOff(bool state);
  void sendTRVMode(uint8_t trv_mode);
  void sendTRVTemperatureSetpoint(int16_t temperature_setpoint);
  void sendTRVTemperatureCalibration(int32_t temperature_calibration);
};

};  // namespace Control
};  // namespace Supla


#endif //SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_