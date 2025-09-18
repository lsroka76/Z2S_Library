#include "z2s_device_temphumidity.h"

void initZ2SDeviceTempHumidity(int16_t channel_number_slot) {
  
  uint8_t timeout = z2s_channels_table[channel_number_slot].timeout_secs / 3600;
  //if (z2s_channels_table[channel_number_slot].user_data_flags & USER_DATA_FLAG_SED_TIMEOUT == USER_DATA_FLAG_SED_TIMEOUT)
  //  timeout = z2s_channels_table[channel_number_slot].user_data_1;
  auto Supla_Z2S_VirtualThermHygroMeter = new Supla::Sensor::Z2S_VirtualThermHygroMeter(timeout);

  Supla_Z2S_VirtualThermHygroMeter->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_VirtualThermHygroMeter->setInitialCaption(z2s_channels_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_channels_table[channel_number_slot].Supla_channel_func != 0) 
    Supla_Z2S_VirtualThermHygroMeter->setDefaultFunction(z2s_channels_table[channel_number_slot].Supla_channel_func);

  if (z2s_channels_table[channel_number_slot].user_data_flags & USER_DATA_FLAG_CORRECTIONS_DISABLED)
    Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(false);
  else
    Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(true);
  
}

void addZ2SDeviceTempHumidity(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func) {

  auto Supla_Z2S_VirtualThermHygroMeter = new Supla::Sensor::Z2S_VirtualThermHygroMeter();

  if (name == nullptr)
    name = (char*)default_th_name;
  
  Z2S_fillChannelsTableSlot(device, free_slot, Supla_Z2S_VirtualThermHygroMeter->getChannelNumber(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, sub_id,
                           name, func);
}

Supla::Sensor::Z2S_VirtualThermHygroMeter* getZ2SDeviceTempHumidityPtr(uint8_t Supla_channel) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR)  
    return reinterpret_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
  else return nullptr;  
}

void msgZ2SDeviceTempHumidityTemp(int16_t channel_number_slot, double temp) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityTemp - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto Supla_Z2S_VirtualThermHygroMeter = getZ2SDeviceTempHumidityPtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_Z2S_VirtualThermHygroMeter) {
    
    Supla_Z2S_VirtualThermHygroMeter->setTemp(temp);
    Supla_Z2S_VirtualThermHygroMeter->Refresh();
  }
}

void msgZ2SDeviceTempHumidityHumi(int16_t channel_number_slot, double humi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityHumi - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto Supla_Z2S_VirtualThermHygroMeter = getZ2SDeviceTempHumidityPtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_Z2S_VirtualThermHygroMeter) {
    
    switch (z2s_channels_table[channel_number_slot].model_id) {
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10: humi *= 10; break;
      default: break;
    }
    Supla_Z2S_VirtualThermHygroMeter->setHumi(humi);
    Supla_Z2S_VirtualThermHygroMeter->Refresh();
  }
}

/*void msgZ2SDeviceTempHumidityBatteryLevel(int16_t channel_number_slot, uint8_t battery_level) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityBatteryLevel - invalid channel number slot");
    return;
  }
  
  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto Supla_Z2S_VirtualThermHygroMeter = getZ2SDeviceTempHumidityPtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_Z2S_VirtualThermHygroMeter) {
    
    Supla_Z2S_VirtualThermHygroMeter->getChannel()->setBatteryLevel(battery_level);
    Supla_Z2S_VirtualThermHygroMeter->Refresh();
  }
}*/
