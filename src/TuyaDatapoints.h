#ifndef TUYA_DATAPOINTS_H_
#define TUYA_DATAPOINTS_H_

#include "ZigbeeGateway.h"

#define TUYA_DP_TYPE_RAW                                            0x00
#define TUYA_DP_TYPE_BOOL                                           0x01
#define TUYA_DP_TYPE_VALUE                                          0x02
#define TUYA_DP_TYPE_STRING                                         0x03
#define TUYA_DP_TYPE_ENUM                                           0x04
#define TUYA_DP_TYPE_BITMAP                                         0x05

#define TUYA_DOUBLE_DIMMER_SWITCH_1_STATE_DP                        0x01
#define TUYA_DOUBLE_DIMMER_SWITCH_2_STATE_DP                        0x07
#define TUYA_DOUBLE_DIMMER_SWITCH_1_BRIGHTNESS_DP                   0x02
#define TUYA_DOUBLE_DIMMER_SWITCH_2_BRIGHTNESS_DP                   0x08

#define TUYA_SOIL_SENSOR_TEMPERATURE_DP                             0x05
#define TUYA_SOIL_SENSOR_MOISTURE_DP                                0x03
#define TUYA_SOIL_SENSOR_BATTERY_LEVEL_DP                           0x0F
#define TUYA_SOIL_SENSOR_BATTERY_LEVEL_DP                           0x0F
#define TUYA_SOIL_SENSOR_TEMPERATURE_SENSITIVITY_DP                 0x6B
#define TUYA_SOIL_SENSOR_MOISTURE_SENSITIVITY_DP                    0x6C
#define TUYA_SOIL_SENSOR_REFRESH_FREQUENCY_DP                       0x6D

#define TUYA_TH_SENSOR_TEMPERATURE_DP                               0x01
#define TUYA_TH_SENSOR_HUMIDITY_DP                                  0x02
#define TUYA_TH_SENSOR_BATTERY_STATE_DP                             0x03
#define TUYA_TH_SENSOR_BATTERY_LEVEL_DP                             0x04
#define TUYA_TH_SENSOR_TEMPERATURE_UNIT_DP                          0x09

#define TUYA_SMOKE_DETECTOR_SMOKE_DP                                0x01
#define TUYA_SMOKE_DETECTOR_PPM_DP                                  0x02
#define TUYA_SMOKE_DETECTOR_TAMPER_DP                               0x04
#define TUYA_SMOKE_DETECTOR_DEVICE_FAULT_DP                         0x0B
#define TUYA_SMOKE_DETECTOR_BATTERY_STATE_DP                        0x0E
#define TUYA_SMOKE_DETECTOR_BATTERY_LEVEL_DP                        0x0F

#define TUYA_SMOKE_DETECTOR_2_SENSITIVITY_DP                        0x66

#define TUYA_CO_DETECTOR_CO_DP                                      0x01
#define TUYA_CO_DETECTOR_CO_CONC_DP                                 0x02
#define TUYA_CO_DETECTOR_SELF_TEST_DP                               0x09
#define TUYA_CO_DETECTOR_BATTERY_LEVEL_DP                           0x0F
#define TUYA_CO_DETECTOR_SILENCE_DP                                 0x10

#define TUYA_GAS_DETECTOR_GAS_DP                                    0x01
#define TUYA_GAS_DETECTOR_GAS_VALUE_DP                              0x02
#define TUYA_GAS_DETECTOR_RINGTONE_DP                               0x06
#define TUYA_GAS_DETECTOR_ALARM_TIME_DP                             0x07
#define TUYA_GAS_DETECTOR_SELF_TEST_DP                              0x08
#define TUYA_GAS_DETECTOR_SELF_TEST_RESULT_DP                       0x09
#define TUYA_GAS_DETECTOR_PREHEAT_DP                                0x0A
#define TUYA_GAS_DETECTOR_BATTERY_LEVEL_DP                          0x0F
#define TUYA_GAS_DETECTOR_SILENCE_DP                                0x10

