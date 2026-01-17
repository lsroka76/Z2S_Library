#ifndef Z2S_DEVICES_TABLE_H_
#define Z2S_DEVICES_TABLE_H_

#include <ZigbeeGateway.h>
#include "z2s_devices_database.h"
#include "priv_auth_data.h"

#ifdef USE_TELNET_CONSOLE

#include "z2s_telnet_server.h"

#endif  //USE_TELNET_CONSOLE

#include <supla/tools.h>
#include <supla/actions.h>
#include <supla/events.h>
#include <supla/element.h>
#include <supla/protocol/supla_srpc.h>
#include <supla/device/register_device.h>
#include <supla/device/channel_conflict_resolver.h>
#include <supla/device/subdevice_pairing_handler.h>
#include <supla/mutex.h>
#include <supla/auto_lock.h>
#include <SuplaDevice.h>

#include <supla/storage/config.h>
#include <supla/storage/storage.h>


#define Z2S_ZB_DEVICES_MAX_NUMBER                               0x20  //32
#define Z2S_CHANNELS_MAX_NUMBER                                 0x80  //128
#define Z2S_ACTIONS_MAX_NUMBER                                  0x100 //256

#define MAX_ZB_DEVICE_SUPLA_CHANNELS                            0x10 //16

#define ALL_SUPLA_CHANNEL_TYPES   -1

#define ADD_Z2S_DEVICE_STATUS_OK                                0x00  //device added 100%
#define ADD_Z2S_DEVICE_STATUS_DUN                               0x01  //device unknown
#define ADD_Z2S_DEVICE_STATUS_DT_FULL                           0x02  //device table full = device not added
#define ADD_Z2S_DEVICE_STATUS_DT_FWA                            0x03  //device table full while adding = device added partialy
#define ADD_Z2S_DEVICE_STATUS_DAP                               0x04  //device already present

#define USER_DATA_FLAG_SED_TIMEOUT                              (1 << 0)  // 0x0001
#define USER_DATA_FLAG_MSG_DISABLED                             (1 << 1)  // 0x0002

#define USER_DATA_FLAG_CORRECTIONS_DISABLED                     (1 << 2)  // 0x0004
#define USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE                     (1 << 3)  // 0x0008
#define USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG                      (1 << 4)  // 0x0010
#define USER_DATA_FLAG_DISABLE_NOTIFICATIONS                    (1 << 5)  // 0x0020
#define USER_DATA_FLAG_SET_SORWNS_ON_START                      (1 << 6)  // 0x0040
#define USER_DATA_FLAG_HAS_EXTENDED_DATA                        (1 << 7)  // 0x0080
#define USER_DATA_FLAG_TRV_FIXED_CORRECTION                     (1 << 8)  // 0x0100
#define USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL              (1 << 9)  // 0x0200
#define USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK                (1 << 10) // 0x0400
#define USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE                (1 << 11) // 0x0800
#define USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS                 (1 << 12) // 0x1000
#define USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0               (1 << 13) // 0x2000

#define ZBD_USER_DATA_FLAG_VERSION_2_0                          (1 << 0)
#define ZBD_USER_DATA_FLAG_RESERVED_1                           (1 << 1)
#define ZBD_USER_DATA_FLAG_RESERVED_2                           (1 << 2)
#define ZBD_USER_DATA_FLAG_RESERVED_3                           (1 << 3)
#define ZBD_USER_DATA_FLAG_RESERVED_4                           (1 << 4)
#define ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED                 (1 << 5)
#define ZBD_USER_DATA_FLAG_DISABLE_BATTERY_MSG                  (1 << 6)
#define ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG       (1 << 7)
#define ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG          (1 << 8)

#define ZBD_USER_DATA_FLAG_TUYA_USE_SEND_DATA                   (1 << 13)
#define ZBD_USER_DATA_FLAG_TUYA_FORCE_TIME_SYNC                 (1 << 14)
#define ZBD_USER_DATA_FLAG_TUYA_MCU_VERSION_REQUEST             (1 << 15)
#define ZBD_USER_DATA_FLAG_TUYA_QUERY_AFTER_REJOIN              (1 << 16)
#define ZBD_USER_DATA_FLAG_IAS_ZONE_STATUS_QUERY_AFTER_REJOIN   (1 << 17)
#define ZBD_USER_DATA_FLAG_ON_OFF_STATE_QUERY_AFTER_REJOIN      (1 << 18)

