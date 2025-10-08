#include <time.h>

#include "z2s_device_tuya_custom_cluster.h"
#include "z2s_device_dimmer.h"
#include "z2s_device_iaszone.h"
#include "z2s_device_temphumidity.h"
#include "z2s_device_general_purpose_measurement.h"
#include "z2s_device_action_trigger.h"
#include "z2s_device_hvac.h"
#include "z2s_device_electricity_meter.h"
#include "z2s_device_virtual_relay.h"
#include "z2s_device_virtual_valve.h"
#include "TuyaDatapoints.h"

#include <arduino_base64.hpp>
#include <math.h>

extern ZigbeeGateway zbGateway;

/*uint8_t learned_ir_code[256];
char    learned_ir_code_base64[256];
uint8_t ir_code_send_buffer[128];
uint8_t ir_code_to_send[512];*/

uint8_t learned_ir_code[1];
char    learned_ir_code_base64[1];
uint8_t ir_code_send_buffer[1];
uint8_t ir_code_to_send[1];


bool ir_code_learning = false;
bool ir_code_receiving = false;
uint32_t ir_message_position = 0;
uint32_t ir_message_last_position = 0;
uint32_t ir_message_chunk_last_position = 0xFFFFFFFF;
uint32_t ir_message_chunk_last_size = 0xFFFFFFFF;
uint32_t ir_message_length = 0;
uint16_t ir_message_seq = 0;

uint16_t learned_ir_code_base64_length = 0;

uint32_t ir_code_send_position = 0;
uint32_t ir_code_send_length  = 0;
uint16_t  ir_code_send_seq = 0;

/*{'key_num': 1,'delay': 300,'key1': {'num': 1,'freq': 38000,'type': 1,'key_code': */
const static uint8_t IR_SEND_CODE_PREAMBLE[] PROGMEM = {0x7B,0x27,0x6B,0x65,0x79,0x5F,0x6E,0x75,0x6D,0x27,0x3A,0x20,0x31,0x2C,0x27,0x64,0x65,
                                              0x6C,0x61,0x79,0x27,0x3A,0x20,0x33,0x30,0x30,0x2C,0x27,0x6B,0x65,0x79,0x31,0x27,0x3A,
                                              0x20,0x7B,0x27,0x6E,0x75,0x6D,0x27,0x3A,0x20,0x31,0x2C,0x27,0x66,0x72,0x65,0x71,0x27,
                                              0x3A,0x20,0x33,0x38,0x30,0x30,0x30,0x2C,0x27,0x74,0x79,0x70,0x65,0x27,0x3A,0x20,0x31,
                                              0x2C,0x27,0x6B,0x65,0x79,0x5F,0x63,0x6F,0x64,0x65,0x27,0x3A,0x20};

/*,},}*/
const static uint8_t IR_SEND_CODE_POSTAMBLE[] = {0x2C,0x7D,0x2C,0x7D};

/*testcode01*/
const static uint8_t TEST_CODE_01[] = {0x74,0x65,0x73,0x74,0x63,0x6F,0x64,0x65,0x30,0x31};

//void updateSuplaBatteryLevel(int16_t channel_number_slot, uint32_t value);

Tuya_read_dp_result_t Z2S_readTuyaDPvalue(uint8_t Tuya_dp_id, uint16_t payload_size, uint8_t *payload) {
  
  uint16_t payload_counter = 2;
  
  //Tuya_read_dp_result_t Tuya_read_dp_result = {};
  Tuya_read_dp_result_t Tuya_read_dp_result_test = {};

  Tuya_dp_data_t *Tuya_dp_data = nullptr;

  while ((payload_size >= 7) && (payload_counter < payload_size)) {

    Tuya_dp_data = (Tuya_dp_data_t *)(payload + payload_counter);
    
    /*log_i("dp %02x, type %02x, size %04x", 
          Tuya_dp_data->dp_id, 
          Tuya_dp_data->dp_type, 
          __builtin_bswap16(Tuya_dp_data->dp_size_16_big_endian));*/

    if (Tuya_dp_data->dp_id == Tuya_dp_id) {

      Tuya_read_dp_result_test.dp_id   = Tuya_dp_data->dp_id;
      Tuya_read_dp_result_test.dp_type = Tuya_dp_data->dp_type;
      Tuya_read_dp_result_test.dp_size = __builtin_bswap16(Tuya_dp_data->dp_size_16_big_endian);

      if (Tuya_read_dp_result_test.dp_type != 0) {

        switch (Tuya_read_dp_result_test.dp_size) {

          case 1: Tuya_read_dp_result_test.dp_value = Tuya_dp_data->dp_value_8; break;

          case 2: Tuya_read_dp_result_test.dp_value = __builtin_bswap16(Tuya_dp_data->dp_value_16_big_endian); break;

          case 4: Tuya_read_dp_result_test.dp_value = __builtin_bswap32(Tuya_dp_data->dp_value_32_big_endian); break;
          
          default: {
            
              log_e("unrecognized Tuya DP size 0x%x", Tuya_read_dp_result_test.dp_size); 
            return Tuya_read_dp_result_test;
          }
        }
      } else {

        if (Tuya_read_dp_result_test.dp_size > 8) {

          log_e("Tuya RAW DP size > 8 (0x%x)", Tuya_read_dp_result_test.dp_size); 
          return Tuya_read_dp_result_test;
        }

        memcpy(Tuya_read_dp_result_test.dp_raw_value_8, &Tuya_dp_data->dp_value_8, Tuya_read_dp_result_test.dp_size);
      }
      Tuya_read_dp_result_test.is_success = true;
      return Tuya_read_dp_result_test;
    }
    else Tuya_read_dp_result_test.dp_size = __builtin_bswap16(Tuya_dp_data->dp_size_16_big_endian);
    payload_counter += 1 /*DP ID*/ + 1 /*DP TYPE*/ + 2/*DP DATA SIZE*/ + Tuya_read_dp_result_test.dp_size;

    /*if ((*(payload + payload_counter)) == Tuya_dp_id) {
      Tuya_read_dp_result.dp_id   = (*(payload + payload_counter));
      Tuya_read_dp_result.dp_type = (*(payload + payload_counter + 1));
      Tuya_read_dp_result.dp_size = ((uint16_t)(*(payload + payload_counter + 2))) * 0x100 + (*(payload + payload_counter + 3));
      
      if (Tuya_read_dp_result.dp_type != 0) {
        switch (Tuya_read_dp_result.dp_size) {
          case 1: Tuya_read_dp_result.dp_value = (*(payload + payload_counter + 4)); break;
          case 2: Tuya_read_dp_result.dp_value = ((uint32_t)(*(payload + payload_counter + 4))) * 0x00000100 +
                                                 ((uint32_t)(*(payload + payload_counter + 5))); break;
          case 4:  Tuya_read_dp_result.dp_value =  ((uint32_t)(*(payload + payload_counter + 4))) * 0x01000000 +
                                                   ((uint32_t)(*(payload + payload_counter + 5))) * 0x00010000 +
                                                   ((uint32_t)(*(payload + payload_counter + 6))) * 0x00000100 +
                                                   ((uint32_t)(*(payload + payload_counter + 7))); break;
          default: {
            log_e("unrecognized Tuya DP size 0x%x", Tuya_read_dp_result.dp_size); 
            return Tuya_read_dp_result;
          }
        }
      } else
      {
        if (Tuya_read_dp_result.dp_size > 8) {
          log_e("Tuya RAW DP size > 8 (0x%x)", Tuya_read_dp_result.dp_size); 
          return Tuya_read_dp_result;
        }
        memcpy(Tuya_read_dp_result.dp_raw_value_8, payload + payload_counter + 4, Tuya_read_dp_result.dp_size);
      }
      Tuya_read_dp_result.is_success = true;
      if ((Tuya_read_dp_result.is_success == Tuya_read_dp_result_test.is_success) &&
          (Tuya_read_dp_result.dp_id == Tuya_read_dp_result_test.dp_id) &&
          (Tuya_read_dp_result.dp_type == Tuya_read_dp_result_test.dp_type) &&
          (Tuya_read_dp_result.dp_size == Tuya_read_dp_result_test.dp_size) &&
          (Tuya_read_dp_result.dp_value == Tuya_read_dp_result_test.dp_value))
        log_i("DP COMPARE SUCCESS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

      log_i("Tuya_read_dp_result.is_success %02x -- Tuya_read_dp_result_test.is_success %02x",
            Tuya_read_dp_result.is_success, Tuya_read_dp_result_test.is_success);
      log_i("Tuya_read_dp_result.dp_id %02x -- Tuya_read_dp_result_test.dp_id %02x",
            Tuya_read_dp_result.dp_id, Tuya_read_dp_result_test.dp_id);
      log_i("Tuya_read_dp_result.dp_type %02x -- Tuya_read_dp_result_test.dp_type %02x",
            Tuya_read_dp_result.dp_type, Tuya_read_dp_result_test.dp_type);
      log_i("Tuya_read_dp_result.dp_size %04x -- Tuya_read_dp_result_test.dp_size %04x",
            Tuya_read_dp_result.dp_size, Tuya_read_dp_result_test.dp_size);
      log_i("Tuya_read_dp_result.dp_value %08x -- Tuya_read_dp_result_test.dp_value %08x",
            Tuya_read_dp_result.dp_value, Tuya_read_dp_result_test.dp_value);
      return Tuya_read_dp_result;
    }
    else Tuya_read_dp_result.dp_size = ((uint16_t)(*(payload + payload_counter + 2))) * 0x100 + (*(payload + payload_counter + 3));*/
    //payload_counter += 1 /*DP ID*/ + 1 /*DP TYPE*/ + 2/*DP DATA SIZE*/ + Tuya_read_dp_result.dp_size;
  }
  return Tuya_read_dp_result_test;
}

