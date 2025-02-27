#include "z2s_device_rgbw_light_source.h"
extern "C" {
#include <ColorFormat.h>
}

void initZ2SDeviceRGBWLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel) {
  
  auto Supla_Z2S_RGBWLightSource = new Supla::Control::Z2S_RGBWLightSource(gateway, device);
  Supla_Z2S_RGBWLightSource->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceRGBWLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  auto Supla_Z2S_RGBWLightSource = new Supla::Control::Z2S_RGBWLightSource(gateway, device);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_RGBWLightSource->getChannelNumber(), SUPLA_CHANNELTYPE_DIMMERANDRGBLED,-1, name, func);
}

void msgZ2SDeviceRGBWLightSource(uint8_t Supla_channel, uint8_t id, uint8_t value, bool state, signed char rssi) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMERANDRGBLED) {
    auto Supla_Z2S_RGBWLightSource = reinterpret_cast<Supla::Control::Z2S_RGBWLightSource *>(element);
    uint8_t map_value = map(value, 1, 254, 0, 100);
    if (id == 10) Supla_Z2S_RGBWLightSource->setStateOnOff(state);
    Supla_Z2S_RGBWLightSource->setRGBWValueOnServer(-1 /*red*/, -1 /*green*/, -1 /*blue*/, -1/*color brightness*/, map_value);     
    //Supla_Z2S_RGBWLightSource->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
  }
}
