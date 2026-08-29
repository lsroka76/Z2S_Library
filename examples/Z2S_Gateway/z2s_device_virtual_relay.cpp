#include "z2s_device_virtual_relay.h"
#include "TuyaDatapoints.h"

/*****************************************************************************/

void initZ2SDeviceVirtualRelay(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot) {

    initZ2SDeviceVirtualRelay(
      channel_number_slot, z2s_channels_table + channel_number_slot);
}

/*****************************************************************************/

void initZ2SDeviceVirtualRelay(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {


  if (_z2s_channel->Supla_channel_func == 
        SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER) {

    uint8_t z2s_function = Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER;
    bool z2s_rs_ignore_moving_direction = false;

    switch (_z2s_channel->model_id) {


      case Z2S_DEVICE_DESC_WINDOW_COVERING_SINGLE: {

        z2s_function = Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER;
        z2s_rs_ignore_moving_direction = true;
      } break;
      
      case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR: 
      case Z2S_DEVICE_DESC_ZEMISMART_SHADES_DRIVE_MOTOR: {
      
        z2s_function = Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR; 
      } break;
      

      case Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE: {
        
        z2s_function = Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT; 
      } break;


      case Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER: {
        
        z2s_function = Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT; 
        z2s_rs_ignore_moving_direction = true;
      } break;


      case Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER_1: {
        
        z2s_function = Z2S_ROLLER_SHUTTER_FNC_LUMI_ANALOG_MULTISTATE; 
        z2s_rs_ignore_moving_direction = true;
      } break;


      case Z2S_DEVICE_DESC_MOES_COVER: {

        z2s_function = Z2S_ROLLER_SHUTTER_FNC_MOES_COVER;
      } break;


      case Z2S_DEVICE_DESC_CURRYSMARTER_COVER: {

        z2s_function = Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER;
      } break;


      case Z2S_DEVICE_DESC_TUYA_MB60L_SMART_BLINDS_MOTOR: {

        z2s_function = Z2S_ROLLER_SHUTTER_FNC_TUYA_MB60L_COVER;
      } break;
    }
    

    auto Supla_Z2S_RollerShutter = 
      new Supla::Control::Z2S_RollerShutter(z2s_function);
  
    Supla_Z2S_RollerShutter->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

    if (strlen(_z2s_channel->Supla_channel_name) > 0) 
      Supla_Z2S_RollerShutter->setInitialCaption(
        _z2s_channel->Supla_channel_name);  
    
    Supla_Z2S_RollerShutter->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

    Supla_Z2S_RollerShutter->setKeepAliveSecs(
      _z2s_channel->keep_alive_secs);

    Supla_Z2S_RollerShutter->setTimeoutSecs(
      _z2s_channel->timeout_secs);
    
    Supla_Z2S_RollerShutter->setRSIgnoreMovingDirection(
      z2s_rs_ignore_moving_direction);

    Supla_Z2S_RollerShutter->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));
    
    Supla_Z2S_RollerShutter->setZ2SChannel(channel_index, _z2s_channel);
  } else {  //VirtualRelay section
    
    uint8_t z2s_function = Z2S_VIRTUAL_RELAY_FNC_NONE;

    switch (_z2s_channel->model_id) {


      case Z2S_DEVICE_DESC_TUYA_LCD_3_RELAYS:
      case Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER:
      case Z2S_DEVICE_DESC_TUYA_4_RELAYS_DP_CONTROLLER:
      case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:
      case Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP:
      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM10024GV3:
      case Z2S_DEVICE_DESC_TUYA_DP_RELAY:
      case Z2S_DEVICE_DESC_TUYA_TS0603_GATE_CONTROLLER:
      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_SZLR08T:
      case Z2S_DEVICE_DESC_TUYA_DUAL_WATER_VALVE: {

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_TUYA_DP_RELAY; 
      } break;


      case Z2S_DEVICE_DESC_LUMI_SMOKE_DETECTOR: {

        switch (_z2s_channel->sub_id) {


          case LUMI_SMOKE_DETECTOR_SELFTEST_SID: 
            
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_LUMI_ATTRIBUTE_BOOL;
          break;


          case LUMI_SMOKE_DETECTOR_BUZZER_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_LUMI_BUZZER_1_2;
          break;
          

          case LUMI_SMOKE_DETECTOR_LINKAGE_ALARM_SID:
          case LUMI_SMOKE_DETECTOR_HEARTBEAT_INDICATOR_SID:
            
            z2s_function = Z2S_VIRTUAL_RELAY_FNC_LUMI_ATTRIBUTE_U8;
          break;  
        } 
      } break;

      
      case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE: {


        switch (_z2s_channel->sub_id) {


          case SONOFF_SMART_VALVE_RUN_PROGRAM_SID: 
          case SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID: 

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE_PROGRAM; 
          break;
        }
      } break;


      case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE: {


        switch (_z2s_channel->sub_id) {


          case SONOFF_SMART_VALVE_RUN_PROGRAM_SID: 
          case SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID: 

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_SONOFF_DUAL_VALVE_PROGRAM; 
          break;
        }
      } break;
      

      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY: {


        switch (_z2s_channel->sub_id) {
          

          case TUYA_PRESENCE_SENSOR_RELAY_SWITCH_SID: 

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_STATE; 
          break;


          case TUYA_PRESENCE_SENSOR_RELAY_MODE_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_MODE; 
          break;


          case TUYA_PRESENCE_SENSOR_RELAY_SENSOR_STATE_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_PRESENCE_SENSOR_STATE_MODE; 
          break;
        }
      } break;


      case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM: {


        switch (_z2s_channel->sub_id) {
          

          case IAS_WD_SILENT_ALARM_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM; 
          break;


          case IAS_WD_LOUD_ALARM_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM; 
          break;
        }
      } break;


      case Z2S_DEVICE_DESC_MOES_ALARM: {


        switch (_z2s_channel->sub_id) {


          case MOES_ALARM_SWITCH_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_SWITCH; 
          break;


          case MOES_ALARM_MELODY_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_MELODY; 
          break;


          case MOES_ALARM_VOLUME_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_VOLUME; 
          break;


          case MOES_ALARM_DURATION_SID:

            z2s_function = Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_DURATION; 
          break;
        }
      } break;       
    }

    auto Supla_Z2S_VirtualRelay = 
      new Supla::Control::Z2S_VirtualRelay(z2s_function);
  
    Supla_Z2S_VirtualRelay->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

    if (strlen(_z2s_channel->Supla_channel_name) > 0) 
      Supla_Z2S_VirtualRelay->setInitialCaption(
        _z2s_channel->Supla_channel_name); 

    if (_z2s_channel->Supla_channel_func !=0) 
      Supla_Z2S_VirtualRelay->setDefaultFunction(
        _z2s_channel->Supla_channel_func);

    Supla_Z2S_VirtualRelay->setKeepAliveSecs(
        _z2s_channel->keep_alive_secs);

    Supla_Z2S_VirtualRelay->setTimeoutSecs(
        _z2s_channel->timeout_secs);

    Supla_Z2S_VirtualRelay->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

    Supla_Z2S_VirtualRelay->setZ2SChannel(channel_index, _z2s_channel);

    switch (_z2s_channel->model_id) {

      
      case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE: {


        switch (_z2s_channel->sub_id) {
          

          case SONOFF_SMART_VALVE_RUN_PROGRAM_SID:
          case SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID: {

            if (_z2s_channel->smart_valve_data.program > 0) {
              
              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8(
                _z2s_channel->smart_valve_data.program);

              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
                  _z2s_channel->smart_valve_data.cycles);
                  
              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS32(
                  _z2s_channel->smart_valve_data.value);

              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32(
                  _z2s_channel->smart_valve_data.pause_time);
              
              log_i(
                "program: %d, cycles#: %d, time/volume: %d, pause: %d",
                _z2s_channel->smart_valve_data.program,
                _z2s_channel->smart_valve_data.cycles,
                _z2s_channel->smart_valve_data.value,
                _z2s_channel->smart_valve_data.pause_time);
            }
          } break;
        }
      } break;


      case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE: {


        switch (_z2s_channel->sub_id) {
          

          case SONOFF_SMART_VALVE_RUN_PROGRAM_SID:
          case SONOFF_SMART_VALVE_RUN_PROGRAM_2_SID: {

            if (_z2s_channel->smart_valve_data.program > 0) {
              
              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8(
                _z2s_channel->smart_valve_data.program);

              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
                  _z2s_channel->smart_valve_data.cycles);
                  
              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS32(
                  _z2s_channel->smart_valve_data.value);

              Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32(
                  _z2s_channel->smart_valve_data.pause_time);
              
              log_i(
                "program: %d, cycles#: %d, time/volume: %d, pause: %d",
                _z2s_channel->smart_valve_data.program,
                _z2s_channel->smart_valve_data.cycles,
                _z2s_channel->smart_valve_data.value,
                _z2s_channel->smart_valve_data.pause_time);
            }
          } break;
        }
      } break;


      case Z2S_DEVICE_DESC_TUYA_LCD_3_RELAYS:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_LCD_PANEL_3_RELAYS_RELAY_1_DP +
          _z2s_channel->sub_id);
      break;


      case Z2S_DEVICE_DESC_TUYA_DUAL_WATER_VALVE:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_DUAL_WATER_VALVE_V1_STATE_DP +
          _z2s_channel->sub_id);
      break;


      case Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER:
      case Z2S_DEVICE_DESC_TUYA_4_RELAYS_DP_CONTROLLER:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          _z2s_channel->sub_id);
      break;


      case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_RGBWCT_LED_EF00_SWITCH_DP);
      break;


      case Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_DIN_RCBO_EM_SWITCH_DP);
      break;


      case Z2S_DEVICE_DESC_TUYA_DP_RELAY:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_DP_RELAY_STATE_DP);
      break;


      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM10024GV3:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_PRESENCE_SENSOR_ZYM10024GV3_FIND_SWITCH_DP);
      break;


      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_SZLR08T: {

        switch (_z2s_channel->sub_id) {


          case TUYA_PRESENCE_SENSOR_RELAY_SWITCH_SID:

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
              TUYA_PRESENCE_SENSOR_SZLR08T_RELAY_SWITCH_DP);
          break;


          case TUYA_PRESENCE_SENSOR_RADAR_SWITCH_SID:

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
              TUYA_PRESENCE_SENSOR_SZLR08T_RADAR_SWITCH_DP);
          break;
        };
      } break;


    case Z2S_DEVICE_DESC_TUYA_TS0603_GATE_CONTROLLER:

        Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(
          TUYA_TS0603_GATE_CONTROLER_STATE_DP);
      break;


      case Z2S_DEVICE_DESC_LUMI_SMOKE_DETECTOR:

        switch (_z2s_channel->sub_id) {


          case LUMI_SMOKE_DETECTOR_SELFTEST_SID: {

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32(
              LUMI_CUSTOM_CLUSTER_SELFTEST_ID);

            //Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(1);
            //Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8(0);
          } break;


          case LUMI_SMOKE_DETECTOR_BUZZER_SID:
          break;


          case LUMI_SMOKE_DETECTOR_HEARTBEAT_INDICATOR_SID: {

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32(
              LUMI_CUSTOM_CLUSTER_HEARTBEAT_INDICATOR_ID);

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(1);
            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8(0);
          } break;


          case LUMI_SMOKE_DETECTOR_LINKAGE_ALARM_SID: {

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32(
              LUMI_CUSTOM_CLUSTER_LINKAGE_ALARM_ID);

            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8(1);
            Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8(0);
          } break;
        }

      
      break;
    }
  }
}

