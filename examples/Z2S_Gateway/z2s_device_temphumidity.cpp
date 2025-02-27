#include "z2s_device_temphumidity.h"

void initZ2SDeviceTempHumidity(int16_t channel_number_slot) {
  
  auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();

  Supla_VirtualThermHygroMeter->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
}

void addZ2SDeviceTempHumidity(zbg_device_params_t *device, uint8_t free_slot) {

  auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();
  
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_VirtualThermHygroMeter->getChannelNumber(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, -1);
}

Supla::Sensor::VirtualThermHygroMeter* getZ2SDeviceTempHumidityPtr(uint8_t Supla_channel) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR)  
    return reinterpret_cast<Supla::Sensor::VirtualThermHygroMeter *>(element);
  else return nullptr;  
}

void msgZ2SDeviceTempHumidityTemp(int16_t channel_number_slot, double temp, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityTemp - invalid channel number slot");
    return;
  }

  auto Supla_VirtualThermHygroMeter = getZ2SDeviceTempHumidityPtr(z2s_devices_table[channel_number_slot].Supla_channel);
  
  if (Supla_VirtualThermHygroMeter) {
    
    Supla_VirtualThermHygroMeter->setTemp(temp);
    //Supla_VirtualThermHygroMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}

void msgZ2SDeviceTempHumidityHumi(int16_t channel_number_slot, double humi, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityHumi - invalid channel number slot");
    return;
  }

  auto Supla_VirtualThermHygroMeter = getZ2SDeviceTempHumidityPtr(z2s_devices_table[channel_number_slot].Supla_channel);
  
  if (Supla_VirtualThermHygroMeter) {
    
    Supla_VirtualThermHygroMeter->setHumi(humi);
    //Supla_VirtualThermHygroMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}

void msgZ2SDeviceTempHumidityBatteryLevel(int16_t channel_number_slot, uint8_t battery_level, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityBatteryLevel - invalid channel number slot");
    return;
  }
  
  auto Supla_VirtualThermHygroMeter = getZ2SDeviceTempHumidityPtr(z2s_devices_table[channel_number_slot].Supla_channel);
  
  if (Supla_VirtualThermHygroMeter) {
    
    Supla_VirtualThermHygroMeter->getChannel()->setBatteryLevel(battery_level);
    //Supla_VirtualThermHygroMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}
