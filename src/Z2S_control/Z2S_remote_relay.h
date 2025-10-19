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

#ifndef SRC_SUPLA_CONTROL_Z2S_REMOTE_RELAY_H_
#define SRC_SUPLA_CONTROL_Z2S_REMOTE_RELAY_H_

#include <NetworkClient.h>
#include <ESPmDNS.h>

#include <supla/control/relay.h>
#include "ZigbeeGateway.h"

#define REMOTE_CMD_TURN_OFF       0x00
#define REMOTE_CMD_TURN_ON        0x01


namespace Supla {
namespace Control {
class Z2S_RemoteRelay : public Relay {

 public:
  
  Z2S_RemoteRelay(NetworkClient *remote_gateway, 
                  uint8_t remote_Supla_channel);

  void onInit() override;
  void turnOn(_supla_int_t duration = 0) override;
  void turnOff(_supla_int_t duration = 0) override;
  void iterateAlways() override;

  void Z2S_setOnOff(bool on_off_state);

  bool isOn() override;

  void ping();

  void setRemoteGatewayIPAddress(IPAddress remote_gateway_ip) {

    _remote_gateway_ip = remote_gateway_ip;
  }

  void setRemoteGatewayIPAddress(String remote_gateway_ip) {

    _remote_gateway_ip.fromString(remote_gateway_ip);
  }

  void setRemoteGatewayMDNSName(char *remote_gateway_mDNS_name) {

    _remote_gateway_mDNS_name = remote_gateway_mDNS_name;
  }

  void setRemoteGatewaySuplaChannel(uint8_t remote_Supla_channel) {

    _remote_Supla_channel = remote_Supla_channel;
  }

  //void setKeepAliveSecs(uint32_t keep_alive_secs);
  //void setTimeoutSecs(uint32_t timeout_secs);

  //uint32_t getKeepAliveSecs();
  //uint32_t getTimeoutSecs();

 protected:

  bool state = false;
  NetworkClient *_remote_gateway = nullptr;
  IPAddress _remote_gateway_ip = INADDR_NONE;
  char *_remote_gateway_mDNS_name = nullptr;
  uint8_t _remote_Supla_channel = 0xFF;


  uint8_t _retries_number = 3;
  uint8_t _current_retry = 0;
  bool _pending_state = false;
  
  uint32_t _retry_pause_ms = 30000;
  uint32_t  _retry_time_ms = 0;

  //bool _keep_alive_enabled = true;
  //bool _timeout_enabled    = true;

  //uint32_t _keep_alive_ms = 45000;
  //uint32_t _timeout_ms    = 60000;
  //uint32_t _last_ping_ms  = 0;
  //uint32_t _last_seen_ms  = 0;

  bool connectRemoteGateway();
  void postponeTurnOnOff(bool pending_state);
 
};

};  // namespace Control
};  // namespace Supla

#endif //SRC_SUPLA_CONTROL_Z2S_REMOTE_RELAY_H_