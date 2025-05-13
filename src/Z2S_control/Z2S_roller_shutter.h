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

#ifndef SRC_SUPLA_CONTROL_Z2S_ROLLER_SHUTTER_H_
#define SRC_SUPLA_CONTROL_Z2S_ROLLER_SHUTTER_H_

#include <supla/control/roller_shutter_interface.h>
#include "ZigbeeGateway.h"

namespace Supla {
namespace Control {
class Z2S_RollerShutter : public RollerShutterInterface {
 public:
  Z2S_RollerShutter(ZigbeeGateway *gateway, zbg_device_params_t *device);

  void onInit() override;
  void onTimer() override;
  void iterateAlways() override;

  void ping();

  void setKeepAliveSecs(uint32_t keep_alive_secs);
  void setTimeoutSecs(uint32_t timeout_secs);

  uint32_t getKeepAliveSecs();
  uint32_t getTimeoutSecs();

 protected:

  void rsOpen();
  void rsClose();
  void rsStop();
  void rsMoveToLiftPercentage(uint8_t lift_percentage);

  bool state = false;
  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;

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

#endif  // SRC_SUPLA_CONTROL_Z2S_ROLLER_SHUTTER_H_
//#endif  // #ifdef Z2S_GATEWAY