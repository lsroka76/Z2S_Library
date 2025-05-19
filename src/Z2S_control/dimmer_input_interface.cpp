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

#include "dimmer_input_interface.h"

#include <supla/log_wrapper.h>

Supla::Control::DimmerInputInterface::DimmerInputInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t dimmer_mode) 
  : _gateway(gateway), _dimmer_mode(dimmer_mode) {
  
  if (device)
    memcpy(&_device, device, sizeof(zbg_device_params_t));  
  else   
  memset(&_device, 0, sizeof(zbg_device_params_t));  

  channel.setType(SUPLA_CHANNELTYPE_DIMMER);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER);
  //channel.setFlag(SUPLA_CHANNEL_FLAG_RGBW_COMMANDS_SUPPORTED);
}

int32_t Supla::Control::DimmerInputInterface::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "DimmerInputInterface[%d].handleNewValueFromServer: red=%d, green=%d, blue=%d, colorBrightness=%d, "
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

  brightness = (brightness/25) * 25;
  
  if (brightness != _brightness) {
    if (brightness == 0) {
      log_i("DIMMER OFF");
    }
    else {
      if (_brightness == 0) {
        log_i("DIMMER ON - SEND ON(ONOFF) + SEND LEVEL(brightness");
      }
      else {
        log_i("DIMMER ALREADY ON - SEND LEVEL(brightness");
      }
    }
  }

  _last_brightness = _brightness;

  _brightness = brightness;

  _lastMsgReceivedMs = millis();

  return 1;
}

void Supla::Control::DimmerInputInterface::fillSuplaChannelNewValue(TSD_SuplaChannelNewValue *value) {

  if (value == nullptr) {
    return;
  }
  SUPLA_LOG_DEBUG(
      "DimmerInputInterface[%d].fillSuplaChannelNewValue: value[0]=%d, value[1]=%d, value[2]=%d, value[3]=%d, value[4]=%d, value[5]=%d, value[6]=%d",
      getChannelNumber(),
      value->value[0],
      value->value[1],
      value->value[2],
      value->value[3],
      value->value[4],
      value->value[5],
      value->value[6]);
}

void Supla::Control::DimmerInputInterface::sendValueToDevice(uint8_t brightness) {

}

void Supla::Control::DimmerInputInterface::setValueOnServer(uint16_t value) {

  
  _lastMsgReceivedMs = millis();
}

void Supla::Control::DimmerInputInterface::ping() {

}

void Supla::Control::DimmerInputInterface::iterateAlways() {
  
  if (_lastMsgReceivedMs != 0) {//&& millis() - _lastMsgReceivedMs >= 400) {
    _lastMsgReceivedMs = 0;
    channel.setNewValue(0, 0, 0, 0, _brightness);
    sendValueToDevice(_brightness);
  }
}

void Supla::Control::DimmerInputInterface::setDimmerMode(uint8_t dimmer_mode) {

  _dimmer_mode = dimmer_mode;
}

uint8_t Supla::Control::DimmerInputInterface::getDimmerMode() {
  
  return _dimmer_mode;
}
