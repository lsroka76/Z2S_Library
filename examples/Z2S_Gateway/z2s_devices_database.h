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
#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_MCU_VERSION        (1 << 12)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_FORCE_TIME_SYNC    (1 << 13)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_USE_SEND_DATA      (1 << 14)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT               (1 << 15)


#define Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_ENROLL_1       (1 << 16)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_ENROLL_2       (1 << 17)

#define Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY   (1 << 20)
#define Z2S_DEVICE_DESC_CONFIG_FLAG_ON_OFF_REJOIN_QUERY     (1 << 21)

#define Z2S_DEVICE_DESC_NULL                                0x0000
#define Z2S_DEVICE_DESC_TUYA_QUERY                          0x0080

#define Z2S_DEVICE_DESC_LOCAL_ACTION_HANDLER                0x0100

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR                 0x1000
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1               0x1001
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL            0x1002
#define Z2S_DEVICE_DESC_TEMPERATURE_SENSOR_POLL             0x1003

#define Z2S_DEVICE_DESC_TEMPERATURE_SENSOR                  0x1005

#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR       0x1010
#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_1     0x1011
#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_2     0x1012
#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_3     0x1013

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10         0x1020

#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR            0x1030

#define Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR             0x1050
#define Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR        0x1055

#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR       0x1100
#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1     0x1101

#define Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F                 0x1110
#define Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F_2               0x1111
#define Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_4F                 0x1112

#define Z2S_DEVICE_DESC_TUYA_TH_SENSOR_TEMP_PROBE           0x1150

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
#define Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR_2                0x2012 

#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B        0x2020

#define Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR               0x2030
#define Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR_2             0x2031

#define Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR                0x2040   
#define Z2S_DEVICE_DESC_TUYA_IAS_ZONE_1_B_SENSOR            0x2041
#define Z2S_DEVICE_DESC_DEVELCO_IAS_ZONE_TEMP_SENSOR        0x2050
#define Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR         0x2060

#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR                 0x2200
#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1               0x2201
#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2               0x2202 //ONENUO
#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_228WZH          0x2205

#define Z2S_DEVICE_DESC_TUYA_CO_DETECTOR                    0x2210
#define Z2S_DEVICE_DESC_TUYA_CO2_DETECTOR                   0x2215
#define Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR                   0x2220
#define Z2S_DEVICE_DESC_TUYA_CO_GAS_DETECTOR                0x2225

#define Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR             0x2230
#define Z2S_DEVICE_DESC_DEVELCO_AIR_QUALITY_SENSOR          0x2250
#define Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR             0x2260
#define Z2S_DEVICE_DESC_IKEA_AIR_QUALITY_SENSOR             0x2270

#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR                0x2300
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_1              0x2301
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5              0x2305
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1           0x2310
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY          0x2315
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_NEO            0x2320
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205Z         0x2325
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205ZL        0x2326
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM100S2       0x2330
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM10024GV3    0x2331
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG204ZE        0x2335
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_MWPS3Z         0x2340

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
#define Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT                0x3112
#define Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00                0x3113

#define Z2S_DEVICE_DESC_IKEA_RGBW_BULB                      0x3120
#define Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB                   0x3130
#define Z2S_DEVICE_DESC_ADEO_RGBW_BULB                      0x3140

#define Z2S_DEVICE_DESC_DIMMER_BULB                         0x3200
#define Z2S_DEVICE_DESC_LED_DIMMER                          0x3201
#define Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB                 0x3205
#define Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0               0x3210
#define Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0                  0x3215

#define Z2S_DEVICE_DESC_IKEA_WW_BULB                        0x3220
#define Z2S_DEVICE_DESC_IKEA_WS_BULB                        0x3221

#define Z2S_DEVICE_DESC_PHILIPS_WW_BULB                     0x3225
#define Z2S_DEVICE_DESC_TUYA_LED_DIMMER                     0x3230
#define Z2S_DEVICE_DESC_DIMMER_CT_BULB                      0x3235

#define Z2S_DEVICE_DESC_RGBW_END                            0x3299

#define Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH                  0x3300
#define Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH           0x3305

#define Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY          0x3400

#define Z2S_DEVICE_DESC_RELAY                               0x4000
#define Z2S_DEVICE_DESC_RELAY_1                             0x4001

#define Z2S_DEVICE_DESC_LUMI_SWITCH                         0x4010
#define Z2S_DEVICE_DESC_LUMI_DOUBLE_SWITCH                  0x4011

#define Z2S_DEVICE_DESC_TUYA_RELAY                          0x4020
#define Z2S_DEVICE_DESC_TUYA_DP_RELAY                       0x4025

#define Z2S_DEVICE_DESC_TUYA_LCD_3_RELAYS                   0x4030

#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER             0x4100
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1           0x4101
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2           0x4102

#define Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER      0x4200
#define Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER     0x4300
#define Z2S_DEVICE_DESC_BOSCH_RELAY_ELECTRICITY_METER       0x4400

#define Z2S_DEVICE_DESC_LUMI_SMART_WALL_OUTLET              0x4450

#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER        0x4500
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1      0x4501
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2      0x4502
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A      0x4503

#define Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP            0x4540
#define Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP               0x4541

#define Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER      0x4550
#define Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER       0x4551

#define Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1                  0x4600
#define Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2                  0x4601

#define Z2S_DEVICE_DESC_TUYA_1GANG_SWITCH                   0x4604
#define Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH                   0x4605
#define Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH                   0x4606
#define Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH                   0x4607

#define Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES      0x4650
#define Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER         0x4660
#define Z2S_DEVICE_DESC_TUYA_4_RELAYS_DP_CONTROLLER         0x4665

#define Z2S_DEVICE_DESC_TUYA_5_RELAYS_CONTROLLER            0x4670

#define Z2S_DEVICE_DESC_ON_OFF_VALVE_DC                     0x4700
#define Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC                0x4705
#define Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY           0x4710

#define Z2S_DEVICE_DESC_WINDOW_COVERING_SINGLE              0x4800
#define Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE         0x4810
#define Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_DOUBLE         0x4815
#define Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR             0x4820
#define Z2S_DEVICE_DESC_ZEMISMART_SHADES_DRIVE_MOTOR        0x4821
#define Z2S_DEVICE_DESC_MOES_COVER                          0x4822
#define Z2S_DEVICE_DESC_CURRYSMARTER_COVER                  0x4823
#define Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE      0x4830
#define Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER                 0x4850
#define Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER_1               0x4851

#define Z2S_DEVICE_DESC_SONOFF_SMART_VALVE                  0x4900
#define Z2S_DEVICE_DESC_GIEX_SMART_VALVE                    0x4910

#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F                0x5000
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F                0x5001
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F                0x5002

#define Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB20          0x5020
#define Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB21          0x5021

#define Z2S_DEVICE_DESC_TUYA_SWITCH_4X3                     0x5100

#define Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3                0x5105
#define Z2S_DEVICE_DESC_TUYA_SOS_BUTTON                     0x5110

#define Z2S_DEVICE_DESC_SMART_BUTTON_2F                     0x5150

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
#define Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F            0x5305

#define Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F        0x5400
#define Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F                0x5410
#define Z2S_DEVICE_DESC_ADEO_ENKI_REMOTE_CONTROL            0x5415

#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH           0x5500
#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1         0x5501
#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2         0x5502

#define Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO                    0x5600
#define Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_1                  0x5601
#define Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_2                  0x5602

#define Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F                0x5610
#define Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_2F                0x5611

#define Z2S_DEVICE_DESC_HVAC_START                          0x6000

