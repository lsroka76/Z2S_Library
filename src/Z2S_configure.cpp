#include <cstring>
#include <Arduino.h>
#include "Z2S_configure.h"

/*****************************************************************************/

int32_t Z2S_findDeviceDescIdxById(uint32_t desc_id) {

  if (desc_id == Z2S_DEVICE_DESC_NULL)
    return -1;

  int32_t low = 0;
  int32_t high = static_cast<int32_t>(Z2S_DEVICES_DESC_COUNT) - 1;

  while (low <= high) {
    int32_t mid = low + ((high - low) >> 1);
    uint16_t desc_idx = Z2S_DEVICES_DESC_SORTED_IDX[mid];
    uint32_t mid_id = Z2S_DEVICES_DESC[desc_idx].z2s_device_desc_id;

    if (mid_id == desc_id) {
      return static_cast<int32_t>(desc_idx);
    }
    if (mid_id < desc_id) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return -1;
}

/*****************************************************************************/

const z2s_device_desc_t *Z2S_getDeviceDescById(
  uint32_t desc_id, int32_t *out_desc_idx) {

  int32_t idx = Z2S_findDeviceDescIdxById(desc_id);
  if (out_desc_idx) {
    *out_desc_idx = idx;
  }
  return (idx >= 0) ? &Z2S_DEVICES_DESC[idx] : nullptr;
}

/*****************************************************************************/

bool Z2S_deviceDescHasCluster(
  const z2s_device_desc_t *device_desc, uint16_t cluster_id) {

  if (!device_desc)
    return false;

  for (uint8_t i = 0; i < device_desc->z2s_device_clusters_count; ++i) {
    if (device_desc->z2s_device_clusters[i] == cluster_id)
      return true;
  }
  return false;
}

/*****************************************************************************/

int32_t Z2S_findDeviceListIdxByUid(uint32_t device_uid) {

  if (device_uid == 0)
    return -1;

  int32_t low = 0;
  int32_t high = static_cast<int32_t>(Z2S_DEVICES_LIST_COUNT) - 1;

  while (low <= high) {
    int32_t mid = low + ((high - low) >> 1);
    uint32_t mid_uid = Z2S_DEVICES_LIST[mid].z2s_device_uid;

    if (mid_uid == device_uid) {
      return mid;
    }
    if (mid_uid < device_uid) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return -1;
}

/*****************************************************************************/

int32_t Z2S_findDeviceListIdxByManufacturerAndModel(
  const char *manufacturer_name, const char *model_name) {

  if (!manufacturer_name || !model_name)
    return -1;

  for (size_t i = 0; i < Z2S_DEVICES_LIST_COUNT; ++i) {
    if ((strcmp(manufacturer_name, Z2S_DEVICES_LIST[i].manufacturer_name) == 0) &&
        (strcmp(model_name, Z2S_DEVICES_LIST[i].model_name) == 0)) {
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}

/*****************************************************************************/

bool Z2S_resolveDeviceEndpointByIndex(
  uint32_t devices_list_idx, uint8_t endpoint_counter,
  z2s_resolved_endpoint_t &out_ep) {

  if (devices_list_idx >= Z2S_DEVICES_LIST_COUNT)
    return false;

  const auto &dev_list = Z2S_DEVICES_LIST[devices_list_idx];

  if (endpoint_counter >= dev_list.z2s_device_endpoints_count)
    return false;

  const z2s_device_endpoint_t *ep_ptr =
    (dev_list.z2s_device_flags & Z2S_DEVICE_CONFIG_FLAG_EXT_ENDPOINTS)
      ? dev_list.z2s_device_endpoints_ext
      : dev_list.z2s_device_endpoints;

  uint8_t ep_idx = endpoint_counter;

  if (dev_list.z2s_device_flags & Z2S_DEVICE_CONFIG_FLAG_MIRROR_ALL_ENDPOINTS) {
    ep_idx = 0;
  } else if (dev_list.z2s_device_flags &
             Z2S_DEVICE_CONFIG_FLAG_MIRROR_2_ENDPOINTS) {
    ep_idx = (endpoint_counter < dev_list.z2s_device_endpoints_count_m1) ? 0 : 1;
  }

  const z2s_device_endpoint_t &raw_ep = ep_ptr[ep_idx];

  if (dev_list.z2s_device_endpoints_count == 1) {
    out_ep.endpoint_id = (raw_ep.endpoint_id != 0) ? raw_ep.endpoint_id : 1;
    out_ep.desc_id = (raw_ep.z2s_device_desc_id != Z2S_DEVICE_DESC_NULL)
                       ? raw_ep.z2s_device_desc_id
                       : dev_list.z2s_device_desc_id;
  } else {
    out_ep.endpoint_id = raw_ep.endpoint_id;
    out_ep.desc_id = raw_ep.z2s_device_desc_id;
  }

  out_ep.reporting_flags = raw_ep.reporting_flags;
  out_ep.reporting_set_id = raw_ep.z2s_reporting_set_id;

  // Inherit default reporting set from Z2S_DEVICES_DESC if not overridden on endpoint
  if (out_ep.reporting_flags == 0 && out_ep.desc_id != Z2S_DEVICE_DESC_NULL) {
    const z2s_device_desc_t *desc = Z2S_getDeviceDescById(out_ep.desc_id);
    if (desc && desc->reporting_flags != 0) {
      out_ep.reporting_flags = desc->reporting_flags;
      out_ep.reporting_set_id = desc->z2s_reporting_set_id;
    }
  }

  // Apply sequential offset for mirrored endpoints
  if (dev_list.z2s_device_flags & Z2S_DEVICE_CONFIG_FLAG_MIRROR_ALL_ENDPOINTS) {
    out_ep.endpoint_id += endpoint_counter;
  } else if (dev_list.z2s_device_flags &
             Z2S_DEVICE_CONFIG_FLAG_MIRROR_2_ENDPOINTS) {
    if (endpoint_counter < dev_list.z2s_device_endpoints_count_m1) {
      out_ep.endpoint_id += endpoint_counter;
    } else {
      out_ep.endpoint_id +=
        (endpoint_counter - dev_list.z2s_device_endpoints_count_m1);
    }
  }

  return true;
}

/*****************************************************************************/

bool Z2S_resolveDeviceEndpointById(
  uint32_t devices_list_idx, uint8_t target_endpoint_id,
  z2s_resolved_endpoint_t &out_ep) {

  if (devices_list_idx >= Z2S_DEVICES_LIST_COUNT)
    return false;

  uint8_t ep_count =
    Z2S_DEVICES_LIST[devices_list_idx].z2s_device_endpoints_count;

  for (uint8_t ep_counter = 0; ep_counter < ep_count; ++ep_counter) {
    z2s_resolved_endpoint_t candidate = {};
    if (Z2S_resolveDeviceEndpointByIndex(
          devices_list_idx, ep_counter, candidate) &&
        candidate.desc_id != Z2S_DEVICE_DESC_NULL &&
        candidate.endpoint_id == target_endpoint_id) {
      out_ep = candidate;
      return true;
    }
  }
  return false;
}

/*****************************************************************************/

void Z2S_initDeviceEndpointBeforeBind(
  zbg_device_params_t *device, const z2s_device_entity_t &device_list,
  const z2s_device_desc_t &device_desc, uint8_t endpoint_counter) {

  if (!device)
    return;

  if (endpoint_counter == 0) {
    if (device_desc.z2s_device_config_flags &
        Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT) {

      log_i("sending Tuya magic packet!");

      uint16_t tuya_init_attributes[6] = {
        ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
        ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID,
        ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID,
        ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
        ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID,
        0xFFFE
      };

      zbGateway.sendAttributesRead(
        device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 6, tuya_init_attributes);
    }

    if (device_desc.z2s_device_config_flags &
        Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_SETUP) {

      log_i("Tuya setup");

      zbGateway.sendCustomClusterCmd(
        device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, TUYA_SETUP_CMD,
        ESP_ZB_ZCL_ATTR_TYPE_SET, 0, nullptr, false,
        ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 0, 0);
    }

    if (device_desc.z2s_device_config_flags &
        Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_QUERY) {

      log_i("Tuya query");

      zbGateway.sendCustomClusterCmd(
        device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_QUERY_CMD,
        ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, nullptr);
    }

    if ((device_desc.z2s_device_config_flags &
         Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_MCU_VERSION) ||
        (strcmp(device_list.model_name, "TS0601") == 0)) {

      uint8_t seq[2] = {0x00, 0x02};

      log_i("Sending TUYA_MCU_VERSION_REQUEST");

      zbGateway.sendCustomClusterCmd(
        device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_MCU_VERSION_REQUEST,
        ESP_ZB_ZCL_ATTR_TYPE_SET, 2, seq, false);
    }

    if (device_desc.z2s_device_config_flags &
        Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT) {

      log_i("LUMI magic");

      uint8_t write_mask = 0x01;
      zbGateway.sendAttributeWrite(
        device, LUMI_CUSTOM_CLUSTER, LUMI_CUSTOM_CLUSTER_MODE_ID,
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, true, 1,
        LUMI_MANUFACTURER_CODE);
    }
  }

  // IAS Zone Enrollment (CIE Address Write) via descriptor flag
  if (device_desc.z2s_device_config_flags &
      Z2S_DEVICE_DESC_CONFIG_FLAG_IAS_ZONE_ENROLL) {

    log_i("IAS ZONE sensor - clearing CIE ADDRESS");

    esp_zb_ieee_addr_t gateway_ieee_addr;
    memset(gateway_ieee_addr, 0, sizeof(esp_zb_ieee_addr_t));

    zbGateway.sendAttributeWrite(
      device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
      ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
      ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, sizeof(esp_zb_ieee_addr_t),
      &gateway_ieee_addr);

    log_i("IAS ZONE sensor - writing gateway CIE ADDRESS");

    esp_zb_get_long_address(gateway_ieee_addr);

    zbGateway.sendAttributeWrite(
      device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,
      ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
      ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, sizeof(esp_zb_ieee_addr_t),
      &gateway_ieee_addr);
  }

  if (device_desc.z2s_device_desc_id ==
      Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2) {

    uint16_t write_mask_16 = 0x000B;

    zbGateway.sendAttributeWrite(
      device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0031,
      ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 2, &write_mask_16, true, 1,
      PHILIPS_MANUFACTURER_CODE);
  }
}

/*****************************************************************************/

bool Z2S_bindDeviceEndpointClusters(
  zbg_device_params_t *device, const z2s_device_desc_t *device_desc) {

  if (!device || !device_desc)
    return false;

  for (uint8_t i = 0; i < device_desc->z2s_device_clusters_count; ++i) {
    uint16_t cluster_id = device_desc->z2s_device_clusters[i];

    if (device_desc->z2s_device_config_flags &
        Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_5) {
      zbGateway.bindDeviceCluster(device, cluster_id, 3);
    }
    zbGateway.bindDeviceCluster(device, cluster_id);
  }
  return true;
}

/*****************************************************************************/

bool Z2S_bindDeviceEndpointClustersById(
  zbg_device_params_t *device, uint32_t desc_id) {

  const z2s_device_desc_t *device_desc = Z2S_getDeviceDescById(desc_id);
  return Z2S_bindDeviceEndpointClusters(device, device_desc);
}

/*****************************************************************************/

void Z2S_configureEndpointStandardReporting(
  zbg_device_params_t *device, const z2s_resolved_endpoint_t &ep) {

  if (!device || !(ep.reporting_flags & Z2S_REPORTING_SET_FLAG_STANDARD))
    return;

  uint8_t saved_ep = device->endpoint;
  device->endpoint = ep.endpoint_id;
  bool sync_cmd = (ep.reporting_flags & Z2S_REPORTING_SET_FLAG_SYNC) != 0;

  log_i(
    "Configuring reporting set 0x%04X for endpoint 0x%02X (sync=%u)",
    ep.reporting_set_id, ep.endpoint_id, sync_cmd);

  for (size_t i = 0; i < Z2S_REPORTING_SETS_DESC_COUNT; ++i) {
    const auto &reporting_set = Z2S_REPORTING_SETS_DESC[i];

    if (reporting_set.z2s_reporting_set_id == ep.reporting_set_id) {
      log_i("reporting set matched #%u", static_cast<unsigned>(i));

      zbGateway.setClusterReportingExt(
        device, reporting_set.z2s_cluster_id,
        reporting_set.z2s_attribute_id,
        reporting_set.z2s_attribute_type,
        reporting_set.z2s_min_interval_value,
        reporting_set.z2s_max_interval_value,
        (void *)&reporting_set.z2s_delta_value, sync_cmd,
        ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1,
        reporting_set.z2s_manufacturer_code > 0 ? 1 : 0,
        reporting_set.z2s_manufacturer_code);
    }
  }

  device->endpoint = saved_ep;
}

/*****************************************************************************/

void Z2S_configureDeviceSpecificReporting(
  zbg_device_params_t *device, uint32_t device_desc_id,
  uint8_t first_endpoint_id) {

  if (!device)
    return;

  uint8_t saved_ep = device->endpoint;
  device->endpoint = first_endpoint_id;
  uint8_t write_mask = 0x00;

  switch (device_desc_id) {

    case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1:
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A:
    case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2:
    case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1:
    case Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH:
    case Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH:
    case Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH:
    case Z2S_DEVICE_DESC_RELAY_1:
    case Z2S_DEVICE_DESC_TUYA_RELAY: {

      write_mask = 0xFF;
      zbGateway.sendAttributeWrite(
        device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
        ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF,
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);

      write_mask = 0x02;
      zbGateway.sendAttributeWrite(
        device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002,
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
    } break;

    case Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB20:
    case Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB21: {

      zbGateway.sendAddGroupRequestCmd(device, 0x4E20, true);
      zbGateway.sendAddGroupRequestCmd(device, 0x4E20, false);
    } break;

    case Z2S_DEVICE_DESC_IKEA_BILRESA_BUTTON_E2489:
    case Z2S_DEVICE_DESC_IKEA_BILRESA_BUTTON_E2490: {

      zbGateway.sendAddGroupRequestCmd(device, 21658, true);
      zbGateway.sendAddGroupRequestCmd(device, 21658, false);
    } break;

    case Z2S_DEVICE_DESC_EGLO_99099: {

      zbGateway.sendAddGroupRequestCmd(device, 32778, true);
      zbGateway.sendAddGroupRequestCmd(device, 32778, false);
      zbGateway.sendAddGroupRequestCmd(device, 32779, true);
      zbGateway.sendAddGroupRequestCmd(device, 32779, false);
      zbGateway.sendAddGroupRequestCmd(device, 32780, true);
      zbGateway.sendAddGroupRequestCmd(device, 32780, false);
    } break;

    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {

      if (zbGateway.sendAttributeRead(
            device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8004, true)) {
        log_i(
          "Tuya custom attribute 0x8004 has been read id 0x%x, value 0x%x",
          zbGateway.getReadAttrLastResult()->id,
          *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
      }

      write_mask = 0x01; // Tuya switch mode, 0x00 - scenes
      zbGateway.sendAttributeWrite(
        device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8004,
        ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);

      if (zbGateway.sendAttributeRead(
            device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8004, true)) {
        log_i(
          "Tuya custom attribute 0x8004 has been read id 0x%x, value 0x%x",
          zbGateway.getReadAttrLastResult()->id,
          *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
      }
    } break;

    case Z2S_DEVICE_DESC_BOSCH_BTHRA: {

      uint32_t write_mask_32 = 1200;
      zbGateway.sendAttributeWrite(
        device, ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL, 0x0000,
        ESP_ZB_ZCL_ATTR_TYPE_U32, 4, &write_mask_32);
    } break;

    case Z2S_DEVICE_DESC_LUMI_TRV: {

      write_mask = 0;
      zbGateway.sendAttributeWrite(
        device, LUMI_CUSTOM_CLUSTER,
        LUMI_CUSTOM_CLUSTER_TRV_WINDOW_DETECTION_ID,
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, false, 1,
        LUMI_MANUFACTURER_CODE);
    } break;

    case Z2S_DEVICE_DESC_LUMI_SMOKE_DETECTOR: {

      write_mask = 0x01;
      zbGateway.sendAttributeWrite(
        device, LUMI_CUSTOM_CLUSTER, LUMI_CUSTOM_CLUSTER_LINKAGE_ALARM_ID,
        ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, true, 1,
        LUMI_MANUFACTURER_CODE);
    } break;

    default:
      break;
  }

  device->endpoint = saved_ep;
}

/*****************************************************************************/

bool Z2S_rebindAndConfigureDevice(
  Z2S_Device *z2s_device, bool rebind_clusters, bool configure_reporting) {

  if (!z2s_device || (!rebind_clusters && !configure_reporting))
    return false;

  uint32_t list_idx = z2s_device->getDevicesListIdx();
  if (list_idx >= Z2S_DEVICES_LIST_COUNT) {
    log_e("Invalid devices_list_idx (%lu) for device #%d",
          list_idx, z2s_device->getDeviceIndex());
    return false;
  }

  const auto &dev_list = Z2S_DEVICES_LIST[list_idx];

  zbg_device_params_t dev_params = {};
  memcpy(dev_params.ieee_addr, z2s_device->getDeviceIeeeAddress(),
         sizeof(esp_zb_ieee_addr_t));
  dev_params.short_addr = z2s_device->getDeviceShortAddress();
  dev_params.zb_device_id = static_cast<uint8_t>(z2s_device->getDeviceIndex());

  uint8_t first_endpoint_id = 1;
  bool first_valid_ep_found = false;

  for (uint8_t ep_counter = 0;
       ep_counter < dev_list.z2s_device_endpoints_count;
       ++ep_counter) {

    z2s_resolved_endpoint_t resolved_ep = {};
    if (!Z2S_resolveDeviceEndpointByIndex(list_idx, ep_counter, resolved_ep))
      continue;

    if (resolved_ep.desc_id == Z2S_DEVICE_DESC_NULL)
      continue;

    if (!first_valid_ep_found) {
      first_endpoint_id = resolved_ep.endpoint_id;
      first_valid_ep_found = true;
    }

    dev_params.endpoint = resolved_ep.endpoint_id;
    dev_params.model_id = resolved_ep.desc_id;

    if (rebind_clusters) {
      const z2s_device_desc_t *desc = Z2S_getDeviceDescById(resolved_ep.desc_id);
      if (desc) {
        log_i("Rebinding clusters for device 0x%04X, ep 0x%02X, desc 0x%04lX",
              dev_params.short_addr, dev_params.endpoint, resolved_ep.desc_id);
        Z2S_bindDeviceEndpointClusters(&dev_params, desc);
      }
    }

    if (configure_reporting) {
      Z2S_configureEndpointStandardReporting(&dev_params, resolved_ep);
    }
  }

  if (configure_reporting) {
    Z2S_configureDeviceSpecificReporting(
      &dev_params, dev_list.z2s_device_desc_id, first_endpoint_id);
  }

  return true;
}