#define TUYA_AIR_QUALITY_SENSOR_TEMPERATURE_DP                      0x12
#define TUYA_AIR_QUALITY_SENSOR_HUMIDITY_DP                         0x13
#define TUYA_AIR_QUALITY_SENSOR_CO2_DP                              0x16
#define TUYA_AIR_QUALITY_SENSOR_VOC_DP                              0x15
#define TUYA_AIR_QUALITY_SENSOR_FA_DP                               0x02

#define TUYA_PRESENCE_SENSOR_PRESENCE_DP                            0x01
#define TUYA_PRESENCE_SENSOR_STATIC_DETECTION_SENSITIVITY_DP        0x02
#define TUYA_PRESENCE_SENSOR_STATIC_DETECTION_DISTANCE_DP           0x04
#define TUYA_PRESENCE_SENSOR_MOTION_STATE_DP                        0x65
#define TUYA_PRESENCE_SENSOR_FADING_TIME_DP                         0x66
#define TUYA_PRESENCE_SENSOR_ILLUMINANCE_DP                         0x6A
#define TUYA_PRESENCE_SENSOR_INDICATOR_DP                           0x6B
#define TUYA_PRESENCE_SENSOR_BATTERY_DP                             0x79
#define TUYA_PRESENCE_SENSOR_MOTION_DETECTION_MODE_DP               0x7A
#define TUYA_PRESENCE_SENSOR_MOTION_DETECTION_SENSITIVITY_DP        0x7B

#define TUYA_PRESENCE_SENSOR_1_BATTERY_DP                           0x04
#define TUYA_PRESENCE_SENSOR_1_PIR_SENSITIVITY_DP                   0x09
#define TUYA_PRESENCE_SENSOR_1_PIR_TIME_DP                          0x0A

#define TUYA_PRESENCE_SENSOR_5_PRESENCE_DP                          0x65
#define TUYA_PRESENCE_SENSOR_5_ILLUMINANCE_DP                       0x0C

#define TUYA_PRESENCE_SENSOR_4IN1_MOTION_DETECTION_SENSITIVITY_DP   0x02
#define TUYA_PRESENCE_SENSOR_4IN1_BATTERY_DP                        0x6E
#define TUYA_PRESENCE_SENSOR_4IN1_HUMIDITY_DP                       0x65
#define TUYA_PRESENCE_SENSOR_4IN1_FADING_TIME_DP                    0x66
#define TUYA_PRESENCE_SENSOR_4IN1_HUMIDITY_CALIBRATION_DP           0x68
#define TUYA_PRESENCE_SENSOR_4IN1_TEMPERATURE_CALIBRATION_DP        0x69
#define TUYA_PRESENCE_SENSOR_4IN1_ILLUMINANCE_DP                    0x6A
#define TUYA_PRESENCE_SENSOR_4IN1_ILLUMINANCE_INTERVAL_DP           0x6B
#define TUYA_PRESENCE_SENSOR_4IN1_LED_INDICATOR_MODE_DP             0x6C
#define TUYA_PRESENCE_SENSOR_4IN1_TEMPERATURE_UNIT_DP               0x6D
#define TUYA_PRESENCE_SENSOR_4IN1_BATTERY_DP                        0x6E
#define TUYA_PRESENCE_SENSOR_4IN1_TEMPERATURE_DP                    0x6F

#define TUYA_PRESENCE_SENSOR_RELAY_ILLUMINANCE_DP                   0x68
#define TUYA_PRESENCE_SENSOR_RELAY_DISTANCE_DP                      0x09
#define TUYA_PRESENCE_SENSOR_RELAY_SWITCH_MODE_DP                   0x6B
#define TUYA_PRESENCE_SENSOR_RELAY_SWITCH_STATE_DP                  0x6C
#define TUYA_PRESENCE_SENSOR_RELAY_SENSITIVITY_DP                   0x02
#define TUYA_PRESENCE_SENSOR_RELAY_MINIMUM_DETECTION_RANGE_DP       0x03
#define TUYA_PRESENCE_SENSOR_RELAY_MAXIMUM_DETECTION_RANGE_DP       0x04
#define TUYA_PRESENCE_SENSOR_RELAY_ENTRY_SENSITIVITY_DP             0x69
#define TUYA_PRESENCE_SENSOR_RELAY_ENTRY_DISTANCE_INDENTATION_DP    0x6A
#define TUYA_PRESENCE_SENSOR_RELAY_ENTRY_FILTER_TIME_DP             0x65
#define TUYA_PRESENCE_SENSOR_RELAY_DEPARTURE_DELAY_DP               0x66
#define TUYA_PRESENCE_SENSOR_RELAY_BLOCK_TIME_DP                    0x70
#define TUYA_PRESENCE_SENSOR_RELAY_SENSOR_STATE_DP                  0x73
#define TUYA_PRESENCE_SENSOR_RELAY_ILLUMINANCE_THRESHOLD_DP         0x6E

