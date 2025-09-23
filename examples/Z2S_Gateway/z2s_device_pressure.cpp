#include "z2s_device_pressure.h"

void initZ2SDevicePressure(int16_t channel_number_slot) {
  
  auto Supla_VirtualPressure = new Supla::Sensor::Z2S_VirtualPressure();

  Supla_VirtualPressure->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (z2s_channels_table[channel_number_slot].user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_VirtualPressure->getChannel()->setStateOfflineRemoteWakeupNotSupported();
      Supla_VirtualPressure->setRWNSFlag(true);
    }

    Supla_VirtualPressure->setTimeoutSecs(z2s_channels_table[channel_number_slot].timeout_secs);
}


void addZ2SDevicePressure(zbg_device_params_t *device, uint8_t free_slot) {

  auto Supla_Z2S_VirtualPressure = new Supla::Sensor::Z2S_VirtualPressure();
  
  Z2S_fillChannelsTableSlot(device, 
                            free_slot, 
                            Supla_Z2S_VirtualPressure->getChannelNumber(), 
                            SUPLA_CHANNELTYPE_PRESSURESENSOR, 
                            NO_CUSTOM_CMD_SID,
                            "PRESSURE", 
                            SUPLA_CHANNELFNC_PRESSURESENSOR);
}

Supla::Sensor::Z2S_VirtualPressure* getZ2SDevicePressurePtr(uint8_t Supla_channel) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if ((element != nullptr) && (element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_PRESSURESENSOR))

    return reinterpret_cast<Supla::Sensor::Z2S_VirtualPressure *>(element);
  
  else 
  
    return nullptr;  
}

void msgZ2SDevicePressure(int16_t channel_number_slot, double pressure) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDevicePressure - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());
  
  auto Supla_Z2S_VirtualPressure = getZ2SDevicePressurePtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_Z2S_VirtualPressure) {
    
    Supla_Z2S_VirtualPressure->setPressure(pressure);
    Supla_Z2S_VirtualPressure->Refresh();
  }
}

/*void msgZ2SDevicePressureBatteryLevel(int16_t channel_number_slot, uint8_t battery_level) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDevicePressureBatteryLevel - invalid channel number slot");
    return;
  }
  
  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto Supla_Z2S_VirtualPressure = getZ2SDevicePressurePtr(z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_Z2S_VirtualPressure) {
    
    Supla_Z2S_VirtualPressure->getChannel()->setBatteryLevel(battery_level);
    Supla_Z2S_VirtualPressure->Refresh();
  }
}*/
