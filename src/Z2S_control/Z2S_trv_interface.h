/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_
#define SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_

#include <supla/element.h>
#include <supla/actions.h>
#include <supla/action_handler.h>
#include <supla/control/remote_output_interface.h>
#include "ZigbeeGateway.h"
#include "TuyaDatapoints.h"
#include "hvac_base_ee.h"
#include <Z2S_sensor/Z2S_virtual_therm_hygro_meter.h>

#define FIRST_0XEF00_CMD_SET 0x01

#define SASWELL_CMD_SET   0x01 

#define SASWELL_CMD_ON_1  0x65
#define SASWELL_CMD_ON_2  0x01
#define SASWELL_CMD_ON_3  0x00
#define SASWELL_CMD_ON_4  0x01
#define SASWELL_CMD_ON_5  0x01

#define SASWELL_CMD_OFF_1 0x65
#define SASWELL_CMD_OFF_2 0x01
#define SASWELL_CMD_OFF_3 0x00
#define SASWELL_CMD_OFF_4 0x01
#define SASWELL_CMD_OFF_5 0x00

#define SASWELL_CMD_SET_TARGET_HEATSETPOINT_1 0x67
#define SASWELL_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define SASWELL_CMD_SET_TEMPERATURE_CALIBRATION_1 0x1B
#define SASWELL_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define SASWELL_CMD_SET_LOCAL_TEMPERATURE_1       0x66
#define SASWELL_CMD_SET_LOCAL_TEMPERATURE_2       0x02

#define SASWELL_CMD_SET_RUNNING_STATE_1           0x03
#define SASWELL_CMD_SET_RUNNING_STATE_IDLE        0x00
#define SASWELL_CMD_SET_RUNNING_STATE_HEAT        0x01

#define SASWELL_CMD_SET_LOW_BATTERY_1             0x69 //05 bitmap type

#define SASWELL_CMD_SET_SCHEDULE_MODE_1           0x6C
#define SASWELL_CMD_SET_SCHEDULE_MODE_ON          0x01
#define SASWELL_CMD_SET_SCHEDULE_MODE_OFF         0x00

#define SASWELL_CMD_SET_CHILD_LOCK_1              0x28
#define SASWELL_CMD_SET_WINDOW_DETECT_1           0x08
#define SASWELL_CMD_SET_ANTI_FREEZE_1             0x0A
#define SASWELL_CMD_SET_LIMESCALE_PROTECT_1       0x82

#define SASWELL_TARGET_HEATSETPOINT_FACTOR        0x0A //*10
#define SASWELL_LOCAL_TEMPERATURE_FACTOR          0x0A //*10
#define SASWELL_TEMPERATURE_CALIBRATION_FACTOR    0x01 //*1

#define SASWELL_CMD_SET_HEATSETPOINT_MIN          0x01F4 //500
#define SASWELL_CMD_SET_HEATSETPOINT_MAX          0x0BB8 //3000

/*---------------------------------------------------------------------------------------------------------------------------*/

#define ME167_CMD_SET   0x02

#define ME167_CMD_ON_1  0x02
#define ME167_CMD_ON_2  0x04
#define ME167_CMD_ON_3  0x00
#define ME167_CMD_ON_4  0x01
#define ME167_CMD_ON_5  0x01

#define ME167_CMD_OFF_1 0x02
#define ME167_CMD_OFF_2 0x04
#define ME167_CMD_OFF_3 0x00
#define ME167_CMD_OFF_4 0x01
#define ME167_CMD_OFF_5 0x02

#define ME167_CMD_SET_TARGET_HEATSETPOINT_1 0x04
#define ME167_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define ME167_CMD_SET_TEMPERATURE_CALIBRATION_1 0x2F
#define ME167_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define ME167_CMD_SET_LOCAL_TEMPERATURE_1 0x05
#define ME167_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define ME167_CMD_SET_RUNNING_STATE_1     0x03
#define ME167_CMD_SET_RUNNING_STATE_IDLE  0x01
#define ME167_CMD_SET_RUNNING_STATE_HEAT  0x00 //inverted

#define ME167_CMD_SET_BATTERY_LEVEL_1           0x06
#define ME167_CMD_SET_LOW_BATTERY_1             0x23 //05 bitmap 0 OK 1 empty

#define ME167_CMD_SET_SCHEDULE_MODE_1           0x02
#define ME167_CMD_SET_SCHEDULE_MODE_ON          0x00
#define ME167_CMD_SET_SCHEDULE_MODE_OFF         0x01

#define ME167_CMD_SET_CHILD_LOCK_1              0x07
#define ME167_CMD_SET_WINDOW_DETECT_1           0x0E //to verify
#define ME167_CMD_SET_ANTI_FREEZE_1             0x24
#define ME167_CMD_SET_LIMESCALE_PROTECT_1       0x27


#define ME167_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define ME167_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define ME167_TEMPERATURE_CALIBRATION_FACTOR 0x01 //*1

