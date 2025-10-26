#include "z2s_device_local_action_handler.h"

NetworkClient Z2S_NetworkClient;

IPAddress Z2S_IPAddress;

/*---------------------------------------------------------------------------------------------------------------------------*/

const char* getZ2SDeviceLocalActionHandlerTypeName(int16_t channel_number_slot){

  switch (z2s_channels_table[channel_number_slot].local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {
      
      return "Local action handler";
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY:
      
      return "Local virtual relay";
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


    default:

    break;  
  }

  return "Unkown local object!";
}

/*---------------------------------------------------------------------------------------------------------------------------*/

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(
  int16_t channel_number_slot) {

  switch (z2s_channels_table[channel_number_slot].local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {
      
      return 
        ACTION_HANDLERS_DEFAULT_NAMES[z2s_channels_table[channel_number_slot].
        local_action_handler_data.logic_operator];
    } break;

    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY:
    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY:
    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY:
    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER:
      
      return "No special functions";
    break;

    default:
    break;
  }
  return "Unknown local object function!";
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceLocalActionHandler(
  int16_t channel_number_slot)  {

  switch (z2s_channels_table[channel_number_slot].
            local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {


      auto Supla_LocalActionHandlerWithTrigger = 
        new Supla::LocalActionHandlerWithTrigger(
          z2s_channels_table[channel_number_slot].
          local_action_handler_data.logic_operator); 

      z2s_channels_table[channel_number_slot].
      local_action_handler_data.Supla_element =
        Supla_LocalActionHandlerWithTrigger;

      Supla_LocalActionHandlerWithTrigger->setPostponedTurnOnSecs(  
        z2s_channels_table[channel_number_slot].keep_alive_secs);
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {
      
      uint8_t Supla_channel = 
        z2s_channels_table[channel_number_slot].Supla_channel;
      
      auto Supla_VirtualRelay = 
        new Supla::Control::VirtualRelay(); 
      
      Supla_VirtualRelay->getChannel()->setChannelNumber(Supla_channel);
    }
    break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY: {
      
      uint8_t Supla_channel = 
        z2s_channels_table[channel_number_slot].Supla_channel;
      
      auto Supla_VirtualBinary = 
        new Supla::Sensor::VirtualBinary(); 
      
      Supla_VirtualBinary->getChannel()->setChannelNumber(Supla_channel);
    }
    break;


    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER: {
      
      auto Supla_Z2S_RemoteThermometer = 
        new Supla::Sensor::Z2S_RemoteThermometer();

      Supla_Z2S_RemoteThermometer->getChannel()->setChannelNumber(
          z2s_channels_table[channel_number_slot].Supla_channel);

      uint8_t local_channel_func = 
        z2s_channels_table[channel_number_slot].local_channel_func;

      if (local_channel_func > 0)
        Supla_Z2S_RemoteThermometer->setConnectedThermometersFunction(
            local_channel_func);
      
      Supla_Z2S_RemoteThermometer->setTimeoutSecs(
          z2s_channels_table[channel_number_slot].timeout_secs);
      
      Supla_Z2S_RemoteThermometer->setConnectedThermometerTimeoutSecs(
          z2s_channels_table[channel_number_slot].refresh_secs);
    } break; 


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY: {
      
      uint8_t Supla_channel = 
        z2s_channels_table[channel_number_slot].Supla_channel;
      
      auto Supla_Z2S_RemoteRelay = 
        new Supla::Control::Z2S_RemoteRelay(&Z2S_NetworkClient, 0xFF); 

      Supla_Z2S_RemoteRelay->getChannel()->setChannelNumber(Supla_channel);

      uint8_t remote_address_type = 
        Z2S_checkChannelFlags(channel_number_slot, 
                              USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ?
        REMOTE_ADDRESS_TYPE_MDNS : REMOTE_ADDRESS_TYPE_IP4;
      
      switch (remote_address_type) {

        case REMOTE_ADDRESS_TYPE_IP4: {

          Supla_Z2S_RemoteRelay->setRemoteGatewayIPAddress(
            z2s_channels_table[channel_number_slot].\
              remote_channel_data.remote_ip_address);
          
          Supla_Z2S_RemoteRelay->setRemoteGatewaySuplaChannel(
            z2s_channels_table[channel_number_slot].Supla_remote_channel);
        } break;


        case REMOTE_ADDRESS_TYPE_MDNS: {

          Supla_Z2S_RemoteRelay->setRemoteGatewayMDNSName(
            z2s_channels_table[channel_number_slot].
            remote_channel_data.mDNS_name);

          Supla_Z2S_RemoteRelay->setRemoteGatewaySuplaChannel(
            z2s_channels_table[channel_number_slot].Supla_remote_channel);
        }
      }      
    }
    break;
  } 
}

/*---------------------------------------------------------------------------------------------------------------------------*/

bool addZ2SDeviceLocalActionHandler(uint8_t local_channel_type, 
                                    uint32_t local_channel_func,
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
    z2s_channels_table[first_free_slot].ieee_addr, 
    0, 
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


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_VirtualRelay = new Supla::Control::VirtualRelay(); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_VirtualRelay->getChannelNumber();

      strcpy(z2s_channels_table[first_free_slot].
        Supla_channel_name, "LOCAL VIRTUAL RELAY");
      
      Supla_VirtualRelay->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_VirtualRelay->setDefaultFunction(local_channel_func);
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY: {

      auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary(); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_VirtualBinary->getChannelNumber();

      strcpy(z2s_channels_table[first_free_slot].
        Supla_channel_name, "LOCAL VIRTUAL BINARY");
      
      Supla_VirtualBinary->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_VirtualBinary->setDefaultFunction(local_channel_func);
    } break;


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_Z2S_RemoteRelay = 
        new Supla::Control::Z2S_RemoteRelay(&Z2S_NetworkClient,
          z2s_channels_table[first_free_slot].Supla_remote_channel); 

      z2s_channels_table[first_free_slot].Supla_channel = 
        Supla_Z2S_RemoteRelay->getChannelNumber();

      Z2S_clearChannelFlags(first_free_slot, 
                            USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS,
                            false);

      strcpy(z2s_channels_table[first_free_slot].
        Supla_channel_name, "LOCAL REMOTE RELAY");
      
      Supla_Z2S_RemoteRelay->setInitialCaption(
          z2s_channels_table[first_free_slot].Supla_channel_name);

      Supla_Z2S_RemoteRelay->setDefaultFunction(local_channel_func);
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
        CONNECTED_THERMOMETERS_FNC_AVG);
      Supla_Z2S_RemoteThermometer->setConnectedThermometerTimeoutSecs(
        MINUTES_30);
    } break;


    default:
      log_i("unknown local action handler channel type 0%u", 
            local_channel_type);
    break;
  }

  z2s_channels_table[first_free_slot].Supla_secondary_channel = 0xFF;

  z2s_channels_table[first_free_slot].Supla_channel_type = 0x0000;

  z2s_channels_table[first_free_slot].sub_id = 0; 
  
  z2s_channels_table[first_free_slot].Supla_channel_func = local_channel_func;

  z2s_channels_table[first_free_slot].ZB_device_id = 0xFF;
  
  return Z2S_saveChannelsTable();
}