#define ZBD_BATTERY_PERCENTAGE_MSG                              0x01
#define ZBD_BATTERY_VOLTAGE_MSG                                 0x02
#define ZBD_BATTERY_LEVEL_MSG                                   0x03
#define ZBD_BATTERY_STATE_MSG                                   0x04
#define ZBD_LOW_BATTERY_MSG                                     0x05
#define ZBD_BATTERY_RESTORE_MSG                                 0x06

#define CHANNEL_EXTENDED_DATA_TYPE_NULL                         0x00

#define GATEWAY_EVENTS_LOCAL_CHANNEL_SLOT                       0x7F

typedef void (*_actionhandler_callback)(int event, int action);

typedef struct z2s_legacy_device_params_s {

  bool valid_record;
  uint32_t model_id;
  esp_zb_ieee_addr_t ieee_addr;
  uint8_t endpoint;
  uint16_t cluster_id;
  uint16_t short_addr;
  uint8_t Supla_channel;
  int32_t Supla_channel_type;
  char Supla_channel_name[30];
  uint32_t Supla_channel_func;
  int8_t sub_id;
} z2s_legacy_device_params_t;

typedef struct z2s_legacy_2_device_params_s {

  bool valid_record;
  uint32_t model_id;
  esp_zb_ieee_addr_t ieee_addr;
  uint8_t endpoint;
  uint16_t cluster_id;
  uint16_t short_addr;
  uint8_t Supla_channel;
  uint8_t Supla_secondary_channel;
  int32_t Supla_channel_type;
  char Supla_channel_name[30];
  uint32_t Supla_channel_func;
  int8_t sub_id;
  uint32_t user_data_1;
  uint32_t user_data_2;
} z2s_legacy_2_device_params_t;

typedef struct z2s_device_params_s {

  bool                valid_record;
  uint8_t             extended_data_type;
  uint8_t             local_channel_type;
  uint8_t             local_channel_func;
  uint32_t            model_id;
  esp_zb_ieee_addr_t  ieee_addr;
  uint8_t             endpoint;
  uint16_t            cluster_id;
  uint16_t            short_addr;
  uint8_t             Supla_channel;
union {
  uint8_t             Supla_secondary_channel;
  uint8_t             Supla_remote_channel;
};
  int32_t             Supla_channel_type;
  char                Supla_channel_name[32];
  uint32_t            Supla_channel_func;
  int8_t              sub_id;
  uint8_t             reserved_4;
  uint16_t            gui_control_id;
  
  union {
    struct {
      uint32_t        user_data_1; 
      uint32_t        user_data_2;
      uint32_t        user_data_3;
      uint32_t        user_data_4; 
  
    };
    struct {
      uint32_t        rain_intensity_treshold;
    };
    struct {
      uint32_t        rgb_color_mode;
    };
    struct {
      int32_t        hvac_fixed_temperature_correction;
    };
    struct {
      uint32_t        value : 24;
      uint32_t        program : 8;
      uint32_t        pause_time : 24;
      uint32_t        cycles : 8;
    } smart_valve_data;
    struct {
      Supla::Element  *Supla_element;
      uint8_t         logic_operator;
    } local_action_handler_data;
    struct {
      Supla::Element  *Supla_element;
      uint32_t         button_flags;
      uint32_t         button_last_seen_ms;
      //uint32_t         button_debounce_ms;
    } virtual_button_data;
    struct {
      char            mDNS_name[12];
      uint32_t        remote_ip_address;
    } remote_channel_data;
  };
  uint32_t            user_data_flags;
  uint32_t            timeout_secs;
  uint32_t            keep_alive_secs;
union {  
  uint32_t            refresh_secs;
  uint32_t            debounce_ms;
};
union {
  struct {
    uint64_t          data_counter;
  };
  struct {
      uint32_t        last_temperature_measurement;//TEMP*100
    };
};
  uint8_t             Zb_device_id;
  uint8_t             reserved_7;
  uint8_t             reserved_8;
  uint8_t             reserved_9;
} z2s_device_params_t;

typedef struct z2s_legacy_zb_device_params_s {

  uint8_t record_id;
  char manufacturer_name[32];
  char model_name[32];
  esp_zb_ieee_addr_t ieee_addr;
  uint16_t short_addr;
  uint8_t endpoints_count;
  uint32_t desc_id;
  uint32_t device_status;
  uint8_t power_source;
  int8_t rssi;
  uint8_t battery_percentage;
  uint8_t battery_voltage_min;
  uint8_t battery_voltage_max;
  uint32_t last_seen_ms;
  uint32_t keep_alive_ms;
  uint32_t timeout_ms;
  uint32_t user_data_flags;
  uint32_t user_data_1;
  uint32_t user_data_2;
  uint8_t Supla_channels[MAX_ZB_DEVICE_SUPLA_CHANNELS];
} z2s_legacy_zb_device_params_t;

