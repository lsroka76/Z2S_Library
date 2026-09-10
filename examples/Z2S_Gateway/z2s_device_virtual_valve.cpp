#include "z2s_device_virtual_valve.h"

/*****************************************************************************/

void initZ2SDeviceVirtualValve(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel, 
  Supla::Element *element) {

  
  uint8_t z2s_function = Z2S_VIRTUAL_VALVE_FNC_DEFAULT_ON_OFF;

  switch (_z2s_channel->model_id) {


    case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY: 
      
      z2s_function = Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY; 
    break;
  }

  Supla::Control::Z2S_VirtualValve *Supla_Z2S_VirtualValve = nullptr;

  if (element) {

    Supla_Z2S_VirtualValve = static_cast<
      Supla::Control::Z2S_VirtualValve *>(element);
  }
  else {

    Supla_Z2S_VirtualValve = new Supla::Control::Z2S_VirtualValve(true);
    
    Supla_Z2S_VirtualValve->setZ2SChannel(channel_index, _z2s_channel);

    Supla_Z2S_VirtualValve->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

    Supla_Z2S_VirtualValve->setInitialCaption(
      _z2s_channel->Supla_channel_name);
    Supla_Z2S_VirtualValve->setDefaultFunction(
      _z2s_channel->Supla_channel_func);
  }

  Supla_Z2S_VirtualValve->setZ2SFunction(z2s_function);
}

/*****************************************************************************/
                                      
void addZ2SDeviceVirtualValve(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id, const char *name, uint32_t func) {

  SuplaDevice.saveStateToStorage();
  Supla::Storage::ConfigInstance()->commit();

  auto Supla_Z2S_VirtualValve = new Supla::Control::Z2S_VirtualValve(true);

  if (name) 
    Supla_Z2S_VirtualValve->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_VirtualValve->setDefaultFunction(func);
  
  Z2S_setChannelData(
    Supla_Z2S_VirtualValve->getZ2SCorePtr(), device, free_slot,
    Supla_Z2S_VirtualValve->getChannelNumber(), 
    SUPLA_CHANNELTYPE_VALVE_OPENCLOSE, sub_id, name, func);

  initZ2SDeviceVirtualValve(
    free_slot, Supla_Z2S_VirtualValve->getZ2SChannel(), 
    Supla_Z2S_VirtualValve->getZ2SElementPtr());

    Supla_Z2S_VirtualValve->getChannel()->setSubDeviceId(
      device->zb_device_id + 1);
    Supla_Z2S_VirtualValve->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(
      Supla_Z2S_VirtualValve->getZ2SCorePtr(), false);  

    Supla_Z2S_VirtualValve->onLoadConfig(&SuplaDevice);
    Supla_Z2S_VirtualValve->onInit();
}

/*****************************************************************************/

void msgZ2SDeviceVirtualValve(Supla::Element* element, bool state) {
    
  auto Supla_Z2S_VirtualValve = static_cast<
    Supla::Control::Z2S_VirtualValve *>(element);

  Supla_Z2S_VirtualValve->setZbDeviceLastSeenMs(millis());   

  Supla_Z2S_VirtualValve->setValueOnServer(state);          
}
