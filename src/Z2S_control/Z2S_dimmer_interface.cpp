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

#include "Z2S_dimmer_interface.h"

#include <supla/log_wrapper.h>

Supla::Control::Z2S_DimmerInterface::Z2S_DimmerInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t dimmer_mode) 
  : _gateway(gateway), _dimmer_mode(dimmer_mode) {
  
  if (device)
    memcpy(&_device, device, sizeof(zbg_device_params_t));  
  else   
  memset(&_device, 0, sizeof(zbg_device_params_t));  

  channel.setType(SUPLA_CHANNELTYPE_DIMMER);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER);
  //channel.setFlag(SUPLA_CHANNEL_FLAG_RGBW_COMMANDS_SUPPORTED);
}

int32_t Supla::Control::Z2S_DimmerInterface::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "Z2S_DimmerInterface[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d, "
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

  return -1;
}

void Supla::Control::Z2S_DimmerInterface::sendValueToDevice(uint8_t brightness) {

  if (_gateway && Zigbee.started()) {
    switch (_dimmer_mode) {

      case Z2S_SEND_TO_LEVEL_DIMMER: {
        uint8_t level = map(brightness, 0, 100, 1, 254);
        _gateway->sendLevelMoveToLevelCmd(&_device, level, 1);
      } break;
      case Z2S_COLOR_TEMPERATURE_DIMMER: {
        uint16_t color_temperature = map(brightness, 0, 100, 454, 200);
	      _gateway->sendColorMoveToColorTemperatureCmd(&_device, color_temperature, 1);
      } break;
      case Z2S_TUYA_COLOR_TEMPERATURE_DIMMER: {
        uint16_t color_temperature = map(brightness, 0, 100, 500, 153);
	      _gateway->sendColorMoveToColorTemperatureCmd(&_device, color_temperature, 1);
      } break; 
      case Z2S_TUYA_F0_CMD_DIMMER: {
        uint16_t F0_brightness = map(brightness, 0, 100, 0, 1000);
	      _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U16, 2, (uint8_t *)&F0_brightness, false);
        if (F0_brightness == 0)
          _gateway->sendOnOffCmd(&_device, false);
      } break;
      case Z2S_TUYA_E0_CMD_DIMMER: {
        uint16_t E0_color_temperature = map(brightness, 0, 100, 0, 1000);
	      _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE0, ESP_ZB_ZCL_ATTR_TYPE_U16, 2, (uint8_t *)&E0_color_temperature, false);
      } break;
    } 
    
    //uint8_t light_mode = 0x01;
    //_gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &light_mode , false);
    //_gateway->sendColorMoveToHueAndSaturationCmd(&_device, brightness, 0xFE, 1);
  }
}

void Supla::Control::Z2S_DimmerInterface::setValueOnServer(uint16_t value) {

  return; //temp solution
  _last_brightness = _brightness;

  switch (_dimmer_mode) {

      case Z2S_SEND_TO_LEVEL_DIMMER:  
        _brightness = map(value, 1, 254, 0, 100); break;

      case Z2S_COLOR_TEMPERATURE_DIMMER: 
        _brightness = map(value, 250, 454, 0, 100); break;
      
      case Z2S_TUYA_COLOR_TEMPERATURE_DIMMER: 
        _brightness = map(value, 153, 500, 0, 100); break;
    }

  uint8_t diff = (_brightness > _last_brightness) ? (_brightness - _last_brightness) : (_last_brightness - _brightness);
  if (diff < 2)
    _brightness = _last_brightness;

  _lastMsgReceivedMs = millis();


}

void Supla::Control::Z2S_DimmerInterface::ping() {

  if (_gateway && Zigbee.started()) {
    _fresh_start = false;
    _gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
  }
}

void Supla::Control::Z2S_DimmerInterface::iterateAlways() {
  if (_lastMsgReceivedMs != 0 && millis() - _lastMsgReceivedMs >= 400) {
    _lastMsgReceivedMs = 0;
    channel.setNewValue(0, 0, 0, 0, _brightness);
    if ((_turn_rgb_off) && _rgb) {
      _rgb->getChannel()->setNewValue(0, 0, 0, 0, 0);
      _turn_rgb_off = false;
    }
    sendValueToDevice(_brightness);
  }

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (_keep_alive_enabled && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (_gateway) {
      
      _last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
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
  if (_timeout_enabled && channel.isStateOnline() && ((millis() - _last_seen_ms) > _timeout_ms)) {
	  log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);
    _last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);
    if ((millis() - _last_seen_ms) > _timeout_ms)
      channel.setStateOffline();
   // _last_ping_ms = current_millis;
  }
}

void Supla::Control::Z2S_DimmerInterface::setDimmerMode(uint8_t dimmer_mode) {

  _dimmer_mode = dimmer_mode;
}
uint8_t Supla::Control::Z2S_DimmerInterface::getDimmerMode() {
  
  return _dimmer_mode;
}


void Supla::Control::Z2S_DimmerInterface::setKeepAliveSecs(uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
  if (_keep_alive_ms == 0)
    _keep_alive_enabled = false;
  else 
    _keep_alive_enabled = true;
}

void Supla::Control::Z2S_DimmerInterface::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    channel.setStateOnline();
  }
  else
   _timeout_enabled = true;
}

uint32_t Supla::Control::Z2S_DimmerInterface::getKeepAliveSecs() {

  return _keep_alive_ms * 1000;
}

uint32_t Supla::Control::Z2S_DimmerInterface::getTimeoutSecs() {

  return _timeout_ms * 1000;
}

void Supla::Control::Z2S_DimmerInterface::increaseBrightness(int8_t add_to_brightness) {

  _last_brightness = _brightness;

  int16_t new_brightness = _brightness + add_to_brightness;

  if (new_brightness < 0)
    new_brightness = 0;

  if (new_brightness > 100)
    new_brightness = 100;

  _brightness = new_brightness;

  _lastMsgReceivedMs = millis();
}

void Supla::Control::Z2S_DimmerInterface::handleAction(int event, int action) {

  (void)(event);
  
  switch (action) {
    case DIM_W: {

      increaseBrightness(-10);
      break;
    }

    case BRIGHTEN_W: {
      
      increaseBrightness(10);
      break;
    }
  }
}