#define ME167_STATE_1 0x03
#define ME167_STATE_2 0x04

#define ME167_STATE_IDLE    0x01
#define ME167_STATE_HEATING 0x00

#define ME167_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define ME167_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500

/*---------------------------------------------------------------------------------------------------------------------------*/

#define TRV603_CMD_SET   0x03

#define TRV603_CMD_ON_1  0x02
#define TRV603_CMD_ON_2  0x04
#define TRV603_CMD_ON_3  0x00
#define TRV603_CMD_ON_4  0x01
#define TRV603_CMD_ON_5  0x02

/*#define TRV603_CMD_OFF_1 0x02
#define TRV603_CMD_OFF_2 0x04
#define TRV603_CMD_OFF_3 0x00
#define TRV603_CMD_OFF_4 0x01
#define TRV603_CMD_OFF_5 0x00 */ // OFF = AUTO

#define TRV603_CMD_OFF_1 0x71
#define TRV603_CMD_OFF_2 0x01
#define TRV603_CMD_OFF_3 0x00
#define TRV603_CMD_OFF_4 0x01
#define TRV603_CMD_OFF_5 0x00 //OFF = STOP HEATING


#define TRV603_CMD_SET_TARGET_HEATSETPOINT_1 0x04
#define TRV603_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define TRV603_CMD_SET_TEMPERATURE_CALIBRATION_1 0x2F
#define TRV603_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define TRV603_CMD_SET_LOCAL_TEMPERATURE_1 0x05
#define TRV603_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define TRV603_CMD_SET_RUNNING_STATE_1     0x03
#define TRV603_CMD_SET_RUNNING_STATE_IDLE  0x01
#define TRV603_CMD_SET_RUNNING_STATE_HEAT  0x00 //inverted

#define TRV603_CMD_SET_LOW_BATTERY_1       0x23 //05 bitmap 0 OK 1 empty

#define TRV603_CMD_SET_SCHEDULE_MODE_1           0x02
#define TRV603_CMD_SET_SCHEDULE_MODE_ON          0x00
#define TRV603_CMD_SET_SCHEDULE_MODE_OFF         0x02

#define TRV603_CMD_SET_BATTERY_LEVEL_1           0x06
#define TRV603_CMD_SET_CHILD_LOCK_1              0x07
#define TRV603_CMD_SET_WINDOW_DETECT_1           0x0E //to verify
#define TRV603_CMD_SET_ANTI_FREEZE_1             0x24
#define TRV603_CMD_SET_LIMESCALE_PROTECT_1       0x27


#define TRV603_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define TRV603_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define TRV603_TEMPERATURE_CALIBRATION_FACTOR 0x0A //*10

#define TRV603_STATE_1 0x03
#define TRV603_STATE_2 0x04

#define TRV603_STATE_IDLE    0x01
#define TRV603_STATE_HEATING 0x00

#define TRV603_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define TRV603_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500

/*---------------------------------------------------------------------------------------------------------------------------*/

#define BECA_CMD_SET   0x04

#define BECA_CMD_ON_1  0x01
#define BECA_CMD_ON_2  0x04
#define BECA_CMD_ON_3  0x00
#define BECA_CMD_ON_4  0x01
#define BECA_CMD_ON_5  0x01

#define BECA_CMD_OFF_1 0x07
#define BECA_CMD_OFF_2 0x04
#define BECA_CMD_OFF_3 0x00
#define BECA_CMD_OFF_4 0x01
#define BECA_CMD_OFF_5 0x00

#define BECA_CMD_SET_TARGET_HEATSETPOINT_1 0x02
#define BECA_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define BECA_CMD_SET_TEMPERATURE_CALIBRATION_1 0x69
#define BECA_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define BECA_CMD_SET_LOCAL_TEMPERATURE_1 0x03
#define BECA_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define BECA_CMD_SET_RUNNING_STATE_1     0x07
#define BECA_CMD_SET_RUNNING_STATE_IDLE  0x00
#define BECA_CMD_SET_RUNNING_STATE_HEAT  0x01 //valve state

#define BECA_CMD_SET_BATTERY_LEVEL_1     0x0E // 02 value 0-100

#define BECA_CMD_SET_SCHEDULE_MODE_1           0x01
#define BECA_CMD_SET_SCHEDULE_MODE_ON          0x00
#define BECA_CMD_SET_SCHEDULE_MODE_OFF         0x01

#define BECA_CMD_SET_CHILD_LOCK_1              0x0D
#define BECA_CMD_SET_WINDOW_DETECT_1           0x08 //09?
//#define BECA_CMD_SET_ANTI_FREEZE_1             0x??
//#define BECA_CMD_SET_LIMESCALE_PROTECT_1       0x??


#define BECA_TARGET_HEATSETPOINT_FACTOR     0x01 //*1
#define BECA_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define BECA_TEMPERATURE_CALIBRATION_FACTOR 0x01 //*1

#define BECA_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define BECA_CMD_SET_HEATSETPOINT_MAX    0x0BB8 //3000


