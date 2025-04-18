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
#include <supla/control/remote_output_interface.h>
#include "ZigbeeGateway.h"
#include "hvac_base_ee.h"

#define SASWELL_CMD_SET   0x01 

#define SASWELL_CMD_ON_1  0x65
#define SASWELL_CMD_ON_2  0x01
#define SASWELL_CMD_ON_3  0x00
#define SASWELL_CMD_ON_4  0x01
#define SASWELL_CMD_ON_5  0x01

#define SASWELL_CMD_OFF_1 0x65
#define SASWELL_CMD_OFF_2 0x01
#define SASWELL_CMD_OFF_3 0x00
#define SASWELL_CMD_OFF_4 0x01
#define SASWELL_CMD_OFF_5 0x00

#define SASWELL_CMD_SET_TARGET_HEATSETPOINT_1 0x67
#define SASWELL_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define SASWELL_CMD_SET_TEMPERATURE_CALIBRATION_1 0x1B
#define SASWELL_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define SASWELL_CMD_SET_LOCAL_TEMPERATURE_1       0x66
#define SASWELL_CMD_SET_LOCAL_TEMPERATURE_2       0x02

#define SASWELL_CMD_SET_RUNNING_STATE_1           0x03
#define SASWELL_CMD_SET_RUNNING_STATE_IDLE        0x00
#define SASWELL_CMD_SET_RUNNING_STATE_HEAT        0x01

#define SASWELL_CMD_SET_HEATSETPOINT_MIN          0x01F4 //500
#define SASWELL_CMD_SET_HEATSETPOINT_MAX          0x0BB8 //3000

#define SASWELL_TARGET_HEATSETPOINT_FACTOR        0x0A //*10
#define SASWELL_LOCAL_TEMPERATURE_FACTOR          0x0A //*10
#define SASWELL_TEMPERATURE_CALIBRATION_FACTOR    0x01 //*1

#define ME167_CMD_SET   0x02

#define ME167_CMD_ON_1  0x02
#define ME167_CMD_ON_2  0x04
#define ME167_CMD_ON_3  0x00
#define ME167_CMD_ON_4  0x01
#define ME167_CMD_ON_5  0x01

#define ME167_CMD_OFF_1 0x02
#define ME167_CMD_OFF_2 0x04
#define ME167_CMD_OFF_3 0x00
#define ME167_CMD_OFF_4 0x01
#define ME167_CMD_OFF_5 0x02

#define ME167_CMD_SET_TARGET_HEATSETPOINT_1 0x04
#define ME167_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define ME167_CMD_SET_TEMPERATURE_CALIBRATION_1 0x2F
#define ME167_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define ME167_CMD_SET_LOCAL_TEMPERATURE_1 0x05
#define ME167_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define ME167_CMD_SET_RUNNING_STATE_1     0x03
#define ME167_CMD_SET_RUNNING_STATE_IDLE  0x01
#define ME167_CMD_SET_RUNNING_STATE_HEAT  0x00 //inverted

#define ME167_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define ME167_CMD_SET_HEATSETPOINT_MAX    0x0BB8 //3000

#define ME167_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define ME167_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define ME167_TEMPERATURE_CALIBRATION_FACTOR 0x01//*10

#define ME167_STATE_1 0x03
#define ME167_STATE_2 0x04

#define ME167_STATE_IDLE    0x01
#define ME167_STATE_HEATING 0x00

#define BECA_CMD_SET   0x03

#define BECA_CMD_ON_1  0x01
#define BECA_CMD_ON_2  0x04
#define BECA_CMD_ON_3  0x00
#define BECA_CMD_ON_4  0x01
#define BECA_CMD_ON_5  0x01

#define BECA_CMD_OFF_1 0x07
#define BECA_CMD_OFF_2 0x04
#define BECA_CMD_OFF_3 0x00
#define BECA_CMD_OFF_4 0x01
#define BECA_CMD_OFF_5 0x00

#define BECA_CMD_SET_TARGET_HEATSETPOINT_1 0x02
#define BECA_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define BECA_CMD_SET_TEMPERATURE_CALIBRATION_1 0x69
#define BECA_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define BECA_CMD_SET_LOCAL_TEMPERATURE_1 0x03
#define BECA_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define BECA_CMD_SET_RUNNING_STATE_1     0x07
#define BECA_CMD_SET_RUNNING_STATE_IDLE  0x00
#define BECA_CMD_SET_RUNNING_STATE_HEAT  0x01 //valve state

