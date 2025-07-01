#ifndef Z2S_DEVICES_DATABASE_H_
#define Z2S_DEVICES_DATABASE_H_

// Z2S_DEVICE_DESC_ID - used for selection of clusters to bind and matching Supla channels#define

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR             0x1000
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1           0x1001
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL        0x1002

#define Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR        0x1010

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10     0x1020

#define Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR         0x1050

#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR   0x1100
#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1 0x1101


#define Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR         0x1200
#define Z2S_DEVICE_DESC_ILLUZONE_SENSOR                 0x1205

#define Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR                0x1300
#define Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2              0x1305

#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR                 0x2000
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T           0x2001
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B           0x2002
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B             0x2003

#define Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR            0x2004
#define Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1          0x2005
#define Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2          0x2006


#define Z2S_DEVICE_DESC_IKEA_VALLHORN_1                 0x2007
#define Z2S_DEVICE_DESC_IKEA_VALLHORN_2                 0x2008
#define Z2S_DEVICE_DESC_IKEA_VALLHORN_3                 0x2009

#define Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR              0x2010
#define Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR              0x2011

#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B    0x2020

#define Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR           0x2030

#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR             0x2200
#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1           0x2201

#define Z2S_DEVICE_DESC_TUYA_CO_DETECTOR                0x2210

#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR            0x2300
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5          0x2305
#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1       0x2310

#define Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR         0x2500
#define Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR   0x2501

#define Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR               0x2600

#define Z2S_DEVICE_DESC_RGBW_START                      0x3000

#define Z2S_DEVICE_DESC_LIGHT_SOURCE                    0x3000 
#define Z2S_DEVICE_DESC_LIGHT_BULB                      0x3010

#define Z2S_DEVICE_DESC_RGB_BULB                        0x3050
#define Z2S_DEVICE_DESC_TUYA_RGB_BULB                   0x3060

#define Z2S_DEVICE_DESC_RGBW_BULB                       0x3100
#define Z2S_DEVICE_DESC_RGBW_BULB_XY                    0x3101
#define Z2S_DEVICE_DESC_RGBW_BULB_HS                    0x3102

#define Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A          0x3110
#define Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B          0x3111
#define Z2S_DEVICE_DESC_IKEA_RGBW_BULB                  0x3120

#define Z2S_DEVICE_DESC_DIMMER_BULB                     0x3200
#define Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0           0x3210
#define Z2S_DEVICE_DESC_IKEA_WW_BULB                    0x3220

#define Z2S_DEVICE_DESC_RGBW_END                        0x3299

#define Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH              0x3300
#define Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH       0x3305

#define Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY      0x3400

#define Z2S_DEVICE_DESC_RELAY                           0x4000
#define Z2S_DEVICE_DESC_RELAY_1                         0x4001

#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER         0x4100
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1       0x4101
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2       0x4102

#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER    0x4500
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1  0x4501
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2  0x4502

#define Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER  0x4550
#define Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER   0x4551

#define Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1              0x4600
#define Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2              0x4601

#define Z2S_DEVICE_DESC_TUYA_1GANG_SWITCH               0x4604
#define Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH               0x4605
#define Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH               0x4606
#define Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH               0x4607

#define Z2S_DEVICE_DESC_ON_OFF_VALVE_DC                 0x4700

#define Z2S_DEVICE_DESC_WINDOW_COVERING_SINGLE          0x4800
#define Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE     0x4810
#define Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR         0x4820

#define Z2S_DEVICE_DESC_SONOFF_SMART_VALVE              0x4900

#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F            0x5000
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F            0x5001
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F            0x5002

#define Z2S_DEVICE_DESC_TUYA_SWITCH_4X3                 0x5100

#define Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3            0x5105

#define Z2S_DEVICE_DESC_IKEA_SMART_BUTTON               0x5200
#define Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F            0x5201

#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1            0x5202
#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1          0x5203
#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2          0x5204
#define Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3          0x5205

#define Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON              0x5210
#define Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1            0x5211
#define Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2            0x5212

#define Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F          0x5300

#define Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F    0x5400
#define Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F            0x5410

#define Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH       0x5500

#define Z2S_DEVICE_DESC_HVAC_START                      0x6000

#define Z2S_DEVICE_DESC_TUYA_HVAC                       0x6000
#define Z2S_DEVICE_DESC_TUYA_HVAC_23457                 0x6001
#define Z2S_DEVICE_DESC_TUYA_HVAC_6567C                 0x6002
#define Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY                0x6003

#define Z2S_DEVICE_DESC_SONOFF_TRVZB                    0x6005

#define Z2S_DEVICE_DESC_TS0601_TRV_SASWELL              0x6010
#define Z2S_DEVICE_DESC_TS0601_TRV_ME167                0x6011
#define Z2S_DEVICE_DESC_TS0601_TRV_BECA                 0x6012
#define Z2S_DEVICE_DESC_TS0601_TRV_MOES                 0x6013
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV601               0x6014
#define Z2S_DEVICE_DESC_TS0601_TRV_TRV603               0x6015

#define Z2S_DEVICE_DESC_HVAC_END                        0x6499

#define Z2S_DEVICE_DESC_IR_REMOTE_CONTROL               0x6500
#define Z2S_DEVICE_DESC_TUYA_SIREN_ALARM                0x6600
#define Z2S_DEVICE_DESC_MOES_ALARM                      0x6610

#define Z2S_DEVICE_DESC_ON_OFF                          0x8000
#define Z2S_DEVICE_DESC_ON_OFF_1                        0x8001


#define MAX_BOUND_ENDPOINTS 8
#define MAX_BOUND_CLUSTERS 16


#define TUYA_ON_OFF_CUSTOM_CMD_BUTTON_PRESS_ID       0xFD
#define TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID      0xFC

#define NO_CUSTOM_CMD_SID                           -0x01

