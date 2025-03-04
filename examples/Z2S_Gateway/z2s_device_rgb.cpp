#include "z2s_device_rgb.h"

static uint32_t hue_saturation_counter = 0;
static uint8_t last_hue_value = 0;
static uint8_t last_saturation_value = 0;

void initZ2SDeviceRGB(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel) {

  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
    channel_element = new Supla::Control::Z2S_TuyaRGBBulb(gateway, device); break;
  }
  if (channel_element)
    channel_element->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceRGB(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
     channel_element = new Supla::Control::Z2S_TuyaRGBBulb(gateway, device); break;
  }
  if (channel_element)
    Z2S_fillDevicesTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, -1, name, func);
}

void msgZ2SDeviceRGB(uint32_t model_id, uint8_t Supla_channel, uint8_t hue, uint8_t saturation, bool state, signed char rssi) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER) {
    switch (model_id) {
      //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: {
        auto Supla_Z2S_TuyaRGBBulb = reinterpret_cast<Supla::Control::Z2S_TuyaRGBBulb *>(element);
        Supla_Z2S_TuyaRGBBulb->getChannel()->setStateOnline();
        if ((hue == 0xFF) && (saturation == 0xFF))
        {
          Supla_Z2S_TuyaRGBBulb->setStateOnServer(state);
          //Supla_Z2S_TuyaRGBBulb->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        }
        else {
          if (hue == 0xFF)
            last_saturation_value = saturation;
          else last_hue_value = hue;
          hue_saturation_counter++;
          if ((hue_saturation_counter % 2) == 0)
          {
            Supla_Z2S_TuyaRGBBulb->setValueOnServer(last_hue_value, last_saturation_value);
            //Supla_Z2S_TuyaRGBBulb->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
          }
        }  
      } break;
    }
  }
}