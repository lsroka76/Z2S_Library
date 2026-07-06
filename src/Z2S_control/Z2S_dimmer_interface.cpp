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
#include "rgbhsv.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

Supla::Control::Z2S_DimmerInterface::Z2S_DimmerInterface(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t dimmer_mode) 
  : /*_gateway(gateway),*/ _dimmer_mode(dimmer_mode) {
  
  if (device)
    memcpy(&_device, device, sizeof(zbg_device_params_t));  
  else   
    memset(&_device, 0, sizeof(zbg_device_params_t));  

  channel.setType(SUPLA_CHANNELTYPE_DIMMER);
  /*channel.setFuncList(
      SUPLA_RGBW_BIT_FUNC_DIMMER | SUPLA_RGBW_BIT_FUNC_RGB_LIGHTING |
      SUPLA_RGBW_BIT_FUNC_DIMMER_AND_RGB_LIGHTING |
      SUPLA_RGBW_BIT_FUNC_DIMMER_CCT | SUPLA_RGBW_BIT_FUNC_DIMMER_CCT_AND_RGB);*/
  //channel.setDefault(SUPLA_RGBW_BIT_FUNC_DIMMER);
  channel.setDefault(SUPLA_CHANNELFNC_DIMMER);
  _cct_mode = _dimmer_mode;
  //channel.setFlag(SUPLA_CHANNEL_FLAG_RGBW_COMMANDS_SUPPORTED);
}

Supla::Control::Z2S_DimmerInterface::Z2S_DimmerInterface(
  zbg_device_params_t *device, uint8_t dimmer_function, uint8_t dimmer_mode,
  uint8_t cct_mode) : _dimmer_function(dimmer_function), 
  _dimmer_mode(dimmer_mode), _cct_mode(cct_mode) {
  
  if (device)
    memcpy(&_device, device, sizeof(zbg_device_params_t));  
  else   
    memset(&_device, 0, sizeof(zbg_device_params_t));  

  channel.setType(SUPLA_CHANNELTYPE_DIMMER);
  channel.setDefault(dimmer_function);
}

void Supla::Control::Z2S_DimmerInterface::onLoadState() {

    Supla::Storage::ReadState(
      (unsigned char *)&_brightness, sizeof(_brightness));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_brightness, sizeof(_last_brightness));

    Supla::Storage::ReadState(
      (unsigned char *)&_whiteTemperature, sizeof(_whiteTemperature));
    Supla::Storage::ReadState(
      (unsigned char *)&_last_whiteTemperature, 
      sizeof(_last_whiteTemperature));
}

  void Supla::Control::Z2S_DimmerInterface::onSaveState() {

    Supla::Storage::WriteState(
      (unsigned char *)&_brightness, sizeof(_brightness));
    Supla::Storage::WriteState(
      (unsigned char *)&_last_brightness, sizeof(_last_brightness));

    Supla::Storage::WriteState(
      (unsigned char *)&_last_whiteTemperature, sizeof(_last_whiteTemperature));
  }

int32_t Supla::Control::Z2S_DimmerInterface::handleNewValueFromServer(
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
      "Z2S_DimmerInterface[%d]: red=%d, green=%d, blue=%d, colorBrightness=%d"
      ", brightness=%d, whiteTemperature=%d, command=%d, toggleOnOff=%d",
      getChannelNumber(), red, green, blue, colorBrightness, brightness,
      whiteTemperature, command, toggleOnOff);

  if (brightness > 100) {
    brightness = 100;
  }
  
  if (brightness != _brightness) {
    if (brightness == 0) {
      
      sendTurnOnOffCmd = 1;
      _last_brightness = _brightness;
      _brightness = brightness;

      log_i(
        "DIMMER SVR OFF: _last_brightness = %u, _brightness = %u", 
        _last_brightness, _brightness);
    }
    else {
      if (_brightness == 0) {

        log_i(
          "DIMMER SVR ON: _last_brightness = %u, _brightness = %u", 
          _last_brightness, _brightness);

        
        if (brightness == 100) {

          if (_last_brightness > 100)
            _last_brightness = 100;
          _brightness = _last_brightness;
        }
        else {
          
          _last_brightness = _brightness;
          _brightness = brightness;
        }
        sendTurnOnOffCmd = 2;
      }
      else {

        _last_brightness = _brightness;
        _brightness = brightness;
        //sendTurnOnOffCmd = 0;

        log_i(
          "DIMMER SVR MSG: _last_brightness = %u, _brightness = %u", 
          _last_brightness, _brightness);
      }
    }
  }

  _last_whiteTemperature = _whiteTemperature;
  _whiteTemperature = whiteTemperature;

  _lastMsgReceivedMs = millis();
  Supla::Storage::ScheduleSave(5000, 2000);

  return -1;
}

