#include "z2s_device_rgb.h"

/*****************************************************************************/

static uint32_t hue_saturation_counter = 0;
static uint8_t last_hue_value = 0;
static uint8_t last_saturation_value = 0;

/*****************************************************************************/

void initZ2SDeviceRGB(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot) {

  initZ2SDeviceRGB(
    channel_number_slot, z2s_channels_table + channel_number_slot);

}

void initZ2SDeviceRGB(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  Supla::Control::Z2S_RGBInterface * Supla_Z2S_RGBInterface = nullptr;

  switch (_z2s_channel->model_id) {


    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 

      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(
        Z2S_TUYA_COLOR_HS_RGB);
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT:

      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(
        Z2S_TUYA_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
    case Z2S_DEVICE_DESC_ADEO_RGBW_BULB:

      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(
        Z2S_COLOR_HS_RGB); 
      break;


    case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:

      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(
        Z2S_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:

      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(
        Z2S_PHILIPS_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

      Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface(
        Z2S_TUYA_DP_COLOR_HS_RGB); 
    break;
  }

  if (Supla_Z2S_RGBInterface) {

    Supla_Z2S_RGBInterface->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

    Supla_Z2S_RGBInterface->setZ2SChannel(channel_index, _z2s_channel);
  
    Supla_Z2S_RGBInterface->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);
   
    if (strlen(_z2s_channel->Supla_channel_name) > 0) 
      Supla_Z2S_RGBInterface->setInitialCaption(
        _z2s_channel->Supla_channel_name);
        
    if (_z2s_channel->Supla_channel_func !=0) 
      Supla_Z2S_RGBInterface->setDefaultFunction(
        _z2s_channel->Supla_channel_func);

    if (_z2s_channel->user_data_1 > 0) 
      Supla_Z2S_RGBInterface->setRGBMode(
        _z2s_channel->user_data_1);

    Supla_Z2S_RGBInterface->setKeepAliveSecs(
      _z2s_channel->keep_alive_secs);

    Supla_Z2S_RGBInterface->setTimeoutSecs(
      _z2s_channel->timeout_secs);
  }
} //initZ2SDeviceRGB

/*****************************************************************************/

void addZ2SDeviceRGB(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot,
  const char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;
  int8_t sub_id = Z2S_COLOR_HS_RGB;

  switch (device->model_id) { 


    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:

      sub_id = Z2S_TUYA_COLOR_HS_RGB;
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT:

      sub_id = Z2S_TUYA_COLOR_HS_XY_RGB; 
    break;


    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
    case Z2S_DEVICE_DESC_ADEO_RGBW_BULB:

      sub_id = Z2S_COLOR_HS_RGB; 
    break;


    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:

      sub_id = Z2S_PHILIPS_COLOR_XY_RGB; 
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

      sub_id = Z2S_TUYA_DP_COLOR_HS_RGB; 
    break;
    
  }
  channel_element = 
    new Supla::Control::Z2S_RGBInterface(sub_id);

  if (channel_element)
    Z2S_fillChannelsTableSlot(
      device, free_slot, channel_element->getChannelNumber(), 
      SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, -1, name, func);
}

/*****************************************************************************/

void msgZ2SDeviceRGB(Supla::Element *element, uint8_t hue, uint8_t saturation, 
  bool state) {

  auto Supla_Z2S_RGBInterface = static_cast<
    Supla::Control::Z2S_RGBInterface *>(element);

  Supla_Z2S_RGBInterface->setZbDeviceLastSeenMs(millis());
}