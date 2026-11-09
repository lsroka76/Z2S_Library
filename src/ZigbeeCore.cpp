/* Zigbee Core Functions */

#include "ZigbeeCore.h"
#include "GeminiProtocol.h"

#if CONFIG_ZB_ENABLED || defined(USE_GEMINI_PROTOCOL)

#if !defined(USE_GEMINI_PROTOCOL_HOST)
#include "ZigbeeHandlers.cpp"
#endif

#include "Arduino.h"

#define ZB_INIT_TIMEOUT 30000  // 30 seconds

#if !defined(USE_GEMINI_PROTOCOL_HOST)
extern "C" void zb_set_ed_node_descriptor(bool power_src, bool rx_on_when_idle, bool alloc_addr);
extern "C" void zb_bdb_set_legacy_device_support(zb_uint8_t state);
extern "C" zb_ret_t zb_nvram_write_dataset(zb_nvram_dataset_types_t t);
static bool edBatteryPowered = false;
#endif

ZigbeeCore::ZigbeeCore() {
  _radio_config.radio_mode = ZB_RADIO_MODE_NATIVE;
  _host_config.host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE;
  #if !defined(USE_GEMINI_PROTOCOL_HOST)
  _zb_ep_list = esp_zb_ep_list_create();
  #else
  _zb_ep_list = nullptr;
  #endif
  _primary_channel_mask = (1 << 20);
  _open_network = 0xFF;
  _scan_status = ZB_SCAN_FAILED;
  _scan_result = nullptr;
  _started = false;
  _connected = false;
  _permit_joining = false;
  _scan_duration = 3;
  _rx_on_when_idle = true;
  if (!lock) {
    lock = xSemaphoreCreateBinary();
    if (lock == NULL) {
      log_e("Semaphore creation failed");
    }
  }
}

ZigbeeCore::~ZigbeeCore() {}

#if !defined(USE_GEMINI_PROTOCOL_HOST)
static esp_err_t zb_action_handler(
  esp_zb_core_action_callback_id_t callback_id, const void *message);
bool zb_apsde_data_indication_handler(esp_zb_apsde_data_ind_t ind);
static void zb_apsde_data_confirm_handler(esp_zb_apsde_data_confirm_t confirm);
#endif

bool ZigbeeCore::begin(esp_zb_cfg_t *role_cfg, bool erase_nvs) {
  if (!zigbeeInit(role_cfg, erase_nvs)) {
    log_e("ZigbeeCore begin failed");
    return false;
  }
  _role = (zigbee_role_t)role_cfg->esp_zb_role;
  if (xSemaphoreTake(lock, pdMS_TO_TICKS(ZB_INIT_TIMEOUT)) != pdTRUE) {
    log_e("ZigbeeCore begin timeout");
  }
  return started();
}

bool ZigbeeCore::begin(zigbee_role_t role, bool erase_nvs) {
  bool status = true;
  switch (role) {
    case ZIGBEE_COORDINATOR: {
      _role = ZIGBEE_COORDINATOR;
      esp_zb_cfg_t zb_nwk_cfg = ZIGBEE_DEFAULT_COORDINATOR_CONFIG();
      status = zigbeeInit(&zb_nwk_cfg, erase_nvs);
      break;
    }
    case ZIGBEE_ROUTER: {
      _role = ZIGBEE_ROUTER;
      esp_zb_cfg_t zb_nwk_cfg = ZIGBEE_DEFAULT_ROUTER_CONFIG();
      status = zigbeeInit(&zb_nwk_cfg, erase_nvs);
      break;
    }
    case ZIGBEE_END_DEVICE: {
      _role = ZIGBEE_END_DEVICE;
      esp_zb_cfg_t zb_nwk_cfg = ZIGBEE_DEFAULT_ED_CONFIG();
      status = zigbeeInit(&zb_nwk_cfg, erase_nvs);
      break;
    }
    default:
      log_e("Invalid Zigbee Role");
      return false;
  }
  if (!status || xSemaphoreTake(lock, pdMS_TO_TICKS(ZB_INIT_TIMEOUT)) != pdTRUE) {
    log_e("ZigbeeCore begin failed or timeout");
  }
  return started();
}

