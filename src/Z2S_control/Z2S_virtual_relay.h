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

#ifndef SRC_SUPLA_CONTROL_Z2S_VIRTUAL_RELAY_H_
#define SRC_SUPLA_CONTROL_Z2S_VIRTUAL_RELAY_H_

#include <supla/control/relay.h>
#include "ZigbeeGateway.h"

#include "Z2S_custom_actions_events.h"

#define MAX_COMMAND_DATA_SIZE                         0x0B 

#define Z2S_VIRTUAL_RELAY_FNC_NONE                    0x00

#define Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE            0x10
#define Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE_PROGRAM    0x11
#define Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE_PROGRAM_T  0x12
#define Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE_PROGRAM_V  0x13

#define Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM     0x20
#define Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM       0x21

#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_SWITCH       0x30
#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_MELODY       0x31
#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_VOLUME       0x32
#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_DURATION     0x33

#define Z2S_VIRTUAL_RELAY_FNC_GIEX_VALVE_MANUAL       0x40
#define Z2S_VIRTUAL_RELAY_FNC_GIEX_VALVE_PROGRAM      0x41

#define Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_STATE    0x50
#define Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_MODE     0x51

#define Z2S_VIRTUAL_RELAY_FNC_TUYA_DP_RELAY           0x60

static const uint32_t RELAY_FLAGS = 
  SUPLA_BIT_FUNC_CONTROLLINGTHEGATEWAYLOCK |
  SUPLA_BIT_FUNC_CONTROLLINGTHEGATE |
  SUPLA_BIT_FUNC_CONTROLLINGTHEGARAGEDOOR |
  SUPLA_BIT_FUNC_CONTROLLINGTHEDOORLOCK |
  SUPLA_BIT_FUNC_POWERSWITCH |
  SUPLA_BIT_FUNC_LIGHTSWITCH |
  SUPLA_BIT_FUNC_STAIRCASETIMER |
  SUPLA_BIT_FUNC_PUMPSWITCH |
  SUPLA_BIT_FUNC_HEATORCOLDSOURCESWITCH;



namespace Supla {
namespace Control {
class Z2S_VirtualRelay : public Relay {

 public:
  
  Z2S_VirtualRelay(
    ZigbeeGateway *gateway, zbg_device_params_t *device,
    uint8_t z2s_function = Z2S_VIRTUAL_RELAY_FNC_NONE);

  void onInit() override;
  void turnOn(_supla_int_t duration = 0) override;
  void turnOff(_supla_int_t duration = 0) override;
  void iterateAlways() override;

  void handleAction(int event, int action) override;

  void Z2S_setOnOff(bool on_off_state);

  void Z2S_setFunctionValueS8(int32_t z2s_function_value_S8);
  void Z2S_setFunctionValueU8(int32_t z2s_function_value_U8);
  void Z2S_setFunctionValueS32(int32_t z2s_function_value_S32);
  void Z2S_setFunctionValueU32(int32_t z2s_function_value_U32);

  bool isOn() override;

  void ping();

  void setKeepAliveSecs(uint32_t keep_alive_secs);
  void setTimeoutSecs(uint32_t timeout_secs);

  uint32_t getKeepAliveSecs();
  uint32_t getTimeoutSecs();

 protected:

  uint8_t Z2S_decValueU8(uint8_t lower_limit, uint8_t upper_limit, uint8_t step = 1);
  uint8_t Z2S_incValueU8(uint8_t lower_limit, uint8_t upper_limit, uint8_t step = 1);

  uint32_t Z2S_decValueU32(uint32_t lower_limit, uint32_t upper_limit, uint32_t step);
  uint32_t Z2S_incValueU32(uint32_t lower_limit, uint32_t upper_limit, uint32_t step);


  bool state = false;
  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;

  uint8_t _z2s_function = Z2S_VIRTUAL_RELAY_FNC_NONE;
  //uint8_t _z2s_function_data[MAX_COMMAND_DATA_SIZE];
  
  int8_t  _z2s_function_value_S8 = INT8_MAX; //0x7F
  int32_t _z2s_function_value_S32 = INT32_MAX;
  uint8_t  _z2s_function_value_U8 = UINT8_MAX; //0xFF
  uint32_t _z2s_function_value_U32 = UINT32_MAX;

  bool _fresh_start = true;

  bool _keep_alive_enabled = true;
  bool _timeout_enabled    = true;

  uint32_t _keep_alive_ms = 45000;
  uint32_t _timeout_ms    = 60000;
  uint32_t _last_ping_ms  = 0;
  uint32_t _last_seen_ms  = 0;
 
};

};  // namespace Control
};  // namespace Supla

#endif  // SRC_SUPLA_CONTROL_Z2S_VIRTUAL_RELAY_H_