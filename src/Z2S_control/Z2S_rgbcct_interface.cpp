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

#include "ColorFormat.h"
#include "Z2S_rgbcct_interface.h"
#include "rgbhsv.h"

#include <supla/log_wrapper.h>

Supla::Control::Z2S_RGBCCTInterface::Z2S_RGBCCTInterface(
  zbg_device_params_t *device, uint8_t rgb_mode) : _rgb_mode(rgb_mode) {

  if (device)
    memcpy(&_device, device, sizeof(zbg_device_params_t));  
  else   
  memset(&_device, 0, sizeof(zbg_device_params_t));  
  
  channel.setType(SUPLA_CHANNELTYPE_DIMMERANDRGBLED);
  channel.setFlag(SUPLA_CHANNEL_FLAG_RGBW_COMMANDS_SUPPORTED);
  channel.setFlag(SUPLA_CHANNEL_FLAG_RUNTIME_CHANNEL_CONFIG_UPDATE);
  channel.setFuncList(
      SUPLA_RGBW_BIT_FUNC_DIMMER | SUPLA_RGBW_BIT_FUNC_RGB_LIGHTING |
      SUPLA_RGBW_BIT_FUNC_DIMMER_AND_RGB_LIGHTING |
      SUPLA_RGBW_BIT_FUNC_DIMMER_CCT | SUPLA_RGBW_BIT_FUNC_DIMMER_CCT_AND_RGB);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER_CCT_AND_RGB);
  usedConfigTypes.set(SUPLA_CONFIG_TYPE_DEFAULT);
}

int32_t Supla::Control::Z2S_RGBCCTInterface::handleNewValueFromServer(
  TSD_SuplaChannelNewValue *newValue) {

  uint8_t whiteTemperature = static_cast<uint8_t>(newValue->value[7]);
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "Z2S_RGBCCTInterface[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d,"
      " brightness=%d, command=%d, toggleOnOff=%d, whiteTemperature = %d", 
      getChannelNumber(),red, green, blue, colorBrightness, brightness, 
      command, toggleOnOff, whiteTemperature);

  if (colorBrightness > 100) {
    colorBrightness = 100;
  }

  if ((_colorBrightness > 0) && (colorBrightness == 0)) {
      
    log_i("RGB OFF");

    if (turnOnOff > 0)
      turnOnOff--;
  }
  
  if ((_colorBrightness == 0) && (colorBrightness > 0)) {

    log_i("RGB ON");
    
    if (turnOnOff == 0)
      turnOnOff = 1;
    else turnOnOff = 2;

    whiteTemperature = 0; //TURN OFF CCT
  }
      
  _last_red = _red;
  _last_green = _green;
  _last_blue  = _blue;
  _last_colorBrightness = _colorBrightness;

  _red = red;
  _green = green;
  _blue = blue;
  _colorBrightness = colorBrightness;


  if (brightness > 100) {
    brightness = 100;
  }
  
  if ((_brightness > 0) && (brightness == 0)) {

    log_i("DIMMER OFF");

    if (turnOnOff > 0)
      turnOnOff--;
  }
  
  if ((_brightness == 0) && (brightness > 0)) {

    log_i("DIMMER ON");
    
    if (turnOnOff == 0)
      turnOnOff = 1;
    else turnOnOff = 2;
    //_colorBrightness = 0;
  }

  _last_brightness = _brightness;
  _brightness = brightness;

  if ((_whiteTemperature == 0) && (whiteTemperature > 0)) 
    _colorBrightness = 0;

  _last_whiteTemperature = _whiteTemperature;
  _whiteTemperature = whiteTemperature;

  _lastMsgReceivedMs = millis();

  return -1;
}

void Supla::Control::Z2S_RGBCCTInterface::setValueOnServer(
  uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness,
  uint8_t brightness, uint8_t whiteTemperature) {

  _last_red = _red;
  _last_green = _green;
  _last_blue = _blue;
  _last_colorBrightness = _colorBrightness;
  _last_brightness = _brightness;
  _last_whiteTemperature = _whiteTemperature;

  _red = red;
  _green = green;
  _blue = blue;
  _colorBrightness = colorBrightness;
  _brightness = brightness;
  _whiteTemperature = whiteTemperature;
  
  _lastMsgReceivedMs = millis();
}

