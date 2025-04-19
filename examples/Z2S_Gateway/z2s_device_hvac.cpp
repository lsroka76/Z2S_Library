#include "z2s_device_hvac.h"

void initZ2SDeviceHvac(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {
  
  uint8_t trv_commands_set;
  int16_t hvac_room_temperature_min = 500;
  int16_t hvac_room_temperature_max = 3000;

  switch (z2s_devices_table[channel_number_slot].model_id) {

    /*case Z2S_DEVICE_DESC_TUYA_HVAC_6567C: trv_commands_set = 1; break;

    case Z2S_DEVICE_DESC_TUYA_HVAC_23457: trv_commands_set = 2; break;

    case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY: trv_commands_set = 3; break;*/
    
    case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL: {  
      
      trv_commands_set = SASWELL_CMD_SET; 

      hvac_room_temperature_min = SASWELL_CMD_SET_HEATSETPOINT_MIN;
      hvac_room_temperature_max = SASWELL_CMD_SET_HEATSETPOINT_MAX;

    } break;

    case Z2S_DEVICE_DESC_TS0601_TRV_ME167: {
      
      trv_commands_set = ME167_CMD_SET; 

      hvac_room_temperature_min = ME167_CMD_SET_HEATSETPOINT_MIN;
      hvac_room_temperature_max = ME167_CMD_SET_HEATSETPOINT_MAX;

    } break;

    case Z2S_DEVICE_DESC_TS0601_TRV_BECA: {
      
      trv_commands_set = BECA_CMD_SET; 

      hvac_room_temperature_min = BECA_CMD_SET_HEATSETPOINT_MIN;
      hvac_room_temperature_max = BECA_CMD_SET_HEATSETPOINT_MAX;

    } break;

    case Z2S_DEVICE_DESC_TS0601_TRV_MOES: {

      trv_commands_set = MOES_CMD_SET; 

      hvac_room_temperature_min = MOES_CMD_SET_HEATSETPOINT_MIN;
      hvac_room_temperature_max = MOES_CMD_SET_HEATSETPOINT_MAX;

    } break;

    case Z2S_DEVICE_DESC_TS0601_TRV_TRV601: {

      trv_commands_set = TRV601_CMD_SET; 

      hvac_room_temperature_min = TRV601_CMD_SET_HEATSETPOINT_MIN;
      hvac_room_temperature_max = TRV601_CMD_SET_HEATSETPOINT_MAX;

    } break;  
    
    default:
      trv_commands_set = 0x00; break;
  }
  
  auto Supla_Z2S_TRVInterface = new Supla::Control::Z2S_TRVInterface(gateway, device, trv_commands_set);

  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Supla_Z2S_TRVInterface);
            
  Supla_Z2S_HvacBase->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_devices_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_HvacBase->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_devices_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_HvacBase->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

  if (z2s_devices_table[channel_number_slot].Supla_secondary_channel < 0xFF) 
    Supla_Z2S_HvacBase->setMainThermometerChannelNo(z2s_devices_table[channel_number_slot].Supla_secondary_channel);

  if (z2s_devices_table[channel_number_slot].user_data_1 > 0) 
    Supla_Z2S_TRVInterface->setTemperatureCalibrationUpdateMs(z2s_devices_table[channel_number_slot].user_data_1 * 1000);

  if (z2s_devices_table[channel_number_slot].user_data_2 > 0) 
    Supla_Z2S_TRVInterface->setTemperatureCalibrationOffsetTrigger(z2s_devices_table[channel_number_slot].user_data_2 * 100);


  Supla_Z2S_HvacBase->addAction(Supla::TURN_OFF, Supla_Z2S_TRVInterface,Supla::ON_HVAC_MODE_OFF);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_OFF, Supla_Z2S_TRVInterface,Supla::ON_TURN_OFF);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_HVAC_MODE_HEAT);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_TURN_ON);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_HVAC_WEEKLY_SCHEDULE_ENABLED);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_HVAC_WEEKLY_SCHEDULE_DISABLED);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_HVAC_STANDBY);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_HVAC_HEATING);
  //Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Supla_Z2S_TRVInterface,Supla::ON_CHANGE);

  Supla_Z2S_HvacBase->allowWrapAroundTemperatureSetpoints();
  Supla_Z2S_HvacBase->setPrimaryOutputEE(Supla_Z2S_TRVInterface);
  Supla_Z2S_TRVInterface->setTRVHvac(Supla_Z2S_HvacBase);

  Supla_Z2S_HvacBase->setTemperatureRoomMin(hvac_room_temperature_min);
  Supla_Z2S_HvacBase->setTemperatureRoomMax(3000); //hvac_room_temperature_max);

  
}

