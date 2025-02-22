#ifndef Z2S_DEVICE_TUYA_CUSTOM_CLUSTER_H_
#define Z2S_DEVICE_TUYA_CUSTON_CLUSTER_H_

#include "z2s_devices_table.h"

#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_Tuya_thermostat.h>

#define TUYA_DATA_REPORT_CMD                   0x02

#define TUYA_23457_SYSTEM_MODE_DP              0x02
#define TUYA_23457_SYSTEM_MODE_AUTO            0x00
#define TUYA_23457_SYSTEM_MODE_HEAT            0x01
#define TUYA_23457_SYSTEM_MODE_OFF             0x02

#define TUYA_23457_RUNNING_STATE_DP            0x03
#define TUYA_23457_RUNNING_STATE_HEAT          0x00
#define TUYA_23457_RUNNING_STATE_IDLE          0x01

#define TUYA_23457_CURRENT_HEATING_SETPOINT_DP 0x04 // /10
#define TUYA_23457_LOCAL_TEMPERATURE_DP        0x05 // /10
#define TUYA_23457_CHILD_LOCK_DP               0x07

#define TUYA_6567C_SYSTEM_MODE_DP              0x65 
//#define TUYA_6567C_SYSTEM_MODE_AUTO            0x00 //?
//#define TUYA_6567C_SYSTEM_MODE_HEAT            0x01  //?
//#define TUYA_6567C_SYSTEM_MODE_OFF             0x02  //?

#define TUYA_6567C_RUNNING_STATE_DP            0x03
//#define TUYA_6567C_RUNNING_STATE_HEAT          0x00
//#define TUYA_6567C_RUNNING_STATE_IDLE          0x01

#define TUYA_6567C_CURRENT_HEATING_SETPOINT_DP 0x67 // /10
#define TUYA_6567C_LOCAL_TEMPERATURE_DP        0x66 // /10
#define TUYA_6567C_CHILD_LOCK_DP               0x28
#define TUYA_6567C_AWAY_MODE_DP                0x6A //7 0x1 0x1 0x0/0x1
#define TUYA_6567C_SCHEDULE_ENABLE_DP          0x6B
#define TUYA_6567C_SCHEDULE_SET_DP             0x6C //7 0x1 0x1 0x1
#define TUYA_6567C_LOW_BATTERY_DP              0x69 //7 0x5 0x1 0x0

#define TUYA_LEGACY_SYSTEM_MODE_DP              0x65 
//#define TUYA_6567C_SYSTEM_MODE_AUTO            0x00 //?
//#define TUYA_6567C_SYSTEM_MODE_HEAT            0x01  //?
//#define TUYA_6567C_SYSTEM_MODE_OFF             0x02  //?

#define TUYA_LEGACY_RUNNING_STATE_DP            0x03
//#define TUYA_6567C_RUNNING_STATE_HEAT          0x00
//#define TUYA_6567C_RUNNING_STATE_IDLE          0x01

#define TUYA_LEGACY_CURRENT_HEATING_SETPOINT_DP 0x67 // /10
#define TUYA_LEGACY_LOCAL_TEMPERATURE_DP        0x66 // /10
#define TUYA_LEGACY_CHILD_LOCK_DP               0x28
#define TUYA_LEGACY_AWAY_MODE_DP                0x6A
#define TUYA_LEGACY_SCHEDULE_ENABLE_DP          0x6B
#define TUYA_LEGACY_SCHEDULE_SET_DP             0x6C

#define TUYA_DOUBLE_DIMMER_SWITCH_1_STATE_DP      0x01
#define TUYA_DOUBLE_DIMMER_SWITCH_2_STATE_DP      0x07
#define TUYA_DOUBLE_DIMMER_SWITCH_1_BRIGHTNESS_DP 0x02
#define TUYA_DOUBLE_DIMMER_SWITCH_2_BRIGHTNESS_DP 0x08

#define TUYA_SOIL_SENSOR_TEMPERATURE_DP   0x05
#define TUYA_SOIL_SENSOR_MOISTURE_DP      0x03
#define TUYA_SOIL_SENSOR_BATTERY_LEVEL_DP 0x0F

#define TUYA_TH_SENSOR_TEMPERATURE_DP   0x01
#define TUYA_TH_SENSOR_HUMIDITY_DP      0x02
#define TUYA_TH_SENSOR_BATTERY_STATE_DP 0x03
#define TUYA_TH_SENSOR_BATTERY_LEVEL_DP 0x04

#define TUYA_SMOKE_DETECTOR_SMOKE_DP          0x01
#define TUYA_SMOKE_DETECTOR_PPM_DP            0x02
#define TUYA_SMOKE_DETECTOR_BATTERY_LEVEL_DP  0x0F

#define TUYA_PRESENCE_SENSOR_PRESENCE_DP     0x01
#define TUYA_PRESENCE_SENSOR_MOTION_STATE_DP 0x65
#define TUYA_PRESENCE_SENSOR_ILLUMINANCE_DP  0x6A


typedef struct Tuya_read_dp_result_s {
  bool is_success;
  uint8_t dp_id;
  uint8_t dp_type;
  uint16_t dp_size;
  uint32_t dp_value;
} Tuya_read_dp_result_t;

void processTuyaCustomCluster(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, uint16_t payload_size, uint8_t *payload, signed char rssi);

#endif