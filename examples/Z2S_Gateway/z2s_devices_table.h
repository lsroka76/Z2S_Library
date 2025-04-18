#ifndef Z2S_DEVICES_TABLE_H_
#define Z2S_DEVICES_TABLE_H_

#include <ZigbeeGateway.h>
#include "z2s_devices_database.h"
#include "priv_auth_data.h"
#include "z2s_telnet_server.h"

#include <supla/tools.h>

#define Z2S_ZBDEVICESMAXCOUNT         32
#define Z2S_CHANNELMAXCOUNT           128

#define MAX_ZB_DEVICE_SUPLA_CHANNELS  16 

#define ALL_SUPLA_CHANNEL_TYPES       -1

#define ADD_Z2S_DEVICE_STATUS_OK      0x00 //device added 100%
#define ADD_Z2S_DEVICE_STATUS_DUN     0x01
#define ADD_Z2S_DEVICE_STATUS_DT_FULL 0x02 //device table full = device not added
#define ADD_Z2S_DEVICE_STATUS_DT_FWA  0x03  //device table full while adding = device added partialy
#define ADD_Z2S_DEVICE_STATUS_DAP     0x04 //device already present

#define USER_DATA_FLAG_SED_TIMEOUT    0x01
#define USER_DATA_FLAG_MSG_DISABLED   0x02
//#define USER_DATA_FLA

typedef struct z2s_legacy_device_params_s {

  bool                valid_record;
  uint32_t            model_id;
  esp_zb_ieee_addr_t  ieee_addr;
  uint8_t             endpoint;
  uint16_t            cluster_id;
  uint16_t            short_addr;
  uint8_t             Supla_channel;
  int32_t             Supla_channel_type;
  char                Supla_channel_name[30];
  uint32_t            Supla_channel_func;
  int8_t              sub_id;
} z2s_legacy_device_params_t;

typedef struct z2s_legacy_2_device_params_s {

  bool                valid_record;
  uint32_t            model_id;
  esp_zb_ieee_addr_t  ieee_addr;
  uint8_t             endpoint;
  uint16_t            cluster_id;
  uint16_t            short_addr;
  uint8_t             Supla_channel;
  uint8_t             Supla_secondary_channel;
  int32_t             Supla_channel_type;
  char                Supla_channel_name[30];
  uint32_t            Supla_channel_func;
  int8_t              sub_id;
  uint32_t            user_data_1;
  uint32_t            user_data_2;
} z2s_legacy_2_device_params_t;

typedef struct z2s_device_params_s {

  bool                valid_record;
  uint32_t            model_id;
  esp_zb_ieee_addr_t  ieee_addr;
  uint8_t             endpoint;
  uint16_t            cluster_id;
  uint16_t            short_addr;
  uint8_t             Supla_channel;
  uint8_t             Supla_secondary_channel;
  int32_t             Supla_channel_type;
  char                Supla_channel_name[30];
  uint32_t            Supla_channel_func;
  int8_t              sub_id;
  uint32_t            user_data_1;
  uint32_t            user_data_2;
  uint32_t            user_data_3;
  uint32_t            user_data_4;
  uint32_t            user_data_flags;
  uint32_t            timeout_secs;
  uint32_t            keep_alive_secs;
  uint32_t            refresh_secs;
  uint64_t            data_counter;
  uint8_t             ZB_device_id;
} z2s_device_params_t;

typedef struct z2s_legacy_zb_device_params_s {
  
  uint8_t             record_id;
  char                manufacturer_name[32];
  char                model_name[32];
  esp_zb_ieee_addr_t  ieee_addr;
  uint16_t            short_addr;
  uint8_t             endpoints_count;
  uint32_t            desc_id;
  uint32_t            device_status;
  uint8_t             power_source;
  int8_t              rssi;
  uint8_t             battery_percentage;
  int8_t              last_rssi;
  uint8_t             last_battery_percentage;
  uint32_t            last_seen_ms;
  uint32_t            keep_alive_ms;
  uint32_t            timeout_ms;
  uint32_t            user_data_flags;
  uint32_t            user_data_1;
  uint32_t            user_data_2;
  uint8_t             Supla_channels[MAX_ZB_DEVICE_SUPLA_CHANNELS];
} z2s_legacy_zb_device_params_t;


typedef struct z2s_zb_device_params_s {
  
  uint8_t             record_id;
  char                manufacturer_name[33];
  char                model_name[33];
  esp_zb_ieee_addr_t  ieee_addr;
  uint16_t            short_addr;
  uint8_t             endpoints_count;
  uint32_t            desc_id;
  uint32_t            device_status;
  uint8_t             power_source;
  int8_t              rssi;
  uint8_t             battery_percentage;
  int8_t              last_rssi;
  uint8_t             last_battery_percentage;
  uint32_t            last_seen_ms;
  uint32_t            keep_alive_ms;
  uint32_t            timeout_ms;
  uint32_t            user_data_flags;
  uint32_t            user_data_1;
  uint32_t            user_data_2;
  uint64_t            user_data_3;
  uint64_t            user_data_4;
  uint8_t             Supla_channels[MAX_ZB_DEVICE_SUPLA_CHANNELS];
} z2s_zb_device_params_t;

