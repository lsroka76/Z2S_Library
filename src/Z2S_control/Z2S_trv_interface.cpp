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

#include <time.h>

#include "Z2S_trv_interface.h"

/*****************************************************************************/

Supla::Control::Z2S_TRVInterface::Z2S_TRVInterface(
  ZigbeeGateway *gateway, 
  zbg_device_params_t *device, 
  uint8_t trv_commands_set,
  bool onOffOnly) 
  : RemoteOutputInterface(onOffOnly), 
  _gateway(gateway), 
  _trv_commands_set(trv_commands_set) {

  memcpy(&_device, device, sizeof(zbg_device_params_t));     

  if ((_trv_commands_set >= saswell_cmd_set) &&
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

    if (ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id == 
        _trv_commands_set) {
        
        _trv_temperature_histeresis_enabled = 
          (ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_temperature_histeresis_dp_id > 0);

        _trv_temperature_calibration_trigger /= 
          ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_temperature_calibration_factor;
            
        _init_sequence = 1; //
        _init_temperature_setpoint = 
          ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_target_heatsetpoint_min;
  } else
    log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
          ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
          _trv_commands_set);  
  }

  if ((_trv_commands_set == TRVZB_CMD_SET) ||
      (_trv_commands_set == BOSCH_CMD_SET) ||
      (_trv_commands_set == EUROTRONIC_CMD_SET) ||
      (_trv_commands_set == LUMI_CMD_SET)) {
  
      _trv_temperature_calibration_trigger = 10;
  }

}

/*****************************************************************************/

Supla::Control::HvacBaseEE *Supla::Control::Z2S_TRVInterface::getTRVHvac(){

  return _trv_hvac;
}

/*****************************************************************************/

bool Supla::Control::Z2S_TRVInterface::inInitSequence(){

  return (_init_sequence > 0);
}

/*****************************************************************************/

