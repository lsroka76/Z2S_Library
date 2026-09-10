#include "z2s_device_rgbcct.h"


/*****************************************************************************/

int8_t getZ2SDeviceRGBColorMode(uint32_t model_id) {

  int8_t sub_id = Z2S_COLOR_HS_RGB;

  switch (model_id) { 


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


    case Z2S_DEVICE_DESC_IKEA_RGBCCT_BULB:
    case Z2S_DEVICE_DESC_RGBCCT_LIGHT_SOURCE:

      sub_id = Z2S_COLOR_HS_RGB; 
    break;


    case Z2S_DEVICE_DESC_RGBCCT_LIGHT_SOURCE_XY:
    case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:

      sub_id = Z2S_COLOR_XY_RGB; 
    break;


    case Z2S_DEVICE_DESC_LUMI_RGBCCT_LIGHT_SOURCE:

      sub_id = Z2S_COLOR_XY_RGB;
    break;
  }
  return sub_id;
}

/*****************************************************************************/

void initZ2SDeviceRGBCCT(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel,
  Supla::Element *element) {

  Supla::Control::Z2S_RGBCCTInterface *Supla_Z2S_RGBCCTInterface = nullptr;

  if (element) {

    Supla_Z2S_RGBCCTInterface = static_cast<
      Supla::Control::Z2S_RGBCCTInterface *>(element); 
  }
  else {

    Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface();

    Supla_Z2S_RGBCCTInterface->setZ2SChannel(channel_index, _z2s_channel);
    
    Supla_Z2S_RGBCCTInterface->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);
   
    Supla_Z2S_RGBCCTInterface->setInitialCaption(
      _z2s_channel->Supla_channel_name);
         
    Supla_Z2S_RGBCCTInterface->setDefaultFunction(
      _z2s_channel->Supla_channel_func);
  }

  Supla_Z2S_RGBCCTInterface->setRGBColorMode(
    getZ2SDeviceRGBColorMode(_z2s_channel->model_id));

  if (_z2s_channel->model_id == Z2S_DEVICE_DESC_LUMI_RGBCCT_LIGHT_SOURCE) {

    Supla_Z2S_RGBCCTInterface->setMaxWarmCCT(500);
    Supla_Z2S_RGBCCTInterface->setMinCoolCCT(111);
  }
} //initZ2SDeviceRGBCCT

/*****************************************************************************/

void addZ2SDeviceRGBCCT(
  zbg_device_params_t *device, uint8_t free_slot,const char *name, 
  uint32_t func) {
  
  SuplaDevice.saveStateToStorage();
  Supla::Storage::ConfigInstance()->commit();
  
  auto Supla_Z2S_RGBCCTInterface = new Supla::Control::Z2S_RGBCCTInterface();

  if (Supla_Z2S_RGBCCTInterface) {

    Z2S_setChannelData(
      Supla_Z2S_RGBCCTInterface->getZ2SCorePtr(), device, free_slot,
      Supla_Z2S_RGBCCTInterface->getChannelNumber(), 
      SUPLA_CHANNELTYPE_DIMMERANDRGBLED, -1, name, func);

    Supla_Z2S_RGBCCTInterface->setInitialCaption(name);
    Supla_Z2S_RGBCCTInterface->setDefaultFunction(func);

    initZ2SDeviceRGBCCT(
      free_slot, Supla_Z2S_RGBCCTInterface->getZ2SChannel(), 
      Supla_Z2S_RGBCCTInterface->getZ2SElementPtr());

    Supla_Z2S_RGBCCTInterface->getChannel()->setSubDeviceId(
      device->zb_device_id + 1);
    Supla_Z2S_RGBCCTInterface->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(Supla_Z2S_RGBCCTInterface->getZ2SCorePtr());  

    Supla_Z2S_RGBCCTInterface->onLoadConfig(&SuplaDevice);
    Supla_Z2S_RGBCCTInterface->onInit();
  }
}

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