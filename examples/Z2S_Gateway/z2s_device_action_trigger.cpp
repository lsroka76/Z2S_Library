#include "z2s_device_action_trigger.h"

/*****************************************************************************/

void initZ2SDeviceActionTrigger(int16_t channel_number_slot) {
  
  if (z2s_channels_table[channel_number_slot].user_data_flags &
      USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0) {
    initZ2SDeviceActionTriggerV2(channel_number_slot);
    return;
  }
  
  uint32_t debounce_time_ms = 100;

  if (z2s_channels_table[channel_number_slot].model_id == 
        Z2S_DEVICE_DESC_TUYA_SWITCH_4X3)
    debounce_time_ms = 1500;
  
  if (z2s_channels_table[channel_number_slot].refresh_secs > 0)
    debounce_time_ms = z2s_channels_table[channel_number_slot].refresh_secs;

  auto Supla_Z2S_ActionTrigger = 
    new Supla::Control::VirtualRelaySceneSwitch(
      0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER, debounce_time_ms);
  
  Supla_Z2S_ActionTrigger->getChannel()->setChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_ActionTrigger->setInitialCaption(
      z2s_channels_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_channels_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_ActionTrigger->setDefaultFunction(
      z2s_channels_table[channel_number_slot].Supla_channel_func);

  Supla_Z2S_ActionTrigger->setDefaultStateRestore();
}

/*****************************************************************************/

void initZ2SDeviceActionTriggerV2(int16_t channel_number_slot) {
  
  uint32_t debounce_time_ms = 100;

  if (z2s_channels_table[channel_number_slot].model_id == 
        Z2S_DEVICE_DESC_TUYA_SWITCH_4X3)
    debounce_time_ms = 1500;
  
  if (z2s_channels_table[channel_number_slot].refresh_secs > 0)
    debounce_time_ms = z2s_channels_table[channel_number_slot].refresh_secs;

  z2s_channels_table[channel_number_slot].button_last_seen_ms = 0;
  z2s_channels_table[channel_number_slot].button_debounce_ms = 
    debounce_time_ms;

  auto Supla_Z2S_ActionTrigger = new Supla::Control::LocalActionTrigger();
  
  Supla_Z2S_ActionTrigger->getChannel()->setChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_ActionTrigger->setInitialCaption(
      z2s_channels_table[channel_number_slot].Supla_channel_name);
  
  Supla_Z2S_ActionTrigger->getChannel()->setActionTriggerCaps(
    z2s_channels_table[channel_number_slot].virtual_button_data.button_flags);
}

/*****************************************************************************/

void addZ2SDeviceActionTrigger(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t func) {

  addZ2SDeviceActionTriggerV2(device, free_slot, sub_id, name, func);
  return; //temp test

  auto Supla_Z2S_ActionTrigger = new Supla::Control::VirtualRelaySceneSwitch(
    0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER);

  if (name == nullptr)
    name = (char*)default_at_name;
  
  Supla_Z2S_ActionTrigger->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_ActionTrigger->setDefaultFunction(func);
  
  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_ActionTrigger->getChannelNumber(), 
    SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id, name, func);
}

/*****************************************************************************/

void addZ2SDeviceActionTriggerV2(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t func) {

  virtual_button_data_t virtual_button_data = {};

  getVirtualButtonNumber(
    virtual_button_data, device->endpoint, device->cluster_id, 
    device->model_id, sub_id);

  int16_t channel_number_slot = Z2S_findChannelNumberSlotV2(
    device->ieee_addr, device->endpoint, device->cluster_id, 
    SUPLA_CHANNELTYPE_ACTIONTRIGGER, 0x40 + virtual_button_data.button_id, 
    USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0);
  
  if (channel_number_slot < 0)  {

    log_i("no AT channel found - registering new one!");

    auto Supla_Z2S_ActionTrigger = new Supla::Control::LocalActionTrigger();
    //auto Supla_Z2S_VirtualButton = new Supla::LocalActionVirtualButton();

    if (getVirtualButtonNumber(
          virtual_button_data, device->endpoint, device->cluster_id, 
          device->model_id, sub_id)) {

      Supla_Z2S_ActionTrigger->getChannel()->setActionTriggerCaps(
        virtual_button_data.button_action_trigger_flag);
      
      Supla_Z2S_ActionTrigger->setInitialCaption(
        virtual_button_data.button_action_trigger_name);

    }

  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_ActionTrigger->getChannelNumber(), 
    SUPLA_CHANNELTYPE_ACTIONTRIGGER, 0x40 + virtual_button_data.button_id,
    virtual_button_data.button_action_trigger_name, func);

  z2s_channels_table[free_slot].virtual_button_data.button_flags =
    virtual_button_data.button_action_trigger_flag;

  z2s_channels_table[free_slot].user_data_flags |=
    USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0;

  Z2S_saveChannelsTable();

  } else {

    uint32_t button_flags = 
      z2s_channels_table[channel_number_slot].virtual_button_data.button_flags;
    
    log_i(
      "AT channel found at slot %02u, virtual button flags %08x", 
      channel_number_slot, button_flags);

    auto element = Supla::Element::getElementByChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

    if (element) {
    
      auto Supla_Z2S_ActionTrigger = 
        reinterpret_cast<Supla::Control::LocalActionTrigger *>(element);

      if (getVirtualButtonNumber(
          virtual_button_data, device->endpoint, device->cluster_id, 
          device->model_id, sub_id)) {

        button_flags |= virtual_button_data.button_action_trigger_flag;
        Supla_Z2S_ActionTrigger->getChannel()->setActionTriggerCaps(
          button_flags);
        
        z2s_channels_table[channel_number_slot].\
          virtual_button_data.button_flags = button_flags;
        Z2S_saveChannelsTable();
      }
    }
  }  
}