void ZigbeeCore::addEndpoint(ZigbeeEP *ep) {
  ep_objects.push_back(ep);
  #if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (ep->_ep_config.endpoint == 0 || ep->_cluster_list == nullptr) {
    log_e("Endpoint config or Cluster list is not initialized");
    return;
  }
  esp_zb_ep_list_add_ep(_zb_ep_list, ep->_cluster_list, ep->_ep_config);
  esp_zb_ep_list_add_ep(_zb_ep_list, ep->_cluster_list_2, ep->_ep_config_2);
  #endif
}

void ZigbeeCore::addGatewayEndpoint(ZigbeeEP *ep) {
  ep_objects.push_back(ep);
  #if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (ep->_ep_config.endpoint == 0 || ep->_cluster_list == nullptr) {
    log_e("Endpoint config or Cluster list is not initialized");
    return;
  }
  esp_zb_ep_list_add_gateway_ep(_zb_ep_list, ep->_cluster_list, ep->_ep_config);
  #endif
}

#if !defined(USE_GEMINI_PROTOCOL_HOST)
static void esp_zb_task(void *pvParameters) {
  esp_zb_bdb_set_scan_duration(Zigbee.getScanDuration());
  ESP_ERROR_CHECK(esp_zb_start(false));

  if (((zigbee_role_t)Zigbee.getRole() == ZIGBEE_END_DEVICE) && edBatteryPowered) {
    zb_set_ed_node_descriptor(0, Zigbee.getRxOnWhenIdle(), 1);
  }
  esp_zb_stack_main_loop();
}
#endif

bool ZigbeeCore::zigbeeInit(esp_zb_cfg_t *zb_cfg, bool erase_nvs) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  GeminiBridge::initUart();
  G_PktZbBegin pkt = {
    .channel_mask     = _primary_channel_mask,
    .role             = (uint8_t)_role,
    .erase_nvs        = (uint8_t)(erase_nvs ? 1 : 0),
    .open_network_sec = _open_network
  };
  GeminiBridge::sendFrame(G_CMD_ZB_BEGIN, &pkt, sizeof(pkt));
  return true;
#else
  esp_zb_platform_config_t platform_config = {
    .radio_config = _radio_config,
    .host_config  = _host_config,
  };

  if (esp_zb_platform_config(&platform_config) != ESP_OK) return false;
  if (esp_zb_overall_network_size_set(128) != ESP_OK) return false;
  if (esp_zb_io_buffer_size_set(128) != ESP_OK) return false;
  if (esp_zb_scheduler_queue_size_set(254) != ESP_OK) return false;
  if (esp_zb_aps_src_binding_table_size_set(16) != ESP_OK) return false;
  if (esp_zb_aps_dst_binding_table_size_set(16) != ESP_OK) return false;

  esp_zb_init(zb_cfg);

  if (!ep_objects.empty()) {
    if (esp_zb_device_register(_zb_ep_list) != ESP_OK) {
      log_e("Failed to register Zigbee EPs");
      return false;
    }
    for (std::list<ZigbeeEP *>::iterator it = ep_objects.begin(); it != ep_objects.end(); ++it) {
      if ((*it)->_power_source == ZB_POWER_SOURCE_BATTERY) {
        edBatteryPowered = true;
      }
    }
  }

  esp_zb_secur_network_min_join_lqi_set(0);
  esp_zb_raw_command_handler_register(zb_raw_cmd_handler);
  esp_zb_core_action_handler_register(zb_action_handler);
  zb_bdb_set_legacy_device_support(1);

  if (esp_zb_set_primary_network_channel_set(_primary_channel_mask) != ESP_OK) return false;
  if (esp_zb_set_secondary_network_channel_set(_primary_channel_mask) != ESP_OK) return false;

  esp_zb_aps_data_indication_handler_register(zb_apsde_data_indication_handler);
  esp_zb_aps_data_confirm_handler_register(zb_apsde_data_confirm_handler);

  if (erase_nvs) {
    esp_zb_nvram_erase_at_start(true);
  }

  xTaskCreate(esp_zb_task, "Zigbee_main", 8192, NULL, 15, &_zigbee_task_handle);
  return true;
#endif
}