/*---------------------------------------------------------------------------------------------------------------------------*/

#define MOES_CMD_SET   0x05

#define MOES_CMD_ON_1  0x6A
#define MOES_CMD_ON_2  0x04
#define MOES_CMD_ON_3  0x00
#define MOES_CMD_ON_4  0x01
#define MOES_CMD_ON_5  0x00

#define MOES_CMD_OFF_1 0x6A
#define MOES_CMD_OFF_2 0x04
#define MOES_CMD_OFF_3 0x00
#define MOES_CMD_OFF_4 0x01
#define MOES_CMD_OFF_5 0x02

#define MOES_CMD_SET_TARGET_HEATSETPOINT_1 0x02
#define MOES_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define MOES_CMD_SET_TEMPERATURE_CALIBRATION_1 0x2C
#define MOES_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define MOES_CMD_SET_LOCAL_TEMPERATURE_1 0x03
#define MOES_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define MOES_CMD_SET_RUNNING_STATE_1     0x6D
#define MOES_CMD_SET_RUNNING_STATE_IDLE  0x00
#define MOES_CMD_SET_RUNNING_STATE_HEAT  0x64 //valve state

#define MOES_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define MOES_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500

#define MOES_CMD_SET_LOW_BATTERY_1       0x6E //
#define MOES_CMD_SET_BATTERY_LEVEL_1     0x15 //TO VERIFY

#define MOES_CMD_SET_SCHEDULE_MODE_1           0x04
#define MOES_CMD_SET_SCHEDULE_MODE_ON          0x01
#define MOES_CMD_SET_SCHEDULE_MODE_OFF         0x02

#define MOES_CMD_SET_CHILD_LOCK_1              0x07
#define MOES_CMD_SET_WINDOW_DETECT_1           0x68 //00 RAW [0,35,5] on/off, temperature, operating time (min)
//#define MOES_CMD_SET_ANTI_FREEZE_1             0x??
//#define MOES_CMD_SET_LIMESCALE_PROTECT_1       0x??

#define MOES_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define MOES_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define MOES_TEMPERATURE_CALIBRATION_FACTOR 0x0A //*10

/*---------------------------------------------------------------------------------------------------------------------------*/

#define TRV601_CMD_SET   0x06

#define TRV601_CMD_ON_1  0x01
#define TRV601_CMD_ON_2  0x04
#define TRV601_CMD_ON_3  0x00
#define TRV601_CMD_ON_4  0x01
#define TRV601_CMD_ON_5  0x01

#define TRV601_CMD_OFF_1 0x01  
#define TRV601_CMD_OFF_2 0x04
#define TRV601_CMD_OFF_3 0x00
#define TRV601_CMD_OFF_4 0x01
#define TRV601_CMD_OFF_5 0x02

#define TRV601_CMD_SET_TARGET_HEATSETPOINT_1 0x02
#define TRV601_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define TRV601_CMD_SET_TEMPERATURE_CALIBRATION_1 0x65
#define TRV601_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define TRV601_CMD_SET_LOCAL_TEMPERATURE_1 0x03
#define TRV601_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define TRV601_CMD_SET_RUNNING_STATE_1     0x06
#define TRV601_CMD_SET_RUNNING_STATE_IDLE  0x00
#define TRV601_CMD_SET_RUNNING_STATE_HEAT  0x01 //normal

#define TRV601_CMD_SET_BATTERY_LEVEL_1     0x0D //05 bitmap 0 OK 1 empty

#define TRV601_CMD_SET_SCHEDULE_MODE_1           0x01
#define TRV601_CMD_SET_SCHEDULE_MODE_ON          0x00
#define TRV601_CMD_SET_SCHEDULE_MODE_OFF         0x01

#define TRV601_CMD_SET_CHILD_LOCK_1              0x0C
#define TRV601_CMD_SET_WINDOW_DETECT_1           0x08 //7 open/close
//#define TRV601_CMD_SET_ANTI_FREEZE_1             0x??
//#define TRV601_CMD_SET_LIMESCALE_PROTECT_1       0x??


#define TRV601_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define TRV601_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define TRV601_TEMPERATURE_CALIBRATION_FACTOR 0x0A //*10

#define TRV601_STATE_1 0x06
#define TRV601_STATE_2 0x04

#define TRV601_STATE_IDLE    0x00
#define TRV601_STATE_HEATING 0x01

#define TRV601_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define TRV601_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500

/*---------------------------------------------------------------------------------------------------------------------------*/

#define GTZ10_CMD_SET   0x07

#define GTZ10_CMD_ON_1  0x02
#define GTZ10_CMD_ON_2  0x04
#define GTZ10_CMD_ON_3  0x00
#define GTZ10_CMD_ON_4  0x01
#define GTZ10_CMD_ON_5  0x00

#define GTZ10_CMD_OFF_1 0x02
#define GTZ10_CMD_OFF_2 0x04
#define GTZ10_CMD_OFF_3 0x00
#define GTZ10_CMD_OFF_4 0x01
#define GTZ10_CMD_OFF_5 0x05

