#include "z2s_device_tuya_custom_cluster.h"
#include "z2s_device_dimmer.h"
#include "z2s_device_iaszone.h"
#include "z2s_device_temphumidity.h"
#include "z2s_device_general_purpose_measurement.h"
#include "z2s_device_action_trigger.h"
#include "z2s_device_hvac.h"

Tuya_read_dp_result_t Z2S_readTuyaDPvalue(uint8_t Tuya_dp_id, uint16_t payload_size, uint8_t *payload) {
  
  uint16_t payload_counter = 2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result.is_success = false;
  Tuya_read_dp_result.dp_value = 0;

  while ((payload_size >= 7) && (payload_counter < payload_size)) {
    if ((*(payload + payload_counter)) == Tuya_dp_id) {
      Tuya_read_dp_result.dp_id   = (*(payload + payload_counter));
      Tuya_read_dp_result.dp_type = (*(payload + payload_counter + 1));
      Tuya_read_dp_result.dp_size = ((uint16_t)(*(payload + payload_counter + 2))) * 0x100 + (*(payload + payload_counter + 3));
      switch (Tuya_read_dp_result.dp_size) {
        case 1: Tuya_read_dp_result.dp_value = (*(payload + payload_counter + 4)); break;
        case 2: Tuya_read_dp_result.dp_value = ((uint32_t)(*(payload + payload_counter + 4))) * 0x00000100 +
                                          ((uint32_t)(*(payload + payload_counter + 5))); break;
        case 4:  Tuya_read_dp_result.dp_value =  ((uint32_t)(*(payload + payload_counter + 4))) * 0x01000000 +
                                            ((uint32_t)(*(payload + payload_counter + 5))) * 0x00010000 +
                                            ((uint32_t)(*(payload + payload_counter + 6))) * 0x00000100 +
                                            ((uint32_t)(*(payload + payload_counter + 7))); break;
        default: {
          log_e("unrecognized Tuya DP size 0x%x", Tuya_read_dp_result.dp_size); 
          return Tuya_read_dp_result;
        }
      }
      Tuya_read_dp_result.is_success = true;
      return Tuya_read_dp_result;
    }
    else Tuya_read_dp_result.dp_size = ((uint16_t)(*(payload + payload_counter + 2))) * 0x100 + (*(payload + payload_counter + 3));
    payload_counter += 1 /*DP ID*/ + 1 /*DP TYPE*/ + 2/*DP DATA SIZE*/ + Tuya_read_dp_result.dp_size;
  }
  return Tuya_read_dp_result;
}
// HVAC data reporting                         //

void processTuyaHvacDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi, uint32_t model_id) {

  Tuya_read_dp_result_t Tuya_read_dp_result;
  uint8_t local_temperature_dp_id        =  0xFF;
  uint8_t current_heating_setpoint_dp_id =  0xFF;
  uint8_t system_mode_dp_id              =  0xFF;
  uint8_t running_state_dp_id            =  0xFF;
  uint8_t system_mode_value_on           =  0xFF;
  uint8_t system_mode_value_off          =  0xFF;
  uint8_t system_mode_value               =  0xFF;

  int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                            z2s_devices_table[channel_number_slot].endpoint, 
                                                            z2s_devices_table[channel_number_slot].cluster_id, 
                                                            SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, NO_CUSTOM_CMD_SID);

  int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                            z2s_devices_table[channel_number_slot].endpoint, 
                                                            z2s_devices_table[channel_number_slot].cluster_id, 
                                                            SUPLA_CHANNELTYPE_HVAC, NO_CUSTOM_CMD_SID);


  switch (model_id) {
    case Z2S_DEVICE_DESC_TUYA_HVAC_6567C: {
      local_temperature_dp_id         = TUYA_6567C_LOCAL_TEMPERATURE_DP; 
      current_heating_setpoint_dp_id  = TUYA_6567C_CURRENT_HEATING_SETPOINT_DP;
      system_mode_dp_id               = TUYA_6567C_SYSTEM_MODE_DP;
      running_state_dp_id             = TUYA_6567C_RUNNING_STATE_DP;
      system_mode_value_on            = 0x01;
      system_mode_value_off           = 0x00;
    } break;  
    case Z2S_DEVICE_DESC_TUYA_HVAC_23457: {
      local_temperature_dp_id         = TUYA_23457_LOCAL_TEMPERATURE_DP;
      current_heating_setpoint_dp_id  = TUYA_23457_CURRENT_HEATING_SETPOINT_DP; 
      system_mode_dp_id               = TUYA_23457_SYSTEM_MODE_DP;
      running_state_dp_id             = TUYA_23457_RUNNING_STATE_DP;
      system_mode_value_on            = 0x01;
      system_mode_value_off           = 0x02;
    } break;
    case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY: {
      local_temperature_dp_id         = TUYA_LEGACY_LOCAL_TEMPERATURE_DP;
      current_heating_setpoint_dp_id  = TUYA_LEGACY_CURRENT_HEATING_SETPOINT_DP; 
      system_mode_dp_id               = TUYA_LEGACY_SYSTEM_MODE_DP;
      running_state_dp_id             = TUYA_LEGACY_RUNNING_STATE_DP;
      system_mode_value_on            = 0x02;
      system_mode_value_off           = 0x03;
    } break;
  }
  if (local_temperature_dp_id < 0xFF)
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(local_temperature_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    msgZ2SDeviceTempHumidityTemp(channel_number_slot_1, (float)Tuya_read_dp_result.dp_value/10, rssi);
    msgZ2SDeviceHvac(channel_number_slot_2, TRV_LOCAL_TEMPERATURE_MSG, Tuya_read_dp_result.dp_value, rssi);
  }

  if (current_heating_setpoint_dp_id < 0xFF)
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(current_heating_setpoint_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    log_i("Tuya_read_dp_result.dp_value 0x%x, payload_size 0x%x, payload[9] 0x%x", Tuya_read_dp_result.dp_value, payload_size, *(payload+9));
    msgZ2SDeviceHvac(channel_number_slot_2, TRV_HEATING_SETPOINT_MSG, Tuya_read_dp_result.dp_value, rssi);
  }

  if (system_mode_dp_id < 0xFF)
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(system_mode_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    log_i("Tuya_read_dp_result.dp_value 0x%x, payload_size 0x%x, payload[9] 0x%x", Tuya_read_dp_result.dp_value, payload_size, *(payload+9));
    system_mode_value = (Tuya_read_dp_result.dp_value == system_mode_value_off) ? 0 : 1;
    msgZ2SDeviceHvac(channel_number_slot_2, TRV_SYSTEM_MODE_MSG, system_mode_value, rssi);
  }

  if (running_state_dp_id < 0xFF)
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(running_state_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    log_i("Tuya_read_dp_result.dp_value 0x%x, payload_size 0x%x, payload[9] 0x%x", Tuya_read_dp_result.dp_value, payload_size, *(payload+9));
    msgZ2SDeviceHvac(channel_number_slot_2, TRV_RUNNING_STATE_MSG, Tuya_read_dp_result.dp_value, rssi);
  }
}

void processTuyaDoubleDimmerSwitchDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  int8_t sub_id = ((*(payload + 2)) < 7) ? TUYA_DOUBLE_DIMMER_SWITCH_1_SID : TUYA_DOUBLE_DIMMER_SWITCH_2_SID; 
  
  int16_t channel_number_slot_sub_id = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                                 z2s_devices_table[channel_number_slot].endpoint, 
                                                                 z2s_devices_table[channel_number_slot].cluster_id, 
                                                                 SUPLA_CHANNELTYPE_DIMMER, sub_id);
  
  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_1_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, DIMMER_NO_LEVEL_DATA, Tuya_read_dp_result.dp_value, rssi); 

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_2_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, DIMMER_NO_LEVEL_DATA, Tuya_read_dp_result.dp_value, rssi); 
       
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_1_BRIGHTNESS_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, Tuya_read_dp_result.dp_value, false, rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_2_BRIGHTNESS_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, Tuya_read_dp_result.dp_value, false, rssi); 
}

void processTuyaSoilTempHumiditySensorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi, float divider) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SOIL_SENSOR_TEMPERATURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/divider, rssi);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SOIL_SENSOR_MOISTURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value, rssi);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SOIL_SENSOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level 0x0F is %d", Tuya_read_dp_result.dp_value);
    msgZ2SDeviceTempHumidityBatteryLevel(channel_number_slot, Tuya_read_dp_result.dp_value, rssi);  
  }
    /*Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x0E, payload_size, payload); //battery_state
    if (Tuya_read_dp_result.is_success) {
      log_i("Battery state 0x0E is %d, level %d", Tuya_read_dp_result.dp_value * 50);
      Supla_VirtualThermHygroMeter->getChannel()->setBatteryLevel(Tuya_read_dp_result.dp_value * 50);
      Supla_VirtualThermHygroMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
    }*/
}

void processTuyaTempHumiditySensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi){

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_TEMPERATURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/10, rssi);
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_HUMIDITY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value, rssi);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_BATTERY_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery state is %d", Tuya_read_dp_result.dp_value);
    msgZ2SDeviceTempHumidityBatteryLevel(channel_number_slot, Tuya_read_dp_result.dp_value * 50, rssi);  
  }
}

