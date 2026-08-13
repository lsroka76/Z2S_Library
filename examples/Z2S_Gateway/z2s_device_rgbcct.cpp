#include "z2s_device_rgbcct.h"

/*****************************************************************************/

static uint32_t hue_saturation_counter = 0;
static uint8_t last_hue_value = 0;
static uint8_t last_saturation_value = 0;

/*****************************************************************************/

void initZ2SDeviceRGBCCT(
  zbg_device_params_t *device, int16_t channel_number_slot) {

  initZ2SDeviceRGBCCT(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

void initZ2SDeviceRGBCCT(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  Supla::Control::Z2S_RGBCCTInterface* Supla_Z2S_RGBCCTInterface = nullptr;

  switch (_z2s_channel->model_id) {


    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_TUYA_COLOR_HS_RGB);
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_TUYA_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_IKEA_RGBCCT_BULB:
    case Z2S_DEVICE_DESC_RGBCCT_LIGHT_SOURCE:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_COLOR_HS_RGB); 
    break;


    case Z2S_DEVICE_DESC_RGBCCT_LIGHT_SOURCE_XY:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_LUMI_RGBCCT_LIGHT_SOURCE: {

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_COLOR_XY_RGB);

      if (Supla_Z2S_RGBCCTInterface) {
        Supla_Z2S_RGBCCTInterface->setMaxWarmCCT(500);
        Supla_Z2S_RGBCCTInterface->setMinCoolCCT(111);
      }
    } break;


    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
    case Z2S_DEVICE_DESC_ADEO_RGBW_BULB:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_COLOR_HS_RGB); 
      break;


    case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_PHILIPS_COLOR_XY_RGB); 
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

      Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface(
        Z2S_TUYA_DP_COLOR_HS_RGB); 
    break;
  }

  if (Supla_Z2S_RGBCCTInterface) {

    Supla_Z2S_RGBCCTInterface->setZ2SZbDevice(Z2S_getZbDevicePtr(
      _z2s_channel->Zb_device_id));

    Supla_Z2S_RGBCCTInterface->setZ2SChannel(channel_index, _z2s_channel);
    
    Supla_Z2S_RGBCCTInterface->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);
   
    if (strlen(_z2s_channel->Supla_channel_name) > 0) 
      Supla_Z2S_RGBCCTInterface->setInitialCaption(
        _z2s_channel->Supla_channel_name);
        
    if (_z2s_channel->Supla_channel_func !=0) 
      Supla_Z2S_RGBCCTInterface->setDefaultFunction(
        _z2s_channel->Supla_channel_func);

    if (_z2s_channel->user_data_1 > 0) 
      Supla_Z2S_RGBCCTInterface->setRGBMode(_z2s_channel->user_data_1);

    Supla_Z2S_RGBCCTInterface->setKeepAliveSecs(
      _z2s_channel->keep_alive_secs);

    Supla_Z2S_RGBCCTInterface->setTimeoutSecs(_z2s_channel->timeout_secs);
  }
} //initZ2SDeviceRGB

/*****************************************************************************/

void addZ2SDeviceRGBCCT(
  zbg_device_params_t *device, uint8_t free_slot,const char *name, 
  uint32_t func) {
  
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
    new Supla::Control::Z2S_RGBCCTInterface(sub_id);

  if (channel_element)
    Z2S_fillChannelsTableSlot(
      device, free_slot, channel_element->getChannelNumber(), 
      SUPLA_CHANNELTYPE_DIMMERANDRGBLED, -1, name, func);
}

/*****************************************************************************/

/*void msgZ2SDeviceRGBCCT(int16_t channel_number_slot, RGBCCTMessage rgbcct_msg,
  uint32_t value) {

  auto element = Supla::Element::getElementByChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);
  
  msgZ2SDeviceRGBCCT(element, rgbcct_msg, value);
}*/

/*****************************************************************************/

void msgZ2SDeviceRGBCCT(
  Supla::Element* element, RGBCCTMessage rgbcct_msg, uint32_t value) {

  auto Supla_Z2S_RGBCCTInterface = static_cast<
    Supla::Control::Z2S_RGBCCTInterface *>(element);
    
  Supla_Z2S_RGBCCTInterface->setZbDeviceLastSeenMs(millis());

  switch (rgbcct_msg) {
    
    
    case RGBCCTMessage::ON_OFF_STATE_MSG: {

      Supla_Z2S_RGBCCTInterface->setStateOnServer((bool)value);
    } break;


    case RGBCCTMessage::COLOR_MODE_MSG: {

      Supla_Z2S_RGBCCTInterface->setDeviceColorMode((uint8_t)value);
    } break;


    default:

      log_e("Unknown RGBCCT MSG %02u", rgbcct_msg);
    break;
  }
}