void addZ2SDeviceHvac(ZigbeeGateway * gateway, zbg_device_params_t *device, uint8_t free_slot, uint8_t trv_thermometer_slot) {
  
  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE();

  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_HvacBase->getChannel()->getChannelNumber(), SUPLA_CHANNELTYPE_HVAC, -1, 
                           "THERMOSTAT", SUPLA_CHANNELFNC_HVAC_THERMOSTAT, z2s_devices_table[trv_thermometer_slot].Supla_secondary_channel); 


}

void msgZ2SDeviceHvac(int16_t channel_number_slot, uint8_t msg_id, int32_t msg_value, signed char rssi) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceHvac - invalid channel number slot");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());
  
  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if ((element == nullptr) || (element->getChannel()->getChannelType() != SUPLA_CHANNELTYPE_HVAC))
    return;
  
  auto Supla_Z2S_HvacBase = reinterpret_cast<Supla::Control::HvacBaseEE *>(element);
  auto Supla_Z2S_TRVInterface = reinterpret_cast<Supla::Control::Z2S_TRVInterface *>(Supla_Z2S_HvacBase->getPrimaryOutputEE());

  switch (msg_id) {
    case TRV_HEATING_SETPOINT_MSG: {   //degrees*100
      log_i("msgZ2SDeviceHvac - TRV_HEATING_SETPOINT_MSG: 0x%x", msg_value);
          
        Supla_Z2S_HvacBase->setTemperatureSetpointHeat(msg_value);
        Supla_Z2S_TRVInterface->setTRVTemperatureSetpoint(msg_value);
    } break;

    case TRV_SYSTEM_MODE_MSG: { //0:off, 1:on
      log_i("msgZ2SDeviceHvac - TRV_SYSTEM_MODE_MSG: 0x%x", msg_value);

      switch (msg_value) {
        case 1: Supla_Z2S_HvacBase->setTargetMode(SUPLA_HVAC_MODE_CMD_TURN_ON); break;
        case 0: Supla_Z2S_HvacBase->setTargetMode(SUPLA_HVAC_MODE_OFF, false); break;
      }
        Supla_Z2S_TRVInterface->setTRVSystemMode(msg_value);

    } break;

    case TRV_RUNNING_STATE_MSG: { //0:idle, 1:heat
      log_i("msgZ2SDeviceHvac - TRV_RUNNING_STATE_MSG: 0x%x", msg_value);
      
      Supla_Z2S_TRVInterface->setTRVRunningState(msg_value);
      Supla_Z2S_TRVInterface->setOutputValueFromRemote(msg_value*100);
    } break;
  
    case TRV_LOCAL_TEMPERATURE_MSG: { //degrees*100
      log_i("msgZ2SDeviceHvac - TRV_LOCAL_TEMPERATURE_MSG: 0x%x", msg_value);
        Supla_Z2S_TRVInterface->setTRVLocalTemperature(msg_value);
    } break;
    
    case TRV_TEMPERATURE_CALIBRATION_MSG: { //degrees*100
      log_i("msgZ2SDeviceHvac - TRV_TEMPERATURE_CALIBRATION_MSG: 0x%x", msg_value);
        Supla_Z2S_TRVInterface->setTRVTemperatureCalibration(msg_value);
    } break;
    
    case TRV_LOW_BATTERY_MSG: { //O == OK, 1 - low battery
      log_i("msgZ2SDeviceHvac - TRV_LOW_BATTERY_MSG: 0x%x", msg_value);
    } break;

    case TRV_BATTERY_LEVEL_MSG: { 
      log_i("msgZ2SDeviceHvac - TRV_BATTERY_LEVEL_MSG: 0x%x", msg_value);
    } break;
  
    case TRV_CHILD_LOCK_MSG: { 
      log_i("msgZ2SDeviceHvac - TRV_CHILD_LOCK_MSG: 0x%x", msg_value);
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

    default: log_i("msgZ2SDeviceHvac - unknown message id: 0x%x, value 0x%x", msg_id, msg_value); break;
  }
}

