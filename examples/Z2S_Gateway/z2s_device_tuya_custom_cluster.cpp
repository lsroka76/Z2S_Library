#include "z2s_device_tuya_custom_cluster.h"
#include "z2s_device_dimmer.h"
#include "z2s_device_iaszone.h"
#include "z2s_device_temphumidity.h"
#include "z2s_device_general_purpose_measurement.h"


Tuya_read_dp_result_t Z2S_readTuyaDPvalue(uint8_t Tuya_dp_id, uint16_t payload_size, uint8_t *payload) {
  
  uint16_t payload_counter = 2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result.is_success = false;
  Tuya_read_dp_result.dp_value = 0;

  while ((payload_size >= 7) && (payload_counter < payload_size)) {
    if ((*(payload + payload_counter)) == Tuya_dp_id) {
      Tuya_read_dp_result.dp_id   = (*(payload + payload_counter));
      Tuya_read_dp_result.dp_type = (*(payload + payload_counter + 1));
      Tuya_read_dp_result.dp_size = ((uint16_t)(*(payload + payload_counter + 2))) * 0xFF + (*(payload + payload_counter + 3));
      switch (Tuya_read_dp_result.dp_size) {
        case 1: Tuya_read_dp_result.dp_value = (*(payload + payload_counter + 4)); break;
        case 2: Tuya_read_dp_result.dp_value = ((uint32_t)(*(payload + payload_counter + 4))) * 0x000000FF +
                                          ((uint32_t)(*(payload + payload_counter + 5))); break;
        case 4:  Tuya_read_dp_result.dp_value =  ((uint32_t)(*(payload + payload_counter + 4))) * 0x00FF0000 +
                                            ((uint32_t)(*(payload + payload_counter + 5))) * 0x0000FF00 +
                                            ((uint32_t)(*(payload + payload_counter + 6))) * 0x000000FF +
                                            ((uint32_t)(*(payload + payload_counter + 7))); break;
        default: {
          log_e("unrecognized Tuya DP size 0x%x", Tuya_read_dp_result.dp_size); 
          return Tuya_read_dp_result;
        }
      }
      Tuya_read_dp_result.is_success = true;
      return Tuya_read_dp_result;
    }
    else Tuya_read_dp_result.dp_size = ((uint16_t)(*(payload + payload_counter + 2))) * 0xFF + (*(payload + payload_counter + 3));
    payload_counter += 1 /*DP ID*/ + 1 /*DP TYPE*/ + 2/*DP DATA SIZE*/ + Tuya_read_dp_result.dp_size;
  }
  return Tuya_read_dp_result;
}

void processTuyaHvacDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi){

}

void processTuyaDoubleDimmerSwitchDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  int8_t sub_id = ((*(payload + 2)) < 7) ? 1 : 2; 
  
  int16_t channel_number_slot_sub_id = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                                 z2s_devices_table[channel_number_slot].endpoint, 
                                                                 z2s_devices_table[channel_number_slot].cluster_id, 
                                                                 SUPLA_CHANNELTYPE_DIMMER, sub_id);
  
  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x01/*dimmer_1_state*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, -1, Tuya_read_dp_result.dp_value, rssi); 
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x07/*dimmer_2_state*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, -1, Tuya_read_dp_result.dp_value, rssi); 
       
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x02/*dimmer_1_brightness*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, Tuya_read_dp_result.dp_value, false, rssi); 
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x08/*dimmer_2_brightness*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, Tuya_read_dp_result.dp_value, false, rssi); 
}

void processTuyaSoilTempHumiditySensorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x05/*temperature*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/10, rssi);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x03/*soil moisture*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value, rssi);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x0F/*battery level*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level 0x0F is %d", Tuya_read_dp_result.dp_value);
    msgZ2SDeviceTempHumidityBatteryLevel(channel_number_slot,(float)Tuya_read_dp_result.dp_value, rssi);  
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

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x01/*temperature*/, payload_size, payload);
    if (Tuya_read_dp_result.is_success)
      msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/10, rssi);
            
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x02/*humidity*/, payload_size, payload);
    if (Tuya_read_dp_result.is_success)
      msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value, rssi);  
            
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x04/*battery level*/, payload_size, payload);
    if (Tuya_read_dp_result.is_success) { 
      log_i("Battery level 0x04 is %d", Tuya_read_dp_result.dp_value);
      msgZ2SDeviceTempHumidityBatteryLevel(channel_number_slot,(float)Tuya_read_dp_result.dp_value, rssi);  
    }
}

void updateSuplaBatteryLevel(int16_t channel_number_slot, uint32_t value, signed char rssi) {

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    
  if (element != nullptr) {
    element->getChannel()->setBatteryLevel(value);
    element->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
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

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x01/*smoke detected*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1), rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x02/*smoke concentration*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, Tuya_read_dp_result.dp_value, rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x0F/*battery level*/, payload_size, payload);
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

void processTuyaPresenceSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, signed char rssi) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                    z2s_devices_table[channel_number_slot].endpoint, 
                                                    z2s_devices_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                    z2s_devices_table[channel_number_slot].endpoint, 
                                                    z2s_devices_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 0x65);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                  z2s_devices_table[channel_number_slot].endpoint, 
                                                  z2s_devices_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 0x6A);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x01/*presence*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1), rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x65/*motion state*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, Tuya_read_dp_result.dp_value, rssi);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(0x6A/*illuminance*/, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, Tuya_read_dp_result.dp_value, rssi);
}

void processTuyaDataReport(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t payload_size, uint8_t *payload, signed char rssi) {

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, TUYA_PRIVATE_CLUSTER_EF00, 
                                                          ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID); //first find anything to recognize model_id
  if (channel_number_slot < 0) {
    log_i("processTuyaDataReport failed - no Supla channel for that device");
    return;
  }
  switch (z2s_devices_table[channel_number_slot].model_id) {
    case Z2S_DEVICE_DESC_TUYA_HVAC_6567C: 
      processTuyaHvacDataReport(channel_number_slot, payload_size, payload, rssi); break;
    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: 
      processTuyaDoubleDimmerSwitchDataReport(channel_number_slot, payload_size, payload, rssi); break;
    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR: 
      processTuyaSoilTempHumiditySensorReport(channel_number_slot, payload_size, payload, rssi); break;
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR: 
      processTuyaTempHumiditySensorDataReport(channel_number_slot, payload_size, payload, rssi); break;
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR: 
      processTuyaSmokeDetectorReport(channel_number_slot, payload_size, payload, rssi); break;
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: 
      processTuyaPresenceSensorDataReport(channel_number_slot, payload_size, payload, rssi); break;
    default: 
      log_i("Unknown device model id 0x%x", z2s_devices_table[channel_number_slot].model_id); break;
  }
}

void processTuyaCustomCluster(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, uint16_t payload_size, uint8_t *payload, signed char rssi) {
  log_i("processing Tuya custom cluster 0xEF00, command id 0x%x", command_id);
  switch (command_id) {
    case TUYA_DATA_REPORT_CMD: processTuyaDataReport(ieee_addr, endpoint, payload_size, payload, rssi); break;
    default: log_i("Tuya custom cluster 0xEF00 command id 0x%x wasn't processed", command_id); break;
  }
}