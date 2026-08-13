#include "z2s_device_dimmer.h"

/*****************************************************************************/

void initZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot) {

  initZ2SDeviceDimmer(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

void initZ2SDeviceDimmer(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {
  
  if (_z2s_channel->model_id == Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH) {

    auto Supla_Z2S_TuyaDimmerSwitch = new Supla::Control::Z2S_TuyaDimmerSwitch(
      _z2s_channel->sub_id);

    Supla_Z2S_TuyaDimmerSwitch->setZ2SZbDevice(Z2S_getZbDevicePtr(
      _z2s_channel->Zb_device_id));

    Supla_Z2S_TuyaDimmerSwitch->setZ2SChannel(channel_index, _z2s_channel);

    Supla_Z2S_TuyaDimmerSwitch->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

    if (strlen(_z2s_channel->Supla_channel_name) > 0) 
    Supla_Z2S_TuyaDimmerSwitch->setInitialCaption(
      _z2s_channel->Supla_channel_name);
        
    if (_z2s_channel->Supla_channel_func !=0) 
    Supla_Z2S_TuyaDimmerSwitch->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

    return;
  }  //Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH

  uint8_t dimmer_function = SUPLA_RGBW_BIT_FUNC_DIMMER;
  uint8_t dimmer_mode = 0xFF;
  uint8_t cct_mode = 0xFF;
  bool new_interface = false;

  switch (_z2s_channel->sub_id) {

    case DIMMER_FUNC_BRIGHTNESS_SID:
      
      switch (_z2s_channel->model_id) {


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
        case Z2S_DEVICE_DESC_ADEO_RGBW_BULB:
        case Z2S_DEVICE_DESC_SONOFF_SMART_DIMMER:

          dimmer_mode = Z2S_SEND_TO_LEVEL_DIMMER; 
        break;


        case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:
        case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0:

          dimmer_mode = Z2S_TUYA_F0_CMD_DIMMER; 
        break;


        case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

          dimmer_mode = Z2S_TUYA_BRIGHTNESS_DP_DIMMER;
        break;
      } 
    break;


    case DIMMER_FUNC_COLOR_TEMPERATURE_SID:
      
      switch (_z2s_channel->model_id) {


        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
        case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
        case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
        case Z2S_DEVICE_DESC_IKEA_WW_BULB:
        case Z2S_DEVICE_DESC_IKEA_WS_BULB:
        case Z2S_DEVICE_DESC_RGBW_BULB_XY:
        case Z2S_DEVICE_DESC_RGBW_BULB_HS: 
        case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB:
        case Z2S_DEVICE_DESC_DIMMER_CT_BULB:
        case Z2S_DEVICE_DESC_ADEO_RGBW_BULB:

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
      } 
    break;


      case DIMMER_FUNC_BRIGHTNESS_COLOR_TEMPERATURE_SID:
      
      switch (_z2s_channel->model_id) {

        
        case Z2S_DEVICE_DESC_DIMMER_CT_LIGHT_SOURCE: {

          dimmer_function = SUPLA_CHANNELFNC_DIMMER_CCT;
          dimmer_mode = Z2S_SEND_TO_LEVEL_DIMMER; 
          cct_mode = Z2S_COLOR_TEMPERATURE_DIMMER;
          new_interface = true;
        } break;


        case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_LIGHT_SOURCE: {

          dimmer_function = SUPLA_CHANNELFNC_DIMMER_CCT;
          dimmer_mode = Z2S_TUYA_F0_CMD_DIMMER; //Z2S_SEND_TO_LEVEL_DIMMER; 
          cct_mode = Z2S_TUYA_COLOR_TEMPERATURE_DIMMER;
          new_interface = true;
        } break;
      }
    break;
  }
  
  if (dimmer_mode == 0xFF) {

    log_e(
      "initZ2SDeviceDimmer error - dimmer id 0x%x, model id 0x%x", 
      _z2s_channel->sub_id, _z2s_channel->model_id);
    return;
  }

  Supla::Control::Z2S_DimmerInterface *Supla_Z2S_DimmerInterface = nullptr;
  
  if (new_interface)
    Supla_Z2S_DimmerInterface = new Supla::Control::Z2S_DimmerInterface(
      dimmer_function, dimmer_mode, cct_mode);
  else 
    Supla_Z2S_DimmerInterface = new Supla::Control::Z2S_DimmerInterface(
      SUPLA_CHANNELFNC_DIMMER, dimmer_mode, dimmer_mode);


  Supla_Z2S_DimmerInterface->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

  Supla_Z2S_DimmerInterface->setZ2SChannel(channel_index, _z2s_channel);
  
  Supla_Z2S_DimmerInterface->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

  if (strlen(_z2s_channel->Supla_channel_name) > 0) 
    Supla_Z2S_DimmerInterface->setInitialCaption(
      _z2s_channel->Supla_channel_name);

  if (_z2s_channel->Supla_channel_func !=0) 
    Supla_Z2S_DimmerInterface->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

  Supla_Z2S_DimmerInterface->setKeepAliveSecs(
      _z2s_channel->keep_alive_secs);

  Supla_Z2S_DimmerInterface->setTimeoutSecs(
      _z2s_channel->timeout_secs);
}

/*****************************************************************************/

void addZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id, const char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    

    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: 
      
      channel_element = 
        new Supla::Control::Z2S_TuyaDimmerSwitch(sub_id); 
    break;


    /*case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:
    case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0:
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
    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:*/
    default:

      channel_element = new Supla::Control::Z2S_DimmerInterface(); 
    break;
  }
  
  if (channel_element)
    Z2S_fillChannelsTableSlot(
      device, free_slot, channel_element->getChannelNumber(), 
      SUPLA_CHANNELTYPE_DIMMER, sub_id, name, func);
}

/*****************************************************************************/

void addZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  const char *name, uint32_t func) {
  
  addZ2SDeviceDimmer(gateway, device, free_slot, -1, name, func);
}