void Supla::Control::Z2S_RGBCCTInterface::sendValueToDevice(
  uint8_t red, uint8_t green, uint8_t blue, uint8_t colorBrightness,
  uint8_t brightness, uint8_t whiteTemperature) {

  if (Zigbee.started()) {

    log_i("turnOnOff %u", turnOnOff);

    switch (turnOnOff) {


      case 0: {

        zbGateway.sendOnOffCmd(&_device, false);
        return;
      } break;

      
      case 1: {

        zbGateway.sendOnOffCmd(&_device, true);
        //return;
      } break;


      default: break;
    }
    if (colorBrightness > 0) {

      log_i("red %d, green %d, blue %d, color brightness %d",
            red, green, blue, colorBrightness);
      
      ColorHSV hsv;
      ColorRGB rgb;

      uint8_t red_cb = (red*colorBrightness) / 100;
      uint8_t green_cb = (green*colorBrightness) / 100;
      uint8_t blue_cb = (blue*colorBrightness) / 100;

      rgb.m_r = (float)red_cb / 255;
      rgb.m_g = (float)green_cb / 255;
      rgb.m_b = (float)blue_cb / 255;

      hsv = rgbToHsv(rgb);

      uint16_t _hue = hsv.m_h*360;
      uint16_t _hue_360 = _hue;
      uint8_t  _saturation = hsv.m_s*100; //colorBrightness;
      uint16_t  _saturation_1000 = hsv.m_s*1000;

      log_i("hue %d, saturation %d before mapping", _hue, _saturation);
      _hue = map(_hue, 0, 360, 0, 254);
      _saturation = map(_saturation, 0, 100, 0, 254);
      log_i("hue %d, saturation %d after mapping", _hue, _saturation);
      
      uint8_t light_mode = 0x01;

      switch (_rgb_mode) {

        case Z2S_COLOR_HS_RGB:
        case Z2S_PHILIPS_COLOR_HS_RGB:

          zbGateway.sendColorMoveToHueAndSaturationCmd(
            &_device, _hue, _saturation, 1); 
        break;


        case Z2S_COLOR_XY_RGB:
        case Z2S_PHILIPS_COLOR_XY_RGB: {

          espXyColor_t xy_color = espRgbToXYColor(red_cb, green_cb, blue_cb);
          xy_color.x = map(xy_color.x, 0 ,0xFFFF, 0, 0xFEFF);
          xy_color.y = map(xy_color.y, 0 ,0xFFFF, 0, 0xFEFF);

          log_i("XY color mode x:0x%x, y:0x%x", xy_color.x, xy_color.y);
          
          zbGateway.sendColorMoveToColorCmd(
            &_device, xy_color.x, xy_color.y, 1);
        } break;


        case Z2S_TUYA_COLOR_HS_RGB: {
          
          zbGateway.sendCustomClusterCmd(
            &_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, 
            ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &light_mode ,false);

          zbGateway.sendColorMoveToHueAndSaturationCmd(
            &_device, _hue, _saturation, 1);
        } break;


        case Z2S_TUYA_COLOR_XY_RGB: {
          
          zbGateway.sendCustomClusterCmd(
            &_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, 
            ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &light_mode, false);

          espXyColor_t xy_color = espRgbToXYColor(red_cb, green_cb, blue_cb);
          xy_color.x = map(xy_color.x, 0 ,0xFFFF, 0, 0xFEFF);
          xy_color.y = map(xy_color.y, 0 ,0xFFFF, 0, 0xFEFF);

          log_i("Tuya XY color mode x:0x%x, y:0x%x", xy_color.x, xy_color.y);
          
          zbGateway.sendColorMoveToColorCmd(
            &_device, xy_color.x, xy_color.y, 1);
        } break;


        case Z2S_TUYA_DP_COLOR_HS_RGB:{

          rgb.m_r = (float)red / 255;
          rgb.m_g = (float)green / 255;
          rgb.m_b = (float)blue / 255;

          hsv = rgbToHsv(rgb);

          _hue_360 = hsv.m_h*360;
      
          _saturation_1000 = hsv.m_s*1000;

          uint16_t _color_brightness_1000 = map(
            colorBrightness, 0, 100, 0, 1000);

          uint8_t test_buffer[17];

          test_buffer[0] = 00;
          test_buffer[1] = 22;
          test_buffer[2] = TUYA_RGBWCT_LED_EF00_COLOR_DP;
          test_buffer[3] = TUYA_DP_TYPE_RAW;
          test_buffer[4] = 0x00;
          test_buffer[5] = 11;
          test_buffer[6] = 0x00;
          test_buffer[7] = 0x01;//COLOR
          test_buffer[8] = 0x00; //GRADIENT
          test_buffer[9] = 0x14;
          test_buffer[10] = 0x00; //SEGMENT
          test_buffer[11] = (_hue_360 >> 8) & 0x00FF;
          test_buffer[12] = _hue_360 & 0x00FF;//COLOR
          test_buffer[13] = (_saturation_1000 >> 8) & 0x00FF;
          test_buffer[14] = _saturation & 0x00FF;//SATURATION
          test_buffer[15] = (_color_brightness_1000 >> 8) & 0x00FF;
          test_buffer[16] = _color_brightness_1000 & 0x00FF;//BRIGHTNESS

          zbGateway.sendCustomClusterCmd(
            &_device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_REQUEST_CMD, 
            ESP_ZB_ZCL_ATTR_TYPE_SET, 17, test_buffer, false);
        } break;
      }
    }
    
    if (brightness > 0) {

      uint8_t level = mapFloat(brightness, 1, 100, 1, 254);
      zbGateway.sendLevelMoveToLevelCmd(&_device, level, 1);
    }

    if (whiteTemperature > 0) {

      log_i(
        "SUPLA_CHANNELFNC_DIMMER_CCT_AND_RGB - sending white temperature!");

      uint16_t color_temperature = mapFloat(whiteTemperature, 1, 100, 454, 200);
	    zbGateway.sendColorMoveToColorTemperatureCmd(
        &_device, color_temperature, 1);
    }
  }
}