/*****************************************************************************/

void addZ2SDeviceVirtualRelay(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id, const char *name, uint32_t func) {
  
  if (func == SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER) {

    auto Supla_Z2S_RollerShutter = new Supla::Control::Z2S_RollerShutter(
      Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER);

    if (name == nullptr)
      name = (char*)default_rs_name;

    Supla_Z2S_RollerShutter->setInitialCaption(name);
  
    Supla_Z2S_RollerShutter->setDefaultFunction(func);
  
    Z2S_fillChannelsTableSlot(
      device, free_slot, Supla_Z2S_RollerShutter->getChannelNumber(),
      SUPLA_CHANNELTYPE_RELAY, sub_id, name, func);

  } else {

    auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay();

    if (name == nullptr)
      name = (char*)default_relay_name;

    Supla_Z2S_VirtualRelay->setInitialCaption(name);
  
    if (func == 0)
      func = SUPLA_CHANNELFNC_POWERSWITCH;
    
    Supla_Z2S_VirtualRelay->setDefaultFunction(func);
  
    Z2S_fillChannelsTableSlot(
      device, free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), 
      SUPLA_CHANNELTYPE_RELAY, sub_id, name, func);
  }
}


/*****************************************************************************/

void msgZ2SDeviceVirtualRelay(Supla::Element* element, bool state) {
    
  auto Supla_Z2S_VirtualRelay = static_cast<
    Supla::Control::Z2S_VirtualRelay *>(element);

  Supla_Z2S_VirtualRelay->setZbDeviceLastSeenMs(millis());
    
  Supla_Z2S_VirtualRelay->Z2S_setOnOff(state);          
}