bool Supla::Control::Z2S_TRVInterface::isHvacWindowOpened() {

  return _hvac_window_opened;
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::setTRVHvac(
  Supla::Control::HvacBaseEE * trv_hvac) {

  _trv_hvac = trv_hvac;
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::setTimeoutSecs(
  uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    if (_trv_hvac)
      _trv_hvac->getChannel()->setStateOnline();
  }
  else
   _timeout_enabled = true;
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::refreshTimeout() {

  _last_seen_ms = millis();
  _last_cmd_sent_ms = 0;
  if (_trv_hvac)
      _trv_hvac->getChannel()->setStateOnline();
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::setFixedTemperatureCalibration(
    int32_t trv_fixed_temperature_calibration) {

    if (abs(trv_fixed_temperature_calibration - 
                _trv_temperature_calibration) <= 10) {

      _trv_fixed_temperature_calibration = _trv_temperature_calibration;
      return;
    }
    
    _trv_fixed_temperature_calibration = trv_fixed_temperature_calibration;
    _trv_fixed_temperature_calibration_updated = true;

    log_i("_trv_fixed_temperature_calibration updated to %ld,"
          "_trv_temperature_calibration %ld", 
          _trv_fixed_temperature_calibration,
          _trv_temperature_calibration);
  }

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::setCooperativeChildLock(
  bool cooperative_child_lock) {

    _cooperative_child_lock = cooperative_child_lock;
  }

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureSensorType(
  uint8_t trv_temperature_sensor_type) {

    _trv_temperature_sensor_type = trv_temperature_sensor_type;
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::enableExternalSensorDetection(
    bool enable_external_sensor_detection, 
    uint8_t external_sensor_mode, 
    uint8_t internal_sensor_channel) {
  
  _trv_external_sensor_detection_enabled = enable_external_sensor_detection;
  _trv_internal_sensor_channel = internal_sensor_channel;
  _trv_external_sensor_mode = external_sensor_mode;
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureSetpoint(
    int32_t temperature_setpoint) {

  if (_gateway && Zigbee.started()) {

    log_i("Z2S_TRVInterface::sendTRVTemperatureSetpoint = %d", 
          temperature_setpoint);

    if ((_trv_commands_set >= saswell_cmd_set) &&
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      if (ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id == _trv_commands_set) {
        
        if (ts0601_command_sets_table[_trv_commands_set].
              ts0601_cmd_set_target_heatsetpoint_dp_type == TUYA_DP_TYPE_VALUE) {

          temperature_setpoint *= ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_target_heatsetpoint_factor;
          temperature_setpoint /= 100;

          sendTuyaRequestCmdValue32(_gateway, 
                                    &_device, 
                                    ts0601_command_sets_table[_trv_commands_set].
                                      ts0601_cmd_set_target_heatsetpoint_dp_id,
                                    temperature_setpoint);
        }
      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set); 
    } else

    if ((_trv_commands_set == TRVZB_CMD_SET) ||
        (_trv_commands_set == BOSCH_CMD_SET) ||
        (_trv_commands_set == LUMI_CMD_SET)) {

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_S16, 2, &temperature_setpoint);

      _gateway->sendAttributeRead(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID);
    }

    if (_trv_commands_set == EUROTRONIC_CMD_SET) {

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        EUROTRONIC_CURRENT_HEATING_SETPOINT_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_S16, 2, &temperature_setpoint,
        false, 1, EUROTRONIC_MANUFACTURER_CODE);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::readTRVLocalTemperature(
    int32_t local_temperature) {

  if (_gateway && Zigbee.started()) {

    //log_i(" = %d", 
     //     local_temperature);

    if ((_trv_commands_set >= saswell_cmd_set) &&
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      //sendTuyaQueryCmd(_gateway, &_device, false);

      /*if (ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id == 
          _trv_commands_set) {
        
        if (ts0601_command_sets_table[_trv_commands_set].\
              ts0601_cmd_set_local_temperature_dp_type == 
            TUYA_DP_TYPE_VALUE) {

          local_temperature *= ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_local_temperature_factor;
          local_temperature /= 100;

          sendTuyaRequestCmdValue32(
            _gateway, &_device, 
            ts0601_command_sets_table[_trv_commands_set].\
              ts0601_cmd_set_local_temperature_dp_id,
            local_temperature);
        }
      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set); */
    } else

    if ((_trv_commands_set == TRVZB_CMD_SET) ||
        (_trv_commands_set == BOSCH_CMD_SET) ||
        (_trv_commands_set == EUROTRONIC_CMD_SET) ||
        (_trv_commands_set == LUMI_CMD_SET)) {

      _gateway->sendAttributeRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureCalibration(
  int32_t temperature_calibration) {

  if (_gateway && Zigbee.started()) {

    log_i("Z2S_TRVInterface::sendTRVTemperatureCalibration = %d", 
          temperature_calibration);

    if ((_trv_commands_set >= saswell_cmd_set) && 
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      if (ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_id == _trv_commands_set) {
        
        if (ts0601_command_sets_table[_trv_commands_set].
          ts0601_cmd_set_temperature_calibration_dp_type == TUYA_DP_TYPE_VALUE) {

          temperature_calibration *= ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_temperature_calibration_factor;

          temperature_calibration /= 100;

          sendTuyaRequestCmdValue32(_gateway, 
                                    &_device, 
                                    ts0601_command_sets_table[_trv_commands_set].
                                      ts0601_cmd_set_temperature_calibration_dp_id,
                                    temperature_calibration);
        }
      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set);
    } else 

    if ((_trv_commands_set == TRVZB_CMD_SET) ||
        (_trv_commands_set == BOSCH_CMD_SET) ||
        (_trv_commands_set == EUROTRONIC_CMD_SET) ||
        (_trv_commands_set == LUMI_CMD_SET)) {

      
      temperature_calibration = temperature_calibration / 10;
      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_S8, 1, &temperature_calibration);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void buildLumiFFF2CmdHeader(uint8_t* fff2_cmd_data_buffer,
                            uint8_t counter,
                            uint8_t action,
                            uint8_t params_size) {

  memset(fff2_cmd_data_buffer, 0 , 0xAA);

  uint8_t fff2_header_size = sizeof(lumi_fff2_cmd_header_t);
  
  memcpy(fff2_cmd_data_buffer, 
         &lumi_fff2_cmd_header_template,
         fff2_header_size);

  lumi_fff2_cmd_header_t *lumi_fff2_cmd_header = 
    (lumi_fff2_cmd_header_t *)fff2_cmd_data_buffer;

  lumi_fff2_cmd_header->cmd_size = params_size + 3;
  lumi_fff2_cmd_header->counter = counter;
  lumi_fff2_cmd_header->cmd_action = action;
  lumi_fff2_cmd_header->params_size = params_size;

  uint16_t integrity = lumi_fff2_cmd_header->cmd_start +
                       lumi_fff2_cmd_header->cmd_category +
                       lumi_fff2_cmd_header->cmd_size +
                       lumi_fff2_cmd_header->cmd_type +
                       lumi_fff2_cmd_header->counter;
  integrity = 512 - integrity;
  lumi_fff2_cmd_header->integrity = integrity;
}

uint8_t buildLumiFFF2CmdLinkParams1(uint8_t* fff2_cmd_data_buffer,
                                    uint8_t counter,
                                    time_t timestamp,
                                    esp_zb_ieee_addr_t ieee_addr) {
  
  uint8_t fff2_header_size = sizeof(lumi_fff2_cmd_header_t);
  uint8_t lumi_sensor_link_params_1_size = 
    sizeof(lumi_sensor_link_params_1_t);

  *fff2_cmd_data_buffer = fff2_header_size + lumi_sensor_link_params_1_size;

  buildLumiFFF2CmdHeader(fff2_cmd_data_buffer + 1,
                         counter, 
                         LUMI_FFF2_CMD_ACTION_LINK_SENSOR,
                         lumi_sensor_link_params_1_size);
    
  memcpy(fff2_cmd_data_buffer + fff2_header_size + 1, 
         &lumi_sensor_link_params_1_template,
         lumi_sensor_link_params_1_size);

  lumi_sensor_link_params_1_t *lumi_sensor_link_params_1 = 
    (lumi_sensor_link_params_1_t*)(fff2_cmd_data_buffer + fff2_header_size + 1);
      
  lumi_sensor_link_params_1->timestamp  = timestamp;

  /*memcpy(lumi_sensor_link_params_1->device_address,
        ieee_addr, 
        sizeof(esp_zb_ieee_addr_t));
        */
  for (uint8_t i = 0; i < sizeof(esp_zb_ieee_addr_t); i++)
    lumi_sensor_link_params_1->device_address[i] = ieee_addr[7 - i];

  return fff2_header_size + lumi_sensor_link_params_1_size + 1;
}

uint8_t buildLumiFFF2CmdLinkParams2(uint8_t* fff2_cmd_data_buffer,
                                    uint8_t counter,
                                    time_t timestamp,
                                    esp_zb_ieee_addr_t ieee_addr) {

  uint8_t fff2_header_size = sizeof(lumi_fff2_cmd_header_t);
  uint8_t lumi_sensor_link_params_2_size = 
    sizeof(lumi_sensor_link_params_2_t);

  *fff2_cmd_data_buffer = fff2_header_size + lumi_sensor_link_params_2_size;

  buildLumiFFF2CmdHeader(fff2_cmd_data_buffer + 1, 
                         counter, 
                         LUMI_FFF2_CMD_ACTION_LINK_SENSOR,
                         lumi_sensor_link_params_2_size);
    
  memcpy(fff2_cmd_data_buffer + fff2_header_size + 1, 
         &lumi_sensor_link_params_2_template,
         lumi_sensor_link_params_2_size);

  lumi_sensor_link_params_2_t *lumi_sensor_link_params_2 = 
    (lumi_sensor_link_params_2_t*)(fff2_cmd_data_buffer + fff2_header_size + 1);
      
  lumi_sensor_link_params_2->timestamp  = timestamp;

  /*memcpy(lumi_sensor_link_params_2->device_address,
        ieee_addr, 
        sizeof(esp_zb_ieee_addr_t));*/

  for (uint8_t i = 0; i < sizeof(esp_zb_ieee_addr_t); i++)
    lumi_sensor_link_params_2->device_address[i] = ieee_addr[7 - i];
  
  return fff2_header_size + lumi_sensor_link_params_2_size + 1;
}

uint8_t buildLumiFFF2CmdUnlinkParams(uint8_t* fff2_cmd_data_buffer,
                                     uint8_t counter,
                                     time_t timestamp,
                                     uint8_t link_id,
                                     esp_zb_ieee_addr_t ieee_addr) {

  uint8_t fff2_header_size = sizeof(lumi_fff2_cmd_header_t);
  uint8_t lumi_sensor_unlink_params_size = 
    sizeof(lumi_sensor_unlink_params_t);

  *fff2_cmd_data_buffer = fff2_header_size + lumi_sensor_unlink_params_size;

  buildLumiFFF2CmdHeader(fff2_cmd_data_buffer + 1, 
                         counter, 
                         LUMI_FFF2_CMD_ACTION_UNLINK_SENSOR,
                         lumi_sensor_unlink_params_size);
    
  memcpy(fff2_cmd_data_buffer + fff2_header_size + 1, 
         &lumi_sensor_unlink_params_template,
         lumi_sensor_unlink_params_size);

  lumi_sensor_unlink_params_t *lumi_sensor_unlink_params = 
    (lumi_sensor_unlink_params_t*)(fff2_cmd_data_buffer + fff2_header_size + 1);
      
  lumi_sensor_unlink_params->timestamp  = timestamp;
  lumi_sensor_unlink_params->link_id    = link_id;

  /*memcpy(lumi_sensor_unlink_params->device_address,
        ieee_addr, 
        sizeof(esp_zb_ieee_addr_t));*/
  
  for (uint8_t i = 0; i < sizeof(esp_zb_ieee_addr_t); i++)
    lumi_sensor_unlink_params->device_address[i] = ieee_addr[7 - i];

  return fff2_header_size + lumi_sensor_unlink_params_size + 1;
}

float ReverseFloat( const float inFloat ) {
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}


uint8_t buildLumiFFF2CmdSendTemperatureParams(uint8_t* fff2_cmd_data_buffer,
                                              float temperature_100) {
  
  uint8_t fff2_header_size = sizeof(lumi_fff2_cmd_header_t);
  uint8_t lumi_sensor_send_temperature_params_size = 
    sizeof(lumi_sensor_send_temperature_params_t);

  *fff2_cmd_data_buffer = fff2_header_size + lumi_sensor_send_temperature_params_size;

  buildLumiFFF2CmdHeader(fff2_cmd_data_buffer + 1, 
                         0x10, 
                         LUMI_FFF2_CMD_ACTION_SEND_TEMPERATURE,
                         lumi_sensor_send_temperature_params_size);
    
  memcpy(fff2_cmd_data_buffer + fff2_header_size + 1, 
         &lumi_sensor_send_temperature_params_template,
         lumi_sensor_send_temperature_params_size);

  lumi_sensor_send_temperature_params_t *lumi_sensor_send_temperature_params = 
    (lumi_sensor_send_temperature_params_t*)(fff2_cmd_data_buffer + 
      fff2_header_size + 1);
      
  lumi_sensor_send_temperature_params->temperature_100  = 
    ReverseFloat(temperature_100);

  return fff2_header_size + lumi_sensor_send_temperature_params_size + 1;
}

/*---------------------------------------------------------------------------*/

void Supla::Control::Z2S_TRVInterface::sendTRVExternalSensorTemperature(
  int32_t external_sensor_temperature) {

  if (_gateway && Zigbee.started()) {

    log_i("external_sensor_temperature = %d", 
          external_sensor_temperature);
 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      
      uint8_t temperature_selector = 1;

      _gateway->sendAttributeWrite(
        &_device, 
        SONOFF_CUSTOM_CLUSTER, 
        TRVZB_CMD_SET_TEMPERATURE_SENSOR_SELECT, 
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);

      _gateway->sendAttributeWrite(
        &_device, 
        SONOFF_CUSTOM_CLUSTER, 
        TRVZB_CMD_SET_EXTERNAL_TEMPERATURE_INPUT, 
        ESP_ZB_ZCL_ATTR_TYPE_S16, 2, &external_sensor_temperature);
    }

    if (_trv_commands_set == BOSCH_CMD_SET) {
      
    
      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        BOSCH_TRV_EXTERNAL_TEMPERATURE_INPUT_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_S16, 
        2, &external_sensor_temperature,
        true,
        1, BOSCH_MANUFACTURER_CODE);
    }

    if (_trv_commands_set == LUMI_CMD_SET) {

      uint8_t fff2_cmd_data_buffer[0xAA];

      time_t timestamp = time(nullptr);

      if ((_trv_temperature_sensor_type == 0) ||
          (_trv_temperature_sensor_type == 0xFF))
        sendTRVExternalSensorInput(true);
      
      uint8_t buffer_size = 
        buildLumiFFF2CmdSendTemperatureParams(fff2_cmd_data_buffer, 
                                              external_sensor_temperature);

      _gateway->sendAttributeWrite(
        &_device, 
        LUMI_CUSTOM_CLUSTER, 
        LUMI_CUSTOM_CLUSTER_FFF2_CMD_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING, 
        buffer_size, &fff2_cmd_data_buffer,
        true,
        1, LUMI_MANUFACTURER_CODE);  

      _gateway->sendAttributeRead(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
        false);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVExternalSensorInput(
  bool trv_external_sensor_present) {

  if (_gateway && Zigbee.started()) {

    log_i("trv_external_sensor_present = %d", 
          trv_external_sensor_present);
 
    if (_trv_commands_set == TRVZB_CMD_SET) {

      uint8_t temperature_selector = trv_external_sensor_present ? 1 : 0;

      _gateway->sendAttributeWrite(
        &_device, 
        SONOFF_CUSTOM_CLUSTER, 
        TRVZB_CMD_SET_TEMPERATURE_SENSOR_SELECT, 
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);
    }

    if (_trv_commands_set == LUMI_CMD_SET) {
      
      uint8_t fff2_cmd_data_buffer[0xAA];

      time_t timestamp = time(nullptr);

      if (trv_external_sensor_present) {
      
        uint8_t buffer_size =
          buildLumiFFF2CmdLinkParams1(fff2_cmd_data_buffer,
                                      0x12, 
                                      timestamp, 
                                      _device.ieee_addr);

        _gateway->sendAttributeWrite(
          &_device, 
          LUMI_CUSTOM_CLUSTER, 
          LUMI_CUSTOM_CLUSTER_FFF2_CMD_ID, 
          ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING, 
          buffer_size, &fff2_cmd_data_buffer,
          true,
          1, LUMI_MANUFACTURER_CODE);

        buffer_size =
          buildLumiFFF2CmdLinkParams2(fff2_cmd_data_buffer, 
                                      0x13,
                                      timestamp, 
                                      _device.ieee_addr);

        _gateway->sendAttributeWrite(
          &_device, 
          LUMI_CUSTOM_CLUSTER, 
          LUMI_CUSTOM_CLUSTER_FFF2_CMD_ID, 
          ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING, 
          buffer_size, &fff2_cmd_data_buffer,
          true,
          1, LUMI_MANUFACTURER_CODE);

        _gateway->sendAttributeRead(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
        false);

      } else {

        uint8_t buffer_size =
          buildLumiFFF2CmdUnlinkParams(fff2_cmd_data_buffer, 
                                       0x12,
                                       timestamp, 
                                       0x05,
                                       _device.ieee_addr);

        _gateway->sendAttributeWrite(
          &_device, 
          LUMI_CUSTOM_CLUSTER, 
          LUMI_CUSTOM_CLUSTER_FFF2_CMD_ID, 
          ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING, 
          buffer_size, &fff2_cmd_data_buffer,
          true,
          1, LUMI_MANUFACTURER_CODE);

        buffer_size =
          buildLumiFFF2CmdUnlinkParams(fff2_cmd_data_buffer, 
                                       0x13,
                                       timestamp, 
                                       0x04,
                                       _device.ieee_addr);

        _gateway->sendAttributeWrite(
          &_device, 
          LUMI_CUSTOM_CLUSTER, 
          LUMI_CUSTOM_CLUSTER_FFF2_CMD_ID, 
          ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING, 
          buffer_size, &fff2_cmd_data_buffer,
          true,
          1, LUMI_MANUFACTURER_CODE);

        _gateway->sendAttributeRead(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
        true);
      } 
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVSystemMode(
  uint8_t trv_system_mode) {

  if (_gateway && Zigbee.started()) {

    log_i("Z2S_TRVInterface::sendTRVMode = %d", 
          trv_system_mode);

    if ((_trv_commands_set >= saswell_cmd_set) && 
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      if (ts0601_command_sets_table[_trv_commands_set].
        ts0601_cmd_set_id == _trv_commands_set) {

        uint8_t system_mode_dp_id;
        uint8_t system_mode_dp_type;
        uint8_t system_mode_value;

        switch (trv_system_mode) {

          case 1: {

            system_mode_dp_id = 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_on_dp_id;

            system_mode_dp_type = 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_on_dp_type;

            system_mode_value = 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_on_dp_value_on;
          } break;

          case 0: {

            system_mode_dp_id = 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_off_dp_id;
              
            system_mode_dp_type = 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_off_dp_type;

            system_mode_value = 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_off_dp_value_off;
          } break;
        }
            
        sendTuyaRequestCmdData(_gateway, 
                               &_device, 
                               system_mode_dp_id,
                               system_mode_dp_type,
                               system_mode_value); 

      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set);
    } else 
    if (_trv_commands_set == TRVZB_CMD_SET) {
      
      trv_system_mode = (trv_system_mode == 0) ? 0 : 4; //

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &trv_system_mode);
    }

    if (_trv_commands_set == BOSCH_CMD_SET) {
      
      trv_system_mode = (trv_system_mode == 0) ? 5 : 1; //

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        BOSCH_TRV_OPERATING_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &trv_system_mode,
        false, 1, BOSCH_MANUFACTURER_CODE);
    }

    if (_trv_commands_set == EUROTRONIC_CMD_SET) {

      esp_zb_uint24_t eurotronic_host_flags;
     
      eurotronic_host_flags.low = (1 << 0) |
                                  (1 << ((trv_system_mode == 0) ? 5 : 2)) |
                                  (1 << ((_trv_child_lock == 1) ? 7 : 0));

      log_i("EUROTRONIC host flags = 0x%04X", eurotronic_host_flags.low);

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_U24, 3, &eurotronic_host_flags,
        false, 1, EUROTRONIC_MANUFACTURER_CODE);
    }

    if (_trv_commands_set == LUMI_CMD_SET) {
      
      trv_system_mode = (trv_system_mode == 0) ? 0 : 1; //

      _gateway->sendAttributeWrite(
        &_device, 
        LUMI_CUSTOM_CLUSTER, 
        LUMI_CUSTOM_CLUSTER_TRV_SYSTEM_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &trv_system_mode,
        false, 1, LUMI_MANUFACTURER_CODE);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVScheduleMode(
  uint8_t trv_schedule_mode) {

  if (_gateway && Zigbee.started()) {

    log_i("Z2S_TRVInterface::sendTRVScheduleMode = %d", 
          trv_schedule_mode);

    if ((_trv_commands_set >= saswell_cmd_set) && 
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      if (ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_id == _trv_commands_set) {

        uint8_t schedule_mode_dp_id = 
          ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_schedule_mode_dp_id;

        uint8_t schedule_mode_dp_type = 
          ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_schedule_mode_dp_type;

        uint8_t schedule_mode_value;

        switch (trv_schedule_mode) {

          case 1: {

            schedule_mode_value = ts0601_command_sets_table[_trv_commands_set].\
              ts0601_cmd_set_schedule_mode_dp_value_on;
          } break;

          case 0: {

            schedule_mode_value = ts0601_command_sets_table[_trv_commands_set].\
              ts0601_cmd_set_schedule_mode_dp_value_off;

            if (schedule_mode_value == 0xFF) {

              schedule_mode_dp_id = 
                ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_on_dp_id;  //when on_off_dp == schedule_dp

              schedule_mode_dp_type = 
                ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_on_dp_type;


              schedule_mode_value = ts0601_command_sets_table[_trv_commands_set].
                ts0601_cmd_on_dp_value_on;              
            }
          } break;
        }
        
        sendTuyaRequestCmdData(_gateway, 
                               &_device, 
                               schedule_mode_dp_id,
                               schedule_mode_dp_type,
                               schedule_mode_value);

            
      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set);
    } else 
    if (_trv_commands_set == TRVZB_CMD_SET) {

      trv_schedule_mode = (trv_schedule_mode == 0) ? 4 : 1; //

      _gateway->sendAttributeWrite(&_device, 
                                   ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                                   ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
                                   ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 
                                   1, 
                                   &trv_schedule_mode);
    }

    if (_trv_commands_set == BOSCH_CMD_SET) {
      
      trv_schedule_mode = (trv_schedule_mode == 0) ? 1 : 0; //

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        BOSCH_TRV_OPERATING_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &trv_schedule_mode,
        false, 1, BOSCH_MANUFACTURER_CODE);
    }


    if (_trv_commands_set == EUROTRONIC_CMD_SET) {

      esp_zb_uint24_t eurotronic_host_flags;

      eurotronic_host_flags.low = (1 << 0) |
                                  //(1 << ((trv_system_mode == 0) ? 5 : 2)) |
                                  (1 << ((trv_schedule_mode == 0) ? 2 : 4)) |
                                  (1 << ((_trv_child_lock == 1) ? 7 : 0));

      log_i("EUROTRONIC host flags = 0x%04X", eurotronic_host_flags.low);
      
      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_U24, 3, &eurotronic_host_flags,
        false, 1, EUROTRONIC_MANUFACTURER_CODE);
    }

    if (_trv_commands_set == LUMI_CMD_SET) {
      
      trv_schedule_mode = (trv_schedule_mode == 0) ? 0 : 1; //

      _gateway->sendAttributeWrite(
        &_device, 
        LUMI_CUSTOM_CLUSTER, 
        LUMI_CUSTOM_CLUSTER_TRV_PRESET_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &trv_schedule_mode,
        false, 1, LUMI_MANUFACTURER_CODE);
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVChildLock(
  uint8_t trv_child_lock) {

  if (_gateway && Zigbee.started()) {

    log_i("Z2S_TRVInterface::sendTRVChildLock = %d", 
          trv_child_lock);

    if ((_trv_commands_set >= saswell_cmd_set) && 
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      if (ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_id == _trv_commands_set) {

        uint8_t child_lock_dp_id = 
          ts0601_command_sets_table[_trv_commands_set].\  
            ts0601_cmd_set_child_lock_dp_id;

        uint8_t child_lock_dp_type = 
          ts0601_command_sets_table[_trv_commands_set].\
            ts0601_cmd_set_child_lock_dp_type;

        uint8_t child_lock_value;

        switch (trv_child_lock) {

          case 1: {

            child_lock_value = ts0601_command_sets_table[_trv_commands_set].\
              ts0601_cmd_set_child_lock_dp_value_on;
          } break;

          case 0: {

            child_lock_value = ts0601_command_sets_table[_trv_commands_set].
              ts0601_cmd_set_child_lock_dp_value_off;
          } break;
        }
      
        sendTuyaRequestCmdData(_gateway, 
                               &_device, 
                               child_lock_dp_id,
                               child_lock_dp_type,
                               child_lock_value);
   
      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set);
    } else 

    if (_trv_commands_set == TRVZB_CMD_SET) {

      _gateway->sendAttributeWrite(
        &_device, 
        SONOFF_CUSTOM_CLUSTER, 
        SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_BOOL, 1, &trv_child_lock);
      delay(200);

      _gateway->sendAttributeRead(
        &_device, 
        SONOFF_CUSTOM_CLUSTER, 
        SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID, 
        false);
    }
    if (_trv_commands_set == BOSCH_CMD_SET) {

      _gateway->sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &trv_child_lock);
      delay(200);

      _gateway->sendAttributeRead(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID, 
        false);
    }

    if (_trv_commands_set == EUROTRONIC_CMD_SET) {

      esp_zb_uint24_t eurotronic_host_flags;

      eurotronic_host_flags.low = (1 << 0) |
                                  (1 << ((_trv_system_mode == 0) ? 5 : 2)) |
                                  //(1 << ((trv_schedule_mode == 0) ? 2 : 4) |
                                  (1 << ((_trv_child_lock == 1) ? 7 : 0));

      log_i("EUROTRONIC host flags = 0x%04X", eurotronic_host_flags.low);
      
      _gateway->sendAttributeWrite(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_U24, 3, &eurotronic_host_flags,false, 1, 
        EUROTRONIC_MANUFACTURER_CODE);
    }

    if (_trv_commands_set == LUMI_CMD_SET) {
      

      _gateway->sendAttributeWrite(
        &_device, LUMI_CUSTOM_CLUSTER, LUMI_CUSTOM_CLUSTER_TRV_CHILD_LOCK_ID, 
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &trv_child_lock,false, 1, 
        LUMI_MANUFACTURER_CODE);
    }
    
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVTemperatureHisteresis(
  int32_t temperature_histeresis) {

  if (_gateway && Zigbee.started()) {

    log_i("temperature histeresis = %d", 
          temperature_histeresis);

    if ((_trv_commands_set >= saswell_cmd_set) &&
        (_trv_commands_set < ts0601_cmd_sets_number)) { 

      if (ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_id == _trv_commands_set) {

        if ((ts0601_command_sets_table[_trv_commands_set].
               ts0601_cmd_set_temperature_histeresis_dp_id > 0) &&
            (ts0601_command_sets_table[_trv_commands_set].
               ts0601_cmd_set_temperature_histeresis_dp_type == TUYA_DP_TYPE_VALUE)) {

          temperature_histeresis *= ts0601_command_sets_table[_trv_commands_set].
            ts0601_cmd_set_temperature_histeresis_factor;

          temperature_histeresis /= 100;
          
          sendTuyaRequestCmdValue32(
            _gateway, &_device, 
            ts0601_command_sets_table[_trv_commands_set].\
              ts0601_cmd_set_temperature_histeresis_dp_id,
            temperature_histeresis);
        }
      } else
        log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
              ts0601_command_sets_table[_trv_commands_set].ts0601_cmd_set_id,
              _trv_commands_set);
    } else 

    if (_trv_commands_set == TRVZB_CMD_SET) {

      /*_gateway->sendAttributeWrite(&_device, 
                                   SONOFF_CUSTOM_CLUSTER, 
                                   SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID, 
                                   ESP_ZB_ZCL_ATTR_TYPE_BOOL, 
                                   1, 
                                   &trv_child_lock);
      delay(200);
      _gateway->sendAttributeRead(&_device, 
                                  SONOFF_CUSTOM_CLUSTER, 
                                  SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID, 
                                  false);*/
    }

    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}


/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::sendTRVPing() {

  if (_gateway && Zigbee.started()) {

    log_i("sending ping");
 
    if ((_trv_commands_set == TRVZB_CMD_SET) ||
        (_trv_commands_set == BOSCH_CMD_SET) ||
        (_trv_commands_set == EUROTRONIC_CMD_SET) ||
        (_trv_commands_set == LUMI_CMD_SET)) {

      uint16_t attributes[5] = { 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID, 
        ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,
        ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID,
        ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID,
        ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID };

    _gateway->sendAttributesRead(
      &_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 5, &attributes[0]);
    }

    bool _trv_state_updated = _trv_system_mode_updated && 
      _trv_running_state_updated && _trv_temperature_setpoint_updated;  
      //&& _trv_local_temperature_updated;

    if ((!_trv_state_updated) && (_trv_commands_set >= saswell_cmd_set) &&
        (_trv_commands_set < ts0601_cmd_sets_number)) { 
      
      //sendTuyaQueryCmd(_gateway, &_device, false);
    }
      
    if (_last_cmd_sent_ms == 0)
      _last_cmd_sent_ms = millis();
  }
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureSetpoint(
    int32_t trv_temperature_setpoint) {

  if (_init_sequence) {

    log_i(
      "\n\rin init sequence:\n\rtrv_temperature_setpoint = %04d <-> "
      "_init_temperature_setpoint = %04d", trv_temperature_setpoint,
      _init_temperature_setpoint);

    //if (trv_temperature_setpoint == _init_temperature_setpoint) {
    if (_trv_hvac->getTemperatureSetpointHeat() == trv_temperature_setpoint) {
      
    _init_sequence = 0;
    
    _trv_temperature_setpoint = trv_temperature_setpoint;
    _trv_temperature_setpoint_updated = true;
    }
  } else {

    _trv_temperature_setpoint = trv_temperature_setpoint;
    _trv_temperature_setpoint_updated = true;
  }
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVSystemMode(
    uint8_t trv_system_mode) {

  _trv_system_mode = trv_system_mode;
  _trv_system_mode_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVRunningState(
    uint8_t trv_running_state) {

  _trv_running_state = trv_running_state;
  _trv_running_state_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVLocalTemperature(
    int32_t trv_local_temperature) {
  
  _trv_last_local_temperature = _trv_local_temperature;
  _trv_local_temperature = trv_local_temperature;
  _trv_local_temperature_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureCalibration(
    int32_t trv_temperature_calibration) {
  
  _trv_last_temperature_calibration = _trv_temperature_calibration;
  _trv_temperature_calibration = trv_temperature_calibration;
  _trv_temperature_calibration_updated = true;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVChildLock(
    uint8_t trv_child_lock) {
  
  if (_trv_child_lock != trv_child_lock)
    _trv_child_lock_changed = true;
  _trv_child_lock = trv_child_lock;
  refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::setTRVTemperatureHisteresis(
    int32_t trv_temperature_histeresis) {

    _trv_temperature_histeresis = trv_temperature_histeresis;
    _trv_temperature_histeresis_updated = true;
    refreshTimeout();
}

void Supla::Control::Z2S_TRVInterface::turnOffTRVScheduleMode() {

  _trv_switch_schedule_off = true;
}

/*****************************************************************************/

bool Supla::Control::Z2S_TRVInterface::isForcedTemperatureSet() {

  auto element = 
    Supla::Element::getElementByChannelNumber(
      _trv_hvac->getMainThermometerChannelNo());

  bool is_forced_temperature_set = false;

  if (element &&
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR)) {

    auto Supla_Z2S_VirtualThermHygroMeter = 
      reinterpret_cast<
        Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
          
    return Supla_Z2S_VirtualThermHygroMeter->isForcedTemperature();
  }

  if (element &&
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_THERMOMETER)) {

    auto Supla_Z2S_VirtualThermometer = 
      reinterpret_cast<
        Supla::Sensor::Z2S_VirtualThermometer *>(element);

    return Supla_Z2S_VirtualThermometer->isForcedTemperature();
  }

  return is_forced_temperature_set;
}

void Supla::Control::Z2S_TRVInterface::forceTRVTemperature() {

  auto element = 
    Supla::Element::getElementByChannelNumber(
      _trv_hvac->getMainThermometerChannelNo());

  if (element &&
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR)) {

    auto Supla_Z2S_VirtualThermHygroMeter = 
      reinterpret_cast\
        <Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);

    Supla_Z2S_VirtualThermHygroMeter->setForcedTemperature(
      (double)_trv_local_temperature / 100);
  } else
    if (element &&
        (element->getChannel()->getChannelType() == 
          SUPLA_CHANNELTYPE_THERMOMETER)) {

      auto Supla_Z2S_VirtualThermometer = 
        reinterpret_cast
          <Supla::Sensor::Z2S_VirtualThermometer *>(element);

      Supla_Z2S_VirtualThermometer->setForcedTemperature(
        (double)_trv_local_temperature / 100);
    }
}

/*****************************************************************************/

void Supla::Control::Z2S_TRVInterface::iterateAlways() {

  int16_t hvacLastTemperature = INT16_MIN;

  if ((_init_sequence > 0) && (millis() - _last_refresh_ms > _refresh_ms)) {

    _last_refresh_ms = millis();

    sendTRVTemperatureSetpoint(_init_temperature_setpoint);

    return; //TODO timeout control
  }

  if (_trv_switch_schedule_off) {

    _trv_switch_schedule_off = false;   
    sendTRVScheduleMode(0);

    if (_trv_hvac)
      sendTRVTemperatureSetpoint(_trv_hvac->getTemperatureSetpointHeat());     
    //sendTRVTemperatureSetpoint(_trv_hvac->getTemperatureSetpointHeat());        
  }

  if (_trv_child_lock_changed) {

    if ((_trv_hvac) && 
        ((uint8_t)_trv_hvac->getLocalUILock() != _trv_child_lock)) {

      log_i("TRV child lock difference detected hvac = %d, trv = %d", 
            (uint8_t)_trv_hvac->getLocalUILock(), 
            _trv_child_lock);
            
      if (_cooperative_child_lock)
        _trv_hvac->setLocalUILock((LocalUILock)_trv_child_lock);
      else
        sendTRVChildLock((uint8_t)_trv_hvac->getLocalUILock()); 

      _trv_child_lock_changed = false;
    }
  }

  if (_trv_temperature_histeresis_updated) {

    if ((_trv_hvac) && 
        (_trv_hvac->getTemperatureHisteresis() != _trv_temperature_histeresis)) {

      log_i("TRV temperature histeresis difference detected hvac = %d, trv = %d", 
            _trv_hvac->getTemperatureHisteresis(),
            _trv_temperature_histeresis);
      
      _trv_hvac->setTemperatureHisteresis(_trv_temperature_histeresis);

      _trv_temperature_histeresis_updated = false;
    }

  }

  if (millis() - _last_refresh_ms > _refresh_ms) {

    _last_refresh_ms = millis();

    /*if (_trv_hvac && _trv_system_mode && 
        _trv_hvac->isHvacFlagForcedOffBySensor()) {

      sendTRVSystemMode(0);
      return; //TODO timeout control
    }*/

    if (_trv_hvac && 
        ((uint8_t)_trv_hvac->getLocalUILock() != _trv_child_lock)) {

      log_i("TRV child lock difference detected hvac = %d, trv = %d", 
            (uint8_t)_trv_hvac->getLocalUILock(), _trv_child_lock);

      sendTRVChildLock((uint8_t)_trv_hvac->getLocalUILock()); 
    }

    if (_trv_temperature_histeresis_enabled &&
        _trv_hvac && 
        (_trv_hvac->getTemperatureHisteresis() != _trv_temperature_histeresis)) {

      log_i("HVAC temperature histeresis difference detected hvac = %d, trv = %d", 
            _trv_hvac->getTemperatureHisteresis(),
            _trv_temperature_histeresis);
      
      sendTRVTemperatureHisteresis(_trv_hvac->getTemperatureHisteresis());
    }


    if ((_trv_hvac) && (_trv_hvac->getMode() != SUPLA_HVAC_MODE_OFF) && 
        (_trv_hvac->getTemperatureSetpointHeat() != _trv_temperature_setpoint)) { //??
      
      log_i("setpoint difference detected: hvac = %d, trv = %d", 
            _trv_hvac->getTemperatureSetpointHeat(), 
            _trv_temperature_setpoint);

      sendTRVTemperatureSetpoint(_trv_hvac->getTemperatureSetpointHeat());        
    }
    
    if (_trv_hvac && 
        ((_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1)  != 
          _trv_system_mode)) {
      
      log_i("trv mode difference detected: hvac=%d, trv=%d", 
            _trv_hvac->getMode(), 
            _trv_system_mode);
      
      sendTRVSystemMode(_trv_hvac->getMode() == SUPLA_HVAC_MODE_OFF ? 0 : 1);        
    }

    bool external_sensor_last_state = _trv_external_sensor_present;

    if (_trv_hvac && 
        _trv_external_sensor_detection_enabled) {
      
      if (_trv_hvac->getMainThermometerChannelNo() != 
          _trv_internal_sensor_channel)
        _trv_external_sensor_present = true;
      else
        _trv_external_sensor_present = false;

      if (_trv_external_sensor_present != external_sensor_last_state)
        _trv_external_sensor_changed = true;
      else
        _trv_external_sensor_changed = false;
    }

    if (_trv_hvac)
        hvacLastTemperature = _trv_hvac->getPrimaryTemp();

    log_i("_trv_external_sensor_detection_enabled = %u\n\r"
          "_trv_external_sensor_present = %u\n\r",
          _trv_external_sensor_detection_enabled,
          _trv_external_sensor_present);

    if (_trv_external_sensor_present) { 
        
      switch (_trv_external_sensor_mode) {

      
        case EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE: {  //standard calibration

       //_trv_hvac->getLastTemperature();
        //if ((_trv_local_temperature != INT32_MIN) && (hvacLastTemperature != INT16_MIN) && 
        //    (hvacLastTemperature != _trv_local_temperature)) {

          if (_trv_last_temperature_calibration == INT32_MIN) {

            log_i("EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE present - reseting calibration to 0");
            
            sendTRVTemperatureCalibration(0);
          }

          if (abs(hvacLastTemperature - _trv_local_temperature) >= 
              _trv_temperature_calibration_trigger)
              _temperature_calibration_update_attempt++;
            else
              _temperature_calibration_update_attempt = 0;

          if (_temperature_calibration_update_attempt >= 60) { //300 seconds

            log_i("Possible calibration deadlock detected - "
                  "resetting calibration value");

            sendTRVTemperatureCalibration(0);
          }

          if ((_trv_local_temperature_updated) && 
              ((_trv_temperature_calibration_updated) || 
              (_trv_temperature_calibration == 0)) &&
              (hvacLastTemperature != INT16_MIN) && 
              (hvacLastTemperature != _trv_local_temperature) &&
              (!isForcedTemperatureSet())) {

            _last_temperature_calibration_offset = _temperature_calibration_offset;

            _temperature_calibration_offset = 
              hvacLastTemperature - (_trv_local_temperature - _trv_temperature_calibration);

            log_i("trv temperature difference detected:\n\r"
                  "hvac_temperature = %d,\n\rtrv_temperature = %d,\n\r"
                  "trv_last_temperature = %d,\n\rtrv_calibration = %d,"
                  "\n\rtrv_last_calibration = %d,\n\r"
                  "calculated offset = %d,\n\rlast calculated offset %d\n\r"
                  "_trv_temperature_calibration_trigger = %u"
                  "_temperature_calibration_update_attempt = %u", 
                  hvacLastTemperature, 
                  _trv_local_temperature,
                  _trv_last_local_temperature,
                  _trv_temperature_calibration,
                  _trv_last_temperature_calibration,
                  _temperature_calibration_offset, 
                  _last_temperature_calibration_offset,
                  _trv_temperature_calibration_trigger,
                  _temperature_calibration_update_attempt);
      
            if ((_trv_temperature_calibration_updated) && 
                (abs(_temperature_calibration_offset - _trv_temperature_calibration) >= 
                  _trv_temperature_calibration_trigger)) { 

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
        } break;

        case EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT: {
      
          if ((hvacLastTemperature != INT16_MIN) && 
              ((abs(hvacLastTemperature - _trv_local_temperature) >= 10) || 
              ((millis() - _last_external_temperature_ping_ms) > 
               _external_temperature_ping_ms))) {

            log_i("external temperature difference detected %d vs %d",
                  hvacLastTemperature,
                  _trv_local_temperature);

            _last_external_temperature_ping_ms = millis();
            sendTRVExternalSensorTemperature(hvacLastTemperature);
          }
        } break;
      }
    } else {
      
      log_i("\n\r_trv_fixed_temperature_calibration %ld"
            "\n\r_trv_temperature_calibration %ld"
            "\n\r_trv_fixed_temperature_calibration_updated %u",
             _trv_fixed_temperature_calibration,
             _trv_temperature_calibration,
             _trv_fixed_temperature_calibration_updated);

      if ((_trv_external_sensor_mode ==
            EXTERNAL_TEMPERATURE_SENSOR_USE_FIXED) &&
          /*((abs(_trv_fixed_temperature_calibration - 
                _trv_temperature_calibration) > 10) ||
          */_trv_fixed_temperature_calibration_updated) { //}) {

        sendTRVTemperatureCalibration(_trv_fixed_temperature_calibration);
        _trv_fixed_temperature_calibration_updated = false;
      }
    }

    if (_trv_external_sensor_changed && 
        //(!_trv_external_sensor_present) && 
        (_trv_external_sensor_mode == EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT)) {

      log_i("Switching external sensor temperature input: %s", 
            _trv_external_sensor_present ? "ON" : "OFF");

      sendTRVExternalSensorInput(_trv_external_sensor_present);
    }

    if (_trv_external_sensor_changed && 
        (!_trv_external_sensor_present) && 
        (_trv_external_sensor_mode == EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE)) {

      log_i("External sensor switched off - clearing calibration");

      sendTRVTemperatureCalibration(0);
    }
  }

  if (millis() - _last_temperature_ping_ms > _temperature_ping_ms) {

    _last_temperature_ping_ms = millis();

    if (_trv_local_temperature == INT32_MIN) {

     // log_i("No TRV temperature data - sending ",
       //     _trv_fixed_temperature_calibration);
            
        (-27500); //???? TODO
    }

    /*if ((_trv_last_temperature_calibration == INT32_MIN) {

      log_i("No TRV temperature calibration data - sending TemperatureCalibration with %ld value",
            _trv_fixed_temperature_calibration);
            
      sendTRVTemperatureCalibration(_trv_fixed_temperature_calibration);
    }*/
        
    
    if(_trv_hvac && 
       _trv_external_sensor_present && 
       ((_trv_hvac->getPrimaryTemp() == INT16_MIN) ||
        isForcedTemperatureSet()) && 
       (_trv_local_temperature > INT32_MIN) &&
       (_trv_external_sensor_mode == 
          EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT)) {

        forceTRVTemperature();
      }      

    if(_trv_hvac && 
       _trv_external_sensor_present && 
       ((_trv_hvac->getPrimaryTemp() == INT16_MIN) ||
        isForcedTemperatureSet()) && 
       (_trv_local_temperature > INT32_MIN) &&
       (_trv_external_sensor_mode == 
          EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE)) {
      
      log_i("No external sensor temperature data available - "
            "clearing TRV calibration");

      sendTRVTemperatureCalibration(0);


      if (_trv_temperature_calibration_updated &&
          _trv_local_temperature_updated &&
          (_trv_temperature_calibration == 0)) {

        log_i("No external sensor temperature data available - "
              "temporarily using TRV local temperature value %d", 
              _trv_local_temperature);

        forceTRVTemperature();
      }
    }
  }

  if (millis() - _last_thermostat_ping_ms > _thermostat_ping_ms) {

    _last_thermostat_ping_ms = millis();

    //log_i("sendTRVPing");
    sendTRVPing();
  }

  if (_timeout_enabled && 
      (_last_cmd_sent_ms > 0) && 
      (millis() - _last_cmd_sent_ms > _timeout_ms)) {

    if (_trv_hvac)
      _trv_hvac->getChannel()->setStateOffline();
  }
}

/*****************************************************************************/

/*void Supla::Control::Z2S_TRVInterface::handleAction(int event, int action) {

  (void)(event);

  log_i("Z2S_TRVInterface::handleAction 0x%x 0x%x", event, action);
  
  switch (action) {
    case TURN_ON: {
	  } break;
    
    case TURN_OFF: {
    } break;   
  }
}*/
