#ifndef Z2S_DEVICE_IASZONE_H_
#define Z2S_DEVICE_IASZONE_H_

#include "Z2S_Devices_Table.h"
//#include <SuplaDevice.h>
#include <supla/sensor/virtual_binary.h>

void initZ2SDeviceIASzone(uint8_t Supla_channel) {
  
  auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
  Supla_VirtualBinary->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceIASzone(zb_device_params_t *device, uint8_t free_slot) {
  
  auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_VirtualBinary->getChannelNumber(), SUPLA_CHANNELTYPE_BINARYSENSOR,-1);
}

void msgZ2SDeviceIASzone(uint8_t Supla_channel, int zone_status) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

        auto Supla_VirtualBinary = reinterpret_cast<Supla::Sensor::VirtualBinary *>(element);
        if (zone_status == 0) Supla_VirtualBinary->set();
        else Supla_VirtualBinary->clear();
    }
}

#endif