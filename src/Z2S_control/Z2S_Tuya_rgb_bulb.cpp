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


#include "Z2S_Tuya_rgb_bulb.h"
#include "rgbhsv.h"
#include <supla/log_wrapper.h>


Supla::Control::Z2S_TuyaRGBBulb::Z2S_TuyaRGBBulb(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t)); 
}

bool Supla::Control::Z2S_TuyaRGBBulb::getTuyaMode() {
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

void Supla::Control::Z2S_TuyaRGBBulb::setTuyaMode(uint8_t mode) {
  if (_gateway && Zigbee.started()) { 
    log_i("setting Tuya mode");		
    _Tuya_bulb_mode = mode;
    _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_bulb_mode, true);
    if (getTuyaMode() != mode)
      log_i("setting Tuya mode failed!");
  } else log_i("Zigbee not started");
}


void Supla::Control::Z2S_TuyaRGBBulb::turnOn() {

  if (_gateway && Zigbee.started()) {
    _state = true;
    log_i("sending ON command");
    _gateway->sendOnOffCmd(&_device, _state);
    log_i("enabling Tuya RGB mode");		
    setTuyaMode(0x01);
    //_Tuya_bulb_mode = 0x01;
    //_gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_bulb_mode, false);   
    //channel.setNewValue(_last_svr_red, _last_svr_green, _last_svr_blue, 100, 0);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  }
}

void Supla::Control::Z2S_TuyaRGBBulb::turnOff() {

  if (_gateway && Zigbee.started()) {   
    _state = false;
    log_i("sending OFF command");
    _gateway->sendOnOffCmd(&_device, _state);
    channel.setNewValue(0, 0, 0, 0, 0);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  }
}

void Supla::Control::Z2S_TuyaRGBBulb::toggle() {
  if (_state) turnOff();
  else turnOn();
}

bool Supla::Control::Z2S_TuyaRGBBulb::isOn() {
  return _state;
}

void Supla::Control::Z2S_TuyaRGBBulb::onInit() {
  
  _last_iterate_ms = 0;
  channel.setOffline();
}

void Supla::Control::Z2S_TuyaRGBBulb::setValueOnServer(uint8_t hue, uint8_t saturation) {

  ColorHSV hsv;
  ColorRGB rgb;

  uint16_t _hue = map(hue, 0, 254, 0, 360);
  uint8_t _saturation = map(saturation, 0, 254, 0, 100);
  
  float _fhue = (float)_hue/360;
  float _fsaturation = (float)_saturation/100;
  
  hsv.m_h = _fhue;
  hsv.m_s = _fsaturation;
  hsv.m_v = 1.0F;

  rgb = hsvToRgb(hsv); 

  uint8_t _red = rgb.m_r*255;
  uint8_t _green = rgb.m_g*255;
  uint8_t _blue = rgb.m_b*255;
  uint8_t _colorBrightness = _saturation;
  
 
  log_i("Hue/Hue %d->%d, Sat/Sat %d->%d", hue, _hue, saturation, _saturation);
  log_i("H:%5.2f\tS:%5.2f\tL:%5.2f\tA:%5.2f", _fhue, _fsaturation, 1.0F, 1.0F);
  log_i("R:%5.2f\tG:%5.2f\tB:%5.2f\tA:%5.2f", rgb.m_r*255, rgb.m_g*255, rgb.m_b*255, 1);
  log_i("R:%d\tG:%d\tB:%d\tA:%d", _red, _green, _blue, 1);

  channel.setNewValue(_red, _green, _blue, _colorBrightness, 0);
}

void Supla::Control::Z2S_TuyaRGBBulb::sendValueToDevice(uint8_t red,
                                 			uint8_t green,
                                 			uint8_t blue,
                                 			uint8_t colorBrightness) {

  if (_gateway && Zigbee.started()) {

    log_i("void Supla::Control::Z2S_TuyaRGBBulb::sendValueToDevice red %d, green %d, blue %d, color brightness %d",
          red, green, blue, colorBrightness);
    
    ColorHSV hsv;
    ColorRGB rgb;

    rgb.m_r = (float)red/255;
    rgb.m_g = (float)green/255;
    rgb.m_b = (float)blue/255;

    hsv = rgbToHsv(rgb);

    uint16_t _hue = hsv.m_h*360;
    uint8_t  _saturation = colorBrightness; //hsv.m_s*100;

    log_i("hue %d, saturation %d before mapping", _hue, _saturation);
    _hue = map(_hue, 0, 360, 0, 254);
    _saturation = map(_saturation, 0, 100, 0, 254);
    log_i("hue %d, saturation %d after mapping", _hue, _saturation);
    uint8_t _brightness = map(colorBrightness, 0, 100, 0, 254);
    if (getTuyaMode()) {
      if (_Tuya_bulb_mode != 0x01)
        setTuyaMode(0x01);
      _gateway->sendColorMoveToHueAndSaturationCmd(&_device, _hue, _saturation, 1);
    }
  }
}

void Supla::Control::Z2S_TuyaRGBBulb::setStateOnServer(bool state) {
  
  if (getTuyaMode())
    channel.setNewValue((_Tuya_bulb_mode == 0x01));
}


bool Supla::Control::Z2S_TuyaRGBBulb::ping() {
  
  if (_gateway && Zigbee.started()) {
    if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true))
      return true;
    else return false;
  } else return false;

}
void Supla::Control::Z2S_TuyaRGBBulb::iterateAlways() {

  if (millis() - _last_iterate_ms >= 10000) {
    
    _last_iterate_ms = millis();

    if (!channel.isOnline()) {  
       if (!ping())
         log_i("Zigbee device(0x%x) offline...", _device.short_addr);
       else {
         channel.setOnline();
         if (getTuyaMode()) 
           channel.setNewValue((_Tuya_bulb_mode == 1));
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