#define Z2S_DEVICE_DESC_TUYA_HVAC                           0x6000
#define Z2S_DEVICE_DESC_TUYA_HVAC_23457                     0x6001
#define Z2S_DEVICE_DESC_TUYA_HVAC_6567C                     0x6002
#define Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY                    0x6003

#define Z2S_DEVICE_DESC_SONOFF_TRVZB                        0x6005
#define Z2S_DEVICE_DESC_BOSCH_BTHRA                         0x6006
#define Z2S_DEVICE_DESC_EUROTRONIC_SPZB0001                 0x6007
#define Z2s_DEVICE_DESC_LUMI_TRV                            0x6008
                                                            //0x6009
                                                            //0x600A
                                                            //0x600B
                                                            //0x600C
                                                            //0x600D
                                                            //0x600E
                                                            //0x600F
                                                            
#define Z2S_DEVICE_DESC_TS0601_TRV_SASWELL                  0x6010
#define Z2S_DEVICE_DESC_TS0601_TRV_ME167                    0x6011
#define Z2S_DEVICE_DESC_TS0601_TRV_BECA                     0x6012
#define Z2S_DEVICE_DESC_TS0601_TRV_MOES                     0x6013
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV601                   0x6014
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV603                   0x6015
#define Z2S_DEVICE_DESC_TS0601_TRV_GTZ10                    0x6016
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z                  0x6017
#define Z2S_DEVICE_DESC_TS0601_TRV_TV02                     0x6018
#define Z2S_DEVICE_DESC_TS0601_TRV_SITERWELL                0x6019
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV16                    0x6020

#define Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198                   0x6200
#define Z2S_DEVICE_DESC_TS0601_MOES_BHT002                  0x6201
#define Z2S_DEVICE_DESC_TS0601_MOES_ZHTSR                   0x6202
#define Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100                   0x6203
#define Z2S_DEVICE_DESC_TS0601_BOTR9V                       0X6204

#define Z2S_DEVICE_DESC_HVAC_END                            0x6499

#define Z2S_DEVICE_DESC_IR_REMOTE_CONTROL                   0x6500
#define Z2S_DEVICE_DESC_TUYA_SIREN_ALARM                    0x6600
#define Z2S_DEVICE_DESC_MOES_ALARM                          0x6610

#define Z2S_DEVICE_DESC_TUYA_FINGERBOT_PLUS                 0x6700

#define Z2S_DEVICE_DESC_TUYA_REPEATER                       0x7000
#define Z2S_DEVICE_DESC_REPEATER                            0x7001

#define Z2S_DEVICE_DESC_ON_OFF                              0x8000
#define Z2S_DEVICE_DESC_ON_OFF_1                            0x8001

#define Z2S_DEVICE_DESC_LAST_ID                             0xFFFF

#define MAX_BOUND_ENDPOINTS                                 0x06
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

#define TUYA_PRESENCE_SENSOR_PRESENCE_SID                   0x00
//#define TUYA_PRESENCE_SENSOR_PRESENCE_SID                   0x01
#define TUYA_PRESENCE_SENSOR_MOTION_STATE_SID               0x02
#define TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID                0x03
#define TUYA_PRESENCE_SENSOR_TEMPHUMIDITY_SID               0x04
#define TUYA_PRESENCE_SENSOR_DISTANCE_SID                   0x05
#define TUYA_PRESENCE_SENSOR_RELAY_SWITCH_SID               0x20
#define TUYA_PRESENCE_SENSOR_RELAY_MODE_SID                 0x21

#define TUYA_SMOKE_DETECTOR_SMOKE_SID                       0x00
#define TUYA_SMOKE_DETECTOR_TAMPER_SID                      0x01
#define TUYA_SMOKE_DETECTOR_SELF_TEST_SID                   0x02
#define TUYA_SMOKE_DETECTOR_TEMPHUMIDITY_SID                0x03

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

#define TUYA_CO_GAS_DETECTOR_GAS_SID                        0x00 
#define TUYA_CO_GAS_DETECTOR_GAS_CONC_SID                   0x01
#define TUYA_CO_GAS_DETECTOR_CO_SID                         0x02 
#define TUYA_CO_GAS_DETECTOR_CO_CONC_SID                    0x03
#define TUYA_CO_GAS_DETECTOR_SELF_TEST_RESULT_SID           0x04

#define DEVELCO_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID         0x00
#define DEVELCO_AIR_QUALITY_SENSOR_VOC_SID                  0x01

#define LUMI_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID            0x00
#define LUMI_AIR_QUALITY_SENSOR_VOC_SID                     0x01
#define LUMI_AIR_QUALITY_SENSOR_AIR_QUALITY_SID             0x02

#define TUYA_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID            0x00
#define TUYA_AIR_QUALITY_SENSOR_CO2_SID                     0x01
#define TUYA_AIR_QUALITY_SENSOR_VOC_SID                     0x02
#define TUYA_AIR_QUALITY_SENSOR_FA_SID                      0x03 //formaldehyde

#define IKEA_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID            0x00
#define IKEA_AIR_QUALITY_SENSOR_PM25_SID                    0x01
#define IKEA_AIR_QUALITY_SENSOR_VOC_SID                     0x02


#define TUYA_RAIN_SENSOR_RAIN_SID                           0x01
#define TUYA_RAIN_SENSOR_ILLUMINANCE_SID                    0x02
#define TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID         0x03
#define TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID          0x04
#define TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID                 0x05

#define TUYA_SOIL_SENSOR_3F_WATER_WARNING_SID               0x00
#define TUYA_SOIL_SENSOR_3F_TH_SID                          0x01
#define TUYA_SOIL_SENSOR_3F_SOIL_MOISTURE_SID               0x02
#define TUYA_SOIL_SENSOR_4F_ILLUMINANCE_SID                 0x03

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

#define Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP_TEMP_SID      0x00
#define Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP_SWITCH_SID    0x01
#define Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP_EM_SID        0x02
#define Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP_FAULT_SID     0x03

#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_SID           0x00
#define TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_SID  0x01
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_SID            0x02
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_SID     0x03
#define TUYA_3PHASES_ELECTRICITY_METER_SID                  0x04

#define CUSTOM_CMD_BUTTON_PRESSED_SID                       0x00
#define CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID                0x01
#define CUSTOM_CMD_BUTTON_HELD_SID                          0x02
#define CUSTOM_CMD_BUTTON_TRIPLE_PRESSED_SID                0x03

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

//#define LIVARNO_DIMMER_SWITCH_REMOTE_TURN_ON_SID            0x00
#define LIVARNO_DIMMER_SWITCH_ON_PRESSED_SID                0x00
#define LIVARNO_DIMMER_SWITCH_OFF_PRESSED_SID               0x01
#define LIVARNO_DIMMER_SWITCH_DIM_UP_PRESSED_SID            0x02
#define LIVARNO_DIMMER_SWITCH_DIM_UP_HOLD_SID               0x03
#define LIVARNO_DIMMER_SWITCH_DIM_UP_LONG_PRESSED_SID       0x04
#define LIVARNO_DIMMER_SWITCH_DIM_DOWN_PRESSED_SID          0x05
#define LIVARNO_DIMMER_SWITCH_DIM_DOWN_HOLD_SID             0x06
#define LIVARNO_DIMMER_SWITCH_DIM_DOWN_LONG_PRESSED_SID     0x07