#define TUYA_PRESENCE_SENSOR_NEO_HUMAN_MOTION_STATE_DP              0x0B
#define TUYA_PRESENCE_SENSOR_NEO_DEPARTURE_DELAY_DP                 0x0C
#define TUYA_PRESENCE_SENSOR_NEO_RADAR_RANGE_DP                     0x0D
#define TUYA_PRESENCE_SENSOR_NEO_RADAR_SENSITIVITY_DP               0x0F
#define TUYA_PRESENCE_SENSOR_NEO_PRESENCE_SENSITIVITY_DP            0x10
#define TUYA_PRESENCE_SENSOR_NEO_CURRENT_DISTANCE_DP                0x13

#define TUYA_RAIN_SENSOR_ILLUMINANCE_DP                             0x65
#define TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_DP                  0x66
#define TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_DP                   0x67
#define TUYA_RAIN_SENSOR_RAIN_INTENSITY_DP                          0x69

#define TUYA_RAIN_SENSOR_2_RAIN_DP                                  0x01
#define TUYA_RAIN_SENSOR_2_SENSITIVITY_DP                           0x02
#define TUYA_RAIN_SENSOR_2_ILLUMINANCE_INTERVAL_DP                  0x65
#define TUYA_RAIN_SENSOR_2_ILLUMINANCE_DP                           0x66
#define TUYA_RAIN_SENSOR_2_BATTERY_DP                               0x68

#define TUYA_EF00_SWITCH_2X3_BUTTON_1_DP                            0x01
#define TUYA_EF00_SWITCH_2X3_BUTTON_2_DP                            0x02
#define TUYA_EF00_SWITCH_2X3_BATTERY_DP                             0x0A

#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_DP                    0x01//[1, "energy", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_DP           0x17//[23, "produced_energy", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_DP                     0x1D//[29, "power", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_AC_FREQUENCY_DP              0x20//[32, "ac_frequency", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_DP              0x32//[50, "power_factor", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_A_DP                 0x67//[103, "voltage_a", tuya.valueConverter.divideBy10],
#define TUYA_3PHASES_ELECTRICITY_METER_CURRENT_A_DP                 0x68//[104, "current_a", tuya.valueConverter.divideBy1000],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_A_DP                   0x69//[105, "power_a", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_A_DP            0x6C//[108, "power_factor_a", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_A_DP                  0x6D//[109, "energy_a", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_A_DP         0x6E//[110, "energy_produced_a", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_B_DP                 0x70//[112, "voltage_b", tuya.valueConverter.divideBy10],
#define TUYA_3PHASES_ELECTRICITY_METER_CURRENT_B_DP                 0x71//[113, "current_b", tuya.valueConverter.divideBy1000],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_B_DP                   0x72//[114, "power_b", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_B_DP            0x75//[117, "power_factor_b", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_B_DP                  0x76//[118, "energy_b", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_B_DP         0x77//[119, "energy_produced_b", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_VOLTAGE_C_DP                 0x79//[121, "voltage_c", tuya.valueConverter.divideBy10],
#define TUYA_3PHASES_ELECTRICITY_METER_CURRENT_C_DP                 0x7A//[122, "current_c", tuya.valueConverter.divideBy1000],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_C_DP                   0x7B//[123, "power_c", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_C_DP            0x7E//[126, "power_factor_c", tuya.valueConverter.raw],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_C_DP                  0x7F//[127, "energy_c", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_PRODUCED_C_DP         0x80//[128, "energy_produced_c", tuya.valueConverter.divideBy100],
#define TUYA_3PHASES_ELECTRICITY_METER_UPDATE_FREQUENCY_DP          0x66//[102, "update_frequency", tuya.valueConverterBasic.divideBy(1)],