#define IAS_ZONE_ALARM_1_SID                        0x00
#define IAS_ZONE_ALARM_2_SID                        0x01
#define IAS_ZONE_TAMPER_SID                         0x02
#define IAS_ZONE_LOW_BATTERY_SID                    0x03

#define SONOFF_CUSTOM_CLUSTER_TAMPER_SID            0x10

#define TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID          0x00
#define TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID   0x01
#define TUYA_CUSTOM_CMD_BUTTON_HELD_SID             0x02
#define TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID     0x0A
#define TUYA_CUSTOM_CMD_BUTTON_ROTATE_LEFT_SID      0x0B

#define TUYA_CUSTOM_CMD_BUTTON_1_PRESSED_SID          0x00
#define TUYA_CUSTOM_CMD_BUTTON_1_DOUBLE_PRESSED_SID   0x01
#define TUYA_CUSTOM_CMD_BUTTON_1_HELD_SID             0x02
#define TUYA_CUSTOM_CMD_BUTTON_2_PRESSED_SID          0x03
#define TUYA_CUSTOM_CMD_BUTTON_2_DOUBLE_PRESSED_SID   0x04
#define TUYA_CUSTOM_CMD_BUTTON_2_HELD_SID             0x05

#define TUYA_DOUBLE_DIMMER_SWITCH_1_SID               0x01
#define TUYA_DOUBLE_DIMMER_SWITCH_2_SID               0x02

#define TUYA_PRESENCE_SENSOR_PRESENCE_SID             0x01
#define TUYA_PRESENCE_SENSOR_MOTION_STATE_SID         0x02
#define TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID          0x03
#define TUYA_PRESENCE_SENSOR_TEMPHUMIDITY_SID         0x04

#define TUYA_SMOKE_DETECTOR_1_SMOKE_SID               0x00
#define TUYA_SMOKE_DETECTOR_1_TAMPER_SID              0x01

#define TUYA_CO_DETECTOR_CO_SID                       0x00
#define TUYA_CO_DETECTOR_CO_CONC_SID                  0x01
#define TUYA_CO_DETECTOR_SELF_TEST_SID                0x02
#define TUYA_CO_DETECTOR_SILENCE_SID                  0x03

#define TUYA_RAIN_SENSOR_RAIN_SID                     0x01
#define TUYA_RAIN_SENSOR_ILLUMINANCE_SID              0x02
#define TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID   0x03
#define TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID    0x04
#define TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID           0x05

//#define IAS_ZONE_ALARM_1_SID                        0x00
//#define IAS_ZONE_ALARM_2_SID                        0x01
//#define IAS_ZONE_TAMPER_SID                         0x02
#define ADEO_SMART_PIRTH_SENSOR_TEMPHUMI_SID          0x10
#define ADEO_SMART_PIRTH_SENSOR_ILLUMINANCE_SID       0x11

#define TUYA_3PHASES_ELECTRICITY_METER_ENERGY_SID           0x00
#define TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_SID  0x01
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_SID            0x02
#define TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_SID     0x03
#define TUYA_3PHASES_ELECTRICITY_METER_SID                  0x04

#define IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID          0x00
#define IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID             0x01
#define IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID          0x02
#define IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID             0x03
#define IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID          0x04
#define IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID             0x05
#define IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID          0x06
#define IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID             0x07

#define IKEA_CUSTOM_CMD_SYMFONISK_PLAY_SID                 0x00
#define IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID            0x01
#define IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID          0x02
#define IKEA_CUSTOM_CMD_SYMFONISK_NEXT_TRACK_SID           0x03
#define IKEA_CUSTOM_CMD_SYMFONISK_PREV_TRACK_SID           0x04

#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID          0x05
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID   0x06
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID             0x07
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID    0x08
#define IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID   0x09

#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID         0x0A
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID  0x0B
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID            0x0C
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID   0x0D
#define IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID  0x0E

#define LUMI_MOTION_SENSOR_OCCUPANCY_SID    0x00
#define LUMI_MOTION_SENSOR_ILLUMINANCE_SID  0x01

#define SONOFF_SMART_VALVE_ON_OFF_SID       0x00
#define SONOFF_SMART_VALVE_FLOW_SID         0x01
#define SONOFF_SMART_VALVE_MODE_SID         0x02
#define SONOFF_SMART_VALVE_CYCLE_NUMBER_SID 0x03
#define SONOFF_SMART_VALVE_CYCLES_COUNT_SID 0x04
#define SONOFF_SMART_VALVE_TIME_SID         0x05
#define SONOFF_SMART_VALVE_PAUSE_SID        0x06
#define SONOFF_SMART_VALVE_VOLUME_SID       0x07

#define SONOFF_PIR_SENSOR_OCCUPANCY_SID    0x00
#define SONOFF_PIR_SENSOR_ILLUMINANCE_SID  0x01

#define DIMMER_FUNC_BRIGHTNESS_SID          0x00
#define DIMMER_FUNC_COLOR_TEMPERATURE_SID   0x01

#define IAS_WD_SILENT_ALARM_SID             0x00
#define IAS_WD_LOUD_ALARM_SID               0x01  

#define IAS_ACE_ARM_CMD_SID                 0x00
#define IAS_ACE_PANIC_CMD_SID               0x01
#define IAS_ACE_2_CMD_SID                   0x02 
#define IAS_ACE_3_CMD_SID                   0x03

#define ADEO_CUSTOM_CMD_BUTTON_PRESSED_SID        0x00
#define ADEO_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID 0x01
#define ADEO_CUSTOM_CMD_BUTTON_HELD_SID           0x02

#define MOES_ALARM_SWITCH_SID     0x00
#define MOES_ALARM_MELODY_SID     0x01
#define MOES_ALARM_VOLUME_SID     0x02
#define MOES_ALARM_DURATION_SID   0x03
#define MOES_ALARM_DISPLAY_SID    0x04

