#include "z2s_device_dimmer.h"

/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot) {

  
  if (device->model_id == 
        Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH) {

    auto Supla_Z2S_TuyaDimmerSwitch = 
      new Supla::Control::Z2S_TuyaDimmerSwitch(
        gateway, 
        device, 
        z2s_channels_table[channel_number_slot].sub_id);

    Supla_Z2S_TuyaDimmerSwitch->getChannel()->setChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

    if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_TuyaDimmerSwitch->setInitialCaption(
      z2s_channels_table[channel_number_slot].Supla_channel_name);
        
    if (z2s_channels_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_TuyaDimmerSwitch->setDefaultFunction(
      z2s_channels_table[channel_number_slot].Supla_channel_func);

    return;
  }  

  uint8_t dimmer_mode = 0xFF;  

  switch (z2s_channels_table[channel_number_slot].sub_id) {

    case DIMMER_FUNC_BRIGHTNESS_SID:
      
      switch (device->model_id) {


        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT:
        case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
        case Z2S_DEVICE_DESC_IKEA_WW_BULB:
        case Z2S_DEVICE_DESC_IKEA_WS_BULB:
        case Z2S_DEVICE_DESC_RGBW_BULB_XY:
        case Z2S_DEVICE_DESC_RGBW_BULB_HS: 
        case Z2S_DEVICE_DESC_PHILIPS_WW_BULB:
        case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
        case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB:
        case Z2S_DEVICE_DESC_TUYA_LED_DIMMER:
        case Z2S_DEVICE_DESC_LED_DIMMER:
        case Z2S_DEVICE_DESC_DIMMER_CT_BULB:

          dimmer_mode = Z2S_SEND_TO_LEVEL_DIMMER; 
        break;


        case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:

          dimmer_mode = Z2S_TUYA_F0_CMD_DIMMER; 
        break;


        case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

          dimmer_mode = Z2S_TUYA_BRIGHTNESS_DP_DIMMER;
        break;
      } break;


    case DIMMER_FUNC_COLOR_TEMPERATURE_SID:
      
      switch (device->model_id) {


        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
        case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
        case Z2S_DEVICE_DESC_IKEA_WW_BULB:
        case Z2S_DEVICE_DESC_IKEA_WS_BULB:
        case Z2S_DEVICE_DESC_RGBW_BULB_XY:
        case Z2S_DEVICE_DESC_RGBW_BULB_HS: 
        case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB:
        case Z2S_DEVICE_DESC_DIMMER_CT_BULB:

          dimmer_mode = Z2S_COLOR_TEMPERATURE_DIMMER; 
        break;


        case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:

          dimmer_mode = Z2S_TUYA_E0_CMD_DIMMER; 
        break;


        case  Z2S_DEVICE_DESC_PHILIPS_WW_BULB:
        case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:

          dimmer_mode = Z2S_PHILIPS_COLOR_TEMPERATURE_DIMMER;
        break;


        case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

          dimmer_mode = Z2S_TUYA_COLOR_TEMPERATURE_DP_DIMMER;
        break;
      } break;
  }
  
  if (dimmer_mode == 0xFF) {

    log_e("initZ2SDeviceDimmer error - dimmer id 0x%x, model id 0x%x", 
          z2s_channels_table[channel_number_slot].sub_id, device->model_id);
    return;
  }

  auto Supla_Z2S_DimmerInterface = 
    new Supla::Control::Z2S_DimmerInterface(gateway, device, dimmer_mode); 

  Supla_Z2S_DimmerInterface->getChannel()->setChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_DimmerInterface->setInitialCaption(
      z2s_channels_table[channel_number_slot].Supla_channel_name);

  if (z2s_channels_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_DimmerInterface->setDefaultFunction(
      z2s_channels_table[channel_number_slot].Supla_channel_func);

  Supla_Z2S_DimmerInterface->setKeepAliveSecs(
      z2s_channels_table[channel_number_slot].keep_alive_secs);

  Supla_Z2S_DimmerInterface->setTimeoutSecs(
      z2s_channels_table[channel_number_slot].timeout_secs);
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void addZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id, const char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    

    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: 
      
      channel_element = 
        new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device, sub_id); 
    break;


    case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT:
    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
    case Z2S_DEVICE_DESC_IKEA_WW_BULB:
    case Z2S_DEVICE_DESC_IKEA_WS_BULB:
    case Z2S_DEVICE_DESC_PHILIPS_WW_BULB:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_HS:
    case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
    case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB:
    case Z2S_DEVICE_DESC_TUYA_LED_DIMMER:
    case Z2S_DEVICE_DESC_LED_DIMMER:
    case Z2S_DEVICE_DESC_DIMMER_CT_BULB:
    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

      channel_element = 
        new Supla::Control::Z2S_DimmerInterface(gateway, device, sub_id); 
    break;
  }
  
  if (channel_element)
    Z2S_fillChannelsTableSlot(
      device, free_slot, channel_element->getChannelNumber(), 
      SUPLA_CHANNELTYPE_DIMMER, sub_id, name, func);
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void addZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  const char *name, uint32_t func) {
  
  addZ2SDeviceDimmer(gateway, device, free_slot, -1, name, func);
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceDimmer(
  int16_t channel_number_slot, int16_t level, bool state) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceDimmer - invalid channel number slot");
    return;
  }
  
  //Z2S_updateZbDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);

  if (element && 
      (element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER)) {
    
    switch (z2s_channels_table[channel_number_slot].model_id) {


      case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0: 
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT:
      case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
      case Z2S_DEVICE_DESC_IKEA_WW_BULB:
      case Z2S_DEVICE_DESC_IKEA_WS_BULB:
      case Z2S_DEVICE_DESC_PHILIPS_WW_BULB:
      case Z2S_DEVICE_DESC_RGBW_BULB_XY:
      case Z2S_DEVICE_DESC_RGBW_BULB_HS:
      case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
      case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB:
      case Z2S_DEVICE_DESC_TUYA_LED_DIMMER:
      case Z2S_DEVICE_DESC_DIMMER_CT_BULB:
      case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00: {

        auto Supla_Z2S_DimmerInterface = 
          reinterpret_cast<Supla::Control::Z2S_DimmerInterface *>(element);
        
        Supla_Z2S_DimmerInterface->setValueOnServer(level);
      } break;


      case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
        
        auto Supla_Z2S_TuyaDimmerSwitch = 
          reinterpret_cast<Supla::Control::Z2S_TuyaDimmerSwitch *>(element);
        
        if (level == DIMMER_NO_LEVEL_DATA)
          Supla_Z2S_TuyaDimmerSwitch->setStateOnServer(state);
        else
          Supla_Z2S_TuyaDimmerSwitch->setValueOnServer(level);
      } break;
    }
  }
}