#define GTZ10_CMD_SET_TARGET_HEATSETPOINT_1 0x04
#define GTZ10_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define GTZ10_CMD_SET_TEMPERATURE_CALIBRATION_1 0x2F
#define GTZ10_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define GTZ10_CMD_SET_LOCAL_TEMPERATURE_1 0x05
#define GTZ10_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define GTZ10_CMD_SET_RUNNING_STATE_1     0x31
#define GTZ10_CMD_SET_RUNNING_STATE_IDLE  0x00
#define GTZ10_CMD_SET_RUNNING_STATE_HEAT  0x01

#define GTZ10_CMD_SET_BATTERY_LEVEL_1       0x06 //

#define GTZ10_CMD_SET_SCHEDULE_MODE_1           0x02
#define GTZ10_CMD_SET_SCHEDULE_MODE_ON          0x01
#define GTZ10_CMD_SET_SCHEDULE_MODE_OFF         0x00

#define GTZ10_CMD_SET_CHILD_LOCK_1              0x07
#define GTZ10_CMD_SET_WINDOW_DETECT_1           0x0E //0x0F open/close
#define GTZ10_CMD_SET_ANTI_FREEZE_1             0x24
#define GTZ10_CMD_SET_LIMESCALE_PROTECT_1       0x27


#define GTZ10_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define GTZ10_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define GTZ10_TEMPERATURE_CALIBRATION_FACTOR 0x0A //*10

#define GTZ10_STATE_1 0x03
#define GTZ10_STATE_2 0x04

#define GTZ10_STATE_IDLE    0x00
#define GTZ10_STATE_HEATING 0x01

#define GTZ10_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define GTZ10_CMD_SET_HEATSETPOINT_MAX    0x0FA0 //4000

/*---------------------------------------------------------------------------------------------------------------------------*/

#define TRV602Z_CMD_SET   0x08 //reserved but currently not used - GTZ10_CMD_SET is used insetead, since only RUNNING_STATE is different

#define TRV602Z_CMD_SET_RUNNING_STATE_1     0x03

#define TRV602Z_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500

/*---------------------------------------------------------------------------------------------------------------------------*/

#define TV02_CMD_SET   0x09

#define TV02_CMD_ON_1  0x02
#define TV02_CMD_ON_2  0x04
#define TV02_CMD_ON_3  0x00
#define TV02_CMD_ON_4  0x01
#define TV02_CMD_ON_5  0x01

#define TV02_CMD_OFF_1 0x6B //HEATING STOP
#define TV02_CMD_OFF_2 0x01
#define TV02_CMD_OFF_3 0x00
#define TV02_CMD_OFF_4 0x01
#define TV02_CMD_OFF_5 0x01

#define TV02_CMD_SET_TARGET_HEATSETPOINT_1 0x10
#define TV02_CMD_SET_TARGET_HEATSETPOINT_2 0x02

#define TV02_CMD_SET_TEMPERATURE_CALIBRATION_1 0x1B
#define TV02_CMD_SET_TEMPERATURE_CALIBRATION_2 0x02

#define TV02_CMD_SET_LOCAL_TEMPERATURE_1 0x18
#define TV02_CMD_SET_LOCAL_TEMPERATURE_2 0x02

#define TV02_CMD_SET_RUNNING_STATE_1     0x6B
#define TV02_CMD_SET_RUNNING_STATE_IDLE  0x01
#define TV02_CMD_SET_RUNNING_STATE_HEAT  0x00

//#define TV02_CMD_SET_BATTERY_LEVEL_1       0x06 //
#define TV02_CMD_SET_LOW_BATTERY_1           0x23 //

#define TV02_CMD_SET_SCHEDULE_MODE_1           0x02
#define TV02_CMD_SET_SCHEDULE_MODE_ON          0x00
#define TV02_CMD_SET_SCHEDULE_MODE_OFF         0x01

#define TV02_CMD_SET_CHILD_LOCK_1              0x28
#define TV02_CMD_SET_WINDOW_DETECT_1           0x08 //0x0F open/close
#define TV02_CMD_SET_ANTI_FREEZE_1             0x0A //ON , OFF -0x02/1 manual
#define TV02_CMD_SET_LIMESCALE_PROTECT_1       0x27


#define TV02_TARGET_HEATSETPOINT_FACTOR     0x0A //*10
#define TV02_LOCAL_TEMPERATURE_FACTOR       0x0A //*10
#define TV02_TEMPERATURE_CALIBRATION_FACTOR 0x0A //*10

#define TV02_STATE_1 0x03
#define TV02_STATE_2 0x04

#define TV02_STATE_IDLE    0x01
#define TV02_STATE_HEATING 0x00

#define TV02_CMD_SET_HEATSETPOINT_MIN    0x01F4 //500
#define TV02_CMD_SET_HEATSETPOINT_MAX    0x0BB8 //3000