void processTuya3PhasesElectricityMeterDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_TEMPERATURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/10, rssi);
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_HUMIDITY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value, rssi);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_BATTERY_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery state is %d", Tuya_read_dp_result.dp_value);
    msgZ2SDeviceTempHumidityBatteryLevel(channel_number_slot, Tuya_read_dp_result.dp_value * 50, rssi);  
  }
}



void processTuyaEF00Switch2x3DataReport(int16_t channel_number_slot, uint16_t payload_size, uint8_t *payload, signed char rssi) {
  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_EF00_SWITCH_2X3_BUTTON_1_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                              z2s_devices_table[channel_number_slot].endpoint, 
                                                              z2s_devices_table[channel_number_slot].cluster_id, 
                                                              SUPLA_CHANNELTYPE_ACTIONTRIGGER, Tuya_read_dp_result.dp_value);

    msgZ2SDeviceActionTrigger(channel_number_slot_1, rssi);
  }
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_EF00_SWITCH_2X3_BUTTON_2_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                              z2s_devices_table[channel_number_slot].endpoint, 
                                                              z2s_devices_table[channel_number_slot].cluster_id, 
                                                              SUPLA_CHANNELTYPE_ACTIONTRIGGER, 3 + Tuya_read_dp_result.dp_value);

    msgZ2SDeviceActionTrigger(channel_number_slot_1, rssi);
  }          
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_EF00_SWITCH_2X3_BATTERY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery state is %d", Tuya_read_dp_result.dp_value);
    msgZ2SDeviceActionTriggerBatteryLevel(channel_number_slot, Tuya_read_dp_result.dp_value, rssi);  
  }
}

void updateSuplaBatteryLevel(int16_t channel_number_slot, uint32_t value, signed char rssi) {

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());
  
  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    
  if (element != nullptr) {
    element->getChannel()->setBatteryLevel(value);
    //element->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}

void processTuyaSmokeDetectorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  int16_t channel_number_slot_1, channel_number_slot_2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  if (z2s_devices_table[channel_number_slot].Supla_channel_type == SUPLA_CHANNELTYPE_BINARYSENSOR) {

    channel_number_slot_1 = channel_number_slot;
    channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                      z2s_devices_table[channel_number_slot].endpoint, 
                                                      z2s_devices_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, NO_CUSTOM_CMD_SID);
  }
  else {
    channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                      z2s_devices_table[channel_number_slot].endpoint, 
                                                      z2s_devices_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);
    channel_number_slot_2 = channel_number_slot;
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_SMOKE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1), rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_PPM_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_PPM, 
                                          Tuya_read_dp_result.dp_value, rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level 0x0F is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot_1, Tuya_read_dp_result.dp_value, rssi);
    updateSuplaBatteryLevel(channel_number_slot_2, Tuya_read_dp_result.dp_value, rssi);
  }
  /*Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x0E, payload_size, payload); //battery_state
    if (Tuya_read_dp_result.is_success) {
    log_i("Battery state 0x0E is %d, level %d", Tuya_read_dp_result.dp_value, Tuya_read_dp_result.dp_value * 50);
    Supla_GeneralPurposeMeasurement->getChannel()->setBatteryLevel(Tuya_read_dp_result.dp_value * 50);
  } */
}

void processTuyaPresenceSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi, uint32_t model_id) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3;

  Tuya_read_dp_result_t Tuya_read_dp_result;

  uint8_t presence_dp_id = 0xFF;
  uint8_t motion_state_dp_id = 0xFF;
  uint8_t illuminance_dp_id = 0xFF;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                    z2s_devices_table[channel_number_slot].endpoint, 
                                                    z2s_devices_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, TUYA_PRESENCE_SENSOR_PRESENCE_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                    z2s_devices_table[channel_number_slot].endpoint, 
                                                    z2s_devices_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_PRESENCE_SENSOR_MOTION_STATE_SID);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                  z2s_devices_table[channel_number_slot].endpoint, 
                                                  z2s_devices_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID);

  switch(model_id) {
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
      presence_dp_id = TUYA_PRESENCE_SENSOR_PRESENCE_DP;
      motion_state_dp_id = TUYA_PRESENCE_SENSOR_MOTION_STATE_DP;
      illuminance_dp_id = TUYA_PRESENCE_SENSOR_ILLUMINANCE_DP;
    } break;

    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5: {
      presence_dp_id = TUYA_PRESENCE_SENSOR_5_PRESENCE_DP;
      illuminance_dp_id = TUYA_PRESENCE_SENSOR_5_ILLUMINANCE_DP;
    } break;
  }
  
  if (presence_dp_id < 0xFF) 
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(presence_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1), rssi);

  if (motion_state_dp_id < 0xFF)
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(motion_state_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("MOTION STATE CHECK int %d, float %f", Tuya_read_dp_result.dp_value, Tuya_read_dp_result.dp_value);
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_MOTION_STATE,
                                          Tuya_read_dp_result.dp_value, rssi);
  }

  if (illuminance_dp_id < 0xFF)
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(illuminance_dp_id, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE,
                                          Tuya_read_dp_result.dp_value, rssi);
  
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_PRESENCE_SENSOR_BATTERY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level  is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot_1, Tuya_read_dp_result.dp_value, rssi);
    updateSuplaBatteryLevel(channel_number_slot_2, Tuya_read_dp_result.dp_value, rssi);
    updateSuplaBatteryLevel(channel_number_slot_3, Tuya_read_dp_result.dp_value, rssi);
  }
}

void processTuyaRainSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3, channel_number_slot_4, channel_number_slot_5;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                    z2s_devices_table[channel_number_slot].endpoint, 
                                                    z2s_devices_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_RAIN_SENSOR_ILLUMINANCE_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                    z2s_devices_table[channel_number_slot].endpoint, 
                                                    z2s_devices_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                  z2s_devices_table[channel_number_slot].endpoint, 
                                                  z2s_devices_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID);
  
  channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                  z2s_devices_table[channel_number_slot].endpoint, 
                                                  z2s_devices_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID);

  channel_number_slot_5 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                  z2s_devices_table[channel_number_slot].endpoint, 
                                                  z2s_devices_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_ILLUMINANCE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_1, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value, rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
  { log_i("MOTION STATE CHECK int %d, float %f", Tuya_read_dp_result.dp_value, Tuya_read_dp_result.dp_value);
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value, rssi);}

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value, rssi);
  
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_RAIN_INTENSITY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_4, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value, rssi);
    if (z2s_devices_table[channel_number_slot_4].user_data_1 > 0) {
      if (Tuya_read_dp_result.dp_value > z2s_devices_table[channel_number_slot_4].user_data_1)
        msgZ2SDeviceIASzone(channel_number_slot_5, true, rssi, false); 
      else
        msgZ2SDeviceIASzone(channel_number_slot_5, false, rssi, false); 
    }    
  }
}

void processTuyaDataReport(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t payload_size, uint8_t *payload, signed char rssi) {

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, TUYA_PRIVATE_CLUSTER_EF00, 
                                                          ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID); //first find anything to recognize model_id
  if (channel_number_slot < 0) {
    log_i("processTuyaDataReport failed - no Supla channel for that device");
    return;
  }
  uint32_t model_id = z2s_devices_table[channel_number_slot].model_id;

  switch (model_id) {
    case Z2S_DEVICE_DESC_TUYA_HVAC_6567C: 
    case Z2S_DEVICE_DESC_TUYA_HVAC_23457:
    case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY:
      processTuyaHvacDataReport(channel_number_slot, payload_size, payload, rssi, model_id); break;

    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: 
      processTuyaDoubleDimmerSwitchDataReport(channel_number_slot, payload_size, payload, rssi); break;

    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR:
      processTuyaSoilTempHumiditySensorReport(channel_number_slot, payload_size, payload, rssi, 10); break;

    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
      processTuyaSoilTempHumiditySensorReport(channel_number_slot, payload_size, payload, rssi, 1); break;

    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR: 
      processTuyaTempHumiditySensorDataReport(channel_number_slot, payload_size, payload, rssi); break;

    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR: 
      processTuyaSmokeDetectorReport(channel_number_slot, payload_size, payload, rssi); break;

    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: 
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5:
      processTuyaPresenceSensorDataReport(channel_number_slot, payload_size, payload, rssi, model_id); break;

    case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3:
      processTuyaEF00Switch2x3DataReport(channel_number_slot, payload_size, payload, rssi); break;

    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR:
      processTuyaRainSensorDataReport(channel_number_slot, payload_size, payload, rssi); break;

    /*case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER:
      processTuya3PhasesElectricityMeterDataReport(channel_number_slot, payload_size, payload, rssi); break;*/

    default: 
      log_i("Unknown device model id 0x%x", z2s_devices_table[channel_number_slot].model_id); break;
  }
}

void processTuyaCustomCluster(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, uint16_t payload_size, uint8_t *payload, signed char rssi) {
  log_i("processing Tuya custom cluster 0xEF00, command id 0x%x", command_id);
  switch (command_id) {
    case TUYA_DATA_REPORT_CMD:
    case 0x01:
    case 0x06:
       processTuyaDataReport(ieee_addr, endpoint, payload_size, payload, rssi); break;
    default: log_i("Tuya custom cluster 0xEF00 command id 0x%x wasn't processed", command_id); break;
  }
}
