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

namespace Supla {
namespace Control {
class Z2S_VirtualRelay : public Relay {
 public:
  Z2S_VirtualRelay(ZigbeeGateway *Gateway, esp_zb_ieee_addr_t ieee_addr, _supla_int_t functions =
                   (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

  void onInit() override;
  void turnOn(_supla_int_t duration = 0) override;
  void turnOff(_supla_int_t duration = 0) override;
  void Z2S_setOnOff(bool on_off_state);
  bool isOn() override;

 protected:
  bool state = false;
  ZigbeeGateway *Gateway = nullptr;
  esp_zb_ieee_addr_t relay_ieee_addr;
};

};  // namespace Control
};  // namespace Supla

#endif  // SRC_SUPLA_CONTROL_Z2S_VIRTUAL_RELAY_H_
//#endif  // #ifdef Z2S_GATEWAY