/*****************************************************************************/

void msgZ2SDeviceActionTrigger(int16_t channel_number_slot) {

  /*if (channel_number_slot < 0) {
    
    log_e("invalid channel number slot");
    return;
  }

  Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].ieee_addr, millis());*/
  
  auto element = 
    Supla::Element::getElementByChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

  if (element) {
    
    auto Supla_Z2S_ActionTrigger = 
      reinterpret_cast<Supla::Control::VirtualRelaySceneSwitch *>(element);
    
    Supla_Z2S_ActionTrigger->toggle();
  }
}

/*****************************************************************************/

void msgZ2SDeviceActionTriggerV2(int16_t channel_number_slot, int8_t sub_id) {

  if (channel_number_slot < 0) {
    
    log_e("invalid channel number slot");
    return;
  }

  Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].ieee_addr, millis());
  
  if (!(z2s_channels_table[channel_number_slot].user_data_flags &
      USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0)) {
    msgZ2SDeviceActionTrigger(channel_number_slot);
    return;
  }
  
  auto element = 
    Supla::Element::getElementByChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

  if (element) {
    
    auto Supla_Z2S_ActionTrigger = 
      reinterpret_cast<Supla::Control::LocalActionTrigger *>(element);

    virtual_button_data_t virtual_button_data = {};

    uint32_t millis_ms = millis();

    log_i(
      "millis - button_last_seen_ms = %lu, button_debounce_ms = %u",
      millis_ms - z2s_channels_table[channel_number_slot].button_last_seen_ms,
      z2s_channels_table[channel_number_slot].button_debounce_ms);

    if ((millis_ms - 
         z2s_channels_table[channel_number_slot].button_last_seen_ms) < 
         z2s_channels_table[channel_number_slot].button_debounce_ms) 
      return;
    else
      z2s_channels_table[channel_number_slot].button_last_seen_ms =
        millis_ms;

    if (getVirtualButtonNumber(
      virtual_button_data, 
      z2s_channels_table[channel_number_slot].endpoint, 
      z2s_channels_table[channel_number_slot].cluster_id, 
      z2s_channels_table[channel_number_slot].model_id, sub_id))
        Supla_Z2S_ActionTrigger->handleAction(
          0, virtual_button_data.button_action_id);
  }
}

/*****************************************************************************/