void Supla::Control::Z2S_DimmerInterface::turnOff() {

  sendTurnOnOffCmd = 1;
  _last_brightness = _brightness;
  _brightness = 0;

  _lastMsgReceivedMs = 1; //millis();
}

void Supla::Control::Z2S_DimmerInterface::turnOn() {

  if (_last_brightness > 0)
    _brightness = _last_brightness;
  else {
    _last_brightness = _brightness;
    _brightness = 100;
  }
  sendTurnOnOffCmd = 2;

  _lastMsgReceivedMs = millis();
}


void Supla::Control::Z2S_DimmerInterface::sendValueToDimmer(
  uint8_t brightness) {

  log_i("_dimmer_mode = %u, brightness = %u", _dimmer_mode, brightness);
  log_i(
    "_brightness = %u, _last_brightness = %u",_brightness, _last_brightness);
  log_i(
    "_whiteTemperature = %u, _last_whiteTemperature = %u",_whiteTemperature, 
    _last_whiteTemperature);
  

  if (Zigbee.started()) {
    switch (_dimmer_mode) {


      case Z2S_SEND_TO_LEVEL_DIMMER: {

        if (sendTurnOnOffCmd == 1) {

          _state = DIMMER_STATE_OFF;
          zbGateway.sendOnOffCmd(&_device, false);
          zbGateway.sendAttributeRead(
            &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
            ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);

          sendTurnOnOffCmd = 0;
          break;
        }

        if (sendTurnOnOffCmd == 2) {

          _state = DIMMER_STATE_ON;
          zbGateway.sendOnOffCmd(&_device, true);
          zbGateway.sendAttributeRead(
            &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
            ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);          
          sendTurnOnOffCmd = 0;
        }
        
        uint8_t level = mapFloat(brightness, 1, 100, 1, 254);
        zbGateway.sendLevelMoveToLevelCmd(&_device, level, 1);
        zbGateway.sendAttributeRead(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 
          ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, false);

      } break;


      case Z2S_TUYA_F0_CMD_DIMMER: {

        if (sendTurnOnOffCmd == 1) {

          _state = DIMMER_STATE_OFF;
		      sendTurnOnOffCmd = 0;
          zbGateway.sendOnOffCmd(&_device, false);
          break;
        }

        if (sendTurnOnOffCmd == 2) {

          _state = DIMMER_STATE_ON;
		      sendTurnOnOffCmd = 0;
          zbGateway.sendOnOffCmd(&_device, true);
        }

        uint16_t F0_brightness = mapFloat(brightness, 1, 100, 1, 1000);
	      
        zbGateway.sendCustomClusterCmd(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 0xF0, 
          ESP_ZB_ZCL_ATTR_TYPE_U16, 2, (uint8_t *)&F0_brightness, 
          false);
      } break;


      case Z2S_TUYA_BRIGHTNESS_DP_DIMMER: {

        if (sendTurnOnOffCmd == 1) {

          _state = DIMMER_STATE_OFF;
		      sendTurnOnOffCmd = 0;
          sendTuyaRequestCmdBool(
            &zbGateway, &_device, TUYA_RGBWCT_LED_EF00_SWITCH_DP, 0);
          break;
        }

        if (sendTurnOnOffCmd == 2) {

          _state = DIMMER_STATE_ON;
		      sendTurnOnOffCmd = 0;
          sendTuyaRequestCmdBool(
            &zbGateway, &_device, TUYA_RGBWCT_LED_EF00_SWITCH_DP, 1);
        }

        uint16_t dp_brightness = mapFloat(brightness, 1, 100, 1, 1000);
        
        //WHITE mode
        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, TUYA_RGBWCT_LED_EF00_MODE_DP, 0); 

        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, TUYA_RGBWCT_LED_EF00_BRIGHTNESS_DP, 
          dp_brightness, false);
      } break;
    } 
  }
}

