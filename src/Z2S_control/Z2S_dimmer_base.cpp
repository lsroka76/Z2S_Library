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

#include "Z2S_dimmer_base.h"

#include <supla/log_wrapper.h>

Supla::Control::Z2S_DimmerBase::Z2S_DimmerBase() {

  channel.setType(SUPLA_CHANNELTYPE_DIMMER);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER);
}

int32_t Supla::Control::Z2S_DimmerBase::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "Z2S_DimmerBase hNVFS[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d, "
      "brightness=%d, command=%d, toggleOnOff=%d",
      getChannelNumber(),
      red,
      green,
      blue,
      colorBrightness,
      brightness,
      command,
      toggleOnOff);

  if (toggleOnOff > 0) {
    _last_channel_update_ms = 1;
  } else {
     _last_channel_update_ms = millis();
  }


  if (brightness > 100) {
    brightness = 100;
  }
 
  if ((toggleOnOff > 0) && (brightness == 100)) {
    brightness = _last_brightness;
  } else if (brightness > 0) {
        _last_brightness = brightness;
    }

  _current_brightness = brightness;

  sendValueToDevice(brightness);

  return -1;
}

void Supla::Control::Z2S_DimmerBase::setValueOnServer(uint32_t brightness) {

  uint8_t _brightness = map(brightness, 1, 254, 0, 100);
  channel.setNewValue(0, 0, 0, 0, _brightness);
}

void Supla::Control::Z2S_DimmerBase::setStateOnServer(bool state) {
  channel.setNewValue(0, 0, 0, 0, (state ? _last_brightness : 0));
}

void Supla::Control::Z2S_DimmerBase::handleAction(int event, int action) {
  SUPLA_LOG_DEBUG("Z2S_DimmerBase hA event=%d, action=%d", event, action);
}

void Supla::Control::Z2S_DimmerBase::iterateAlways() {

  if ((_last_channel_update_ms !=0) && (millis() - _last_channel_update_ms >= 400)) {
    
    _last_channel_update_ms = 0;
    channel.setNewValue(0, 0, 0, 0, _current_brightness);
  }

  if (millis() - _last_ping_ms >= 10000) {
    
    _last_ping_ms = millis();
  
    if (!channel.isStateOnline()) {  
       if (!ping())
         log_i("Zigbee device offline"); //(0x%x) offline...", _device.short_addr);
       else {
         channel.setStateOnline();
         channel.setNewValue(0, 0, 0, 0, (isOn() ? _last_brightness : 0));
       }
    } else {
        if (!ping()) {
          log_i("Zigbee device offline"); //(0x%x) offline...", _device.short_addr);
          channel.setNewValue(0, 0, 0, 0, 0);
          channel.setStateOffline(); 
        }
    }
  } 
} 