/*****************************************************************************/

void addZ2SDeviceDimmer(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t function) {

  auto Supla_Z2S_DimmerInterface = new Supla::Control::Z2S_DimmerInterface(
    function); 
  
  if (Supla_Z2S_DimmerInterface)
    Z2S_fillChannelsTableSlot(
      device, free_slot, Supla_Z2S_DimmerInterface->getChannelNumber(), 
      SUPLA_CHANNELTYPE_DIMMER, sub_id, name, function);
}

/*****************************************************************************/

void msgZ2SDeviceDimmer(
  int16_t channel_number_slot, int16_t level, bool state, 
  DimmerMessage dimmer_msg) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceDimmer - invalid channel number slot");
    return;
  }
  
  auto element = Supla::Element::getElementByChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);

  msgZ2SDeviceDimmer(
    element, z2s_channels_table[channel_number_slot].model_id, level, state, 
    dimmer_msg);
}

/*****************************************************************************/

void msgZ2SDeviceDimmer(
  Supla::Element* element, uint32_t model_id, int16_t level, bool state, 
  DimmerMessage dimmer_msg) {


  switch (model_id) {


    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
      
      auto Supla_Z2S_TuyaDimmerSwitch = 
        reinterpret_cast<Supla::Control::Z2S_TuyaDimmerSwitch *>(element);

      Supla_Z2S_TuyaDimmerSwitch->setZbDeviceLastSeenMs(millis());  
      
      if (level == DIMMER_NO_LEVEL_DATA)
        Supla_Z2S_TuyaDimmerSwitch->setStateOnServer(state);
      else
        Supla_Z2S_TuyaDimmerSwitch->setValueOnServer(level);
    } break;

    
    default: {

      auto Supla_Z2S_DimmerInterface = 
        reinterpret_cast<Supla::Control::Z2S_DimmerInterface *>(element);
      
      Supla_Z2S_DimmerInterface->setZbDeviceLastSeenMs(millis());  
      
      Supla_Z2S_DimmerInterface->setValueOnServer(level, state, dimmer_msg);
    } break;
  }
}