void ZigbeeCore::setRadioConfig(esp_zb_radio_config_t config) { _radio_config = config; }
esp_zb_radio_config_t ZigbeeCore::getRadioConfig() { return _radio_config; }
void ZigbeeCore::setHostConfig(esp_zb_host_config_t config) { _host_config = config; }
esp_zb_host_config_t ZigbeeCore::getHostConfig() { return _host_config; }
void ZigbeeCore::setPrimaryChannelMask(uint32_t mask) { _primary_channel_mask = mask; }

void ZigbeeCore::setScanDuration(uint8_t duration) {
  if (duration < 1 || duration > 4) return;
  _scan_duration = duration;
}

void ZigbeeCore::setRebootOpenNetwork(uint8_t time) { _open_network = time; }

void ZigbeeCore::openNetwork(uint8_t time) {
  if (started()) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
    GeminiBridge::sendFrame(G_CMD_OPEN_NETWORK, &time, sizeof(uint8_t));
#else
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_bdb_open_network(time);
    esp_zb_lock_release();
#endif
  }
}

void ZigbeeCore::factoryReset() {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  GeminiBridge::sendFrame(G_CMD_FACTORY_RESET, nullptr, 0);
  esp_restart();
#else
  esp_zb_factory_reset();
#endif
}

void ZigbeeCore::setNVRAMChannelMask(uint32_t mask) {
  _primary_channel_mask = mask;
#if defined(USE_GEMINI_PROTOCOL_HOST)
  GeminiBridge::sendFrame(G_CMD_SET_CHANNEL_MASK, &mask, sizeof(uint32_t));
#else
  esp_zb_set_channel_mask(_primary_channel_mask);
  zb_nvram_write_dataset(ZB_NVRAM_COMMON_DATA);
#endif
}

