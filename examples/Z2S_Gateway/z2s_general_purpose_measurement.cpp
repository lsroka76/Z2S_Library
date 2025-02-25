#include "z2s_device_general_purpose_measurement.h"

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

    case Z2S_DEVICE_DESC_ILLUTEMPHUMIZONE_SENSOR:
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
      }
    default: break;
  }
} 

void addZ2SDeviceGeneralPurposeMeasurement(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func, char *unit) {
  
  auto Supla_GeneralPurposeMeasurement = new Supla::Sensor::GeneralPurposeMeasurement();
  
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_GeneralPurposeMeasurement->getChannelNumber(), 
                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, sub_id, name, func);
  
  Supla_GeneralPurposeMeasurement->setDefaultUnitAfterValue(unit);
}

void msgZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot, double value, signed char rssi) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceGeneralPurposeMeasurement - invalid channel number slot");
    return;
  }

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
  
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT) {

    auto Supla_GeneralPurposeMeasurement = reinterpret_cast<Supla::Sensor::GeneralPurposeMeasurement *>(element);
    Supla_GeneralPurposeMeasurement->setValue(value);
    Supla_GeneralPurposeMeasurement->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }  
}

