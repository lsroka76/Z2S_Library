#include "z2s_device_dimmer.h"

void initZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel, int8_t sub_id) {

  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: channel_element = new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device, sub_id); break;
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
  }
  if (channel_element)
    channel_element->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func) {
  
  Supla::ChannelElement *channel_element = nullptr;

  switch (device->model_id) {
    //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
    case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: channel_element = new Supla::Control::Z2S_TuyaDimmerSwitch(gateway, device, sub_id); break;
    case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: channel_element = new Supla::Control::Z2S_TuyaDimmerBulb(gateway, device); break;
  }
  if (channel_element)
    Z2S_fillDevicesTableSlot(device, free_slot, channel_element->getChannelNumber(), SUPLA_CHANNELTYPE_DIMMER, sub_id, name, func);
}

void addZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name, uint32_t func) {
  
  addZ2SDeviceDimmer(gateway, device, free_slot, -1, name, func);
}

void msgZ2SDeviceDimmer(uint32_t model_id, uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                       uint16_t payload_size, uint8_t *payload) {

  log_i("msgZ2SDeviceDimmer custom command %d for model %d", command_id, model_id);
  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {
    switch (model_id) {
      //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
      //case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: channel_element = new Supla::Control::Z2S_DimmerBulb(gateway, device); break;
      case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
        log_i("command id 0x%x", command_id);
        for (uint8_t i = 0; i < payload_size; i++)
          log_i("payload [%d] = %d", i, *(((uint8_t *)payload)+i));
        auto Supla_Z2S_TuyaDimmerSwitch = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerSwitch *>(element);
        switch (*(payload + 2)) {
          case 0x01:
          case 0x07: 
            if ((*(payload + 6)) == 0x01) Supla_Z2S_TuyaDimmerSwitch->setStateOnServer(true);
            else Supla_Z2S_TuyaDimmerSwitch->setStateOnServer(false); break;
          case 0x02:
          case 0x08: {
            //uint8_t _brightness = map(*(payload + 9), 0, 254, 0, 100); 
            Supla_Z2S_TuyaDimmerSwitch->setValueOnServer(*(payload + 9));
          } break;
        }
      } break;
    }
  }
}

void msgZ2SDeviceDimmer(uint32_t model_id, uint8_t Supla_channel, int16_t level, bool state) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {
    switch (model_id) {
      //case Z2S_DEVICE_DESC_DIMMER_LIGHT_SOURCE : break; 
      case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: {
        auto Supla_Z2S_TuyaDimmerBulb = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerBulb *>(element);
        if (level == -1)
          Supla_Z2S_TuyaDimmerBulb->setStateOnServer(state);
        else
          Supla_Z2S_TuyaDimmerBulb->setValueOnServer(level);
      } break;
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB: {
        auto Supla_Z2S_TuyaDimmerBulb = reinterpret_cast<Supla::Control::Z2S_TuyaDimmerBulb *>(element);
        if (level == -1)
          Supla_Z2S_TuyaDimmerBulb->setStateOnServer(state);
        else
          Supla_Z2S_TuyaDimmerBulb->setValueOnServer(level);
      } break;
    }
  }
}