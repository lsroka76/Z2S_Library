#ifndef Z2S_DEVICES_DATABASE_H_
#define Z2S_DEVICES_DATABASE_H_

// Z2S_DEVICE_DESC_ID - used for selection of clusters to bind and matching Supla channels#define

#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_0              (1 << 0)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_1              (1 << 1)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_2              (1 << 2)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_3              (1 << 3)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_4              (1 << 4)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_5              (1 << 5)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_6              (1 << 6)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_7              (1 << 7)

#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT               (1 << 8)  //aka Tuya magic
#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_QUERY              (1 << 9)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY       (1 << 10)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_SETUP              (1 << 11)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT               (1 << 12)

#define Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_ENROLL_1       (1 << 16)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_ENROLL_2       (1 << 17)

#define Z2S_DEVICE_DESC_NULL                                0x0000

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR                 0x1000
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1               0x1001
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL            0x1002
#define Z2S_DEVICE_DESC_TEMPERATURE_SENSOR_POLL             0x1003

#define Z2S_DEVICE_DESC_TEMPERATURE_SENSOR                  0x1005

#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR       0x1010

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10         0x1020

#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR            0x1030

#define Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR             0x1050
#define Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR        0x1055

#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR       0x1100
#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1     0x1101


#define Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR             0x1200
#define Z2S_DEVICE_DESC_TUYA_ILLUZONE_SENSOR                0x1205
#define Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_DP_SENSOR          0x1210

#define Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR                    0x1300
#define Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2                  0x1305

#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR                     0x2000
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T               0x2001
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B               0x2002
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B                 0x2003

#define Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR                0x2004
#define Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1              0x2005
#define Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2              0x2006

#define Z2S_DEVICE_DESC_IKEA_VALLHORN_1                     0x2007
#define Z2S_DEVICE_DESC_IKEA_VALLHORN_2                     0x2008
#define Z2S_DEVICE_DESC_IKEA_VALLHORN_3                     0x2009

#define Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR                  0x2010
#define Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR                  0x2011

#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B        0x2020

#define Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR               0x2030
#define Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR                0x2040   
#define Z2S_DEVICE_DESC_DEVELCO_IAS_ZONE_TEMP_SENSOR        0x2050
#define Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR         0x2060

#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR                 0x2200
#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1               0x2201
#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2               0x2202 //ONENUO

#define Z2S_DEVICE_DESC_TUYA_CO_DETECTOR                    0x2210

#define Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR                   0x2220

#define Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR             0x2230
#define Z2S_DEVICE_DESC_DEVELCO_AIR_QUALITY_SENSOR          0x2250
#define Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR             0x2260

#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR                0x2300
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5              0x2305
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1           0x2310

#define Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR             0x2500
#define Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR       0x2501

#define Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR                   0x2600

#define Z2S_DEVICE_DESC_RGBW_START                          0x3000

#define Z2S_DEVICE_DESC_LIGHT_SOURCE                        0x3000
#define Z2S_DEVICE_DESC_LIGHT_BULB                          0x3010

#define Z2S_DEVICE_DESC_RGB_BULB                            0x3050
#define Z2S_DEVICE_DESC_TUYA_RGB_BULB                       0x3060

#define Z2S_DEVICE_DESC_RGBW_BULB                           0x3100
#define Z2S_DEVICE_DESC_RGBW_BULB_XY                        0x3101
#define Z2S_DEVICE_DESC_RGBW_BULB_HS                        0x3102

#define Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A              0x3110
#define Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B              0x3111
#define Z2S_DEVICE_DESC_IKEA_RGBW_BULB                      0x3120
#define Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB                   0x3130

#define Z2S_DEVICE_DESC_DIMMER_BULB                         0x3200
#define Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB                 0x3205
#define Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0               0x3210
#define Z2S_DEVICE_DESC_IKEA_WW_BULB                        0x3220
#define Z2S_DEVICE_DESC_PHILIPS_WW_BULB                     0x3225

#define Z2S_DEVICE_DESC_RGBW_END                            0x3299

#define Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH                  0x3300
#define Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH           0x3305

#define Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY          0x3400

#define Z2S_DEVICE_DESC_RELAY                               0x4000
#define Z2S_DEVICE_DESC_RELAY_1                             0x4001

#define Z2S_DEVICE_DESC_LUMI_SWITCH                         0x4010

#define Z2S_DEVICE_DESC_TUYA_RELAY                          0x4020

#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER             0x4100
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1           0x4101
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2           0x4102

#define Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER      0x4200
#define Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER     0x4300

#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER        0x4500
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1      0x4501
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2      0x4502
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A      0x4503

#define Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP            0x4540

#define Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER      0x4550
#define Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER       0x4551

#define Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1                  0x4600
#define Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2                  0x4601

#define Z2S_DEVICE_DESC_TUYA_1GANG_SWITCH                   0x4604
#define Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH                   0x4605
#define Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH                   0x4606
#define Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH                   0x4607

#define Z2S_DEVICE_DESC_ON_OFF_VALVE_DC                     0x4700
#define Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC                0x4705
#define Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY           0x4710

#define Z2S_DEVICE_DESC_WINDOW_COVERING_SINGLE              0x4800
#define Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE         0x4810
#define Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR             0x4820
#define Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE      0x4830

#define Z2S_DEVICE_DESC_SONOFF_SMART_VALVE                  0x4900
#define Z2S_DEVICE_DESC_GIEX_SMART_VALVE                    0x4910

#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F                0x5000
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F                0x5001
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F                0x5002

#define Z2S_DEVICE_DESC_TUYA_SWITCH_4X3                     0x5100

#define Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3                0x5105
#define Z2S_DEVICE_DESC_TUYA_SOS_BUTTON                     0x5110

#define Z2S_DEVICE_DESC_IKEA_SMART_BUTTON                   0x5200
#define Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F                0x5201

#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1                0x5202
#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1              0x5203
#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2              0x5204
#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3              0x5205

#define Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON                  0x5210
#define Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1                0x5211
#define Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2                0x5212

#define Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F              0x5300

#define Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F        0x5400
#define Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F                0x5410

#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH           0x5500
#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1         0x5501
#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2         0x5502

#define Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO                    0x5600
#define Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_1                  0x5601
#define Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_2                  0x5602

#define Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F                0x5610

#define Z2S_DEVICE_DESC_HVAC_START                          0x6000

#define Z2S_DEVICE_DESC_TUYA_HVAC                           0x6000
#define Z2S_DEVICE_DESC_TUYA_HVAC_23457                     0x6001
#define Z2S_DEVICE_DESC_TUYA_HVAC_6567C                     0x6002
#define Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY                    0x6003

#define Z2S_DEVICE_DESC_SONOFF_TRVZB                        0x6005