typedef struct z2s_zb_device_v2_params_s {

  uint8_t device_uid[4];
  uint8_t devices_list_idx[4];
  uint8_t device_gui_id;

} z2s_zb_device_v2_params_t;

typedef struct z2s_legacy_2_zb_device_params_s {

  uint8_t record_id;

  union {  //V2 extension
    char manufacturer_name[33];
    z2s_zb_device_v2_params_t v2_params;
  };

  union {  //V2 extension
    char model_name[33];
    char device_local_name[33];
  };

  esp_zb_ieee_addr_t ieee_addr;
  uint16_t short_addr;
  uint8_t endpoints_count;
  uint32_t desc_id;
  uint32_t device_status;
  uint8_t power_source;
  int8_t rssi;
  uint8_t battery_percentage;
  uint8_t battery_voltage_min;
  uint8_t battery_voltage_max;
  uint32_t last_seen_ms;
  uint32_t keep_alive_ms;
  uint32_t timeout_ms;
  uint32_t user_data_flags;
  union {
    struct {
      uint32_t user_data_1;  //Sonoff SWV b31 = 0 (time)/1(volume) b30-b24 cycles# b23-b0 worktime/volume
      uint32_t user_data_2;  //Sonoff SWV b31-b24 reserved, b23-b0 pause
    };
    struct {
      uint32_t value : 24;
      uint32_t program : 8;
      uint32_t pause_time : 24;
      uint32_t cycles : 8;
    } smart_valve_data;
  };
  uint64_t user_data_3;
  uint64_t user_data_4;
  uint8_t Supla_channels[MAX_ZB_DEVICE_SUPLA_CHANNELS];
} z2s_legacy_2_zb_device_params_t;

typedef struct z2s_zb_device_params_s {

  uint32_t record_id;
  uint32_t device_uid;
  uint32_t devices_list_idx;
  uint32_t desc_id;
  uint32_t device_gui_id;
  uint32_t reserved_0;
  uint32_t reserved_1;
  char device_local_name[36];
  esp_zb_ieee_addr_t ieee_addr;
  uint16_t short_addr;
  uint8_t endpoints_count;
  uint8_t power_source;
  int8_t rssi;
  uint8_t battery_percentage;
  uint8_t battery_voltage_min;
  uint8_t battery_voltage_max;
  uint32_t last_seen_ms;
  uint32_t keep_alive_ms;
  uint32_t timeout_ms;
  uint32_t user_data_flags;
  union {
    struct {
      uint32_t user_data_1;
      uint32_t user_data_2; 
    };
    struct {
      uint32_t value : 24;
      uint32_t program : 8;
      uint32_t pause_time : 24;
      uint32_t cycles : 8;
    } smart_valve_data;
  };
  uint64_t user_data_3;
  uint64_t user_data_4;
  //uint8_t             Supla_channels[MAX_ZB_DEVICE_SUPLA_CHANNELS];
} __attribute__((packed)) z2s_zb_device_params_t;  //fields are padded properly anyway

typedef struct z2s_channel_action_s {
  uint16_t action_id;
  bool is_enabled;
  uint8_t src_Supla_channel;
  uint8_t dst_Supla_channel;
  Supla::Event src_Supla_event;
  Supla::Action dst_Supla_action;
  char action_name[33];
  char action_description[128];
  bool is_condition;
  double min_value;
  double max_value;
} z2s_channel_action_t;

typedef struct sonoff_smart_valve_cycle_data_s {
  uint8_t id;
  uint8_t cycle_number;
  uint8_t cycles_count;
  uint32_t cycle_data;
  uint32_t cycle_pause;
} ESP_ZB_PACKED_STRUCT sonoff_smart_valve_cycle_data_t;

extern z2s_device_params_t z2s_channels_table[Z2S_CHANNELS_MAX_NUMBER];

const static char Z2S_CHANNELS_TABLE_ID[] PROGMEM = "Z2S_devs_table";
const static char Z2S_CHANNELS_TABLE_ID_V2[] PROGMEM = "channels_table_v2.z2s";
const static char Z2S_CHANNELS_TABLE_BACKUP_ID_V2[] PROGMEM = "channels_table_v2.bak";
const static char Z2S_CHANNELS_TABLE_SIZE_ID[] PROGMEM = "Z2S_devs_ts";

extern z2s_zb_device_params_t z2s_zb_devices_table[Z2S_ZB_DEVICES_MAX_NUMBER];

