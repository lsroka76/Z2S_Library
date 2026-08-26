#include "z2s_device_local_action_handler.h"

NetworkClient Z2S_NetworkClient;

IPAddress Z2S_IPAddress;

/*****************************************************************************/

const char* getZ2SDeviceLocalActionHandlerTypeName(
  int16_t channel_number_slot) {

    getZ2SDeviceLocalActionHandlerTypeName(
      z2s_channels_table + channel_number_slot);
}

const char* getZ2SDeviceLocalActionHandlerTypeName(
  z2s_device_params_t* _z2s_channel) {

  switch (_z2s_channel->local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {
      
      return "Local action handler";
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY:
      
      return "Local virtual relay";
    break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC:
      
      return "Local virtual hvac";
    break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY:
      
      return "Local virtual binary";
    break;


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY:
      
      return "Local remote relay";
    break;


    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER:
      
      return "Local remote thermometer";
    break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BUTTON:
      
      return "Local virtual button";
    break;


case LOCAL_CHANNEL_TYPE_SWITCHBOT:
      
      return "Switchbot object";
    break;


    case LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS:

      return "Gateway events";
    break;


    default:

    break;  
  }

  return "Unkown local object!";
}

/*****************************************************************************/

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(
  int16_t channel_number_slot) {

    getZ2SDeviceLocalActionHandlerLogicOperatorName(
      z2s_channels_table + channel_number_slot);
}

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(
  z2s_device_params_t* _z2s_channel) {

  switch (_z2s_channel->local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {
      
      return 
        ACTION_HANDLERS_DEFAULT_NAMES[
          _z2s_channel->local_action_handler_data.logic_operator];
    } break;

    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY:
    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY:
    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY:
    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER:
    case LOCAL_CHANNEL_TYPE_VIRTUAL_BUTTON:
    case LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC:
    case LOCAL_CHANNEL_TYPE_SWITCHBOT:
    case LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS:
      
      return "No special functions";
    break;

    default:
    break;
  }
  return "Unknown local object function!";
}

/*****************************************************************************/

void initZ2SDeviceGatewayEvents(int16_t channel_number_slot) {

  initZ2SDeviceGatewayEvents(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

void initZ2SDeviceGatewayEvents(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  if (_z2s_channel->local_channel_type != LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS) {
    
    log_e(
      "Fatal error - channel at %u is not LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS!",
      channel_index);
  
    return;
  }
  
  auto Supla_GatewayEvents = new Supla::GatewayEvents(); 

  _z2s_channel->local_action_handler_data.Supla_element = Supla_GatewayEvents;

  Supla_GatewayEvents->setZ2SChannel(channel_index, _z2s_channel);
  
  SuplaDevice.addAction(
    0x6000, Supla_GatewayEvents, Supla::ON_DEVICE_STATUS_CHANGE, false);
}

/*****************************************************************************/

void initZ2SDeviceLocalActionHandler(int16_t channel_number_slot)  {

  initZ2SDeviceLocalActionHandler(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

void initZ2SDeviceLocalActionHandler(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  switch (_z2s_channel->local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {

      auto Supla_LocalActionHandlerWithTrigger = 
        new Supla::LocalActionHandlerWithTrigger(
          _z2s_channel->local_action_handler_data.logic_operator); 

      _z2s_channel->local_action_handler_data.Supla_element =
        Supla_LocalActionHandlerWithTrigger;

      Supla_LocalActionHandlerWithTrigger->setZ2SChannel(
        channel_index, _z2s_channel);

      Supla_LocalActionHandlerWithTrigger->setPostponedTurnOnSecs(  
        _z2s_channel->keep_alive_secs);
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BUTTON: {

      auto Supla_LocalActionVirtualButton = 
        new Supla::LocalActionVirtualButton(); 

      _z2s_channel->local_action_handler_data.Supla_element =
        Supla_LocalActionVirtualButton;
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {
      
      uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
      auto Supla_LocalVirtualRelay = 
        new Supla::Control::LocalVirtualRelay(RELAY_FLAGS); 

      Supla_LocalVirtualRelay->setZ2SChannel(channel_index, _z2s_channel);
      
      Supla_LocalVirtualRelay->getChannel()->setChannelNumber(Supla_channel);
      Supla_LocalVirtualRelay->setDefaultFunction(
        SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_LocalVirtualRelay->setDefaultStateRestore();
    }
    break;


    case LOCAL_CHANNEL_TYPE_SWITCHBOT: {
      
      uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
      auto Supla_SwitchBotRelay = 
        new Supla::Control::SwitchBotRelay(_z2s_channel->local_channel_func); 

      Supla_SwitchBotRelay->setZ2SChannel(channel_index, _z2s_channel);
      
      Supla_SwitchBotRelay->getChannel()->setChannelNumber(Supla_channel);
      Supla_SwitchBotRelay->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_SwitchBotRelay->setDefaultStateRestore();

      channel_extended_data_sb_t channel_extended_data_sb = {};
  
      if (Z2S_loadChannelExtendedData(
            channel_index, CHANNEL_EXTENDED_DATA_TYPE_SB,
            (uint8_t*)&channel_extended_data_sb)) {

        log_i(
          "SwitchBot device BLE address: %s",
          channel_extended_data_sb.ble_mac_address);
        log_i(
          "SwitchBot device token: %s (%u)",
          channel_extended_data_sb.token, channel_extended_data_sb.token_size);
        log_i(
          "SwitchBot device json payload: %s (%u)",
          channel_extended_data_sb.json_payload, 
          channel_extended_data_sb.json_payload_size);

        Supla_SwitchBotRelay->updateSwitchBotData(
          channel_extended_data_sb, SB_UPDATE_DATA_LOAD_DIR);
      }
    }
    break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC: {
      
      uint8_t Supla_channel = _z2s_channel->Supla_channel;

      auto Supla_VirtualOutputInterface = 
        new Supla::Control::VirtualOutputInterface();

      auto Supla_VirtualHvac = 
        new Supla::Control::LocalVirtualHvac(Supla_VirtualOutputInterface); 

      Supla_VirtualHvac->setZ2SChannel(channel_index, _z2s_channel);
      
      Supla_VirtualHvac->getChannel()->setChannelNumber(Supla_channel);
      Supla_VirtualHvac->enableDomesticHotWaterFunctionSupport();
    }

    break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY: {
      
      uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
      auto Supla_LocalVirtualBinary = 
        new Supla::Sensor::LocalVirtualBinary(true); 

      Supla_LocalVirtualBinary->setZ2SChannel(channel_index, _z2s_channel);
      
      Supla_LocalVirtualBinary->getChannel()->setChannelNumber(Supla_channel);
      Supla_LocalVirtualBinary->setDefaultFunction(
        SUPLA_CHANNELFNC_BINARY_SENSOR);

      if (Supla::Notification::RegisterNotification(
          Supla_channel, false, true))
        Supla_LocalVirtualBinary->registerNotification();
      
      Supla_LocalVirtualBinary->setAutoClearSecs(_z2s_channel->refresh_secs);
    }
    break;


    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER: {
      
      auto Supla_Z2S_RemoteThermometer = 
        new Supla::Sensor::Z2S_RemoteThermometer();

      Supla_Z2S_RemoteThermometer->setZ2SChannel(channel_index, _z2s_channel);

      Supla_Z2S_RemoteThermometer->getChannel()->setChannelNumber(
          _z2s_channel->Supla_channel);

      uint8_t local_channel_func = _z2s_channel->local_channel_func;

      if (local_channel_func > 0)
        Supla_Z2S_RemoteThermometer->setConnectedThermometersFunction(
          local_channel_func);
      
      Supla_Z2S_RemoteThermometer->setTimeoutSecs(_z2s_channel->timeout_secs);
    
      Supla_Z2S_RemoteThermometer->setConnectedThermometerTimeoutSecs(
        _z2s_channel->refresh_secs);
    } break; 


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY: {
      
      uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
      auto Supla_Z2S_RemoteRelay = new Supla::Control::Z2S_RemoteRelay(
        &Z2S_NetworkClient, 0xFF); 

      Supla_Z2S_RemoteRelay->setZ2SChannel(channel_index, _z2s_channel);

      Supla_Z2S_RemoteRelay->getChannel()->setChannelNumber(Supla_channel);

      Supla_Z2S_RemoteRelay->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_Z2S_RemoteRelay->setDefaultStateRestore();

      uint8_t remote_address_type = Z2S_checkChannelFlags(
        channel_index, USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ?
        REMOTE_ADDRESS_TYPE_MDNS : REMOTE_ADDRESS_TYPE_IP4;
      
      switch (remote_address_type) {

        case REMOTE_ADDRESS_TYPE_IP4: {

          Supla_Z2S_RemoteRelay->setRemoteGatewayIPAddress(
            _z2s_channel->remote_channel_data.remote_ip_address);
          
          Supla_Z2S_RemoteRelay->setRemoteGatewaySuplaChannel(
            _z2s_channel->Supla_remote_channel);
        } break;


        case REMOTE_ADDRESS_TYPE_MDNS: {

          Supla_Z2S_RemoteRelay->setRemoteGatewayMDNSName(
            _z2s_channel->remote_channel_data.mDNS_name);

          Supla_Z2S_RemoteRelay->setRemoteGatewaySuplaChannel(
            _z2s_channel->Supla_remote_channel);
        }
      }      
    }
    break;
  } 
}

/*****************************************************************************/

bool addZ2SDeviceGatewayEvents(int16_t channel_number_slot) {

  z2s_channels_table[channel_number_slot].valid_record = true;

  z2s_channels_table[channel_number_slot].extended_data_type = 
    CHANNEL_EXTENDED_DATA_TYPE_NULL;

  z2s_channels_table[channel_number_slot].local_channel_type = 
    LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS;

  memset(
    z2s_channels_table[channel_number_slot].ieee_addr, 0, 
    sizeof(esp_zb_ieee_addr_t));
  z2s_channels_table[channel_number_slot].short_addr = 0;
  z2s_channels_table[channel_number_slot].model_id = 
    Z2S_DEVICE_DESC_LOCAL_ACTION_HANDLER;

  z2s_channels_table[channel_number_slot].endpoint = 0;
  z2s_channels_table[channel_number_slot].cluster_id = 0;

  z2s_channels_table[channel_number_slot].Supla_channel = 
    Z2S_findFirstFreeLocalActionHandlerId();

  strcpy(
    z2s_channels_table[channel_number_slot].Supla_channel_name, 
    "GATEWAY EVENTS");

  auto Supla_GatewayEvents = new Supla::GatewayEvents(); 

  z2s_channels_table[channel_number_slot].\
    local_action_handler_data.Supla_element = Supla_GatewayEvents;

  SuplaDevice.addAction(
    0x6000, Supla_GatewayEvents, Supla::ON_DEVICE_STATUS_CHANGE, false);

  z2s_channels_table[channel_number_slot].Supla_secondary_channel = 0xFF;

  z2s_channels_table[channel_number_slot].Supla_channel_type = 0x0000;

  z2s_channels_table[channel_number_slot].sub_id = 0; 
  
  z2s_channels_table[channel_number_slot].Zb_device_id = 0xFF;
  
  return Z2S_saveChannelsTable();   
}

/*****************************************************************************/

bool addZ2SDeviceLocalActionHandler(
  uint8_t local_channel_type, uint32_t local_channel_func, 
  uint8_t logic_operator) {

  uint8_t first_free_slot = Z2S_findFirstFreeChannelsTableSlot();

  if (first_free_slot == 0xFF)
    return false;

  z2s_channels_table[first_free_slot].valid_record = true;

  z2s_channels_table[first_free_slot].extended_data_type = 
    CHANNEL_EXTENDED_DATA_TYPE_NULL;

  z2s_channels_table[first_free_slot].local_channel_type = 
    local_channel_type;

  memset(
    z2s_channels_table[first_free_slot].ieee_addr, 0, 
    sizeof(esp_zb_ieee_addr_t));

  z2s_channels_table[first_free_slot].short_addr = 0;

  z2s_channels_table[first_free_slot].model_id = 
    Z2S_DEVICE_DESC_LOCAL_ACTION_HANDLER;

  z2s_channels_table[first_free_slot].endpoint = 0;
  z2s_channels_table[first_free_slot].cluster_id = 0;

  switch(local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      z2s_channels_table[first_free_slot].Supla_channel = 
        Z2S_findFirstFreeLocalActionHandlerId();

      strcpy(z2s_channels_table[first_free_slot].Supla_channel_name, 
             ACTION_HANDLERS_DEFAULT_NAMES[logic_operator]);

      z2s_channels_table[first_free_slot].
        local_action_handler_data.logic_operator = logic_operator;

      z2s_channels_table[first_free_slot].
        local_action_handler_data.Supla_element = 
        new Supla::LocalActionHandlerWithTrigger(logic_operator);   
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BUTTON: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      z2s_channels_table[first_free_slot].Supla_channel = 
        Z2S_findFirstFreeLocalActionHandlerId();

      strcpy(
        z2s_channels_table[first_free_slot].Supla_channel_name, 
        "VIRTUAL BUTTON");

      /*z2s_channels_table[first_free_slot].
        local_action_handler_data.logic_operator = logic_operator;*/

      z2s_channels_table[first_free_slot].
        local_action_handler_data.Supla_element = 
        new Supla::LocalActionVirtualButton();   
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_LocalVirtualRelay = 
        new Supla::Control::LocalVirtualRelay(RELAY_FLAGS); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_LocalVirtualRelay->getChannelNumber();

      strcpy(z2s_channels_table[first_free_slot].
        Supla_channel_name, "LOCAL VIRTUAL RELAY");
      
      Supla_LocalVirtualRelay->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_LocalVirtualRelay->setDefaultFunction(
        SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_LocalVirtualRelay->setDefaultStateRestore();
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_VirtualOutputInterface = 
        new Supla::Control::VirtualOutputInterface();

      auto Supla_VirtualHvac = 
        new Supla::Control::LocalVirtualHvac(Supla_VirtualOutputInterface);

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_VirtualHvac->getChannelNumber();

      strcpy(
        z2s_channels_table[first_free_slot].Supla_channel_name, 
        "LOCAL VIRTUAL HVAC");
      
      Supla_VirtualHvac->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_VirtualHvac->enableDomesticHotWaterFunctionSupport();
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY: {

      auto Supla_LocalVirtualBinary = 
        new Supla::Sensor::LocalVirtualBinary(true); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_LocalVirtualBinary->getChannelNumber();

      strcpy(z2s_channels_table[first_free_slot].
        Supla_channel_name, "LOCAL VIRTUAL BINARY");
      
      Supla_LocalVirtualBinary->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_LocalVirtualBinary->setDefaultFunction(
        SUPLA_CHANNELFNC_BINARY_SENSOR);
    } break;


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_Z2S_RemoteRelay = 
        new Supla::Control::Z2S_RemoteRelay(&Z2S_NetworkClient,
          z2s_channels_table[first_free_slot].Supla_remote_channel); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_Z2S_RemoteRelay->getChannelNumber();

      Z2S_clearChannelFlags(
        first_free_slot, USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS, false);

      strcpy(
        z2s_channels_table[first_free_slot].Supla_channel_name, 
        "LOCAL REMOTE RELAY");
      
      Supla_Z2S_RemoteRelay->setInitialCaption(
        z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_Z2S_RemoteRelay->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_Z2S_RemoteRelay->setDefaultStateRestore();
    } break;


    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER: {

      auto Supla_Z2S_RemoteThermometer = 
        new Supla::Sensor::Z2S_RemoteThermometer();

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_Z2S_RemoteThermometer->getChannelNumber();

      strcpy(z2s_channels_table[first_free_slot].
        Supla_channel_name, "LOCAL REMOTE THERMOMETER");
      
      Supla_Z2S_RemoteThermometer->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_Z2S_RemoteThermometer->setConnectedThermometersFunction(
        local_channel_func);
      Supla_Z2S_RemoteThermometer->setConnectedThermometerTimeoutSecs(
        MINUTES_30);
    } break;


    case LOCAL_CHANNEL_TYPE_SWITCHBOT: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_SwitchBotRelay = 
        new Supla::Control::SwitchBotRelay(local_channel_func); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_SwitchBotRelay->getChannelNumber();

      /*z2s_channels_table[first_free_slot].extended_data_type = 
        CHANNEL_EXTENDED_DATA_TYPE_SB;*/
      channel_extended_data_sb_t channel_extended_data_sb = {};

      channel_extended_data_sb.device_id = local_channel_func;

      Z2S_saveChannelExtendedData(
        first_free_slot, CHANNEL_EXTENDED_DATA_TYPE_SB, 
        (uint8_t*)&channel_extended_data_sb, false);

      sprintf(
        z2s_channels_table[first_free_slot].Supla_channel_name, 
        "SWITCH BOT (%ux)", local_channel_func + 1);
      
      Supla_SwitchBotRelay->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_SwitchBotRelay->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_SwitchBotRelay->setDefaultStateRestore();
    } break;


    default:
      log_i("unknown local action handler channel type 0%u", 
            local_channel_type);
    break;
  }

  z2s_channels_table[first_free_slot].Supla_secondary_channel = 0xFF;

  z2s_channels_table[first_free_slot].Supla_channel_type = 0x0000;

  z2s_channels_table[first_free_slot].sub_id = 0; 
  
  z2s_channels_table[first_free_slot].local_channel_func = local_channel_func;

  z2s_channels_table[first_free_slot].Zb_device_id = 0xFF;
  
  return Z2S_saveChannelsTable();
}
