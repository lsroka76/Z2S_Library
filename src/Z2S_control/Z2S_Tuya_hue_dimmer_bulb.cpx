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


#include "Z2S_Tuya_hue_dimmer_bulb.h"
#include <supla/log_wrapper.h>


Supla::Control::Z2S_TuyaHueDimmerBulb::Z2S_TuyaHueDimmerBulb(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t)); 
}


bool Supla::Control::Z2S_TuyaHueDimmerBulb::getTuyaMode() {
  if (_gateway && Zigbee.started()) {
    log_i("reading Tuya mode");
    if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF000, true)) {
      _Tuya_bulb_mode = *(uint8_t *)_gateway->getReadAttrLastResult()->data.value;
      return true;
    }	
    else log_i("reading Tuya mode failed!");
  } else log_i("Zigbee not started");
  return false;
}

void Supla::Control::Z2S_TuyaHueDimmerBulb::setTuyaMode(uint8_t mode) {
  if (_gateway && Zigbee.started()) { 
    log_i("setting Tuya mode");		
    _Tuya_bulb_mode = mode;
    _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_bulb_mode, true);
    if (getTuyaMode() != mode)
      log_i("setting Tuya mode failed!");
  } else log_i("Zigbee not started");
}


void Supla::Control::Z2S_TuyaHueDimmerBulb::turnOn() {

  if (_gateway && Zigbee.started()) {   
    _state = true;
    _gateway->sendOnOffCmd(&_device, _state);
    channel.setNewValue(_state);
    setTuyaMode(01);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  }
}

void Supla::Control::Z2S_TuyaHueDimmerBulb::turnOff() {

  if (_gateway && Zigbee.started()) {   
    _state = false;
    _gateway->sendOnOffCmd(&_device, _state);
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  }
}

void Supla::Control::Z2S_TuyaHueDimmerBulb::toggle() {
  if (_state) turnOff();
  else turnOn();
}

bool Supla::Control::Z2S_TuyaHueDimmerBulb::isOn() {
  return _state;
}

void Supla::Control::Z2S_TuyaHueDimmerBulb::sendValueToDevice(uint32_t brightness) {

  if (_gateway && Zigbee.started()) {
    uint8_t _brightness = map(brightness, 0, 100, 1, 254); 
    
    _gateway->sendColorMoveToHueAndSaturationCmd(&_device, _brightness, 0xFE, 1);
  }
}
