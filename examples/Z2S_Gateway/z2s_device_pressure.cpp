#include "z2s_device_pressure.h"

/*****************************************************************************/

void initZ2SDevicePressure(int16_t channel_number_slot) {

  initZ2SDevicePressure(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

/*****************************************************************************/

void initZ2SDevicePressure(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  auto Supla_VirtualPressure = 
    new Supla::Sensor::Z2S_VirtualPressure();

  Supla_VirtualPressure->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

  Supla_VirtualPressure->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

  Supla_VirtualPressure->setZ2SChannel(channel_index, _z2s_channel);

  if (_z2s_channel->user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
    Supla_VirtualPressure->getChannel()->
      setStateOfflineRemoteWakeupNotSupported();
      
    Supla_VirtualPressure->setRWNSFlag(true);
  }

  Supla_VirtualPressure->setTimeoutSecs(_z2s_channel->timeout_secs);
}

/*****************************************************************************/

void addZ2SDevicePressure(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id) {

  auto Supla_Z2S_VirtualPressure = 
    new Supla::Sensor::Z2S_VirtualPressure();
  
  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_VirtualPressure->getChannelNumber(), 
    SUPLA_CHANNELTYPE_PRESSURESENSOR, sub_id, "PRESSURE", 
    SUPLA_CHANNELFNC_PRESSURESENSOR);
}

/*****************************************************************************/

Supla::Sensor::Z2S_VirtualPressure* getZ2SDevicePressurePtr(uint8_t Supla_channel) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element && (element->getChannel()->getChannelType() == 
      SUPLA_CHANNELTYPE_PRESSURESENSOR))

    return 
      reinterpret_cast<Supla::Sensor::Z2S_VirtualPressure *>(element);
  
  else 
  
    return 
      nullptr;  
}

/*****************************************************************************/

void msgZ2SDevicePressure(Supla::Element *element, double pressure) {

  auto Supla_Z2S_VirtualPressure = static_cast<
    Supla::Sensor::Z2S_VirtualPressure *>(element);

  Supla_Z2S_VirtualPressure->setZbDeviceLastSeenMs(millis());  
  Supla_Z2S_VirtualPressure->setPressure(pressure);
}

/*****************************************************************************/
