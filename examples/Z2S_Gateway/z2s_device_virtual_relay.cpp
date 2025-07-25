#include "z2s_device_virtual_relay.h"

void initZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {
  
  if (z2s_devices_table[channel_number_slot].Supla_channel_func == SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER)
  {
    uint8_t z2s_function = Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER;

    switch (z2s_devices_table[channel_number_slot].model_id) {

      case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR: 
        z2s_function = Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR; break;
      
      case Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE: 
        z2s_function = Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT; break;
    }
    
    auto Supla_Z2S_RollerShutter = new Supla::Control::Z2S_RollerShutter(gateway, device, z2s_function);
  
    Supla_Z2S_RollerShutter->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

    if (strlen(z2s_devices_table[channel_number_slot].Supla_channel_name) > 0) 
      Supla_Z2S_RollerShutter->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);  
    
    Supla_Z2S_RollerShutter->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

    Supla_Z2S_RollerShutter->setKeepAliveSecs(z2s_devices_table[channel_number_slot].keep_alive_secs);
    Supla_Z2S_RollerShutter->setTimeoutSecs(z2s_devices_table[channel_number_slot].timeout_secs);
  } else {  
    
    uint8_t z2s_function = Z2S_VIRTUAL_RELAY_FNC_NONE;

    switch (z2s_devices_table[channel_number_slot].model_id) {

      case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM: {

        switch (z2s_devices_table[channel_number_slot].sub_id) {
          
          case IAS_WD_SILENT_ALARM_SID:
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM; break;

          case IAS_WD_LOUD_ALARM_SID:
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM; break;
        }
      } break;

      case Z2S_DEVICE_DESC_MOES_ALARM: {

        switch (z2s_devices_table[channel_number_slot].sub_id) {
          
          case MOES_ALARM_SWITCH_SID:
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_SWITCH; break;

          case MOES_ALARM_MELODY_SID:
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_MELODY; break;

          case MOES_ALARM_VOLUME_SID:
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_VOLUME; break;

          case MOES_ALARM_DURATION_SID:
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_DURATION; break;
        }
      } break;       
    }

    auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway, device, z2s_function);
  
    Supla_Z2S_VirtualRelay->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

    if (strlen(z2s_devices_table[channel_number_slot].Supla_channel_name) > 0) 
      Supla_Z2S_VirtualRelay->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);  
    if (z2s_devices_table[channel_number_slot].Supla_channel_func !=0) 
      Supla_Z2S_VirtualRelay->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

    Supla_Z2S_VirtualRelay->setKeepAliveSecs(z2s_devices_table[channel_number_slot].keep_alive_secs);
    Supla_Z2S_VirtualRelay->setTimeoutSecs(z2s_devices_table[channel_number_slot].timeout_secs);
  }
}

                                      
void addZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
                              int8_t sub_id, char *name, uint32_t func) {
  
  if (func == SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER) {

    auto Supla_Z2S_RollerShutter = new Supla::Control::Z2S_RollerShutter(gateway, device, Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER);

    if (name) 
      Supla_Z2S_RollerShutter->setInitialCaption(name);
  
    Supla_Z2S_RollerShutter->setDefaultFunction(func);
  
    Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_RollerShutter->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY, sub_id, name, func);

  } else {

    auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway,device);

    if (name) 
      Supla_Z2S_VirtualRelay->setInitialCaption(name);
  
    if (func !=0) 
      Supla_Z2S_VirtualRelay->setDefaultFunction(func);
  
    Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY, sub_id, name, func);
  }
}

void msgZ2SDeviceVirtualRelay(int16_t channel_number_slot, bool state, signed char rssi) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceVirtualRelay - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    
    auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
    
    //Supla_Z2S_VirtualRelay->getChannel()->setStateOnline();
    Supla_Z2S_VirtualRelay->Z2S_setOnOff(state);     
    //Supla_Z2S_VirtualRelay->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
  }
}

void msgZ2SDeviceVirtualRelayValue(int16_t channel_number_slot, uint8_t value_id, uint32_t value) {

  if (channel_number_slot < 0) {
    
    log_e("error: invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    
    auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);

    switch (value_id) {

      case VRV_U8_ID:
        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8((uint8_t)value); break;
      case VRV_S8_ID:
        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8((int8_t)value); break;
      case VRV_U32_ID:
        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32((uint32_t)value); break;
      case VRV_S32_ID:
        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS32((int32_t)value); break;
      default:
        log_e("error: invalid VRV_ID"); break;
    }         
  }
}



void msgZ2SDeviceRollerShutter(int16_t channel_number_slot, uint8_t msg_id, uint16_t msg_value, signed char rssi) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceRollerShutter - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {
    
    auto Supla_Z2S_RollerShutter = reinterpret_cast<Supla::Control::Z2S_RollerShutter *>(element);

    switch (msg_id) {
      case RS_CURRENT_POSITION_LIFT_PERCENTAGE_MSG:
        Supla_Z2S_RollerShutter->setRSCurrentPosition(msg_value); break;
      case RS_MOVING_DIRECTION_MSG:
        Supla_Z2S_RollerShutter->setRSMovingDirection(msg_value); break;
    }
    //Supla_Z2S_VirtualRelay->getChannel()->setStateOnline();
    //Supla_Z2S_VirtualRelay->Z2S_setOnOff(state);     
    //Supla_Z2S_VirtualRelay->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));     
  }
}