#if !defined(USE_GEMINI_PROTOCOL_HOST)
static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask) {
  ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mode_mask));
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct) {
  uint32_t *p_sg_p = signal_struct->p_app_signal;
  esp_err_t err_status = signal_struct->esp_err_status;
  esp_zb_app_signal_type_t sig_type = (esp_zb_app_signal_type_t)*p_sg_p;

  switch (sig_type) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
      esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
      break;

    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
      if (err_status == ESP_OK) {
        Zigbee._is_factory_new = esp_zb_bdb_is_factory_new();
        log_i("Device started up in %s factory-reset mode", Zigbee._is_factory_new ? "" : "non");

        if (Zigbee._is_factory_new) {
          if ((zigbee_role_t)Zigbee.getRole() == ZIGBEE_COORDINATOR) {
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_FORMATION);
          } else {
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
            Zigbee._started = true;
            xSemaphoreGive(Zigbee.lock);
          }
        } else {
          Zigbee._started = true;
          xSemaphoreGive(Zigbee.lock);
          if ((zigbee_role_t)Zigbee.getRole() == ZIGBEE_COORDINATOR && Zigbee._open_network > 0) {
            esp_zb_bdb_open_network(Zigbee._open_network);
          } else {
            Zigbee._connected = true;
          }
          Zigbee.searchBindings();
        }
        } else {
        esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_INITIALIZATION, 500);
      }
      break;

    case ESP_ZB_BDB_SIGNAL_FORMATION:
      if ((zigbee_role_t)Zigbee.getRole() == ZIGBEE_COORDINATOR) {
        if (err_status == ESP_OK) {
          esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
        } else {
          esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_NETWORK_FORMATION, 1000);
        }
      }
      break;

    case ESP_ZB_BDB_SIGNAL_STEERING:
      if ((zigbee_role_t)Zigbee.getRole() == ZIGBEE_COORDINATOR) {
        Zigbee._started = true;
        xSemaphoreGive(Zigbee.lock);
      } else {
        if (err_status == ESP_OK) {
          Zigbee._connected = true;
        } else {
          esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
        }
      }
      break;

    case ESP_ZB_ZDO_SIGNAL_DEVICE_ANNCE:
      if ((zigbee_role_t)Zigbee.getRole() == ZIGBEE_COORDINATOR) {
        auto *dev_annce_params = (esp_zb_zdo_signal_device_annce_params_t *)esp_zb_app_signal_get_params(p_sg_p);
        if (dev_annce_params->device_short_addr == 0x0000) break;

        esp_zb_zdo_match_desc_req_param_t cmd_req = {};
        cmd_req.dst_nwk_addr     = dev_annce_params->device_short_addr;
        cmd_req.addr_of_interest = dev_annce_params->device_short_addr;

        for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
          if (!(*it)->bound() || (*it)->epAllowMultipleBinding()) {
            uint8_t annce_result = (*it)->zbDeviceAnnce(
              dev_annce_params->device_short_addr, dev_annce_params->ieee_addr);
            if (annce_result == DEVICE_ANNCE_RESULT_JOIN) {
              if (Zigbee.isNetworkOpen()) (*it)->findEndpoint(&cmd_req);
            } else if (annce_result == DEVICE_ANNCE_RESULT_REJOIN) {
              (*it)->zbDeviceRejoin(dev_annce_params->device_short_addr, dev_annce_params->ieee_addr);
            }
          }
        }
      }
      break;

    case ESP_ZB_ZDO_SIGNAL_DEVICE_UPDATE: {
      auto *dev_update_params = (esp_zb_zdo_signal_device_update_params_t *)esp_zb_app_signal_get_params(p_sg_p);

      static constexpr uint8_t LUMI_pattern[]   = {0x44, 0xEF, 0x54};
      static constexpr uint8_t LUMI_pattern_2[] = {0x3C, 0xC2, 0x18};
      static constexpr uint8_t LUMI_pattern_3[] = {0x8C, 0xCF, 0x04};
      static constexpr uint8_t LUMI_pattern_4[] = {0x8D, 0x15, 0x00};

      if ((memcmp(&dev_update_params->long_addr[5], LUMI_pattern,   3) == 0) ||
          (memcmp(&dev_update_params->long_addr[5], LUMI_pattern_2, 3) == 0) ||
          (memcmp(&dev_update_params->long_addr[5], LUMI_pattern_3, 3) == 0) ||
          (memcmp(&dev_update_params->long_addr[5], LUMI_pattern_4, 3) == 0)) {
        esp_zb_set_node_descriptor_manufacturer_code(0x115F);
      } else {
        esp_zb_set_node_descriptor_manufacturer_code(0x131B);
      }

      for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
        if (!(*it)->bound() || (*it)->epAllowMultipleBinding()) {
          (*it)->zbDeviceUpdate(
            dev_update_params->short_addr, dev_update_params->long_addr, dev_update_params->status);
        }
      }
    } break;

    case ESP_ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS:
      if ((zigbee_role_t)Zigbee.getRole() == ZIGBEE_COORDINATOR && err_status == ESP_OK) {
        uint8_t open_sec = *(uint8_t *)esp_zb_app_signal_get_params(p_sg_p);
        Zigbee.setNetworkOpen(open_sec > 0);
        #if defined(USE_GEMINI_PROTOCOL_NCP)
        GeminiBridge::sendFrame(G_EVT_OPEN_NETWORK, &open_sec, sizeof(uint8_t));
        #endif
        for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
          (*it)->zbOpenNetwork(open_sec);
        }
      }
      break;

    case ESP_ZB_ZDO_SIGNAL_LEAVE:
      if ((zigbee_role_t)Zigbee.getRole() != ZIGBEE_COORDINATOR) {
        Zigbee.factoryReset();
      }
      break;

    case ESP_ZB_ZDO_SIGNAL_LEAVE_INDICATION: {
      auto *dev_leave_params = (esp_zb_zdo_signal_leave_indication_params_t *)esp_zb_app_signal_get_params(p_sg_p);
      for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
        (*it)->zbDeviceLeave(
          dev_leave_params->short_addr, dev_leave_params->device_addr, dev_leave_params->rejoin);
      }
    } break;

    default:
      break;
  }
}

bool zb_apsde_data_indication_handler(esp_zb_apsde_data_ind_t ind) {
  return false;
}

static void zb_apsde_data_confirm_handler(esp_zb_apsde_data_confirm_t confirm) {}
#endif // !defined(USE_GEMINI_PROTOCOL_HOST)