// HVAC data reporting                         //

void processTuyaHvacDataReport(int16_t channel_number_slot, uint16_t payload_size, uint8_t *payload, uint32_t model_id) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  uint8_t local_temperature_dp_id        = 0x00;
  uint8_t current_heating_setpoint_dp_id = 0x00;

  uint8_t system_mode_on_dp_id           = 0x00;
  uint8_t system_mode_off_dp_id          = 0x00;

  uint8_t running_state_dp_id            = 0x00;
  uint8_t running_state_value_idle       = 0xFF;
  uint8_t running_state_value_heat       = 0xFF;

  uint8_t temperature_calibration_dp_id  = 0x00;

  uint8_t low_battery_dp_id              = 0x00;
  uint8_t battery_level_dp_id            = 0x00;

  uint8_t schedule_mode_dp_id            = 0x00;

  uint8_t child_lock_dp_id               = 0x00;
  uint8_t window_detect_dp_id            = 0x00;
  uint8_t anti_freeze_dp_id              = 0x00;
  uint8_t limescale_protect_dp_id        = 0x00;

  uint8_t system_mode_value_on           = 0xFF;
  uint8_t system_mode_value_off          = 0xFF;
  
  uint8_t schedule_mode_value_on         = 0xFF;
  uint8_t schedule_mode_value_off        = 0xFF;

  int32_t local_temperature_factor       = 1;
  int32_t target_heatsetpoint_factor     = 1;
  int32_t temperature_calibration_factor = 1;

  int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, //legacy compatibility
                                                            z2s_channels_table[channel_number_slot].endpoint, 
                                                            z2s_channels_table[channel_number_slot].cluster_id, 
                                                            SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, 
                                                            NO_CUSTOM_CMD_SID);
                                                            
  if (channel_number_slot_1 < 0)
    channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_THERMOMETER, 
                                                      NO_CUSTOM_CMD_SID);

  int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                            z2s_channels_table[channel_number_slot].endpoint, 
                                                            z2s_channels_table[channel_number_slot].cluster_id, 
                                                            SUPLA_CHANNELTYPE_HVAC, 
                                                            NO_CUSTOM_CMD_SID);

  uint8_t trv_commands_set = getZ2SDeviceHvacCmdSet(model_id);

  if ((trv_commands_set >= saswell_cmd_set) && (trv_commands_set < ts0601_cmd_sets_number)) {

    if (ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_id == trv_commands_set) {

      local_temperature_dp_id        =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_local_temperature_dp_id;
      current_heating_setpoint_dp_id =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_target_heatsetpoint_dp_id;

      system_mode_on_dp_id           =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_on_dp_id;
      system_mode_off_dp_id          =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_off_dp_id;
      system_mode_value_on           =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_on_dp_value_on;
      system_mode_value_off          =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_off_dp_value_off;
      
      running_state_dp_id            =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_running_state_dp_id;
      running_state_value_idle       =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_running_state_dp_value_idle;
      running_state_value_heat       =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_running_state_dp_value_heat;

      temperature_calibration_dp_id  =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_temperature_calibration_dp_id;

      low_battery_dp_id              =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_low_battery_dp_id;
      battery_level_dp_id            =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_battery_level_dp_id;

      schedule_mode_dp_id            =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_schedule_mode_dp_id;
      schedule_mode_value_on         =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_schedule_mode_dp_value_on;
      schedule_mode_value_off        =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_schedule_mode_dp_value_off;

      child_lock_dp_id               =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_child_lock_dp_id;
      window_detect_dp_id            =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_winodow_detect_dp_id;
      anti_freeze_dp_id              =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_anti_freeze_protect_dp_id;
      limescale_protect_dp_id        =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_limescale_protect_dp_id;

      local_temperature_factor       =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_local_temperature_factor;
      target_heatsetpoint_factor     =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_target_heatsetpoint_factor;
      temperature_calibration_factor =  
        ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_temperature_calibration_factor;
    } else
      log_e("ts0601_command_sets_table internal mismatch! %02x <> %02x", 
            ts0601_command_sets_table[trv_commands_set].ts0601_cmd_set_id,
            trv_commands_set);
  } else
    log_e("unsupported ts0601 commands set %02x", trv_commands_set);

  if (local_temperature_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(local_temperature_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
      
      msgZ2SDeviceTempHumidityTemp(channel_number_slot_1, 
                                   (float)Tuya_read_dp_result.dp_value /
                                   local_temperature_factor);
                                   
      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_LOCAL_TEMPERATURE_MSG, 
                       (Tuya_read_dp_result.dp_value * 100) / 
                       local_temperature_factor);
    }
  }

  if (current_heating_setpoint_dp_id) {
    
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(current_heating_setpoint_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_HEATING_SETPOINT_MSG, 
                       (Tuya_read_dp_result.dp_value * 100) / 
                       target_heatsetpoint_factor);
    }
  }

  if (system_mode_on_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(system_mode_on_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
      
      if (Tuya_read_dp_result.dp_value == system_mode_value_on)
        msgZ2SDeviceHvac(channel_number_slot_2, 
                         TRV_SYSTEM_MODE_MSG, 
                         1);
    }
  }

  if (system_mode_off_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(system_mode_off_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
     
      if (Tuya_read_dp_result.dp_value == system_mode_value_off)
        msgZ2SDeviceHvac(channel_number_slot_2, 
                         TRV_SYSTEM_MODE_MSG, 
                         0);
    }
  }

  if (schedule_mode_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(schedule_mode_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
      
      if (Tuya_read_dp_result.dp_value == schedule_mode_value_on)
        msgZ2SDeviceHvac(channel_number_slot_2, 
                         TRV_SCHEDULE_MODE_MSG, 
                         1);
    }
  } 

  if (schedule_mode_dp_id && (schedule_mode_value_off < 0xFF)) {
    
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(schedule_mode_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
      
      if (Tuya_read_dp_result.dp_value == schedule_mode_value_off)
        msgZ2SDeviceHvac(channel_number_slot_2, 
                         TRV_SCHEDULE_MODE_MSG, 
                         0);
    }
  }

  if (running_state_dp_id) {
    
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(running_state_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {
      
      if (Tuya_read_dp_result.dp_value == running_state_value_idle)
        msgZ2SDeviceHvac(channel_number_slot_2, 
                        TRV_RUNNING_STATE_MSG, 
                        0);
      else
        msgZ2SDeviceHvac(channel_number_slot_2, 
                         TRV_RUNNING_STATE_MSG, 
                         1);
    }
  }

  if (temperature_calibration_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(temperature_calibration_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_TEMPERATURE_CALIBRATION_MSG, 
                       (Tuya_read_dp_result.dp_value * 100) / 
                       temperature_calibration_factor);
    }
  }
  
  if (low_battery_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(low_battery_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      uint8_t battery_level = (Tuya_read_dp_result.dp_value == 0) ? 100 : 0; 
    
      updateSuplaBatteryLevel(channel_number_slot_1, 
                              ZBD_LOW_BATTERY_MSG, 
                              battery_level);

      msgZ2SDeviceHvac(channel_number_slot_2,
                       TRV_LOW_BATTERY_MSG, 
                       Tuya_read_dp_result.dp_value);
    }
  }

  if (battery_level_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(battery_level_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      updateSuplaBatteryLevel(channel_number_slot_1, 
                              ZBD_BATTERY_LEVEL_MSG, 
                              Tuya_read_dp_result.dp_value);

      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_BATTERY_LEVEL_MSG, 
                       Tuya_read_dp_result.dp_value);
    }
  }

  if (child_lock_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(child_lock_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_CHILD_LOCK_MSG, 
                       Tuya_read_dp_result.dp_value);
    }
  }

  if (window_detect_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(window_detect_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_WINDOW_DETECT_MSG, 
                       Tuya_read_dp_result.dp_value);
    }
  }

  if (anti_freeze_dp_id) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(anti_freeze_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_ANTI_FREEZE_MSG, 
                       Tuya_read_dp_result.dp_value);
    }
  }

  if (limescale_protect_dp_id) {
    
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(limescale_protect_dp_id, 
                                              payload_size, 
                                              payload);
    
    if (Tuya_read_dp_result.is_success) {

      msgZ2SDeviceHvac(channel_number_slot_2, 
                       TRV_LIMESCALE_PROTECT_MSG, 
                       Tuya_read_dp_result.dp_value);
    }
  }
}

void processTuyaDoubleDimmerSwitchDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload) {

  int8_t sub_id = ((*(payload + 2)) < 7) ? TUYA_DOUBLE_DIMMER_SWITCH_1_SID : TUYA_DOUBLE_DIMMER_SWITCH_2_SID; 
  
  int16_t channel_number_slot_sub_id = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                                 z2s_channels_table[channel_number_slot].endpoint, 
                                                                 z2s_channels_table[channel_number_slot].cluster_id, 
                                                                 SUPLA_CHANNELTYPE_DIMMER, sub_id);
  
  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_1_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, DIMMER_NO_LEVEL_DATA, Tuya_read_dp_result.dp_value); 

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_2_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, DIMMER_NO_LEVEL_DATA, Tuya_read_dp_result.dp_value); 
       
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_1_BRIGHTNESS_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, Tuya_read_dp_result.dp_value, false);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_DOUBLE_DIMMER_SWITCH_2_BRIGHTNESS_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceDimmer(channel_number_slot_sub_id, Tuya_read_dp_result.dp_value, false); 
}

void processTuyaSoilTempHumiditySensorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, float divider) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SOIL_SENSOR_TEMPERATURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/divider);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SOIL_SENSOR_MOISTURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SOIL_SENSOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level 0x0F is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);  
  }
}

void processTuyaTempHumiditySensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload){

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_TEMPERATURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, (float)Tuya_read_dp_result.dp_value/10);
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_HUMIDITY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, (float)Tuya_read_dp_result.dp_value);  
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_BATTERY_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery state is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot,ZBD_BATTERY_STATE_MSG, Tuya_read_dp_result.dp_value * 50);  
  }
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_TH_SENSOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value );  
  }
}

void processTuyaIlluminanceSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload){

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_ILLUMINANCE_SENSOR_ILLUMINANCE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, Tuya_read_dp_result.dp_value);
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_ILLUMINANCE_SENSOR_BATTERY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value );  
  }
}

void processTuya3PhasesElectricityMeterDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload) {


  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3, channel_number_slot_4, channel_number_slot_5;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_3PHASES_ELECTRICITY_METER_ENERGY_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_SID);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_3PHASES_ELECTRICITY_METER_POWER_SID);

  channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_SID);

  channel_number_slot_5 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_ELECTRICITY_METER, TUYA_3PHASES_ELECTRICITY_METER_SID);

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_1, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          ((double)Tuya_read_dp_result.dp_value) / 100);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          ((double)Tuya_read_dp_result.dp_value) / 100);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_4, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          (int32_t)Tuya_read_dp_result.dp_value);

  
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_AC_FREQUENCY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_AC_FREQUENCY, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_VOLTAGE_A_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_CURRENT_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_CURRENT_A_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACTIVE_POWER_A_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_POWER_FACTOR_A_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACT_FWD_ENERGY_A_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACT_RVR_ENERGY_A_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_B_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_VOLTAGE_B_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_CURRENT_B_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_CURRENT_B_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_B_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACTIVE_POWER_B_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_B_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_POWER_FACTOR_B_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_B_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACT_FWD_ENERGY_B_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_B_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACT_RVR_ENERGY_B_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_C_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_VOLTAGE_C_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_CURRENT_C_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_CURRENT_C_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_C_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACTIVE_POWER_C_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_C_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_POWER_FACTOR_C_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_C_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACT_FWD_ENERGY_C_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_C_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_5, Z2S_EM_ACT_RVR_ENERGY_C_SEL, Tuya_read_dp_result.dp_value);
}

void processTuya1PhaseElectricityMeterDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload) {


  int16_t channel_number_slot_1;

  
  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_ELECTRICITY_METER, NO_CUSTOM_CMD_SID);

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_AC_FREQUENCY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_AC_FREQUENCY, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_VOLTAGE_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_VOLTAGE_A_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_CURRENT_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_CURRENT_A_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_POWER_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_ACTIVE_POWER_A_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_POWER_FACTOR_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_POWER_FACTOR_A_SEL, (int32_t)Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_ENERGY_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_ACT_FWD_ENERGY_A_SEL, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_1PHASE_ELECTRICITY_METER_ENERGY_PRODUCED_A_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceElectricityMeter(channel_number_slot_1, Z2S_EM_ACT_RVR_ENERGY_A_SEL, Tuya_read_dp_result.dp_value);
}

