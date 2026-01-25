#include "z2s_device_virtual_valve.h"

/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceVirtualValve(ZigbeeGateway *gateway, 
                               zbg_device_params_t *device, 
                               int16_t channel_number_slot) {
  
  uint8_t z2s_function = Z2S_VIRTUAL_VALVE_FNC_DEFAULT_ON_OFF;

  switch (z2s_channels_table[channel_number_slot].model_id) {

    case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY: 
      z2s_function = Z2S_VIRTUAL_VALVE_FNC_TUYA_BATTERY; break;
  }

  
  auto Supla_Z2S_VirtualValve = 
    new Supla::Control::Z2S_VirtualValve(gateway, device, true, z2s_function);
  
  Supla_Z2S_VirtualValve->getChannel()->setChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_VirtualValve->setInitialCaption(
      z2s_channels_table[channel_number_slot].Supla_channel_name);  

  if (z2s_channels_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_VirtualValve->setDefaultFunction(
      z2s_channels_table[channel_number_slot].Supla_channel_func);

  //Supla_Z2S_VirtualValve->setKeepAliveSecs(z2s_channels_table[channel_number_slot].keep_alive_secs);
  //Supla_Z2S_VirtualValve->setTimeoutSecs(z2s_channels_table[channel_number_slot].timeout_secs);
}

/*---------------------------------------------------------------------------------------------------------------------------*/
                                      
void addZ2SDeviceVirtualValve(ZigbeeGateway *gateway, 
                              zbg_device_params_t *device, 
                              uint8_t free_slot, 
                              int8_t sub_id, 
                              const char *name, 
                              uint32_t func) {
  
 auto Supla_Z2S_VirtualValve = 
  new Supla::Control::Z2S_VirtualValve(gateway,device, true);

  if (name) 
    Supla_Z2S_VirtualValve->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_VirtualValve->setDefaultFunction(func);
  
  Z2S_fillChannelsTableSlot(device, 
                            free_slot, 
                            Supla_Z2S_VirtualValve->getChannelNumber(), 
                            SUPLA_CHANNELTYPE_VALVE_OPENCLOSE, 
                            sub_id, 
                            name, 
                            func);
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceVirtualValve(int16_t channel_number_slot, 
                              bool state) {

  if (channel_number_slot < 0) {
    
    log_e("Invalid channel number slot!");
    return;
  }

  Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].short_addr, millis());

  auto element = 
    Supla::Element::getElementByChannelNumber(
        z2s_channels_table[channel_number_slot].Supla_channel);

  if (element && 
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_VALVE_OPENCLOSE)) {
    
    auto Supla_Z2S_VirtualValve = 
      reinterpret_cast<Supla::Control::Z2S_VirtualValve *>(element);
    
    //Supla_Z2S_VirtualRelay->getChannel()->setStateOnline();
    Supla_Z2S_VirtualValve->setValueOnServer(state);     
    //Supla_Z2S_VirtualRelay->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
  }
}