#define Z2S_DEVICE_DESC_TS0601_TRV_SASWELL                  0x6010
#define Z2S_DEVICE_DESC_TS0601_TRV_ME167                    0x6011
#define Z2S_DEVICE_DESC_TS0601_TRV_BECA                     0x6012
#define Z2S_DEVICE_DESC_TS0601_TRV_MOES                     0x6013
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV601                   0x6014
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV603                   0x6015
#define Z2S_DEVICE_DESC_TS0601_TRV_GTZ10                    0x6016
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z                  0x6017
#define Z2S_DEVICE_DESC_TS0601_TRV_TV02                     0x6018


#define Z2S_DEVICE_DESC_HVAC_END                            0x6499

#define Z2S_DEVICE_DESC_IR_REMOTE_CONTROL                   0x6500
#define Z2S_DEVICE_DESC_TUYA_SIREN_ALARM                    0x6600
#define Z2S_DEVICE_DESC_MOES_ALARM                          0x6610

#define Z2S_DEVICE_DESC_TUYA_REPEATER                       0x7000

#define Z2S_DEVICE_DESC_ON_OFF                              0x8000
#define Z2S_DEVICE_DESC_ON_OFF_1                            0x8001


#define MAX_BOUND_ENDPOINTS                                 0x04
#define MAX_BOUND_CLUSTERS                                  0x08


#define TUYA_ON_OFF_CUSTOM_CMD_BUTTON_PRESS_ID              0xFD
#define TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID             0xFC

#define NO_CUSTOM_CMD_SID                                  -0x01

#define IAS_ZONE_ALARM_1_SID                                0x00
#define IAS_ZONE_ALARM_2_SID                                0x01
#define IAS_ZONE_TAMPER_SID                                 0x02
#define IAS_ZONE_LOW_BATTERY_SID                            0x03

#define SONOFF_CUSTOM_CLUSTER_TAMPER_SID                    0x10

#define TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID                  0x00
#define TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID           0x01
#define TUYA_CUSTOM_CMD_BUTTON_HELD_SID                     0x02
#define TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID             0x0A
#define TUYA_CUSTOM_CMD_BUTTON_ROTATE_LEFT_SID              0x0B

#define TUYA_CUSTOM_CMD_BUTTON_1_PRESSED_SID                0x00
#define TUYA_CUSTOM_CMD_BUTTON_1_DOUBLE_PRESSED_SID         0x01
#define TUYA_CUSTOM_CMD_BUTTON_1_HELD_SID                   0x02
#define TUYA_CUSTOM_CMD_BUTTON_2_PRESSED_SID                0x03
#define TUYA_CUSTOM_CMD_BUTTON_2_DOUBLE_PRESSED_SID         0x04
#define TUYA_CUSTOM_CMD_BUTTON_2_HELD_SID                   0x05

#define TUYA_DOUBLE_DIMMER_SWITCH_1_SID                     0x01
#define TUYA_DOUBLE_DIMMER_SWITCH_2_SID                     0x02

#define Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR_PIR_SID 0x00
#define Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR_IL_SID  0x01

#define TUYA_PRESENCE_SENSOR_PRESENCE_SID                   0x01
#define TUYA_PRESENCE_SENSOR_MOTION_STATE_SID               0x02
#define TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID                0x03
#define TUYA_PRESENCE_SENSOR_TEMPHUMIDITY_SID               0x04

#define TUYA_SMOKE_DETECTOR_SMOKE_SID                       0x00
#define TUYA_SMOKE_DETECTOR_TAMPER_SID                      0x01

#define TUYA_SMOKE_DETECTOR_1_SMOKE_SID                     0x00
#define TUYA_SMOKE_DETECTOR_1_TAMPER_SID                    0x01

#define TUYA_CO_DETECTOR_CO_SID                             0x00
#define TUYA_CO_DETECTOR_CO_CONC_SID                        0x01
#define TUYA_CO_DETECTOR_SELF_TEST_SID                      0x02
#define TUYA_CO_DETECTOR_SILENCE_SID                        0x03

#define TUYA_GAS_DETECTOR_GAS_SID                           0x00 //keep compatibility with co_detector they have common routine
#define TUYA_GAS_DETECTOR_GAS_CONC_SID                      0x01
#define TUYA_GAS_DETECTOR_SELF_TEST_RESULT_SID              0x02
#define TUYA_GAS_DETECTOR_SILENCE_SID                       0x03
#define TUYA_GAS_DETECTOR_PREHEAT_SID                       0x05

#define DEVELCO_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID         0x00
#define DEVELCO_AIR_QUALITY_SENSOR_VOC_SID                  0x01

#define LUMI_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID            0x00
#define LUMI_AIR_QUALITY_SENSOR_VOC_SID                     0x01
#define LUMI_AIR_QUALITY_SENSOR_AIR_QUALITY_SID             0x02


#define TUYA_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID            0x00
#define TUYA_AIR_QUALITY_SENSOR_CO2_SID                     0x01
#define TUYA_AIR_QUALITY_SENSOR_VOC_SID                     0x02
#define TUYA_AIR_QUALITY_SENSOR_FA_SID                      0x03 //formaldehyde


#define TUYA_RAIN_SENSOR_RAIN_SID                           0x01
#define TUYA_RAIN_SENSOR_ILLUMINANCE_SID                    0x02
#define TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID         0x03
#define TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID          0x04
#define TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID                 0x05

//#define IAS_ZONE_ALARM_1_SID                              0x00
//#define IAS_ZONE_ALARM_2_SID                              0x01
//#define IAS_ZONE_TAMPER_SID                               0x02
#define ADEO_SMART_PIRTH_SENSOR_TEMPHUMI_SID                0x10
#define ADEO_SMART_PIRTH_SENSOR_ILLUMINANCE_SID             0x11

#define SONOFF_ON_OFF_SID                                   0X00
#define SONOFF_ELECTRICITY_METER_SID                        0x01
#define SONOFF_ELECTRICITY_METER_ENERGY_TODAY_SID           0x02
#define SONOFF_ELECTRICITY_METER_ENERGY_MONTH_SID           0x03
#define SONOFF_ELECTRICITY_METER_ENERGY_YESTERDAY_SID       0x04

#define Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP_TEMP_SID   0x00
#define Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP_SWITCH_SID 0x01
#define Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP_EM_SID     0x02

#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_SID           0x00
#define TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_SID  0x01
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_SID            0x02
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_SID     0x03
#define TUYA_3PHASES_ELECTRICITY_METER_SID                  0x04

#define IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID                0x00
#define IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID                   0x01
#define IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID                0x02
#define IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID                   0x03
#define IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID                0x04
#define IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID                   0x05
#define IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID                0x06
#define IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID                   0x07

#define IKEA_CUSTOM_CMD_SYMFONISK_PLAY_SID                  0x00
#define IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID             0x01
#define IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID           0x02
#define IKEA_CUSTOM_CMD_SYMFONISK_NEXT_TRACK_SID            0x03
#define IKEA_CUSTOM_CMD_SYMFONISK_PREV_TRACK_SID            0x04

