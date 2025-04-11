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

#include "Z2S_trv_interface.h"

Supla::Control::Z2S_TRVInterface::Z2S_TRVInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_commands_set, uint8_t trv_thermometer_channel_no) 
                                  : _gateway(gateway), _trv_commands_set(trv_commands_set), _trv_thermometer_channel_no(trv_thermometer_channel_no) {

    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

Supla::Control::HvacBaseEE *Supla::Control::Z2S_TRVInterface::getTRVHvac(){

  return _trv_hvac;
}

void Supla::Control::Z2S_TRVInterface::setTRVHvac(Supla::Control::HvacBaseEE * trv_hvac) {

  _trv_hvac = trv_hvac;
}


int Supla::Control::Z2S_TRVInterface::getOutputValue() const {

  return _trv_state;
}

void Supla::Control::Z2S_TRVInterface::sendOnOff(bool state) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendOnOff to %d", state);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    switch(_trv_commands_set) {
      case 1: {
        _Tuya_dp_data[2] = 0x65; 
        _Tuya_dp_data[3] = 0x01;
      } break;
      case 2: {
        _Tuya_dp_data[2] = 0x02; 
        _Tuya_dp_data[3] = 0x04;
      } break;
      case 3: {
        _Tuya_dp_data[2] = 0x01; 
        _Tuya_dp_data[3] = 0x04;
      } break;
    }
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x01;
    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    delay(200);
    
    _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    switch(_trv_commands_set) {
      case 1: _Tuya_dp_data[2] = 0x67; break;
      case 2: _Tuya_dp_data[2] = 0x04; break;
      case 3: _Tuya_dp_data[2] = 0x02; break;
    }
    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = 0x00;
    _Tuya_dp_data[7] = 0x00;
    if (state) {
      _Tuya_dp_data[8] = 0x01;
      _Tuya_dp_data[9] = 0x2C;
    } else {
      _Tuya_dp_data[8] = 0x00;
      _Tuya_dp_data[9] = 0x32;
    }
    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureSetpoint(int16_t temperature_setpoint) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVTemperatureSetpoint = %d", temperature_setpoint);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);

    switch(_trv_commands_set) {
      case 1: _Tuya_dp_data[2] = 0x67; break;
      case 2: _Tuya_dp_data[2] = 0x04; break;
      case 3: _Tuya_dp_data[2] = 0x02; break;
    }
    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = 0x00;
    _Tuya_dp_data[7] = 0x00;
    _Tuya_dp_data[8] = temperature_setpoint / 256;
    _Tuya_dp_data[9] = temperature_setpoint % 256;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureCalibration(int32_t temperature_calibration) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVTemperatureCorrection = %d", temperature_calibration);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);

    switch(_trv_commands_set) {
      case 1: _Tuya_dp_data[2] = 0x1B; break;
      case 2: _Tuya_dp_data[2] = 0x2F; break;
      case 3: _Tuya_dp_data[2] = 0x2C; break;
    }
    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = (temperature_calibration & 0xFF000000) >> 24;
    _Tuya_dp_data[7] = (temperature_calibration & 0x00FF0000) >> 16;
    _Tuya_dp_data[8] = (temperature_calibration & 0x0000FF00) >> 8;
    _Tuya_dp_data[9] = (temperature_calibration & 0x000000FF);

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVSystemMode(uint8_t trv_system_mode) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVMode = %d", trv_system_mode);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    switch(_trv_commands_set) {
      case 1: {
        _Tuya_dp_data[2] = 0x65; 
        _Tuya_dp_data[3] = 0x01;
        _Tuya_dp_data[6] = trv_system_mode;

      } break;
      case 2: {
        _Tuya_dp_data[2] = 0x02; 
        _Tuya_dp_data[3] = 0x04;
        _Tuya_dp_data[6] = trv_system_mode == 0 ? 2 : 1;
    
      } break;
      case 3: {
        _Tuya_dp_data[2] = 0x01; 
        _Tuya_dp_data[3] = 0x04;
        _Tuya_dp_data[6] = trv_system_mode;
      } break;
    }
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    
    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureSetpoint(int16_t trv_temperature_setpoint) {

  _trv_temperature_setpoint = trv_temperature_setpoint;
}

void Supla::Control::Z2S_TRVInterface::setTRVSystemMode(uint8_t trv_system_mode) {

  _trv_system_mode = trv_system_mode;
}

void Supla::Control::Z2S_TRVInterface::setTRVRunningState(uint8_t trv_running_state) {

  _trv_running_state = trv_running_state;
}

void Supla::Control::Z2S_TRVInterface::setTRVLocalTemperature(int16_t trv_local_temperature) {
  
  _trv_local_temperature = trv_local_temperature;
}

void Supla::Control::Z2S_TRVInterface::setOutputValue(int value) {

  if (_output_enabled) {
    _trv_state = value;
    sendOnOff((value == 1));
  }
  else
  {
    log_i("Z2S_TRVInterface output disabled");
  }
}

bool Supla::Control::Z2S_TRVInterface::isOnOffOnly() const {
  return true;
}

bool Supla::Control::Z2S_TRVInterface::isOutputEnabled() {

  return _output_enabled;
}

void Supla::Control::Z2S_TRVInterface::setOutputEnabled(bool output_enabled) {

  _output_enabled = output_enabled;
}


void Supla::Control::Z2S_TRVInterface::iterateAlways() {

  if (millis() - _last_refresh_ms > _refresh_ms) {
    _last_refresh_ms = millis();
    
    //auto element = Supla::Element::getElementByChannelNumber(_trv_thermometer_channel_no);
    
    if ((_trv_hvac) && (_trv_hvac->getTemperatureSetpointHeat() != _trv_temperature_setpoint * 10)) {
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - setpoint difference detected: hvac=%d, trv=%d", _trv_hvac->getTemperatureSetpointHeat(), _trv_temperature_setpoint);
      sendTRVTemperatureSetpoint(_trv_hvac->getTemperatureSetpointHeat()/10);        
    }
    
    if ((_trv_hvac) && ((_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1)  != _trv_system_mode)) {
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - trv mode difference detected: hvac=%d, trv=%d", _trv_hvac->getMode(), _trv_system_mode);
      sendTRVSystemMode(_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1);        
    }

    int16_t hvacLastTemperature = INT16_MIN;

    if (_trv_hvac)
      hvacLastTemperature = _trv_hvac->getLastTemperature();
    if ((_trv_local_temperature != INT16_MIN) && (hvacLastTemperature != INT16_MIN) && (hvacLastTemperature != _trv_local_temperature*10)) {
      int16_t temperature_calibration_offset = (hvacLastTemperature - _trv_local_temperature*10) / 100; 
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - trv temperature difference detected: hvac=%d, trv=%d, offset=%d", 
            hvacLastTemperature, _trv_local_temperature*10, temperature_calibration_offset);
      sendTRVTemperatureCalibration(temperature_calibration_offset);        
    }

    if (_trv_local_temperature == INT16_MIN) {
      log_i("No TRV temperature data - sending TemperatureCalibration with 0 value");
      sendTRVTemperatureCalibration(0);
    }
  }
}

void Supla::Control::Z2S_TRVInterface::handleAction(int event, int action) {

  (void)(event);

  log_i("Z2S_TRVInterface::handleAction 0x%x 0x%x", event, action);
  
  switch (action) {
    case TURN_ON: {
      _output_enabled = true;
	  } break;
    
    case TURN_OFF: {
      
    } break;   
  }
}
