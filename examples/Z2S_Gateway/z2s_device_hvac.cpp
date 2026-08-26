#include "z2s_device_hvac.h"

/*****************************************************************************/

uint8_t getZ2SDeviceHvacCmdSet(z2s_device_params_t* _z2s_channel) {

  switch (_z2s_channel->model_id) {


    case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL: {  
      
      return saswell_cmd_set; 
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_ME167: {

      if ((strcmp(Z2S_getZbDeviceManufacturerName(
            _z2s_channel->Zb_device_id), "_TZE200_9xfjixap") == 0) || 
          (strcmp(Z2S_getZbDeviceManufacturerName(
            _z2s_channel->Zb_device_id), "_TZE200_hvaxb2tc") == 0) || 
          (strcmp(Z2S_getZbDeviceManufacturerName(
            _z2s_channel->Zb_device_id), "_TZE200_rxq4iti9") == 0))
        return me167_no_pi_cmd_set;
      else
        return me167_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_BECA: {
      
      return beca_cmd_set; 
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_MOES: {

      return moes_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_TRV601: {

      return trv601_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_TRV603: {

      return trv603_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_GTZ10: {

      return gtz10_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z: {

      return trv602z_cmd_set;
    } break;  


    case Z2S_DEVICE_DESC_TS0601_TRV_TV02: {

      return tv02_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_SITERWELL: {

      return siterwell_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_TRV_TRV16: {

      return trv16_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198: {

      return zwt198_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100: {

      return zwt100_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_MOES_BHT002: {

      return bht002_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_MOES_ZHTSR: {

      return zhtsr_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_TS0601_BOTR9V: {

      return botr9v_cmd_set;
    } break;


    case Z2S_DEVICE_DESC_SONOFF_TRVZB: {

      return TRVZB_CMD_SET;
    } break;


    case Z2S_DEVICE_DESC_BOSCH_BTHRA: {

      return BOSCH_CMD_SET;
    } break;


    case Z2S_DEVICE_DESC_EUROTRONIC_SPZB0001: {

      return EUROTRONIC_CMD_SET;
    } break;


    case Z2S_DEVICE_DESC_LUMI_TRV: {

      return LUMI_CMD_SET;
    } break;


    case Z2S_DEVICE_DESC_DANFOSS_ETRV0103: {

      return DANFOSS_CMD_SET;
    } break;


    default:
      return 0xFF; break;
  }  
}
/*****************************************************************************/

uint8_t getZ2SDeviceHvacCmdSet(Supla::Element* element) {

  auto Supla_Z2S_TRVInterface = static_cast<
    Supla::Control::Z2S_TRVInterface *>(element);

  return Supla_Z2S_TRVInterface->getTRVCommandsSet();
}

/*****************************************************************************/

void initZ2SDeviceHvac(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot) {

  initZ2SDeviceHvac(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

/*****************************************************************************/

void initZ2SDeviceHvac(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {

  uint8_t trv_commands_set;
  uint8_t trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_IGNORE;
  int16_t hvac_room_temperature_min = 500;
  int16_t hvac_room_temperature_max = 3000;
  bool onOffOnly = true;
  
  trv_commands_set = getZ2SDeviceHvacCmdSet(_z2s_channel);

  if ((trv_commands_set >= saswell_cmd_set) && 
      (trv_commands_set < ts0601_cmd_sets_number)) {

    if (ts0601_command_sets_table[trv_commands_set].\
        ts0601_cmd_set_id == trv_commands_set) {

      trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE; 
      
      hvac_room_temperature_min = ts0601_command_sets_table[trv_commands_set].\
        ts0601_cmd_set_target_heatsetpoint_min;
      
      hvac_room_temperature_max = ts0601_command_sets_table[trv_commands_set].\
        ts0601_cmd_set_target_heatsetpoint_max;
    } else {

      log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
            ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_id,
            trv_commands_set);
      return;
    }
  } else {

    switch (trv_commands_set) {


      case TRVZB_CMD_SET: {

        trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT; 
        hvac_room_temperature_min = TRVZB_CMD_SET_HEATSETPOINT_MIN;
        hvac_room_temperature_max = TRVZB_CMD_SET_HEATSETPOINT_MAX;
      } break;


      case BOSCH_CMD_SET: {

        trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT; 
        hvac_room_temperature_min = BOSCH_CMD_SET_HEATSETPOINT_MIN;
        hvac_room_temperature_max = BOSCH_CMD_SET_HEATSETPOINT_MAX;
        onOffOnly = false;
      } break;


      case EUROTRONIC_CMD_SET: {

        trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE; 
        hvac_room_temperature_min = EUROTRONIC_CMD_SET_HEATSETPOINT_MIN;
        hvac_room_temperature_max = EUROTRONIC_CMD_SET_HEATSETPOINT_MAX;
        onOffOnly = false;
      } break;


      case LUMI_CMD_SET: {

        trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT; 
        hvac_room_temperature_min = LUMI_CMD_SET_HEATSETPOINT_MIN;
        hvac_room_temperature_max = LUMI_CMD_SET_HEATSETPOINT_MAX;
        onOffOnly = true;
      } break;


      case DANFOSS_CMD_SET: {

        trv_external_sensor_mode = EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT; 
        hvac_room_temperature_min = DANFOSS_CMD_SET_HEATSETPOINT_MIN;
        hvac_room_temperature_max = DANFOSS_CMD_SET_HEATSETPOINT_MAX;
        onOffOnly = false;
      } break;
    }
  }

  if ((trv_commands_set == moes_cmd_set) ||
      (trv_commands_set == me167_cmd_set) ||
      (trv_commands_set == trv602z_cmd_set))
    onOffOnly = false;

  auto Supla_Z2S_TRVInterface = new Supla::Control::Z2S_TRVInterface(
    trv_commands_set, onOffOnly);

  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(
    Supla_Z2S_TRVInterface);
            
  Supla_Z2S_HvacBase->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

  Supla_Z2S_HvacBase->setMainThermometerChannelNo(
    _z2s_channel->Supla_secondary_channel);

  Supla_Z2S_HvacBase->setBinarySensorChannelNo(_z2s_channel->Supla_channel);

  if (strlen(_z2s_channel->Supla_channel_name) > 0) 
    Supla_Z2S_HvacBase->setInitialCaption(_z2s_channel->Supla_channel_name);
  
  if (_z2s_channel->Supla_channel_func !=0) 
    Supla_Z2S_HvacBase->setDefaultFunction(_z2s_channel->Supla_channel_func);

  Supla_Z2S_TRVInterface->setTimeoutSecs(_z2s_channel->timeout_secs);
  Supla_Z2S_TRVInterface->setKeepAliveSecs(_z2s_channel->keep_alive_secs);

  Supla_Z2S_HvacBase->allowWrapAroundTemperatureSetpoints();
  Supla_Z2S_HvacBase->setPrimaryOutputEE(Supla_Z2S_TRVInterface);
  Supla_Z2S_TRVInterface->setTRVHvac(Supla_Z2S_HvacBase);

  Supla_Z2S_HvacBase->setDefaultTemperatureRoomMin(
    SUPLA_CHANNELFNC_HVAC_THERMOSTAT, hvac_room_temperature_min);

  Supla_Z2S_HvacBase->setDefaultTemperatureRoomMax(
    SUPLA_CHANNELFNC_HVAC_THERMOSTAT, hvac_room_temperature_max);

  Supla_Z2S_HvacBase->setButtonTemperatureStep(50);
  Supla_Z2S_HvacBase->addLocalUILockCapability(Supla::LocalUILock::Full);
  
  if (_z2s_channel->user_data_flags & USER_DATA_FLAG_TRV_FIXED_CORRECTION) {

    Supla_Z2S_TRVInterface->enableExternalSensorDetection(
      false, EXTERNAL_TEMPERATURE_SENSOR_USE_FIXED, 
      _z2s_channel->Supla_secondary_channel);  

    Supla_Z2S_TRVInterface->setFixedTemperatureCalibration(
      _z2s_channel->hvac_fixed_temperature_correction);
  } else {

    Supla_Z2S_TRVInterface->enableExternalSensorDetection(
      true, trv_external_sensor_mode, _z2s_channel->Supla_secondary_channel); 
  }

  if (_z2s_channel->user_data_flags & USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK)
    Supla_Z2S_TRVInterface->setCooperativeChildLock(true);

  _z2s_channel->user_data_flags &= ~USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG;

  _z2s_channel->user_data_2 = 0;

  Supla_Z2S_TRVInterface->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

  Supla_Z2S_TRVInterface->setZ2SChannel(channel_index, _z2s_channel);
}

/*****************************************************************************/

void addZ2SDeviceHvac(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  uint8_t trv_thermometer_slot) {
  
  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE();

  Supla_Z2S_HvacBase->setMainThermometerChannelNo(
    z2s_channels_table[trv_thermometer_slot].Supla_channel);
    
  Supla_Z2S_HvacBase->setBinarySensorChannelNo(
    Supla_Z2S_HvacBase->getChannel()->getChannelNumber());

  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_HvacBase->getChannel()->getChannelNumber(), 
    SUPLA_CHANNELTYPE_HVAC, -1, "THERMOSTAT", SUPLA_CHANNELFNC_HVAC_THERMOSTAT, 
    z2s_channels_table[trv_thermometer_slot].Supla_channel); 
}

/*****************************************************************************/

void msgZ2SDeviceHvac(
  Supla::Element* element, uint8_t msg_id, int32_t msg_value) {

  auto Supla_Z2S_TRVInterface = static_cast<
    Supla::Control::Z2S_TRVInterface *>(element);

  auto Supla_Z2S_HvacBase = Supla_Z2S_TRVInterface->getTRVHvac();
  
  Supla_Z2S_TRVInterface->setZbDeviceLastSeenMs(millis());

  if (Supla_Z2S_HvacBase->isHvacFlagForcedOffBySensor()) {

    log_i(
      "HvacFlagForcedOffBySensor set - ignoring message id: 0x%x, value 0x%x",
       msg_id, msg_value);
       
    if (msg_id == TRV_SYSTEM_MODE_MSG) 
      Supla_Z2S_TRVInterface->setTRVSystemMode(msg_value);

    if (msg_id == TRV_HEATING_SETPOINT_MSG) 
      Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);  

    if (msg_id == TRV_RUNNING_STATE_MSG) {

      Supla_Z2S_TRVInterface->setTRVRunningState(msg_value);
      
      if (Supla_Z2S_TRVInterface->isOnOffOnly())
        Supla_Z2S_TRVInterface->setOutputValueFromRemote(msg_value * 100);
      else 
        Supla_Z2S_TRVInterface->setOutputValueFromRemote(msg_value);
    }
    
    return; //? 
  }

  switch (msg_id) {
    
    
    case TRV_HEATING_SETPOINT_MSG: {   //degrees*100

      if (Supla_Z2S_TRVInterface->inInitSequence()) {

        log_i("TRV_HEATING_SETPOINT_MSG(inInitSequence) %04d", msg_value);

        Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);  

        return;      
      }

      if (Supla_Z2S_TRVInterface->inScheduleMode()) {

        log_i("TRV_HEATING_SETPOINT_MSG(inScheduleMode) %04d", msg_value);

        Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);  

        return;      
      }

      if (Supla_Z2S_TRVInterface->isHvacWindowOpened()) {

        log_i("TRV_HEATING_SETPOINT_MSG(HvacWindowOpened) %04d", msg_value);

        Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);
        return;
      }

      if (Supla_Z2S_HvacBase->isThermostatDisabled()) {

        log_i("TRV_HEATING_SETPOINT_MSG(HVAC_OFF_MODE) %04d", msg_value);

        Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);
        return;
      }

      log_i("msgZ2SDeviceHvac - TRV_HEATING_SETPOINT_MSG: %04d", msg_value);
      log_i(
        "HVAC flags: 0x%x", Supla_Z2S_HvacBase->getChannel()->getHvacFlags());

      if (Supla_Z2S_TRVInterface->checkChannelUserDataFlags(
            USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG)) {

        log_i(
          "Flag set - ignoring TRV_HEATING_SETPOINT_MSG: %04d\n\r"
            "Supla_Z2S_HvacBase->getTemperatureSetpointHeat %04d", 
            msg_value, Supla_Z2S_HvacBase->getTemperatureSetpointHeat());
        
        if (Supla_Z2S_TRVInterface->getChannelUserData2() == 0)
          Supla_Z2S_TRVInterface->clearChannelUserDataFlags(
            USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG);

        Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);
        break;
      }
      Supla_Z2S_TRVInterface->setHvacTemperatureSetpoint(msg_value);
    } break;


    //0:off, 1:on
    case TRV_SYSTEM_MODE_MSG: { 

      log_i(
        "msgZ2SDeviceHvac - TRV_SYSTEM_MODE_MSG: 0x%x", msg_value);

      log_i(
        "HVAC flags: 0x%x", Supla_Z2S_HvacBase->getChannel()->getHvacFlags());

      switch (msg_value) {


        case 1: 
        
          if (Supla_Z2S_HvacBase->isThermostatDisabled()) 
            Supla_Z2S_HvacBase->setTargetMode(SUPLA_HVAC_MODE_CMD_TURN_ON);

          if (Supla_Z2S_TRVInterface->checkChannelUserDataFlags(
                USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG))
            Supla_Z2S_TRVInterface->setChannelUserData2(0);  
        break;


        case 0: 
        
          if (!Supla_Z2S_HvacBase->isThermostatDisabled())
            Supla_Z2S_HvacBase->setTargetMode(SUPLA_HVAC_MODE_OFF, false); 
          
          Supla_Z2S_TRVInterface->setOutputValueFromRemote(0);
          
          Supla_Z2S_TRVInterface->clearChannelUserDataFlags(
            USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG);
          Supla_Z2S_TRVInterface->setChannelUserData2(0);
        break;
      }
      
      Supla_Z2S_TRVInterface->setTRVSystemMode(msg_value);

    } break;


    //0:off, 1:on
    case TRV_SCHEDULE_MODE_ALT_MSG:
    case TRV_SCHEDULE_MODE_MSG: { 
    

      log_i(
        "msgZ2SDeviceHvac - TRV_SCHEDULE_MODE_MSG: 0x%x::0x%x", msg_id, 
        msg_value);

      switch (msg_value) {
   
        
        case 0: {

          if (Supla_Z2S_TRVInterface->checkChannelUserDataFlags(
                USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG))
            Supla_Z2S_TRVInterface->setChannelUserData2(0);  
        } break;
        
        
        //case 1:
        case 2:
        case 3:
        case 4: {

          if (Supla_Z2S_TRVInterface->checkChannelUserDataFlags(
                USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE)) {
            
            if (Supla_Z2S_TRVInterface->checkChannelUserDataFlags(
                USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL)) {
          
              log_i(
                "USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE & "
                "USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL");

              Supla_Z2S_HvacBase->handleAction(
                0, Supla::TOGGLE_MANUAL_WEEKLY_SCHEDULE_MODES);
            } 
            else {
              
              log_i("USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE");
              
              Supla_Z2S_HvacBase->setTargetMode(
                SUPLA_HVAC_MODE_CMD_WEEKLY_SCHEDULE, true); 
            }              
            Supla_Z2S_TRVInterface->turnOffTRVScheduleMode();
          } 
          else {

            Supla_Z2S_HvacBase->setTargetMode(SUPLA_HVAC_MODE_OFF, false); 
            Supla_Z2S_TRVInterface->setTRVSystemMode(0);
          }
        } break;


        case 1:
        case 5:
          Supla_Z2S_TRVInterface->turnOffTRVScheduleMode();
        break;
      }        
    } break;


    //0:idle, 1:heat
    case TRV_RUNNING_STATE_MSG: { 

      log_i("msgZ2SDeviceHvac - TRV_RUNNING_STATE_MSG: %02u", msg_value);
      
      Supla_Z2S_TRVInterface->setTRVRunningState(msg_value);
      
      if (Supla_Z2S_TRVInterface->isOnOffOnly())
        Supla_Z2S_TRVInterface->setOutputValueFromRemote(msg_value * 100);
      else 
        Supla_Z2S_TRVInterface->setOutputValueFromRemote(msg_value);
    } break;

    //degrees*100
    case TRV_LOCAL_TEMPERATURE_MSG: { 

      log_i(
        "msgZ2SDeviceHvac - TRV_LOCAL_TEMPERATURE_MSG: %04d", msg_value);
      
      Supla_Z2S_TRVInterface->setTRVLocalTemperature(msg_value);
    } break;


    //degrees*100
    case TRV_TEMPERATURE_CALIBRATION_MSG: { 

      log_i(
        "msgZ2SDeviceHvac - TRV_TEMPERATURE_CALIBRATION_MSG: %04d", msg_value);

      Supla_Z2S_TRVInterface->setTRVTemperatureCalibration(msg_value);

      
      if (Supla_Z2S_TRVInterface->checkChannelUserDataFlags(
            USER_DATA_FLAG_TRV_FIXED_CORRECTION)) {
          
        updateHvacFixedCalibrationTemperature(
          Supla_Z2S_TRVInterface->getZ2SChannelIndex(), msg_value, false);

        Supla_Z2S_TRVInterface->setFixedTemperatureCalibration(msg_value);
      }  
    } break; 


    //O == OK, 1 - low battery
    case TRV_LOW_BATTERY_MSG: { 

      log_i("msgZ2SDeviceHvac - TRV_LOW_BATTERY_MSG: 0x%x", msg_value);
    } break;


    case TRV_BATTERY_LEVEL_MSG: { 

      log_i("msgZ2SDeviceHvac - TRV_BATTERY_LEVEL_MSG: %02u", msg_value);
    } break;


    case TRV_CHILD_LOCK_MSG: { 

      log_i("msgZ2SDeviceHvac - TRV_CHILD_LOCK_MSG: 0x%x", msg_value);

      Supla_Z2S_TRVInterface->setTRVChildLock(msg_value);
    } break;


    case TRV_WINDOW_DETECT_MSG: { 

      log_i("msgZ2SDeviceHvac - WINDOW_DETECT_MSG: 0x%x", msg_value);
    } break;


    case TRV_ANTI_FREEZE_MSG: { 

      log_i("msgZ2SDeviceHvac - ANTI_FREEZE_MSG: 0x%x", msg_value);
    } break;


    case TRV_LIMESCALE_PROTECT_MSG: { 
      
      log_i("msgZ2SDeviceHvac - LIMESCALE_PROTECT_MSG: 0x%x", msg_value);
    } break;


    case TRV_TEMPERATURE_HISTERESIS_MSG: { 
      
      log_i(
        "msgZ2SDeviceHvac - TRV_TEMPERATURE_HISTERESIS_MSG: %04d", msg_value);

      Supla_Z2S_TRVInterface->setTRVTemperatureHisteresis(msg_value);
    } break;


    case TRV_SENSOR_TYPE_MSG: {

      log_i(
        "msgZ2SDeviceHvac - TRV_SENSOR_TYPE_MSG: 0x%x", msg_value);

      Supla_Z2S_TRVInterface->setTRVTemperatureSensorType(msg_value);
    } break;


    default: 

      log_i("unknown message id: 0x%x, value 0x%x", msg_id, msg_value); 
    break;
  }
}