void Supla::Control::Z2S_DimmerInterface::sendValueToCCT(
  uint8_t whiteTemperature) {

  log_i("_cct_mode = %u, whiteTemperature = %u", _cct_mode, whiteTemperature);
  log_i(
    "_brightness = %u, _last_brightness = %u",_brightness, _last_brightness);
  log_i(
    "_whiteTemperature = %u, _last_whiteTemperature = %u",_whiteTemperature, 
    _last_whiteTemperature);
  
  if (Zigbee.started()) {
    switch (_cct_mode) {


      case Z2S_COLOR_TEMPERATURE_DIMMER: {

        uint16_t color_temperature = mapFloat(
          whiteTemperature, 0, 100, 500, 158); //454, 200);

	      zbGateway.sendColorMoveToColorTemperatureCmd(
          &_device, color_temperature, 1);
        
        zbGateway.sendAttributeRead(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
          ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID, false);
      } break;


      case Z2S_PHILIPS_COLOR_TEMPERATURE_DIMMER: {

        uint16_t color_temperature = mapFloat(
          whiteTemperature, 0, 100, 500, 153);
	      
        zbGateway.sendColorMoveToColorTemperatureCmd(
          &_device, color_temperature, 1);

        zbGateway.sendAttributeRead(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
          ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID, false);
      } break;


      case Z2S_TUYA_COLOR_TEMPERATURE_DIMMER:
      case Z2S_TUYA_E0_CMD_DIMMER: {

        uint16_t E0_color_temperature = mapFloat(
          whiteTemperature, 0, 100, 0, 1000);
	      
        zbGateway.sendCustomClusterCmd(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE0, 
          ESP_ZB_ZCL_ATTR_TYPE_U16, 2, (uint8_t *)&E0_color_temperature, 
          false);
      } break;


      case Z2S_TUYA_COLOR_TEMPERATURE_DP_DIMMER: {

        uint16_t dp_color_temperature = mapFloat(
          whiteTemperature, 0, 100, 1, 1000);

        //WHITE mode
        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, TUYA_RGBWCT_LED_EF00_MODE_DP, 0);
          
        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, TUYA_RGBWCT_LED_EF00_COLOR_TEMPERATURE_DP, 
          dp_color_temperature, false);
      } break;
    }
  }
}

