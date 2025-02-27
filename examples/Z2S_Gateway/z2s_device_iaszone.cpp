#include "z2s_device_iaszone.h"

void initZ2SDeviceIASzone(int16_t channel_number_slot) {
  
  auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary(true);
  
  Supla_VirtualBinary->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (z2s_devices_table[channel_number_slot].Supla_channel_name) 
    Supla_VirtualBinary->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_devices_table[channel_number_slot].Supla_channel_func != 0) 
    Supla_VirtualBinary->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);
  
}

void addZ2SDeviceIASzone(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func) {
  
  auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary(true);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_VirtualBinary->getChannelNumber(), SUPLA_CHANNELTYPE_BINARYSENSOR, sub_id, name, func);

  if (name) 
    Supla_VirtualBinary->setInitialCaption(name);
  
  if (func !=0) 
    Supla_VirtualBinary->setDefaultFunction(func);
  
}

void msgZ2SDeviceIASzone(int16_t channel_number_slot, bool state, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceIASzone - invalid channel number slot");
    return;
  }

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

        auto Supla_VirtualBinary = reinterpret_cast<Supla::Sensor::VirtualBinary *>(element);
        //Supla_VirtualBinary->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        if (state) Supla_VirtualBinary->set();
        else Supla_VirtualBinary->clear();
    }
}
