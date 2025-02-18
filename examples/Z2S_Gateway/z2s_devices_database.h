#ifndef Z2S_DEVICES_DATABASE_H_
#define Z2S_DEVICES_DATABASE_H_

// Z2S_DEVICE_DESC_ID - used for selection of clusters to bind and matching Supla channels

#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR             0x1000
#define Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1           0x1001

#define Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR   0x1100

#define Z2S_DEVICE_DESC_ILLUTEMPHUMIZONE_SENSOR         0x1200
#define Z2S_DEVICE_DESC_ILLUZONE_SENSOR                 0x1205

#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR                 0x2000
#define Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1               0x2001

#define Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR             0x2200

#define Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR            0x2300

#define Z2S_DEVICE_DESC_LIGHT_SOURCE                    0x3000
#define Z2S_DEVICE_DESC_LIGHT_BULB                      0x3010

#define Z2S_DEVICE_DESC_RGB_BULB                        0x3050
#define Z2S_DEVICE_DESC_TUYA_RGB_BULB                   0x3060

#define Z2S_DEVICE_DESC_RGBW_BULB                       0x3100
#define Z2S_DEVICE_DESC_TUYA_RGBW_BULB                  0x3110

#define Z2S_DEVICE_DESC_DIMMER_BULB                     0x3200
#define Z2S_DEVICE_DESC_TUYA_DIMMER_BULB                0x3210

#define Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH              0x3300
#define Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH       0x3305

#define Z2S_DEVICE_DESC_RELAY                           0x4000
#define Z2S_DEVICE_DESC_RELAY_1                         0x4001

#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER         0x4100
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1       0x4101
#define Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2       0x4102

#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER    0x4500
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1  0x4501
#define Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2  0x4502

#define Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_1             0x4600
#define Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_2             0x4601

#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F            0x5000
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F            0x5001
#define Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F            0x5002

#define Z2S_DEVICE_DESC_TUYA_SWITCH_4X3               0x5100
#define Z2S_DEVICE_DESC_IKEA_SMART_BUTTON             0x5200

#define Z2S_DEVICE_TUYA_HVAC                          0x6000

#define Z2S_DEVICE_DESC_ON_OFF                        0x8000
#define Z2S_DEVICE_DESC_ON_OFF_1                      0x8001


#define MAX_BOUND_ENDPOINTS 8
#define MAX_BOUND_CLUSTERS 16


#define TUYA_ON_OFF_CUSTOM_CMD_BUTTON_PRESS_ID        0xFD
#define TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID       0xFC

#define NO_CUSTOM_CMD_SID                            -0x01

#define TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID          0x00
#define TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID   0x01
#define TUYA_CUSTOM_CMD_BUTTON_HELD_SID             0x02
#define TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID     0x0A
#define TUYA_CUSTOM_CMD_BUTTON_ROTATE_LEFT_SID      0x0B

#define IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID        0x00
#define IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID           0x01
#define IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID        0x02
#define IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID           0x03
#define IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID        0x04
#define IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID           0x05
#define IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID        0x06
#define IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID           0x07

static char IKEA_STYRBAR_BUTTONS[][16] PROGMEM = {"ON PRESSED", "ON HELD", "OFF PRESSED", "OFF HELD", "LEFT PRESSED", "LEFT HELD", "RIGHT PRESSED", "RIGHT HELD"};


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
  char manufacturer_name[32];
  char model_name[32];
  uint32_t z2s_device_desc_id;
  uint8_t z2s_device_endpoints_count;
  z2s_device_endpoint_t z2s_device_endpoints[MAX_BOUND_ENDPOINTS];
} z2s_device_entity_t;

static z2s_device_desc_t Z2S_DEVICES_DESC[] PROGMEM = {
  
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ILLUTEMPHUMIZONE_SENSOR, .z2s_device_clusters_count = 5, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ILLUZONE_SENSOR, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
      ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE }},


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

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_TUYA_HVAC, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_SCENES,
      ESP_ZB_ZCL_CLUSTER_ID_GROUPS,
      TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_RGBW_BULB, .z2s_device_clusters_count = 3, .z2s_device_clusters =
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

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB, .z2s_device_clusters_count = 3, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
      ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL }},
      
  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_BULB, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
      ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_SWITCH, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }}, 

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_1, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_2, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_ON_OFF }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR, .z2s_device_clusters_count = 1, .z2s_device_clusters =
    { TUYA_PRIVATE_CLUSTER_EF00 }},

  { .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR, .z2s_device_clusters_count = 2, .z2s_device_clusters =
    { ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
      TUYA_PRIVATE_CLUSTER_EF00 }} 
    
};
        
