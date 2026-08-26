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

  initZ2SDeviceGeneralPurposeMeasurement(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

/*****************************************************************************/

void initZ2SDeviceGeneralPurposeMeasurement(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  auto Supla_Z2S_GeneralPurposeMeasurement = 
    new Supla::Sensor::Z2S_GeneralPurposeMeasurement();

  Supla_Z2S_GeneralPurposeMeasurement->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

  if (_z2s_channel->Supla_channel_name)
    Supla_Z2S_GeneralPurposeMeasurement->setInitialCaption(
      _z2s_channel->Supla_channel_name);

  if (_z2s_channel->Supla_channel_func != 0)
    Supla_Z2S_GeneralPurposeMeasurement->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

  if (_z2s_channel->user_data_flags & 
        USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
    Supla_Z2S_GeneralPurposeMeasurement->getChannel()->
      setStateOfflineRemoteWakeupNotSupported();

    Supla_Z2S_GeneralPurposeMeasurement->setRWNSFlag(true);
  }

  Supla_Z2S_GeneralPurposeMeasurement->setTimeoutSecs(
    _z2s_channel->timeout_secs);

  Supla_Z2S_GeneralPurposeMeasurement->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

  Supla_Z2S_GeneralPurposeMeasurement->setZ2SChannel(
    channel_index, _z2s_channel);

  if (_z2s_channel->user_data_2 > 0)
    _z2s_channel->user_data_3 = millis();

  switch (_z2s_channel->model_id) {


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

      switch (_z2s_channel->sub_id) {


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
  Supla::Element * element, uint8_t function, double value) {
  
  if (!element) {

    log_e("missing Supla::Element object!");
    return;
  }

  auto Supla_Z2S_GeneralPurposeMeasurement = static_cast<
    Supla::Sensor::Z2S_GeneralPurposeMeasurement *>(element);

  Supla_Z2S_GeneralPurposeMeasurement->setZbDeviceLastSeenMs(millis());
  
  uint32_t gpm_time_threshold = 
    Supla_Z2S_GeneralPurposeMeasurement->getChannelUserData2() * 1000;

  uint32_t gpm_value_threshold = 
    Supla_Z2S_GeneralPurposeMeasurement->getChannelUserData1();
  
  if ((gpm_time_threshold > 0) && (value >= gpm_value_threshold)) {

    uint32_t gpm_time_delta = millis() - 
      Supla_Z2S_GeneralPurposeMeasurement->getChannelUserData3();

    log_i(
      "gpm_time_threshold: %lu, gpm_time_delta: %lu", gpm_time_threshold, 
      gpm_time_delta);

    if (gpm_time_delta < gpm_time_threshold)
      return;
    else
      Supla_Z2S_GeneralPurposeMeasurement->setChannelUserData3(millis());
  }

  switch (function) {


    case ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_INIT_VALUE: {

      Supla_Z2S_GeneralPurposeMeasurement->setValue(value);
      Supla_Z2S_GeneralPurposeMeasurement->setChannelInitialGPMValue(value);
    } break;


    case ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_DEC_VALUE: {

      double initial_gpm_value = 
        Supla_Z2S_GeneralPurposeMeasurement->getChannelInitialGPMValue();

      if (initial_gpm_value >= value)
        Supla_Z2S_GeneralPurposeMeasurement->setValue(
          initial_gpm_value - value);
    } break;


    default:

      Supla_Z2S_GeneralPurposeMeasurement->setValue(value);
    break;
  }  
}  

/*****************************************************************************/

void msgZ2SDeviceGeneralPurposeMeasurementDisplay(
  Supla::Element* element, uint8_t first_digit, uint8_t last_digit, 
  uint64_t digits_to_insert) {

  auto Supla_Z2S_GeneralPurposeMeasurement = static_cast<
    Supla::Sensor::Z2S_GeneralPurposeMeasurement *>(element);
  
  uint64_t gpm_value = (uint64_t)
    Supla_Z2S_GeneralPurposeMeasurement->getValue();
    
  Supla_Z2S_GeneralPurposeMeasurement->setValue(
    setU64Digits(gpm_value, first_digit, last_digit, digits_to_insert));  
} 