#define TUYA_1PHASE_ELECTRICITY_METER_ENERGY_A_DP                   0x01//[1, "energy", tuya.valueConverter.divideBy100],
#define TUYA_1PHASE_ELECTRICITY_METER_ENERGY_PRODUCED_A_DP          0x17//[23, "produced_energy", tuya.valueConverter.divideBy100],
#define TUYA_1PHASE_ELECTRICITY_METER_AC_FREQUENCY_DP               0x20//[32, "ac_frequency", tuya.valueConverter.divideBy100],
#define TUYA_1PHASE_ELECTRICITY_METER_POWER_FACTOR_A_DP             0x32//[50, "power_factor", tuya.valueConverter.raw],
#define TUYA_1PHASE_ELECTRICITY_METER_VOLTAGE_A_DP                  0x66//[102, "voltage_a", tuya.valueConverter.divideBy10],
#define TUYA_1PHASE_ELECTRICITY_METER_CURRENT_A_DP                  0x67//[103, "current_a", tuya.valueConverter.divideBy1000],
#define TUYA_1PHASE_ELECTRICITY_METER_POWER_A_DP                    0x68//[104, "power", tuya.valueConverter.raw]

#define MOES_ALARM_BATTERY_PERCENTAGE_DP                            0x0F //15 02 04
#define MOES_ALARM_MELODY_DP                                        0x15 //21 ENUM
#define MOES_ALARM_DURATION_DP                                      0x07 //07 02 04
#define MOES_ALARM_SWITCH_DP                                        0x0D //13 BOOL
#define MOES_ALARM_VOLUME_DP                                        0x05// ENUM 0,1,2

#define MOES_ALARM_MELODY_FIRST_MELODY                              0x01
#define MOES_ALARM_MELODY_LAST_MELODY                               0x12
#define MOES_ALARM_VOLUME_LOWEST                                    0x00
#define MOES_ALARM_VOLUME_HIGHEST                                   0x02
#define MOES_ALARM_MELODY_MIN_DURATION                              0x0000
#define MOES_ALARM_MELODY_MAX_DURATION                              0x0708 //1800 s

#define MOES_SHADES_DRIVE_MOTOR_STATE_DP                            0x01 //ENUM OPEN(0) STOP (1) CLOSE(2)
#define MOES_SHADES_DRIVE_MOTOR_STATE_COVER_POSITION_PERCENTAGE_DP  0x03
#define MOES_SHADES_DRIVE_MOTOR_STATE_COVER_POSITION_DP             0x02
#define MOES_SHADES_DRIVE_MOTOR_STATE_MOTOR_DIRECTION_DP            0x05 //ENUM? NORMAL (0) REVERSED(1)
#define MOES_SHADES_DRIVE_MOTOR_STATE_OPENING_MODE_DP               0x65 //101 ENUM? TILT(0) LIFT(1)
#define MOES_SHADES_DRIVE_MOTOR_STATE_FACTORY_RESET_DP              0x66 //102
#define MOES_SHADES_DRIVE_MOTOR_SET_UPPER_LIMIT_DP                  0x67 //103
#define MOES_SHADES_DRIVE_MOTOR_SET_BOTTOM_LIMIT_DP                 0x68 //104
#define MOES_SHADES_DRIVE_MOTOR_SET_MOTOR_SPEED_DP                  0x69 //105

#define TUYA_COVER_MOTOR_BATTERY_LEVEL_DP                           0x0D

#define MOES_COVER_STATE_DP                                         0x01 //ENUM OPEN(0) STOP (1) CLOSE(2)
#define MOES_COVER_STATE_COVER_POSITION_DP                          0x02
#define MOES_COVER_CALIBRATION_DP                                   0x03
#define MOES_COVER_BACKLIGHT_DP                                     0x07
#define MOES_COVER_STATE_MOTOR_REVERSAL_DP                          0x08