static z2s_device_entity_t Z2S_DEVICES_LIST[] PROGMEM = { 
  { .manufacturer_name = "_TZ3000_akqdg6g7", .model_name = "TS0201", 
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
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_xr3htd96", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "SNZB-02D", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},
  
  { .manufacturer_name = "_TZ3000_bjawzodf", .model_name = "TY0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_a8sdabtg", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_fllyghyj", .model_name = "SNZB-02", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "TH01", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "LUMI", .model_name = "lumi.weather", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_bguser20", .model_name = "TS0201", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE284_aao3yzhs", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR, .z2s_device_endpoints_count = 1},

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

  { .manufacturer_name = "_TZE200_bh3n6gk8", .model_name = "TS0601", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_k4ej3ww2", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_85czd6fy", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_upgcbody", .model_name = "TS0207", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_upgcbody", .model_name = "SNZB-05", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "TEST1", .model_name = "TS0205", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_lf56vpxj", .model_name = "TS0202", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_decxrtwa", .model_name = "TS0203", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1}, 

  { .manufacturer_name = "_TZ3000_yxqnffam", .model_name = "TS0203",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_3towulqd", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_ILLUZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_1bwpjvlz", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-04", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_au1rjicn", .model_name = "TS0203", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "eWeLink", .model_name = "SNZB-03", 
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
  
  { .manufacturer_name = "LUMI", .model_name = "lumi.magnet.agl02", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_tgvtvdoc", .model_name = "TS0207", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Heiman", .model_name = "SMOK_V16", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "Heiman", .model_name = "GAS_V15", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_iba1ckek", .model_name = "TS0601", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_prits6g4", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_skueekg3", .model_name = "TS0001", 
   .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_cehuw1lw", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},
  
    { .manufacturer_name = "_TZ3000_okaz9tjs", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_5f43h46b", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_w0qqde0g", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_ww6drja5", .model_name = "TS011F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER, .z2s_device_endpoints_count = 1},

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

  { .manufacturer_name = "_TZ3000_ja5osu5g", .model_name = "TS004F", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_mrpevh8p", .model_name = "TS0041", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "Remote Control N2", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "IKEA of Sweden", .model_name = "TRADFRI on/off switch", 
    .z2s_device_desc_id = Z2S_DEVICE_DESC_IKEA_SMART_BUTTON, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_gjrubzje", .model_name = "TS0001",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_ON_OFF, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "SONOFF", .model_name = "ZBMINIL2",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_fdxihpp7", .model_name = "TS0001",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_0dvm9mva", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_TUYA_HVAC, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_keabpigv", .model_name = "TS0505A",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_bfwvfyx1", .model_name = "TS0505B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3210_31btuxgp", .model_name = "TS0505B",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB, .z2s_device_endpoints_count = 1},
    
  { .manufacturer_name = "_TZ3000_c7xsiexw", .model_name = "TS0002",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_1, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_1},
                              { 2, Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_2 }}},

  { .manufacturer_name = "_TZ3000_h1ipgkwn", .model_name = "TS0002",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_RELAY_1, .z2s_device_endpoints_count = 2,
    .z2s_device_endpoints = { { 1, Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER },
                              { 2, Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER }}},
                              
  { .manufacturer_name = "_TZE204_jtbgusdc", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_rccxox8p", .model_name = "TS0601",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZE200_2aaelwxk", .model_name = "TS0225",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR, .z2s_device_endpoints_count = 1},

  { .manufacturer_name = "_TZ3000_9kbbfeho", .model_name = "TS0222",
    .z2s_device_desc_id = Z2S_DEVICE_DESC_ILLUTEMPHUMIZONE_SENSOR, .z2s_device_endpoints_count = 1}

};

#endif

//  
