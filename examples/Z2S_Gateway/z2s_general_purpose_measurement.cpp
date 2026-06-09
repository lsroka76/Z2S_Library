#include "z2s_device_general_purpose_measurement.h"
#include <math.h>

/*****************************************************************************/

uint64_t uipow10(uint8_t power) {
    
    switch (power){
      case 0: return 1;
      case 1: return 10;
    }
    uint64_t result = pow(10, power);
    return result;
}

/*****************************************************************************/

uint64_t setU64Digits(
  int64_t value, uint8_t first_digit, uint8_t last_digit, 
  uint64_t digits_to_insert) {
    
    uint64_t result = (value/uipow10(last_digit));
    result = result * uipow10(last_digit );
    result += value % uipow10(first_digit-1);
    //printf("%lu, %lu\n\r", value, result);
    uint8_t digits = last_digit-first_digit + 1;
    for (uint8_t i =1; i <= digits; i++) {
        result += ((digits_to_insert/uipow10(i-1)) % 10)*uipow10(i-1+first_digit-1);
       // printf ("%lu, %lu\n\r"//,insert, tresult);
    }
   // result += insert *pow(10, _//last_digit-first_digit-1);
    //printf ("%lu, %u", result,(uint32_t) pow(10,2));    
    return result;
}

/*****************************************************************************/

void initZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot) {

  auto Supla_Z2S_GeneralPurposeMeasurement = 
    new Supla::Sensor::Z2S_GeneralPurposeMeasurement();

  Supla_Z2S_GeneralPurposeMeasurement->getChannel()->setChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);

  if (z2s_channels_table[channel_number_slot].Supla_channel_name)
    Supla_Z2S_GeneralPurposeMeasurement->setInitialCaption(
      z2s_channels_table[channel_number_slot].Supla_channel_name);

  if (z2s_channels_table[channel_number_slot].Supla_channel_func != 0)
    Supla_Z2S_GeneralPurposeMeasurement->setDefaultFunction(
      z2s_channels_table[channel_number_slot].Supla_channel_func);

  if (z2s_channels_table[channel_number_slot].user_data_flags & 
        USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
    Supla_Z2S_GeneralPurposeMeasurement->getChannel()->
      setStateOfflineRemoteWakeupNotSupported();

    Supla_Z2S_GeneralPurposeMeasurement->setRWNSFlag(true);
  }

  Supla_Z2S_GeneralPurposeMeasurement->setTimeoutSecs(
    z2s_channels_table[channel_number_slot].timeout_secs);

  Supla_Z2S_GeneralPurposeMeasurement->setZ2SZbDevice(
    Z2S_getChannelZbDevicePtr(channel_number_slot));

  Supla_Z2S_GeneralPurposeMeasurement->setZ2SChannel(
    Z2S_getChannelPtr(channel_number_slot));

  if (z2s_channels_table[channel_number_slot].user_data_2 > 0)
    z2s_channels_table[channel_number_slot].user_data_3 = millis();

  switch (z2s_channels_table[channel_number_slot].model_id) {


    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
      
      Supla_Z2S_GeneralPurposeMeasurement->setDefaultUnitAfterValue("ppm"); 
    break;


    case Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_ILLUZONE_SENSOR:

      Supla_Z2S_GeneralPurposeMeasurement->setDefaultUnitAfterValue("lx"); 
    break;


    case Z2S_DEVICE_DESC_IKEA_VALLHORN_3:

      Supla_Z2S_GeneralPurposeMeasurement->setDefaultUnitAfterValue("lx"); 
    break;


    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:

      switch (z2s_channels_table[channel_number_slot].sub_id) {


        case TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID:

          Supla_Z2S_GeneralPurposeMeasurement->setDefaultUnitAfterValue("lx"); 
        break;


        case TUYA_PRESENCE_SENSOR_MOTION_STATE_SID:
          
          Supla_Z2S_GeneralPurposeMeasurement->setDefaultUnitAfterValue(
            "[0..5]"); 
        break;


        default: break;
      } break;
    

    case Z2S_DEVICE_DESC_MOES_ALARM: {
      
      Supla_Z2S_GeneralPurposeMeasurement->setValue(90000000000);
      //Supla_GeneralPurposeMeasurement->setUnitBeforeValue("AxMxxVxDxxxx", true);
    } break;


    case Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER: {
      
      Supla_Z2S_GeneralPurposeMeasurement->setValuePrecision(3, true);
      Supla_Z2S_GeneralPurposeMeasurement->setValueDivider(1000000, true);
      Supla_Z2S_GeneralPurposeMeasurement->setUnitAfterValue("kWh", true);
    } break;


    case Z2S_DEVICE_DESC_DIY_BATTERY_CHARGING_SENSOR: {
      
      Supla_Z2S_GeneralPurposeMeasurement->setValuePrecision(2, true);
      Supla_Z2S_GeneralPurposeMeasurement->setUnitAfterValue("V", true);
    } break;


    default: 
    break;
  }

  //if (channel_number_slot == 1)
  //  Z2S_initChannelExtendedDataCounter(channel_number_slot);
} 

