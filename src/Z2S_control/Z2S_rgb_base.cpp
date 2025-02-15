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

#include "Z2S_rgb_base.h"

#include <supla/log_wrapper.h>

Supla::Control::Z2S_RGBBase::Z2S_RGBBase() {

  channel.setType(SUPLA_CHANNELTYPE_RGBLEDCONTROLLER);
  channel.setDefault(SUPLA_CHANNELFNC_RGBLIGHTING);
}

int32_t Supla::Control::Z2S_RGBBase::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "Z2S_RGBBase[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d, "
      "brightness=%d, command=%d, toggleOnOff=%d",
      getChannelNumber(),
      red,
      green,
      blue,
      colorBrightness,
      brightness,
      command,
      toggleOnOff);

  if (brightness > 100) {
    brightness = 100;
  }
  if (colorBrightness > 100) {
    colorBrightness = 100;
  }

  if ((toggleOnOff == 1) && (colorBrightness == 0)) {
    log_i("Supla::Control::Z2S_RGBBase::handleNewValueFromServer - turnOff");
    turnOff();
  }
  else {
    if (toggleOnOff == 1) {
      log_i("Supla::Control::Z2S_RGBBase::handleNewValueFromServer - turnOn");
      turnOn(); 
    }
    log_i("Supla::Control::Z2S_RGBBase::handleNewValueFromServer -> sendValueToDevice");
    
    _last_svr_red = red;
    _last_svr_green = green;
    _last_svr_blue = blue;
    _last_svr_colorBrightness = colorBrightness;
    _last_svr_brightness = brightness;

    channel.setNewValue(red, green, blue, colorBrightness,0);
    sendValueToDevice(red, green, blue, colorBrightness);
  }
  return -1;
}

void Supla::Control::Z2S_RGBBase::setValueOnServer(uint8_t red,
                                 		   uint8_t green,
                                                   uint8_t blue,
                                                   uint8_t colorBrightness) {

  channel.setNewValue(red, green, blue, colorBrightness, 0);
}

void Supla::Control::Z2S_RGBBase::setStateOnServer(bool state) {
  channel.setNewValue(-1, -1, -1, state ? 100 : 0, -1);
}