#define TUYA_VIBRATION_SENSOR_VIBRATION_SID 0x10
#define TUYA_VIBRATION_SENSOR_CONTACT_SID   0x11


//#define DIMMER_FUNC_

[[maybe_unused]]
static char IKEA_STYRBAR_BUTTONS[][16] PROGMEM = {"ON PRESSED", "ON HELD", "OFF PRESSED", "OFF HELD", "LEFT PRESSED", "LEFT HELD", "RIGHT PRESSED", "RIGHT HELD"};

[[maybe_unused]]
static char IKEA_SYMFONISK_BUTTONS[][20] PROGMEM = {"PLAY", "VOLUME UP", "VOLUME DOWN", "NEXT TRACK ", "PREV TRACK", 
                                                    "DOT PRESSED", "DOT SHORT RELEASE", "DOT HELD", "DOT LONG RELEASED", "DOT DOUBLE PRESSED",
                                                    "DOTS PRESSED", "DOTS SHORT RELEASE", "DOTS HELD", "DOTS LONG RELEASED", "DOTS DOUBLE PRESSED"};


typedef struct z2s_device_desc_s {
  uint32_t z2s_device_desc_id;
  uint8_t z2s_device_clusters_count;
  uint16_t z2s_device_clusters[MAX_BOUND_CLUSTERS];
} z2s_device_desc_t;

typedef struct z2s_device_endpoint_s {
  uint8_t endpoint_id;
  uint32_t z2s_device_desc_id;
} z2s_device_endpoint_t;

typedef struct z2s_device_entity_s {
  char manufacturer_name[33];
  char model_name[33];
  uint32_t z2s_device_desc_id;
  uint8_t z2s_device_endpoints_count;
  z2s_device_endpoint_t z2s_device_endpoints[MAX_BOUND_ENDPOINTS];
} z2s_device_entity_t;


static z2s_device_desc_t Z2S_DEVICES_DESC[] PROGMEM [[maybe_unused]] = {
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},
      
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL, .z2s_device_clusters_count = 4, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR, .z2s_device_clusters_count = 4, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT }},
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ILLUZONE_SENSOR, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR, .z2s_device_clusters_count = 5, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},


  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
      ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_2, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_3, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},


  { .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING,
      ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY, 
      ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_METERING,
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_METERING,
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_METERING,
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_METERING,
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_METERING,
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_METERING,
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

   { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},
 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL}},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    {0xFC80,
     0xFC7F}},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    {0xFC80,
     0xFC7F}}, 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    {0xFC80}},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    {0xFC80}},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ACE }},
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ADEO_CUSTOM_CLUSTER }},
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_VALVE_DC, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_HVAC, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
      TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_TRVZB, .z2s_device_clusters_count = 4, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT,
      SONOFF_CUSTOM_CLUSTER }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SIREN_ALARM, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_BECA, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_MOES, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV603, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_HVAC_23457, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
      TUYA_PRIVATE_CLUSTER_EF00 }}, /*2, 'system_mode', tuya.valueConverterBasic.lookup({auto: tuya.enum(0), heat: tuya.enum(1), off: tuya.enum(2)})],
                [3, 'running_state', tuya.valueConverterBasic.lookup({heat: tuya.enum(0), idle: tuya.enum(1)})],
                [4, 'current_heating_setpoint', tuya.valueConverter.divideBy10],
                [5, 'local_temperature', tuya.valueConverter.divideBy10],
                [7, 'child_lock', tuya.valueConverter.lockUnlock],*/

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_HVAC_6567C, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { //ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      //ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
      //TUYA_PRIVATE_CLUSTER_EF00 
      ESP_ZB_ZCL_CLUSTER_ID_BASIC }}, /*
      0x65 on/off
      0x66 current temperature 
      0x67 setpoint
      0x6C auto/manual
      saswellHeating: 3,
    saswellWindowDetection: 8,
    saswellFrostDetection: 10,
    saswellTempCalibration: 27,
    saswellChildLock: 40,
    saswellState: 101,
    saswellLocalTemp: 102,
    saswellHeatingSetpoint: 103,
    saswellValvePos: 104,
    saswellBatteryLow: 105,
    saswellAwayMode: 106,
    saswellScheduleMode: 107,
    saswellScheduleEnable: 108,
    saswellScheduleSet: 109,
    saswellSetpointHistoryDay: 110,
    saswellTimeSync: 111,
    saswellSetpointHistoryWeek: 112,
    saswellSetpointHistoryMonth: 113,
    saswellSetpointHistoryYear: 114,
    saswellLocalHistoryDay: 115,
    saswellLocalHistoryWeek: 116,
    saswellLocalHistoryMonth: 117,
    saswellLocalHistoryYear: 118,
    saswellMotorHistoryDay: 119,
    saswellMotorHistoryWeek: 120,
    saswellMotorHistoryMonth: 121,
    saswellMotorHistoryYear: 122,
    saswellScheduleSunday: 123,
    saswellScheduleMonday: 124,
    saswellScheduleTuesday: 125,
    saswellScheduleWednesday: 126,
    saswellScheduleThursday: 127,
    saswellScheduleFriday: 128,
    saswellScheduleSaturday: 129,
    saswellAntiScaling: 130,
*/

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
      TUYA_PRIVATE_CLUSTER_EF00 }},

