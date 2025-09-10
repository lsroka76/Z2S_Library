#include "z2s_device_rgb.h"

static uint32_t hue_saturation_counter = 0;
static uint8_t last_hue_value = 0;
static uint8_t last_saturation_value = 0;

void initZ2SDeviceRGB(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {

  Supla::Control::Z2S_RGBInterface * Supla_Z2S_RGBInterface = nullptr;

  switch (device->model_id) {

    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(gateway, device, Z2S_TUYA_COLOR_HS_RGB); break;

    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(gateway, device, Z2S_TUYA_COLOR_XY_RGB); break;

    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(gateway, device, Z2S_COLOR_HS_RGB); break;

    case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:
      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(gateway, device, Z2S_COLOR_XY_RGB); break;

    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(gateway, device, Z2S_PHILIPS_COLOR_XY_RGB); break;
  }

  if (Supla_Z2S_RGBInterface) {

    Supla_Z2S_RGBInterface->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);
   
    if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
      Supla_Z2S_RGBInterface->setInitialCaption(z2s_channels_table[channel_number_slot].Supla_channel_name);
        
    if (z2s_channels_table[channel_number_slot].Supla_channel_func !=0) 
      Supla_Z2S_RGBInterface->setDefaultFunction(z2s_channels_table[channel_number_slot].Supla_channel_func);

    if (z2s_channels_table[channel_number_slot].user_data_1 > 0) 
      Supla_Z2S_RGBInterface->setRGBMode(z2s_channels_table[channel_number_slot].user_data_1);

    Supla_Z2S_RGBInterface->setKeepAliveSecs(z2s_channels_table[channel_number_slot].keep_alive_secs);
    Supla_Z2S_RGBInterface->setTimeoutSecs(z2s_channels_table[channel_number_slot].timeout_secs);
  }
} //initZ2SDeviceRGB

void addZ2SDeviceRGB(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;
  int8_t sub_id = Z2S_COLOR_HS_RGB;

  switch (device->model_id) { 
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      sub_id = Z2S_TUYA_COLOR_HS_RGB; break;

    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
      sub_id = Z2S_TUYA_COLOR_HS_XY_RGB; break;

    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
      sub_id = Z2S_COLOR_XY_RGB; break;

    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
      sub_id = Z2S_PHILIPS_COLOR_XY_RGB; break;
    
  }
  channel_element = new Supla::Control::Z2S_RGBInterface(gateway, device, sub_id);

  if (channel_element)
    Z2S_fillChannelsTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, -1, name, func);
}

void msgZ2SDeviceRGB(uint32_t model_id, uint8_t Supla_channel, uint8_t hue, uint8_t saturation, bool state, signed char rssi) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER) {

    auto Supla_Z2S_RGBInterface = reinterpret_cast<Supla::Control::Z2S_RGBInterface *>(element);
    Supla_Z2S_RGBInterface->getChannel()->setStateOnline();

    switch (model_id) {

      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
      case Z2S_DEVICE_DESC_IKEA_RGBW_BULB: {
        if ((hue == 0xFF) && (saturation == 0xFF))
        {
        //  Supla_Z2S_TuyaRGBBulb->setStateOnServer(state);
          //Supla_Z2S_TuyaRGBBulb->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        }
        else {
          if (hue == 0xFF)
            last_saturation_value = saturation;
          else last_hue_value = hue;
          hue_saturation_counter++;
          if ((hue_saturation_counter % 2) == 0)
          {
          //  Supla_Z2S_TuyaRGBBulb->setValueOnServer(last_hue_value, last_saturation_value);
            //Supla_Z2S_TuyaRGBBulb->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
          }
        }  
      } break;

      default: break;
    }
  }
}