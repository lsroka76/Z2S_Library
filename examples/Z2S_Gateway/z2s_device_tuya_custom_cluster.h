#ifndef Z2S_DEVICE_TUYA_CUSTOM_CLUSTER_H_
#define Z2S_DEVICE_TUYA_CUSTON_CLUSTER_H_

#include "z2s_devices_table.h"

#include <Z2S_control/hvac_base_ee.h>


#define TUYA_DATA_REPORT_CMD                   0x02

//#define TUYA_23457_SYSTEM_MODE_DP              0x02
//#define TUYA_23457_SYSTEM_MODE_AUTO            0x00
//#define TUYA_23457_SYSTEM_MODE_HEAT            0x01
//#define TUYA_23457_SYSTEM_MODE_OFF             0x02

//#define TUYA_23457_RUNNING_STATE_DP            0x03
//#define TUYA_23457_RUNNING_STATE_HEAT          0x00
//#define TUYA_23457_RUNNING_STATE_IDLE          0x01

//#define TUYA_23457_CURRENT_HEATING_SETPOINT_DP 0x04 // /10
//#define TUYA_23457_LOCAL_TEMPERATURE_DP        0x05 // /10
//#define TUYA_23457_CHILD_LOCK_DP               0x07

//#define TUYA_6567C_SYSTEM_MODE_DP              0x65 
//#define TUYA_6567C_SYSTEM_MODE_AUTO            0x00 //?
//#define TUYA_6567C_SYSTEM_MODE_HEAT            0x01  //?
//#define TUYA_6567C_SYSTEM_MODE_OFF             0x02  //?

//#define TUYA_6567C_RUNNING_STATE_DP            0x03
//#define TUYA_6567C_RUNNING_STATE_HEAT          0x00
//#define TUYA_6567C_RUNNING_STATE_IDLE          0x01

/*#define TUYA_6567C_CURRENT_HEATING_SETPOINT_DP 0x67 // /10
#define TUYA_6567C_LOCAL_TEMPERATURE_DP        0x66 // /10
#define TUYA_6567C_CHILD_LOCK_DP               0x28
#define TUYA_6567C_AWAY_MODE_DP                0x6A //7 0x1 0x1 0x0/0x1
#define TUYA_6567C_SCHEDULE_ENABLE_DP          0x6B
#define TUYA_6567C_SCHEDULE_SET_DP             0x6C //7 0x1 0x1 0x1
#define TUYA_6567C_LOW_BATTERY_DP              0x69 //7 0x5 0x1 0x0*/

//#define TUYA_LEGACY_SYSTEM_MODE_DP              0x01 
//#define TUYA_6567C_SYSTEM_MODE_AUTO            0x00 //?
//#define TUYA_6567C_SYSTEM_MODE_HEAT            0x01  //?
//#define TUYA_6567C_SYSTEM_MODE_OFF             0x02  //?

//#define TUYA_LEGACY_RUNNING_STATE_DP            0x0E
//#define TUYA_6567C_RUNNING_STATE_HEAT          0x00
//#define TUYA_6567C_RUNNING_STATE_IDLE          0x01

/*#define TUYA_LEGACY_CURRENT_HEATING_SETPOINT_DP 0x02 // /10
#define TUYA_LEGACY_LOCAL_TEMPERATURE_DP        0x03 // /10
#define TUYA_LEGACY_CHILD_LOCK_DP               0x28
#define TUYA_LEGACY_AWAY_MODE_DP                0x6A
#define TUYA_LEGACY_SCHEDULE_ENABLE_DP          0x6B
#define TUYA_LEGACY_SCHEDULE_SET_DP             0x6C*/

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
#define TUYA_SMOKE_DETECTOR_TAMPER_DP         0x04
#define TUYA_SMOKE_DETECTOR_BATTERY_STATE_DP  0x0E
#define TUYA_SMOKE_DETECTOR_BATTERY_LEVEL_DP  0x0F

#define TUYA_CO_DETECTOR_CO_DP               0x01
#define TUYA_CO_DETECTOR_CO_CONC_DP          0x02
#define TUYA_CO_DETECTOR_SELF_TEST_DP        0x09
#define TUYA_CO_DETECTOR_BATTERY_LEVEL_DP    0x0F
#define TUYA_CO_DETECTOR_SILENCE_DP          0x10

#define TUYA_PRESENCE_SENSOR_PRESENCE_DP     0x01
#define TUYA_PRESENCE_SENSOR_MOTION_STATE_DP 0x65
#define TUYA_PRESENCE_SENSOR_ILLUMINANCE_DP  0x6A
#define TUYA_PRESENCE_SENSOR_BATTERY_DP      0x79

#define TUYA_PRESENCE_SENSOR_5_PRESENCE_DP     0x65
#define TUYA_PRESENCE_SENSOR_5_ILLUMINANCE_DP  0x0C

#define TUYA_RAIN_SENSOR_ILLUMINANCE_DP             0x65
#define TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_DP  0x66
#define TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_DP   0x67
#define TUYA_RAIN_SENSOR_RAIN_INTENSITY_DP          0x69

#define TUYA_EF00_SWITCH_2X3_BUTTON_1_DP     0x01
#define TUYA_EF00_SWITCH_2X3_BUTTON_2_DP     0x02
#define TUYA_EF00_SWITCH_2X3_BATTERY_DP      0x0A

#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_DP            0x01//[1, "energy", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_DP   0x17//[23, "produced_energy", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_DP             0x1D//[29, "power", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_AC_FREQUENCY_DP      0x20//[32, "ac_frequency", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_DP      0x32//[50, "power_factor", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_A_DP         0x67//[103, "voltage_a", tuya.valueConverter.divideBy10],
#define TUYA_3PHASES_ELECTRICITY_METER_CURRENT_A_DP         0x68//[104, "current_a", tuya.valueConverter.divideBy1000],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_A_DP           0x69//[105, "power_a", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_A_DP    0x6C//[108, "power_factor_a", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_A_DP          0x6D//[109, "energy_a", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_A_DP 0x6E//[110, "energy_produced_a", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_B_DP         0x70//[112, "voltage_b", tuya.valueConverter.divideBy10],
#define TUYA_3PHASES_ELECTRICITY_METER_CURRENT_B_DP         0x71//[113, "current_b", tuya.valueConverter.divideBy1000],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_B_DP           0x72//[114, "power_b", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_B_DP    0x75//[117, "power_factor_b", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_B_DP          0x76//[118, "energy_b", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_B_DP 0x77//[119, "energy_produced_b", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_C_DP         0x79//[121, "voltage_c", tuya.valueConverter.divideBy10],
#define TUYA_3PHASES_ELECTRICITY_METER_CURRENT_C_DP         0x7A//[122, "current_c", tuya.valueConverter.divideBy1000],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_C_DP           0x7B//[123, "power_c", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_C_DP    0x7E//[126, "power_factor_c", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_C_DP          0x7F//[127, "energy_c", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_C_DP 0x80//[128, "energy_produced_c", tuya.valueConverter.divideBy100],


typedef struct Tuya_read_dp_result_s {
  bool     is_success;
  uint8_t  dp_id;
  uint8_t  dp_type;
  uint16_t dp_size;
  uint32_t dp_value;
  uint8_t  dp_raw_value_8[8];
} Tuya_read_dp_result_t;

void processTuyaCustomCluster(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, uint16_t payload_size, uint8_t *payload, signed char rssi);

#endif