/*---------------------------------------------------------------------------------------------------------------------------*/

#define LAST_0XEF00_CMD_SET 0x3F

/*---------------------------------------------------------------------------------------------------------------------------*/

#define TRVZB_CMD_SET 0x40 //1-63 reserved for Tuya/Saswell/Moes and other TS0601 variants

#define TRVZB_CMD_SET_CHILD_LOCK                    0x0000 //BOOL
#define TRVZB_CMD_SET_TAMPER                        0x2000 //U8
#define TRVZB_CMD_SET_ILLUMINATION                  0x2001 //U8
#define TRVZB_CMD_SET_OPEN_WINDOW                   0x6000 //BOOL
#define TRVZB_CMD_SET_FROST_PROTECTION_TEMPERATURE  0x6002 //INT16
#define TRVZB_CMD_SET_IDLE_STEPS                    0x6003 //U16                    
#define TRVZB_CMD_SET_CLOSING_STEPS                 0x6004 //U16
#define TRVZB_CMD_SET_VALVE_OPENING_LIMIT_VOLTAGE   0x6005  //U16
#define TRVZB_CMD_SET_VALVE_CLOSING_LIMIT_VOLTAGE   0x6006 //U16
#define TRVZB_CMD_SET_VALVE_MOTOR_RUNNING_VOLTAGE   0x6007 //U16
#define TRVZB_CMD_SET_VALVE_OPENING_DEGREE          0x600B //U8
#define TRVZB_CMD_SET_VALVE_CLOSING_DEGREE          0x600C  //U8
#define TRVZB_CMD_SET_EXTERNAL_TEMPERATURE_INPUT    0x600D //INT16
#define TRVZB_CMD_SET_TEMPERATURE_SENSOR_SELECT     0x600E //U8

//#define TRVZB_CMD_SET_SCHEDULE_MODE_1           0x02 THERMOSTAT CLUSTER
//#define MEVZB_CMD_SET_SCHEDULE_MODE_ON          0x00

#define TRVZB_CMD_SET_HEATSETPOINT_MIN    0x0190 //400
#define TRVZB_CMD_SET_HEATSETPOINT_MAX    0x0DAC //3500
                    
/*---------------------------------------------------------------------------------------------------------------------------*/

#define EXTERNAL_TEMPERATURE_SENSOR_IGNORE          0x0000
#define EXTERNAL_TEMPERATURE_SENSOR_USE_CALIBRATE   0x0001
#define EXTERNAL_TEMPERATURE_SENSOR_USE_INPUT       0x0002

typedef struct ts0601_command_set_s {

  uint32_t ts0601_cmd_set_id;
  
  uint8_t ts0601_cmd_on_dp_id;
  uint8_t ts0601_cmd_on_dp_type;
  uint8_t ts0601_cmd_on_dp_value_on;

  uint8_t ts0601_cmd_off_dp_id;
  uint8_t ts0601_cmd_off_dp_type;
  uint8_t ts0601_cmd_off_dp_value_off;
  
  uint8_t ts0601_cmd_set_target_heatsetpoint_dp_id;
  uint8_t ts0601_cmd_set_target_heatsetpoint_dp_type;
  uint8_t ts0601_cmd_set_temperature_calibration_dp_id;
  uint8_t ts0601_cmd_set_temperature_calibration_dp_type;

  uint8_t ts0601_cmd_set_local_temperature_dp_id;
  uint8_t ts0601_cmd_set_local_temperature_dp_type;

  uint8_t ts0601_cmd_set_running_state_dp_id;
  uint8_t ts0601_cmd_set_running_state_dp_type;
  uint8_t ts0601_cmd_set_running_state_dp_value_idle;
  uint8_t ts0601_cmd_set_running_state_dp_value_heat;

  uint8_t ts0601_cmd_set_schedule_mode_dp_id;
  uint8_t ts0601_cmd_set_schedule_mode_dp_type;
  uint8_t ts0601_cmd_set_schedule_mode_dp_value_on;
  uint8_t ts0601_cmd_set_schedule_mode_dp_value_off;

  uint8_t ts0601_cmd_set_child_lock_dp_id;
  uint8_t ts0601_cmd_set_child_lock_dp_type;
  uint8_t ts0601_cmd_set_child_lock_dp_value_on;
  uint8_t ts0601_cmd_set_child_lock_dp_value_off;

  uint8_t ts0601_cmd_set_winodow_detect_dp_id;
  uint8_t ts0601_cmd_set_window_detect_dp_type;
  uint8_t ts0601_cmd_set_window_detect_dp_value_on;
  uint8_t ts0601_cmd_set_window_detect_dp_value_off;
  
  uint8_t ts0601_cmd_set_anti_freeze_protect_dp_id;
  uint8_t ts0601_cmd_set_anti_freeze_protect_dp_type;
  uint8_t ts0601_cmd_set_anti_freeze_protect_dp_value_on;
  uint8_t ts0601_cmd_set_anti_freeze_protect_dp_value_off;

  uint8_t ts0601_cmd_set_limescale_protect_dp_id;
  uint8_t ts0601_cmd_set_limescale_protect_dp_type;
  uint8_t ts0601_cmd_set_limescale_protect_dp_value_on;
  uint8_t ts0601_cmd_set_limescale_protect_dp_value_off;

  uint8_t ts0601_cmd_set_battery_level_dp_id;
  uint8_t ts0601_cmd_set_battery_level_dp_type;
  uint8_t ts0601_cmd_set_low_battery_dp_id;
  uint8_t ts0601_cmd_set_low_battery_dp_type;

  uint8_t ts0601_cmd_set_target_heatsetpoint_factor;
  uint8_t ts0601_cmd_set_local_temperature_factor;
  uint8_t ts0601_cmd_set_temperature_calibration_factor;

  uint16_t ts0601_cmd_set_target_heatsetpoint_min;
  uint16_t ts0601_cmd_set_target_heatsetpoint_max;

} ts0601_command_set_t;