const static char Z2S_ZB_DEVICES_TABLE[] PROGMEM = "Z2S_zbd_table";
const static char Z2S_ZB_DEVICES_TABLE_ID_V2[] PROGMEM = "zb_devices_table_v2.z2s";
const static char Z2S_ZB_DEVICES_TABLE_BACKUP_ID_V2[] PROGMEM = "zb_devices_table_v2.bak";

const static char Z2S_ZB_DEVICES_TABLE_SIZE[] PROGMEM = "Z2S_zbd_ts";

extern uint8_t z2s_actions_index_table[Z2S_ACTIONS_MAX_NUMBER / 8];  //bit-indexed table

//const static char   Z2S_CHANNELS_ACTIONS_TABLE      []  PROGMEM = "Z2S_actions";
//const static char   Z2S_CHANNELS_ACTIONS_TABLE_SIZE []  PROGMEM = "Z2S_actions_ts";

const static char Z2S_CHANNELS_ACTIONS_INDEX_TABLE[] PROGMEM = "Z2S_actions_i";
const static char Z2S_CHANNELS_ACTIONS_INDEX_TABLE_V2[] PROGMEM = "actions_index_table.z2s";
const static char Z2S_CHANNELS_ACTIONS_PPREFIX[] PROGMEM = "Z2S_an_";
const static char Z2S_CHANNELS_ACTIONS_PPREFIX_V2[] PROGMEM = "action_%04d.z2s";
const static char Z2S_CHANNELS_ACTIONS_NUMBER[] PROGMEM = "Z2S_actions_n";


const static char Z2S_FILES_STRUCTURE_VERSION[] PROGMEM = "Z2S_files_ver";

const static char Z2S_CHANNELS_EXTENDED_DATA_PPREFIX_V2[] PROGMEM = "channel_ext_data_%03d_%02d.z2s";

extern bool sendIASNotifications;

//static NetworkClient Z2S_NetworkClient;

extern char GatewayMDNSLocalName[12];

//extern Supla::Sensor::GeneralPurposeMeasurement *Test_GeneralPurposeMeasurement;

const static char Z2S_ZIGBEE_PRIMARY_CHANNEL[] PROGMEM = "Z2S_primary_ch";

const static char Z2S_ENABLE_GUI_ON_START[] PROGMEM = "Z2S_enable_gui";
const static char Z2S_ENABLE_GUI_ON_START_V2[] PROGMEM = "Z2S_gui_mode";
const static char Z2S_GUI_ON_START_DELAY[] PROGMEM = "Z2S_gui_delay";
const static char Z2S_GUI_ON_START_DELAY_V2[] PROGMEM = "Z2S_gui_delay2";
const static char Z2S_FORCE_CONFIG_ON_START[] PROGMEM = "Z2S_force_cfg";
const static char Z2S_REBUILD_CHANNELS_ON_START[] PROGMEM = "Z2S_rebuild";
const static char Z2S_USE_NEW_AT_MODEL[] PROGMEM = "Z2S_new_at";
const static char Z2S_GATEWAY_MDNS_LOCAL_NAME[] PROGMEM = "Z2S_mdns_name";

namespace Supla {
enum Conditions {
  ON_LESS,
  ON_LESS_EQ,
  ON_GREATER,
  ON_GREATER_EQ,
  ON_BETWEEN,
  ON_BETWEEN_EQ,
  ON_EQUAL,
};
};

void ieee_addr_to_str(char *ieee_addr_str, esp_zb_ieee_addr_t ieee_addr);

/*****************************************************************************/

uint32_t Z2S_getChannelsTableSize();
uint8_t Z2S_findFirstFreeChannelsTableSlot(uint8_t start_slot = 0);
uint8_t Z2S_findFirstFreeLocalActionHandlerId(uint8_t start_slot = 0);
void Z2S_printChannelsTableSlots(bool toTelnet = false);
bool Z2S_loadChannelsTable();
bool Z2S_saveChannelsTable();
bool Z2S_removeAllChannels();

bool Z2S_removeChannel(int16_t channel_number_slot, bool save_table = true);

/*****************************************************************************/

uint8_t Z2S_findFirstFreeZbDevicesTableSlot(uint8_t start_slot = 0);
uint32_t Z2S_getZbDevicesTableSize();
bool Z2S_saveZbDevicesTable();
bool Z2S_loadZbDevicesTable();
bool Z2S_clearZbDevicesTable();
void Z2S_printZbDevicesTableSlots(bool toTelnet = false);
uint8_t Z2S_findZbDeviceTableSlot(esp_zb_ieee_addr_t ieee_addr);
uint8_t Z2S_countChannelsWithZbDeviceId(uint8_t Zb_device_id);
bool Z2S_hasZbDevice(uint32_t desc_id);
void Z2S_initZbDevices(uint32_t init_ms);
void Z2S_updateZbDeviceLastSeenMs(
  esp_zb_ieee_addr_t ieee_addr, uint32_t last_seen_ms);

