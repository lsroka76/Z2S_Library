#include "z2s_device_dimmer.h"

/*****************************************************************************/

void initZ2SDeviceDimmer(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel, 
  Supla::Element *element) {
  
  if (_z2s_channel->model_id == Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH) {

    Supla::Control::Z2S_TuyaDimmerSwitch *Supla_Z2S_TuyaDimmerSwitch = nullptr;

    if (element) {

      Supla_Z2S_TuyaDimmerSwitch = static_cast<
        Supla::Control::Z2S_TuyaDimmerSwitch *>(element);
    }
    else {

      Supla_Z2S_TuyaDimmerSwitch = new Supla::Control::Z2S_TuyaDimmerSwitch(
      _z2s_channel->sub_id);

      Supla_Z2S_TuyaDimmerSwitch->setZ2SChannel(channel_index, _z2s_channel);

      Supla_Z2S_TuyaDimmerSwitch->getChannel()->setChannelNumber(
        _z2s_channel->Supla_channel);

      Supla_Z2S_TuyaDimmerSwitch->setInitialCaption(
        _z2s_channel->Supla_channel_name);
      
      Supla_Z2S_TuyaDimmerSwitch->setDefaultFunction(
        _z2s_channel->Supla_channel_func);
    }
    return;
  }  //Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH

  uint8_t dimmer_function = SUPLA_CHANNELFNC_DIMMER;
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

  if (element) {

    Supla_Z2S_DimmerInterface = static_cast<
      Supla::Control::Z2S_DimmerInterface *>(element);
  }
  else {

    Supla_Z2S_DimmerInterface = new Supla::Control::Z2S_DimmerInterface();

    Supla_Z2S_DimmerInterface->setZ2SChannel(channel_index, _z2s_channel);
  
    Supla_Z2S_DimmerInterface->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

    Supla_Z2S_DimmerInterface->setInitialCaption(
      _z2s_channel->Supla_channel_name);
  }

  Supla_Z2S_DimmerInterface->setDefaultFunction(dimmer_function);
  Supla_Z2S_DimmerInterface->setDimmerMode(dimmer_mode);
  if (new_interface)
    Supla_Z2S_DimmerInterface->setCCTMode(cct_mode);
  else
    Supla_Z2S_DimmerInterface->setCCTMode(dimmer_mode);
}

/*****************************************************************************/

void addZ2SDeviceDimmer(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id, const char *name, uint32_t func) {
  
  switch (device->model_id) {
    

    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();
      
      auto Supla_Z2S_TuyaDimmerSwitch =  
        new Supla::Control::Z2S_TuyaDimmerSwitch(sub_id); 
      
      Z2S_setChannelData(
        Supla_Z2S_TuyaDimmerSwitch->getZ2SCorePtr(), device, free_slot,
        Supla_Z2S_TuyaDimmerSwitch->getChannelNumber(), 
        SUPLA_CHANNELTYPE_DIMMER, sub_id, name, func);

      Supla_Z2S_TuyaDimmerSwitch->setInitialCaption(name);
      Supla_Z2S_TuyaDimmerSwitch->setDefaultFunction(func);

    initZ2SDeviceDimmer(
      free_slot, Supla_Z2S_TuyaDimmerSwitch->getZ2SChannel(),
      Supla_Z2S_TuyaDimmerSwitch->getZ2SElementPtr());

    Supla_Z2S_TuyaDimmerSwitch->getChannel()->setSubDeviceId(
      device->zb_device_id + 1);
    Supla_Z2S_TuyaDimmerSwitch->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(Supla_Z2S_TuyaDimmerSwitch->getZ2SCorePtr());

    Supla_Z2S_TuyaDimmerSwitch->onLoadConfig(&SuplaDevice);
    Supla_Z2S_TuyaDimmerSwitch->onInit();
    } break;

    

    default: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();
      
      auto Supla_Z2S_DimmerInterface = 
        new Supla::Control::Z2S_DimmerInterface(); 

      Z2S_setChannelData(
        Supla_Z2S_DimmerInterface->getZ2SCorePtr(), device, free_slot,
        Supla_Z2S_DimmerInterface->getChannelNumber(), 
        SUPLA_CHANNELTYPE_DIMMER, sub_id, name, func); 

      Supla_Z2S_DimmerInterface->setInitialCaption(name);
      Supla_Z2S_DimmerInterface->setDefaultFunction(func);

      initZ2SDeviceDimmer(
        free_slot, Supla_Z2S_DimmerInterface->getZ2SChannel(),
        Supla_Z2S_DimmerInterface->getZ2SElementPtr());

      Supla_Z2S_DimmerInterface->getChannel()->setSubDeviceId(
        device->zb_device_id + 1);
      Supla_Z2S_DimmerInterface->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_Z2S_DimmerInterface->getZ2SCorePtr());

      Supla_Z2S_DimmerInterface->onLoadConfig(&SuplaDevice);
      Supla_Z2S_DimmerInterface->onInit(); 
    } break;
  }  
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

  SuplaDevice.saveStateToStorage();
  Supla::Storage::ConfigInstance()->commit();
      
  auto Supla_Z2S_DimmerInterface = new Supla::Control::Z2S_DimmerInterface(); 
  
  if (Supla_Z2S_DimmerInterface) {

    Z2S_setChannelData(
      Supla_Z2S_DimmerInterface->getZ2SCorePtr(), device, free_slot,
      Supla_Z2S_DimmerInterface->getChannelNumber(), 
      SUPLA_CHANNELTYPE_DIMMER, sub_id, name, function);

    Supla_Z2S_DimmerInterface->setInitialCaption(name);
    Supla_Z2S_DimmerInterface->setDefaultFunction(function);

    initZ2SDeviceDimmer(
      free_slot, Supla_Z2S_DimmerInterface->getZ2SChannel(),
      Supla_Z2S_DimmerInterface->getZ2SElementPtr());

    Supla_Z2S_DimmerInterface->getChannel()->setSubDeviceId(
      device->zb_device_id + 1);
    Supla_Z2S_DimmerInterface->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(Supla_Z2S_DimmerInterface->getZ2SCorePtr());

    Supla_Z2S_DimmerInterface->onLoadConfig(&SuplaDevice);
    Supla_Z2S_DimmerInterface->onInit(); 
  }
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