void processTuyaEF00Switch2x3DataReport(int16_t channel_number_slot, 
                                        uint16_t payload_size, 
                                        uint8_t *payload) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_EF00_SWITCH_2X3_BUTTON_1_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
    int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                              z2s_channels_table[channel_number_slot].endpoint, 
                                                              z2s_channels_table[channel_number_slot].cluster_id, 
                                                              SUPLA_CHANNELTYPE_ACTIONTRIGGER, 
                                                              Tuya_read_dp_result.dp_value);

    msgZ2SDeviceActionTrigger(channel_number_slot_1);
  }
            
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_EF00_SWITCH_2X3_BUTTON_2_DP, payload_size, payload);

  if (Tuya_read_dp_result.is_success) {

    int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                              z2s_channels_table[channel_number_slot].endpoint, 
                                                              z2s_channels_table[channel_number_slot].cluster_id, 
                                                              SUPLA_CHANNELTYPE_ACTIONTRIGGER, 
                                                              3 + Tuya_read_dp_result.dp_value);

    msgZ2SDeviceActionTrigger(channel_number_slot_1);
  }
  
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_EF00_SWITCH_2X3_BATTERY_DP, payload_size, payload);

  if (Tuya_read_dp_result.is_success) { 
    
    log_i("Battery level is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);  
  }
}

void processTuyaSmokeDetectorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  int16_t channel_number_slot_1;
  int16_t channel_number_slot_2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  if (z2s_channels_table[channel_number_slot].Supla_channel_type == SUPLA_CHANNELTYPE_BINARYSENSOR) {

    channel_number_slot_1 = channel_number_slot;
    channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, NO_CUSTOM_CMD_SID);
  }
  else {
    channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);
    channel_number_slot_2 = channel_number_slot;
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_SMOKE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    if (model_id == Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2)
      msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 0));
    else
      msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1));
  }
  
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_PPM_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_PPM, 
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level 0x0F is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
    //updateSuplaBatteryLevel(channel_number_slot_2, Tuya_read_dp_result.dp_value);
  }

  if (model_id == Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1) {

    Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_SMOKE_DETECTOR_BATTERY_STATE_DP, payload_size, payload);
    if (Tuya_read_dp_result.is_success) { 
      log_i("Battery state 0x0E is %d", Tuya_read_dp_result.dp_value * 50);
      updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_STATE_MSG, Tuya_read_dp_result.dp_value * 50);
    }
  }
}

void processTuyaCOGasDetectorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3, channel_number_slot_4, channel_number_slot_5;
  Tuya_read_dp_result_t Tuya_read_dp_result;


    channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_BINARYSENSOR, TUYA_CO_DETECTOR_CO_SID);
    
    channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_CO_DETECTOR_CO_CONC_SID);

    channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_CO_DETECTOR_SELF_TEST_SID);

    channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_BINARYSENSOR, TUYA_CO_DETECTOR_SILENCE_SID);

    channel_number_slot_5 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_BINARYSENSOR, TUYA_GAS_DETECTOR_PREHEAT_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_CO_DETECTOR_CO_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1));

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_CO_DETECTOR_CO_CONC_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_PPM, 
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_CO_DETECTOR_SELF_TEST_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_CO_DETECTOR_SILENCE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_4, (Tuya_read_dp_result.dp_value == 1));

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_CO_DETECTOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level 0x0F is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_GAS_DETECTOR_PREHEAT_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceIASzone(channel_number_slot_5, (Tuya_read_dp_result.dp_value == 1));
}

void processTuyaAirQualitySensorReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3, channel_number_slot_4;
  Tuya_read_dp_result_t Tuya_read_dp_result;


    channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, TUYA_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID);
    
    channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_AIR_QUALITY_SENSOR_CO2_SID);

    channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_AIR_QUALITY_SENSOR_VOC_SID);

    channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, TUYA_AIR_QUALITY_SENSOR_FA_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_AIR_QUALITY_SENSOR_TEMPERATURE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    float temperature = Tuya_read_dp_result.dp_value;
    if (Tuya_read_dp_result.dp_value > 0x2000)
      temperature -= 0xFFFF;
    temperature /= 10;
    msgZ2SDeviceTempHumidityTemp(channel_number_slot_1, temperature);  
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_AIR_QUALITY_SENSOR_HUMIDITY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceTempHumidityHumi(channel_number_slot_1, (float)Tuya_read_dp_result.dp_value/10);  

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_AIR_QUALITY_SENSOR_CO2_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_PPM, 
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_AIR_QUALITY_SENSOR_VOC_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_AIR_QUALITY_SENSOR_FA_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_4, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          Tuya_read_dp_result.dp_value);
}

