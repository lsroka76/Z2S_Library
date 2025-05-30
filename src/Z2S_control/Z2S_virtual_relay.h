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

#define Z2S_VIRTUAL_RELAY_FNC_NONE                0x00

#define Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE        0x10

#define Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM 0x20
#define Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM   0x21

#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_SWITCH   0x30
#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_MELODY   0x31
#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_VOLUME   0x32
#define Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_DURATION 0x33

namespace Supla {
namespace Control {
class Z2S_VirtualRelay : public Relay {
 public:
  Z2S_VirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t z2s_function = Z2S_VIRTUAL_RELAY_FNC_NONE);

  void onInit() override;
  void turnOn(_supla_int_t duration = 0) override;
  void turnOff(_supla_int_t duration = 0) override;
  void iterateAlways() override;

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
  bool state = false;
  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;

  uint8_t _z2s_function = Z2S_VIRTUAL_RELAY_FNC_NONE;
  uint8_t _z2s_function_data[10];
  
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
//#endif  // #ifdef Z2S_GATEWAY