#include "z2s_device_dimmer.h"

void initZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {

  switch (device->model_id) {

    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
      auto Supla_Z2S_TuyaDimmerSwitch = new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device, z2s_devices_table[channel_number_slot].sub_id);
      Supla_Z2S_TuyaDimmerSwitch->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

      if (strlen(z2s_devices_table[channel_number_slot].Supla_channel_name) > 0) 
        Supla_Z2S_TuyaDimmerSwitch->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);
        
      if (z2s_devices_table[channel_number_slot].Supla_channel_func !=0) 
        Supla_Z2S_TuyaDimmerSwitch->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

     } break;
    
    case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_HS: {
      auto Supla_Z2S_DimmerInterface = new Supla::Control::Z2S_DimmerInterface(gateway, device, z2s_devices_table[channel_number_slot].sub_id); 
      Supla_Z2S_DimmerInterface->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

      if (strlen(z2s_devices_table[channel_number_slot].Supla_channel_name) > 0) 
        Supla_Z2S_DimmerInterface->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);

      if (z2s_devices_table[channel_number_slot].Supla_channel_func !=0) 
        Supla_Z2S_DimmerInterface->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

      Supla_Z2S_DimmerInterface->setKeepAliveSecs(z2s_devices_table[channel_number_slot].keep_alive_secs);
      Supla_Z2S_DimmerInterface->setTimeoutSecs(z2s_devices_table[channel_number_slot].timeout_secs);
    } break;
  }
}

void addZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    
    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: 
      channel_element = new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device, sub_id); break;

    case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB:
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A: 
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
    case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
    case Z2S_DEVICE_DESC_RGBW_BULB_XY:
    case Z2S_DEVICE_DESC_RGBW_BULB_HS:
      channel_element = new Supla::Control::Z2S_DimmerInterface(gateway, device, sub_id); break;
  }
  if (channel_element)
    Z2S_fillDevicesTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_DIMMER, sub_id, name, func);
}

void addZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  addZ2SDeviceDimmer(gateway, device, free_slot, -1, name, func);
}



void msgZ2SDeviceDimmer(int16_t channel_number_slot, int16_t level, bool state, signed char rssi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceDimmer - invalid channel number slot");
    return;
  }
  
  //Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {
    switch (z2s_devices_table[channel_number_slot].model_id) {
      
      case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: 
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B: 
      case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
      case Z2S_DEVICE_DESC_RGBW_BULB_XY:
      case Z2S_DEVICE_DESC_RGBW_BULB_HS: {
        auto Supla_Z2S_DimmerInterface = reinterpret_cast<Supla::Control::Z2S_DimmerInterface *>(element);
        //Supla_Z2S_TuyaDimmerBulb->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
          Supla_Z2S_DimmerInterface->setValueOnServer(level);
      } break;

      case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
        auto Supla_Z2S_TuyaDimmerSwitch = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerSwitch *>(element);
        //Supla_Z2S_TuyaDimmerSwitch->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        if (level == DIMMER_NO_LEVEL_DATA)
          Supla_Z2S_TuyaDimmerSwitch->setStateOnServer(state);
        else
          Supla_Z2S_TuyaDimmerSwitch->setValueOnServer(level);
      } break;
    }
  }
}