void processTuyaPresenceSensorDataReport(int16_t channel_number_slot, 
                                         uint16_t payload_size,
                                         uint8_t *payload, 
                                         uint32_t model_id) {

  int16_t channel_number_slot_1; 
  int16_t channel_number_slot_2; 
  int16_t channel_number_slot_3;
  int16_t channel_number_slot_4;
  int16_t channel_number_slot_5;
  int16_t channel_number_slot_6;
  int16_t channel_number_slot_7;

  Tuya_read_dp_result_t Tuya_read_dp_result;

  uint8_t presence_dp_id = 0x00;
  uint8_t motion_state_dp_id  = 0x00;
  uint8_t illuminance_dp_id   = 0x00;
  uint8_t temperature_dp_id   = 0x00;
  uint8_t humidity_dp_id      = 0x00;
  uint8_t battery_dp_id       = 0x00;
  uint8_t distance_dp_id      = 0x00;
  uint8_t relay_state_dp_id   = 0x00;
  uint8_t relay_mode_dp_id    = 0x00;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                    TUYA_PRESENCE_SENSOR_PRESENCE_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_PRESENCE_SENSOR_MOTION_STATE_SID);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID);

  channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, 
                                                    TUYA_PRESENCE_SENSOR_TEMPHUMIDITY_SID);
  
  channel_number_slot_5 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_PRESENCE_SENSOR_DISTANCE_SID);

  channel_number_slot_6 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_RELAY, 
                                                    TUYA_PRESENCE_SENSOR_RELAY_SWITCH_SID);

    channel_number_slot_7 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                      z2s_channels_table[channel_number_slot].endpoint, 
                                                      z2s_channels_table[channel_number_slot].cluster_id, 
                                                      SUPLA_CHANNELTYPE_RELAY, 
                                                      TUYA_PRESENCE_SENSOR_RELAY_MODE_SID);

  switch(model_id) {


    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
      
      presence_dp_id = TUYA_PRESENCE_SENSOR_PRESENCE_DP;
      motion_state_dp_id = TUYA_PRESENCE_SENSOR_MOTION_STATE_DP;
      illuminance_dp_id = TUYA_PRESENCE_SENSOR_ILLUMINANCE_DP;
      battery_dp_id = TUYA_PRESENCE_SENSOR_BATTERY_DP;
    } break;


    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5: {

      presence_dp_id = TUYA_PRESENCE_SENSOR_5_PRESENCE_DP;
      illuminance_dp_id = TUYA_PRESENCE_SENSOR_5_ILLUMINANCE_DP;
      battery_dp_id = TUYA_PRESENCE_SENSOR_BATTERY_DP;
    } break;


    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1: {

      presence_dp_id = TUYA_PRESENCE_SENSOR_PRESENCE_DP;
      illuminance_dp_id = TUYA_PRESENCE_SENSOR_ILLUMINANCE_DP;
      temperature_dp_id = TUYA_PRESENCE_SENSOR_4IN1_TEMPERATURE_DP;
      humidity_dp_id =  TUYA_PRESENCE_SENSOR_4IN1_HUMIDITY_DP;
      battery_dp_id = TUYA_PRESENCE_SENSOR_4IN1_BATTERY_DP;
    } break;


    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY: {

      presence_dp_id = TUYA_PRESENCE_SENSOR_PRESENCE_DP;
      illuminance_dp_id = TUYA_PRESENCE_SENSOR_RELAY_ILLUMINANCE_DP;
      distance_dp_id = TUYA_PRESENCE_SENSOR_RELAY_DISTANCE_DP;
      relay_state_dp_id = TUYA_PRESENCE_SENSOR_RELAY_SWITCH_STATE_DP;
      relay_mode_dp_id = TUYA_PRESENCE_SENSOR_RELAY_SWITCH_MODE_DP;
    } break;
  }
  
  if (presence_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(presence_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success)
      msgZ2SDeviceIASzone(channel_number_slot_1, 
                          (Tuya_read_dp_result.dp_value == 1));
  }

  if (motion_state_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(motion_state_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) { 
      log_i("MOTION STATE CHECK int %d, float %f", 
            Tuya_read_dp_result.dp_value, Tuya_read_dp_result.dp_value);

      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, 
                                            ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_MOTION_STATE,
                                            Tuya_read_dp_result.dp_value);
    }                                         
  }

  if (illuminance_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(illuminance_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) 
      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, 
                                            ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE,
                                            Tuya_read_dp_result.dp_value);
  }
  
  if (temperature_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(temperature_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) 
      msgZ2SDeviceTempHumidityTemp(channel_number_slot_4, 
                                   (float)Tuya_read_dp_result.dp_value/10);
  }

  if (humidity_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(humidity_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) 
      msgZ2SDeviceTempHumidityHumi(channel_number_slot_4, 
                                   (float)Tuya_read_dp_result.dp_value);
  }

  if (distance_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(distance_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) 
      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_5, 
                                            ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                            Tuya_read_dp_result.dp_value);
  }

  if (relay_state_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(relay_state_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) 
      msgZ2SDeviceVirtualRelay(channel_number_slot_6,
                               Tuya_read_dp_result.dp_value);
  }

  if (relay_mode_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(relay_mode_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) 
      msgZ2SDeviceVirtualRelay(channel_number_slot_7,
                               Tuya_read_dp_result.dp_value);
  }

  if (battery_dp_id) {
    Tuya_read_dp_result = Z2S_readTuyaDPvalue(battery_dp_id, payload_size, payload);
    if (Tuya_read_dp_result.is_success) { 
      log_i("Battery level  is %d", Tuya_read_dp_result.dp_value);
      updateSuplaBatteryLevel(channel_number_slot_1, 
                              ZBD_BATTERY_LEVEL_MSG, 
                              Tuya_read_dp_result.dp_value);
    }
  }
}

void processTuyaRainSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3, channel_number_slot_4, channel_number_slot_5;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_RAIN_SENSOR_ILLUMINANCE_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID);
  
  channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID);

  channel_number_slot_5 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                    NO_CUSTOM_CMD_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_ILLUMINANCE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_1, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
  { log_i("MOTION STATE CHECK int %d, float %f", Tuya_read_dp_result.dp_value, Tuya_read_dp_result.dp_value);
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value);}

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value);
  
  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_RAIN_INTENSITY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_4, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value);
    if (z2s_channels_table[channel_number_slot_4].user_data_1 > 0) {
      if (Tuya_read_dp_result.dp_value > z2s_channels_table[channel_number_slot_4].user_data_1)
        msgZ2SDeviceIASzone(channel_number_slot_5, true, false); 
      else
        msgZ2SDeviceIASzone(channel_number_slot_5, false, false); 
    }    
  }
}

void processTuyaRainSensor2DataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload) {

  int16_t channel_number_slot_1, channel_number_slot_2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    TUYA_RAIN_SENSOR_ILLUMINANCE_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                    TUYA_RAIN_SENSOR_RAIN_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_2_ILLUMINANCE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_1, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE,
                                          Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_2_RAIN_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) 
    msgZ2SDeviceIASzone(channel_number_slot_2, Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_RAIN_SENSOR_2_BATTERY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) { 
    log_i("Battery level  is %d", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }
}

void processMoesShadesDriveMotorDataReport(int16_t channel_number_slot, 
                                           uint16_t payload_size,
                                           uint8_t *payload, 
                                           uint32_t model_id) {

  Tuya_read_dp_result_t Tuya_read_dp_result;


  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_SHADES_DRIVE_MOTOR_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success)
    log_i("state = %u", Tuya_read_dp_result.dp_value);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_SHADES_DRIVE_MOTOR_STATE_COVER_POSITION_PERCENTAGE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
    log_i("position(%) = %u", Tuya_read_dp_result.dp_value);
  
    msgZ2SDeviceRollerShutter(channel_number_slot, RS_MOVING_DIRECTION_MSG, 0);
    msgZ2SDeviceRollerShutter(channel_number_slot, RS_CURRENT_POSITION_LIFT_PERCENTAGE_MSG, 100 - Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_SHADES_DRIVE_MOTOR_STATE_COVER_POSITION_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
    log_i("position = %u", Tuya_read_dp_result.dp_value);
    
    msgZ2SDeviceRollerShutter(channel_number_slot, RS_MOVING_DIRECTION_MSG, 0);
    msgZ2SDeviceRollerShutter(channel_number_slot, RS_CURRENT_POSITION_LIFT_PERCENTAGE_MSG, 100 - Tuya_read_dp_result.dp_value);
  } 

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_COVER_MOTOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
    log_i("battery level = %u", Tuya_read_dp_result.dp_value);
    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }
}

void processMoesAlarmDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  int16_t channel_number_slot_1, channel_number_slot_2, channel_number_slot_3, channel_number_slot_4, channel_number_slot_5;
  Tuya_read_dp_result_t Tuya_read_dp_result;
  char display_buffer[5] = {};

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_RELAY, 
                                                    MOES_ALARM_SWITCH_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_RELAY, 
                                                    MOES_ALARM_MELODY_SID);

  channel_number_slot_3 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                  z2s_channels_table[channel_number_slot].endpoint, 
                                                  z2s_channels_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_RELAY, 
                                                  MOES_ALARM_VOLUME_SID);
  
  channel_number_slot_4 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                  z2s_channels_table[channel_number_slot].endpoint, 
                                                  z2s_channels_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_RELAY, 
                                                  MOES_ALARM_DURATION_SID);

  channel_number_slot_5 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                  z2s_channels_table[channel_number_slot].endpoint, 
                                                  z2s_channels_table[channel_number_slot].cluster_id, 
                                                  SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                  MOES_ALARM_DISPLAY_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_ALARM_SWITCH_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
    msgZ2SDeviceVirtualRelay(channel_number_slot_1, Tuya_read_dp_result.dp_value);
    //display_buffer[0] = Tuya_read_dp_result.dp_value + '0';
    //msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_5, setU32Digits(Tuya_read_dp_result.dp_value,9,10
    msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 9, 10, Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_ALARM_MELODY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceVirtualRelayValue(channel_number_slot_2, VRV_U8_ID, Tuya_read_dp_result.dp_value); 
    //sprintf(display_buffer, "%02u", Tuya_read_dp_result.dp_value);
    //msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 3, 2, display_buffer);
    msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 7, 8, Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_ALARM_VOLUME_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceVirtualRelayValue(channel_number_slot_3, VRV_U8_ID, Tuya_read_dp_result.dp_value); 
    //sprintf(display_buffer, "%u", Tuya_read_dp_result.dp_value);
    //msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 6, 1, display_buffer);
    msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 5, 6, Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_ALARM_DURATION_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceVirtualRelayValue(channel_number_slot_4, VRV_U32_ID, Tuya_read_dp_result.dp_value);
    //sprintf(display_buffer, "%04u", Tuya_read_dp_result.dp_value);
   //msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 8, 4, display_buffer); 
   msgZ2SDeviceGeneralPurposeMeasurementDisplay(channel_number_slot_5, 1, 4, Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(MOES_ALARM_BATTERY_PERCENTAGE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }  
}

void processTuyaVibrationSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  int16_t channel_number_slot_1, channel_number_slot_2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                    TUYA_VIBRATION_SENSOR_VIBRATION_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                    TUYA_VIBRATION_SENSOR_CONTACT_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_VIBRATION_SENSOR_VIBRATION_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1));
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_VIBRATION_SENSOR_CONTACT_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceIASzone(channel_number_slot_2, (Tuya_read_dp_result.dp_value == 1));
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_VIBRATION_SENSOR_BATTERY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }  
}

void processTuyaPIRIlluminanceSensorDataReport(int16_t channel_number_slot, uint16_t payload_size,
                                               uint8_t *payload, uint32_t model_id) {

  int16_t channel_number_slot_1, channel_number_slot_2;
  Tuya_read_dp_result_t Tuya_read_dp_result;

  channel_number_slot_1 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                    Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR_PIR_SID);

  channel_number_slot_2 = Z2S_findChannelNumberSlot(z2s_channels_table[channel_number_slot].ieee_addr, 
                                                    z2s_channels_table[channel_number_slot].endpoint, 
                                                    z2s_channels_table[channel_number_slot].cluster_id, 
                                                    SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                    Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR_IL_SID);

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_PIR_ILLUMINANCE_SENSOR_PIR_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceIASzone(channel_number_slot_1, (Tuya_read_dp_result.dp_value == 1));
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_PIR_ILLUMINANCE_SENSOR_ILLUMINANCE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, 
                                          ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, 
                                          Tuya_read_dp_result.dp_value);
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_PIR_ILLUMINANCE_SENSOR_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    updateSuplaBatteryLevel(channel_number_slot_1, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }  
}

void processTuyaOnOffValveBatteryDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_ON_OFF_BATTERY_VALVE_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    switch (Tuya_read_dp_result.dp_value) {

      case 1:
        msgZ2SDeviceVirtualValve(channel_number_slot, true); break;
      
      case 2:
        msgZ2SDeviceVirtualValve(channel_number_slot, false); break;

      case 0:
        log_i("valve state unknown!"); break;
    }
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(TUYA_ON_OFF_BATTERY_VALVE_BATTERY_LEVEL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }  
}

void processGiexSmartValveDataReport(int16_t channel_number_slot, uint16_t payload_size,uint8_t *payload, uint32_t model_id) {

  Tuya_read_dp_result_t Tuya_read_dp_result;

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_STATE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_MODE_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_IRRIGATION_START_TIME_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_IRRIGATION_END_TIME_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_CYCLE_IRRIGATION_NUMBER_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_IRRIGATION_TARGET_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_CYCLE_IRRIGATION_INTERVAL_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_WATER_CONSUMED_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_LAST_IRRIGATION_DURATION_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {
    
  }

  Tuya_read_dp_result = Z2S_readTuyaDPvalue(GIEX_WATER_VALVE_BATTERY_DP, payload_size, payload);
  if (Tuya_read_dp_result.is_success) {

    updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_LEVEL_MSG, Tuya_read_dp_result.dp_value);
  }  
}

void processTuyaDataReport(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t payload_size, uint8_t *payload) {

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, TUYA_PRIVATE_CLUSTER_EF00, 
                                                          ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID); //first find anything to recognize model_id
  if (channel_number_slot < 0) {
    log_i("failed - no Supla channel for that device");
    return;
  }
  uint32_t model_id = z2s_channels_table[channel_number_slot].model_id;

  switch (model_id) {
    
    case Z2S_DEVICE_DESC_TUYA_HVAC_6567C: 
    case Z2S_DEVICE_DESC_TUYA_HVAC_23457:
    case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY:
    case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL:
    case Z2S_DEVICE_DESC_TS0601_TRV_ME167:
    case Z2S_DEVICE_DESC_TS0601_TRV_BECA:
    case Z2S_DEVICE_DESC_TS0601_TRV_MOES:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV601:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV603:
    case Z2S_DEVICE_DESC_TS0601_TRV_GTZ10:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z:
    case Z2S_DEVICE_DESC_TS0601_TRV_TV02:
    case Z2S_DEVICE_DESC_TS0601_TRV_SITERWELL:

      processTuyaHvacDataReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: 

      processTuyaDoubleDimmerSwitchDataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR:

      processTuyaSoilTempHumiditySensorReport(channel_number_slot, payload_size, payload, 10); break;


    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:

      processTuyaSoilTempHumiditySensorReport(channel_number_slot, payload_size, payload, 1); break;


    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR: 

      processTuyaTempHumiditySensorDataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_DP_SENSOR:

      processTuyaIlluminanceSensorDataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR: 
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2:

      processTuyaSmokeDetectorReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR:

      processTuyaCOGasDetectorReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR:

      processTuyaAirQualitySensorReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: 
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY:

      processTuyaPresenceSensorDataReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3:

      processTuyaEF00Switch2x3DataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR:

      processTuyaRainSensorDataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2:

      processTuyaRainSensor2DataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER:

      processTuya3PhasesElectricityMeterDataReport(channel_number_slot, payload_size, payload); break;


    case Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER:

      processTuya1PhaseElectricityMeterDataReport(channel_number_slot, payload_size, payload); break;

    
    case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR:

      processMoesShadesDriveMotorDataReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_MOES_ALARM:

      processMoesAlarmDataReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR:

      processTuyaVibrationSensorDataReport(channel_number_slot, payload_size, payload, model_id); break;

    case Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR:

      processTuyaPIRIlluminanceSensorDataReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY:

      processTuyaOnOffValveBatteryDataReport(channel_number_slot, payload_size, payload, model_id); break;


    case Z2S_DEVICE_DESC_GIEX_SMART_VALVE:

      processGiexSmartValveDataReport(channel_number_slot, payload_size, payload, model_id); break;

      
    default: 
      log_i("Unknown device model id 0x%x", z2s_channels_table[channel_number_slot].model_id); break;
  }
}

