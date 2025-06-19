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

Z2S_VirtualValve::Z2S_VirtualValve(ZigbeeGateway *gateway, zbg_device_params_t *device, bool openClose = true)
: ValveBase(openClose), _gateway(gateway) {

      memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Z2S_VirtualValve::setValueOnDevice(uint8_t openLevel) {
    
  if (_gateway && Zigbee.started()) { 

      valveOpenState = openLevel;
      bool state = (openLevel == 0) ? false : true;
      _gateway->sendOnOffCmd(&_device, state);
  }

}

uint8_t Z2S_VirtualValve::getValueOpenStateFromDevice() {

  if (_gateway && Zigbee.started()) {   
     
    bool state = true;

    if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true))
         state = *((bool *)_gateway->getReadAttrLastResult()->data.value);
    
    valveOpenState = state ? 100 : 0;
  }

  return valveOpenState;
}
