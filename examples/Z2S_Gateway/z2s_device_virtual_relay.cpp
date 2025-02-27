#include "z2s_device_virtual_relay.h"

void initZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway, device);
  
  Supla_Z2S_VirtualRelay->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (z2s_devices_table[channel_number_slot].Supla_channel_func) 
    Supla_Z2S_VirtualRelay->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_devices_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_VirtualRelay->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);
}

                                      
                                      

void addZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway,device);

  if (name) 
    Supla_Z2S_VirtualRelay->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_VirtualRelay->setDefaultFunction(func);
  
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY,-1, name, func);
}

void msgZ2SDeviceVirtualRelay(int16_t channel_number_slot, bool state, signed char rssi) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceVirtualRelay - invalid channel number slot");
    return;
  }

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    
    auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
    
    Supla_Z2S_VirtualRelay->getChannel()->setOnline();
    Supla_Z2S_VirtualRelay->Z2S_setOnOff(state);     
    //Supla_Z2S_VirtualRelay->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
}
}