enum ts0601_cmd_sets {

  saswell_cmd_set,
  me167_cmd_set,
  trv603_cmd_set,
  beca_cmd_set,
  moes_cmd_set,
  trv601_cmd_set,
  gtz10_cmd_set,
  trv602z_cmd_set,
  tv02_cmd_set,
  siterwell_cmd_set,
  ts0601_cmd_sets_number

};

static constexpr ts0601_command_set_t ts0601_command_sets_table[] PROGMEM = {

  { .ts0601_cmd_set_id                               =  saswell_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x65,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_on_dp_value_on                       =  0x01,

    .ts0601_cmd_off_dp_id                            =  0x65,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_off_dp_value_off                     =  0x00,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x67,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x1B,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x66,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x03,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x00,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x01,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x6C,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x00,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x28,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x08,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x0A,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x82,
    .ts0601_cmd_set_limescale_protect_dp_type        =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x00,
    .ts0601_cmd_set_battery_level_dp_type            =  0xFF,
    .ts0601_cmd_set_low_battery_dp_id                =  0x69,
    .ts0601_cmd_set_low_battery_dp_type              =  TUYA_DP_TYPE_BITMAP,

    
    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x01,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0BB8 }, //3000

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  me167_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x02,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x01,

    .ts0601_cmd_off_dp_id                            =  0x02,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x02,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x04,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x2F,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x05,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x03,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x01,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x00,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x02,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x00,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x01,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x07,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x0E,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x24,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x27,
    .ts0601_cmd_set_limescale_protect_dp_type        =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x06,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x23,
    .ts0601_cmd_set_low_battery_dp_type              =  TUYA_DP_TYPE_BITMAP,

    
    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x01,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0DAC }, //3500

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  trv603_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x02,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x02,

    .ts0601_cmd_off_dp_id                            =  0x71,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_off_dp_value_off                     =  0x00,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x04,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x2F,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x05,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x03,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x01,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x00,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x02,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x00,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x02,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x07,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x0E,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x24,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x27,
    .ts0601_cmd_set_limescale_protect_dp_type        =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x06,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x23,
    .ts0601_cmd_set_low_battery_dp_type              =  TUYA_DP_TYPE_BITMAP,

    
    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0DAC }, //3500

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  beca_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x01,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x01,

    .ts0601_cmd_off_dp_id                            =  0x07,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x00,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x02,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x69,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x03,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x07,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x00,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x01,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x00,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x01,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x0D,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x08,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x00,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  0xFF,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x00,
    .ts0601_cmd_set_limescale_protect_dp_type        =  0xFF,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x0E,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x00,
    .ts0601_cmd_set_low_battery_dp_type              =  0xFF,

    
    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x01,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x01,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0BB8 }, //3000

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  moes_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x6A,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x00,

    .ts0601_cmd_off_dp_id                            =  0x6A,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x02,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x02,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x2C,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x03,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x6D,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x00,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x64,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x04,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x02,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x07,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x68,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_RAW, ////00 RAW [0,35,5] on/off, temperature, operating time (min)
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x00,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x00,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  0xFF,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x00,
    .ts0601_cmd_set_limescale_protect_dp_type        =  0xFF,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x15,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x6E,
    .ts0601_cmd_set_low_battery_dp_type              =  TUYA_DP_TYPE_BITMAP,

    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0DAC }, //3500

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  trv601_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x01,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x01,

    .ts0601_cmd_off_dp_id                            =  0x01,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x02,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x02,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x65,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x03,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x06,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x00,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x01,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x00,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x01,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x0C,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x08,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL, //7 open/close
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x00,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  0xFF,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x00,
    .ts0601_cmd_set_limescale_protect_dp_type        =  0xFF,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x00,
    .ts0601_cmd_set_battery_level_dp_type            =  0xFF,
    .ts0601_cmd_set_low_battery_dp_id                =  0x0D,
    .ts0601_cmd_set_low_battery_dp_type              =  TUYA_DP_TYPE_BITMAP,

    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0DAC }, //3500

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  gtz10_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x02,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x00,

    .ts0601_cmd_off_dp_id                            =  0x02,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x05,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x04,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x2F,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x05,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x31,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x00,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x01,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x02,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x00,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x07,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x0E,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL, //0x0F open/close
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x24,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x27,
    .ts0601_cmd_set_limescale_protect_dp_type        =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x06,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x00,
    .ts0601_cmd_set_low_battery_dp_type              =  0xFF,

    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0FA0 }, //4000

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  trv602z_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x02,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x00,

    .ts0601_cmd_off_dp_id                            =  0x02,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x05,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x04,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x2F,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x05,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x03,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x00,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x01,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x02,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x00,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x07,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x0E,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL, //0x0F open/close
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x24,
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x27,
    .ts0601_cmd_set_limescale_protect_dp_type        =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x06,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x00,
    .ts0601_cmd_set_low_battery_dp_type              =  0xFF,

    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0DAC }, //3500

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  tv02_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x02,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x01,

    .ts0601_cmd_off_dp_id                            =  0x6B,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_off_dp_value_off                     =  0x01,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x10,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x1B,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x18,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x6B,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x01,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x00,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x02,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x00,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x01,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x28,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x08,
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL, //0x0F open/close
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x0A, //ON , OFF -0x02/1 manual
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x27,
    .ts0601_cmd_set_limescale_protect_dp_type        =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x00,
    .ts0601_cmd_set_battery_level_dp_type            =  0xFF,
    .ts0601_cmd_set_low_battery_dp_id                =  0x23,
    .ts0601_cmd_set_low_battery_dp_type              =  TUYA_DP_TYPE_BITMAP,

    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0BB8 }, //3000

