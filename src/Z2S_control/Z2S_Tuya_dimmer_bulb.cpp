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


#include "Z2S_Tuya_dimmer_bulb.h"
#include <supla/log_wrapper.h>


Supla::Control::Z2S_TuyaDimmerBulb::Z2S_TuyaDimmerBulb(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t)); 
}

bool Supla::Control::Z2S_TuyaDimmerBulb::getTuyaMode() {
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

void Supla::Control::Z2S_TuyaDimmerBulb::setTuyaMode(uint8_t mode) {
  if (_gateway && Zigbee.started()) { 
    log_i("setting Tuya mode");		
    _Tuya_bulb_mode = mode;
    _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_bulb_mode, true);
    if (getTuyaMode() != mode)
      log_i("setting Tuya mode failed!");
  } else log_i("Zigbee not started");
}


void Supla::Control::Z2S_TuyaDimmerBulb::turnOn() {

  if (_gateway && Zigbee.started()) {   
    _state = true;
    _gateway->sendOnOffCmd(&_device, _state);
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  }
}

void Supla::Control::Z2S_TuyaDimmerBulb::turnOff() {

  if (_gateway && Zigbee.started()) {   
    _state = false;
    _gateway->sendOnOffCmd(&_device, _state);
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  }
}

void Supla::Control::Z2S_TuyaDimmerBulb::toggle() {
  if (_state) turnOff();
  else turnOn();
}

bool Supla::Control::Z2S_TuyaDimmerBulb::isOn() {
  return _state;
}

void Supla::Control::Z2S_TuyaDimmerBulb::onInit() {
  
  _last_iterate_ms = 0;
  channel.setOffline();
}

void Supla::Control::Z2S_TuyaDimmerBulb::sendValueToDevice(uint32_t brightness) {

  if (_gateway && Zigbee.started()) {
    uint8_t _brightness = map(brightness, 0, 100, 1, 254); 
    if (getTuyaMode()) {
      if (_Tuya_bulb_mode != 0x00)
        setTuyaMode(0x00);
      _gateway->sendLevelMoveToLevelCmd(&_device, _brightness, 1);
    }
  }
}

void Supla::Control::Z2S_TuyaDimmerBulb::setStateOnServer(bool state) {
  
  if (getTuyaMode())
    channel.setNewValue((_Tuya_bulb_mode == 0x00));
}

bool Supla::Control::Z2S_TuyaDimmerBulb::ping() {
  
  if (_gateway && Zigbee.started()) {
    if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true))
      return true;
    else return false;
  } else return false;

}
void Supla::Control::Z2S_TuyaDimmerBulb::iterateAlways() {

  if (millis() - _last_iterate_ms >= 10000) {
    
    _last_iterate_ms = millis();

    if (!channel.isOnline()) {  
       if (!ping())
         log_i("Zigbee device(0x%x) offline...", _device.short_addr);
       else {
         channel.setOnline();
         if (getTuyaMode()) 
           channel.setNewValue((_Tuya_bulb_mode == 0));
       }
    } else 
        if (!ping()) {
          log_i("Zigbee device(0x%x) offline...", _device.short_addr);
          channel.setNewValue(false);
          channel.setOffline(); 
        }
//else channel.setNewValue(_last_red, _last_green, _last_blue, _last_colorBrightness, _last_brightness);
  } //time loop
} //iterateAlways()

