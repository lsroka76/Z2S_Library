#include "z2s_device_pressure.h"

/*****************************************************************************/

void initZ2SDevicePressure(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel,
  Supla::Element *element) {

  Supla::Sensor::Z2S_VirtualPressure *Supla_VirtualPressure = nullptr;

  if (element) {

    Supla_VirtualPressure = static_cast<
      Supla::Sensor::Z2S_VirtualPressure *>(element);
  }
  else {

    Supla_VirtualPressure = new Supla::Sensor::Z2S_VirtualPressure();

    Supla_VirtualPressure->setZ2SChannel(channel_index, _z2s_channel);

    Supla_VirtualPressure->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);
  }

  
  if (_z2s_channel->user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
    Supla_VirtualPressure->getChannel()->
      setStateOfflineRemoteWakeupNotSupported();
      
    Supla_VirtualPressure->setRWNSFlag(true);
  }
}

/*****************************************************************************/

void addZ2SDevicePressure(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id) {

  SuplaDevice.saveStateToStorage();
  Supla::Storage::ConfigInstance()->commit();

  auto Supla_Z2S_VirtualPressure = new Supla::Sensor::Z2S_VirtualPressure();
  
  Z2S_setChannelData(
    Supla_Z2S_VirtualPressure->getZ2SCorePtr(), device, free_slot,
    Supla_Z2S_VirtualPressure->getChannelNumber(), 
    SUPLA_CHANNELTYPE_PRESSURESENSOR, sub_id, "PRESSURE", 
    SUPLA_CHANNELFNC_PRESSURESENSOR);

  initZ2SDevicePressure(
    free_slot, Supla_Z2S_VirtualPressure->getZ2SChannel(), 
    Supla_Z2S_VirtualPressure->getZ2SElementPtr());

  Supla_Z2S_VirtualPressure->getChannel()->setSubDeviceId(
    device->zb_device_id + 1);
  Supla_Z2S_VirtualPressure->getChannel()->setFlag(
    SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

  addChannelsSelectorChannel(Supla_Z2S_VirtualPressure->getZ2SCorePtr());  

  Supla_Z2S_VirtualPressure->onLoadConfig(&SuplaDevice);
  Supla_Z2S_VirtualPressure->onInit();
}

/*****************************************************************************/

void msgZ2SDevicePressure(Supla::Element *element, double pressure) {

  auto Supla_Z2S_VirtualPressure = static_cast<
    Supla::Sensor::Z2S_VirtualPressure *>(element);

  Supla_Z2S_VirtualPressure->setZbDeviceLastSeenMs(millis());  
  Supla_Z2S_VirtualPressure->setPressure(pressure);
}

/*****************************************************************************/
