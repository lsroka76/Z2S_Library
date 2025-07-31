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

        uint8_t Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[] = { 00, 00, 0x01, 01, 00, 01, 00};

        uint16_t _tsn_number = random(0x0000, 0xFFFF); 

        Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[0] = (_tsn_number & 0xFF00) >> 8;
        Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[1] = (_tsn_number & 0x00FF);
        Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD[6] = (openLevel == 0) ? 0 : 1;

        _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY_SWITCH_CMD, false);

      } break;
    }
  }
}

uint8_t Z2S_VirtualValve::getValueOpenStateFromDevice() {

  /*if (_gateway && Zigbee.started()) {   
     
    bool state = true;

    if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true))
         state = *((bool *)_gateway->getReadAttrLastResult()->data.value);
    
    valveOpenState = state ? 100 : 0;
  }*/

  return valveOpenState;
}

void Z2S_VirtualValve::setValueOnServer(bool state) {
  
  /*state = on_off_state;

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();
*/valveOpenState = state ? 100 : 0;
  channel.setValveOpenState(valveOpenState);
  //channel.setNewValue(state);
  // Schedule save in 5 s after state change
  //Supla::Storage::ScheduleSave(5000);
}