#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID           0x05
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID    0x06
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID              0x07
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID     0x08
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID    0x09

#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID          0x0A
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID   0x0B
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID             0x0C
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID    0x0D
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID   0x0E

#define PHILIPS_HUE_DIMMER_SWITCH_ON_PRESS_SID              0x00
#define PHILIPS_HUE_DIMMER_SWITCH_ON_HOLD_SID               0x01
#define PHILIPS_HUE_DIMMER_SWITCH_ON_PRESS_RELEASE_SID      0x02
#define PHILIPS_HUE_DIMMER_SWITCH_ON_HOLD_RELEASE_SID       0x03
#define PHILIPS_HUE_DIMMER_SWITCH_UP_PRESS_SID              0x04
#define PHILIPS_HUE_DIMMER_SWITCH_UP_HOLD_SID               0x05
#define PHILIPS_HUE_DIMMER_SWITCH_UP_PRESS_RELEASE_SID      0x06
#define PHILIPS_HUE_DIMMER_SWITCH_UP_HOLD_RELEASE_SID       0x07
#define PHILIPS_HUE_DIMMER_SWITCH_DOWN_PRESS_SID            0x08
#define PHILIPS_HUE_DIMMER_SWITCH_DOWN_HOLD_SID             0x09
#define PHILIPS_HUE_DIMMER_SWITCH_DOWN_PRESS_RELEASE_SID    0x0A
#define PHILIPS_HUE_DIMMER_SWITCH_DOWN_HOLD_RELEASE_SID     0x0B
#define PHILIPS_HUE_DIMMER_SWITCH_OFF_PRESS_SID             0x0C
#define PHILIPS_HUE_DIMMER_SWITCH_OFF_HOLD_SID              0x0D
#define PHILIPS_HUE_DIMMER_SWITCH_OFF_PRESS_RELEASE_SID     0x0E
#define PHILIPS_HUE_DIMMER_SWITCH_OFF_HOLD_RELEASE_SID      0x0F

#define LUMI_MOTION_SENSOR_OCCUPANCY_SID                    0x00
#define LUMI_MOTION_SENSOR_ILLUMINANCE_SID                  0x01

#define SONOFF_SMART_VALVE_ON_OFF_SID                       0x00
#define SONOFF_SMART_VALVE_RUN_PROGRAM_SID                  0x01
#define SONOFF_SMART_VALVE_FLOW_SID                         0x02
#define SONOFF_SMART_VALVE_MODE_SID                         0x03
#define SONOFF_SMART_VALVE_CYCLE_NUMBER_SID                 0x04
#define SONOFF_SMART_VALVE_CYCLES_COUNT_SID                 0x05
#define SONOFF_SMART_VALVE_TIME_SID                         0x06
#define SONOFF_SMART_VALVE_PAUSE_SID                        0x07
#define SONOFF_SMART_VALVE_VOLUME_SID                       0x08

#define GIEX_SMART_VALVE_STATE_SID                          0x00
#define GIEX_SMART_VALVE_MODE_SID                           0x01
#define GIEX_SMART_VALVE_CYCLES_NUMBER_SID                  0x02
#define GIEX_SMART_VALVE_TARGET_SID                         0x03
#define GIEX_SMART_VALVE_INTERVAL_SID                       0x04
#define GIEX_SMART_VALVE_VOLUME_SID                         0x07

#define SONOFF_PIR_SENSOR_OCCUPANCY_SID                     0x00
#define SONOFF_PIR_SENSOR_ILLUMINANCE_SID                   0x01

#define DIMMER_FUNC_BRIGHTNESS_SID                          0x00
#define DIMMER_FUNC_COLOR_TEMPERATURE_SID                   0x01
#define DIMMER_ON_OFF_SWITCH_SID                            0x02

#define IAS_WD_SILENT_ALARM_SID                             0x00
#define IAS_WD_LOUD_ALARM_SID                               0x01

#define IAS_ACE_ARM_CMD_SID                                 0x00
#define IAS_ACE_PANIC_CMD_SID                               0x01
#define IAS_ACE_2_CMD_SID                                   0x02
#define IAS_ACE_3_CMD_SID                                   0x03

#define ADEO_CUSTOM_CMD_BUTTON_PRESSED_SID                  0x00
#define ADEO_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID           0x01
#define ADEO_CUSTOM_CMD_BUTTON_HELD_SID                     0x02

#define MOES_ALARM_SWITCH_SID                               0x00
#define MOES_ALARM_MELODY_SID                               0x01
#define MOES_ALARM_VOLUME_SID                               0x02
#define MOES_ALARM_DURATION_SID                             0x03
#define MOES_ALARM_DISPLAY_SID                              0x04

#define TUYA_VIBRATION_SENSOR_VIBRATION_SID                 0x10
#define TUYA_VIBRATION_SENSOR_CONTACT_SID                   0x11

#define TUYA_SOS_BUTTON_SOS_SID                             0x00
#define TUYA_SOS_BUTTON_EMERGENCY_SID                       0x01
#define TUYA_SOS_BUTTON_LOW_BATTERY_SID                     0x03 //IAS_ZONE_LOW_BATTERY_SID


[[maybe_unused]]
static const char IKEA_STYRBAR_BUTTONS[][16] PROGMEM = {"ON PRESSED", "ON HELD", "OFF PRESSED", 
                                                        "OFF HELD", "LEFT PRESSED", "LEFT HELD", 
                                                        "RIGHT PRESSED", "RIGHT HELD"};

[[maybe_unused]]
static const char IKEA_SYMFONISK_BUTTONS[][20] PROGMEM = {"PLAY", "VOLUME UP", "VOLUME DOWN", 
                                                          "NEXT TRACK ", "PREV TRACK",
                                                          "DOT PRESSED", "DOT SHORT RELEASE", 
                                                          "DOT HELD", "DOT LONG RELEASED", 
                                                          "DOT DOUBLE PRESSED",
                                                          "DOTS PRESSED", "DOTS SHORT RELEASE", 
                                                          "DOTS HELD", "DOTS LONG RELEASED", 
                                                          "DOTS DOUBLE PRESSED"};
[[maybe_unused]]
static const char PHILIPS_HUE_DIMMER_SWITCH_BUTTONS[][19] PROGMEM = {"ON_PRESS", "ON_PRESS_RELEASE", 
                                                                     "ON_HOLD", "ON_HOLD_RELEASE",
                                                                     "UP_PRESS", "UP_PRESS_RELEASE", 
                                                                     "UP_HOLD", "UP_HOLD_RELEASE",
                                                                     "DOWN_PRESS", "DOWN_PRESS_RELEASE", 
                                                                     "DOWN_HOLD", "DOWN_HOLD_RELEASE",
                                                                     "OFF_PRESS", "OFF_PRESS_RELEASE", 
                                                                     "OFF_HOLD","OFF_HOLD_RELEASE"};

