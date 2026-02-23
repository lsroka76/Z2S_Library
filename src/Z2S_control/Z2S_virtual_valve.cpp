/*
   Copyright (C) AC SOFTWARE SP. Z O.O

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

#include "Z2S_virtual_valve.h"
#include "TuyaDatapoints.h"

using Supla::Control::Z2S_VirtualValve;

Z2S_VirtualValve::Z2S_VirtualValve(ZigbeeGateway *gateway, zbg_device_params_t *device, bool openClose, uint8_t z2s_function)
: ValveBase(openClose), _gateway(gateway), _z2s_function(z2s_function) {

      memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Z2S_VirtualValve::setValueOnDevice(uint8_t openLevel) {
    
  if (_gateway && Zigbee.started()) { 

    valveOpenState = openLevel;

    switch (_z2s_function) {

      case Z2S_VIRTUAL_VALVE_FNC_DEFAULT_ON_OFF: {

        bool state = (openLevel == 0) ? false : true;
        _gateway->sendOnOffCmd(&_device, state); 

      } break;

      case Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY: {

        uint8_t Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[] = 
        { 00, 00, TUYA_ON_OFF_BATTERY_VALVE_SWITCH_DP, TUYA_DP_TYPE_BOOL, 
          00, 01, 00};

        uint16_t _tsn_number = random(0x0000, 0xFFFF); 

        Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[0] = (_tsn_number & 0xFF00) >> 8;
        Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[1] = (_tsn_number & 0x00FF);
        Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[6] = (openLevel == 0) ? 0 : 1;

        _gateway->sendCustomClusterCmd(
          &_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, 
          Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD, false);

      } break;
    }
  }
}

uint8_t Z2S_VirtualValve::getValueOpenStateFromDevice() {

  return valveOpenState;
}

void Z2S_VirtualValve::setValueOnServer(bool state) {
  
  Refresh();
  valveOpenState = state ? 100 : 0;
  channel.setValveOpenState(valveOpenState);
}

void Z2S_VirtualValve::ping() {

  if (Zigbee.started()) {

    _fresh_start = false;

    switch (_z2s_function) {


      case Z2S_VIRTUAL_VALVE_FNC_DEFAULT_ON_OFF:

      _gateway->sendAttributeRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
        ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
      break;


      case Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY: 

        sendTuyaRequestCmdBool(
          _gateway, &_device, TUYA_ON_OFF_BATTERY_VALVE_SWITCH_DP, 
          (valveOpenState == 0) ? 0 : 1);

    }
  }
}


void Z2S_VirtualValve::iterateAlways() {

  
  //uint32_t current_millis = millis();

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (_keep_alive_ms && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (true) {
      
      //_last_seen_ms = zbGateway.getZbgDeviceUnitLastSeenMs(_device.short_addr);
      if ((millis() - _last_seen_ms) > _keep_alive_ms) {
      	ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        if (!channel.isStateOnline()) 
	        channel.setStateOnline();
      }
    }
  }
  if (_timeout_ms && channel.isStateOnline() && 
      ((millis() - _last_seen_ms) > _timeout_ms)) {

	  log_i(
      "current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);

    log_i(
      "current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);

    if ((millis() - _last_seen_ms) > _timeout_ms)
      channel.setStateOffline();
  }
}

void Z2S_VirtualValve::Refresh() {

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();
}


void Z2S_VirtualValve::setKeepAliveSecs(uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
}

void Z2S_VirtualValve::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  
  if (_timeout_ms == 0)
    channel.setStateOnline();
}

uint32_t Z2S_VirtualValve::getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t Z2S_VirtualValve::getTimeoutSecs() {

  return _timeout_ms / 1000;
}