uint8_t Z2S_addZbDeviceTableSlot(
  esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr,
  const char *manufacturer_name, const char *model_name,
  uint8_t endpoints_count, uint32_t desc_id, uint8_t power_source);

bool Z2S_removeZbDevice(uint8_t zb_device_slot, bool save_table = true);
bool Z2S_removeZbDeviceWithAllChannels(uint8_t zb_device_slot, 
  bool save_tables = true);
bool Z2S_removeAllZbDeviceWithAllChannels();

/*****************************************************************************/

int16_t Z2S_findChannelNumberSlot(
  esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, 
  int32_t channel_type, int8_t sub_id);

int16_t Z2S_findChannelNumberSlotV2(
  esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, 
  int32_t channel_type, int8_t sub_id, uint32_t channel_flags = UINT32_MAX);


int16_t Z2S_findChannelNumberNextSlot(
  int16_t prev_slot, 
  esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster,
  int32_t channel_type, int8_t sub_id);

void Z2S_fillChannelsTableSlot(
  zbg_device_params_t *device, uint8_t slot, uint8_t channel,
  int32_t channel_type, int8_t sub_id, const char *name = nullptr, 
  uint32_t func = 0, uint8_t secondary_channel = 0xFF,
  uint8_t extended_data_type = CHANNEL_EXTENDED_DATA_TYPE_NULL,
  uint8_t *extended_data = nullptr);

/*****************************************************************************/

bool Z2S_setChannelFlags(
  int16_t channel_number_slot, uint32_t flags_to_set, bool save_table = true);

bool Z2S_clearChannelFlags(
  int16_t channel_number_slot, uint32_t flags_to_clear, 
  bool save_table = true);

bool Z2S_checkChannelFlags(
  int16_t channel_number_slot, uint32_t flags_to_check);

/*****************************************************************************/

void Z2S_syncZbDeviceDescFlags(
  uint32_t devices_desc_slot, uint8_t zb_device_slot);

bool Z2S_setZbDeviceFlags(int8_t device_number_slot, uint32_t flags_to_set);

bool Z2S_clearZbDeviceFlags(
  int8_t device_number_slot, uint32_t flags_to_clear);

bool Z2S_checkZbDeviceFlags(
  int8_t device_number_slot, uint32_t flags_to_check);

/*****************************************************************************/

const char *Z2S_getZbDeviceManufacturerName(int8_t device_number_slot);
const char *Z2S_getZbDeviceModelName(int8_t device_number_slot);
char *Z2S_getZbDeviceLocalName(int8_t device_number_slot);

/*****************************************************************************/

int16_t Z2S_findTableSlotByChannelNumber(uint8_t channel_id);

/*****************************************************************************/

bool Z2S_updateZbDeviceUidIdx(
  uint8_t zb_device_slot, 
  const char *manufacturer_name, const char *model_name);

/*****************************************************************************/

Supla::Element *Z2S_getSuplaElementByChannelNumber(uint8_t channel_id);

/*****************************************************************************/

void Z2S_initSuplaChannels();

void Z2S_rebuildSuplaChannels();

/*****************************************************************************/

bool checkActionsIndexTablePosition(uint16_t index_position);
bool setActionsIndexTablePosition(uint16_t index_position);
bool clearActionsIndexTablePosition(uint16_t index_position);

bool Z2S_loadActionsIndexTable();
bool Z2S_saveActionsIndexTable();
uint16_t Z2S_getActionsNumber();
int16_t Z2S_getActionCounter(uint16_t action_position);

int16_t Z2S_findFreeActionIndex();
int16_t Z2S_findNextActionPosition(uint16_t action_position = 0);
int16_t Z2S_findPrevActionPosition(
  uint16_t action_position = Z2S_ACTIONS_MAX_NUMBER);

bool Z2S_saveAction(uint16_t action_index, z2s_channel_action_t &action);
bool Z2S_loadAction(uint16_t action_index, z2s_channel_action_t &action);
bool Z2S_removeAction(uint16_t action_index);
void Z2S_removeChannelActions(uint8_t channel_id, bool all_channels = false);

void Z2S_initSuplaActions();

/*****************************************************************************/

uint16_t Z2S_getChannelExtendedDataTypeSize(
  uint8_t extended_data_type);