void Supla::Control::Z2S_RGBCCTInterface::ping() {

  if (Zigbee.started()) {
    _fresh_start = false;
    zbGateway.sendAttributeRead(
      &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
      ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
  }
}

void Supla::Control::Z2S_RGBCCTInterface::iterateAlways() {

  if (_lastMsgReceivedMs != 0 && millis() - _lastMsgReceivedMs >= 400) {
    _lastMsgReceivedMs = 0;
    channel.setNewValue(
      _red, _green, _blue, _colorBrightness, _brightness, _whiteTemperature);
    
    sendValueToDevice(
      _red, _green, _blue, _colorBrightness, _brightness, _whiteTemperature);
  }

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (_keep_alive_ms && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (true) {
      
      if (_z2s_zb_device)
        _last_seen_ms = _z2s_zb_device->last_seen_ms;

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
  if (_timeout_ms && channel.isStateOnline() && 
      ((millis() - _last_seen_ms) > _timeout_ms)) {

	  log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);

    if (_z2s_zb_device)
      _last_seen_ms = _z2s_zb_device->last_seen_ms;

    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);

    if ((millis() - _last_seen_ms) > _timeout_ms)
      channel.setStateOffline();
  }
}

void Supla::Control::Z2S_RGBCCTInterface::setRGBMode(uint8_t rgb_mode) {

  _rgb_mode = rgb_mode;
}

uint8_t Supla::Control::Z2S_RGBCCTInterface::getRGBMode() {
  
  return _rgb_mode;
}

void Supla::Control::Z2S_RGBCCTInterface::setKeepAliveSecs(
  uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
}

void Supla::Control::Z2S_RGBCCTInterface::setTimeoutSecs(
  uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;

  if (_timeout_ms == 0) 
    channel.setStateOnline();
}

uint32_t Supla::Control::Z2S_RGBCCTInterface::getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t Supla::Control::Z2S_RGBCCTInterface::getTimeoutSecs() {

  return _timeout_ms / 1000;
}
