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


#include "Z2S_Tuya_rgbw_bulb.h"
#include "rgbhsv.h"
#include <supla/log_wrapper.h>


Supla::Control::Z2S_TuyaRGBWBulb::Z2S_TuyaRGBWBulb(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t)); 
}

bool Supla::Control::Z2S_TuyaRGBWBulb::getTuyaMode() {
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

void Supla::Control::Z2S_TuyaRGBWBulb::setTuyaMode(uint8_t mode) {
  if (_gateway && Zigbee.started()) { 
    log_i("setting Tuya mode");		
    _Tuya_bulb_mode = mode;
    _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_bulb_mode, true);
    if (getTuyaMode() != mode)
      log_i("setting Tuya mode failed!");
  } else log_i("Zigbee not started");
}

uint8_t Supla::Control::Z2S_TuyaRGBWBulb::getTuyaBulbState() {

  uint8_t _Tuya_bulb_state = 0;
  bool    _Tuya_on_off_state = 0;

  if (_gateway && Zigbee.started()) {
    log_i("reading Tuya state");
    if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true)) {
      _Tuya_on_off_state = *(bool *)_gateway->getReadAttrLastResult()->data.value;
      if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF000, true))
     	 _Tuya_bulb_mode = *(uint8_t *)_gateway->getReadAttrLastResult()->data.value;
      else {
        log_i("reading Tuya mode failed while reading state!");  
        return 0xFF;
      }    
      if (_Tuya_on_off_state) _Tuya_bulb_state = 0x80 + _Tuya_bulb_mode;
      else _Tuya_bulb_state = _Tuya_bulb_mode;
      log_i("Tuya bulb state is [0%x%]", _Tuya_bulb_state);
      return _Tuya_bulb_state;
    } else log_i("reading Tuya on/off state failed while reading state!");
  } else log_i("Zigbee not started");
  return 0xFF;
}

bool Supla::Control::Z2S_TuyaRGBWBulb::setTuyaBulbState(uint8_t Tuya_bulb_state) {
  
  uint8_t _Tuya_bulb_state;

  if (_gateway && Zigbee.started()) {   
    
    log_i("setting Tuya bulb state to [0%x%]", Tuya_bulb_state);
    if ((Tuya_bulb_state & 0x80) == 0x80) 
      _gateway->sendOnOffCmd(&_device, true);
    else
      _gateway->sendOnOffCmd(&_device, false); 
    _Tuya_bulb_mode = (Tuya_bulb_state & 0x01);
    _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_bulb_mode, true);
    if (getTuyaMode() != _Tuya_bulb_mode) {
      log_i("setting Tuya mode failed while setting state!");
      return false;
    }   
  } else log_i("Zigbee not started");   
  return false;
}

void Supla::Control::Z2S_TuyaRGBWBulb::turnOn() {

  if (_gateway && Zigbee.started()) {
     _state = true;
     log_i("sending ON command");
    _gateway->sendOnOffCmd(&_device, _state);
        
    channel.setNewValue(_last_red, _last_green, _last_blue, (_Tuya_bulb_mode == 1) ? 100 : 0, (_Tuya_bulb_mode == 0) ? 100 : 0);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  } else log_i("Zigbee not started");
}

void Supla::Control::Z2S_TuyaRGBWBulb::turnOff() {

  if (_gateway && Zigbee.started()) {   
    _state = false;
    log_i("sending OFF command");
    _gateway->sendOnOffCmd(&_device, _state);
    //channel.setNewValue(-1, -1, -1, 0, 0);
    // Schedule save in 5 s after state change
    //Supla::Storage::ScheduleSave(5000);
  } else log_i("Zigbee not started");
}

void Supla::Control::Z2S_TuyaRGBWBulb::toggle() {
  if (_state) turnOff();
  else turnOn();
}

bool Supla::Control::Z2S_TuyaRGBWBulb::isOn() {
  return _state;
}

void Supla::Control::Z2S_TuyaRGBWBulb::turnOnDimmer() {
  
  setTuyaMode(TUYA_RGBW_MODE_DIMMER);
}
void Supla::Control::Z2S_TuyaRGBWBulb::turnOffDimmer() {

}

void Supla::Control::Z2S_TuyaRGBWBulb::turnOnRGB() {

  setTuyaMode(TUYA_RGBW_MODE_RGB);
}

void Supla::Control::Z2S_TuyaRGBWBulb::turnOffRGB() {}

void Supla::Control::Z2S_TuyaRGBWBulb::toggleDimmerRGB() {}