void Supla::Control::Z2S_DimmerInterface::setValueOnServer(
  int16_t value, bool new_state, DimmerMessage dimmer_msg) {

  log_i(
    "value =%d, new_state = %u, dimmer_msg = %u, _state = %u", value, 
    new_state, dimmer_msg, _state);
  log_i(
    "_brightness = %u, _last_brightness = %u",_brightness, _last_brightness);
  log_i(
    "_whiteTemperature = %u, _last_whiteTemperature = %u",_whiteTemperature, 
    _last_whiteTemperature);

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();

    
  if (value < 0) {

    if (_state == DIMMER_STATE_UNKNOWN) {

      if (new_state)
        _state = DIMMER_STATE_ON;
      else
      _state = DIMMER_STATE_OFF;
      return;
    }

    if ((new_state) && (_state == DIMMER_STATE_OFF) && 
        (_last_brightness > 0)) {

      if (_last_brightness > 100)
        _last_brightness = 100;
        _brightness = _last_brightness;
        _state = DIMMER_STATE_ON;
        _lastDeviceMsgReceivedMs = millis();
        //_lastMsgReceivedMs = millis();
      return;
    }

    if ((!new_state) && (_state == DIMMER_STATE_ON)) {

      if (_brightness > 0)
        _last_brightness = _brightness;
      _brightness = 0;
      _state = DIMMER_STATE_OFF;
      _lastDeviceMsgReceivedMs = millis();
      //_lastMsgReceivedMs = millis();
      return;
    }
    return;

    if ((!new_state) && (_state != DIMMER_STATE_OFF)) {

      //if (_brightness > 0)
      //  _last_brightness = _brightness;
      //_brightness = 0;
      _state = DIMMER_STATE_OFF;
      //_lastDeviceMsgReceivedMs = millis();
      //channel.setNewValue(0, 0, 0, 0, _brightness, _whiteTemperature);

      return;
    }

    if (new_state && (_state != DIMMER_STATE_ON)) {

      //if (_last_brightness > 100)
      //  _last_brightness = 100;
      //_brightness = _last_brightness;
      _state = DIMMER_STATE_ON  ;
      //_lastDeviceMsgReceivedMs = millis();
      //channel.setNewValue(0, 0, 0, 0, _brightness, _whiteTemperature);
      return;
    }
    return;
  }

  uint8_t sent_brightness = 0;

  if ((dimmer_msg == COLOR_TEMPERATURE_MSG) || (dimmer_msg == E000_CCT_MSG)) {

    _last_whiteTemperature = _whiteTemperature;
	  
	  switch (_cct_mode) {

      
      case Z2S_COLOR_TEMPERATURE_DIMMER: 

        _whiteTemperature = mapFloat(value, /*454*/500, /*200*/158, 0, 100); 
      break;


      case Z2S_TUYA_COLOR_TEMPERATURE_DIMMER: 

        if (dimmer_msg == E000_CCT_MSG)
          _whiteTemperature = mapFloat(value, 0, 1000, 0, 100); 
      break;


      case Z2S_PHILIPS_COLOR_TEMPERATURE_DIMMER:

        _whiteTemperature = mapFloat(value, 500, 153, 0, 100); 
      break;


      case Z2S_TUYA_E0_CMD_DIMMER:
      case Z2S_TUYA_COLOR_TEMPERATURE_DP_DIMMER: //TODO
      break;
    }
    _deviceWhiteTemperature = value;
	  
    if ((dimmer_msg == COLOR_TEMPERATURE_MSG) && (value == 0)) {

      log_i("device sent color temperature with zero value!");
      /*if (_last_whiteTemperature > 100)
        _last_whiteTemperature = 0;
      _whiteTemperature =  _last_whiteTemperature;*/
      turnOff();
      //_lastMsgReceivedMs = millis();
    } 
  }
  else {

    switch (_dimmer_mode) {


      case Z2S_SEND_TO_LEVEL_DIMMER:  
        
        sent_brightness = mapFloat(value, 1, 254, 1, 100); 
      break;


      case Z2S_COLOR_TEMPERATURE_DIMMER: 

        //_brightness = mapFloat(value, 250, 454, 0, 100); 
        sent_brightness = mapFloat(value, /*454*/500, /*200*/158, 0, 100); 
      break;


      case Z2S_TUYA_COLOR_TEMPERATURE_DIMMER: 
      case Z2S_PHILIPS_COLOR_TEMPERATURE_DIMMER:

        sent_brightness = mapFloat(value, 500, 153, 0, 100); 
      break;
    }
    _deviceBrightness = value;

    if ((!_state) || (value == 0)) {

      log_i(
        "_last_brightness %u, sent_brightness = %u", _last_brightness, 
        sent_brightness);

      _last_brightness = sent_brightness;
    }
    else {

      log_i(
        "_brightness %u,_last_brightness %u, sent_brightness = %u", 
        _brightness, _last_brightness, sent_brightness);

      _last_brightness = _brightness;
      _brightness = sent_brightness;
    }
  }
  //channel.setNewValue(0, 0, 0, 0, _brightness, _whiteTemperature);
  _lastDeviceMsgReceivedMs = millis();
}