/*****************************************************************************/

void addZ2SDeviceGeneralPurposeMeasurement(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t func, const char *unit) {
  
  auto Supla_Z2S_GeneralPurposeMeasurement = 
    new Supla::Sensor::Z2S_GeneralPurposeMeasurement();
  
  if (name == nullptr)
    name = (char*)default_gpm_name;

  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_GeneralPurposeMeasurement->getChannelNumber(), 
    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, sub_id, name, func);
  
  Supla_Z2S_GeneralPurposeMeasurement->setDefaultUnitAfterValue(unit);
}

/*****************************************************************************/

void msgZ2SDeviceGeneralPurposeMeasurement(
  int16_t channel_number_slot, uint8_t function, double value) {

  if (channel_number_slot < 0) {
    
    log_e("error: invalid channel number slot");
    return;
  }
  log_i("channel(%u), value: %f", 
        z2s_channels_table[channel_number_slot].Supla_channel, 
        value);
  
  /*Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].short_addr, millis());*/

  /*switch (z2s_channels_table[channel_number_slot].model_id) {
      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
        int8_t sub_id = -1;
        switch (function) {
          case ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE: sub_id = TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID; break;
          case ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_MOTION_STATE: sub_id = TUYA_PRESENCE_SENSOR_MOTION_STATE_SID; break;
        }
      
        int16_t func_channel_number_slot = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                                      sub_id); 
        if (func_channel_number_slot < 0) {
          log_e("msgZ2SDeviceGeneralPurposeMeasurement - no channel numer slot for device model id 0x%x, function 0x%x",
                z2s_channels_table[channel_number_slot].model_id, function);
          return;
        }
        channel_number_slot = func_channel_number_slot;
      } break; 
      default: break;
  } */

  uint32_t gpm_time_threshold = 
    z2s_channels_table[channel_number_slot].user_data_2;

    uint32_t gpm_value_threshold = 
    z2s_channels_table[channel_number_slot].user_data_1;
  
  if ((gpm_time_threshold > 0) &&
  (value >= gpm_value_threshold)) {

    uint32_t gpm_time_delta = 
      millis() - z2s_channels_table[channel_number_slot].user_data_3;

    log_i(
      "gpm_time_threshold: %lu, gpm_time_delta: %lu", 
      gpm_time_threshold * 1000, gpm_time_delta);

    if (gpm_time_delta < gpm_time_threshold * 1000)
    return;
    else
      z2s_channels_table[channel_number_slot].user_data_3 = millis();
  }
  auto element = Supla::Element::getElementByChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (element && 
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT)) {

    auto Supla_Z2S_GeneralPurposeMeasurement = 
      reinterpret_cast<
        Supla::Sensor::Z2S_GeneralPurposeMeasurement *>(element);

    Supla_Z2S_GeneralPurposeMeasurement->setValue(value);
    //Z2S_setChannelExtendedDataCounter(channel_number_slot, value);
  }  
}

/*****************************************************************************/

void msgZ2SDeviceGeneralPurposeMeasurementDisplay(
  int16_t channel_number_slot, uint8_t first_digit, uint8_t last_digit, 
  uint64_t digits_to_insert) {

  if (channel_number_slot < 0) {
    
    log_e("error: invalid channel number slot");
    return;
  }
  
  if ((first_digit > 10) || ((last_digit)> 10))  {
    
    log_e(
      "error: invalid first digit position (%u) and/or last digit position (%u)", 
          first_digit, last_digit);
    return;
  }

  /*Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].short_addr, millis());*/

  auto element = 
    Supla::Element::getElementByChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

  //auto element = Supla::Element::getElementByChannelNumber(102);
  
  if (element && 
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT)) {

    auto Supla_Z2S_GeneralPurposeMeasurement = 
      reinterpret_cast<
        Supla::Sensor::Z2S_GeneralPurposeMeasurement *>(element);
      
    /*char unitBefore[SUPLA_GENERAL_PURPOSE_UNIT_SIZE] = {};
    Supla_GeneralPurposeMeasurement->getUnitBeforeValue(unitBefore);
    memcpy (unitBefore + str_position, str_display, str_length);
    Supla_GeneralPurposeMeasurement->setValue((uint8_t)(Supla_GeneralPurposeMeasurement->getValue() + 1) % 10);
    Supla_GeneralPurposeMeasurement->setUnitBeforeValue(unitBefore, true);*/
    uint64_t gpm_value = 
      (uint64_t)Supla_Z2S_GeneralPurposeMeasurement->getValue();
    
    Supla_Z2S_GeneralPurposeMeasurement->setValue(
      setU64Digits(gpm_value, first_digit, last_digit, digits_to_insert));
  }  
} 
