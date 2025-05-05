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

void Supla::Control::Z2S_TRVInterface::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    if (_trv_hvac)
      _trv_hvac->getChannel()->setStateOnline();
  }
  else
   _timeout_enabled = true;
}

void Supla::Control::Z2S_TRVInterface::refreshTimeout() {

  _last_seen_ms = millis();
  _last_cmd_sent_ms = 0;
  if (_trv_hvac)
      _trv_hvac->getChannel()->setStateOnline();
}

void Supla::Control::Z2S_TRVInterface::setTemperatureCalibrationOffsetTrigger(int32_t temperature_calibration_offset_trigger) {

  _temperature_calibration_offset_trigger = temperature_calibration_offset_trigger;
}

void Supla::Control::Z2S_TRVInterface::setTemperatureCalibrationUpdateMs(uint32_t temperature_calibration_update_ms) {

  _temperature_calibration_update_ms = temperature_calibration_update_ms;
}

void Supla::Control::Z2S_TRVInterface::enableExternalSensorDetection(bool enable_external_sensor_detection, uint8_t external_sensor_mode, uint8_t internal_sensor_channel) {
  
  _trv_external_sensor_detection_enabled = enable_external_sensor_detection;
  _trv_internal_sensor_channel = internal_sensor_channel;
  _trv_external_sensor_mode = external_sensor_mode;
}

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureSetpoint(int32_t temperature_setpoint) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVTemperatureSetpoint = %d", temperature_setpoint);

    if ((_trv_commands_set >= FIRST_0XEF00_CMD_SET) && (_trv_commands_set <= LAST_0XEF00_CMD_SET)) {

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
        case TRV601_CMD_SET: {     
          _Tuya_dp_data[2] = TRV601_CMD_SET_TARGET_HEATSETPOINT_1;
          temperature_setpoint *= TRV601_TARGET_HEATSETPOINT_FACTOR;
        } break;
        case TRV603_CMD_SET: {     
          _Tuya_dp_data[2] = TRV603_CMD_SET_TARGET_HEATSETPOINT_1;
          temperature_setpoint *= TRV603_TARGET_HEATSETPOINT_FACTOR;
        } break;
        default: return; break;
      }

      temperature_setpoint /= 100;

      _Tuya_dp_data[3] = 0x02;
      _Tuya_dp_data[4] = 0x00;
      _Tuya_dp_data[5] = 0x04;
      _Tuya_dp_data[6] = (temperature_setpoint & 0xFF000000) >> 24;
      _Tuya_dp_data[7] = (temperature_setpoint & 0x00FF0000) >> 16;
      _Tuya_dp_data[8] = (temperature_setpoint & 0x0000FF00) >> 8;
      _Tuya_dp_data[9] = (temperature_setpoint & 0x000000FF);

      _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
    } else 
    if (_trv_commands_set == TRVZB_CMD_SET) {

      _gateway->sendAttributeWrite(&_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID, 
                                   ESP_ZB_ZCL_ATTR_TYPE_S16,2, &temperature_setpoint);
    }
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureCalibration(int32_t temperature_calibration) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVTemperatureCalibration = %d", temperature_calibration);

    if ((_trv_commands_set >= FIRST_0XEF00_CMD_SET) && (_trv_commands_set <= LAST_0XEF00_CMD_SET)) {
      
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
        case TRV601_CMD_SET: {     
          _Tuya_dp_data[2] = TRV601_CMD_SET_TEMPERATURE_CALIBRATION_1;
          temperature_calibration *= TRV601_TEMPERATURE_CALIBRATION_FACTOR;
        } break;
        case TRV603_CMD_SET: {     
          _Tuya_dp_data[2] = TRV603_CMD_SET_TEMPERATURE_CALIBRATION_1;
          temperature_calibration *= TRV603_TEMPERATURE_CALIBRATION_FACTOR;
        } break;
        default: return; break;
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
    } else 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      temperature_calibration = temperature_calibration / 10;
      _gateway->sendAttributeWrite(&_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID, 
                                   ESP_ZB_ZCL_ATTR_TYPE_S8,1, &temperature_calibration);
    }
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVExternalSensorTemperature(int32_t external_sensor_temperature) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVExternalSensorTemperature = %d", external_sensor_temperature);
 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      uint8_t temperature_selector = 1;
      _gateway->sendAttributeWrite(&_device, SONOFF_TRVZB_CUSTOM_CLUSTER, TRVZB_CMD_SET_TEMPERATURE_SENSOR_SELECT, 
                                   ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);

      _gateway->sendAttributeWrite(&_device, SONOFF_TRVZB_CUSTOM_CLUSTER, TRVZB_CMD_SET_EXTERNAL_TEMPERATURE_INPUT, 
                                   ESP_ZB_ZCL_ATTR_TYPE_S16, 2, &external_sensor_temperature);
    }
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVExternalSensorInput(bool trv_external_sensor_present) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVExternalSensorInput = %d", trv_external_sensor_present);
 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      uint8_t temperature_selector = trv_external_sensor_present ? 1 : 0;
      _gateway->sendAttributeWrite(&_device, SONOFF_TRVZB_CUSTOM_CLUSTER, TRVZB_CMD_SET_TEMPERATURE_SENSOR_SELECT, 
                                   ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);
    }
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVSystemMode(uint8_t trv_system_mode) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVMode = %d", trv_system_mode);

    if ((_trv_commands_set >= FIRST_0XEF00_CMD_SET) && (_trv_commands_set <= LAST_0XEF00_CMD_SET)) {

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
          case TRV601_CMD_SET: {     
            _Tuya_dp_data[2] = TRV601_CMD_ON_1; 
            _Tuya_dp_data[3] = TRV601_CMD_ON_2;
            _Tuya_dp_data[4] = TRV601_CMD_ON_3;
            _Tuya_dp_data[5] = TRV601_CMD_ON_4;
            _Tuya_dp_data[6] = TRV601_CMD_ON_5;
          } break;
          case TRV603_CMD_SET: {     
            _Tuya_dp_data[2] = TRV603_CMD_ON_1; 
            _Tuya_dp_data[3] = TRV603_CMD_ON_2;
            _Tuya_dp_data[4] = TRV603_CMD_ON_3;
            _Tuya_dp_data[5] = TRV603_CMD_ON_4;
            _Tuya_dp_data[6] = TRV603_CMD_ON_5;
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
          case TRV601_CMD_SET: {     
            _Tuya_dp_data[2] = TRV601_CMD_OFF_1; 
            _Tuya_dp_data[3] = TRV601_CMD_OFF_2;
            _Tuya_dp_data[4] = TRV601_CMD_OFF_3;
            _Tuya_dp_data[5] = TRV601_CMD_OFF_4;
            _Tuya_dp_data[6] = TRV601_CMD_OFF_5;
          } break;
          case TRV603_CMD_SET: {     
            _Tuya_dp_data[2] = TRV603_CMD_OFF_1; 
            _Tuya_dp_data[3] = TRV603_CMD_OFF_2;
            _Tuya_dp_data[4] = TRV603_CMD_OFF_3;
            _Tuya_dp_data[5] = TRV603_CMD_OFF_4;
            _Tuya_dp_data[6] = TRV603_CMD_OFF_5;
          } break;
        }  
      }

      _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);
    } else 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      trv_system_mode = (trv_system_mode == 0) ? 0 : 4; //

      _gateway->sendAttributeWrite(&_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
                                   ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &trv_system_mode);
    }
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::sendTRVPing() {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVInterface::sendTRVPing");
 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      uint16_t attributes[5] = { ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID, 
                                 ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,
                                 ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID,
                                 ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID,
                                 ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID };

    _gateway->sendAttributesRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 5, &attributes[0]);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureSetpoint(int32_t trv_temperature_setpoint) {

  _trv_temperature_setpoint = trv_temperature_setpoint;
  _trv_temperature_setpoint_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVSystemMode(uint8_t trv_system_mode) {

  _trv_system_mode = trv_system_mode;
  _trv_system_mode_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVRunningState(uint8_t trv_running_state) {

  _trv_running_state = trv_running_state;
  _trv_running_state_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVLocalTemperature(int32_t trv_local_temperature) {
  
  _trv_last_local_temperature = _trv_local_temperature;
  _trv_local_temperature = trv_local_temperature;
  _trv_local_temperature_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureCalibration(int32_t trv_temperature_calibration) {
  
  _trv_last_temperature_calibration = _trv_temperature_calibration;
  _trv_temperature_calibration = trv_temperature_calibration;
  _trv_temperature_calibration_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::iterateAlways() {

  if (millis() - _last_refresh_ms > _refresh_ms) {
    _last_refresh_ms = millis();

    if ((_trv_hvac) && (_trv_hvac->getMode() != SUPLA_HVAC_MODE_OFF) && (_trv_hvac->getTemperatureSetpointHeat() != _trv_temperature_setpoint)) { //??
      
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - setpoint difference detected: hvac=%d, trv=%d", 
            _trv_hvac->getTemperatureSetpointHeat(), _trv_temperature_setpoint);

      sendTRVTemperatureSetpoint(_trv_hvac->getTemperatureSetpointHeat());        
    }
    
    if ((_trv_hvac) && ((_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1)  != _trv_system_mode)) {
      
      log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - trv mode difference detected: hvac=%d, trv=%d", _trv_hvac->getMode(), _trv_system_mode);
      
      sendTRVSystemMode(_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1);        
    }

    bool external_sensor_last_state = _trv_external_sensor_present;

    if (_trv_hvac && _trv_external_sensor_detection_enabled) {
      
      if (_trv_hvac->getMainThermometerChannelNo() != _trv_internal_sensor_channel)
        _trv_external_sensor_present = true;
      else
        _trv_external_sensor_present = false;

      if (_trv_external_sensor_present != external_sensor_last_state)
        _trv_external_sensor_changed = true;
      else
        _trv_external_sensor_changed = false;
    }
    

    int16_t hvacLastTemperature = INT16_MIN;

    if (_trv_hvac)
        hvacLastTemperature = _trv_hvac->getPrimaryTemp();

    if (_trv_external_sensor_present && (_trv_external_sensor_mode == EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE)) {  //standard calibration

       //_trv_hvac->getLastTemperature();
        //if ((_trv_local_temperature != INT32_MIN) && (hvacLastTemperature != INT16_MIN) && 
        //    (hvacLastTemperature != _trv_local_temperature)) {

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
      
        if ((_trv_temperature_calibration_updated) && (abs(_temperature_calibration_offset - _trv_temperature_calibration) >= 100)) { 

          _trv_local_temperature_updated = false;
          _trv_temperature_calibration_updated = false;

          sendTRVTemperatureCalibration(_temperature_calibration_offset);
        }
        /*if ((millis() - _trv_temperature_calibration_last_update_ms > _trv_temperature_calibration_update_ms) ||
              (abs(_trv_last_temperature_calibration_offset) > _trv_temperature_calibration_offset_trigger)) {
            log_i("Supla::Control::Z2S_TRVInterface::iterateAlways() - _trv_temperature_calibration_last_update_ms %d", _trv_temperature_calibration_last_update_ms);
                  _trv_temperature_calibration_last_update_ms = millis();
            sendTRVTemperatureCalibration(_trv_temperature_calibration_offset);
        }*/        
      }
    } else 
    if (_trv_external_sensor_present && (_trv_external_sensor_mode == EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT)) {
      
      if ((hvacLastTemperature != INT16_MIN) && 
          ((abs(hvacLastTemperature - _trv_local_temperature) >= 10) || (millis() - _last_external_temperature_ping_ms > _external_temperature_ping_ms))) {
        log_i("external temperature difference detected %d vs %d",hvacLastTemperature,_trv_local_temperature);
        _last_external_temperature_ping_ms = millis();
        sendTRVExternalSensorTemperature(hvacLastTemperature);
      }
    }
    if (_trv_external_sensor_changed && (!_trv_external_sensor_present) && (_trv_external_sensor_mode == EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT)) {
      log_i("Switching OFF external sensor temperature input");
      sendTRVExternalSensorInput(_trv_external_sensor_present);
    }
  }

  if (millis() - _last_temperature_ping_ms > _temperature_ping_ms) {

    _last_temperature_ping_ms = millis();

    if (_trv_local_temperature == INT32_MIN) {
      log_i("No TRV temperature data - sending TemperatureCalibration with 0 value");
      sendTRVTemperatureCalibration(0);
    }
  }

  if (millis() - _last_thermostat_ping_ms > _thermostat_ping_ms) {

    _last_thermostat_ping_ms = millis();

    log_i("sendTRVPing");
    sendTRVPing();
  }

  if (_timeout_enabled && (_last_cmd_sent_ms > 0) && (millis() - _last_cmd_sent_ms > _timeout_ms)) {

    if (_trv_hvac)
      _trv_hvac->getChannel()->setStateOffline();
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