void processTuyaCustomCluster(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, uint16_t payload_size, uint8_t *payload) {
  log_i("processing Tuya custom cluster 0xEF00, command id 0x%x", command_id);
  switch (command_id) {
    
    case TUYA_DATA_REPORT_CMD:
    case 0x01:
    case 0x06:
       processTuyaDataReport(ieee_addr, endpoint, payload_size, payload); break;

    case TUYA_MCU_SYNC_TIME: {
      uint8_t time_sync[10];
      struct tm *tptr;
      time_t secs, local_secs, gmt_secs;
      zbg_device_params_t device = {};

      int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, TUYA_PRIVATE_CLUSTER_EF00, 
                                                              ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID); 
      if (channel_number_slot < 0) {
        log_i("TUYA_MCU_SYNC_TIME failed - no Supla channel for that device");
        return;
      }

      device.endpoint = z2s_channels_table[channel_number_slot].endpoint;
      device.cluster_id = z2s_channels_table[channel_number_slot].cluster_id;
      memcpy(device.ieee_addr, z2s_channels_table[channel_number_slot].ieee_addr, 8);
      device.short_addr = z2s_channels_table[channel_number_slot].short_addr;
      device.model_id = z2s_channels_table[channel_number_slot].model_id;
  

      time_sync[0] = *payload;
      time_sync[1] = *(payload + 1);

      time( &secs );  // Current time in GMT
      // Remember that localtime/gmtime overwrite same location
      tptr = localtime( &secs );
      local_secs = mktime( tptr );
      tptr = gmtime( &secs );
      if (!tptr) log_e("gmtime not supported\n\r");
      gmt_secs = mktime( tptr );
      long diff_secs = long(local_secs - gmt_secs);
      log_i("local secs: %llu, gmt_secs: %llu, diff: %llu\n\r", local_secs, gmt_secs, diff_secs);

      time_sync[2] = uint8_t(gmt_secs>>24); 
      time_sync[3] = uint8_t(gmt_secs>>16); 
      time_sync[4] = uint8_t(gmt_secs>>8);
      time_sync[5] = uint8_t(gmt_secs);
      
      time_sync[6] = uint8_t(local_secs>>24); 
      time_sync[7] = uint8_t(local_secs>>16); 
      time_sync[8] = uint8_t(local_secs>>8);
      time_sync[9] = uint8_t(local_secs);

      log_i("TUYA_MCU_SYNC_TIME response payload: %X%X:%X%X%X%X:%X%X%X%X",
            time_sync[0], time_sync[1], time_sync[2],time_sync[3], time_sync[4], 
            time_sync[5], time_sync[6], time_sync[7], time_sync[8], time_sync[9]);
      zbGateway.sendCustomClusterCmd(&device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_MCU_SYNC_TIME, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, time_sync, false);

    } break;

    case TUYA_MCU_VERSION_RESPONSE: {

      uint8_t seq[2];
    
      zbg_device_params_t device = {};

      int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, TUYA_PRIVATE_CLUSTER_EF00, 
                                                              ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID); 
      if (channel_number_slot < 0) {
        log_i("TUYA_MCU_VERSION_REQUEST failed - no Supla channel for that device");
        return;
      }

      device.endpoint = z2s_channels_table[channel_number_slot].endpoint;
      device.cluster_id = z2s_channels_table[channel_number_slot].cluster_id;
      memcpy(device.ieee_addr, z2s_channels_table[channel_number_slot].ieee_addr, 8);
      device.short_addr = z2s_channels_table[channel_number_slot].short_addr;
      device.model_id = z2s_channels_table[channel_number_slot].model_id;
  

      seq[0] = 00;
      seq[1] = 02;

      log_i("Sending TUYA_VERSION_REQUEST");
      zbGateway.sendCustomClusterCmd(&device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_MCU_VERSION_REQUEST, ESP_ZB_ZCL_ATTR_TYPE_SET, 2, seq, false);
    } break;
    
    default: log_i("Tuya custom cluster 0xEF00 command id 0x%x wasn't processed", command_id); break;
  }
}

