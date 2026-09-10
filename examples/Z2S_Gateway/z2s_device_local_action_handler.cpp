#include "z2s_device_local_action_handler.h"

NetworkClient Z2S_NetworkClient;

IPAddress Z2S_IPAddress;

/*****************************************************************************/

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

void initZ2SDeviceLocalActionHandler(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel, 
  Supla::Element *element) {

  switch (_z2s_channel->local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {

      Supla::LocalActionHandlerWithTrigger *Supla_LAHWT =nullptr;

      if (element) {

        Supla_LAHWT = static_cast<
          Supla::LocalActionHandlerWithTrigger *>(element);
      }
      else {

        Supla_LAHWT = new Supla::LocalActionHandlerWithTrigger(
          _z2s_channel->local_action_handler_data.logic_operator); 

        Supla_LAHWT->setZ2SChannel(channel_index, _z2s_channel);
      }
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {

      Supla::Control::LocalVirtualRelay *Supla_LocalVirtualRelay =
        nullptr;

      if (element) {

        Supla_LocalVirtualRelay = static_cast<
          Supla::Control::LocalVirtualRelay *>(element);
      }
      else {

        Supla_LocalVirtualRelay = new Supla::Control::LocalVirtualRelay(
          RELAY_FLAGS); 
    
        Supla_LocalVirtualRelay->setZ2SChannel(channel_index, _z2s_channel);
        uint8_t Supla_channel = _z2s_channel->Supla_channel;
        Supla_LocalVirtualRelay->getChannel()->setChannelNumber(Supla_channel);
      }
      Supla_LocalVirtualRelay->setDefaultStateRestore();
    }
    break;


    case LOCAL_CHANNEL_TYPE_SWITCHBOT: {

      Supla::Control::SwitchBotRelay *Supla_SwitchBotRelay = nullptr;

      if (element) {

        Supla_SwitchBotRelay = static_cast<
          Supla::Control::SwitchBotRelay *>(element);
      }
      else {
      
        uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
        Supla_SwitchBotRelay = new Supla::Control::SwitchBotRelay(
          _z2s_channel->local_channel_func); 

        Supla_SwitchBotRelay->setZ2SChannel(channel_index, _z2s_channel);
      
        Supla_SwitchBotRelay->getChannel()->setChannelNumber(Supla_channel);
      }
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
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC: {

      Supla::Control::LocalVirtualHvac *Supla_VirtualHvac = nullptr;

      if (element) {

        Supla_VirtualHvac = static_cast<
          Supla::Control::LocalVirtualHvac *>(element);
      }
      else {

        uint8_t Supla_channel = _z2s_channel->Supla_channel;

        auto Supla_VirtualOutputInterface = 
          new Supla::Control::VirtualOutputInterface();

        Supla_VirtualHvac = new Supla::Control::LocalVirtualHvac(
          Supla_VirtualOutputInterface); 

        Supla_VirtualHvac->setZ2SChannel(channel_index, _z2s_channel);
      
        Supla_VirtualHvac->getChannel()->setChannelNumber(Supla_channel);

        Supla_VirtualHvac->setInitialCaption(LOCAL_VIRTUAL_HVAC_NAME);
        Supla_VirtualHvac->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);
      }
      Supla_VirtualHvac->enableDomesticHotWaterFunctionSupport();
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY: {

      Supla::Sensor::LocalVirtualBinary *Supla_LocalVirtualBinary = nullptr;

      if (element) {

        Supla_LocalVirtualBinary = static_cast<
          Supla::Sensor::LocalVirtualBinary *>(element);
      }
      else {

        uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
        Supla_LocalVirtualBinary = new Supla::Sensor::LocalVirtualBinary(true); 

        Supla_LocalVirtualBinary->setZ2SChannel(channel_index, _z2s_channel);
      
        Supla_LocalVirtualBinary->getChannel()->setChannelNumber(Supla_channel);
      }
      Supla_LocalVirtualBinary->setDefaultFunction(
        SUPLA_CHANNELFNC_BINARY_SENSOR);

      if (Supla::Notification::RegisterNotification(
            Supla_LocalVirtualBinary->getZ2SChannelNumber(), false, true))
        Supla_LocalVirtualBinary->registerNotification();
    } break;


    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER: {

      Supla::Sensor::Z2S_RemoteThermometer *Supla_Z2S_RemoteThermometer = 
        nullptr;

      if (element) {

        Supla_Z2S_RemoteThermometer = static_cast<
          Supla::Sensor::Z2S_RemoteThermometer *>(element);
      }
      else {
      
        Supla_Z2S_RemoteThermometer = 
          new Supla::Sensor::Z2S_RemoteThermometer();

        Supla_Z2S_RemoteThermometer->setZ2SChannel(
          channel_index, _z2s_channel);

        Supla_Z2S_RemoteThermometer->getChannel()->setChannelNumber(
          _z2s_channel->Supla_channel);
      }
      if (!Supla_Z2S_RemoteThermometer->getConnectedThermometerTimeoutMs())
        Supla_Z2S_RemoteThermometer->setConnectedThermometerTimeoutValue(30); 
    } break; 


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY: {

      Supla::Control::Z2S_RemoteRelay *Supla_Z2S_RemoteRelay = nullptr;

      if (element) {

        Supla_Z2S_RemoteRelay = static_cast<
          Supla::Control::Z2S_RemoteRelay *>(element);
      }
      else {

        uint8_t Supla_channel = _z2s_channel->Supla_channel;
      
        Supla_Z2S_RemoteRelay = new Supla::Control::Z2S_RemoteRelay(
          &Z2S_NetworkClient, 0xFF); 

        Supla_Z2S_RemoteRelay->setZ2SChannel(channel_index, _z2s_channel);

        Supla_Z2S_RemoteRelay->getChannel()->setChannelNumber(Supla_channel);
      }
      Supla_Z2S_RemoteRelay->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      Supla_Z2S_RemoteRelay->setDefaultStateRestore();      
    }
    break;
  } 
}

/*****************************************************************************/

bool addZ2SDeviceLocalActionHandler(
  uint8_t local_channel_type, uint32_t local_channel_func, 
  uint8_t logic_operator) {

  zbg_device_params_t device = {};

  uint8_t first_free_slot = Z2S_findFirstFreeChannelsTableSlot();

  if (first_free_slot == 0xFF)
    return false;
  
  device.model_id = Z2S_DEVICE_DESC_LOCAL_ACTION_HANDLER;

  switch(local_channel_type) {


    case LOCAL_CHANNEL_TYPE_ACTION_HANDLER: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto lah = new Supla::LocalActionHandlerWithTrigger(logic_operator);

      Z2S_setLocalChannelData(
        lah->getZ2SCorePtr(), first_free_slot,
        Z2S_findFirstFreeLocalActionHandlerId(), NO_CUSTOM_CMD_SID, 
        ACTION_HANDLERS_DEFAULT_NAMES[logic_operator], SUPLA_CHANNELFNC_NONE,
        0xFF, local_channel_type, local_channel_func, logic_operator);

      initZ2SDeviceLocalActionHandler(
        first_free_slot, lah->getZ2SChannel(), lah->getZ2SElementPtr());
      addChannelsSelectorChannel(lah->getZ2SCorePtr());
      sortChannelsSelectors();
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_LocalVirtualRelay = 
        new Supla::Control::LocalVirtualRelay(RELAY_FLAGS); 

      Z2S_setLocalChannelData(
        Supla_LocalVirtualRelay->getZ2SCorePtr(), first_free_slot,
        Supla_LocalVirtualRelay->getChannelNumber(), NO_CUSTOM_CMD_SID, 
        LOCAL_VIRTUAL_RELAY_NAME, SUPLA_CHANNELFNC_POWERSWITCH, 0xFF, 
        local_channel_type, local_channel_func);
      
      Supla_LocalVirtualRelay->setInitialCaption(LOCAL_VIRTUAL_RELAY_NAME);
      Supla_LocalVirtualRelay->setDefaultFunction(
        SUPLA_CHANNELFNC_POWERSWITCH);
        
      initZ2SDeviceLocalActionHandler(
        first_free_slot, Supla_LocalVirtualRelay->getZ2SChannel(), 
        Supla_LocalVirtualRelay->getZ2SElementPtr());

      Supla_LocalVirtualRelay->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_LocalVirtualRelay->getZ2SCorePtr());
      sortChannelsSelectors();

      Supla_LocalVirtualRelay->onLoadConfig(&SuplaDevice);
      Supla_LocalVirtualRelay->onInit();
      
      Supla::Network::DisconnectProtocols();
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_VirtualOutputInterface = 
        new Supla::Control::VirtualOutputInterface();

      auto Supla_VirtualHvac = 
        new Supla::Control::LocalVirtualHvac(Supla_VirtualOutputInterface);

      Z2S_setLocalChannelData(
        Supla_VirtualHvac->getZ2SCorePtr(), first_free_slot,
        Supla_VirtualHvac->getChannelNumber(), NO_CUSTOM_CMD_SID, 
        LOCAL_VIRTUAL_HVAC_NAME, SUPLA_CHANNELFNC_HVAC_THERMOSTAT, 0xFF, 
        local_channel_type, local_channel_func);

      Supla_VirtualHvac->setInitialCaption(LOCAL_VIRTUAL_HVAC_NAME);
      Supla_VirtualHvac->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);
      
      initZ2SDeviceLocalActionHandler(
        first_free_slot, Supla_VirtualHvac->getZ2SChannel(), 
        Supla_VirtualHvac->getZ2SElementPtr());

      Supla_VirtualHvac->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_VirtualHvac->getZ2SCorePtr());
      sortChannelsSelectors();

      Supla_VirtualHvac->onLoadConfig(&SuplaDevice);
      Supla_VirtualHvac->onInit();
      
      Supla::Network::DisconnectProtocols();
    } break;


    case LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY: {

      auto Supla_LocalVirtualBinary = 
        new Supla::Sensor::LocalVirtualBinary(true); 

      Z2S_setLocalChannelData(
        Supla_LocalVirtualBinary->getZ2SCorePtr(), first_free_slot,
        Supla_LocalVirtualBinary->getChannelNumber(), NO_CUSTOM_CMD_SID, 
        LOCAL_VIRTUAL_BINARY_NAME, SUPLA_CHANNELFNC_BINARY_SENSOR, 0xFF, 
        local_channel_type, local_channel_func);

      Supla_LocalVirtualBinary->setInitialCaption(LOCAL_VIRTUAL_BINARY_NAME);
      Supla_LocalVirtualBinary->setDefaultFunction(
        SUPLA_CHANNELFNC_BINARY_SENSOR);

      initZ2SDeviceLocalActionHandler(
        first_free_slot, Supla_LocalVirtualBinary->getZ2SChannel(), 
        Supla_LocalVirtualBinary->getZ2SElementPtr());

      Supla_LocalVirtualBinary->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_LocalVirtualBinary->getZ2SCorePtr());
      sortChannelsSelectors();

      Supla_LocalVirtualBinary->onLoadConfig(&SuplaDevice);
      Supla_LocalVirtualBinary->onInit();
      
      Supla::Network::DisconnectProtocols();

    } break;


    case LOCAL_CHANNEL_TYPE_REMOTE_RELAY: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_Z2S_RemoteRelay = 
        new Supla::Control::Z2S_RemoteRelay(&Z2S_NetworkClient, 0xFF); 

      Supla_Z2S_RemoteRelay->setDefaultStateRestore();

      Z2S_setLocalChannelData(
        Supla_Z2S_RemoteRelay->getZ2SCorePtr(), first_free_slot,
        Supla_Z2S_RemoteRelay->getChannelNumber(), NO_CUSTOM_CMD_SID, 
        LOCAL_REMOTE_RELAY_NAME, SUPLA_CHANNELFNC_POWERSWITCH, 0xFF, 
        local_channel_type, local_channel_func);

      Supla_Z2S_RemoteRelay->setInitialCaption(LOCAL_REMOTE_RELAY_NAME);  
      Supla_Z2S_RemoteRelay->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);

      initZ2SDeviceLocalActionHandler(
        first_free_slot, Supla_Z2S_RemoteRelay->getZ2SChannel(), 
        Supla_Z2S_RemoteRelay->getZ2SElementPtr());

      Supla_Z2S_RemoteRelay->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_Z2S_RemoteRelay->getZ2SCorePtr());
      sortChannelsSelectors();

      Supla_Z2S_RemoteRelay->onLoadConfig(&SuplaDevice);
      Supla_Z2S_RemoteRelay->onInit();
      
      Supla::Network::DisconnectProtocols();
    } break;


    case LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER: {

      auto Supla_Z2S_RemoteThermometer = 
        new Supla::Sensor::Z2S_RemoteThermometer();

      Z2S_setLocalChannelData(
        Supla_Z2S_RemoteThermometer->getZ2SCorePtr(),first_free_slot, 
        Supla_Z2S_RemoteThermometer->getChannelNumber(), NO_CUSTOM_CMD_SID, 
        LOCAL_REMOTE_THERMOMETER_NAME, SUPLA_CHANNELFNC_THERMOMETER, 0xFF, 
        local_channel_type, local_channel_func);

      Supla_Z2S_RemoteThermometer->setInitialCaption(
        LOCAL_REMOTE_THERMOMETER_NAME);
      Supla_Z2S_RemoteThermometer->setDefaultFunction(
        SUPLA_CHANNELFNC_THERMOMETER);

      initZ2SDeviceLocalActionHandler(
        first_free_slot, Supla_Z2S_RemoteThermometer->getZ2SChannel(), 
        Supla_Z2S_RemoteThermometer->getZ2SElementPtr());

      Supla_Z2S_RemoteThermometer->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_Z2S_RemoteThermometer->getZ2SCorePtr());
      sortChannelsSelectors();

      Supla_Z2S_RemoteThermometer->onLoadConfig(&SuplaDevice);
      Supla_Z2S_RemoteThermometer->onInit();
      
      Supla::Network::DisconnectProtocols();
    } break;


    case LOCAL_CHANNEL_TYPE_SWITCHBOT: {

      SuplaDevice.saveStateToStorage();
      Supla::Storage::ConfigInstance()->commit();

      auto Supla_SwitchBotRelay = 
        new Supla::Control::SwitchBotRelay(local_channel_func); 

      Z2S_setLocalChannelData(
        Supla_SwitchBotRelay->getZ2SCorePtr(), first_free_slot,
        Supla_SwitchBotRelay->getChannelNumber(), NO_CUSTOM_CMD_SID, 
        Supla::Control::SwitchBotRelay::getSwitchBotChannelName(
          local_channel_func), SUPLA_CHANNELFNC_POWERSWITCH, 0xFF, 
        local_channel_type, local_channel_func);

      Supla_SwitchBotRelay->setInitialCaption(
        Supla_SwitchBotRelay->getZ2SChannelName());
      Supla_SwitchBotRelay->setDefaultFunction(
        SUPLA_CHANNELFNC_POWERSWITCH);

      initZ2SDeviceLocalActionHandler(
        first_free_slot, Supla_SwitchBotRelay->getZ2SChannel(), 
        Supla_SwitchBotRelay->getZ2SElementPtr());

      Supla_SwitchBotRelay->getChannel()->setFlag(
        SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

      addChannelsSelectorChannel(Supla_SwitchBotRelay->getZ2SCorePtr(), true);
      sortChannelsSelectors();

      Supla_SwitchBotRelay->onLoadConfig(&SuplaDevice);
      Supla_SwitchBotRelay->onInit();
      
      Supla::Network::DisconnectProtocols();
    } break;


    default:
      
      log_i(
        "unknown local action handler channel type 0%u", local_channel_type);
    break;
  }
  return true;
}