/*---------------------------------------------------------------------------------------*/

  { .ts0601_cmd_set_id                               =  siterwell_cmd_set,
    
    .ts0601_cmd_on_dp_id                             =  0x04,
    .ts0601_cmd_on_dp_type                           =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_on_dp_value_on                       =  0x02,

    .ts0601_cmd_off_dp_id                            =  0x04,
    .ts0601_cmd_off_dp_type                          =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_off_dp_value_off                     =  0x00,
    
    .ts0601_cmd_set_target_heatsetpoint_dp_id        =  0x02,
    .ts0601_cmd_set_target_heatsetpoint_dp_type      =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_temperature_calibration_dp_id    =  0x2C,
    .ts0601_cmd_set_temperature_calibration_dp_type  =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_local_temperature_dp_id          =  0x03,
    .ts0601_cmd_set_local_temperature_dp_type        =  TUYA_DP_TYPE_VALUE,

    .ts0601_cmd_set_running_state_dp_id              =  0x0E,
    .ts0601_cmd_set_running_state_dp_type            =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_running_state_dp_value_idle      =  0x01,
    .ts0601_cmd_set_running_state_dp_value_heat      =  0x00,

    .ts0601_cmd_set_schedule_mode_dp_id              =  0x04,
    .ts0601_cmd_set_schedule_mode_dp_type            =  TUYA_DP_TYPE_ENUM,
    .ts0601_cmd_set_schedule_mode_dp_value_on        =  0x01,
    .ts0601_cmd_set_schedule_mode_dp_value_off       =  0x02,

    .ts0601_cmd_set_child_lock_dp_id                 =  0x07,
    .ts0601_cmd_set_child_lock_dp_type               =  TUYA_DP_TYPE_BOOL,
    .ts0601_cmd_set_child_lock_dp_value_on           =  0x01,
    .ts0601_cmd_set_child_lock_dp_value_off          =  0x00,

    .ts0601_cmd_set_winodow_detect_dp_id             =  0x12,// ALARM 0x11 ENUM
    .ts0601_cmd_set_window_detect_dp_type            =  TUYA_DP_TYPE_BOOL, 
    .ts0601_cmd_set_window_detect_dp_value_on        =  0x01,
    .ts0601_cmd_set_window_detect_dp_value_off       =  0x00,
    
    .ts0601_cmd_set_anti_freeze_protect_dp_id        =  0x00, 
    .ts0601_cmd_set_anti_freeze_protect_dp_type      =  0xFF,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_on  =  0x01,
    .ts0601_cmd_set_anti_freeze_protect_dp_value_off =  0x00,

    .ts0601_cmd_set_limescale_protect_dp_id          =  0x00,
    .ts0601_cmd_set_limescale_protect_dp_type        =  0xFF,
    .ts0601_cmd_set_limescale_protect_dp_value_on    =  0x01,
    .ts0601_cmd_set_limescale_protect_dp_value_off   =  0x00,

    .ts0601_cmd_set_battery_level_dp_id              =  0x15,
    .ts0601_cmd_set_battery_level_dp_type            =  TUYA_DP_TYPE_VALUE,
    .ts0601_cmd_set_low_battery_dp_id                =  0x00,
    .ts0601_cmd_set_low_battery_dp_type              =  0xFF,

    .ts0601_cmd_set_target_heatsetpoint_factor       =  0x0A,
    .ts0601_cmd_set_local_temperature_factor         =  0x0A,
    .ts0601_cmd_set_temperature_calibration_factor   =  0x0A,
    

    .ts0601_cmd_set_target_heatsetpoint_min          =  0x01F4, //500
    .ts0601_cmd_set_target_heatsetpoint_max          =  0x0BB8 }, //3000
};

