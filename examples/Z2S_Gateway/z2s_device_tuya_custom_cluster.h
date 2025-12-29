#ifndef Z2S_DEVICE_TUYA_CUSTOM_CLUSTER_H_
#define Z2S_DEVICE_TUYA_CUSTON_CLUSTER_H_

#include "z2s_devices_table.h"

#include <Z2S_control/hvac_base_ee.h>

#define TUYA_DATA_REPORT_CMD      0x02
#define TUYA_MCU_SYNC_TIME        0x24
#define TUYA_MCU_VERSION_REQUEST  0x10                
#define TUYA_MCU_VERSION_RESPONSE 0x11            

typedef struct Tuya_read_dp_result_s {
  bool     is_success;
  uint8_t  dp_id;
  uint8_t  dp_type;
  uint16_t dp_size;
union {
  uint32_t dp_value;
  int32_t  dp_value_signed;
};
  uint8_t  dp_raw_value_8[8];
} Tuya_read_dp_result_t;

void processTuyaCustomCluster(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, 
  uint16_t payload_size, uint8_t *payload);

void processZosungCustomCluster(
  esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint8_t command_id, 
  uint16_t payload_size, uint8_t *payload);

#endif