#define TUYA_VIBRATION_SENSOR_VIBRATION_DP                          0x01
#define TUYA_VIBRATION_SENSOR_CONTACT_DP                            0x65
#define TUYA_VIBRATION_SENSOR_BATTERY_DP                            0x04
#define TUYA_VIBRATION_SENSOR_SENSITIVITY_DP                        0x09

#define TUYA_ON_OFF_BATTERY_VALVE_SWITCH_DP                         0x01
#define TUYA_ON_OFF_BATTERY_VALVE_STATE_DP                          0x08
#define TUYA_ON_OFF_BATTERY_VALVE_BATTERY_LEVEL_DP                  0x65


#define GIEX_WATER_VALVE_MODE_DP                                    0x01            
#define GIEX_WATER_VALVE_STATE_DP                                   0x02
#define GIEX_WATER_VALVE_IRRIGATION_START_TIME_DP                   0x65
#define GIEX_WATER_VALVE_IRRIGATION_END_TIME_DP                     0x66
#define GIEX_WATER_VALVE_CYCLE_IRRIGATION_NUMBER_DP                 0x67
#define GIEX_WATER_VALVE_IRRIGATION_TARGET_DP                       0x68
#define GIEX_WATER_VALVE_CYCLE_IRRIGATION_INTERVAL_DP               0x69
#define GIEX_WATER_VALVE_CURRENT_TEMPERATURE_DP                     0x6A
#define GIEX_WATER_VALVE_BATTERY_DP                                 0x6C
#define GIEX_WATER_VALVE_WATER_CONSUMED_DP                          0x6F
#define GIEX_WATER_VALVE_LAST_IRRIGATION_DURATION_DP                0x72

#define TUYA_ILLUMINANCE_SENSOR_ILLUMINANCE_DP                      0x02
#define TUYA_ILLUMINANCE_SENSOR_BATTERY_DP                          0x04

#define TUYA_PIR_ILLUMINANCE_SENSOR_PIR_DP                          0x01
#define TUYA_PIR_ILLUMINANCE_SENSOR_ILLUMINANCE_DP                  0x0C
#define TUYA_PIR_ILLUMINANCE_SENSOR_BATTERY_LEVEL_DP                0x04
#define TUYA_PIR_ILLUMINANCE_SENSOR_SENSITIVITY_DP                  0x09 //ENUM
#define TUYA_PIR_ILLUMINANCE_SENSOR_KEEP_TIME_DP                    0x0A //ENUM
#define TUYA_PIR_ILLUMINANCE_SENSOR_ILLUMINANCE_INTERVAL_DP         0x66 //VALUE

#define TUYA_LCD_PANEL_3_RELAYS_RELAY_1_DP                          0x18
#define TUYA_LCD_PANEL_3_RELAYS_RELAY_2_DP                          0x19
#define TUYA_LCD_PANEL_3_RELAYS_RELAY_3_DP                          0x1A

#define TUYA_LCD_PANEL_3_RELAYS_TURN_ON_DP                          0x05
#define TUYA_LCD_PANEL_3_RELAYS_TURN_OFF_DP                         0x06
#define TUYA_LCD_PANEL_3_RELAYS_TV_MODE_DP                          0x07
#define TUYA_LCD_PANEL_3_RELAYS_MEETING_MODE_DP                     0x08
#define TUYA_LCD_PANEL_3_RELAYS_SLEEP_MODE_DP                       0x09
#define TUYA_LCD_PANEL_3_RELAYS_COFFE_BREAK_MODE_DP                 0x0A


#define TUYA_LCD_PANEL_3_RELAYS_SHUTTER_CLOSE_DP                    0x6B
#define TUYA_LCD_PANEL_3_RELAYS_SHUTTER_STOP_DP                     0x6C
#define TUYA_LCD_PANEL_3_RELAYS_SHUTTER_OPEN_DP                     0x6D

#define TUYA_LCD_PANEL_3_RELAYS_CURTAIN_MOVE_UP_DP                  0x76
#define TUYA_LCD_PANEL_3_RELAYS_CURTAIN_STOP_DP                     0x77
#define TUYA_LCD_PANEL_3_RELAYS_CURTAIN_MOVE_DOWN_DP                0x78
#define TUYA_LCD_PANEL_3_RELAYS_AC_ON_DP                            0x7C
#define TUYA_LCD_PANEL_3_RELAYS_AC_OFF_DP                           0x7D
#define TUYA_LCD_PANEL_3_RELAYS_COOL_ON_DP                          0x7E
#define TUYA_LCD_PANEL_3_RELAYS_HEAT_ON_DP                          0x7F

