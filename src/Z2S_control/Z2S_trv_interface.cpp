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

Supla::Control::Z2S_TRVInterface::Z2S_TRVInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_commands_set) 
  : RemoteOutputInterface(true), _gateway(gateway), _trv_commands_set(trv_commands_set) {

    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

Supla::Control::HvacBaseEE *Supla::Control::Z2S_TRVInterface::getTRVHvac(){

  return _trv_hvac;
}

void Supla::Control::Z2S_TRVInterface::setTRVHvac(Supla::Control::HvacBaseEE * trv_hvac) {

  _trv_hvac = trv_hvac;
}

void Supla::Control::Z2S_TRVInterface::setTemperatureCalibrationOffsetTrigger(int32_t temperature_calibration_offset_trigger) {

  _temperature_calibration_offset_trigger = temperature_calibration_offset_trigger;
}

void Supla::Control::Z2S_TRVInterface::setTemperatureCalibrationUpdateMs(uint32_t temperature_calibration_update_ms) {

  _temperature_calibration_update_ms = temperature_calibration_update_ms;
}

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureSetpoint(int32_t temperature_setpoint) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVTemperatureSetpoint = %d", temperature_setpoint);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);

    switch(_trv_commands_set) {
      case SASWELL_CMD_SET: {
        _Tuya_dp_data[2] = SASWELL_CMD_SET_TARGET_HEATSETPOINT_1; 
        temperature_setpoint *= SASWELL_TARGET_HEATSETPOINT_FACTOR;
      } break;
      case ME167_CMD_SET: {
        _Tuya_dp_data[2] = ME167_CMD_SET_TARGET_HEATSETPOINT_1; 
        temperature_setpoint *= ME167_TARGET_HEATSETPOINT_FACTOR;
      } break;
      case BECA_CMD_SET: {   
        _Tuya_dp_data[2] = BECA_CMD_SET_TARGET_HEATSETPOINT_1; 
        temperature_setpoint *= BECA_TARGET_HEATSETPOINT_FACTOR;
      } break;
      case MOES_CMD_SET: {     
        _Tuya_dp_data[2] = MOES_CMD_SET_TARGET_HEATSETPOINT_1;
        temperature_setpoint *= MOES_TARGET_HEATSETPOINT_FACTOR;
       } break;
      default: temperature_setpoint = 0; break;
    }
    if (temperature_setpoint == 0)
      return;
    temperature_setpoint /= 100;

    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = (temperature_setpoint & 0xFF000000) >> 24;
    _Tuya_dp_data[7] = (temperature_setpoint & 0x00FF0000) >> 16;
    _Tuya_dp_data[8] = (temperature_setpoint & 0x0000FF00) >> 8;
    _Tuya_dp_data[9] = (temperature_setpoint & 0x000000FF);

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureCalibration(int32_t temperature_calibration) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVTemperatureCalibration = %d", temperature_calibration);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);

    switch(_trv_commands_set) {
      case SASWELL_CMD_SET: {
        _Tuya_dp_data[2] = SASWELL_CMD_SET_TEMPERATURE_CALIBRATION_1; 
        temperature_calibration *= SASWELL_TEMPERATURE_CALIBRATION_FACTOR;
      } break;
      case ME167_CMD_SET: {
        _Tuya_dp_data[2] = ME167_CMD_SET_TEMPERATURE_CALIBRATION_1; 
        temperature_calibration *= ME167_TEMPERATURE_CALIBRATION_FACTOR;
      } break;
      case BECA_CMD_SET: {   
        _Tuya_dp_data[2] = BECA_CMD_SET_TEMPERATURE_CALIBRATION_1; 
        temperature_calibration *= BECA_TEMPERATURE_CALIBRATION_FACTOR;
      } break;
      case MOES_CMD_SET: {     
        _Tuya_dp_data[2] = MOES_CMD_SET_TEMPERATURE_CALIBRATION_1;
        temperature_calibration *= MOES_TEMPERATURE_CALIBRATION_FACTOR;
       } break;
      default:
        return; break;
    }

    temperature_calibration /= 100; 

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

    if (trv_system_mode == 1) {
      switch(_trv_commands_set) {
        case SASWELL_CMD_SET: {
          _Tuya_dp_data[2] = SASWELL_CMD_ON_1; 
          _Tuya_dp_data[3] = SASWELL_CMD_ON_2;
          _Tuya_dp_data[4] = SASWELL_CMD_ON_3;
          _Tuya_dp_data[5] = SASWELL_CMD_ON_4;
          _Tuya_dp_data[6] = SASWELL_CMD_ON_5;
        } break;
        case ME167_CMD_SET: {
          _Tuya_dp_data[2] = ME167_CMD_ON_1; 
          _Tuya_dp_data[3] = ME167_CMD_ON_2;
          _Tuya_dp_data[4] = ME167_CMD_ON_3;
          _Tuya_dp_data[5] = ME167_CMD_ON_4;
          _Tuya_dp_data[6] = ME167_CMD_ON_5;
        } break;
        case BECA_CMD_SET: {   
          _Tuya_dp_data[2] = BECA_CMD_ON_1; 
          _Tuya_dp_data[3] = BECA_CMD_ON_2;
          _Tuya_dp_data[4] = BECA_CMD_ON_3;
          _Tuya_dp_data[5] = BECA_CMD_ON_4;
          _Tuya_dp_data[6] = BECA_CMD_ON_5;
        } break;
        case MOES_CMD_SET: {     
          _Tuya_dp_data[2] = MOES_CMD_ON_1; 
          _Tuya_dp_data[3] = MOES_CMD_ON_2;
          _Tuya_dp_data[4] = MOES_CMD_ON_3;
          _Tuya_dp_data[5] = MOES_CMD_ON_4;
          _Tuya_dp_data[6] = MOES_CMD_ON_5;
        } break;
      }
    } else {
      switch(_trv_commands_set) {
        case SASWELL_CMD_SET: {
          _Tuya_dp_data[2] = SASWELL_CMD_OFF_1; 
          _Tuya_dp_data[3] = SASWELL_CMD_OFF_2;
          _Tuya_dp_data[4] = SASWELL_CMD_OFF_3;
          _Tuya_dp_data[5] = SASWELL_CMD_OFF_4;
          _Tuya_dp_data[6] = SASWELL_CMD_OFF_5;
        } break;
        case ME167_CMD_SET: {
          _Tuya_dp_data[2] = ME167_CMD_OFF_1; 
          _Tuya_dp_data[3] = ME167_CMD_OFF_2;
          _Tuya_dp_data[4] = ME167_CMD_OFF_3;
          _Tuya_dp_data[5] = ME167_CMD_OFF_4;
          _Tuya_dp_data[6] = ME167_CMD_OFF_5;
        } break;
        case BECA_CMD_SET: {   
          _Tuya_dp_data[2] = BECA_CMD_OFF_1; 
          _Tuya_dp_data[3] = BECA_CMD_OFF_2;
          _Tuya_dp_data[4] = BECA_CMD_OFF_3;
          _Tuya_dp_data[5] = BECA_CMD_OFF_4;
          _Tuya_dp_data[6] = BECA_CMD_OFF_5;
        } break;
        case MOES_CMD_SET: {     
          _Tuya_dp_data[2] = MOES_CMD_OFF_1; 
          _Tuya_dp_data[3] = MOES_CMD_OFF_2;
          _Tuya_dp_data[4] = MOES_CMD_OFF_3;
          _Tuya_dp_data[5] = MOES_CMD_OFF_4;
          _Tuya_dp_data[6] = MOES_CMD_OFF_5;
        } break;
      }  
    }

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureSetpoint(int32_t trv_temperature_setpoint) {

  _trv_temperature_setpoint = trv_temperature_setpoint;
  _trv_temperature_setpoint_updated = true;

  /*switch(_trv_commands_set) {
    case SASWELL_CMD_SET: {
      _trv_temperature_setpoint = trv_temperature_setpoint / SASWELL_LOCAL_TEMPERATURE_FACTOR;
    } break;
    case ME167_CMD_SET: {
      _trv_temperature_setpoint = trv_temperature_setpoint / ME167_LOCAL_TEMPERATURE_FACTOR;
    } break;
    case BECA_CMD_SET: {   
      _trv_temperature_setpoint = trv_temperature_setpoint / BECA_LOCAL_TEMPERATURE_FACTOR;
    } break;
    case MOES_CMD_SET: {     
      _trv_temperature_setpoint = trv_temperature_setpoint / MOES_LOCAL_TEMPERATURE_FACTOR;
    } break;
  }*/
}

void Supla::Control::Z2S_TRVInterface::setTRVSystemMode(uint8_t trv_system_mode) {

  _trv_system_mode = trv_system_mode;
  _trv_system_mode_updated = true;
}

void Supla::Control::Z2S_TRVInterface::setTRVRunningState(uint8_t trv_running_state) {

  _trv_running_state = trv_running_state;
  _trv_running_state_updated = true;
}

void Supla::Control::Z2S_TRVInterface::setTRVLocalTemperature(int32_t trv_local_temperature) {
  
  _trv_last_local_temperature = _trv_local_temperature;
  _trv_local_temperature = trv_local_temperature;
  _trv_local_temperature_updated = true;

  /*switch(_trv_commands_set) {
    case SASWELL_CMD_SET: {
      _trv_local_temperature = trv_local_temperature / SASWELL_LOCAL_TEMPERATURE_FACTOR;
    } break;
    case ME167_CMD_SET: {
      _trv_local_temperature = trv_local_temperature / ME167_LOCAL_TEMPERATURE_FACTOR;
    } break;
    case BECA_CMD_SET: {   
      _trv_local_temperature = trv_local_temperature / BECA_LOCAL_TEMPERATURE_FACTOR;
    } break;
    case MOES_CMD_SET: {     
      _trv_local_temperature = trv_local_temperature / MOES_LOCAL_TEMPERATURE_FACTOR;
    } break;
  }*/
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureCalibration(int32_t trv_temperature_calibration) {
  
  _trv_last_temperature_calibration = _trv_temperature_calibration;
  _trv_temperature_calibration = trv_temperature_calibration;
  _trv_temperature_calibration_updated = true;
}

void Supla::Control::Z2S_TRVInterface::iterateAlways() {

  if (millis() - _last_refresh_ms > _refresh_ms) {
    _last_refresh_ms = millis();
    
    if ((_trv_hvac) && (_trv_hvac->getTemperatureSetpointHeat() != _trv_temperature_setpoint)) {
      
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - setpoint difference detected: hvac=%d, trv=%d", 
            _trv_hvac->getTemperatureSetpointHeat(), _trv_temperature_setpoint);

      sendTRVTemperatureSetpoint(_trv_hvac->getTemperatureSetpointHeat());        
    }
    
    if ((_trv_hvac) && ((_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1)  != _trv_system_mode)) {
      
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - trv mode difference detected: hvac=%d, trv=%d", _trv_hvac->getMode(), _trv_system_mode);
      
      sendTRVSystemMode(_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1);        
    }

    int16_t hvacLastTemperature = INT16_MIN;

    if (_trv_hvac)
      hvacLastTemperature = _trv_hvac->getPrimaryTemp(); //_trv_hvac->getLastTemperature();
    //if ((_trv_local_temperature != INT32_MIN) && (hvacLastTemperature != INT16_MIN) && 
      //  (hvacLastTemperature != _trv_local_temperature)) {

    if ((_trv_local_temperature_updated) && ((_trv_temperature_calibration_updated) || (_trv_temperature_calibration == 0)) &&
    (hvacLastTemperature != INT16_MIN) && (hvacLastTemperature != (_trv_local_temperature /*- _trv_temperature_calibration*/))) {

      _last_temperature_calibration_offset = _temperature_calibration_offset;
      _temperature_calibration_offset = hvacLastTemperature - (_trv_local_temperature - _trv_temperature_calibration);

      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - trv temperature difference detected:\n\rhvac_temperature = %d,\n\rtrv_temperature = %d,\n\r"
            "trv_last_temperature = %d,\n\rtrv_calibration = %d,\n\rtrv_last_calibration = %d,\n\rcalculated offset = %d,\n\rlast calculated offset %d", 
            hvacLastTemperature, 
            _trv_local_temperature,
            _trv_last_local_temperature,
            _trv_temperature_calibration,
            _trv_last_temperature_calibration,
            _temperature_calibration_offset, 
            _last_temperature_calibration_offset);
      
      if ((_trv_temperature_calibration_updated) && (abs(_temperature_calibration_offset - _trv_temperature_calibration) > 100)) {

        _trv_local_temperature_updated = false;
        _trv_temperature_calibration_updated = false;

        sendTRVTemperatureCalibration(_temperature_calibration_offset);
      }


      /*if ((millis() - _trv_temperature_calibration_last_update_ms > _trv_temperature_calibration_update_ms) ||
          (abs(_trv_last_temperature_calibration_offset) > _trv_temperature_calibration_offset_trigger))
        {
          log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - _trv_temperature_calibration_last_update_ms %d", _trv_temperature_calibration_last_update_ms);
          _trv_temperature_calibration_last_update_ms = millis();
          sendTRVTemperatureCalibration(_trv_temperature_calibration_offset);
        }*/        
    }
  }

  if (millis() - _last_temperature_ping_ms > _temperature_ping_ms) {

    _last_temperature_ping_ms = millis();

    if (_trv_local_temperature == INT32_MIN) {
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
	  } break;
    
    case TURN_OFF: {
    } break;   
  }
}
