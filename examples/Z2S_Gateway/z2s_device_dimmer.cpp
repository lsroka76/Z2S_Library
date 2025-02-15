#include "z2s_device_dimmer.h"

void initZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel) {

  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH: channel_element = new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
  }
  if (channel_element)
    channel_element->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH: channel_element = new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
  }
  if (channel_element)
    Z2S_fillDevicesTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_DIMMER, -1, name, func);
}


void msgZ2SDeviceTuyaDimmer(uint32_t model_id, uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                       uint16_t payload_size, uint8_t *payload) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {
    switch (model_id) {
      //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
      //case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: channel_element = new Supla::Control::Z2S_DimmerBulb(gateway, device); break;
      case Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH: {
        auto Supla_Z2S_TuyaDimmerSwitch = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerSwitch *>(element);
        uint8_t _brightness = 128; //temp value
        Supla_Z2S_TuyaDimmerSwitch->setValueOnServer(_brightness);
      } break;
    }
  }
}

void msgZ2SDeviceDimmer(uint32_t model_id, uint8_t Supla_channel, int16_t level, bool state) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {
    switch (model_id) {
      //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
      case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: {
        auto Supla_Z2S_TuyaDimmerBulb = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerBulb *>(element);
        if (level == -1)
          Supla_Z2S_TuyaDimmerBulb->setStateOnServer(state);
        else
          Supla_Z2S_TuyaDimmerBulb->setValueOnServer(level);
      } break;
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: {
        auto Supla_Z2S_TuyaDimmerBulb = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerBulb *>(element);
        if (level == -1)
          Supla_Z2S_TuyaDimmerBulb->setStateOnServer(state);
        else
          Supla_Z2S_TuyaDimmerBulb->setValueOnServer(level);
      } break;
    }
  }
}