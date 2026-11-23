#ifndef Z2S_DEVICE_SALUS_CUSTOM_CLUSTER_H_
#define Z2S_DEVICE_SALUS_CUSTOM_CLUSTER_H_

#include <Arduino.h>
#include "z2s_devices_table.h"
#include "SalusIt600.h"

void processSalusIt600CustomCluster(
  uint8_t tsn, esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr,
  uint16_t endpoint, uint8_t command_id, uint16_t payload_size,
  uint8_t *payload);

#endif // Z2S_DEVICE_SALUS_CUSTOM_CLUSTER_H_