#define ADEO_ENKI_REMOTE_CONTROL_ON_PRESSED_SID             0x00
#define ADEO_ENKI_REMOTE_CONTROL_OFF_PRESSED_SID            0x01
#define ADEO_ENKI_REMOTE_CONTROL_UP_PRESSED_SID             0x02
//#define ADEO_ENKI_REMOTE_CONTROL_UP__LONG_PRESSED_SID       0x03
#define ADEO_ENKI_REMOTE_CONTROL_DOWN_PRESSED_SID           0x04
//#define ADEO_ENKI_REMOTE_CONTROL_DOWN_LONG_PRESSED_SID      0x05
#define ADEO_ENKI_REMOTE_CONTROL_ROUND_WUP_PRESSED_SID      0x06
#define ADEO_ENKI_REMOTE_CONTROL_ROUND_WDOWN_PRESSED_SID    0x07
#define ADEO_ENKI_REMOTE_CONTROL_ROUND_CUP_PRESSED_SID      0x08
#define ADEO_ENKI_REMOTE_CONTROL_ROUND_CDOWN_PRESSED_SID    0x09
#define ADEO_ENKI_REMOTE_CONTROL_ROUND_CLEFT_PRESSED_SID    0x0A
#define ADEO_ENKI_REMOTE_CONTROL_ROUND_CRIGHT_PRESSED_SID   0x0B
#define ADEO_ENKI_REMOTE_CONTROL_SCENE_1_PRESSED_SID        0x0C
#define ADEO_ENKI_REMOTE_CONTROL_SCENE_2_PRESSED_SID        0x0D
#define ADEO_ENKI_REMOTE_CONTROL_SCENE_3_PRESSED_SID        0x0E
#define ADEO_ENKI_REMOTE_CONTROL_SCENE_4_PRESSED_SID        0x0F

#define LUMI_DOUBLE_SWITCH_LEFT_PRESSED_SID                 0x00
#define LUMI_DOUBLE_SWITCH_LEFT_DOUBLE_PRESSED_SID          0x01
#define LUMI_DOUBLE_SWITCH_RIGHT_PRESSED_SID                0x02
#define LUMI_DOUBLE_SWITCH_RIGHT_DOUBLE_PRESSED_SID         0x03
#define LUMI_DOUBLE_SWITCH_BOTH_PRESSED_SID                 0x04
#define LUMI_DOUBLE_SWITCH_BOTH_DOUBLE_PRESSED_SID          0x05
//#define LUMI_DOUBLE_SWITCH_CHANGE_MODE_SID                  0x06
#define LUMI_DOUBLE_SWITCH_LEFT_SWITCH_SID                  0x07
#define LUMI_DOUBLE_SWITCH_RIGHT_SWITCH_SID                 0x08

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
#define TUYA_VIBRATION_SENSOR_X_Y_Z_POSITION_SID            0x12
#define TUYA_VIBRATION_SENSOR_VIBRATION_STATE_SID           0x13

#define TUYA_SOS_BUTTON_SOS_SID                             0x00
#define TUYA_SOS_BUTTON_EMERGENCY_SID                       0x01
#define TUYA_SOS_BUTTON_LOW_BATTERY_SID                     0x03 //IAS_ZONE_LOW_BATTERY_SID

#define TUYA_LCD_PANEL_3_RELAYS_RELAY_1_SID                 0x00
#define TUYA_LCD_PANEL_3_RELAYS_RELAY_2_SID                 0x01
#define TUYA_LCD_PANEL_3_RELAYS_RELAY_3_SID                 0x02

#define TUYA_LCD_PANEL_3_RELAYS_TURN_ON_SID                 0x03
#define TUYA_LCD_PANEL_3_RELAYS_TURN_OFF_SID                0x04
#define TUYA_LCD_PANEL_3_RELAYS_TV_MODE_SID                 0x05
#define TUYA_LCD_PANEL_3_RELAYS_MEETING_MODE_SID            0x06
#define TUYA_LCD_PANEL_3_RELAYS_SLEEP_MODE_SID              0x07
#define TUYA_LCD_PANEL_3_RELAYS_COFFE_BREAK_MODE_SID        0x08


#define TUYA_LCD_PANEL_3_RELAYS_SHUTTER_CLOSE_SID           0x09
#define TUYA_LCD_PANEL_3_RELAYS_SHUTTER_STOP_SID            0x0A
#define TUYA_LCD_PANEL_3_RELAYS_SHUTTER_OPEN_SID            0x0B

#define TUYA_LCD_PANEL_3_RELAYS_CURTAIN_MOVE_UP_SID         0x0C
#define TUYA_LCD_PANEL_3_RELAYS_CURTAIN_STOP_SID            0x0D
#define TUYA_LCD_PANEL_3_RELAYS_CURTAIN_MOVE_DOWN_SID       0x0E
#define TUYA_LCD_PANEL_3_RELAYS_AC_ON_SID                   0x0F
#define TUYA_LCD_PANEL_3_RELAYS_AC_OFF_SID                  0x10
#define TUYA_LCD_PANEL_3_RELAYS_COOL_ON_SID                 0x11
#define TUYA_LCD_PANEL_3_RELAYS_HEAT_ON_SID                 0x12

#define TUYA_X_RELAYS_CONTROLLER_RELAY_1_SID                0x01
#define TUYA_X_RELAYS_CONTROLLER_RELAY_2_SID                0x02
#define TUYA_X_RELAYS_CONTROLLER_RELAY_3_SID                0x03
#define TUYA_X_RELAYS_CONTROLLER_RELAY_4_SID                0x04
#define TUYA_X_RELAYS_CONTROLLER_RELAY_5_SID                0x05
#define TUYA_X_RELAYS_CONTROLLER_RELAY_6_SID                0x06
#define TUYA_X_RELAYS_CONTROLLER_RELAY_7_SID                0x07
#define TUYA_X_RELAYS_CONTROLLER_RELAY_8_SID                0x08
#define TUYA_8_RELAYS_CONTROLLER_STATUS_SID                 0x1B // ENUM ??
#define TUYA_8_RELAYS_CONTROLLER_LOCK_SID                   0x1D

#define TUYA_TH_SENSOR_TEMP_PROBE_INTERNAL_TH_SID           0x00
#define TUYA_TH_SENSOR_TEMP_PROBE_EXTERNAL_TEMP_SID         0x01


[[maybe_unused]]
static const char *IKEA_STYRBAR_BUTTONS[] PROGMEM = { 
    "ON PRESSED", 
    "ON HELD", 
    "OFF PRESSED", 
    "OFF HELD", 
    "LEFT PRESSED", 
    "LEFT HELD", 
    "RIGHT PRESSED", 
    "RIGHT HELD" };

[[maybe_unused]]
static const char *IKEA_SYMFONISK_BUTTONS[] PROGMEM = { 
    "PLAY", 
    "VOLUME UP", 
    "VOLUME DOWN", 
    "NEXT TRACK ", 
    "PREV TRACK",
    "DOT PRESSED", 
    "DOT SHORT RELEASE", 
    "DOT HELD", 
    "DOT LONG RELEASED", 
    "DOT DOUBLE PRESSED",
    "DOTS PRESSED", 
    "DOTS SHORT RELEASE", 
    "DOTS HELD", 
    "DOTS LONG RELEASED", 
    "DOTS DOUBLE PRESSED" };

