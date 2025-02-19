#include "z2s_device_virtual_relay.h"

void initZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel, char *name, uint32_t func) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway, device);
  Supla_Z2S_VirtualRelay->getChannel()->setChannelNumber(Supla_channel);
  if (name) 
    Supla_Z2S_VirtualRelay->setInitialCaption(name);
  if (func !=0) 
    Supla_Z2S_VirtualRelay->setDefaultFunction(func);
}

void addZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway,device);
  if (name) 
    Supla_Z2S_VirtualRelay->setInitialCaption(name);
  if (func !=0) 
    Supla_Z2S_VirtualRelay->setDefaultFunction(func);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY,-1, name, func);
}

void msgZ2SDeviceVirtualRelay(uint8_t Supla_channel, bool state, signed char rssi) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
    Supla_Z2S_VirtualRelay->getChannel()->setOnline();
    Supla_Z2S_VirtualRelay->Z2S_setOnOff(state);     
    Supla_Z2S_VirtualRelay->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
}
}