#define TUYA_8_RELAYS_CONTROLLER_RELAY_1_DP                         0x01
#define TUYA_8_RELAYS_CONTROLLER_RELAY_2_DP                         0x02
#define TUYA_8_RELAYS_CONTROLLER_RELAY_3_DP                         0x03
#define TUYA_8_RELAYS_CONTROLLER_RELAY_4_DP                         0x04
#define TUYA_8_RELAYS_CONTROLLER_RELAY_5_DP                         0x05
#define TUYA_8_RELAYS_CONTROLLER_RELAY_6_DP                         0x06
#define TUYA_8_RELAYS_CONTROLLER_RELAY_7_DP                         0x07
#define TUYA_8_RELAYS_CONTROLLER_RELAY_8_DP                         0x08
#define TUYA_8_RELAYS_CONTROLLER_STATUS_DP                          0x1B // ENUM ??
#define TUYA_8_RELAYS_CONTROLLER_LOCK_DP                            0x1D

#define TUYA_FINGERBOT_PLUS_MODE_DP                                 0x65
#define TUYA_FINGERBOT_PLUS_LOWER_LIMIT_DP                          0x66
#define TUYA_FINGERBOT_SUSTAIN_TIME_DP                              0x67
#define TUYA_FINGERBOT_PLUS_REVERSE_DP                              0x68
#define TUYA_FINGERBOT_PLUS_BATTERY_DP                              0x69
#define TUYA_FINGERBOT_PLUS_UPPER_LIMIT_DP                          0x6A
#define TUYA_FINGERBOT_PLUS_TOUCH_DP                                0x6B

#define TUYA_ZWT_ZWT198_TEMPERATURE_CALIBRATION_DP                  0x13
#define TUYA_ZWT_ZWT198_SENSOR_DP                                   0x6A
#define TUYA_ZWT_ZWT198_BACKLIGHT_MODE_DP                           0x6E

#define TUYA_DP_ZCL_PAYLOAD_32  0x0A //32bit value /10 bytes/
#define TUYA_DP_ZCL_PAYLOAD_16  0x08 //16bit value /8 bytes/
#define TUYA_DP_ZCL_PAYLOAD_8   0x07 //8 bit value /7 bytes/
#define TUYA_DP_ZCL_PAYLOAD_0   0x00 //

/*---------------------------------------------------------------------------*/

typedef struct Tuya_dp_zcl_payload_s {

union {
    uint16_t  tsn_16_big_endian;
    uint8_t   tsn_8[2];
};
    uint8_t   dp_id;
    uint8_t   dp_type;
union {
    uint16_t  dp_size_16_big_endian;
    uint8_t   dp_size_8[2];
};
union {
    uint32_t  dp_value_32_big_endian;
    uint8_t   dp_value_32_8[4];
    uint16_t  dp_value_16_big_endian;
    uint8_t   dp_value_16_8[2];
    uint8_t   dp_value_8;
};
} __attribute__((packed)) Tuya_dp_zcl_payload_t;

typedef struct Tuya_dp_data_s {

    uint8_t   dp_id;
    uint8_t   dp_type;
union {
    uint16_t  dp_size_16_big_endian;
    uint8_t   dp_size_8[2];
};
union {
    uint32_t  dp_value_32_big_endian;
    uint8_t   dp_value_32_8[4];
    uint16_t  dp_value_16_big_endian;
    uint8_t   dp_value_16_8[2];
    uint8_t   dp_value_8;
};
} __attribute__((packed)) Tuya_dp_data_t;

/*---------------------------------------------------------------------------*/