/*****************************************************************************/

void msgZ2SDeviceVirtualRelayValue(
  Supla::Element* element, uint8_t value_id, uint32_t value) {

  auto Supla_Z2S_VirtualRelay = static_cast<
    Supla::Control::Z2S_VirtualRelay *>(element);

  switch (value_id) {


    case VRV_U8_ID:

      Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU8((uint8_t)value); 
    break;


    case VRV_S8_ID:

      Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS8((int8_t)value); 
    break;


    case VRV_U32_ID:

      Supla_Z2S_VirtualRelay->Z2S_setFunctionValueU32((uint32_t)value); 
    break;


    case VRV_S32_ID:

      Supla_Z2S_VirtualRelay->Z2S_setFunctionValueS32((int32_t)value); 
    break;


    case VRV_TIMER_ID:

      Supla_Z2S_VirtualRelay->updateTimer(value);
    break;


    default:
      
      log_e("error: invalid VRV_ID"); 
    break;
  }         
}

/*****************************************************************************/

void msgZ2SDeviceRollerShutter(
  Supla::Element* element, uint8_t msg_id, uint16_t msg_value) {
    
  auto Supla_Z2S_RollerShutter = static_cast<
    Supla::Control::Z2S_RollerShutter *>(element);

  Supla_Z2S_RollerShutter->setZbDeviceLastSeenMs(millis());

  switch (msg_id) {


    case RS_CURRENT_POSITION_LIFT_PERCENTAGE_MSG:

      if (Supla_Z2S_RollerShutter->checkChannelUserDataFlags(
        USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG)) {
        
        if (Supla_Z2S_RollerShutter->getIgnoreNextMsgCounter() == 0)
          Supla_Z2S_RollerShutter->clearChannelUserDataFlags(
            USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG);
        else {

          Supla_Z2S_RollerShutter->decIgnoreNextMsgCounter();
          return;
        }
      }
      
      Supla_Z2S_RollerShutter->setRSCurrentPosition(msg_value); 
    break;


    case RS_MOVING_DIRECTION_MSG:

      Supla_Z2S_RollerShutter->setRSMovingDirection(msg_value); 
    break;
  }   
  Supla_Z2S_RollerShutter->Refresh();
}