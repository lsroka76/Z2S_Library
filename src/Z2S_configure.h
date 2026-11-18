#ifndef Z2S_CONFIGURE_H_
#define Z2S_CONFIGURE_H_

#include <Arduino.h>
#include <ZigbeeGateway.h>
#include <SuplaDevice.h>
#include <supla/element.h>

#include "z2s_devices_database.h"
#include <Z2S_StorageManager.h>
#include <Z2S_common.h>

class Z2S_Device;

/*****************************************************************************/

typedef struct z2s_resolved_endpoint_s {
  uint8_t  endpoint_id;
  uint8_t  reporting_flags;
  uint16_t reporting_set_id;
  uint32_t desc_id;
} z2s_resolved_endpoint_t;

/*****************************************************************************/

int32_t Z2S_findDeviceDescIdxById(uint32_t desc_id);
const z2s_device_desc_t *Z2S_getDeviceDescById(
  uint32_t desc_id, int32_t *out_desc_idx = nullptr);

bool Z2S_deviceDescHasCluster(
  const z2s_device_desc_t *device_desc, uint16_t cluster_id);

int32_t Z2S_findDeviceListIdxByUid(uint32_t device_uid);
int32_t Z2S_findDeviceListIdxByManufacturerAndModel(
  const char *manufacturer_name, const char *model_name);

bool Z2S_resolveDeviceEndpointByIndex(
  uint32_t devices_list_idx, uint8_t endpoint_counter,
  z2s_resolved_endpoint_t &out_ep);

bool Z2S_resolveDeviceEndpointById(
  uint32_t devices_list_idx, uint8_t target_endpoint_id,
  z2s_resolved_endpoint_t &out_ep);

void Z2S_initDeviceEndpointBeforeBind(
  zbg_device_params_t *device, const z2s_device_entity_t &device_list,
  const z2s_device_desc_t &device_desc, uint8_t endpoint_counter);

bool Z2S_bindDeviceEndpointClusters(
  zbg_device_params_t *device, const z2s_device_desc_t *device_desc);

bool Z2S_bindDeviceEndpointClustersById(
  zbg_device_params_t *device, uint32_t desc_id);

void Z2S_configureEndpointStandardReporting(
  zbg_device_params_t *device, const z2s_resolved_endpoint_t &ep);

void Z2S_configureDeviceSpecificReporting(
  zbg_device_params_t *device, uint32_t device_desc_id,
  uint8_t first_endpoint_id);

bool Z2S_rebindAndConfigureDevice(
  Z2S_Device *z2s_device, bool rebind_clusters = true,
  bool configure_reporting = false);

#endif // Z2S_CONFIGURE_H_