#define BECA_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define BECA_CMD_SET_HEATSETPOINT_MAX    0x0BB8 //3000

#define BECA_TARGET_HEATSETPOINT_FACTOR     0x01 //*1
#define BECA_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define BECA_TEMPERATURE_CALIBRATION_FACTOR 0x01 //*1

#define MOES_CMD_SET   0x04

#define MOES_CMD_ON_1  0x6A
#define MOES_CMD_ON_2  0x04
#define MOES_CMD_ON_3  0x00
#define MOES_CMD_ON_4  0x01
#define MOES_CMD_ON_5  0x00

#define MOES_CMD_OFF_1 0x6A
#define MOES_CMD_OFF_2 0x04
#define MOES_CMD_OFF_3 0x00
#define MOES_CMD_OFF_4 0x01
#define MOES_CMD_OFF_5 0x02

#define MOES_CMD_SET_TARGET_HEATSETPOINT_1 0x02
#define MOES_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define MOES_CMD_SET_TEMPERATURE_CALIBRATION_1 0x2C
#define MOES_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define MOES_CMD_SET_LOCAL_TEMPERATURE_1 0x03
#define MOES_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define MOES_CMD_SET_RUNNING_STATE_1     0x6D
#define MOES_CMD_SET_RUNNING_STATE_IDLE  0x00
#define MOES_CMD_SET_RUNNING_STATE_HEAT  0x64 //valve state

#define MOES_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define MOES_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500

#define MOES_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define MOES_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define MOES_TEMPERATURE_CALIBRATION_FACTOR 0x0A //*10


namespace Supla {
namespace Control {
class Z2S_TRVInterface : public RemoteOutputInterface, public ActionHandler, public Element {
 public:
  Z2S_TRVInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_commands_set);

  Supla::Control::HvacBaseEE *getTRVHvac();
  void setTRVHvac(Supla::Control::HvacBaseEE *trv_hvac);

  void setTemperatureCalibrationOffsetTrigger(int32_t temperature_calibration_offset_trigger);
  void setTemperatureCalibrationUpdateMs(uint32_t temperature_calibration_update_ms);

  void setTRVTemperatureSetpoint(int32_t trv_temperature_setpoint);
  void setTRVSystemMode(uint8_t trv_system_mode);
  void setTRVRunningState(uint8_t trv_running_state);
  void setTRVLocalTemperature(int32_t trv_local_temperature);
  void setTRVTemperatureCalibration(int32_t trv_temperature_calibration);

  void iterateAlways() override;
  void handleAction(int event, int action) override;


protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t _device;
  uint8_t _Tuya_dp_data[10];
  uint8_t _trv_commands_set;

  HvacBaseEE *_trv_hvac = nullptr;

  uint8_t _trv_system_mode         = 0;
  bool    _trv_system_mode_updated = false;

  uint8_t _trv_running_state         = 0;
  bool    _trv_running_state_updated = false;

  int32_t _trv_temperature_setpoint         = 0;
  bool    _trv_temperature_setpoint_updated = false;

  int32_t _trv_local_temperature         = INT32_MIN;
  int32_t _trv_last_local_temperature    = INT32_MIN;
  bool    _trv_local_temperature_updated = false;

  int32_t _trv_temperature_calibration         = 0;
  int32_t _trv_last_temperature_calibration    = 0;
  bool    _trv_temperature_calibration_updated = false;

  int32_t _temperature_calibration_offset      = 0;
  int32_t _last_temperature_calibration_offset = 0;
  
  int32_t _temperature_calibration_offset_trigger = 500;

  uint32_t  _temperature_calibration_update_ms      = 5 * 60 * 1000; //5 minutes
  uint32_t  _temperature_calibration_last_update_ms = 0;

  uint32_t _temperature_ping_ms = 60 * 1000;
  uint32_t _last_temperature_ping_ms = 0;

  uint32_t _refresh_ms      = 5000;
  uint32_t _last_refresh_ms = 0;

  void sendTRVSystemMode(uint8_t trv_system_mode);
  void sendTRVTemperatureSetpoint(int32_t temperature_setpoint);
  void sendTRVTemperatureCalibration(int32_t temperature_calibration);
};

};  // namespace Control
};  // namespace Supla


#endif //SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_
