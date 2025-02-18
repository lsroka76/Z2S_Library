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

#include "Z2S_rgbw_light_source.h"
#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>


Supla::Control::Z2S_RGBWLightSource::Z2S_RGBWLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Supla::Control::Z2S_RGBWLightSource::onInit() {
}

void Supla::Control::Z2S_RGBWLightSource::setRGBWValueOnServer(uint32_t red,
                     		         uint32_t green,
                                         uint32_t blue,
                                         uint32_t colorBrightness,
                                         uint32_t brightness) {
   channel.setNewValue(
       red, green, blue, colorBrightness, brightness);
}


void Supla::Control::Z2S_RGBWLightSource::turnOn() {


  if (_gateway && Zigbee.started()) {   
    _state = true;
    _gateway->sendOnOffCmd(&_device, _state);
    //channel.setNewValue(state);
  }
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void Supla::Control::Z2S_RGBWLightSource::turnOff() {
  
  _state = false;
  if (_gateway && Zigbee.started()) {   
    _gateway->sendOnOffCmd(&_device, _state);
    //channel.setNewValue(state);
  }
  
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}


void Supla::Control::Z2S_RGBWLightSource::setRGBWValueOnDevice(uint32_t red,
                          uint32_t green,
                          uint32_t blue,
                          uint32_t colorBrightness,
                          uint32_t brightness) {

  if (_gateway && Zigbee.started()) {   

    log_i("Supla::Control::Z2S_RGBWLightSource::setRGBWValueOnDevice: red 0x%x, green 0x%x, blue 0x%x, color brightness 0x%x, brightness 0x%x",
          red, green, blue, colorBrightness, brightness);
    
   }
}


//#endif