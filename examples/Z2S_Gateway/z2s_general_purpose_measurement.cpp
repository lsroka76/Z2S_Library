#include "z2s_device_general_purpose_measurement.h"
#include <math.h>

uint32_t uipow10(uint8_t power) {
    
    switch (power){
      case 0: return 1;
      case 1: return 10;
    }
    uint32_t result = pow(10, power);
    return result;
}

uint32_t setU32Digits(int32_t value, uint8_t first_digit, uint8_t last_digit, uint32_t insert) {
    
    uint32t result = (value/uipow10(last_digit));
    result = result * uipow10(last_digit );
    result += value % uipow10(first_digit-1);
    //printf("%lu, %lu\n\r", value, result);
    uint8_t digits = last_digit-first_digit + 1;
    for (uint8_t i =1; i <= digits; i++) {
        result += ((insert/uipow10(i-1)) % 10)*uipow10(i-1+first_digit-1);
       // printf ("%lu, %lu\n\r"//,insert, tresult);
    }
   // result += insert *pow(10, _//last_digit-first_digit-1);
    //printf ("%lu, %u", result,(uint32_t) pow(10,2));    
    return result;
}

void initZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot) {

  auto Supla_GeneralPurposeMeasurement = new Supla::Sensor::GeneralPurposeMeasurement();

  Supla_GeneralPurposeMeasurement->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
  if (z2s_devices_table[channel_number_slot].Supla_channel_name)
    Supla_GeneralPurposeMeasurement->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);
  if (z2s_devices_table[channel_number_slot].Supla_channel_func != 0)
    Supla_GeneralPurposeMeasurement->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

  switch (z2s_devices_table[channel_number_slot].model_id) {
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
      Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue("ppm"); break;

    case Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR:
    case Z2S_DEVICE_DESC_ILLUZONE_SENSOR:
      Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue("lx"); break;

    case Z2S_DEVICE_DESC_IKEA_VALLHORN_3:
      Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue("lx"); break;

    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
      switch (z2s_devices_table[channel_number_slot].sub_id) {
        case TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID:
          Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue("lx"); break;
        case TUYA_PRESENCE_SENSOR_MOTION_STATE_SID:
          Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue("[0..5]"); break;
        default: break;
      } break;
    
    case Z2S_DEVICE_DESC_MOES_ALARM: {
     // Supla_GeneralPurposeMeasurement->setValue(0);
     // Supla_GeneralPurposeMeasurement->setUnitBeforeValue("AxMxxVxDxxxx", true);
    } break;
    default: break;
  }
} 

void addZ2SDeviceGeneralPurposeMeasurement(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func, char *unit) {
  
  auto Supla_GeneralPurposeMeasurement = new Supla::Sensor::GeneralPurposeMeasurement();
  
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_GeneralPurposeMeasurement->getChannelNumber(), 
                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, sub_id, name, func);
  
  Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue(unit);
}

void msgZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot, uint8_t function, double value, signed char rssi) {

  if (channel_number_slot < 0) {
    
    log_e("error: invalid channel number slot");
    return;
  }
  log_i("channel(%u), value: %f", z2s_devices_table[channel_number_slot].Supla_channel, value);
  
  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  /*switch (z2s_devices_table[channel_number_slot].model_id) {
      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
        int8_t sub_id = -1;
        switch (function) {
          case ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE: sub_id = TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID; break;
          case ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_MOTION_STATE: sub_id = TUYA_PRESENCE_SENSOR_MOTION_STATE_SID; break;
        }
      
        int16_t func_channel_number_slot = Z2S_findChannelNumberSlot(z2s_devices_table[channel_number_slot].ieee_addr, 
                                                                      z2s_devices_table[channel_number_slot].endpoint, 
                                                                      z2s_devices_table[channel_number_slot].cluster_id, 
                                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                                      sub_id); 
        if (func_channel_number_slot < 0) {
          log_e("msgZ2SDeviceGeneralPurposeMeasurement - no channel numer slot for device model id 0x%x, function 0x%x",
                z2s_devices_table[channel_number_slot].model_id, function);
          return;
        }
        channel_number_slot = func_channel_number_slot;
      } break; 
      default: break;
  } */

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
  
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT) {

    auto Supla_GeneralPurposeMeasurement = reinterpret_cast<Supla::Sensor::GeneralPurposeMeasurement *>(element);
    Supla_GeneralPurposeMeasurement->setValue(value);
    //Supla_GeneralPurposeMeasurement->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }  
}

void msgZ2SDeviceGeneralPurposeMeasurementDisplay(int16_t channel_number_slot, uint8_t first_digit, uint8_t last_digit, uint32_t insert) {

  if (channel_number_slot < 0) {
    
    log_e("error: invalid channel number slot");
    return;
  }
  
  if ((first_digit > 10) || ((last_digit)> 10))  {
    
    log_e("error: invalid first digit position (%u) and/or last digit position (%u)", first_digit, last_digit);
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  //auto element = Supla::Element::getElementByChannelNumber(102);
  
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT) {

    auto Supla_GeneralPurposeMeasurement = reinterpret_cast<Supla::Sensor::GeneralPurposeMeasurement *>(element);
      
    /*char unitBefore[SUPLA_GENERAL_PURPOSE_UNIT_SIZE] = {};
    Supla_GeneralPurposeMeasurement->getUnitBeforeValue(unitBefore);
    memcpy (unitBefore + str_position, str_display, str_length);
    Supla_GeneralPurposeMeasurement->setValue((uint8_t)(Supla_GeneralPurposeMeasurement->getValue() + 1) % 10);
    Supla_GeneralPurposeMeasurement->setUnitBeforeValue(unitBefore, true);*/
    uint32_t gpm_value = (uint32_t)Supla_GeneralPurposeMeasurement->getValue();
    Supla_GeneralPurposeMeasurement->setValue(setU32Digits(gpm_value, first_digit, last_digit, insert));
  }  
} 
