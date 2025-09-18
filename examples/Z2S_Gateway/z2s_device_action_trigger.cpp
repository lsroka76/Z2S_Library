#include "z2s_device_action_trigger.h"

void initZ2SDeviceActionTrigger(int16_t channel_number_slot) {
  
  uint32_t debounce_time_ms = 100;

  if (z2s_channels_table[channel_number_slot].model_id == Z2S_DEVICE_DESC_TUYA_SWITCH_4X3)
    debounce_time_ms = 1500;
  
  if (z2s_channels_table[channel_number_slot].refresh_secs > 0)
    debounce_time_ms = z2s_channels_table[channel_number_slot].refresh_secs;

  auto Supla_Z2S_ActionTrigger = new Supla::Control::VirtualRelaySceneSwitch(0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER, debounce_time_ms);
  
  Supla_Z2S_ActionTrigger->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_ActionTrigger->setInitialCaption(z2s_channels_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_channels_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_ActionTrigger->setDefaultFunction(z2s_channels_table[channel_number_slot].Supla_channel_func);

  Supla_Z2S_ActionTrigger->setDefaultStateRestore();
}

void addZ2SDeviceActionTrigger(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func) {
  
  auto Supla_Z2S_ActionTrigger = new Supla::Control::VirtualRelaySceneSwitch(0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER);

  if (name == nullptr)
    name = (char*)default_at_name;
  
  Supla_Z2S_ActionTrigger->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_ActionTrigger->setDefaultFunction(func);
  
  Z2S_fillChannelsTableSlot(device, free_slot, Supla_Z2S_ActionTrigger->getChannelNumber(), SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id, name, func);
}

void msgZ2SDeviceActionTrigger(int16_t channel_number_slot) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceActionTrigger - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());
  
  auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (element != nullptr) { // && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    
    auto Supla_Z2S_ActionTrigger = reinterpret_cast<Supla::Control::VirtualRelaySceneSwitch *>(element);
    
    Supla_Z2S_ActionTrigger->toggle();
    //Supla_Z2S_ActionTrigger->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
  }
}

/*void msgZ2SDeviceActionTriggerBatteryLevel(int16_t channel_number_slot, uint8_t battery_level) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceActionTriggerBatteryLevel - invalid channel number slot");
    return;
  }
  
  auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (element != nullptr) { // && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    
    auto Supla_Z2S_ActionTrigger = reinterpret_cast<Supla::Control::VirtualRelaySceneSwitch *>(element);
    
    Supla_Z2S_ActionTrigger->getChannel()->setBatteryLevel(battery_level);
    //Supla_Z2S_ActionTrigger->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
  }
}*/