bool Z2S_saveChannelExtendedData(
  int16_t channel_number_slot, uint8_t extended_data_type, 
  uint8_t *extended_data, bool save_table = true);

bool Z2S_removeChannelExtendedData(
  int16_t channel_number_slot, uint8_t extended_data_type, 
  bool save_table = true);

bool Z2S_loadChannelExtendedData(
  int16_t channel_number_slot, uint8_t extended_data_type,
  uint8_t *extended_data);


/*****************************************************************************/

void Z2S_onTemperatureReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  float temperature);

void Z2S_onHumidityReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  float humidity);

void Z2S_onPressureReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  float pressure);

void Z2S_onPM25Receive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  float pm25);

void Z2S_onIlluminanceReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t illuminance);

void Z2S_onFlowReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t flow);

void Z2S_onOccupancyReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint8_t occupancy);

void Z2S_onOnOffReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  bool state);

void Z2S_onElectricalMeasurementReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onMultistateInputReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onAnalogInputReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onMeteringReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onBasicReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr, uint16_t endpoint, 
  uint16_t cluster, const esp_zb_zcl_attribute_t *attribute);

void Z2S_onCurrentLevelReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t level);

void Z2S_onColorHueReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint8_t hue);

void Z2S_onColorSaturationReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint8_t saturation);

void Z2S_onThermostatTemperaturesReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t id, int16_t temperature);

void Z2S_onThermostatModesReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t id, uint16_t mode);

void Z2S_onWindowCoveringReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t id, uint16_t value);

void Z2S_onSonoffCustomClusterReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onDevelcoCustomClusterReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onLumiCustomClusterReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onIkeaCustomClusterReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster,
  const esp_zb_zcl_attribute_t *attribute);

void Z2S_onBatteryReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint16_t id, uint8_t battery_remaining);

bool Z2S_onCustomCmdReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster_id, 
  uint8_t command_id, uint8_t buffer_size, uint8_t *buffer);

void Z2S_onCmdCustomClusterReceive(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  uint8_t command_id, uint16_t payload_size, uint8_t *payload);

void Z2S_onIASzoneStatusChangeNotification(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
  int iaszone_status);

void Z2S_onBTCBoundDevice(zbg_device_params_t *device, uint8_t count, 
                          uint8_t position);

void Z2S_onBoundDevice(zbg_device_params_t *device, bool last_cluster);

void Z2S_onDataSaveRequest(
  uint8_t Supla_channel, uint8_t data_save_mode,  uint8_t extended_data_type, 
  uint8_t *extended_data);

void Z2S_onDeviceRejoin(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr);

uint8_t Z2S_addZ2SDevice(
  zbg_device_params_t *device, int8_t sub_id = -1, const char *name = nullptr,
  uint32_t func = 0, const char *unit = nullptr);

void Z2S_buildSuplaChannels(
  zbg_device_params_t *joined_device, uint8_t endpoint_counter);

uint8_t Z2S_addZ2SZbDevice(
  const char *manufacturer_name, const char *model_name,
  esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr, uint8_t endpoints,
  uint32_t desc_id);

void updateTimeout(
  uint8_t channel_number_slot, uint8_t timeout, uint8_t selector = 0,
  uint32_t timings_secs = 0);

void updateRGBMode(uint8_t channel_number_slot, uint8_t rgb_mode);

void sendChannelAction(uint8_t Supla_channel, uint16_t channel_action);

void setRemoteRelay(uint8_t Supla_channel, bool state);

void updateRemoteRelayMDNSName(uint8_t channel_number_slot, char * mDNS_name);

void updateRemoteRelayIPAddress(
  uint8_t channel_number_slot, uint32_t remote_ip_address);

void updateRemoteRelaySuplaChannel(
  uint8_t channel_number_slot, uint8_t remote_Supla_channel);

void updateRemoteThermometer(
  uint8_t Supla_channel, uint32_t connected_thermometer_ip_address,
  uint32_t connected_thermometer_channel, 
  int32_t connected_thermometer_temperature);

void setRemoteThermometerFunction(
  uint8_t Supla_channel, uint32_t connected_thermometers_function);

void updateHvacFixedCalibrationTemperature(
  uint8_t channel_number_slot, int32_t hvac_fixed_calibration_temperature,
  bool set_trv_interface = true);

void updateDeviceTemperature(
  uint8_t channel_number_slot, int32_t temperature);

void updateSuplaBatteryLevel(
  int16_t channel_number_slot, uint8_t msg_id, uint32_t msg_value,
  bool restore = false);