bool getVirtualButtonNumber(
  virtual_button_data_t &virtual_button_data, uint8_t endpoint_id, 
  uint16_t cluster_id, uint32_t model_id, int8_t sub_id) {

  virtual_button_data = {};

  switch (endpoint_id) {


    case 1:
      virtual_button_data.button_action_trigger_name = "BUTTON 1";
    break;


  case 2:
      virtual_button_data.button_action_trigger_name = "BUTTON 2";
    break;


  case 3:
      virtual_button_data.button_action_trigger_name = "BUTTON 3";
    break;


  case 4:
      virtual_button_data.button_action_trigger_name = "BUTTON 4";
    break;


  case 5:
      virtual_button_data.button_action_trigger_name = "BUTTON 5";
    break;


  case 6:
      virtual_button_data.button_action_trigger_name = "BUTTON 6";
    break;


  default:
      virtual_button_data.button_action_trigger_name = "BUTTON";
    break;
  }

  switch (model_id) {


    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: 
        
      switch (sub_id) {


        case TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          return true;
        } break;

        
        case TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x4;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x4;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_ROTATE_LEFT_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x5;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x5;
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return false;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB20:
    case Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB21:
    
      switch (sub_id) {


        case LIVARNO_DIMMER_SWITCH_ON_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER ON";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_OFF_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER OFF";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_DIM_UP_PRESSED_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_DIM_UP_HOLD_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_DIM_UP_LONG_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_DIM_DOWN_PRESSED_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_DIM_DOWN_HOLD_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        case LIVARNO_DIMMER_SWITCH_DIM_DOWN_LONG_PRESSED_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;



    case Z2S_DEVICE_DESC_TUYA_SWITCH_4X3:
        
      switch (sub_id) {


        case TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_HELD_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3:

      switch (sub_id) {


        case TUYA_CUSTOM_CMD_BUTTON_1_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "BUTTON 1";
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_1_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "BUTTON 1";
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_1_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "BUTTON 1";
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_2_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "BUTTON 2";
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_2_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "BUTTON 2";
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_2_HELD_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "BUTTON 2";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      }
    break;


    case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON:
    
      switch (sub_id) {


        case IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "ON";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "ON";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "OFF";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "OFF";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "LEFT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "LEFT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "RIGHT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "RIGHT";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F:  

      switch (sub_id) {


        case IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "ON";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "ON";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "OFF";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "OFF";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1:        
    case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1: 

      switch (sub_id) {


        case IKEA_CUSTOM_CMD_SYMFONISK_PLAY_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "PLAY";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "VOLUME UP";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "VOLUME DOWN";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_NEXT_TRACK_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "NEXT TRACK";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_PREV_TRACK_SID: {

          virtual_button_data.button_id = 5;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "PREV TRACK";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2:   
    
      switch (sub_id) {


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID: {

          virtual_button_data.button_id = 6;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID: {

          virtual_button_data.button_id = 6;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID: {

          virtual_button_data.button_id = 6;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID: {

          virtual_button_data.button_id = 6;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 6;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


       
    case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1:              

      switch (sub_id) {


        case NO_CUSTOM_CMD_SID:
        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DOT";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;
          

    case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3:                   

      switch (sub_id) {


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID: {

          virtual_button_data.button_id = 7;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID: {

          virtual_button_data.button_id = 7;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID: {

          virtual_button_data.button_id = 7;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID: {

          virtual_button_data.button_id = 7;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 7;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2:         

      switch (sub_id) {


        case NO_CUSTOM_CMD_SID:
        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        case IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DOTS";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;
   

    case Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F:       
    
      switch (sub_id) {


        case TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          return true;
        } break;


        case TUYA_CUSTOM_CMD_BUTTON_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F:            

      switch (sub_id) {


        case CUSTOM_CMD_BUTTON_PRESSED_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          return true;
        } break;


        case CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          return true;
        } break;


        case CUSTOM_CMD_BUTTON_HELD_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          return true;
        } break;


        case CUSTOM_CMD_BUTTON_TRIPLE_PRESSED_SID: {

          virtual_button_data.button_id = endpoint_id;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    case Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F:
        
      switch (sub_id) {


        case IAS_ACE_ARM_CMD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "ARM";
          return true;
        } break;


        case IAS_ACE_PANIC_CMD_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "PANIC";
          return true;
        } break;


        case IAS_ACE_2_CMD_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "2";
          return true;
        } break;


        case IAS_ACE_3_CMD_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "3";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;
  

    case Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F: 
        
      switch (sub_id) {


        case ADEO_CUSTOM_CMD_BUTTON_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          return true;
        } break;


        case ADEO_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          return true;
        } break;


        case ADEO_CUSTOM_CMD_BUTTON_HELD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;
     

    //case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH:        
    //case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1:         
    case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2: 
    
      switch (sub_id) {


        case PHILIPS_HUE_DIMMER_SWITCH_ON_PRESS_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER ON";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_ON_PRESS_RELEASE_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DIMMER ON";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_ON_HOLD_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DIMMER ON";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_ON_HOLD_RELEASE_SID: {

          virtual_button_data.button_id = 1;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          virtual_button_data.button_action_trigger_name = "DIMMER ON";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_UP_PRESS_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_UP_PRESS_RELEASE_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_UP_HOLD_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_UP_HOLD_RELEASE_SID: {

          virtual_button_data.button_id = 2;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          virtual_button_data.button_action_trigger_name = "DIMMER UP";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_DOWN_PRESS_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_DOWN_PRESS_RELEASE_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_DOWN_HOLD_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_DOWN_HOLD_RELEASE_SID: {

          virtual_button_data.button_id = 3;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          virtual_button_data.button_action_trigger_name = "DIMMER DOWN";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_OFF_PRESS_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x1;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x1;
          virtual_button_data.button_action_trigger_name = "DIMMER OFF";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_OFF_PRESS_RELEASE_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x2;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x2;
          virtual_button_data.button_action_trigger_name = "DIMMER OFF";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_OFF_HOLD_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_HOLD;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_HOLD;
          virtual_button_data.button_action_trigger_name = "DIMMER OFF";
          return true;
        } break;


        case PHILIPS_HUE_DIMMER_SWITCH_OFF_HOLD_RELEASE_SID: {

          virtual_button_data.button_id = 4;
          virtual_button_data.button_action_trigger_flag = 
            SUPLA_ACTION_CAP_SHORT_PRESS_x3;
          virtual_button_data.button_action_id = 
            Supla::SEND_AT_SHORT_PRESS_x3;
          virtual_button_data.button_action_trigger_name = "DIMMER OFF";
          return true;
        } break;


        default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
      } 
    break;


    default: {

          log_e(
            "unknown combination: model id = 0x%04X, sub id = 0x%02X", 
            model_id, sub_id);
          return 0;
        } break;
  }
}