void Supla::Control::Z2S_DimmerInterface::syncDevice() {

  if (Zigbee.started()) {

    uint8_t sync_counter = 0;
    
    if (_state == DIMMER_STATE_UNKNOWN) {
      sendTurnOnOffCmd = (_brightness == 0) ? 1 : 2;
      if (_brightness == 0)
        sendValueToDimmer(_brightness);
      else
        sendValueToDimmer(_brightness);

      return;

    }
    else
      sync_counter++;

    if (_deviceBrightness == 0xFF) {

      if (_brightness == 0)
        //sendValueToDimmer(_last_brightness);
      else
        sendValueToDimmer(_brightness);
    }
    else
      sync_counter++;

    if (_deviceWhiteTemperature == 0xFFFF) {

      if (_brightness != 0)
        sendValueToCCT(_whiteTemperature);
    }
    else
      sync_counter++;

    if (sync_counter == 3)
      _fresh_start = false;

    log_i("sync counter %u", sync_counter);
  }
}

void Supla::Control::Z2S_DimmerInterface::ping() {

  if (Zigbee.started()) {

    uint8_t ping_counter = 0;
    
    if (!_fresh_start || (_state == DIMMER_STATE_UNKNOWN)) 
      zbGateway.sendAttributeRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
        ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
    else
      ping_counter++;

    if (!_fresh_start || (_deviceBrightness == 0xFF))
      zbGateway.sendAttributeRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL, 
        ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, false);
    else
      ping_counter++;

    if (!_fresh_start || (_deviceWhiteTemperature == 0xFFFF))
      zbGateway.sendAttributeRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 
        ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID, false);
    else
      ping_counter++;

    //if ((_last_whiteTemperature != 0xFF) && (_last_brightness != 0xFF))
    if (ping_counter == 3)
      _fresh_start = false;
  }
}

void Supla::Control::Z2S_DimmerInterface::iterateAlways() {

  if (_fresh_start && ((millis() - _last_sync_ms) > 5000)) {

    _last_sync_ms = millis();
    syncDevice();
    return;
  }

  if (_lastMsgReceivedMs != 0 && millis() - _lastMsgReceivedMs >= 400) {

    _lastMsgReceivedMs = 0;
    if (channel.getDefaultFunction() == SUPLA_CHANNELFNC_DIMMER_CCT) {
      log_i("SUPLA_CHANNELFNC_DIMMER_CCT");
      //channel.setNewValue(0, 0, 0, 0, _brightness, _whiteTemperature);
      if ((sendTurnOnOffCmd == 0) && (_brightness == 0))
      ;//skip
      else
        sendValueToDimmer(_brightness);

      //if ((sendTurnOnOffCmd == 0) && (_whiteTemperature == 0))
      ;//skip
      //else
      if (_state)
        sendValueToCCT(_whiteTemperature);
    }
    else {
      //channel.setNewValue(0, 0, 0, 0, _brightness, 0);
      log_i("SUPLA_CHANNELFNC_DIMMER");
      sendValueToDimmer(_brightness);
      sendValueToCCT(_brightness);
    }
  }

  if (_lastDeviceMsgReceivedMs && millis() - _lastDeviceMsgReceivedMs >= 400) {

    _lastDeviceMsgReceivedMs = 0;
    channel.setNewValue(0, 0, 0, 0, _brightness, _whiteTemperature);
    Supla::Storage::ScheduleSave(5000, 2000);
  }

  /*if (_fresh_start && ((millis() - _last_ping_ms) > 5000)) {

    _last_ping_ms = millis();
    ping();
  }*/

  if (_keep_alive_ms && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (true) {
      
      
      _last_seen_ms = getZbDeviceLastSeenMs();

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

    
    _last_seen_ms = getZbDeviceLastSeenMs();

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
}

void Supla::Control::Z2S_DimmerInterface::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) 
    channel.setStateOnline();
}

uint32_t Supla::Control::Z2S_DimmerInterface::getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t Supla::Control::Z2S_DimmerInterface::getTimeoutSecs() {

  return _timeout_ms / 1000;
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


    case TURN_ON:

      if (!isOn())
        turnOn();
     break;


    case TURN_OFF:

      if (isOn())
        turnOff();
     break;


    case DIM_W:

      increaseBrightness(-10);
    break;


    case BRIGHTEN_W: 
      
      increaseBrightness(10);
    break;
  }
}