bool Z2S_add_action(
  const char *action_name, uint8_t src_channel_id, uint16_t Supla_action,
  uint8_t dst_channel_id, uint16_t Supla_event,bool condition,
  double threshold_1 = 0, double threshold_2 = 0);

bool hasTuyaCustomCluster(uint32_t model_id);

void log_i_telnet2(char *log_line, bool toTelnet = false);

void onTuyaCustomClusterReceive(
  void (*callback)(uint8_t command_id,uint16_t payload_size, 
  uint8_t *payload_data));

void handleGatewayEvent(int event);
void setGatewayEventHandler(_actionhandler_callback actionhandler_callback);
void disableGatewayActions();

void printSizeOfClasses();
void printTaskInfo(bool toTelnet = false);

class ZbDevicesConfigurator : public Supla::Element {

public:
  void onRegistered(Supla::Protocol::SuplaSrpc *srpc) {
  
    this->srpc = srpc;
    // delay sending subdevice details to server by 1 s
    lastSendTime = millis();

    for (int i = 0; i < Z2S_ZB_DEVICES_MAX_NUMBER; i++) {

      if (z2s_zb_devices_table[i].record_id > 0)
        //z2s_zb_devices_table[i].reserved_0 = 0;
        z2s_zb_devices_table[i].user_data_flags &= 
          ~ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED;
    }
  }

  bool iterateConnected() {
  
    bool dataSend = false;
    
    if (srpc == nullptr ||  millis() - lastSendTime < 12000) {
      return !dataSend;
    }
    lastSendTime = millis() - 11500;

    for (int i = 0; i < Z2S_ZB_DEVICES_MAX_NUMBER; i++) {


      if (z2s_zb_devices_table[i].record_id > 0) {

        if (z2s_zb_devices_table[i].user_data_flags & 
          ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED)
        continue;

        TDS_SubdeviceDetails subdeviceDetails = {};
    
        subdeviceDetails.SubDeviceId = i + 1;
    
        snprintf(
          subdeviceDetails.Name, SUPLA_DEVICE_NAME_MAXSIZE - 1, "%s (%s::%s)",
          Z2S_getZbDeviceLocalName(i), Z2S_getZbDeviceManufacturerName(i), 
          Z2S_getZbDeviceModelName(i));

        strncpy(subdeviceDetails.SoftVer, "", SUPLA_SOFTVER_MAXSIZE);

        snprintf(
          subdeviceDetails.ProductCode, SUPLA_SUBDEVICE_PRODUCT_CODE_MAXSIZE, 
          "");

        char ieee_addr_str[24];
        ieee_addr_to_str(ieee_addr_str, z2s_zb_devices_table[i].ieee_addr);
        strncpy(
          subdeviceDetails.SerialNumber, ieee_addr_str, 
          SUPLA_SUBDEVICE_SERIAL_NUMBER_MAXSIZE);

        srpc->sendSubdeviceDetails(&subdeviceDetails);
        z2s_zb_devices_table[i].user_data_flags |= 
          ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED;
        dataSend = true;
        break;
      }
    }
    return !dataSend;
  }

private:

  Supla::Protocol::SuplaSrpc *srpc = nullptr;

  uint32_t lastSendTime = 0;
};

class ZbConflictResolver : public Supla::Device::ChannelConflictResolver {

public:

  bool onChannelConflictReport(
    uint8_t *channelReport,uint8_t channelReportSize, 
    bool hasConflictInvalidType, bool hasConflictChannelMissingOnServer,
    bool hasConflictChannelMissingOnDevice) {

    if (hasConflictChannelMissingOnDevice) {
      
      log_i(
        "ConflictResolver: Channel conflict - channel missing "
        "on device. Not recoverable. Aborting...");
      
      return false;
    }
    
    if (hasConflictInvalidType) {
      
      log_i(
        "ConflictResolver: Channel conflict - channel type "
        "mismatch. Not recoverable. Aborting...");
      
      return false;
    }
    
    if (hasConflictChannelMissingOnServer) {
      
      log_i(
        "ConflictResolver: Channel conflict - channel missing "
        "on server. Trying to remove affected devices...");

      auto maxChannelNumber = Supla::RegisterDevice::getMaxChannelNumberUsed();

      uint8_t zb_device_slot = 0xFF;

      for (int i = 0; i <= maxChannelNumber; i++) {

        if ((i >= channelReportSize || channelReport[i] == 0) &&
            !Supla::RegisterDevice::isChannelNumberFree(i)) {
       
          log_i("%u, %u, %u", channelReportSize, i, channelReport[i]);
          int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(i);
          if ( channel_number_slot >= 0) {

            //Z2S_findZbDeviceTableSlot(
          //z2s_channels_table[channel_number_slot].ieee_addr)
            if ((zb_device_slot < 0xFF) && 
                (zb_device_slot != z2s_channels_table[channel_number_slot].Zb_device_id)) {

              log_e(
                "Unexpected SuplaDevice behavior - multiple subdevices conflict!!!");
              break;
            }
            zb_device_slot = 
              z2s_channels_table[channel_number_slot].Zb_device_id;

            if ( zb_device_slot == 0xFF) {

              log_e("missing ZB device id for channel #%02u", i);
              //continue;
            } 

            Z2S_removeChannel(channel_number_slot, true);
            auto element =
              Supla::Element::getElementByChannelNumber(i);
            if (element) {

              Supla::AutoLock lock(SuplaDevice.getTimerAccessMutex());
              delete element;
              element = nullptr;
              if (!Supla::Storage::IsStateStorageValid()) {
              
                Supla::Storage::WriteStateStorage();
              }
            }
          }
        }
      }
      if ((zb_device_slot < 0xFF) &&
          (Z2S_countChannelsWithZbDeviceId(zb_device_slot) == 0)) {

        log_i("all channels removed - removing device %02u", zb_device_slot);

        Z2S_removeZbDevice(zb_device_slot);
      }
    }
    return false;
  }
};

