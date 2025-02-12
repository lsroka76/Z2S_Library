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

#include "Z2S_dimmer_light_source.h"
#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

Supla::Control::Z2S_DimmerLightSource::Z2S_DimmerLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Supla::Control::Z2S_DimmerLightSource::onInit() {
}

void Supla::Control::Z2S_DimmerLightSource::setRGBWValueOnServer(uint32_t red,
                     		         uint32_t green,
                                         uint32_t blue,
                                         uint32_t colorBrightness,
                                         uint32_t brightness) {
   channel.setNewValue(
        red, green, blue, colorBrightness, brightness);
}

void Supla::Control::Z2S_DimmerLightSource::turnOn() {


  if (_gateway && Zigbee.started()) {   
    _state = true;
    
    _Tuya_dp_data[0] = 0x00; //Dimmer 1 on, payload 7
    _Tuya_dp_data[1] = 0x03;
    _Tuya_dp_data[2] = 0x01;
    _Tuya_dp_data[3] = 0x01;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x01;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    channel.setNewValue(_state);
  }
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void Supla::Control::Z2S_DimmerLightSource::turnOff() {
  
  _state = false;
  if (_gateway && Zigbee.started()) {   
    
    _Tuya_dp_data[0] = 0x00; //Dimmer 1 on, payload 7
    _Tuya_dp_data[1] = 0x03;
    _Tuya_dp_data[2] = 0x01;
    _Tuya_dp_data[3] = 0x01;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x00;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    channel.setNewValue(_state);
  }
  
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}


void Supla::Control::Z2S_DimmerLightSource::setRGBWValueOnDevice(uint32_t red,
                          uint32_t green,
                          uint32_t blue,
                          uint32_t colorBrightness,
                          uint32_t brightness) {

  if (_gateway && Zigbee.started()) {   

    log_i("Supla::Control::Z2S_DimmerLightSource::setRGBWValueOnDevice: red 0x%x, green 0x%x, blue 0x%x, color brightness 0x%x, brightness 0x%x",
          red, green, blue, colorBrightness, brightness);
    
    uint8_t _brightness = map(brightness, 0, 1023, 1, 254);
    log_i("brightness mapping before 0x%x and after 0x%x", brightness, _brightness);
    if (_brightness == 1) turnOff();
    else {
      if (!_state) turnOn();       
    }  
  }
}


//#endif