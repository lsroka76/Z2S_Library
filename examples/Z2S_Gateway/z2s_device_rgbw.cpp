#include "z2s_device_rgbw.h"

void initZ2SDeviceRGBW(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel) {

  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    //case Z2S_DEVICE_DESC_TUYA_RGB_BULB: channel_element = new Supla::Control::Z2S_TuyaRGBBulb(gateway, device); break;
    //case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaRGBWBulb(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaHueDimmerBulb(gateway, device); break;
  }
  if (channel_element)
    channel_element->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceRGBW(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    //case Z2S_DEVICE_DESC_TUYA_RGB_BULB: channel_element = new Supla::Control::Z2S_TuyaRGBBulb(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaRGBWBulb(gateway, device); break;
  }
  if (channel_element)
    Z2S_fillDevicesTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_DIMMER, -1, name, func);
    //Z2S_fillDevicesTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_DIMMERANDRGBLED, -1, name, func);
}

void msgZ2SDeviceRGBW(uint32_t model_id, uint8_t Supla_channel, uint8_t hue, uint8_t saturation, int16_t level, bool state) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) { //SUPLA_CHANNELTYPE_DIMMERANDRGBLED) {
    switch (model_id) {
      //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: {
        auto Supla_Z2S_TuyaHueDimmerBulb = reinterpret_cast<Supla::Control::Z2S_TuyaHueDimmerBulb *>(element);
        //auto Supla_Z2S_TuyaRGBWBulb = reinterpret_cast<Supla::Control::Z2S_TuyaRGBWBulb *>(element);
        Supla_Z2S_TuyaHueDimmerBulb->getChannel()->setOnline();
        /*if ((hue == 0xFF) && (saturation == 0xFF) && (level == -1))
          ;//Supla_Z2S_TuyaRGBWBulb->setStateOnServer(state);
        else
          ;//Supla_Z2S_TuyaRGBWBulb->setValueOnServer(hue, saturation, level);*/
          if (level == -1)
          Supla_Z2S_TuyaHueDimmerBulb->setStateOnServer(state);
        else
          Supla_Z2S_TuyaHueDimmerBulb->setValueOnServer(level);
      } break;
    }
  }
}