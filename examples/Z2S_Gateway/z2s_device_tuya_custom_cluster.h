#ifndef Z2S_DEVICE_TUYA_CUSTOM_CLUSTER_H_
#define Z2S_DEVICE_TUYA_CUSTOM_CLUSTER_H_

#include "z2s_devices_table.h"

#include <Z2S_control/hvac_base_ee.h>

/*****************************************************************************/

typedef struct Tuya_read_dp_result_s {
  bool     is_success;        
  uint8_t  bytes_read;        
  uint8_t  dp_id;              
  uint8_t  dp_type;           
  union {
    uint32_t dp_value;         
    int32_t  dp_value_signed;
  };
  uint16_t dp_size;            
  uint8_t  dp_raw_value_8[64]; 
} Tuya_read_dp_result_t;

/*****************************************************************************/

void processTuyaCustomCluster(
  uint8_t tsn, uint16_t short_addr, uint16_t endpoint, uint8_t command_id, 
  uint16_t payload_size, uint8_t *payload);

/*****************************************************************************/

void processZosungCustomCluster(
  uint8_t tsn, uint16_t short_addr, uint16_t endpoint, uint8_t command_id, 
  uint16_t payload_size, uint8_t *payload);

/*****************************************************************************/

char *getLearnedIrCodeBase64();

/*****************************************************************************/

void startLearnIRCode(uint16_t short_addr, uint8_t endpoint);

/*****************************************************************************/

void stopLearnIRCode(uint16_t short_addr, uint8_t endpoint);

/*****************************************************************************/

void sendLearnedIRCode(
  uint16_t short_addr, uint8_t endpoint, const char* base64_code);

/*****************************************************************************/

void cancelLearnIRCode(uint16_t short_addr, uint8_t endpoint);

/*****************************************************************************/

#endif
