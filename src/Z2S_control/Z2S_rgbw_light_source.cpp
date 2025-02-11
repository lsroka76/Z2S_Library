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
extern "C" {
#include <ColorFormat.h>
}

/*uint8_t getHue(uint8_t red, uint8T_green, uint8_t blue){
float _hue, _red, _green, _blue, _cmax, _cmin, _delta;

  _cmin = min({red, green, blue});
  _cmax = max ({red, green, blue});
  _delta = _cmax - _cmin;
  _red = red / 255;
  _green = green / 255;
  _blue = blue/255;
  _cmax = _cmax/255;
  _cmin = _cmin/255;
  _delta = _delta/255;

  
  if _hue = 60*(((_blue - _red) / _delta) + 2);

}*/

Supla::Control::Z2S_RGBWLightSource::Z2S_RGBWLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device)
    : _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Supla::Control::Z2S_RGBWLightSource::onInit() {
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
    
    uint8_t _brightness = map(brightness, 0, 1023, 1, 254);
    log_i("brightness mapping before 0x%x and after 0x%x", brightness, _brightness);
    if (_brightness == 1) turnOff();
    else {
      if (!_state) turnOn();
      uint8_t _red = map(red, 0, 1023, 0, 255);
      uint8_t _green = map(green, 0, 1023, 0, 255);
      uint8_t _blue = map(blue, 0, 1023, 0, 255);

      HsvColor_t HSV_color;
      espXyColor_t XY_color;
      HSV_color = espRgbToHsvColor(_red, _green, _blue);

      //XY_color = espRgbToXYColor(_red, _green, _blue);
      
      log_i("hue %d, saturation %d before mapping", HSV_color.h, HSV_color.s);
      HSV_color.h = map(HSV_color.h, 0, 360, 1, 254);
      HSV_color.s = map(HSV_color.s, 0, 255, 1, 254);
      log_i("hue %d, saturation %d after mapping", HSV_color.h, HSV_color.s);

      //XY_color.x = map(XY_color.x, 0, 0xffff, 0, 0xfeff);
      //XY_color.y = map(XY_color.y, 0, 0xffff, 0, 0xfeff);
      
	if (_Tuya_mode == 0) {
	  _Tuya_mode = 1;
          _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF0, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_Tuya_mode, false);
	}   
	 _gateway->sendColorMoveToHueAndSaturationCmd(&_device, 86/*HSV_color.h*/, HSV_color.s, 1);
         //_write_mask = 0xFF;
	//_gateway->sendAttributeWrite(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF001, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_write_mask);
        //_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xF001, false);
        //_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE100, false);

	//_gateway->sendColorMoveToHueAndSaturationCmd(&_device, hue_st, 100, 1);
        //hue_start = hue_start + 1; if (hue_start > 254) hue_start = 1;

       	  /*_Tuya_dp_data[0] = 0x00;
          _Tuya_dp_data[1] = 0x03;
          _Tuya_dp_data[2] = 0x05;
          _Tuya_dp_data[3] = 0x02;
          _Tuya_dp_data[4] = 0x00;
          _Tuya_dp_data[5] = 0x06;
          _Tuya_dp_data[6] = HSV_color.h & 0xFF;
          _Tuya_dp_data[7] = (HSV_color.h >> 8) & 0xFF;
          _Tuya_dp_data[8] = HSV_color.s & 0xFF;
          _Tuya_dp_data[9] = (HSV_color.s >> 8) & 0xFF;
          _Tuya_dp_data[10] = 0E8;
          _Tuya_dp_data[11] = 0x03;
          
	  //_Tuya_dp_data[0] = 0x00;
          //_Tuya_dp_data[1] = 0x03;
          //_Tuya_dp_data[2] = 0x05;
          //_Tuya_dp_data[3] = 0x02;
          _Tuya_dp_data[0] = 0x00;
          _Tuya_dp_data[1] = 0x06;
          _Tuya_dp_data[2] = 1;//HSV_color.h & 0xFF;
          _Tuya_dp_data[3] = 1;//(HSV_color.h >> 8) & 0xFF;
          _Tuya_dp_data[4] = 1;//HSV_color.s & 0xFF;
          _Tuya_dp_data[5] = 1;//(HSV_color.s >> 8) & 0xFF;
          _Tuya_dp_data[6] = 1;//0E8;
          _Tuya_dp_data[7] = 1;// 0x03;
     
        _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_ARRAY, 8, _Tuya_dp_data, false);


	  _Tuya_dp_data[0] = 0x06;
          _Tuya_dp_data[1] = 0x00;
          _Tuya_dp_data[2] = 1;//HSV_color.h & 0xFF;
          _Tuya_dp_data[3] = 1;//(HSV_color.h >> 8) & 0xFF;
          _Tuya_dp_data[4] = 1;//HSV_color.s & 0xFF;
          _Tuya_dp_data[5] = 1;//(HSV_color.s >> 8) & 0xFF;
          _Tuya_dp_data[6] = 1;//0E8;
          _Tuya_dp_data[7] = 1;// 0x03;
     
        _gateway->sendCustomClusterCmd(&_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_ARRAY, 8, _Tuya_dp_data, false);*/


      //_gateway->sendLevelMoveToLevelCmd(&_device, _brightness, 20);
      //_gateway->sendColorMoveToHueAndSaturationCmd(&_device, HSV_color.h, HSV_color.s, 1);
      //_gateway->sendColorMoveToColorCmd(&_device, XY_color.x, XY_color.y, 10);
      //_gateway->sendColorMoveToSaturationCmd(&_device, HSV_color.s, 20);
	//_gateway->sendColorMoveToColorTemperatureCmd(&_device, 0 + (rand()%1000), 20);
    }  
  }
}


//#endif