namespace Supla {
namespace Control {
class Z2S_TRVInterface : public RemoteOutputInterface, public ActionHandler, public Element {
 public:
  Z2S_TRVInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_commands_set);

  Supla::Control::HvacBaseEE *getTRVHvac();
  void setTRVHvac(Supla::Control::HvacBaseEE *trv_hvac);

  void setTemperatureCalibrationOffsetTrigger(int32_t temperature_calibration_offset_trigger);
  void setTemperatureCalibrationUpdateMs(uint32_t temperature_calibration_update_ms);

  void enableExternalSensorDetection(bool enable_external_sensor_detection, uint8_t external_sensor_mode, uint8_t internal_sensor_channel);

  void setTRVTemperatureSetpoint(int32_t trv_temperature_setpoint);
  void setTRVSystemMode(uint8_t trv_system_mode);
  void setTRVRunningState(uint8_t trv_running_state);
  void setTRVLocalTemperature(int32_t trv_local_temperature);
  void setTRVTemperatureCalibration(int32_t trv_temperature_calibration);
  void setTRVChildLock(uint8_t trv_child_lock);
  void turnOffTRVScheduleMode();

  void setTimeoutSecs(uint32_t timeout_secs);
  void refreshTimeout();

  void iterateAlways() override;
  void handleAction(int event, int action) override;


protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t _device;
  //uint8_t _Tuya_dp_data[10];
  uint8_t _trv_commands_set;

  HvacBaseEE *_trv_hvac = nullptr;

  uint8_t _trv_system_mode         = 0;
  bool    _trv_system_mode_updated = false;

  uint8_t _trv_running_state         = 0;
  bool    _trv_running_state_updated = false;

  int32_t _trv_temperature_setpoint         = 0;
  bool    _trv_temperature_setpoint_updated = false;

  int32_t _trv_local_temperature         = INT32_MIN;
  int32_t _trv_last_local_temperature    = INT32_MIN;
  bool    _trv_local_temperature_updated = false;

  int32_t _trv_external_sensor_temperature    = INT32_MIN;
  bool _trv_external_sensor_detection_enabled = false;
  bool _trv_external_sensor_present           = false;
  bool _trv_external_sensor_changed           = false;       
  bool _trv_external_sensor_status            = 0xFF;
  uint8_t _trv_internal_sensor_channel        = 0xFF;
  uint8_t _trv_external_sensor_mode           = EXTERNAL_TEMPERATURE_SENSOR_IGNORE;

  int32_t _trv_temperature_calibration         = 0;
  int32_t _trv_last_temperature_calibration    = 0;
  bool    _trv_temperature_calibration_updated = false;

  uint8_t _trv_child_lock = 0xFF;
  bool _trv_child_lock_changed = false;

  bool _trv_switch_schedule_off = false;

  int32_t _temperature_calibration_offset      = 0;
  int32_t _last_temperature_calibration_offset = 0;
  
  int32_t _temperature_calibration_offset_trigger = 500;

  uint32_t  _temperature_calibration_update_ms      = 5 * 60 * 1000; //5 minutes
  uint32_t  _temperature_calibration_last_update_ms = 0;

  uint32_t _temperature_ping_ms = 60 * 1000;
  uint32_t _last_temperature_ping_ms = 0;

  uint32_t _thermostat_ping_ms = 20 * 60 * 1000;
  uint32_t _last_thermostat_ping_ms = 0;

  uint32_t  _external_temperature_ping_ms = 30 * 60 * 1000;
  uint32_t  _last_external_temperature_ping_ms = 0;

  uint32_t _refresh_ms      = 5000;
  uint32_t _last_refresh_ms = 0;

  uint32_t  _timeout_ms = 0;
  uint32_t  _last_seen_ms = 0;
  uint32_t  _last_cmd_sent_ms = 0;
  bool _timeout_enabled = false;

  void sendTRVSystemMode(uint8_t trv_system_mode);
  void sendTRVTemperatureSetpoint(int32_t temperature_setpoint);
  void sendTRVTemperatureCalibration(int32_t temperature_calibration);
  void sendTRVExternalSensorTemperature(int32_t external_sensor_temperature);
  void sendTRVExternalSensorInput(bool trv_external_sensor_present);
  void sendTRVChildLock(uint8_t trv_child_lock);
  void sendTRVScheduleMode(uint8_t trv_schedule_mode);
  void sendTRVPing();
};

};  // namespace Control
};  // namespace Supla


#endif //SRC_SUPLA_CONTROL_Z2S_TRV_INTERFACE_H_
