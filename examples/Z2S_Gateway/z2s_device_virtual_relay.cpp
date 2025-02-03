#include "z2s_device_virtual_relay.h"

void initZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zb_device_params_t *device, uint8_t Supla_channel) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway, device);
  Supla_Z2S_VirtualRelay->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zb_device_params_t *device, uint8_t free_slot) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway,device);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY,-1);
}

void msgZ2SDeviceVirtualRelay(uint8_t Supla_channel, bool state) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
    Supla_Z2S_VirtualRelay->Z2S_setOnOff(state);     
  }
}