/*
const dataPoints = {
    wateringTimer: {
        valve_state_auto_shutdown: 2,
        water_flow: 3,
        shutdown_timer: 11,
        remaining_watering_time: 101,
        valve_state: 102,
        last_watering_duration: 107,
        battery: 110,
    },
    // Common data points
    // Below data points are usually shared between devices
    state: 1,
    heatingSetpoint: 2,
    coverPosition: 2,
    dimmerLevel: 3,
    dimmerMinLevel: 3,
    localTemp: 3,
    coverArrived: 3,
    occupancy: 3,
    mode: 4,
    fanMode: 5,
    dimmerMaxLevel: 5,
    motorDirection: 5,
    config: 5,
    childLock: 7,
    coverChange: 7,
    runningState: 14,
    valveDetection: 20,
    battery: 21,
    tempCalibration: 44,
    // Data points above 100 are usually custom function data points
    waterLeak: 101,
    minTemp: 102,
    maxTemp: 103,
    windowDetection: 104,
    boostTime: 105,
    coverSpeed: 105,
    forceMode: 106,
    comfortTemp: 107,
    ecoTemp: 108,
    valvePos: 109,
    batteryLow: 110,
    weekFormat: 111,
    scheduleWorkday: 112,
    scheduleHoliday: 113,
    awayTemp: 114,
    windowOpen: 115,
    autoLock: 116,
    awayDays: 117,
    // Manufacturer specific
    // Earda
    eardaDimmerLevel: 2,
    // Siterwell Thermostat
    siterwellWindowDetection: 18,
    // Moes Thermostat
    moesHold: 2,
    moesScheduleEnable: 3,
    moesHeatingSetpoint: 16,
    moesMaxTempLimit: 18,
    moesMaxTemp: 19,
    moesDeadZoneTemp: 20,
    moesLocalTemp: 24,
    moesMinTempLimit: 26,
    moesTempCalibration: 27,
    moesValve: 36,
    moesChildLock: 40,
    moesSensor: 43,
    moesSchedule: 101,
    etopErrorStatus: 13,
    // MoesS Thermostat
    moesSsystemMode: 1,
    moesSheatingSetpoint: 2,
    moesSlocalTemp: 3,
    moesSboostHeating: 4,
    moesSboostHeatingCountdown: 5,
    moesSreset: 7,
    moesSwindowDetectionFunktion_A2: 8,
    moesSwindowDetection: 9,
    moesSchildLock: 13,
    moesSbattery: 14,
    moesSschedule: 101,
    moesSvalvePosition: 104,
    moesSboostHeatingCountdownTimeSet: 103,
    moesScompensationTempSet: 105,
    moesSecoMode: 106,
    moesSecoModeTempSet: 107,
    moesSmaxTempSet: 108,
    moesSminTempSet: 109,
    moesCoverCalibration: 3,
    moesCoverBacklight: 7,
    moesCoverMotorReversal: 8,
    // Neo T&H
    neoOccupancy: 101,
    neoPowerType: 101,
    neoMelody: 102,
    neoDuration: 103,
    neoTamper: 103,
    neoAlarm: 104,
    neoTemp: 105,
    neoTempScale: 106,
    neoHumidity: 106,
    neoMinTemp: 107,
    neoMaxTemp: 108,
    neoMinHumidity: 109,
    neoMaxHumidity: 110,
    neoUnknown2: 112,
    neoTempAlarm: 113,
    neoTempHumidityAlarm: 113,
    neoHumidityAlarm: 114,
    neoUnknown3: 115,
    neoVolume: 116,
    // Neo AlarmOnly
    neoAOBattPerc: 15,
    neoAOMelody: 21,
    neoAODuration: 7,
    neoAOAlarm: 13,
    neoAOVolume: 5,
    // Saswell TRV
    saswellHeating: 3,
    saswellWindowDetection: 8,
    saswellFrostDetection: 10,
    saswellTempCalibration: 27,
    saswellChildLock: 40,
    saswellState: 101,
    saswellLocalTemp: 102,
    saswellHeatingSetpoint: 103,
    saswellValvePos: 104,
    saswellBatteryLow: 105,
    saswellAwayMode: 106,
    saswellScheduleMode: 107,
    saswellScheduleEnable: 108,
    saswellScheduleSet: 109,
    saswellSetpointHistoryDay: 110,
    saswellTimeSync: 111,
    saswellSetpointHistoryWeek: 112,
    saswellSetpointHistoryMonth: 113,
    saswellSetpointHistoryYear: 114,
    saswellLocalHistoryDay: 115,
    saswellLocalHistoryWeek: 116,
    saswellLocalHistoryMonth: 117,
    saswellLocalHistoryYear: 118,
    saswellMotorHistoryDay: 119,
    saswellMotorHistoryWeek: 120,
    saswellMotorHistoryMonth: 121,
    saswellMotorHistoryYear: 122,
    saswellScheduleSunday: 123,
    saswellScheduleMonday: 124,
    saswellScheduleTuesday: 125,
    saswellScheduleWednesday: 126,
    saswellScheduleThursday: 127,
    saswellScheduleFriday: 128,
    saswellScheduleSaturday: 129,
    saswellAntiScaling: 130,
    // HY thermostat
    hyHeating: 102,
    hyExternalTemp: 103,
    hyAwayDays: 104,
    hyAwayTemp: 105,
    hyMaxTempProtection: 106,
    hyMinTempProtection: 107,
    hyTempCalibration: 109,
    hyHysteresis: 110,
    hyProtectionHysteresis: 111,
    hyProtectionMaxTemp: 112,
    hyProtectionMinTemp: 113,
    hyMaxTemp: 114,
    hyMinTemp: 115,
    hySensor: 116,
    hyPowerOnBehavior: 117,
    hyWeekFormat: 118,
    hyWorkdaySchedule1: 119,
    hyWorkdaySchedule2: 120,
    hyHolidaySchedule1: 121,
    hyHolidaySchedule2: 122,
    hyState: 125,
    hyHeatingSetpoint: 126,
    hyLocalTemp: 127,
    hyMode: 128,
    hyChildLock: 129,
    hyAlarm: 130,
    // Silvercrest
    silvercrestChangeMode: 2,
    silvercrestSetBrightness: 3,
    silvercrestSetColorTemp: 4,
    silvercrestSetColor: 5,
    silvercrestSetEffect: 6,
    // Fantem
    fantemPowerSupplyMode: 101,
    fantemReportingTime: 102,
    fantemExtSwitchType: 103,
    fantemTempCalibration: 104,
    fantemHumidityCalibration: 105,
    fantemLoadDetectionMode: 105,
    fantemLuxCalibration: 106,
    fantemExtSwitchStatus: 106,
    fantemTemp: 107,
    fantemHumidity: 108,
    fantemMotionEnable: 109,
    fantemControlMode: 109,
    fantemBattery: 110,
    fantemLedEnable: 111,
    fantemReportingEnable: 112,
    fantemLoadType: 112,
    fantemLoadDimmable: 113,
    // Woox
    wooxSwitch: 102,
    wooxBattery: 14,
    wooxSmokeTest: 8,
    // FrankEver
    frankEverTimer: 9,
    frankEverTreshold: 101,
    // Dinrail power meter switch
    dinrailPowerMeterTotalEnergy: 17,
    dinrailPowerMeterCurrent: 18,
    dinrailPowerMeterPower: 19,
    dinrailPowerMeterVoltage: 20,
    dinrailPowerMeterTotalEnergy2: 101,
    dinrailPowerMeterPower2: 103,
    // tuya smart air box
    tuyaSabCO2: 2,
    tuyaSabTemp: 18,
    tuyaSabHumidity: 19,
    tuyaSabVOC: 21,
    tuyaSabFormaldehyd: 22,
    // tuya Smart Air House Keeper, Multifunctionale air quality detector.
    // CO2, Temp, Humidity, VOC and Formaldehyd same as Smart Air Box
    tuyaSahkMP25: 2,
    tuyaSahkCO2: 22,
    tuyaSahkFormaldehyd: 20,
    // Tuya CO (carbon monoxide) smart air box
    tuyaSabCOalarm: 1,
    tuyaSabCO: 2,
    // Moes MS-105 Dimmer
    moes105DimmerState1: 1,
    moes105DimmerLevel1: 2,
    moes105DimmerState2: 7,
    moes105DimmerLevel2: 8,
    // Tuya Radar Sensor
    trsPresenceState: 1,
    trsSensitivity: 2,
    trsMotionState: 102,
    trsIlluminanceLux: 103,
    trsDetectionData: 104,
    trsScene: 112,
    trsMotionDirection: 114,
    trsMotionSpeed: 115,
    // Tuya Radar Sensor with fall function
    trsfPresenceState: 1,
    trsfSensitivity: 2,
    trsfMotionState: 102,
    trsfIlluminanceLux: 103,
    trsfTumbleSwitch: 105,
    trsfTumbleAlarmTime: 106,
    trsfScene: 112,
    trsfMotionDirection: 114,
    trsfMotionSpeed: 115,
    trsfFallDownStatus: 116,
    trsfStaticDwellAlarm: 117,
    trsfFallSensitivity: 118,
    // Human Presence Sensor AIR
    msVSensitivity: 101,
    msOSensitivity: 102,
    msVacancyDelay: 103,
    msMode: 104,
    msVacantConfirmTime: 105,
    msReferenceLuminance: 106,
    msLightOnLuminancePrefer: 107,
    msLightOffLuminancePrefer: 108,
    msLuminanceLevel: 109,
    msLedStatus: 110,
    // TV01 Moes Thermostat
    tvMode: 2,
    tvWindowDetection: 8,
    tvFrostDetection: 10,
    tvHeatingSetpoint: 16,
    tvLocalTemp: 24,
    tvTempCalibration: 27,
    tvWorkingDay: 31,
    tvHolidayTemp: 32,
    tvBattery: 35,
    tvChildLock: 40,
    tvErrorStatus: 45,
    tvHolidayMode: 46,
    tvBoostTime: 101,
    tvOpenWindowTemp: 102,
    tvComfortTemp: 104,
    tvEcoTemp: 105,
    tvWeekSchedule: 106,
    tvHeatingStop: 107,
    tvMondaySchedule: 108,
    tvWednesdaySchedule: 109,
    tvFridaySchedule: 110,
    tvSundaySchedule: 111,
    tvTuesdaySchedule: 112,
    tvThursdaySchedule: 113,
    tvSaturdaySchedule: 114,
    tvBoostMode: 115,
    // HOCH / WDYK DIN Rail
    hochCountdownTimer: 9,
    hochFaultCode: 26,
    hochRelayStatus: 27,
    hochChildLock: 29,
    hochVoltage: 101,
    hochCurrent: 102,
    hochActivePower: 103,
    hochLeakageCurrent: 104,
    hochTemperature: 105,
    hochRemainingEnergy: 106,
    hochRechargeEnergy: 107,
    hochCostParameters: 108,
    hochLeakageParameters: 109,
    hochVoltageThreshold: 110,
    hochCurrentThreshold: 111,
    hochTemperatureThreshold: 112,
    hochTotalActivePower: 113,
    hochEquipmentNumberType: 114,
    hochClearEnergy: 115,
    hochLocking: 116,
    hochTotalReverseActivePower: 117,
    hochHistoricalVoltage: 118,
    hochHistoricalCurrent: 119,
    // NOUS SMart LCD Temperature and Humidity Sensor E6
    nousTemperature: 1,
    nousHumidity: 2,
    nousBattery: 4,
    nousTempUnitConvert: 9,
    nousMaxTemp: 10,
    nousMinTemp: 11,
    nousMaxHumi: 12,
    nousMinHumi: 13,
    nousTempAlarm: 14,
    nousHumiAlarm: 15,
    nousHumiSensitivity: 20,
    nousTempSensitivity: 19,
    nousTempReportInterval: 17,
    nousHumiReportInterval: 18,
    // TUYA Temperature and Humidity Sensor
    tthTemperature: 1,
    tthHumidity: 2,
    tthBatteryLevel: 3,
    tthBattery: 4,
    // TUYA / HUMIDITY/ILLUMINANCE/TEMPERATURE SENSOR
    thitBatteryPercentage: 3,
    thitIlluminanceLux: 7,
    tIlluminanceLux: 2,
    thitHumidity: 9,
    thitTemperature: 8,
    // TUYA SMART VIBRATION SENSOR
    tuyaVibration: 10,
    // TUYA WLS-100z Water Leak Sensor
    wlsWaterLeak: 1,
    wlsBatteryPercentage: 4,
    // Evanell
    evanellMode: 2,
    evanellHeatingSetpoint: 4,
    evanellLocalTemp: 5,
    evanellBattery: 6,
    evanellChildLock: 8,
    // ZMAM02 Zemismart RF Courtain Converter
    AM02Control: 1,
    AM02PercentControl: 2,
    AM02PercentState: 3,
    AM02Mode: 4,
    AM02Direction: 5,
    AM02WorkState: 7,
    AM02CountdownLeft: 9,
    AM02TimeTotal: 10,
    AM02SituationSet: 11,
    AM02Fault: 12,
    AM02Border: 16,
    AM02MotorWorkingMode: 20,
    AM02AddRemoter: 101,
    // Matsee Tuya Garage Door Opener
    garageDoorTrigger: 1,
    garageDoorContact: 3,
    garageDoorStatus: 12,
    // Moes switch with optional neutral
    moesSwitchPowerOnBehavior: 14,
    moesSwitchIndicateLight: 15,
    // X5H thermostat
    x5hState: 1,
    x5hMode: 2,
    x5hWorkingStatus: 3,
    x5hSound: 7,
    x5hFrostProtection: 10,
    x5hSetTemp: 16,
    x5hSetTempCeiling: 19,
    x5hCurrentTemp: 24,
    x5hTempCorrection: 27,
    x5hWeeklyProcedure: 30,
    x5hWorkingDaySetting: 31,
    x5hFactoryReset: 39,
    x5hChildLock: 40,
    x5hSensorSelection: 43,
    x5hFaultAlarm: 45,
    x5hTempDiff: 101,
    x5hProtectionTempLimit: 102,
    x5hOutputReverse: 103,
    x5hBackplaneBrightness: 104,
    // Connected thermostat
    connecteState: 1,
    connecteMode: 2,
    connecteHeatingSetpoint: 16,
    connecteLocalTemp: 24,
    connecteTempCalibration: 28,
    connecteChildLock: 30,
    connecteTempFloor: 101,
    connecteSensorType: 102,
    connecteHysteresis: 103,
    connecteRunningState: 104,
    connecteTempProgram: 105,
    connecteOpenWindow: 106,
    connecteMaxProtectTemp: 107,
    // Tuya Smart Human Presence Sensor
    tshpsPresenceState: 1,
    tshpscSensitivity: 2,
    tshpsMinimumRange: 3,
    tshpsMaximumRange: 4,
    tshpsTargetDistance: 9,
    tshpsDetectionDelay: 101,
    tshpsFadingTime: 102,
    tshpsIlluminanceLux: 104,
    tshpsCLI: 103, // not recognize
    tshpsSelfTest: 6, // not recognize
    // Tuya Luminance Motion sensor
    lmsState: 1,
    lmsBattery: 4,
    lmsSensitivity: 9,
    lmsKeepTime: 10,
    lmsIlluminance: 12,
    // Alecto SMART-SMOKE10
    alectoSmokeState: 1,
    alectoSmokeValue: 2,
    alectoSelfChecking: 8,
    alectoCheckingResult: 9,
    alectoSmokeTest: 11,
    alectoLifecycle: 12,
    alectoBatteryState: 14,
    alectoBatteryPercentage: 15,
    alectoSilence: 16,
    // BAC-002-ALZB - Moes like thermostat with Fan control
    bacFanMode: 28,
    // Human Presence Sensor Zigbee Radiowave Tuya
    HPSZInductionState: 1,
    HPSZPresenceTime: 101,
    HPSZLeavingTime: 102,
    HPSZLEDState: 103,
    giexWaterValve: {
        battery: 108,
        currentTemperature: 106,
        cycleIrrigationInterval: 105,
        cycleIrrigationNumTimes: 103,
        irrigationEndTime: 102,
        irrigationStartTime: 101,
        irrigationTarget: 104,
        lastIrrigationDuration: 114,
        mode: 1,
        state: 2,
        waterConsumed: 111,
    },
    zsHeatingSetpoint: 16,
    zsChildLock: 40,
    zsTempCalibration: 104,
    zsLocalTemp: 24,
    zsBatteryVoltage: 35,
    zsComfortTemp: 101,
    zsEcoTemp: 102,
    zsHeatingSetpointAuto: 105,
    zsOpenwindowTemp: 116,
    zsOpenwindowTime: 117,
    zsErrorStatus: 45,
    zsMode: 2,
    zsAwaySetting: 103,
    zsBinaryOne: 106,
    zsBinaryTwo: 107,
    zsScheduleMonday: 109,
    zsScheduleTuesday: 110,
    zsScheduleWednesday: 111,
    zsScheduleThursday: 112,
    zsScheduleFriday: 113,
    zsScheduleSaturday: 114,
    zsScheduleSunday: 115,
};

*/

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB_XY, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_DIMMER_BULB, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_BULB, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},
      
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},
      
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},
      
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY, .z2s_device_clusters_count = 4, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }}, 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

    { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_DETECTOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
      TUYA_PRIVATE_CLUSTER_EF00 }}, 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
      TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }}, 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
      TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }}, 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IR_REMOTE_CONTROL, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ZOSUNG_IR_CONTROL_CUSTOM_CLUSTER }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_VALVE, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},
    
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_ALARM, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_BASIC }}

};
        