extern z2s_device_params_t z2s_devices_table[Z2S_CHANNELMAXCOUNT];

const static char   Z2S_DEVICES_TABLE []  PROGMEM = "Z2S_devs_table";
const static char   Z2S_DEVICES_TABLE_SIZE []  PROGMEM = "Z2S_devs_ts";

extern z2s_zb_device_params_t z2s_zb_devices_table[Z2S_ZBDEVICESMAXCOUNT];

const static char   Z2S_ZB_DEVICES_TABLE []  PROGMEM = "Z2S_zbd_table";
const static char   Z2S_ZB_DEVICES_TABLE_SIZE []  PROGMEM = "Z2S_zbd_ts";


uint32_t  Z2S_getDevicesTableSize();
uint8_t   Z2S_findFirstFreeDevicesTableSlot(uint8_t start_slot = 0);
void      Z2S_printDevicesTableSlots(bool toTelnet = false);
bool      Z2S_loadDevicesTable();
bool      Z2S_saveDevicesTable();
void      Z2S_clearDevicesTable();

uint8_t   Z2S_findFirstFreeZBDevicesTableSlot(uint8_t start_slot = 0);
uint32_t  Z2S_getZBDevicesTableSize();
bool      Z2S_saveZBDevicesTable();
bool      Z2S_loadZBDevicesTable();
void      Z2S_clearZBDevicesTable();
void      Z2S_printZBDevicesTableSlots(bool toTelnet = false);
uint8_t   Z2S_findZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr);
void      Z2S_initZBDevices(uint32_t init_ms);
void      Z2S_updateZBDeviceLastSeenMs(esp_zb_ieee_addr_t  ieee_addr, uint32_t last_seen_ms);

bool      Z2S_addZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr, uint16_t short_addr, char *manufacturer_name, char *model_name, 
                                   uint8_t endpoints_count, uint32_t desc_id, uint8_t power_source);
uint8_t    Z2S_updateZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr, uint8_t Supla_channel);

int16_t Z2S_findChannelNumberSlot(esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, int32_t channel_type, int8_t sub_id);
int16_t Z2S_findChannelNumberNextSlot(int16_t prev_slot, esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, 
                                      int32_t channel_type, int8_t sub_id);
//int32_t Z2S_findChannelType(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster);
void    Z2S_fillDevicesTableSlot(zbg_device_params_t *device, uint8_t slot, uint8_t channel, int32_t channel_type, int8_t sub_id,
                                 char *name = nullptr, uint32_t func = 0, uint8_t secondary_channel = 0xFF);

int16_t Z2S_findTableSlotByChannelNumber(uint8_t channel_id);

void    Z2S_initSuplaChannels(); 

void Z2S_onTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float temperature, signed char rssi); 
void Z2S_onHumidityReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float humidity, signed char rssi); 
void Z2S_onPressureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float pressure, signed char rssi); 
void Z2S_onIlluminanceReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t illuminance, signed char rssi);
void Z2S_onOccupancyReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t occupancy, signed char rssi);
void Z2S_onOnOffReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, bool state, signed char rssi); 
void Z2S_onRMSVoltageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t voltage, signed char rssi); 
void Z2S_onRMSCurrentReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t current, signed char rssi);
void Z2S_onRMSActivePowerReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t active_power, signed char rssi);
void Z2S_onCurrentSummationReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint64_t active_fwd_energy, signed char rssi);  
void Z2S_onCurrentLevelReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t level, signed char rssi);
void Z2S_onColorHueReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t hue, signed char rssi);
void Z2S_onColorSaturationReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t saturation, signed char rssi);
void Z2S_onBatteryPercentageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t battery_remaining);
bool Z2S_onCustomCmdReceive( esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster_id, uint8_t command_id, uint8_t buffer_size, uint8_t *buffer, signed char  rssi);
void Z2S_onCmdCustomClusterReceive( esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t command_id,
                                    uint16_t payload_size, uint8_t *payload, signed char rssi);
void Z2S_onIASzoneStatusChangeNotification(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int iaszone_status, signed char rssi);

void Z2S_onBTCBoundDevice(zbg_device_params_t *device); 
void Z2S_onBoundDevice(zbg_device_params_t *device, bool last_cluster);

void Z2S_onDataSaveRequest(uint8_t selector);

uint8_t Z2S_addZ2SDevice(zbg_device_params_t *device, int8_t sub_id = -1, char *name = nullptr, uint32_t func = 0, char *unit = nullptr);
uint8_t Z2S_addZ2SZBDevice(char *manufacturer_name, char *model_name, esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr, 
                           uint8_t endpoints, uint32_t desc_id);


void updateTimeout(uint8_t device_id, uint8_t timeout, uint8_t selector = 0, uint32_t timings_secs = 0);
void updateRGBMode(uint8_t device_id, uint8_t rgb_mode);
void updateDeviceTemperature(uint8_t device_id, int32_t temperature);

#endif