void processZosungCustomCluster(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, uint16_t payload_size, uint8_t *payload) {
  
  log_i("processing Zosung custom cluster 0xED00, command id 0x%x", command_id);
  uint8_t ir_code_data_1[17];
  uint8_t ir_code_data_2[7];
  uint8_t ir_code_data_4[5];
  uint8_t ir_code_data_5[4];
  zbg_device_params_t device = {};

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 
                                                              ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID); 
  if (channel_number_slot < 0) {
    log_i("processZosungCustomCluster failed - no Supla channel for that device");
    return;
  }

  device.endpoint = z2s_channels_table[channel_number_slot].endpoint;
  device.cluster_id = ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER;
  memcpy(device.ieee_addr, z2s_channels_table[channel_number_slot].ieee_addr, 8);
  device.short_addr = z2s_channels_table[channel_number_slot].short_addr;
  device.model_id = z2s_channels_table[channel_number_slot].model_id;
  
  switch (command_id) {
    case 0: if ((!ir_code_learning) && (payload_size == 16)) {
      
      ir_code_learning = true;
      ir_code_receiving = false;
      
      memset(learned_ir_code, 0, sizeof(learned_ir_code));
      ir_message_position = 0;
      ir_message_last_position = 0;
      
      ir_message_chunk_last_position = 0xFFFFFFFF;
      ir_message_chunk_last_size = 0xFFFFFFFF;
      
      ir_message_length = *(payload + 2) + (*(payload + 3) * 0x100) + (*(payload + 4) * 0x10000) + (*(payload + 5) * 0x1000000);
      ir_message_seq = *payload + (*(payload + 1) * 0x100);
      log_i("message seq 0x%x, length 0x%x", ir_message_seq, ir_message_length);

      learned_ir_code[0] = ir_message_length;

      memset(ir_code_data_1, 0, sizeof(ir_code_data_1));
      memcpy(ir_code_data_1+1, payload, payload_size);
      for (int i = 0; i < 17; i++)        log_i("ir_code_data_1[%u] = 0x%x", i, ir_code_data_1[i]);
      memset(ir_code_data_2, 0, sizeof(ir_code_data_2));
      ir_code_data_2[0] = *payload;
      ir_code_data_2[1] = *(payload + 1);
      ir_code_data_2[6] = 0x38;
      for (int i = 0; i < 7; i++)
        log_i("ir_code_data_2[%u] = 0x%x", i, ir_code_data_2[i]);
      zbGateway.sendCustomClusterCmd(&device, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 1, ESP_ZB_ZCL_ATTR_TYPE_SET, 17, ir_code_data_1, true, 
                                     ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 0x1002);
      zbGateway.sendCustomClusterCmd(&device, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 0x0B, ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, NULL,true, 1, 1, 0, 0);
      zbGateway.sendCustomClusterCmd(&device, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 2, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, ir_code_data_2, true, 
                                     ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 0x1002);
    } break;
    case 1: {
      ir_code_send_position = 0;
      ir_code_send_seq = *(payload + 1) + (*(payload + 2) * 0x100);
      log_i("IR Remote confirmed IR code receiving start, seq %u", ir_code_send_seq);
      memcpy(ir_code_to_send, IR_SEND_CODE_PREAMBLE, sizeof(IR_SEND_CODE_PREAMBLE));
      //memcpy(ir_code_to_send + sizeof(IR_SEND_CODE_PREAMBLE), TEST_CODE_01, sizeof(TEST_CODE_01));
      //memcpy(ir_code_to_send + sizeof(IR_SEND_CODE_PREAMBLE), learned_ir_code + 1, ir_message_length);
      memcpy(ir_code_to_send + sizeof(IR_SEND_CODE_PREAMBLE), learned_ir_code_base64, learned_ir_code_base64_length);
      //memcpy(ir_code_to_send + sizeof(IR_SEND_CODE_PREAMBLE) + sizeof(TEST_CODE_01), IR_SEND_CODE_POSTAMBLE, sizeof(IR_SEND_CODE_POSTAMBLE));
      //memcpy(ir_code_to_send + sizeof(IR_SEND_CODE_PREAMBLE) + ir_message_length, IR_SEND_CODE_POSTAMBLE, sizeof(IR_SEND_CODE_POSTAMBLE));
      memcpy(ir_code_to_send + sizeof(IR_SEND_CODE_PREAMBLE) + learned_ir_code_base64_length, IR_SEND_CODE_POSTAMBLE, sizeof(IR_SEND_CODE_POSTAMBLE));
      ir_code_send_length = sizeof(IR_SEND_CODE_PREAMBLE) + learned_ir_code_base64_length + sizeof(IR_SEND_CODE_POSTAMBLE);
      //ir_code_send_length = sizeof(IR_SEND_CODE_PREAMBLE) + ir_message_length + sizeof(IR_SEND_CODE_POSTAMBLE);
      //ir_message_length = sizeof(IR_SEND_CODE_PREAMBLE) + sizeof(TEST_CODE_01) + sizeof(IR_SEND_CODE_POSTAMBLE);

    } break;
    case 2: {
      
      uint8_t preamble_size = sizeof(IR_SEND_CODE_PREAMBLE);
      uint8_t  postamble_size = sizeof(IR_SEND_CODE_POSTAMBLE);

      uint16_t cur_seq = *(payload) + (*(payload + 1) * 0x100);
      if (ir_code_send_seq == cur_seq) {
        ir_code_send_position = *(payload + 2) + (*(payload + 3) * 0x100) + (*(payload + 4) * 0x10000) + (*(payload + 5) * 0x1000000);
        uint32_t crc_sum = 0;
        uint16_t max_length = *(payload + 6);
        uint16_t calc_length = ir_code_send_length - ir_code_send_position;
        if (calc_length >= max_length) 
          calc_length = max_length;
        else (calc_length++); 
        log_i("IR Remote command 2 received, asking for code - ir_code_send_length %u, position %u, max length %u, calc length %u", 
              ir_code_send_length, ir_code_send_position, max_length, calc_length);
        ir_code_send_buffer[0] = 0;
        ir_code_send_buffer[1] = *(payload);
        ir_code_send_buffer[2] = *(payload + 1);
        ir_code_send_buffer[3] = *(payload + 2);
        ir_code_send_buffer[4] = *(payload + 3);
        ir_code_send_buffer[5] = *(payload + 4);
        ir_code_send_buffer[6] = *(payload + 5);
        ir_code_send_buffer[7] = calc_length - 1;
        for (uint16_t i = 0; i < (calc_length - 1); i++) {
          //if (ir_code_send_position + i != 0)
            crc_sum += ir_code_to_send[ir_code_send_position + i];//learned_ir_code[1 + ir_code_send_position + i];
          ir_code_send_buffer[8 + i] = ir_code_to_send[ir_code_send_position + i];//learned_ir_code[1 + ir_code_send_position + i];
        }  
        ir_code_send_buffer[7 + calc_length] = crc_sum % 256;
        for (int i = 0; i < 8 + calc_length; i++)
          log_i("IR Remote command 2 buffer[%u] = 0x%x", i, ir_code_send_buffer[i]);
        zbGateway.sendCustomClusterCmd(&device, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 3, ESP_ZB_ZCL_ATTR_TYPE_SET, 7 + calc_length + 1, ir_code_send_buffer, 
                                      true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,  1, 0, 0);
      }
    } break;
    case 3: {
      ir_code_receiving = true;
      uint32_t crc_sum = 0;
      uint32_t crc_sum_mod = 0;
      uint16_t cur_seq = *(payload + 1) + (*(payload + 2) * 0x100);
      if (ir_message_seq == cur_seq)
        log_i("ir code learing seq matched 0x%x == 0x%x", ir_message_seq, cur_seq);
      else
        log_i("ir code learing seq not matched 0x%x != 0x%x", ir_message_seq, cur_seq);
      
      ir_message_position = *(payload + 3) + (*(payload + 4) * 0x100) + (*(payload + 5) * 0x10000) + (*(payload + 6) * 0x1000000);
      
      if (ir_message_position == 0)
          ir_message_position++;

      if (ir_message_position != ir_message_chunk_last_position) {
        
        ir_message_chunk_last_position = ir_message_position;
        ir_message_chunk_last_size = *(payload + 7);
        for (int i = 0; i < ir_message_chunk_last_size; i++) {
          log_i("cmd 3 [%u] = 0x%x", i + 8, *(payload + 8 + i));
          crc_sum = crc_sum + *(payload + 8 + i);
          learned_ir_code[ir_message_position + i] = *(payload + 8 + i);
        }
        log_i("crc_sum 0x%x, crc_sum mod 256 0x%x", crc_sum, crc_sum % 256);
        log_i("ir_message_position %u, ir_message_chunk_last_position %u, ir_message_chunk_last_size %u", 
               ir_message_position, ir_message_chunk_last_position,ir_message_chunk_last_size);

        if ((ir_message_position + ir_message_chunk_last_size) < ir_message_length) {
          ir_code_data_2[0] = *(payload + 1);
          ir_code_data_2[1] = *(payload + 2);
          ir_code_data_2[2] = ir_message_position + ir_message_chunk_last_size;
          ir_code_data_2[3] = *(payload + 4);
          ir_code_data_2[4] = *(payload + 5);
          ir_code_data_2[5] = *(payload + 6);
          ir_code_data_2[6] = 0x38;
          for (int i = 0; i < 7; i++) {
            log_i("cmd 2 [%u] = 0x%x", i, ir_code_data_2[i]);
          }
          zbGateway.sendCustomClusterCmd(&device, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 2, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, ir_code_data_2, true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
                                       1, 1, 0x1002);
        }
        else {
          ir_code_data_4[0] = 0;
          ir_code_data_4[1] = *(payload + 1);
          ir_code_data_4[2] = *(payload + 2);
          ir_code_data_4[3] = 0;
          ir_code_data_4[4] = 0;
          log_i("Code 3 has ended, ready for command 4, seq %d:%d", ir_code_data_4[1], ir_code_data_4[1]);
          zbGateway.sendCustomClusterCmd(&device, ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 4, ESP_ZB_ZCL_ATTR_TYPE_SET, 5, ir_code_data_4, true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
                                       1, 1, 0x1002); 
        }
      }
    } break;
    case 4: {
      ir_code_data_5[0] = *(payload + 1);
      ir_code_data_5[1] = *(payload + 2);
      ir_code_data_5[2] = 0;
      ir_code_data_4[3] = 0;
      zbGateway.sendCustomClusterCmd(&device, 
                                     ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER, 
                                     5, 
                                     ESP_ZB_ZCL_ATTR_TYPE_SET, 
                                     4, 
                                     ir_code_data_5, true, 
                                     ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
                                     1, 1, 0x1002); 
    } break;
    case 5: {
      ir_code_learning = false;
      ir_code_receiving = false;
      for (int i = 0; i <= ir_message_length + 1; i++)
        log_i("learned_ir_code[%u] = 0x%x", i, learned_ir_code[i]);
      learned_ir_code_base64_length = base64::encodeLength(ir_message_length);
      base64::encode(learned_ir_code + 1, ir_message_length, learned_ir_code_base64);
      log_i("IR code base64 encoded(size 0x%d) = %s", learned_ir_code_base64_length, learned_ir_code_base64);
    } break;
  }
}