typedef struct z2s_device_desc_s {
  uint32_t z2s_device_desc_id;
  uint8_t  z2s_device_clusters_count;
  uint32_t z2s_device_config_flags;
  uint16_t z2s_device_clusters[MAX_BOUND_CLUSTERS];
} z2s_device_desc_t;

typedef struct z2s_device_endpoint_s {
  uint8_t endpoint_id;
  uint32_t z2s_device_desc_id;
} z2s_device_endpoint_t;

/*typedef struct z2s_device_entity_s {
  char manufacturer_name[33];
  char model_name[33];
  uint32_t z2s_device_desc_id;
  uint8_t z2s_device_endpoints_count;
  z2s_device_endpoint_t z2s_device_endpoints[MAX_BOUND_ENDPOINTS];
} z2s_device_entity_t;*/

typedef struct z2s_device_entity_s {
  const char *manufacturer_name;
  const char *model_name;
  uint32_t z2s_device_uid;
  uint32_t z2s_device_desc_id;
  uint8_t z2s_device_endpoints_count;
  z2s_device_endpoint_t z2s_device_endpoints[MAX_BOUND_ENDPOINTS];
} z2s_device_entity_t;

/*constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;

constexpr uint32_t hash_32_fnv1a_const2(const char* str_1, const char* str_2, uint32_t value = val_32_const) noexcept {
  for (; *str_1 != '\0'; ++str_1) {
    value = (value ^ static_cast<uint32_t>(static_cast<uint8_t>(*str_1))) * prime_32_const;
  }
  for (; *str_2 != '\0'; ++str_2) {
    value = (value ^ static_cast<uint32_t>(static_cast<uint8_t>(*str_2))) * prime_32_const;
  }
  return value;
}
*/
static const z2s_device_desc_t Z2S_DEVICES_DESC[] PROGMEM [[maybe_unused]] = {


  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_NULL, 
    .z2s_device_clusters_count = 0, 
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = {0xFFFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPERATURE_SENSOR_POLL,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPERATURE_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUZONE_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_DP_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR,
    .z2s_device_clusters_count = 5,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},


  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

    {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_3,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                           ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                           ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
                           ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY,
                           ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                           ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2,
    .z2s_device_clusters_count = 3, .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             SONOFF_CUSTOM_CLUSTER }},

   {	.z2s_device_desc_id = Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER,
     .z2s_device_clusters_count = 3,
     .z2s_device_config_flags = 0x0,
     .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                              ESP_ZB_ZCL_CLUSTER_ID_METERING,
                              ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT |
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_SETUP,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,  
                             ESP_ZB_ZCL_CLUSTER_ID_METERING,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { IKEA_PRIVATE_CLUSTER_2,
                             IKEA_PRIVATE_CLUSTER_2 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { IKEA_PRIVATE_CLUSTER_2,
                             IKEA_PRIVATE_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { IKEA_PRIVATE_CLUSTER_2 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { IKEA_PRIVATE_CLUSTER_2 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { PHILIPS_CUSTOM_CLUSTER,
                             ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT,
                             LUMI_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ACE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F,
    .z2s_device_clusters_count = 2, .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ADEO_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_VALVE_DC,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_TRVZB,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT,
                             SONOFF_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_REPEATER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SIREN_ALARM,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_WD }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_BECA,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_MOES,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV603,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_GTZ10,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TV02,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB_XY,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_DIMMER_BULB,
    .z2s_device_clusters_count = 2, 
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB,
    .z2s_device_clusters_count = 3, 
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_BULB,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_WW_BULB,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT | Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_DETECTOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
                             LUMI_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_DEVELCO_AIR_QUALITY_SENSOR,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
                             DEVELCO_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOS_BUTTON,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IR_REMOTE_CONTROL,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ZOSUNG_IR_CONTROL_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_VALVE,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT }},

   {	.z2s_device_desc_id = Z2S_DEVICE_DESC_GIEX_SMART_VALVE,
    .z2s_device_clusters_count = 1,
      .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_ALARM,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }}
};

static const z2s_device_entity_t Z2S_DEVICES_LIST[] PROGMEM = {

  {	.manufacturer_name = "Unknown", .model_name = "Unknown",
    .z2s_device_uid = 0,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_NULL,
	  .z2s_device_endpoints_count = 0 },

  {	.manufacturer_name = "_TZ3000_akqdg6g7", .model_name = "TS0201",
    .z2s_device_uid = 100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_5nrcorgu", .model_name = "TS0201",
    .z2s_device_uid = 105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_i8jfiezr", .model_name = "TS0201",
    .z2s_device_uid = 110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_bgsigers", .model_name = "TS0201",
    .z2s_device_uid = 115,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_dowj6gyi", .model_name = "TS0201",
    .z2s_device_uid = 200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "Xiaomi", .model_name = "LYWSD03MMC-z",
    .z2s_device_uid = 300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Xiaomi", .model_name = "LYWSD03MMC",
    .z2s_device_uid = 400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_rdhukkmi", .model_name = "TS0201",
    .z2s_device_uid = 500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_fllyghyj", .model_name = "TS0201",
    .z2s_device_uid = 600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_v1w2k9dd", .model_name = "TS0201",
    .z2s_device_uid = 700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "TUYATEC-yg5dcbfu", .model_name = "RH3052",
    .z2s_device_uid = 800,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_upagmta9", .model_name = "TS0601",
    .z2s_device_uid = 900,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_1wnh8bqp", .model_name = "TS0601",
    .z2s_device_uid = 1000,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_d7lpruvi", .model_name = "TS0601",
    .z2s_device_uid = 1100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_9yapgbuv", .model_name = "TS0601",
    .z2s_device_uid = 1200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_cirvgep4", .model_name = "TS0601",
    .z2s_device_uid = 1300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_d7lpruvi", .model_name = "TS0601",
    .z2s_device_uid = 1400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_vaarscrt", .model_name = "TS0601",
    .z2s_device_uid = 1500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
	  .z2s_device_endpoints_count = 1}, //NOUS SZ-T04

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-227Z",
    .z2s_device_uid = 1600,
  	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
  	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_xr3htd96", .model_name = "TS0201",
    .z2s_device_uid = 1700,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "TH01",
    .z2s_device_uid = 1800,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-02",
    .z2s_device_uid = 1900,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-02D",
    .z2s_device_uid = 2000,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-02P",
    .z2s_device_uid = 2100,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-02WD",
    .z2s_device_uid = 2200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-02LD",
    .z2s_device_uid = 2300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPERATURE_SENSOR_POLL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_bjawzodf", .model_name = "TY0201",
    .z2s_device_uid = 2400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_a8sdabtg", .model_name = "TS0601",
    .z2s_device_uid = 2500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_vs0skpuc", .model_name = "TS0601",
    .z2s_device_uid = 2600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_fllyghyj", .model_name = "SNZB-02",
    .z2s_device_uid = 2700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "TH01",
    .z2s_device_uid = 2800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.weather",
    .z2s_device_uid = 2900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_bguser20", .model_name = "TS0201",
    .z2s_device_uid = 3000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_yujem9ee", .model_name = "TS0201",
    .z2s_device_uid = 3100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sens",
    .z2s_device_uid = 3200,
	. z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_hzkeh3nv", .model_name = "TS0201",
    .z2s_device_uid = 3300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_ncw88jfq", .model_name = "TS0201",
    .z2s_device_uid = 3400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_aao3yzhs", .model_name = "TS0601",
    .z2s_device_uid = 3500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_awepdiwi", .model_name = "TS0601",
    .z2s_device_uid = 3600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_myd45weu", .model_name = "TS0601",
    .z2s_device_uid = 3700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_0hkmcrza", .model_name = "TS0203",
   .z2s_device_uid = 3800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mcxw5ehu", .model_name = "TS0202",
   .z2s_device_uid = 3900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TYZB01_jytabjkb", .model_name = "TS0202",
   .z2s_device_uid = 3905,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_lltemgsf", .model_name = "TS0202",
   .z2s_device_uid = 3910,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TYZB01_5nr7ncpl", .model_name = "TS0202",
   .z2s_device_uid = 3915,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mg4dy6z6", .model_name = "TS0202",
   .z2s_device_uid = 3920,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_bsvqrxru", .model_name = "TS0202",
   .z2s_device_uid = 3925,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_26fmupbb", .model_name = "TS0203",
    .z2s_device_uid = 4000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_rcuyhwe3", .model_name = "TS0203",
    .z2s_device_uid = 4100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_2mbfxlzr", .model_name = "TS0203",
    .z2s_device_uid = 4200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_x8q36xwf", .model_name = "TS0203",
    .z2s_device_uid = 4300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_zutizvyk", .model_name = "TS0203",
    .z2s_device_uid = 4400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_bh3n6gk8", .model_name = "TS0601",
    .z2s_device_uid = 4500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_k4ej3ww2", .model_name = "TS0207",
    .z2s_device_uid = 4600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_qhozxs2b", .model_name = "TS0207",
    .z2s_device_uid = 4605,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_85czd6fy", .model_name = "TS0207",
    .z2s_device_uid = 4700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_upgcbody", .model_name = "TS0207",
    .z2s_device_uid = 4800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_wuep9zng", .model_name = "TS0207",
    .z2s_device_uid = 4900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_otwpdq1d", .model_name = "TS0207",
    .z2s_device_uid = 5000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_upgcbody", .model_name = "SNZB-05",
    .z2s_device_uid = 5100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_lf56vpxj", .model_name = "TS0202",
    .z2s_device_uid = 5200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_decxrtwa", .model_name = "TS0203",
    .z2s_device_uid = 5300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 2},

  {	.manufacturer_name = "_TZ3000_yxqnffam", .model_name = "TS0203",
    .z2s_device_uid = 5400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_lqpt3mvr", .model_name = "TS0210",
    .z2s_device_uid = 5500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "PARASOLL Door/Window Sensor",
    .z2s_device_uid = 5600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1},
                              { 2, Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2 }}},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "BADRING Water Leakage Sensor",
    .z2s_device_uid = 5700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_3towulqd", .model_name = "TS0601",
   .z2s_device_uid = 5800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_1bwpjvlz", .model_name = "TS0203",
   .z2s_device_uid = 5900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-04",
   .z2s_device_uid = 6000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_au1rjicn", .model_name = "TS0203",
   .z2s_device_uid = 6100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_bb6xaihh", .model_name = "SNZB-03",
    .z2s_device_uid = 6200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-03",
    .z2s_device_uid = 6300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "MS01",
    .z2s_device_uid = 6400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "DS01",
    .z2s_device_uid = 6500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_n2egfsli", .model_name = "SNZB-04",
    .z2s_device_uid = 6600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_pay2byax", .model_name = "TS0601",
    .z2s_device_uid = 6700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_n8dljorx", .model_name = "TS0601",
    .z2s_device_uid = 6800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_up3pngle", .model_name = "TS0205",
    .z2s_device_uid = 6900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_c8ozah8n", .model_name = "TS0202",
    .z2s_device_uid = 7000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_6zvw8ham", .model_name = "TS0203",
    .z2s_device_uid = 7100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3040_bb6xaihh", .model_name = "TS0202",
    .z2s_device_uid = 7200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.magnet.ac01",
    .z2s_device_uid = 7300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.magnet.agl02",
    .z2s_device_uid = 7400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sensor_magnet.aq2",
    .z2s_device_uid = 7500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sensor_motion.aq2",
    .z2s_device_uid = 7600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_tgvtvdoc", .model_name = "TS0207",
    .z2s_device_uid = 7700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_p68kms0l", .model_name = "TS0207",
    .z2s_device_uid = 7701,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_u6x1zyv2", .model_name = "TS0601",
    .z2s_device_uid = 7800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Heiman", .model_name = "SMOK_V16",
    .z2s_device_uid = 7900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Heiman", .model_name = "GAS_V15",
    .z2s_device_uid = 8000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_iba1ckek", .model_name = "TS0601",
    .z2s_device_uid = 8100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_6ygjfyll", .model_name = "TS0202",
    .z2s_device_uid = 8200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_7d8yme6f", .model_name = "TS0203",
    .z2s_device_uid = 8300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_bpkijo14", .model_name = "TS0203",
    .z2s_device_uid = 8400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_oxslv1c9", .model_name = "TS0203",
    .z2s_device_uid = 8500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HEIMAN", .model_name = "SmokeSensor-EF-3.0",
    .z2s_device_uid = 8600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_lzdjjfss", .model_name = "TS0210",
    .z2s_device_uid = 8700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "ZB-DoorSensor-D0007",
    .z2s_device_uid = 8800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_d93wtje5", .model_name = "TS0203",
    .z2s_device_uid = 8900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_8yhypbo7", .model_name = "TS0203",
    .z2s_device_uid = 9000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-222Z",
    .z2s_device_uid = 9100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-102Z",
    .z2s_device_uid = 9200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Develco Products A/S", .model_name = "WISZB-120",
    .z2s_device_uid = 9300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_DEVELCO_IAS_ZONE_TEMP_SENSOR,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 35, Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B },
                             { 38, Z2S_DEVICE_DESC_TEMPERATURE_SENSOR }}},

  {	.manufacturer_name = "Develco Products A/S", .model_name = "SPLZB-132",
    .z2s_device_uid = 9400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 2, Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER },
                             { 1, Z2S_DEVICE_DESC_NULL }}},

  {	.manufacturer_name = "Develco Products A/S", .model_name = "AQSZB-110",
   .z2s_device_uid = 9500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_DEVELCO_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 38, Z2S_DEVICE_DESC_DEVELCO_AIR_QUALITY_SENSOR },
                             { 1, Z2S_DEVICE_DESC_NULL }}},

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-05P",
    .z2s_device_uid = 9600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-204Z",
    .z2s_device_uid = 9700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-04P",
    .z2s_device_uid = 9800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "ZB-SMART-PIRTH-V3",
    .z2s_device_uid = 9900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-03P",
    .z2s_device_uid = 10000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_prits6g4", .model_name = "TS0001",
    .z2s_device_uid = 10100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS0001",
    .z2s_device_uid = 10200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS000F",
    .z2s_device_uid = 10300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_q6a3tepg", .model_name = "TS0001",
    .z2s_device_uid = 10400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_tqlv4ug4", .model_name = "TS0001",
    .z2s_device_uid = 10500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SWITCH-ZR03-1",
    .z2s_device_uid = 10600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "S26R2ZB",
    .z2s_device_uid = 10700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_cmcjbqup", .model_name = "TS0001",
    .z2s_device_uid = 10800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Aqara", .model_name = "lumi.valve.agl001",
    .z2s_device_uid = 10900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ahxwesag", .model_name = "TS0011",
    .z2s_device_uid = 11000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_8n7lqbm0", .model_name = "TS0001",
    .z2s_device_uid = 11100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.switch.b1lc04",
    .z2s_device_uid = 11200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SWITCH,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI control outlet",
    .z2s_device_uid = 11300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRETAKT Smart plug",
    .z2s_device_uid = 11400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "LDSENK01F",
    .z2s_device_uid = 11500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mmkbptmx", .model_name = "TS0004",
    .z2s_device_uid = 11600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 4, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_cehuw1lw", .model_name = "TS011F",
    .z2s_device_uid = 11700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A,
	  .z2s_device_endpoints_count = 1}, //A*1000

  {	.manufacturer_name = "_TZ3000_okaz9tjs", .model_name = "TS011F",
    .z2s_device_uid = 11800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_5f43h46b", .model_name = "TS011F",
    .z2s_device_uid = 11900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_w0qqde0g", .model_name = "TS011F",
    .z2s_device_uid = 12000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ww6drja5", .model_name = "TS011F",
    .z2s_device_uid = 12100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_gjnozsaz", .model_name = "TS011F",
    .z2s_device_uid = 12200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.plug.mmeu01",
    .z2s_device_uid = 12300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_2putqrmw", .model_name = "TS011F",
    .z2s_device_uid = 12400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "TS011F",
    .z2s_device_uid = 12500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_2uollq9d", .model_name = "TS011F",
    .z2s_device_uid = 12600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_2uollq9d", .model_name = "TS011F",
    .z2s_device_uid = 12700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_qeuvnohg", .model_name = "TS011F",
    .z2s_device_uid = 12800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_fukaa7nc", .model_name = "TS011F",
    .z2s_device_uid = 12900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_5ct6e7ye", .model_name = "TS011F",
    .z2s_device_uid = 13000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_kqvb5akv", .model_name = "TS0001",
    .z2s_device_uid = 13100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A,
	  .z2s_device_endpoints_count = 1}, //A*1000

  {	.manufacturer_name = "_TZ3000_zrm3oxsh", .model_name = "TS011F",
    .z2s_device_uid = 13105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "LDSENK02F",
    .z2s_device_uid = 13200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "INSPELNING Smart plug",
    .z2s_device_uid = 13300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_wkai4ga5", .model_name = "TS0044",
    .z2s_device_uid = 13400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_mh9px7cq", .model_name = "TS0044",
    .z2s_device_uid = 13500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_j61x9rxn", .model_name = "TS0044",
    .z2s_device_uid = 13505,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_a4xycprs", .model_name = "TS0044",
    .z2s_device_uid = 13510,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_dziaict4", .model_name = "TS0044",
    .z2s_device_uid = 13515,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_u3nv1jwk", .model_name = "TS0044",
    .z2s_device_uid = 13520,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_bgtzm4ny", .model_name = "TS0044",
    .z2s_device_uid = 13525,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_ee8nrt2l", .model_name = "TS0044",
    .z2s_device_uid = 13530,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_kfu8zapd", .model_name = "TS0044",
    .z2s_device_uid = 13600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_zgyzgdua", .model_name = "TS0044",
    .z2s_device_uid = 13700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_famkxci2", .model_name = "TS0043",
    .z2s_device_uid = 13800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_sj7jbgks", .model_name = "TS0043",
    .z2s_device_uid = 13900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  {	.manufacturer_name = "_TZ3000_yj6k7vfo", .model_name = "TS0041",
    .z2s_device_uid = 14000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_4fjiwweb", .model_name = "TS004F",
    .z2s_device_uid = 14100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_uri7ongn", .model_name = "TS004F",
    .z2s_device_uid = 14200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ixla93vd", .model_name = "TS004F",
    .z2s_device_uid = 14300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_qja6nq5z", .model_name = "TS004F",
    .z2s_device_uid = 14400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_abrsvsou", .model_name = "TS004F",
    .z2s_device_uid = 14500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_402vrq2i", .model_name = "TS004F",
    .z2s_device_uid = 14600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ja5osu5g", .model_name = "TS004F",
    .z2s_device_uid = 14700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_kjfzuycl", .model_name = "TS004F",
    .z2s_device_uid = 14705,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_egvb1p2g", .model_name = "TS004F",
    .z2s_device_uid = 14710,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_kaflzta4", .model_name = "TS004F",
    .z2s_device_uid = 14715,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_lrfvzq1e", .model_name = "TS004F",
    .z2s_device_uid = 14720,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mrpevh8p", .model_name = "TS0041",
    .z2s_device_uid = 14800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_8rppvwda", .model_name = "TS0041",
    .z2s_device_uid = 14900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_5bpeda8u", .model_name = "TS0041",
    .z2s_device_uid = 15000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-101ZL",
    .z2s_device_uid = 15100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-01P",
    .z2s_device_uid = 15200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-01",
    .z2s_device_uid = 15205,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "CK-TLSR8656-SS5-01(7000)",
    .z2s_device_uid = 15300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sensor_86sw1",
    .z2s_device_uid = 15400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "Remote Control N2",
    .z2s_device_uid = 15500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "SYMFONISK sound remote gen2",
    .z2s_device_uid = 15600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints =  {{ 1, Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1 },
                              { 2, Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2 },
                              { 3, Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3 }}},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "SYMFONISK Sound Controller",
    .z2s_device_uid = 15700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI on/off switch",
    .z2s_device_uid = 15800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "RODRET Dimmer",
    .z2s_device_uid = 15900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "SOMRIG shortcut button",
    .z2s_device_uid = 16000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints =  {{ 1, Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1 },
                              { 2, Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2 }}},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "VALLHORN Wireless Motion Sensor",
    .z2s_device_uid = 16100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_1,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_IKEA_VALLHORN_1 },
                              { 2, Z2S_DEVICE_DESC_IKEA_VALLHORN_2 },
                              { 3, Z2S_DEVICE_DESC_IKEA_VALLHORN_3 }}},

  {	.manufacturer_name = "Philips", .model_name = "RWL021",
    .z2s_device_uid = 16200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints =  {{ 1, Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1 },
                              { 2, Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2}}},

  {	.manufacturer_name = "_TZ3000_gjrubzje", .model_name = "TS0001",
    .z2s_device_uid = 16300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "ZBMINIL2",
    .z2s_device_uid = 16400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "ZBMicro",
    .z2s_device_uid = 16405,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "ZBMINIR2",
    .z2s_device_uid = 16500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "01MINIZB",
    .z2s_device_uid = 16600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_fdxihpp7", .model_name = "TS0001",
    .z2s_device_uid = 16700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

// TUYA HVAC SECTION
  
// SASWELL
  {	.manufacturer_name = "_TZE200_0dvm9mva", .model_name = "TS0601",
    .z2s_device_uid = 16800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_fqm2sfpe", .model_name = "TS0601",
    .z2s_device_uid = 16900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_9gvruqf5", .model_name = "TS0601",
    .z2s_device_uid = 17000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_zr9c0day", .model_name = "TS0601",
    .z2s_device_uid = 17005,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_c88teujp", .model_name = "TS0601",
    .z2s_device_uid = 17010,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_yw7cahqs", .model_name = "TS0601",
    .z2s_device_uid = 17015,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_azqp6ssj", .model_name = "TS0601",
    .z2s_device_uid = 17020,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_zuhszj9s", .model_name = "TS0601",
    .z2s_device_uid = 17025,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_h4cgnbzg", .model_name = "TS0601",
    .z2s_device_uid = 17030,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_gd4rvykv", .model_name = "TS0601",
    .z2s_device_uid = 17035,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_exfrnlow", .model_name = "TS0601",
    .z2s_device_uid = 17040,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_9m4kmbfu", .model_name = "TS0601",
    .z2s_device_uid = 17045,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_3yp57tby", .model_name = "TS0601",
    .z2s_device_uid = 17050,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_7p8ugv8d", .model_name = "TS0601",
    .z2s_device_uid = 17055,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_3yp57tby", .model_name = "TS0601",
    .z2s_device_uid = 17060,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
	  .z2s_device_endpoints_count = 1},  

//ME167

  {	.manufacturer_name = "_TZE284_o3x45p96", .model_name = "TS0601",
    .z2s_device_uid = 17100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_ybsqljjg", .model_name = "TS0601",
    .z2s_device_uid = 17105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_9xfjixap", .model_name = "TS0601",
    .z2s_device_uid = 17200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_rxq4iti9", .model_name = "TS0601",
    .z2s_device_uid = 17300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_bvu2wnxz", .model_name = "TS0601",
    .z2s_device_uid = 17400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_6rdj8dzm", .model_name = "TS0601",
    .z2s_device_uid = 17500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_p3dbf6qs", .model_name = "TS0601",
   .z2s_device_uid = 17600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_rxntag7i", .model_name = "TS0601",
   .z2s_device_uid = 17700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_ymldrmzx", .model_name = "TS0601",
   .z2s_device_uid = 17800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV603,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_cpmgn2cf", .model_name = "TS0601",
    .z2s_device_uid = 17900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_MOES,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_b6wax7g0", .model_name = "TS0601",
    .z2s_device_uid = 18000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_BECA,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_rtrmfadk", .model_name = "TS0601",
    .z2s_device_uid = 18100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601,
	  .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_rtrmfadk", .model_name = "TS0601",
    .z2s_device_uid = 18200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601,
  	.z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_dv8abrrz", .model_name = "TS0601",
    .z2s_device_uid = 18205,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601,
  	.z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_pbo8cj0z", .model_name = "TS0601",
    .z2s_device_uid = 18300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_GTZ10,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_ltwbm23f", .model_name = "TS0601",
    .z2s_device_uid = 18400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_mudxchsu", .model_name = "TS0601",
    .z2s_device_uid = 18450,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TV02,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "TRVZB",
    .z2s_device_uid = 18500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_TRVZB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_keabpigv", .model_name = "TS0505A",
    .z2s_device_uid = 18600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "zbeacon", .model_name = "TS0505",
    .z2s_device_uid = 18700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "TZ3000_riwp3k79", .model_name = "TS0505A",
    .z2s_device_uid = 18705,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "TZ3000_8uaoilu9", .model_name = "TS0502A",
    .z2s_device_uid = 18750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E27 CWS globe 806lm",
    .z2s_device_uid = 18800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E14 CWS globe 806lm",
    .z2s_device_uid = 18900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E14 CWS globe 806lm",
    .z2s_device_uid = 19000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Philips", .model_name = "LCA006",
    .z2s_device_uid = 19050,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "\u001aTRADFRI bulb GU10 WW 345lm",
    .z2s_device_uid = 19100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E27 WW globe 806lm",
    .z2s_device_uid = 19105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E27 WW G95 CL 470lm",
    .z2s_device_uid = 19110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},
  
  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 WW 345lm",
    .z2s_device_uid = 19200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "\\u001TRADFRI bulb GU10 WW 345lm",
    .z2s_device_uid = 19300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "\u001aTRADFRI bulb GU10 WW 345lm8",
    .z2s_device_uid = 19400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 WW 380lm",
    .z2s_device_uid = 19500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI Driver 10W",
    .z2s_device_uid = 19600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

   {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI Driver 30W",
    .z2s_device_uid = 19700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Philips", .model_name = "LWW003",
    .z2s_device_uid = 19750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Philips", .model_name = "LWF003",
    .z2s_device_uid = 19755,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_ctv0gump", .model_name = "TS0501B",
    .z2s_device_uid = 19800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_bfwvfyx1", .model_name = "TS0505B",
    .z2s_device_uid = 19900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0505B",
    .z2s_device_uid = 19905,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_3lbtuxgp", .model_name = "TS0505B",
    .z2s_device_uid = 20000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_wbsgmojq", .model_name = "TS0505B",
    .z2s_device_uid = 20100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_ljoasixl", .model_name = "TS0505B",
    .z2s_device_uid = 20200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_0zabbfax", .model_name = "TS0503B",
    .z2s_device_uid = 20300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_f0byevky", .model_name = "TS0503B",
    .z2s_device_uid = 20305,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "CK-BL702-AL-01(7009_Z102LG03-1)",
    .z2s_device_uid = 20400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB_XY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLight", .model_name = "ZB-CL01",
    .z2s_device_uid = 20500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB_XY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_hhiodade", .model_name = "TS0011",
    .z2s_device_uid = 20600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1,
  	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ji4araar", .model_name = "TS0011",
    .z2s_device_uid = 20700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1,
  	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_aetquff4", .model_name = "TS0011",
    .z2s_device_uid = 20800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1,
  	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "TZ3000_hafsqare", .model_name = "TS0011",
    .z2s_device_uid = 20900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_c7xsiexw", .model_name = "TS0002",
    .z2s_device_uid = 21000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_nuenzetq", .model_name = "TS0002",
    .z2s_device_uid = 21005,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_criiahcg", .model_name = "TS0002",
    .z2s_device_uid = 21100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_zmy4lslw", .model_name = "TS0002",
    .z2s_device_uid = 21200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_p8alo7qa", .model_name = "TS0012",
    .z2s_device_uid = 21300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_jl7qyupf", .model_name = "TS0012",
    .z2s_device_uid = 21400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_ljhbw1c9", .model_name = "TS0012",
    .z2s_device_uid = 21500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_xk5udnd6", .model_name = "TS0012",
    .z2s_device_uid = 21600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_18ejxno0", .model_name = "TS0012",
    .z2s_device_uid = 21700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_zmlunnhy", .model_name = "TS0012",
    .z2s_device_uid = 21800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_e98krvvk", .model_name = "TS0012",
    .z2s_device_uid = 21900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},


  {	.manufacturer_name = "_TZ3000_aezbqpcu", .model_name = "TS0013",
    .z2s_device_uid = 22000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_ltgngnqz", .model_name = "TS0002",
    .z2s_device_uid = 22100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_sueu2ppq", .model_name = "TS0003",
    .z2s_device_uid = 22200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_mw1pqqqt", .model_name = "TS0003",
    .z2s_device_uid = 22300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_imaccztn", .model_name = "TS0004",
    .z2s_device_uid = 22400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 4, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1}}},

  {	.manufacturer_name = "_TZ3000_h1ipgkwn", .model_name = "TS0002",
    .z2s_device_uid = 22500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 2, Z2S_DEVICE_DESC_TUYA_RELAY }}},

  {	.manufacturer_name = "_TZ3000_5ajpkyq6", .model_name = "TS0004",
    .z2s_device_uid = 22600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 2, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 3, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 4, Z2S_DEVICE_DESC_TUYA_RELAY }}},

  {	.manufacturer_name = "_TZE204_jtbgusdc", .model_name = "TS0601",
    .z2s_device_uid = 22700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_rccxox8p", .model_name = "TS0601",
    .z2s_device_uid = 22800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_rccxox8p", .model_name = "TS0601",
    .z2s_device_uid = 22900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_ntcy3xu1", .model_name = "TS0601",
    .z2s_device_uid = 23000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_ai4rqhky", .model_name = "TS0601",
    .z2s_device_uid = 23100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_n4ttsck2", .model_name = "TS0601",
    .z2s_device_uid = 23200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2,
	  .z2s_device_endpoints_count = 1}, //ONENUO

  {	.manufacturer_name = "_TZE284_rjxqso4a", .model_name = "TS0601",
    .z2s_device_uid = 23300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_chbyv06x", .model_name = "TS0601",
    .z2s_device_uid = 23400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_mja3fuja", .model_name = "TS0601",
    .z2s_device_uid = 23500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_2aaelwxk", .model_name = "TS0225",
    .z2s_device_uid = 23600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_kb5noeto", .model_name = "TS0601",
    .z2s_device_uid = 23700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_ex3rcdha", .model_name = "TS0601",
    .z2s_device_uid = 23800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_rhgsbacq", .model_name = "TS0601",
    .z2s_device_uid = 23900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-204ZV",
    .z2s_device_uid = 24000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_9kbbfeho", .model_name = "TS0222",
    .z2s_device_uid = 24100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_hy6ncvmw", .model_name = "TS0222",
    .z2s_device_uid = 24200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_3ulg9kpo", .model_name = "TS0021",
    .z2s_device_uid = 24300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3,
	  .z2s_device_endpoints_count = 1},

  //{	.manufacturer_name = "_TZE200_nojsjtj2", .model_name = "TS0601",
  // .z2s_device_uid = 24400,
	//  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOS_BUTTON,
	//  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_dikb3dp6", .model_name = "TS0601",
    .z2s_device_uid = 24500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_iwn0gpzz", .model_name = "TS0601",
    .z2s_device_uid = 24600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "SWV",
    .z2s_device_uid = 24800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_VALVE,
	  .z2s_device_endpoints_count = 1},

   {	.manufacturer_name = "_TZE204_a7sghmms", .model_name = "TS0601",
    .z2s_device_uid = 24900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_GIEX_SMART_VALVE,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3290_7v1k4vufotpowp9z", .model_name = "TS1201",
    .z2s_device_uid = 25000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IR_REMOTE_CONTROL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_ol1uhvza", .model_name = "TS130F",
    .z2s_device_uid = 25100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 1},

   {	.manufacturer_name = "_TZ3000_fccpjz5z", .model_name = "TS130F",
    .z2s_device_uid = 25200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_femsaaua", .model_name = "TS130F",
    .z2s_device_uid = 25300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE},
                              { 2, Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE }}},

  {	.manufacturer_name = "_TZ3000_5iixzdo7", .model_name = "TS130F",
    .z2s_device_uid = 25400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE},
                              { 2, Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE }}},

  {	.manufacturer_name = "_TZ3000_c2yscrj8", .model_name = "TS0224",
    .z2s_device_uid = 25500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SIREN_ALARM,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_cipibmnp", .model_name = "TS0224",
    .z2s_device_uid = 25600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SIREN_ALARM,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "ZB-Remote-D0001",
    .z2s_device_uid = 25700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "LDSENK08",
    .z2s_device_uid = 24700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ADEO", .model_name = "LDSENK09",
    .z2s_device_uid = 25800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_icka1clh", .model_name = "TS0601",
    .z2s_device_uid = 25900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_68nvbio9", .model_name = "TS0601",
    .z2s_device_uid = 26000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_t1blo2bj", .model_name = "TS0601",
    .z2s_device_uid = 26100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_ALARM,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_wzk0x7fq", .model_name = "TS0601",
    .z2s_device_uid = 26200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-102ZM",
    .z2s_device_uid = 26300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_n0lphcok", .model_name = "TS0001",
    .z2s_device_uid = 26400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_REPEATER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_zm8zpwas", .model_name = "TS0601",
    .z2s_device_uid = 26500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.remote.cagl02",
    .z2s_device_uid = 26600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_1},
                              { 2, Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_2 }}},

  {	.manufacturer_name = "SONOFF", .model_name = "S60ZBTPF",
    .z2s_device_uid = 26700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "CK-BL702-SWP-01(7020)",
    .z2s_device_uid = 26800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_7kscdesh", .model_name = "TS0222",
    .z2s_device_uid = 26900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_DP_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-204ZL",
    .z2s_device_uid = 26905,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR ,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.airmonitor.acn01",
    .z2s_device_uid = 27000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1}
};
#endif