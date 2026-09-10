#include "z2s_device_rgb.h"

/*****************************************************************************/

int8_t getZ2SDeviceColorMode(uint32_t model_id) {

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


    case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:

      sub_id = Z2S_COLOR_XY_RGB; 
    break;

    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:

      sub_id = Z2S_PHILIPS_COLOR_XY_RGB; 
    break;


    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

      sub_id = Z2S_TUYA_DP_COLOR_HS_RGB; 
    break;  
  }
  return sub_id;
}

/*****************************************************************************/

void initZ2SDeviceRGB(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel,
  Supla::Element *element) {

  Supla::Control::Z2S_RGBInterface *Supla_Z2S_RGBInterface = nullptr;

  if (element) {

    Supla_Z2S_RGBInterface = static_cast<
      Supla::Control::Z2S_RGBInterface *>(element);
  }
  else {

    Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface();

    Supla_Z2S_RGBInterface->setZ2SChannel(channel_index, _z2s_channel);
  
    Supla_Z2S_RGBInterface->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);
   
    Supla_Z2S_RGBInterface->setInitialCaption(
      _z2s_channel->Supla_channel_name);
        
    Supla_Z2S_RGBInterface->setDefaultFunction(
      _z2s_channel->Supla_channel_func);
  }
  Supla_Z2S_RGBInterface->setRGBColorMode(
    getZ2SDeviceColorMode(_z2s_channel->model_id));
}

/*****************************************************************************/

void addZ2SDeviceRGB(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot,
  const char *name, uint32_t func) {
  
  SuplaDevice.saveStateToStorage();
  Supla::Storage::ConfigInstance()->commit();
  
  auto Supla_Z2S_RGBInterface = new Supla::Control::Z2S_RGBInterface();

  if (Supla_Z2S_RGBInterface) {

    Z2S_setChannelData(
      Supla_Z2S_RGBInterface->getZ2SCorePtr(), device, free_slot,
      Supla_Z2S_RGBInterface->getChannelNumber(), 
      SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, -1, name, func);
    
    Supla_Z2S_RGBInterface->setInitialCaption(name);
    Supla_Z2S_RGBInterface->setDefaultFunction(func);

    initZ2SDeviceRGB(
      free_slot, Supla_Z2S_RGBInterface->getZ2SChannel(),
      Supla_Z2S_RGBInterface->getZ2SElementPtr());

    Supla_Z2S_RGBInterface->getChannel()->setSubDeviceId(
      device->zb_device_id + 1);
    Supla_Z2S_RGBInterface->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(Supla_Z2S_RGBInterface->getZ2SCorePtr());

    Supla_Z2S_RGBInterface->onLoadConfig(&SuplaDevice);
    Supla_Z2S_RGBInterface->onInit();
  }
}

/*****************************************************************************/

void msgZ2SDeviceRGB(Supla::Element *element, uint8_t hue, uint8_t saturation, 
  bool state) {

  auto Supla_Z2S_RGBInterface = static_cast<
    Supla::Control::Z2S_RGBInterface *>(element);

  Supla_Z2S_RGBInterface->setZbDeviceLastSeenMs(millis());
}