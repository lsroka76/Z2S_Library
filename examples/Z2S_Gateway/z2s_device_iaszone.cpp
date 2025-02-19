#include "z2s_device_iaszone.h"

void initZ2SDeviceIASzone(uint8_t Supla_channel) {
  
  auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary(true);
  Supla_VirtualBinary->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceIASzone(zbg_device_params_t *device, uint8_t free_slot) {
  
  auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary(true);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_VirtualBinary->getChannelNumber(), SUPLA_CHANNELTYPE_BINARYSENSOR,-1);
}

void msgZ2SDeviceIASzone(uint8_t Supla_channel, int zone_status, signed char rssi) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

        auto Supla_VirtualBinary = reinterpret_cast<Supla::Sensor::VirtualBinary *>(element);
        Supla_VirtualBinary->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        if (zone_status == 0) Supla_VirtualBinary->set();
        else Supla_VirtualBinary->clear();
    }
}