inline int32_t getTuyaDPValue(Tuya_dp_zcl_payload_s *Tuya_dp_zcl_payload) {

  uint16_t dp_size = __builtin_bswap16(Tuya_dp_zcl_payload->dp_size_16_big_endian);

  int32_t dp_value = INT32_MIN;

  switch (dp_size) {


    case 0x01:

        dp_value = Tuya_dp_zcl_payload->dp_value_8;
    break;

    case 0x02:

        dp_value = __builtin_bswap16(Tuya_dp_zcl_payload->dp_value_16_big_endian);
    break;

    case 0x04:

        dp_value = __builtin_bswap32(Tuya_dp_zcl_payload->dp_value_32_big_endian);
    break;
  }

  return dp_value;
}


/*---------------------------------------------------------------------------*/

inline void makeTuyaDPValue32(Tuya_dp_zcl_payload_s &Tuya_dp_zcl_payload, 
                              uint8_t dp_id, 
                              uint32_t dp_value_32_little_endian) {
 
  Tuya_dp_zcl_payload.tsn_16_big_endian      =  __builtin_bswap16(random(0x0000, 0xFFFF));
  Tuya_dp_zcl_payload.dp_id                  =  dp_id;
  Tuya_dp_zcl_payload.dp_type                =  0x02; //VALUE

  Tuya_dp_zcl_payload.dp_size_16_big_endian  =  0x0400; 
  Tuya_dp_zcl_payload.dp_value_32_big_endian =  __builtin_bswap32(dp_value_32_little_endian);
}

/*---------------------------------------------------------------------------*/

inline void makeTuyaDPBool(Tuya_dp_zcl_payload_s &Tuya_dp_zcl_payload, 
                           uint8_t dp_id, 
                           uint8_t value_8) {
 
  Tuya_dp_zcl_payload.tsn_16_big_endian      =  __builtin_bswap16(random(0x0000, 0xFFFF));
  Tuya_dp_zcl_payload.dp_id                  =  dp_id;
  Tuya_dp_zcl_payload.dp_type                =  0x01; //BOOL

  Tuya_dp_zcl_payload.dp_size_16_big_endian  =  0x0100; 
  Tuya_dp_zcl_payload.dp_value_8             =  value_8;
}

/*---------------------------------------------------------------------------*/

inline void makeTuyaDPEnum8(Tuya_dp_zcl_payload_s 
                            &Tuya_dp_zcl_payload, 
                            uint8_t dp_id, 
                            uint8_t value_8) {
 
  Tuya_dp_zcl_payload.tsn_16_big_endian      =  __builtin_bswap16(random(0x0000, 0xFFFF));
  Tuya_dp_zcl_payload.dp_id                  =  dp_id;
  Tuya_dp_zcl_payload.dp_type                =  0x04; //ENUM8

  Tuya_dp_zcl_payload.dp_size_16_big_endian  =  0x0100; 
  Tuya_dp_zcl_payload.dp_value_8             =  value_8;
}

/*---------------------------------------------------------------------------*/

bool sendTuyaRequestCmdBool(ZigbeeGateway *gateway, 
                            zbg_device_params_t *device, 
                            uint8_t dp_id, 
                            bool dp_value,
                            bool cmd_sync = CUSTOM_CMD_ASYNC);

/*---------------------------------------------------------------------------*/

bool sendTuyaRequestCmdEnum8(ZigbeeGateway *gateway, 
                             zbg_device_params_t *device, 
                             uint8_t dp_id, 
                             uint8_t dp_value,
                             bool cmd_sync = CUSTOM_CMD_ASYNC);

/*---------------------------------------------------------------------------*/

bool sendTuyaRequestCmdValue32(ZigbeeGateway *gateway, 
                               zbg_device_params_t *device,
                               uint8_t dp_id, 
                               uint32_t dp_value,
                               bool cmd_sync = CUSTOM_CMD_ASYNC);

/*---------------------------------------------------------------------------*/

bool sendTuyaRequestCmdData(ZigbeeGateway *gateway, 
                            zbg_device_params_t *device,
                            uint8_t dp_id,
                            uint8_t dp_type, 
                            uint32_t dp_value,
                            bool cmd_sync = CUSTOM_CMD_ASYNC);

/*---------------------------------------------------------------------------*/

bool sendTuyaQueryCmd(ZigbeeGateway *gateway, 
                      zbg_device_params_t *device,
                      bool cmd_sync = CUSTOM_CMD_ASYNC);

#endif //TUYA_DATAPOINTS_H_