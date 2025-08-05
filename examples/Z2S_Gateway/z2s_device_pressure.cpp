#include "z2s_device_pressure.h"

void initZ2SDevicePressure(int16_t channel_number_slot) {
  
  auto Supla_VirtualPressure = new Supla::Sensor::VirtualPressure();

  Supla_VirtualPressure->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);
}

void addZ2SDevicePressure(zbg_device_params_t *device, uint8_t free_slot) {

  auto Supla_VirtualPressure = new Supla::Sensor::VirtualPressure();
  
  Z2S_fillChannelsTableSlot(device, free_slot, Supla_VirtualPressure->getChannelNumber(), 
                           SUPLA_CHANNELTYPE_PRESSURESENSOR, -1,"PRESSURE", SUPLA_CHANNELFNC_PRESSURESENSOR);
}

Supla::Sensor::VirtualPressure* getZ2SDevicePressurePtr(uint8_t Supla_channel) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if ((element != nullptr) && (element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR))
    return reinterpret_cast<Supla::Sensor::VirtualPressure *>(element);
  else return nullptr;  
}

void msgZ2SDevicePressure(int16_t channel_number_slot, double pressure, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDevicePressure - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());
  
  auto Supla_VirtualPressure = getZ2SDevicePressurePtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_VirtualPressure) {
    
    Supla_VirtualPressure->setPressure(pressure);
    //Supla_VirtualThermHygroMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}

void msgZ2SDevicePressureBatteryLevel(int16_t channel_number_slot, uint8_t battery_level, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDevicePressureBatteryLevel - invalid channel number slot");
    return;
  }
  
  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto Supla_VirtualPressure = getZ2SDevicePressurePtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_VirtualPressure) {
    
    Supla_VirtualPressure->getChannel()->setBatteryLevel(battery_level);
    //Supla_VirtualThermHygroMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}