[[maybe_unused]]
static const char *PHILIPS_HUE_DIMMER_SWITCH_BUTTONS[] PROGMEM = {  
    "ON_PRESS", 
    "ON_PRESS_RELEASE", 
    "ON_HOLD", 
    "ON_HOLD_RELEASE",
    "UP_PRESS", 
    "UP_PRESS_RELEASE", 
    "UP_HOLD", 
    "UP_HOLD_RELEASE",
    "DOWN_PRESS", 
    "DOWN_PRESS_RELEASE", 
    "DOWN_HOLD", 
    "DOWN_HOLD_RELEASE",
    "OFF_PRESS", 
    "OFF_PRESS_RELEASE", 
    "OFF_HOLD",
    "OFF_HOLD_RELEASE" };

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_QUERY, 
    .z2s_device_clusters_count = 1, 
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT |
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_QUERY,
    .z2s_device_clusters = {TUYA_PRIVATE_CLUSTER_EF00 }},

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
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPERATURE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT }},

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
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

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
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

    {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT ||
      Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},
    
    {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_1_B_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT ||
      Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_1,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_3,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_ON_OFF_REJOIN_QUERY,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
                             ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                           ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DP_RELAY,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LCD_3_RELAYS,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_4_RELAYS_DP_CONTROLLER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_5_RELAYS_CONTROLLER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_BOSCH_RELAY_ELECTRICITY_METER,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_METERING }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_WALL_OUTLET,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x00, //Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             LUMI_CUSTOM_CLUSTER }},

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
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1,
    .z2s_device_clusters_count = 4,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_SCENES,
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
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             IKEA_PRIVATE_CLUSTER_2 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { IKEA_PRIVATE_CLUSTER_2 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { PHILIPS_CUSTOM_CLUSTER,
                             ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT,
                             LUMI_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_DOUBLE_SWITCH,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},
                            // ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT,
                            // LUMI_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ACE }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F,
    .z2s_device_clusters_count = 2, 
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ADEO_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_ENKI_REMOTE_CONTROL,
    .z2s_device_clusters_count = 5, 
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC,
                             ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_BOSCH_BTHRA,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_EUROTRONIC_SPZB0001,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT }},

  {	.z2s_device_desc_id = Z2s_DEVICE_DESC_LUMI_TRV,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT,
                             LUMI_CUSTOM_CLUSTER }},


  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_REPEATER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_REPEATER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SIREN_ALARM,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_WD }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_BECA,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_MOES,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV603,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_GTZ10,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TV02,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SITERWELL,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV16,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_BOTR9V,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_MOES_BHT002,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_MOES_ZHTSR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             SONOFF_CUSTOM_CLUSTER_2 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_2F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SMART_BUTTON_2F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB20,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0, //Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_5, //0, //Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB21,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0, //Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_5, //Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LED_DIMMER,
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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_DIMMER_CT_BULB,
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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},
                             
  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                             ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_RGBW_BULB,
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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_PHILIPS_WW_BULB,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER,
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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0,
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
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0, //Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0, //Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_4F,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0, //Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT | 
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY |
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_MCU_VERSION,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT | 
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY |
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_MCU_VERSION,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_3,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_FORCE_TIME_SYNC,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},
	  
  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TH_SENSOR_TEMP_PROBE,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = 0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER_1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
                             ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_228WZH,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = 0, //Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_DETECTOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO2_DETECTOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_GAS_DETECTOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

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

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_AIR_QUALITY_SENSOR,
    .z2s_device_clusters_count = 5,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
                             ESP_ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT,
                             IKEA_CUSTOM_CLUSTER_FC7E }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_1,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_NEO,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205Z,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205ZL,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM100S2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM10024GV3,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG204ZE,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_MWPS3Z,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOS_BUTTON,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},
  
  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT |
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY,
    .z2s_device_clusters = { TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_IR_REMOTE_CONTROL,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ZOSUNG_IR_CONTROL_CUSTOM_CLUSTER }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_VALVE,
    .z2s_device_clusters_count = 3,
    .z2s_device_config_flags = 0x0,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_GIEX_SMART_VALVE,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             TUYA_PRIVATE_CLUSTER_EF00 }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_COVER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_CURRYSMARTER_COVER,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},


  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_ZEMISMART_SHADES_DRIVE_MOTOR,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_ALARM,
    .z2s_device_clusters_count = 1,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR_2,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  {	.z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_FINGERBOT_PLUS,
    .z2s_device_clusters_count = 2,
    .z2s_device_config_flags = Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT |
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_USE_SEND_DATA,
    .z2s_device_clusters = { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                             ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }}
};