void ZigbeeCore::scanCompleteCallback(
  esp_zb_zdp_status_t zdo_status, uint8_t count, 
  esp_zb_network_descriptor_t *nwk_descriptor) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    if (Zigbee._scan_result != nullptr) {
      free(Zigbee._scan_result);
    }
    Zigbee._scan_result = (esp_zb_network_descriptor_t *)malloc(count * sizeof(esp_zb_network_descriptor_t));
    if (Zigbee._scan_result) {
      memcpy(Zigbee._scan_result, nwk_descriptor, count * sizeof(esp_zb_network_descriptor_t));
    }
    Zigbee._scan_status = count;
  } else {
    Zigbee._scan_status = ZB_SCAN_FAILED;
  }
#endif
}

void ZigbeeCore::scanNetworks(u_int32_t channel_mask, u_int8_t scan_duration) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (!started()) return;
  esp_zb_zdo_active_scan_request(channel_mask, scan_duration, scanCompleteCallback);
  _scan_status = ZB_SCAN_RUNNING;
#endif
}

int16_t ZigbeeCore::scanComplete() { return _scan_status; }
zigbee_scan_result_t *ZigbeeCore::getScanResult() { return _scan_result; }

void ZigbeeCore::scanDelete() {
  if (_scan_result != nullptr) {
    free(_scan_result);
    _scan_result = nullptr;
  }
  _scan_status = ZB_SCAN_FAILED;
}

void ZigbeeCore::bindingTableCb(const esp_zb_zdo_binding_table_info_t *table_info, void *user_ctx) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  bool done = true;
  esp_zb_zdo_mgmt_bind_param_t *req = (esp_zb_zdo_mgmt_bind_param_t *)user_ctx;
  esp_zb_zdp_status_t zdo_status = (esp_zb_zdp_status_t)table_info->status;
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    if (table_info->total == 0) {
      free(req); 
      return;
    }

    esp_zb_zdo_binding_table_record_t *record = table_info->record;
    for (int i = 0; i < table_info->count; i++) {
      zb_device_params_t *device = (zb_device_params_t *)calloc(1, sizeof(zb_device_params_t));
      device->endpoint = record->dst_endp;
      if (record->dst_addr_mode == ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT || 
          record->dst_addr_mode == ESP_ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT) {
        device->short_addr = record->dst_address.addr_short;
      } else {
        memcpy(device->ieee_addr, record->dst_address.addr_long, sizeof(esp_zb_ieee_addr_t));
        device->short_addr = 0;
      }

      bool claimed = false;
      for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
        if ((*it)->getEndpoint() == record->src_endp) {
          (*it)->addBoundDevice(device, record->cluster_id, table_info->total, table_info->index);
          claimed = true;
          break;
        }
      }
      if (!claimed) {
        free(device); // Prevent memory leak if endpoint wasn't matched!
      }
      record = record->next;
    }

    if (table_info->index + table_info->count < table_info->total) {
      req->start_index = table_info->index + table_info->count;
      esp_zb_zdo_binding_table_req(req, bindingTableCb, req);
      done = false;
    }
  }

  if (done) {
    free(req);
  }
#endif
}

void ZigbeeCore::searchBindings() {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  esp_zb_zdo_mgmt_bind_param_t *mb_req = 
    (esp_zb_zdo_mgmt_bind_param_t *)malloc(sizeof(esp_zb_zdo_mgmt_bind_param_t));
  mb_req->dst_addr = esp_zb_get_short_address();
  mb_req->start_index = 0;
  esp_zb_zdo_binding_table_req(mb_req, bindingTableCb, (void *)mb_req);
#endif
}

void ZigbeeCore::stop() {
  if (started()) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
    vTaskSuspend(xTaskGetHandle("Zigbee_main"));
#endif
    _started = false;
  }
}

void ZigbeeCore::start() {
  if (!started()) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
    vTaskResume(xTaskGetHandle("Zigbee_main"));
#endif
    _started = true;
  }
}

const char *ZigbeeCore::getDeviceTypeString(esp_zb_ha_standard_devices_t deviceId) {
  switch (deviceId) {
    case ESP_ZB_HA_HOME_GATEWAY_DEVICE_ID:       return "Home Gateway";
    case ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID:     return "Remote Control";
    default:                                     return "Zigbee Device";
  }
}

ZigbeeCore Zigbee = ZigbeeCore();

#endif // CONFIG_ZB_ENABLED || defined(USE_GEMINI_PROTOCOL)