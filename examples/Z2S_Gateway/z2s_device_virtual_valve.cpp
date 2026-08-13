#include "z2s_device_virtual_valve.h"

/*****************************************************************************/

void initZ2SDeviceVirtualValve(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot) {
    initZ2SDeviceVirtualValve(
      channel_number_slot, z2s_channels_table + channel_number_slot);
}

/*****************************************************************************/

void initZ2SDeviceVirtualValve(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  
  uint8_t z2s_function = Z2S_VIRTUAL_VALVE_FNC_DEFAULT_ON_OFF;

  switch (_z2s_channel->model_id) {


    case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY: 
      
      z2s_function = Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY; 
    break;
  }
  
  auto Supla_Z2S_VirtualValve = 
    new Supla::Control::Z2S_VirtualValve(true, z2s_function);
  
  Supla_Z2S_VirtualValve->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

  Supla_Z2S_VirtualValve->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

  Supla_Z2S_VirtualValve->setZ2SChannel(channel_index, _z2s_channel);

  if (strlen(_z2s_channel->Supla_channel_name) > 0) 
    Supla_Z2S_VirtualValve->setInitialCaption(
      _z2s_channel->Supla_channel_name);  

  if (_z2s_channel->Supla_channel_func !=0) 
    Supla_Z2S_VirtualValve->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

  Supla_Z2S_VirtualValve->setKeepAliveSecs(_z2s_channel->keep_alive_secs);

  Supla_Z2S_VirtualValve->setTimeoutSecs(_z2s_channel->timeout_secs);
}

/*****************************************************************************/
                                      
void addZ2SDeviceVirtualValve(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id, const char *name, uint32_t func) {
  
 auto Supla_Z2S_VirtualValve = 
  new Supla::Control::Z2S_VirtualValve(true);

  if (name) 
    Supla_Z2S_VirtualValve->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_VirtualValve->setDefaultFunction(func);
  
  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_VirtualValve->getChannelNumber(), 
    SUPLA_CHANNELTYPE_VALVE_OPENCLOSE, sub_id, name, func);
}

/*****************************************************************************/

/*void msgZ2SDeviceVirtualValve(int16_t channel_number_slot, bool state) {

  if (channel_number_slot < 0) {
    
    log_e("Invalid channel number slot!");
    return;
  }

  auto element = Supla::Element::getElementByChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);
  
  msgZ2SDeviceVirtualValve(element, state);
}*/

/*****************************************************************************/

void msgZ2SDeviceVirtualValve(Supla::Element* element, bool state) {
    
  auto Supla_Z2S_VirtualValve = static_cast<
    Supla::Control::Z2S_VirtualValve *>(element);

  Supla_Z2S_VirtualValve->setZbDeviceLastSeenMs(millis());   

  Supla_Z2S_VirtualValve->setValueOnServer(state);          
}