static const z2s_device_entity_t Z2S_DEVICES_LIST[] PROGMEM = {
//DEVICES_START
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

  {	.manufacturer_name = "_TZ3000_zl1kmjqx", .model_name = "", //only manufacturer name
    .z2s_device_uid = 120,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0201",
    .z2s_device_uid = 125,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0202",
    .z2s_device_uid = 130,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0203",
    .z2s_device_uid = 135,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_bgsigers", .model_name = "",
    .z2s_device_uid = 140,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_kkerjand", .model_name = "TS0601",
    .z2s_device_uid = 145,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "_TZ3000_dowj6gyi", .model_name = "TS0201",
    .z2s_device_uid = 200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "AoYan ", .model_name = "TS0201",
    .z2s_device_uid = 205,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
	  .z2s_device_endpoints_count = 1 },

  {	.manufacturer_name = "Wing", .model_name = "TS0201",
    .z2s_device_uid = 210,
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

  {	.manufacturer_name = "_TZE284_upagmta9", .model_name = "TS0601",
    .z2s_device_uid = 905,
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

  {	.manufacturer_name = "_TZE200_cirvgep4", .model_name = "TS0601",
    .z2s_device_uid = 1305,
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

  {	.manufacturer_name = "TELINK", .model_name = "TLSR82xx",
    .z2s_device_uid = 1905,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "CK-TLSR8656-SS5-01(7014)",
    .z2s_device_uid = 1910,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
	.z2s_device_endpoints_count = 1},


  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-02D",
    .z2s_device_uid = 2000,
	.z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL,
	.z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-02DR2",
    .z2s_device_uid = 2050,
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

  {	.manufacturer_name = "_TZ2000_a476raq2", .model_name = "TS0201",
    .z2s_device_uid = 2405,
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

  {	.manufacturer_name = "lumi", .model_name = "lumi.sens",
    .z2s_device_uid = 3205,
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

  {	.manufacturer_name = "_TZ3000_isw9u95y", .model_name = "TS0201",
    .z2s_device_uid = 3405,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ywagc4rj", .model_name = "TS0201",
    .z2s_device_uid = 3410,
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

  {	.manufacturer_name = "_TZ3000_zz6u0auj", .model_name = "TS0203",
   .z2s_device_uid = 3805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000", .model_name = "SNZB-03",
   .z2s_device_uid = 3810,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_996rpfy6", .model_name = "TS0203",
   .z2s_device_uid = 3815,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_yihvt3gp", .model_name = "TS0203",
    .z2s_device_uid = 3820,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LINCUKOO", .model_name = "TS0203",
    .z2s_device_uid = 3825,
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

  {	.manufacturer_name = "_TZ3040_fuhjqokx", .model_name = "TS0202",
   .z2s_device_uid = 3930,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_1_B_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_n2egfsli", .model_name = "TS0203",
   .z2s_device_uid = 3935,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_1_B_SENSOR,
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
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_k4ej3ww2", .model_name = "TS0207",
    .z2s_device_uid = 4600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_qhozxs2b", .model_name = "TS0207",
    .z2s_device_uid = 4605,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_rr1cqjwu", .model_name = "TS0207",
    .z2s_device_uid = 4610,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ww9i3e0y", .model_name = "TS0207",
    .z2s_device_uid = 4615,
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
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_1bwpjvlz", .model_name = "TS0203",
   .z2s_device_uid = 5900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-04",
   .z2s_device_uid = 6000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "CK-TLSR8656-SS5-01(7003)",
   .z2s_device_uid = 6005,
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

  {	.manufacturer_name = "eWeLink", .model_name = "CK-TLSR8656-SS5-01(7002)",
    .z2s_device_uid = 6205,
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

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sensor_magnet",
    .z2s_device_uid = 7305,
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

  {	.manufacturer_name = "ClimaxTechnology", .model_name = "IR-9ZBS-SL",
   .z2s_device_uid = 8805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HEIMAN", .model_name = "PIRSensor-EM",
   .z2s_device_uid = 8810,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Heiman", .model_name = "PIR_TPV12",
   .z2s_device_uid = 8815,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_d93wtje5", .model_name = "TS0203",
    .z2s_device_uid = 8900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_uw3dadam", .model_name = "TS0202",
    .z2s_device_uid = 8905,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_kyb656no", .model_name = "TS0207",
    .z2s_device_uid = 8910,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_sr0vaafi", .model_name = "TS0202",
    .z2s_device_uid = 8915,
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

  {	.manufacturer_name = "Develco Products A/S", .model_name = "FLSZB-110",
    .z2s_device_uid = 9350,
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

  {	.manufacturer_name = "Develco Products A/S", .model_name = "SPLZB-137",
    .z2s_device_uid = 9405,
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

  {	.manufacturer_name = "eWeLink", .model_name = "SNZB-05",
    .z2s_device_uid = 9605,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "DS01",
    .z2s_device_uid = 9610,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0207",
    .z2s_device_uid = 9615,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "SNZB-05P",
    .z2s_device_uid = 9620,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLink", .model_name = "CK-TLSR8656-SS5-01(7019)",
    .z2s_device_uid = 9625,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-204Z",
    .z2s_device_uid = 9700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sensor_wleak.aq1",
    .z2s_device_uid = 9705,
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

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-06P",
    .z2s_device_uid = 10005,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_prits6g4", .model_name = "TS0001",
    .z2s_device_uid = 10100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_iutkth2a", .model_name = "TS0001",
    .z2s_device_uid = 10105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_czjsxcwp", .model_name = "TS0001",
    .z2s_device_uid = 10110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_n6fqajob", .model_name = "TS0001",
    .z2s_device_uid = 10115,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0001",
    .z2s_device_uid = 10120,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS0001",
    .z2s_device_uid = 10200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-301Z",
    .z2s_device_uid = 10205,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_hktqahrq", .model_name = "TS0001",
    .z2s_device_uid = 10210,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_npzfdcof", .model_name = "TS0001",
    .z2s_device_uid = 10215,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_5ng23zjs", .model_name = "TS0001",
    .z2s_device_uid = 10220,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_rmjr4ufz", .model_name = "TS0001",
    .z2s_device_uid = 10225,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_v7gnj3ad", .model_name = "TS0001",
    .z2s_device_uid = 10230,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_3a9beq8a", .model_name = "TS0001",
    .z2s_device_uid = 10235,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ark8nv4y", .model_name = "TS0001",
    .z2s_device_uid = 10240,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mx3vgyea", .model_name = "TS0001",
    .z2s_device_uid = 10245,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_qsp2pwtf", .model_name = "TS0001",
    .z2s_device_uid = 10250,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_kycczpw8", .model_name = "TS0001",
    .z2s_device_uid = 10255,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_46t1rvdu", .model_name = "TS0001",
    .z2s_device_uid = 10260,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_bhcpnvud", .model_name = "TS0001",
    .z2s_device_uid = 10265,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_i9oy2rdq", .model_name = "TS0001",
    .z2s_device_uid = 10270,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS000F",
    .z2s_device_uid = 10300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_hktqahrq", .model_name = "TS000F",
    .z2s_device_uid = 10305,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_m9af2l6g", .model_name = "TS000F",
    .z2s_device_uid = 10310,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mx3vgyea", .model_name = "TS000F",
    .z2s_device_uid = 10315,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_dlhhrhs8", .model_name = "TS000F",
    .z2s_device_uid = 10320,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_fdxihpp7", .model_name = "TS000F",
    .z2s_device_uid = 10325,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_hdc8bbha", .model_name = "TS000F",
    .z2s_device_uid = 10330,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3218_hdc8bbha", .model_name = "TS000F",
    .z2s_device_uid = 10335,
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

  {	.manufacturer_name = "_TZ3210_n0wbkysi", .model_name = "TS0003",
    .z2s_device_uid = 10550,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_TUYA_RELAY },
                             { 2, Z2S_DEVICE_DESC_TUYA_RELAY },
                             { 3, Z2S_DEVICE_DESC_TUYA_RELAY }}},

  {	.manufacturer_name = "eWeLink", .model_name = "SWITCH-ZR03-1",
    .z2s_device_uid = 10600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "OSRAM", .model_name = "Plug 01",
    .z2s_device_uid = 10605,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 3, Z2S_DEVICE_DESC_RELAY_1 },
                             { 1, Z2S_DEVICE_DESC_NULL }}},

  {	.manufacturer_name = "SONOFF", .model_name = "S26R2ZB",
    .z2s_device_uid = 10700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_cmcjbqup", .model_name = "TS0001",
    .z2s_device_uid = 10800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_o4cjetlm", .model_name = "TS0001",
    .z2s_device_uid = 10805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_iedbgyxt", .model_name = "TS0001",
    .z2s_device_uid = 10810,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_h3noz0a5", .model_name = "TS0001",
    .z2s_device_uid = 10815,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TYZB01_4tlksk8a", .model_name = "TS0001",
    .z2s_device_uid = 10820,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_5ucujjts", .model_name = "TS0001",
    .z2s_device_uid = 10825,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_h8ngtlxy", .model_name = "TS0001",
    .z2s_device_uid = 10830,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_w0ypwa1f", .model_name = "TS0001",
    .z2s_device_uid = 10835,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_wpueorev", .model_name = "TS0001",
    .z2s_device_uid = 10840,
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

  {	.manufacturer_name = "_TYZB01_rifa0wlb", .model_name = "TS0011",
    .z2s_device_uid = 11005,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_rk2yzt0u", .model_name = "TS011F",
    .z2s_device_uid = 11010,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_o4cjetlm", .model_name = "TS011F",
    .z2s_device_uid = 11015,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_DC,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_8n7lqbm0", .model_name = "TS0001",
    .z2s_device_uid = 11105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_g8n1n7lg", .model_name = "TS0001",
    .z2s_device_uid = 11110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.switch.b1lc04",
    .z2s_device_uid = 11200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SWITCH,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.switch.l2aeu1",
    .z2s_device_uid = 11205,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_DOUBLE_SWITCH,
	  .z2s_device_endpoints_count = 2,
     .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_LUMI_DOUBLE_SWITCH },
                              { 2, Z2S_DEVICE_DESC_LUMI_DOUBLE_SWITCH }}},

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

  {	.manufacturer_name = "_TZ3000_xzhnra8x", .model_name = "TS011F",
    .z2s_device_uid = 11705,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A,
	  .z2s_device_endpoints_count = 1}, //A*1000

  {	.manufacturer_name = "_TZ3000_okaz9tjs", .model_name = "TS011F",
    .z2s_device_uid = 11800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_fgwhjm9j", .model_name = "TS011F",
    .z2s_device_uid = 11805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_w0qqde0g", .model_name = "TS011F",
    .z2s_device_uid = 11810,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_waho4jtj", .model_name = "TS011F",
    .z2s_device_uid = 11815,
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

  {	.manufacturer_name = "_TZ3000_wzmuk9ai", .model_name = "TS011F",
    .z2s_device_uid = 12105,
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
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_2uollq9d", .model_name = "TS011F",
    .z2s_device_uid = 12600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_2uollq9d", .model_name = "TS011F",
    .z2s_device_uid = 12700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_drzlxjne", .model_name = "TS011F",
    .z2s_device_uid = 12705,
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

  {	.manufacturer_name = "_TZ3000_lepzuhto", .model_name = "TS011F",
    .z2s_device_uid = 13110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_cayepv1a", .model_name = "TS011F",
    .z2s_device_uid = 13115,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_qystbcjg", .model_name = "TS011F",
    .z2s_device_uid = 13120,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_303avxxt", .model_name = "TS011F",
    .z2s_device_uid = 13125,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_ibefeicf", .model_name = "TS011F",
    .z2s_device_uid = 13130,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_zjchz7pd", .model_name = "TS011F",
    .z2s_device_uid = 13135,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_zv6x8bt2", .model_name = "TS011F",
    .z2s_device_uid = 13140,
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

  {	.manufacturer_name = "HOBEIAN", .model_name = "TS0044",
    .z2s_device_uid = 13535,
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

  {	.manufacturer_name = "_TZ3000_5tqxpine", .model_name = "TS0044",
    .z2s_device_uid = 13605,
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

  {	.manufacturer_name = "_TZ3000_iszegwpd", .model_name = "TS0046",
    .z2s_device_uid = 13750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 6,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 5, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 6, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

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

  {	.manufacturer_name = "_TZ3000_tzvbimpq", .model_name = "TS0042",
    .z2s_device_uid = 13950,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

{	.manufacturer_name = "_TZ3000_dfgbtub0", .model_name = "TS0042",
    .z2s_device_uid = 13955,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},


  {	.manufacturer_name = "_TZ3000_yj6k7vfo", .model_name = "TS0041",
    .z2s_device_uid = 14000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_4fjiwweb", .model_name = "TS004F",
    .z2s_device_uid = 14100,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_gwkzibhs", .model_name = "TS004F",
    .z2s_device_uid = 14105,
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

{	.manufacturer_name = "_TZ3000_b4awzgct", .model_name = "TS0041",
    .z2s_device_uid = 14805,
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

  {	.manufacturer_name = "_TYZB01_bngwdjsr", .model_name = "TS1001",
    .z2s_device_uid = 15050,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB20,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TYZB01_hww2py6b", .model_name = "TS1001",
    .z2s_device_uid = 15055,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB21,
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

  {	.manufacturer_name = "eWeLink", .model_name = "WB01",
    .z2s_device_uid = 15210,
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

  {	.manufacturer_name = "LUMI", .model_name = "lumi.sensor_switch",
    .z2s_device_uid = 15450,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_2F,
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

{	.manufacturer_name = "IKEA of Sweden", .model_name = "RODRET wireless dimmer",
    .z2s_device_uid = 15905,
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

  {	.manufacturer_name = "SONOFF", .model_name = "ZBMINI-L",
    .z2s_device_uid = 16410,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "BASICZBR3",
    .z2s_device_uid = 16415,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "ZBMINIR2",
    .z2s_device_uid = 16500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "MINI-ZB2GS",
    .z2s_device_uid = 16550,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_RELAY_1 },
                             { 2, Z2S_DEVICE_DESC_RELAY_1 }}},

  {	.manufacturer_name = "SONOFF", .model_name = "01MINIZB",
    .z2s_device_uid = 16600,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_wamqdr3f", .model_name = "TS011F",
    .z2s_device_uid = 16650,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 11, Z2S_DEVICE_DESC_RELAY_1 },
                             { 1, Z2S_DEVICE_DESC_NULL }}},

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

  {	.manufacturer_name = "_TZE200_4utwozi2", .model_name = "TS0601",
    .z2s_device_uid = 17110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_hvaxb2tc", .model_name = "TS0601",
    .z2s_device_uid = 17115,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_c6wv4xyo", .model_name = "TS0601",
    .z2s_device_uid = 17120,
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

  {	.manufacturer_name = "_TZE204_vjpaih9f", .model_name = "TS0601",
    .z2s_device_uid = 17750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV16,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_ymldrmzx", .model_name = "TS0601",
   .z2s_device_uid = 17800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV603,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_cpmgn2cf", .model_name = "TS0601",
    .z2s_device_uid = 17900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_MOES,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_cwnjrr72", .model_name = "TS0601",
    .z2s_device_uid = 17905,
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

  { .manufacturer_name = "_TZE200_ps5v5jor", .model_name = "TS0601",
    .z2s_device_uid = 18350,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SITERWELL,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_ltwbm23f", .model_name = "TS0601",
    .z2s_device_uid = 18400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_qyr2m29i", .model_name = "TS0601",
    .z2s_device_uid = 18405,
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

  {	.manufacturer_name = "_TZ3000_riwp3k79", .model_name = "TS0505A",
    .z2s_device_uid = 18705,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_8uaoilu9", .model_name = "TS0502A",
    .z2s_device_uid = 18750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZB210_ue01a0s2", .model_name = "TS0502B",
    .z2s_device_uid = 18755,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E27 CWS globe 806lm",
    .z2s_device_uid = 18800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 CWS 345lm",
    .z2s_device_uid = 18805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "ORMANAS LED Strip",
    .z2s_device_uid = 18810,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E14 CWS globe 806lm",
    .z2s_device_uid = 18900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Adeo", .model_name = "ZBEK-1",
    .z2s_device_uid = 18950,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_RGBW_BULB,
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

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "SILVERGLANS IP44 LED driver",
    .z2s_device_uid = 19605,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E14 WS globe 470lm",
    .z2s_device_uid = 19645,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E27 WS globe 1055lm",
    .z2s_device_uid = 19650,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI_bulb_GU10_WS_345lm",
    .z2s_device_uid = 19655,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRIbulbGU10WS345lm",
    .z2s_device_uid = 19660,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 WS 345lm",
    .z2s_device_uid = 19665,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRIbulbGU10WS380lm",
    .z2s_device_uid = 19670,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 WS 380lm",
    .z2s_device_uid = 19675,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRIbulbE14WSglobeopal470lm",
    .z2s_device_uid = 19680,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRIbulbE12WSglobeopal470lm",
    .z2s_device_uid = 19685,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E17 WS globe 440lm",
    .z2s_device_uid = 19690,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRIbulbE17WSglobeopal470lm",
    .z2s_device_uid = 19695,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WS_BULB,
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

  {	.manufacturer_name = "_TZ3210_i680rtja", .model_name = "TS0501B",
    .z2s_device_uid = 19805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_mgusv51k", .model_name = "TS0052",
    .z2s_device_uid = 19810,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "eWeLight", .model_name = "ZB-DL01",
    .z2s_device_uid = 19850,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LED_DIMMER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Trust International B.V.", .model_name = "ZLL-DimmableLigh",
    .z2s_device_uid = 19855,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LED_DIMMER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_bfwvfyx1", .model_name = "TS0505B",
    .z2s_device_uid = 19900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Zbeacon", .model_name = "TS0505B",
    .z2s_device_uid = 19905,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZB210_jogusbq1", .model_name = "TS0505B",
    .z2s_device_uid = 19910,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZB210_rs0ufzwg", .model_name = "TS0505B",
    .z2s_device_uid = 19915,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_mja6r5ix", .model_name = "TS0505B",
    .z2s_device_uid = 19920,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_jd3z4yig", .model_name = "TS0505B",
    .z2s_device_uid = 19925,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3210_r5afgmkl", .model_name = "TS0505B",
    .z2s_device_uid = 19930,
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

  {	.manufacturer_name = "Aqara", .model_name = "lumi.light.acn132",
    .z2s_device_uid = 20450,
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

  {	.manufacturer_name = "_TZ3000_mtnpt6ws", .model_name = "TS0002",
    .z2s_device_uid = 21010,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_m8f3z8ju", .model_name = "TS000F",
    .z2s_device_uid = 21015,
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

  {	.manufacturer_name = "_TYZB01_ugy1bbbg", .model_name = "TS0002",
    .z2s_device_uid = 21105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_jyua7aqt", .model_name = "TS0002",
    .z2s_device_uid = 21110,
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


  {	.manufacturer_name = "_TZ3000_vzopcetz", .model_name = "TS011F",
    .z2s_device_uid = 22005,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_qewo8dlz", .model_name = "TS0013",
    .z2s_device_uid = 22010,
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

  {	.manufacturer_name = "_TZ3000_hbic3ka3", .model_name = "TS0003",
    .z2s_device_uid = 22205,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_ju82pu2b", .model_name = "TS0003",
    .z2s_device_uid = 22210,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                             { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                             { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  {	.manufacturer_name = "_TZ3000_eatarlvc", .model_name = "TS0003",
    .z2s_device_uid = 22215,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH,
	  .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
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

  {	.manufacturer_name = "_TZ3000_h1ipgkwn", .model_name = "",
    .z2s_device_uid = 22501,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 2, Z2S_DEVICE_DESC_TUYA_RELAY }}},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-305Z",
    .z2s_device_uid = 22505,
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

  {	.manufacturer_name = "_TZ3000_knoj8lpk", .model_name = "TS0004",
    .z2s_device_uid = 22605,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 2, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 3, Z2S_DEVICE_DESC_TUYA_RELAY },
                              { 4, Z2S_DEVICE_DESC_TUYA_RELAY }}},

  {	.manufacturer_name = "_TZ3000_3n2minvf", .model_name = "TS0004",
    .z2s_device_uid = 22610,
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

  {	.manufacturer_name = "_TZE204_ogkdpgy2", .model_name = "TS0601",
    .z2s_device_uid = 23350,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO2_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_chbyv06x", .model_name = "TS0601",
    .z2s_device_uid = 23400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_uo8qcagc", .model_name = "TS0601",
    .z2s_device_uid = 23405,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_mja3fuja", .model_name = "TS0601",
    .z2s_device_uid = 23500,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_yvx5lh6k", .model_name = "TS0601",
    .z2s_device_uid = 23505,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_c2fmom5z", .model_name = "TS0601",
    .z2s_device_uid = 23510,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_c2fmom5z", .model_name = "TS0601",
    .z2s_device_uid = 23515,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_ryfmq5rl", .model_name = "TS0601",
    .z2s_device_uid = 23520,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_yvx5lh6k", .model_name = "TS0601",
    .z2s_device_uid = 23525,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_8ygsuhe1", .model_name = "TS0601",
    .z2s_device_uid = 23530,
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

  {	.manufacturer_name = "_TZE284_debczeci", .model_name = "TS0601",
    .z2s_device_uid = 23750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE284_1lvln0x6", .model_name = "TS0601",
    .z2s_device_uid = 23755,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_ex3rcdha", .model_name = "TS0601",
    .z2s_device_uid = 23800,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_rhgsbacq", .model_name = "TS0601",
    .z2s_device_uid = 23900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_uli8wasj", .model_name = "TS0601",
    .z2s_device_uid = 23905,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_grgol3xp", .model_name = "TS0601",
    .z2s_device_uid = 23910,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE200_y8jijhba", .model_name = "TS0601",
    .z2s_device_uid = 23915,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "HOBEIAN", .model_name = "ZG-204ZV",
    .z2s_device_uid = 24000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZE204_mtoaryre", .model_name = "TS0601",
    .z2s_device_uid = 24005,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY,
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

  /*{	.manufacturer_name = "TELINK", .model_name = "TLSR82xx",
    .z2s_device_uid = 24305,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SMART_BUTTON_2F,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_SMART_BUTTON_2F },
                              { 2, Z2S_DEVICE_DESC_SMART_BUTTON_2F }}},*/

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

  {	.manufacturer_name = "_TZ3210_1ptgp0az", .model_name = "TS130F",
    .z2s_device_uid = 25105,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3218_zhg356le", .model_name = "TS130F",
    .z2s_device_uid = 25110,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_eg7awg6a", .model_name = "TS130F",
    .z2s_device_uid = 25115,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_fccpjz5z", .model_name = "TS130F",
    .z2s_device_uid = 25200,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_j1xl73iw", .model_name = "TS130F",
    .z2s_device_uid = 25250,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_DOUBLE,
    .z2s_device_endpoints_count = 2,
	  .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE},
                             { 2, Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE }}},

{	.manufacturer_name = "_TZ3000_kmsbwdol", .model_name = "TS130F",
    .z2s_device_uid = 25255,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_DOUBLE,
    .z2s_device_endpoints_count = 2,
	  .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE},
                             { 2, Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE }}},

  {	.manufacturer_name = "_TZ3000_femsaaua", .model_name = "TS130F",
    .z2s_device_uid = 25300,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = {{ 1, Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE},
                             { 2, Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE }}},

  {	.manufacturer_name = "_TZ3210_dwytrmda", .model_name = "TS130F",
    .z2s_device_uid = 25305,
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

  {	.manufacturer_name = "ADEO", .model_name = "ZBEK-26",
    .z2s_device_uid = 25750,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_ENKI_REMOTE_CONTROL,
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
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_ZEMISMART_SHADES_DRIVE_MOTOR,
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

  {	.manufacturer_name = "_TZ3000_piuensvr", .model_name = "CK-BL702-ROUTER-01(7018)",
    .z2s_device_uid = 26405,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_REPEATER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_nlsszmzl", .model_name = "TS0207",
    .z2s_device_uid = 26410,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_REPEATER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ZZRR", .model_name = "MY_RANGE_EXTENDER",
    .z2s_device_uid = 26415,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_REPEATER,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "ZIBI", .model_name = "MY_RANGE_EXTENDER",
    .z2s_device_uid = 26420,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_REPEATER,
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

  {	.manufacturer_name = "_TZE200_qxyh4r7g", .model_name = "TS0601",
    .z2s_device_uid = 26910,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR ,
	  .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "LUMI", .model_name = "lumi.airmonitor.acn01",
    .z2s_device_uid = 27000,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR,
	  .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_utwgoauk", .model_name = "TS0201",
    .z2s_device_uid = 27001,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.sensor_ht",
    .z2s_device_uid = 27102,  
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR,
    .z2s_device_endpoints_count = 1 },

  { .manufacturer_name = "_TZE200_mua6ucdj", .model_name = "TS0601",
    .z2s_device_uid = 28000,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LCD_3_RELAYS,
    .z2s_device_endpoints_count = 1},

  /*{ .manufacturer_name = "_TZE200_oahqgdig", .model_name = "TS0601",
    .z2s_device_uid = 28100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES,
    .z2s_device_endpoints_count = 6,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 2, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 3, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 4, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 5, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 6, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES }}},*/

  { .manufacturer_name = "_TZE200_oahqgdig", .model_name = "TS0601",
    .z2s_device_uid = 28100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES,
    .z2s_device_endpoints_count = 1},
    /*.z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 2, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 3, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 4, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 5, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES },
                              { 6, Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES }}},*/

  { .manufacturer_name = "_TZ3210_j4pdtz9v", .model_name = "TS0001",
    .z2s_device_uid = 29000,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_FINGERBOT_PLUS,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_a04acm9s", .model_name = "TS0001",
    .z2s_device_uid = 29005,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_FINGERBOT_PLUS,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_aaeaifez", .model_name = "TS0601",
    .z2s_device_uid = 29100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_lzriup1j", .model_name = "TS0601",
    .z2s_device_uid = 29105,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_gops3slb", .model_name = "TS0601",
    .z2s_device_uid = 29110,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_gops3slb", .model_name = "TS0601",
    .z2s_device_uid = 29115,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_cvub6xbb", .model_name = "TS0601",
    .z2s_device_uid = 29120,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_xnbkhhdr", .model_name = "TS0601",
    .z2s_device_uid = 29125,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_xnbkhhdr", .model_name = "",
    .z2s_device_uid = 29130,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100,
    .z2s_device_endpoints_count = 1},

{ .manufacturer_name = "_TZE200_viy9ihs7", .model_name = "",
    .z2s_device_uid = 29135,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100,
    .z2s_device_endpoints_count = 1},

{ .manufacturer_name = "_TZE200_viy9ihs7", .model_name = "TS0601",
    .z2s_device_uid = 29140,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_ye5jkfsb", .model_name = "TS0601",
    .z2s_device_uid = 29200,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_MOES_BHT002,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_1oft6qso", .model_name = "TS0601",
    .z2s_device_uid = 29300,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_adlblwab", .model_name = "TS0601",
    .z2s_device_uid = 29350,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER,
    .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "_TZ3000_j0ktmul1", .model_name = "TS011F",
    .z2s_device_uid = 29400,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_5_RELAYS_CONTROLLER,
	  .z2s_device_endpoints_count = 5,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 4, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 5, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZE204_5slehgeo", .model_name = "TS0601",
    .z2s_device_uid = 29500,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_COVER,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_waa352qv", .model_name = "TS0601",
    .z2s_device_uid = 29550,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_CURRYSMARTER_COVER,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "BOSCH", .model_name = "RBSH-SP-ZB-EU",
    .z2s_device_uid = 29600,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_BOSCH_RELAY_ELECTRICITY_METER,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "BOSCH", .model_name = "RBSH-TRV0-ZB-EU",
    .z2s_device_uid = 29700,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_BOSCH_BTHRA,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "VINDSTYRKA",
    .z2s_device_uid = 29800,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_AIR_QUALITY_SENSOR,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Eurotronic", .model_name = "SPZB0001",
    .z2s_device_uid = 29900,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_EUROTRONIC_SPZB0001,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_kyhbrfyl", .model_name = "TS0601",
    .z2s_device_uid = 30000,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_NEO,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "AwoX", .model_name = "TLSR82xx",
    .z2s_device_uid = 30100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_DIMMER_CT_BULB,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Adeo", .model_name = "ZBEK-12",
    .z2s_device_uid = 30105,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_DIMMER_CT_BULB,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.airrtc.agl001",
    .z2s_device_uid = 30200,
    .z2s_device_desc_id = Z2s_DEVICE_DESC_LUMI_TRV,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_hodyryli", .model_name = "TS0601",
    .z2s_device_uid = 30300,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TH_SENSOR_TEMP_PROBE,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_8se38w3c", .model_name = "TS0601",
    .z2s_device_uid = 30305,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TH_SENSOR_TEMP_PROBE,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.curtain.agl001",
    .z2s_device_uid = 30400,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.curtain.acn002",
    .z2s_device_uid = 30450,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_CURTAIN_DRIVER_1,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_y5rtzkmc", .model_name = "TS0504B",
    .z2s_device_uid = 30500,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_b44hll99", .model_name = "TS0504B",
    .z2s_device_uid = 30505,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_NO_CT,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_dapwryy7", .model_name = "TS0601",
    .z2s_device_uid = 30600,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205Z,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_iadro9bf", .model_name = "TS0601",
    .z2s_device_uid = 30700,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM100S2,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.sensor_motion",
    .z2s_device_uid = 30800,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR_2,
    .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Xiaomi", .model_name = "lumi.sensor_motion",
    .z2s_device_uid = 30805,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR_2,
	  .z2s_device_endpoints_count = 1},


  { .manufacturer_name = "_TZ32101000000_5oy7cysk", .model_name = "TS0210",
    .z2s_device_uid = 30900,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR_2,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_vvmbj46n", .model_name = "TS0601",
    .z2s_device_uid = 31000,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_1,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_lpedvtvr", .model_name = "TS0601",
    .z2s_device_uid = 31100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_MOES_ZHTSR,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Aqara", .model_name = "lumi.plug.aeu001",
    .z2s_device_uid = 31200,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_SMART_WALL_OUTLET,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_8fffc3kb", .model_name = "TS0601",
    .z2s_device_uid = 31300,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_ehhrv2e3", .model_name = "TS0601",
    .z2s_device_uid = 31400,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_2,
    .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZE284_5m4nchbm", .model_name = "TS0601",
    .z2s_device_uid = 31500,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_9ern5sfh", .model_name = "TS0601",
    .z2s_device_uid = 31600,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_3,
    .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "SONOFF", .model_name = "SNZB-01M",
    .z2s_device_uid = 31700,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F,
	  .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F },
                              { 2, Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F },
                              { 3, Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F },
                              { 4, Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_4X4F }}},
    
  { .manufacturer_name = "_TZE200_wqashyqo", .model_name = "TS0601",
    .z2s_device_uid = 31800,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "HOBEIAN", .model_name = "ZG-303Z",
    .z2s_device_uid = 31900,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F_2,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_gkfbdvyx", .model_name = "TS0601",
    .z2s_device_uid = 32000,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM10024GV3,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_npj9bug3", .model_name = "TS0601",
    .z2s_device_uid = 32100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F_2,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_pagajpog", .model_name = "TS110E",
    .z2s_device_uid = 32200,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0,
    .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints ={{ 1, Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0 },
                            { 2, Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0 }}},

  { .manufacturer_name = "_TZ3210_4ubylghk", .model_name = "TS110E",
    .z2s_device_uid = 32300,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0,
    .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints ={{ 1, Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0 },
                            { 2, Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0 }}},
  
  { .manufacturer_name = "ZG-204ZE", .model_name = "CK-BL702-MWS-01(7016)",
    .z2s_device_uid = 32400,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG204ZE,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_wc2w9t1s", .model_name = "TS0601",
    .z2s_device_uid = 32500,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_BOTR9V,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_ekjc2rzh", .model_name = "TS0225",
    .z2s_device_uid = 32600,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_MWPS3Z,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_gyzlwu5q", .model_name = "TS0601",
    .z2s_device_uid = 32700,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_228WZH,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "HOBEIAN", .model_name = "ZG-302Z1",
    .z2s_device_uid = 32800,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DP_RELAY,
    .z2s_device_endpoints_count = 1},

  {	.manufacturer_name = "Shelly", .model_name = "2PM",
    .z2s_device_uid = 32900,
	  .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER,
	  .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints ={{ 1, Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER },
                            { 2, Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER }}},

  { .manufacturer_name = "_TZE204_jpyrsdp3", .model_name = "TS0601",
    .z2s_device_uid = 33000,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_GAS_DETECTOR,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Arteco", .model_name = "ZS-304Z",
    .z2s_device_uid = 33100,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_4F,
    .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_f5efvtbv", .model_name = "TS0601",
    .z2s_device_uid = 33200,
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_4_RELAYS_DP_CONTROLLER,
    .z2s_device_endpoints_count = 1},


  
//DEVICES_END
};
#endif