bool Supla::Control::Z2S_TuyaRGBWBulb::isDimmerOn() {

  if (_Tuya_bulb_mode == TUYA_RGBW_MODE_DIMMER)
    return true;
  else 
    return false;
}

bool Supla::Control::Z2S_TuyaRGBWBulb::isRGBOn() {

  if (_Tuya_bulb_mode == TUYA_RGBW_MODE_RGB)
    return true;
  else 
    return false;
}
 
int32_t Supla::Control::Z2S_TuyaRGBWBulb::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) {
  uint8_t command = static_cast<uint8_t>(newValue->value[6]);
  uint8_t toggleOnOff = static_cast<uint8_t>(newValue->value[5]);
  uint8_t red = static_cast<uint8_t>(newValue->value[4]);
  uint8_t green = static_cast<uint8_t>(newValue->value[3]);
  uint8_t blue = static_cast<uint8_t>(newValue->value[2]);
  uint8_t colorBrightness = static_cast<uint8_t>(newValue->value[1]);
  uint8_t brightness = static_cast<uint8_t>(newValue->value[0]);

  SUPLA_LOG_DEBUG(
      "Z2S_RGBWBase[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d, "
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
  
  /*if (toggleOnOff == 1) {
      
    uint8_t _Tuya_bulb_state = getTuyaBulbState();
    
    if (_Tuya_bulb_state == 0xFF) {
      //toggle_offline = true; 
      channel.setOffline();
      return -1;
    }
    else
      if ((brightness == 0) && (colorBrightness == 0) && (_Tuya_bulb_state >= 0x80))
        turnOff();
      else if ((brightness == 100) && (_Tuya_bulb_state != 0x80))
	setTuyaBulbState(0x80);
      else if ((colorBrightness == 100) && (_Tuya_bulb_state != 0x81))
	setTuyaBulbState(0x81);         
  }*/
  
  uint8_t _Tuya_bulb_state = getTuyaBulbState();
    
  if (_Tuya_bulb_state == 0xFF) {
      //toggle_offline = true; 
    channel.setOffline();
    return -1;
  }

  if ((brightness == 0) && (colorBrightness == 0) && (toggleOnOff == 1)) {
    _last_svr_red = red;
    _last_svr_green = green;
    _last_svr_blue = blue;
    _last_svr_colorBrightness = colorBrightness;
    _last_svr_brightness = brightness;

    _last_red = red;
    _last_green = green;
    _last_blue = blue;
    _last_colorBrightness = colorBrightness;
    _last_brightness = brightness;

    _lastMsgReceivedMs = 1;

    turnOff();
    return -1;
  }
    
  /*if ((brightness != _last_svr_brightness) && (_Tuya_bulb_state != 0x80)) //((_Tuya_bulb_state & 0x01) == 1))
    setTuyaBulbState(0x80);
  else if (((colorBrightness != _last_svr_colorBrightness) || (red != _last_svr_red) || (green != _last_svr_green) || (blue != _last_svr_blue)) && (_Tuya_bulb_state != 0x81)) //&& ((_Tuya_bulb_state & 0x01) == 0))
    setTuyaBulbState(0x81);*/

if ((brightness != _last_svr_brightness) && (_Tuya_bulb_state != 0x81)) //((_Tuya_bulb_state & 0x01) == 1))
    setTuyaBulbState(0x81);
  
	
  _last_svr_red = red;
  _last_svr_green = green;
  _last_svr_blue = blue;
  _last_svr_colorBrightness = colorBrightness;
  _last_svr_brightness = brightness;

  _last_red = red;
  _last_green = green;
  _last_blue = blue;
  _last_colorBrightness = colorBrightness;
  _last_brightness = brightness;

  sendValueToDevice(red, green, blue, colorBrightness, brightness);
  return -1;
}


void Supla::Control::Z2S_TuyaRGBWBulb::setValueOnServer(uint8_t hue, uint8_t saturation, uint16_t level) {

  ColorHSV hsv;
  ColorRGB rgb;
  uint16_t _hue;
  uint8_t _saturation;
  uint8_t _level;

  if (hue == 0xFF)
    _hue = _last_hue;
  else
    _hue = map(hue, 0, 254, 0, 360);
  
  if (saturation == 0xFF)
    _hue = _last_saturation;
  else
    _hue = map(saturation, 0, 254, 0, 100);

  if (level == 0xFFFF)
    _level = _last_level;
  else _level = map(level, 1, 254, 0, 100);
  
  float _fhue = (float)_hue/360;
  float _fsaturation = (float)_saturation/100;
  
  hsv.m_h = _fhue;
  hsv.m_s = _fsaturation;
  hsv.m_v = 1.0F;

  rgb = hsvToRgb(hsv); 

  uint8_t _red = rgb.m_r*255;
  uint8_t _green = rgb.m_g*255;
  uint8_t _blue = rgb.m_b*255;
  uint8_t _colorBrightness = _last_colorBrightness;
  
 
  log_i("Hue/Hue %d->%d, Sat/Sat %d->%d", hue, _hue, saturation, _saturation);
  log_i("H:%5.2f\tS:%5.2f\tL:%5.2f\tA:%5.2f", _fhue, _fsaturation, 1.0F, 1.0F);
  log_i("R:%5.2f\tG:%5.2f\tB:%5.2f\tA:%5.2f", rgb.m_r*255, rgb.m_g*255, rgb.m_b*255, 1);
  log_i("R:%d\tG:%d\tB:%d\tA:%d", _red, _green, _blue, 1);

  _last_red   = _red;
  _last_green = _green;
  _last_blue  = _blue;
  _last_colorBrightness = _colorBrightness;
  _last_brightness = _level;

  _lastMsgReceivedMs = millis();

  //channel.setNewValue(_red, _green, _blue, _colorBrightness, 0);
}

void Supla::Control::Z2S_TuyaRGBWBulb::sendValueToDevice(uint8_t red,
                                 			uint8_t green,
                                 			uint8_t blue,
                                 			uint8_t colorBrightness,
							uint8_t brightness) {

  if (_gateway && Zigbee.started()) {

    log_i("void Supla::Control::Z2S_TuyaRGBWBulb::sendValueToDevice red %d, green %d, blue %d, color brightness %d, brightness %d",
          red, green, blue, colorBrightness, brightness);
    
    ColorHSV hsv;
    ColorRGB rgb;

    rgb.m_r = (float)red/255;
    rgb.m_g = (float)green/255;
    rgb.m_b = (float)blue/255;

    hsv = rgbToHsv(rgb);

    uint16_t _hue = hsv.m_h*360;
    uint8_t  _saturation = hsv.m_s*100;

    _last_hue = _hue;
    _last_saturation = _saturation;
    _last_level = brightness;

    log_i("hue %d, saturation %d before mapping", _hue, _saturation);
    _hue = map(brightness, 0, 100, 0, 254); //map(_hue, 0, 360, 0, 254);
    _saturation = 254; //map(_saturation, 0, 100, 0, 254);
    log_i("hue %d, saturation %d after mapping", _hue, _saturation);
    uint8_t _colorBrightness = map(colorBrightness, 0, 100, 1, 254);
    uint8_t _level = map(brightness, 0, 100, 1, 254);

    int8_t _Tuya_bulb_state = getTuyaBulbState();
    if ((_Tuya_bulb_state & 0x01) == 0x00) 
      _gateway->sendLevelMoveToLevelCmd(&_device, _level, 1);
    else if ((_Tuya_bulb_state & 0x01) == 0x01)
     _gateway->sendColorMoveToHueAndSaturationCmd(&_device, _hue, _saturation, 1);
  } else log_i("Zigbee not started");
}

void Supla::Control::Z2S_TuyaRGBWBulb::onInit(){

  _last_red   = 0;
  _last_green = 0;
  _last_blue  = 0;
  _last_colorBrightness = 0;
  _last_brightness = 0;  
  _lastMsgReceivedMs = 1;
  _zed_offline = true;
  channel.setOffline();
}

void Supla::Control::Z2S_TuyaRGBWBulb::iterateAlways() {
  
  uint8_t _Tuya_bulb_state;

  if (_lastMsgReceivedMs != 0 && millis() - _lastMsgReceivedMs >= 1000) {
    
    _lastMsgReceivedMs = millis();

    //if (_toggle_offline) {
     // channel.setOffline();
     // return;
    //}
    if (!channel.isOnline()) {
       _Tuya_bulb_state = getTuyaBulbState();    
       if (_Tuya_bulb_state == 0xFF)
         log_i("Zigbee device(0x%x) offline...", _device.short_addr);
       else {
         channel.setOnline();
         channel.setNewValue(_last_red, _last_green, _last_blue, (_Tuya_bulb_state == 0x81) ? 100 : 0, (_Tuya_bulb_state == 0x80) ? 100 : 0);
       }
    } else channel.setNewValue(_last_red, _last_green, _last_blue, _last_colorBrightness, _last_brightness);
  } //time loop
} //iterateAlways()