class ZbPairingManager : public Supla::Device::SubdevicePairingHandler {

 public:

  bool startPairing(
    Supla::Protocol::SuplaSrpc *srpc, 
    TCalCfg_SubdevicePairingResult *result) override {
    
    if (result != nullptr) {
      
      result->MaximumDurationSec = 180; //pairingTimeoutSec;
      if (_state == 2) { //Zigbee.isNetworkOpen()
        log_d("Pairing already in progress");
        if (linkStartTimeMs != 0) {
          result->ElapsedTimeSec = (millis() - linkStartTimeMs) / 1000;
        }
        result->PairingResult = SUPLA_CALCFG_PAIRINGRESULT_ONGOING;
        return false;
      }

      if (_state != 1) {
        log_d("Pairing not started, device is not ready");
        result->PairingResult = 
          SUPLA_CALCFG_PAIRINGRESULT_NOT_STARTED_NOT_READY;
        return false;
      }

      result->PairingResult = SUPLA_CALCFG_PAIRINGRESULT_PROCEDURE_STARTED;
    }
 
    if (_state != 1) {
      log_d("Pairing not started, device is not ready");
      return false;
    }

    setState(2);
    linkStartTimeMs = millis();
    
    if (Zigbee.isNetworkOpen()) {
      Zigbee.openNetwork(0);
      Zigbee.openNetwork(180);
    } else
      Zigbee.openNetwork(180);
    
    this->_srpc = srpc;
    return true;
    }

  void notifySrpcAboutParingEnd(int pairingResult, const char *deviceName) {
    
    if (_srpc) {
      
      TCalCfg_SubdevicePairingResult result = {};
      if (linkStartTimeMs != 0) {
        result.ElapsedTimeSec = (millis() - linkStartTimeMs) / 1000;
      }
      int len = 0;
      if (deviceName &&
          pairingResult != SUPLA_CALCFG_PAIRINGRESULT_NO_NEW_DEVICE_FOUND) {
        len = strnlen(deviceName, sizeof(result.Name) - 1);
        strncpy(result.Name, deviceName, len);
        len++;  // NameSize should have a terminating null byte inclueded
      }

      result.MaximumDurationSec = pairingTimeoutSec;
      result.NameSize = len;
      result.PairingResult = pairingResult;

      log_i("updating Cloud pairing status - %u", pairingResult);

      _srpc->sendPendingCalCfgResult(-1, SUPLA_CALCFG_RESULT_TRUE, -1,
          sizeof(result), &result);
      if ((pairingResult != SUPLA_CALCFG_PAIRINGRESULT_PROCEDURE_STARTED) &&
          (pairingResult != SUPLA_CALCFG_PAIRINGRESULT_ONGOING)) {
        _srpc->clearPendingCalCfgResult(-1);
        linkStartTimeMs = 0;
        _state = 1;
      }
    }
  }

  void setState(uint8_t state) {

    _state = state;
  }

  int getState() {

    return _state;
  }

  void setSrpc(Supla::Protocol::SuplaSrpc *srpc) {

    _srpc = srpc;
  }
  private:

  uint8_t _state = 1;
  Supla::Protocol::SuplaSrpc *_srpc = nullptr;

  uint32_t linkStartTimeMs = 0;
  uint32_t pairingTimeoutSec = 180;

};

#endif