static z2s_device_entity_t Z2S_DEVICES_LIST[] PROGMEM = { 
  
  { .manufacturer_name = "_TZ3000_akqdg6g7", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_dowj6gyi", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Xiaomi", .model_name = "LYWSD03MMC-z", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Xiaomi", .model_name = "LYWSD03MMC", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_rdhukkmi", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_fllyghyj", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_v1w2k9dd", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_upagmta9", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_d7lpruvi", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_9yapgbuv", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_cirvgep4", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_d7lpruvi", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_vaarscrt", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1}, //NOUS SZ-T04

  { .manufacturer_name = "HOBEIAN", .model_name = "ZG-227Z", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_xr3htd96", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Zbeacon", .model_name = "TH01", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-02", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "SNZB-02D", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-02P", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "SNZB-02WD", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "SNZB-02LD", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_bjawzodf", .model_name = "TY0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_a8sdabtg", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_vs0skpuc", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_fllyghyj", .model_name = "SNZB-02", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "TH01", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.weather", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_bguser20", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_yujem9ee", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.sens", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_hzkeh3nv", .model_name = "TS0201",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_ncw88jfq", .model_name = "TS0201",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_aao3yzhs", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_myd45weu", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_0hkmcrza", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "TEST1", .model_name = "TS0202", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_mcxw5ehu", .model_name = "TS0202", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_26fmupbb", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_rcuyhwe3", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_2mbfxlzr", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_x8q36xwf", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_zutizvyk", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_bh3n6gk8", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_k4ej3ww2", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_85czd6fy", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_upgcbody", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_wuep9zng", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_upgcbody", .model_name = "SNZB-05", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "TEST1", .model_name = "TS0205", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_lf56vpxj", .model_name = "TS0202", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_decxrtwa", .model_name = "TS0203", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 2}, 

  { .manufacturer_name = "_TZ3000_yxqnffam", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_lqpt3mvr", .model_name = "TS0210",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "PARASOLL Door/Window Sensor",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1},
                              { 2, Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2 }}},
  
  { .manufacturer_name = "IKEA of Sweden", .model_name = "BADRING Water Leakage Sensor",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},
 
  { .manufacturer_name = "_TZE200_3towulqd", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_ILLUZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_1bwpjvlz", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-04", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_au1rjicn", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_bb6xaihh", .model_name = "SNZB-03", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-03", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "MS01", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "DS01", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_n2egfsli", .model_name = "SNZB-04", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_pay2byax", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_n8dljorx", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_up3pngle", .model_name = "TS0205", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_c8ozah8n", .model_name = "TS0202", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_6zvw8ham", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZ3040_bb6xaihh", .model_name = "TS0202", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "LUMI", .model_name = "lumi.magnet.ac01", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "LUMI", .model_name = "lumi.magnet.agl02", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.sensor_magnet.aq2", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.sensor_motion.aq2", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_tgvtvdoc", .model_name = "TS0207", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_u6x1zyv2", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Heiman", .model_name = "SMOK_V16", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Heiman", .model_name = "GAS_V15", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_iba1ckek", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_6ygjfyll", .model_name = "TS0202", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_7d8yme6f", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_bpkijo14", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_oxslv1c9", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "HEIMAN", .model_name = "SmokeSensor-EF-3.0", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_lzdjjfss", .model_name = "TS0210", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "ZB-DoorSensor-D0007", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_d93wtje5", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_8yhypbo7", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "SNZB-05P", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B, .z2s_device_endpoints_count = 1},   

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-04P", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "ZB-SMART-PIRTH-V3", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-03P", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_prits6g4", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS000F", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_q6a3tepg", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_tqlv4ug4", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SWITCH-ZR03-1", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "S26R2ZB", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_cmcjbqup", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_VALVE_DC, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Aqara", .model_name = "lumi.valve.agl001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_VALVE_DC, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_8n7lqbm0", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.switch.b1lc04", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI control outlet", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRETAKT Smart plug", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "LDSENK01F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},   

  { .manufacturer_name = "_TZ3000_mmkbptmx", .model_name = "TS0004", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH, .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 },
                              { 4, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_cehuw1lw", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZ3000_okaz9tjs", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_5f43h46b", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_w0qqde0g", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_ww6drja5", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_gjnozsaz", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.plug.mmeu01", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_2putqrmw", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_kqvb5akv", .model_name = "TS0001", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "LDSENK02F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "INSPELNING Smart plug", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},
 
  { .manufacturer_name = "_TZ3000_wkai4ga5", .model_name = "TS0044", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_endpoints_count = 4, 
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  { .manufacturer_name = "_TZ3000_mh9px7cq", .model_name = "TS0044", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_endpoints_count = 4, 
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  { .manufacturer_name = "_TZ3000_kfu8zapd", .model_name = "TS0044", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_endpoints_count = 4, 
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 4, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  { .manufacturer_name = "_TZ3000_famkxci2", .model_name = "TS0043", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_endpoints_count = 3, 
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  { .manufacturer_name = "_TZ3000_sj7jbgks", .model_name = "TS0043", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_endpoints_count = 3, 
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 2, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 },
                              { 3, Z2S_DEVICE_DESC_TUYA_SWITCH_4X3 }}},

  { .manufacturer_name = "_TZ3000_yj6k7vfo", .model_name = "TS0041", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_4fjiwweb", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_uri7ongn", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZ3000_ixla93vd", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_qja6nq5z", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_abrsvsou", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_402vrq2i", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_ja5osu5g", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_mrpevh8p", .model_name = "TS0041", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_8rppvwda", .model_name = "TS0041", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-01P", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "CK-TLSR8656-SS5-01(7000)", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},
    
  { .manufacturer_name = "IKEA of Sweden", .model_name = "Remote Control N2", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "SYMFONISK sound remote gen2", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1, .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints =  {{ 1, Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1 },
                              { 2, Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2 },
                              { 3, Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3 }}},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "SYMFONISK Sound Controller", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI on/off switch", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F, .z2s_device_endpoints_count = 1},
    
  { .manufacturer_name = "IKEA of Sweden", .model_name = "RODRET Dimmer", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "SOMRIG shortcut button", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints =  {{ 1, Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1 },
                              { 2, Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2 }}},
    

  { .manufacturer_name = "IKEA of Sweden", .model_name = "VALLHORN Wireless Motion Sensor", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_VALLHORN_1, .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_IKEA_VALLHORN_1 },
                              { 2, Z2S_DEVICE_DESC_IKEA_VALLHORN_2 },
                              { 3, Z2S_DEVICE_DESC_IKEA_VALLHORN_3 }}},

  { .manufacturer_name = "_TZ3000_gjrubzje", .model_name = "TS0001",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "ZBMINIL2",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "ZBMINIR2",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "01MINIZB",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_fdxihpp7", .model_name = "TS0001",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},
// TUYA HVAC SECTION 
  { .manufacturer_name = "_TZE200_0dvm9mva", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_fqm2sfpe", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_9gvruqf5", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_SASWELL, .z2s_device_endpoints_count = 1},


  { .manufacturer_name = "_TZE284_o3x45p96", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_9xfjixap", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_rxq4iti9", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_bvu2wnxz", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_6rdj8dzm", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZE200_p3dbf6qs", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZE200_rxntag7i", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_ME167, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_ymldrmzx", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV603, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_cpmgn2cf", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_MOES, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_b6wax7g0", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_BECA, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_rtrmfadk", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_rtrmfadk", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TS0601_TRV_TRV601, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "TRVZB",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_TRVZB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_keabpigv", .model_name = "TS0505A",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "zbeacon", .model_name = "TS0505",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E27 CWS globe 806lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E14 CWS globe 806lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb E14 CWS globe 806lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_RGBW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "\u001aTRADFRI bulb GU10 WW 345lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 WW 345lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "\\u001TRADFRI bulb GU10 WW 345lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "\u001aTRADFRI bulb GU10 WW 345lm8",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI bulb GU10 WW 380lm",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_WW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_ctv0gump", .model_name = "TS0501B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_bfwvfyx1", .model_name = "TS0505B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_3lbtuxgp", .model_name = "TS0505B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_wbsgmojq", .model_name = "TS0505B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_ljoasixl", .model_name = "TS0505B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_0zabbfax", .model_name = "TS0503B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "CK-BL702-AL-01(7009_Z102LG03-1)",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB_XY, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLight", .model_name = "ZB-CL01",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB_XY, .z2s_device_endpoints_count = 1},

   { .manufacturer_name = "_TZ3000_hhiodade", .model_name = "TS0011",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1, .z2s_device_endpoints_count = 1},

   { .manufacturer_name = "_TZ3000_ji4araar", .model_name = "TS0011",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1, .z2s_device_endpoints_count = 1},


  { .manufacturer_name = "_TZ3000_c7xsiexw", .model_name = "TS0002",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_p8alo7qa", .model_name = "TS0012",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_jl7qyupf", .model_name = "TS0012",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

 { .manufacturer_name = "_TZ3000_ljhbw1c9", .model_name = "TS0012",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

 { .manufacturer_name = "_TZ3000_xk5udnd6", .model_name = "TS0012",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_zmlunnhy", .model_name = "TS0012",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_aezbqpcu", .model_name = "TS0013",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH, .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_ltgngnqz", .model_name = "TS0002",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_sueu2ppq", .model_name = "TS0003",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH, .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},

  { .manufacturer_name = "_TZ3000_mw1pqqqt", .model_name = "TS0003",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH, .z2s_device_endpoints_count = 3,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1 }}},
  
  { .manufacturer_name = "_TZ3000_imaccztn", .model_name = "TS0004",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH, .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 3, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1},
                              { 4, Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1}}},
                              
  { .manufacturer_name = "_TZ3000_h1ipgkwn", .model_name = "TS0002",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_RELAY_1 },
                              { 2, Z2S_DEVICE_DESC_RELAY_1 }}},

  { .manufacturer_name = "_TZ3000_5ajpkyq6", .model_name = "TS0004",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 4,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_RELAY_1 },
                              { 2, Z2S_DEVICE_DESC_RELAY_1 },
                              { 3, Z2S_DEVICE_DESC_RELAY_1 },
                              { 4, Z2S_DEVICE_DESC_RELAY_1 }}},

  { .manufacturer_name = "_TZE204_jtbgusdc", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_rccxox8p", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_ntcy3xu1", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_rjxqso4a", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_CO_DETECTOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_2aaelwxk", .model_name = "TS0225",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_kb5noeto", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_ex3rcdha", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_rhgsbacq", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_9kbbfeho", .model_name = "TS0222",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_3ulg9kpo", .model_name = "TS0021",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_dikb3dp6", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_iwn0gpzz", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "LDSENK08",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "SWV",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_SONOFF_SMART_VALVE, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3290_7v1k4vufotpowp9z", .model_name = "TS1201",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IR_REMOTE_CONTROL, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_ol1uhvza", .model_name = "TS130F",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_c2yscrj8", .model_name = "TS0224",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SIREN_ALARM, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "ZB-Remote-D0001",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "ADEO", .model_name = "LDSENK09",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_icka1clh", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE204_t1blo2bj", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_MOES_ALARM, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_wzk0x7fq", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "HOBEIAN", .model_name = "ZG-102ZM",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR, .z2s_device_endpoints_count = 1}
};

#endif
//
