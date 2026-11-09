#include "ZigbeeGateway.h"
#include "GeminiProtocol.h"

#if CONFIG_ZB_ENABLED || defined(USE_GEMINI_PROTOCOL)

static uint16_t s_ota_image_type = 0;
static uint16_t s_ota_manuf_code = 0;
static uint32_t s_ota_image_size = 0;
static uint32_t s_ota_image_file_version = 0;

esp_zb_zcl_ota_upgrade_server_variable_t ota_server_variable = {
  .query_jitter = OTA_UPGRADE_QUERY_JITTER,
  .current_time = OTA_UPGRADE_CURRENT_TIME,
  .file_count   = OTA_UPGRADE_IMAGE_COUNT,
};

// Initialize static members
ZigbeeGateway *ZigbeeGateway::_instance = nullptr;
findcb_userdata_t ZigbeeGateway::findcb_userdata;
volatile bool ZigbeeGateway::_last_bind_success = false;
volatile bool ZigbeeGateway::_in_binding = false;
volatile bool ZigbeeGateway::_new_device_joined = false;
volatile bool ZigbeeGateway::_active_pairing = false;
volatile uint8_t ZigbeeGateway::_binding_error_retries = 0;
query_basic_cluster_data_t ZigbeeGateway::_last_device_query;
volatile uint8_t ZigbeeGateway::_read_attr_last_tsn = 0;
volatile uint8_t ZigbeeGateway::_read_attr_last_tsn_flag = 0xFF;
volatile uint8_t ZigbeeGateway::_custom_cmd_last_tsn = 0;
volatile uint8_t ZigbeeGateway::_custom_cmd_last_tsn_flag = 0xFF;
volatile uint8_t ZigbeeGateway::_set_config_last_tsn = 0;
volatile uint8_t ZigbeeGateway::_set_config_last_tsn_flag = 0xFF;
volatile uint8_t ZigbeeGateway::_read_config_last_tsn = 0;
volatile uint8_t ZigbeeGateway::_read_config_last_tsn_flag = 0xFF;
volatile uint8_t ZigbeeGateway::_write_attr_last_tsn = 0;
volatile uint8_t ZigbeeGateway::_write_attr_last_tsn_flag = 0xFF;  
volatile uint8_t ZigbeeGateway::_write_ext_current_tsn = 0;

esp_zb_zcl_attribute_t ZigbeeGateway::_read_attr_last_result = {};
uint8_t ZigbeeGateway::_read_attr_last_result_value_buffer[256] = {};
esp_zb_zcl_read_report_config_resp_variable_t 
  ZigbeeGateway::_read_report_config_resp_variable_last_result = {};
esp_zb_zcl_status_t ZigbeeGateway::_config_report_status_last_result = {};
esp_zb_zcl_status_t ZigbeeGateway::_read_attr_status_last_result = {};
esp_zb_zcl_status_t ZigbeeGateway::_write_attr_status_last_result = {};
uint16_t ZigbeeGateway::_write_attr_attribute_id_last_result = {};
esp_zb_zcl_status_t ZigbeeGateway::_custom_cmd_status_last_result = {};
uint8_t ZigbeeGateway::_custom_cmd_resp_to_cmd_last_result = {};

#define ZB_CMD_TIMEOUT 10000

SemaphoreHandle_t ZigbeeGateway::gt_lock;

ZigbeeGateway::ZigbeeGateway(uint8_t endpoint) : ZigbeeEP(endpoint) {
  _device_id = ESP_ZB_HA_HOME_GATEWAY_DEVICE_ID; 
  _instance = this;
  
  _new_device_joined = false;
  _last_bind_success = false;

  memset(&_last_device_query, 0, sizeof(query_basic_cluster_data_t));
  
  _joined_devices.clear();
  _gateway_devices.clear();

  #if !CONFIG_DISABLE_HAL_LOCKS
  gt_lock = xSemaphoreCreateBinary();
  if (gt_lock == NULL) {
    log_e("Semaphore creation failed");
  }
  #endif

#if !defined(USE_GEMINI_PROTOCOL_HOST)
  esp_zb_configuration_tool_cfg_t gateway_cfg = ZB_DEFAULT_GATEWAY_CONFIG();

  esp_zb_on_off_cluster_cfg_t on_off_cfg = {};
  on_off_cfg.on_off = ESP_ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE;

  esp_zb_level_cluster_cfg_t level_cfg = {};
  level_cfg.current_level = ESP_ZB_ZCL_LEVEL_CONTROL_CURRENT_LEVEL_DEFAULT_VALUE;

  esp_zb_groups_cluster_cfg_t groups_cfg = {};
  groups_cfg.groups_name_support_id = ESP_ZB_ZCL_GROUPS_NAME_SUPPORT_DEFAULT_VALUE;

  esp_zb_scenes_cluster_cfg_t scenes_cfg = {}; 
  scenes_cfg.scenes_count  = ESP_ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE;         
  scenes_cfg.current_scene = ESP_ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE;         
  scenes_cfg.current_group = ESP_ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE;         
  scenes_cfg.scene_valid   = ESP_ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE;             
  scenes_cfg.name_support  = ESP_ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE;

  esp_zb_color_cluster_cfg_t color_control_cfg = {};

  _cluster_list   = esp_zb_zcl_cluster_list_create();
  _cluster_list_2 = esp_zb_zcl_cluster_list_create();

  esp_zb_attribute_list_t *basic_cluster   = esp_zb_basic_cluster_create(&(gateway_cfg.basic_cfg));
  esp_zb_attribute_list_t *basic_cluster_2 = esp_zb_basic_cluster_create(&(gateway_cfg.basic_cfg));

  esp_zb_cluster_list_add_basic_cluster(_cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_basic_cluster(_cluster_list_2, basic_cluster_2, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_power_config_cluster(_cluster_list, esp_zb_power_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_scenes_cluster(_cluster_list, esp_zb_scenes_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_groups_cluster(_cluster_list, esp_zb_groups_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_scenes_cluster(_cluster_list, esp_zb_scenes_cluster_create(&scenes_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_groups_cluster(_cluster_list, esp_zb_groups_cluster_create(&groups_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_level_cluster(_cluster_list, esp_zb_level_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_level_cluster(_cluster_list, esp_zb_level_cluster_create(&level_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_color_control_cluster(_cluster_list, esp_zb_color_control_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_color_control_cluster(_cluster_list, esp_zb_color_control_cluster_create(&color_control_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  time_t   utc_time    = 798653565;
  uint8_t  time_status = 0x02;
  int32_t  time_zone   = ESP_ZB_ZCL_TIME_TIME_ZONE_DEFAULT_VALUE;
  time_t   local_time  = 798653565;
  uint32_t dst_start   = 0;
  uint32_t dst_end     = 0;
  int32_t  dst_shift   = ESP_ZB_ZCL_TIME_DST_SHIFT_DEFAULT_VALUE;

  esp_zb_attribute_list_t *time_cluster_server = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_TIME);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_TIME_ZONE_ID, (void *)&time_zone);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_TIME_ID, (void *)&utc_time);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, (void *)&time_status);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, (void *)&local_time);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_DST_START_ID, (void *)&dst_start);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_DST_END_ID, (void *)&dst_end);
  esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_DST_SHIFT_ID, (void *)&dst_shift);
  esp_zb_cluster_list_add_time_cluster(_cluster_list, time_cluster_server, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_attribute_list_t *ota_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE);
  esp_zb_ota_cluster_add_attr(ota_cluster, ESP_ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_DATA_ID, (void *)&ota_server_variable);
  esp_zb_cluster_list_add_ota_cluster(_cluster_list, ota_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_cluster_list_add_ias_zone_cluster(_cluster_list, esp_zb_ias_zone_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_temperature_meas_cluster(_cluster_list, esp_zb_temperature_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_humidity_meas_cluster(_cluster_list, esp_zb_humidity_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_pressure_meas_cluster(_cluster_list, esp_zb_pressure_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_pm2_5_measurement_cluster(_cluster_list, esp_zb_pm2_5_measurement_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(&on_off_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_switch_config_cluster(_cluster_list, esp_zb_on_off_switch_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_electrical_meas_cluster(_cluster_list, esp_zb_electrical_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_flow_meas_cluster(_cluster_list, esp_zb_flow_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_metering_cluster(_cluster_list, esp_zb_metering_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_illuminance_meas_cluster(_cluster_list, esp_zb_illuminance_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_occupancy_sensing_cluster(_cluster_list, esp_zb_occupancy_sensing_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_thermostat_cluster(_cluster_list, esp_zb_thermostat_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_window_covering_cluster(_cluster_list, esp_zb_window_covering_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_ias_ace_cluster(_cluster_list, esp_zb_ias_ace_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0x0020), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_0), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_1), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_2), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_0), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_1), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_EF00), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_EF00), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC80), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC80), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(PHILIPS_CUSTOM_CLUSTER), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC7F), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC7F), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(IKEA_CUSTOM_CLUSTER_FC7E), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(ADEO_CUSTOM_CLUSTER), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
#endif

  _ep_config = {
    .endpoint           = _endpoint, 
    .app_profile_id     = ESP_ZB_AF_HA_PROFILE_ID, 
    .app_device_id      = ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID, 
    .app_device_version = 0
  };
  _ep_config_2 = {
    .endpoint           = (uint8_t)(_endpoint + 1), 
    .app_profile_id     = 0xC001, 
    .app_device_id      = ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID, 
    .app_device_version = 0
  };
}

void ZigbeeGateway::bindDevicesCb(esp_zb_zdp_status_t zdo_status, void *user_ctx) {
  bool unbind = (bool)user_ctx;
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) 
    log_i("Devices direct %sbinding success!", unbind ? "un" : "");
  else
    log_e("Devices direct %sbinding error (0x%02X)!", unbind ? "un" : "", zdo_status);
}

void ZigbeeGateway::bindCb(esp_zb_zdp_status_t zdo_status, void *user_ctx) {
  zbg_device_params_t *device = (zbg_device_params_t *)user_ctx;
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    log_i("ZED (0x%x), endpoint (0x%x) cluster (0x%x) has bounded to ZC", 
          device->short_addr, device->endpoint, device->cluster_id);
    _is_bound = true;
    _last_bind_success = true;
  } else {
    log_e("Binding failed (0x%x)! Device (0x%x), endpoint (0x%x), cluster (0x%x)", 
          zdo_status, device->short_addr, device->endpoint, device->cluster_id);
    if (_binding_error_retries > 0) {
      _last_bind_success = false;
      --_binding_error_retries;
    } else {
      _last_bind_success = true;
    }
  }
  _in_binding = false;
  xSemaphoreGive(gt_lock);
}

void ZigbeeGateway::find_Cb(
  esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, 
  void *user_ctx) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    zbg_device_params_t *sensor = 
      (zbg_device_params_t *)calloc(1, sizeof(zbg_device_params_t));
    if (!sensor) return;

    if (addr == 0xFFFF) {
      sensor->endpoint   = 0xFF;
      sensor->short_addr = ((findcb_userdata_t *)(user_ctx))->_device_address;
    } else {
      sensor->endpoint   = endpoint;
      sensor->short_addr = addr;
    }
    esp_zb_ieee_address_by_short(sensor->short_addr, sensor->ieee_addr);

    #if defined(USE_GEMINI_PROTOCOL_NCP)
      GeminiBridge::sendFrame(G_EVT_NEW_DEVICE_JOINED, sensor, sizeof(zbg_device_params_t));
      free(sensor);
    #else
      _new_device_joined = true;
      _instance->_joined_devices.push_back(sensor);
    #endif
  }
#endif
}

void ZigbeeGateway::Z2S_active_ep_req_cb(
  esp_zb_zdp_status_t zdo_status, uint8_t ep_count, uint8_t *ep_id_list, 
  void *user_ctx) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    uint16_t short_addr = (uint16_t)(uintptr_t)user_ctx;
    esp_zb_zdo_simple_desc_req_param_t cl_cmd_req = {};

    for (int i = 0; i < ep_count; i++) {
      cl_cmd_req.addr_of_interest = short_addr;
      cl_cmd_req.endpoint = *(ep_id_list + i);
      if (cl_cmd_req.endpoint != 0xF2) 
        esp_zb_zdo_simple_desc_req(&cl_cmd_req, Z2S_simple_desc_req_cb, user_ctx);
    }
  }
#endif
}

void ZigbeeGateway::Z2S_simple_desc_req_cb(
  esp_zb_zdp_status_t zdo_status, esp_zb_af_simple_desc_1_1_t *simple_desc, 
  void *user_ctx) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    // Fixed LoadAccessFault crash: user_ctx is a value cast to void*, not a pointer!
    uint16_t short_addr = (uint16_t)(uintptr_t)user_ctx;
    esp_zb_zcl_disc_attr_cmd_t disc_attr_cmd_req = {};

    for (int i = 0; i < simple_desc->app_input_cluster_count; i++) {
      disc_attr_cmd_req.zcl_basic_cmd.dst_endpoint = simple_desc->endpoint;
      disc_attr_cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
      disc_attr_cmd_req.zcl_basic_cmd.src_endpoint = 1;
      disc_attr_cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      disc_attr_cmd_req.cluster_id = *(simple_desc->app_cluster_list + i);
      disc_attr_cmd_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
      disc_attr_cmd_req.start_attr_id = 0;
      disc_attr_cmd_req.max_attr_number = 99;
      esp_zb_zcl_disc_attr_cmd_req(&disc_attr_cmd_req);
    }
    
    for (int i = 0; i < simple_desc->app_output_cluster_count; i++) {
      disc_attr_cmd_req.zcl_basic_cmd.dst_endpoint = simple_desc->endpoint;
      disc_attr_cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
      disc_attr_cmd_req.zcl_basic_cmd.src_endpoint = 1;
      disc_attr_cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      disc_attr_cmd_req.cluster_id = 
        *(simple_desc->app_cluster_list + simple_desc->app_input_cluster_count + i);
      disc_attr_cmd_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
      disc_attr_cmd_req.start_attr_id = 0;
      disc_attr_cmd_req.max_attr_number = 99;
      esp_zb_zcl_disc_attr_cmd_req(&disc_attr_cmd_req);
    } 
  }
#endif
}

void ZigbeeGateway::zbPrintDeviceDiscovery(zbg_device_params_t *device) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  GeminiBridge::sendFrame(G_CMD_ACTIVE_EP_DISC, &device->short_addr, sizeof(uint16_t));
#else
  esp_zb_zdo_active_ep_req_param_t ep_cmd_req = {};
  uint16_t short_addr_req = device->short_addr; 
  ep_cmd_req.addr_of_interest = short_addr_req; 
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zdo_active_ep_req(
    &ep_cmd_req, Z2S_active_ep_req_cb, (void *)(uintptr_t)short_addr_req);
  esp_zb_lock_release();
#endif
}

bool ZigbeeGateway::zbQueryDeviceBasicCluster(
  zbg_device_params_t *device, bool single_attribute, uint16_t attribute_id) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktQueryBasicReq req = {};
  req.device           = *device;
  req.single_attribute = single_attribute ? 1 : 0;
  req.attribute_id     = attribute_id;

  G_PktQueryBasicRsp rsp = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_QUERY_BASIC, &req, sizeof(req), G_RSP_QUERY_BASIC, &rsp, sizeof(rsp), 15000)) {
    return false;
  }
  memcpy(&_last_device_query, &rsp.data, sizeof(query_basic_cluster_data_t));
  return (rsp.success != 0);
#else
  esp_zb_zcl_read_attr_cmd_t read_req = {};
  read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
  memcpy(
    read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, 
    sizeof(esp_zb_ieee_addr_t));

  read_req.zcl_basic_cmd.src_endpoint = _endpoint;
  read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  read_req.clusterID = ESP_ZB_ZCL_CLUSTER_ID_BASIC;

  uint16_t attributes[2] = {
    ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, 
    ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID
  };
  if (single_attribute) attributes[0] = attribute_id;
  int8_t attribute_count = single_attribute ? 1 : 2;

  for (int attribute_number = 0; attribute_number < attribute_count; attribute_number++) {
    read_req.attr_number = 1;
    read_req.attr_field  = &attributes[attribute_number];

    xSemaphoreTake(gt_lock, 0); 
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_read_attr_cmd_req(&read_req);
    esp_zb_lock_release();

    uint32_t wait_ms = ((attributes[attribute_number] == ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID) || 
                        (attributes[attribute_number] == ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID))
                       ? 6000 : 2000;

    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(wait_ms)) != pdTRUE) {
      if (attributes[attribute_number] == ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID) {
        snprintf(_last_device_query.software_build_ID, sizeof(_last_device_query.software_build_ID), "!ERROR!");
      }
      if ((attributes[attribute_number] == ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID) || 
          (attributes[attribute_number] == ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID)) {
        return false;
      }
    }
  }
  return true;
#endif
}

void ZigbeeGateway::zbReadBasicCluster(
  esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
  esp_zb_zcl_attribute_t *attribute) {

  if ((attribute->id == ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID) &&
      (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && 
      attribute->data.value) {
    zbstring_t *zbstr = (zbstring_t *)attribute->data.value;
    size_t copy_len = (zbstr->len < sizeof(_last_device_query.zcl_manufacturer_name) - 1)
                      ? zbstr->len : (sizeof(_last_device_query.zcl_manufacturer_name) - 1);
    memcpy(_last_device_query.zcl_manufacturer_name, zbstr->data, copy_len);
    _last_device_query.zcl_manufacturer_name[copy_len] = '\0';
    xSemaphoreGive(gt_lock);
  } else if ((attribute->id == ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID) &&
             (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && 
             attribute->data.value) {
    zbstring_t *zbstr = (zbstring_t *)attribute->data.value;
    size_t copy_len = (zbstr->len < sizeof(_last_device_query.zcl_model_name) - 1)
                      ? zbstr->len : (sizeof(_last_device_query.zcl_model_name) - 1);
    memcpy(_last_device_query.zcl_model_name, zbstr->data, copy_len);
    _last_device_query.zcl_model_name[copy_len] = '\0';
    xSemaphoreGive(gt_lock);
  } else if ((attribute->id == ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID) && 
             (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && 
             attribute->data.value) {
    zbstring_t *zbstr = (zbstring_t *)attribute->data.value;
    size_t copy_len = (zbstr->len < sizeof(_last_device_query.software_build_ID) - 1)
                      ? zbstr->len : (sizeof(_last_device_query.software_build_ID) - 1);
    memcpy(_last_device_query.software_build_ID, zbstr->data, copy_len);
    _last_device_query.software_build_ID[copy_len] = '\0';
    xSemaphoreGive(gt_lock);
  } else if ((attribute->id == ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID) && 
             (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) && 
             attribute->data.value) {
    _last_device_query.zcl_version_id = *((uint8_t *)attribute->data.value);
    xSemaphoreGive(gt_lock);
  } else if ((attribute->id == ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID) &&
             (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) && 
             attribute->data.value) {
    _last_device_query.zcl_application_version_id = *((uint8_t *)attribute->data.value);
    xSemaphoreGive(gt_lock);
  } else if ((attribute->id == ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID) && 
             (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM) && 
             attribute->data.value) {
    _last_device_query.zcl_power_source_id = *((uint8_t *)attribute->data.value);
    xSemaphoreGive(gt_lock);
  } else if (attribute->id == 0xFFFE) {
    xSemaphoreGive(gt_lock);
  } else {
    #if defined(USE_GEMINI_PROTOCOL_NCP)
      GeminiBridge::forwardAttrReport(
        src_address.u.short_addr, src_endpoint, cluster_id, attribute, true, true);
    #else
      if (_on_basic_receive)
        _on_basic_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
    #endif
  }
}

void ZigbeeGateway::bindDeviceCluster(
  zbg_device_params_t *device, uint16_t cluster_id, uint8_t groupcast_flag) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktBindClusterReq req = {
    .device         = *device,
    .cluster_id     = cluster_id,
    .groupcast_flag = groupcast_flag,
    .mode           = 0
  };
  uint8_t rsp[2] = {};
  GeminiBridge::sendCommandWaitReply(
    G_CMD_BIND_CLUSTER, &req, sizeof(req), G_RSP_STATUS, rsp, sizeof(rsp), 15000);
#else
  esp_zb_zdo_bind_req_param_t bind_req = {};
  bind_req.req_dst_addr = device->short_addr;
  memcpy(bind_req.src_address, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.src_endp   = device->endpoint;
  bind_req.cluster_id = cluster_id; 
  
  if (groupcast_flag & 1) {
    bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_16_BIT_GROUP;
    bind_req.dst_address_u.addr_short = esp_zb_get_short_address();
    bind_req.dst_endp = 1;
  } else {
    bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
    esp_zb_get_long_address(bind_req.dst_address_u.addr_long);
    bind_req.dst_endp = _instance->getEndpoint(); 
  }
    
  device->cluster_id = cluster_id;
  zbg_device_params_t bind_device = *device;

  _last_bind_success = false;
  _binding_error_retries = 5;

  while (!_last_bind_success) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)&bind_device);
    esp_zb_lock_release();

    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(ZB_CMD_TIMEOUT)) != pdTRUE) {
      log_e("Semaphore timeout while binding");
      break;
    }
  }
#endif
}

void ZigbeeGateway::bindDevices(
  esp_zb_ieee_addr_t src_ieee_addr, esp_zb_ieee_addr_t dst_ieee_addr,
  uint16_t dst_short_addr, uint8_t src_endpoint, uint8_t dst_endpoint, 
  uint16_t cluster_id, bool unbind) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktBindDirectReq req = {};
  memcpy(req.src_ieee, src_ieee_addr, 8);
  memcpy(req.dst_ieee, dst_ieee_addr, 8);
  req.dst_short_addr = dst_short_addr;
  req.src_ep         = src_endpoint;
  req.dst_ep         = dst_endpoint;
  req.cluster_id     = cluster_id;
  req.unbind         = unbind ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_BIND_DIRECT, &req, sizeof(req));
#else
  esp_zb_zdo_bind_req_param_t bind_req = {};
  bind_req.req_dst_addr = dst_short_addr;
  memcpy(bind_req.src_address, src_ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.src_endp      = src_endpoint;
  bind_req.cluster_id    = cluster_id; 
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  memcpy(bind_req.dst_address_u.addr_long, dst_ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.dst_endp      = dst_endpoint; 

  esp_zb_lock_acquire(portMAX_DELAY);
  if (unbind)
    esp_zb_zdo_device_unbind_req(&bind_req, bindDevicesCb, (void *)unbind);
  else
    esp_zb_zdo_device_bind_req(&bind_req, bindDevicesCb, (void *)unbind);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::bindDeviceCluster2(
  zbg_device_params_t *device, uint16_t cluster_id) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktBindClusterReq req = {
    .device         = *device,
    .cluster_id     = cluster_id,
    .groupcast_flag = 0,
    .mode           = 1
  };
  uint8_t rsp[2] = {};
  GeminiBridge::sendCommandWaitReply(
    G_CMD_BIND_CLUSTER, &req, sizeof(req), G_RSP_STATUS, rsp, sizeof(rsp), 15000);
#else
  esp_zb_zdo_bind_req_param_t bind_req = {};
  bind_req.req_dst_addr = device->short_addr;
  memcpy(bind_req.dst_address_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.dst_endp      = device->endpoint;
  bind_req.cluster_id    = cluster_id; 
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  esp_zb_get_long_address(bind_req.src_address);
  bind_req.src_endp      = _instance->getEndpoint(); 
    
  device->cluster_id = cluster_id;
  _last_bind_success = false;
  _binding_error_retries = 5;

  while (!_last_bind_success) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)device);
    esp_zb_lock_release();

    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(ZB_CMD_TIMEOUT)) != pdTRUE) {
      log_e("Semaphore timeout while binding");
      break;
    }
  }
#endif
}

void ZigbeeGateway::unbindLocalDeviceCluster(
  zbg_device_params_t *device, uint16_t cluster_id) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktBindClusterReq req = {
    .device         = *device,
    .cluster_id     = cluster_id,
    .groupcast_flag = 0,
    .mode           = 2
  };
  uint8_t rsp[2] = {};
  GeminiBridge::sendCommandWaitReply(
    G_CMD_BIND_CLUSTER, &req, sizeof(req), G_RSP_STATUS, rsp, sizeof(rsp), 15000);
#else
  esp_zb_zdo_bind_req_param_t bind_req = {};
  bind_req.req_dst_addr = esp_zb_get_short_address();
  esp_zb_get_long_address(bind_req.src_address);
  bind_req.src_endp      = _instance->getEndpoint();
  bind_req.cluster_id    = cluster_id;
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  memcpy(bind_req.dst_address_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.dst_endp      = device->endpoint;

  device->cluster_id = cluster_id;
  _last_bind_success = false;
  _binding_error_retries = 5;

  while (!_last_bind_success) {
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zdo_device_unbind_req(&bind_req, bindCb, (void *)device);
    esp_zb_lock_release();

    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(ZB_CMD_TIMEOUT)) != pdTRUE) {
      log_e("Semaphore timeout while unbinding");
      break;
    }
  }
#endif
}

void ZigbeeGateway::zbDeviceUpdate(
  uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr, uint8_t status) {
#if defined(USE_GEMINI_PROTOCOL_NCP)
  G_PktEvtDeviceLifecycle pkt = { .short_addr = short_addr, .param = status };
  memcpy(pkt.ieee_addr, ieee_addr, 8);
  GeminiBridge::sendFrame(G_EVT_DEVICE_UPDATE, &pkt, sizeof(pkt));
#else
  if (_on_device_update)
    _on_device_update(short_addr, ieee_addr, status);
#endif
}

uint8_t ZigbeeGateway::zbDeviceAnnce(
  uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) {
#if defined(USE_GEMINI_PROTOCOL_NCP)
  G_PktEvtDeviceLifecycle pkt = { .short_addr = short_addr, .param = 0 };
  memcpy(pkt.ieee_addr, ieee_addr, 8);
  GeminiBridge::sendFrame(G_EVT_DEVICE_ANNCE, &pkt, sizeof(pkt));
  return 0; // S3 Host decides whether to send G_CMD_FIND_ENDPOINT or Rejoin!
#else
  if (_on_device_annce)
    return _on_device_annce(short_addr, ieee_addr);  
  return 0;
#endif
}

void ZigbeeGateway::zbDeviceLeave(
  uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr, uint8_t rejoin) {
#if defined(USE_GEMINI_PROTOCOL_NCP)
  G_PktEvtDeviceLifecycle pkt = { .short_addr = short_addr, .param = rejoin };
  memcpy(pkt.ieee_addr, ieee_addr, 8);
  GeminiBridge::sendFrame(G_EVT_DEVICE_LEAVE, &pkt, sizeof(pkt));
#else
  if (_on_device_leave)
    _on_device_leave(short_addr, ieee_addr, rejoin);
#endif
}

void ZigbeeGateway::zbDeviceRejoin(
  uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) {
  if (_on_device_rejoin)
    _on_device_rejoin(short_addr, ieee_addr);
}

void ZigbeeGateway::findEndpoint(esp_zb_zdo_match_desc_req_param_t *param) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  GeminiBridge::sendFrame(G_CMD_FIND_ENDPOINT, &param->addr_of_interest, sizeof(uint16_t));
#else
  if (_new_device_joined) return;
  static uint16_t cluster_list[] = {ESP_ZB_ZCL_CLUSTER_ID_BASIC};
    
  param->profile_id       = ESP_ZB_AF_HA_PROFILE_ID;
  param->num_in_clusters  = 1;
  param->num_out_clusters = 0;
  param->cluster_list     = cluster_list;
  findcb_userdata._endpoint       = _endpoint;
  findcb_userdata._cluster_id     = ESP_ZB_ZCL_CLUSTER_ID_BASIC;
  findcb_userdata._device_address = param->addr_of_interest;
  
  esp_zb_zdo_match_cluster(param, find_Cb, &findcb_userdata);
#endif
}

void ZigbeeGateway::printGatewayDevices() {
  log_i("gateway devices:");
  for (const auto &device : _gateway_devices) {
    log_i("Device on endpoint %d, short address: 0x%x", device->endpoint, device->short_addr);
  }
}

void ZigbeeGateway::printJoinedDevices() {
  log_i("joined devices:");
  for (const auto &device : _joined_devices) {
    log_i("Device on endpoint %d, short address: 0x%x", device->endpoint, device->short_addr);
  }
}

void ZigbeeGateway::zbProcessAttributeReporting(
  esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id,
  const esp_zb_zcl_attribute_t *attribute, bool reporting) {

#if defined(USE_GEMINI_PROTOCOL_NCP)
  GeminiBridge::forwardAttrReport(
    src_address.u.short_addr, src_endpoint, cluster_id, attribute, reporting);
  return;
#else
  switch (cluster_id) {
    case ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT:
      if (_on_temperature_receive)
        _on_temperature_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT:
      if (_on_humidity_receive)
        _on_humidity_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT:
      if (_on_pressure_receive)
        _on_pressure_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT:
    case ESP_ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT:
    case ZIBI_CUSTOM_CLUSTER_ID_CARBON_MONOXIDE_MEASUREMENT:
      if (_on_concentration_receive)
        _on_concentration_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT:
    case TUYA_PRIVATE_CLUSTER_2:
      if (_on_illuminance_receive)
        _on_illuminance_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT:
      if (_on_flow_receive)
        _on_flow_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING:
      if (_on_occupancy_receive)
        _on_occupancy_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_ON_OFF:
      if (_on_on_off_receive)
        _on_on_off_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_TIME:
      if (_on_time_receive)
        _on_time_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT:
      if (_on_electrical_measurement_receive)
        _on_electrical_measurement_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT:
    case ESP_ZB_ZCL_CLUSTER_ID_MULTI_OUTPUT:
      if (_on_multistate_input_receive)
        _on_multistate_input_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute, reporting);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT:
    case ESP_ZB_ZCL_CLUSTER_ID_BINARY_OUTPUT:
      if (_on_binary_input_receive)
        _on_binary_input_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT:
    case ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT:
      if (_on_analog_input_receive)
        _on_analog_input_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute, reporting);
      break;

    case LUMI_CUSTOM_CLUSTER:
      if (_on_lumi_custom_cluster_receive)
        _on_lumi_custom_cluster_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case IKEA_CUSTOM_CLUSTER_FC7E:
      if (_on_ikea_custom_cluster_receive)
        _on_ikea_custom_cluster_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_METERING:
      if (_on_metering_receive)
        _on_metering_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL:
      if (_on_current_level_receive)
        _on_current_level_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL:
      if (_on_color_control_receive)
        _on_color_control_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG:
      if (_on_battery_receive)
        _on_battery_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE:
      if (_on_IAS_zone_status_change_notification)
        _on_IAS_zone_status_change_notification(src_address.u.short_addr, src_endpoint, cluster_id, nullptr, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT:
      if (_on_thermostat_receive)
        _on_thermostat_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG:
      if (_on_thermostat_ui_receive)
        _on_thermostat_ui_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case SONOFF_CUSTOM_CLUSTER:
    case SONOFF_CUSTOM_CLUSTER_2:
      if (_on_sonoff_custom_cluster_receive)
        _on_sonoff_custom_cluster_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_DOOR_LOCK:
      if (_on_door_lock_receive)
        _on_door_lock_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING:
      if (_on_window_covering_receive)
        _on_window_covering_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    case SHELLY_CUSTOM_CLUSTER_ID_WS90_WIND:
    case SHELLY_CUSTOM_CLUSTER_ID_WS90_UV:
    case SHELLY_CUSTOM_CLUSTER_ID_WS90_RAIN:
    case HEIMAN_CUSTOM_CLUSTER:
      if (_on_fcxx_custom_cluster_receive)
        _on_fcxx_custom_cluster_receive(src_address.u.short_addr, src_endpoint, cluster_id, attribute);
      break;

    default:
      log_i("Unknown report from 0x%04X, ep 0x%02X, cluster 0x%04X, attr 0x%04X",
            src_address.u.short_addr, src_endpoint, cluster_id, attribute->id);
      break;
  }
#endif
}

void ZigbeeGateway::zbAttributeReporting(
  esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
  const esp_zb_zcl_attribute_t *attribute) {
  zbProcessAttributeReporting(src_address, src_endpoint, cluster_id, attribute, true);
}

void ZigbeeGateway::zbReadAttrResponse(
  uint8_t tsn, int8_t rssi, esp_zb_zcl_addr_t src_address, 
  uint16_t src_endpoint, uint16_t cluster_id, esp_zb_zcl_status_t status, 
  const esp_zb_zcl_attribute_t *attribute) {
  
  if ((_read_attr_last_tsn_flag == ZCL_CMD_TSN_SYNC) && (tsn == _read_attr_last_tsn)) {
    #if defined(USE_GEMINI_PROTOCOL_NCP)
      uint8_t rssi_pkt[3];
      memcpy(rssi_pkt, &src_address.u.short_addr, 2);
      rssi_pkt[2] = (uint8_t)rssi;
      GeminiBridge::sendFrame(G_EVT_UPDATE_RSSI, rssi_pkt, sizeof(rssi_pkt));
    #else
      if (_on_update_device_last_rssi)
        _on_update_device_last_rssi(src_address.u.short_addr, rssi);
    #endif

    _read_attr_status_last_result = status;
    memcpy(&_read_attr_last_result, attribute, sizeof(const esp_zb_zcl_attribute_t));

    // Safe bounds-checked copy into 256-byte static buffer
    if (attribute->data.value && attribute->data.size > 0) {
      size_t copy_len = (attribute->data.size < sizeof(_read_attr_last_result_value_buffer))
                        ? attribute->data.size : sizeof(_read_attr_last_result_value_buffer);
      memcpy(_read_attr_last_result_value_buffer, attribute->data.value, copy_len);
      _read_attr_last_result.data.size = copy_len;
    } else {
      _read_attr_last_result.data.size = 0;
    }
    _read_attr_last_result.data.value = _read_attr_last_result_value_buffer;
    
    _read_attr_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  } else {
    if (status == ESP_ZB_ZCL_STATUS_SUCCESS) {
      #if defined(USE_GEMINI_PROTOCOL_NCP)
        GeminiBridge::forwardAttrReport(
          src_address.u.short_addr, src_endpoint, cluster_id, attribute, false, false, rssi);
      #else
        if (_on_update_device_last_rssi)
          _on_update_device_last_rssi(src_address.u.short_addr, rssi);
        zbProcessAttributeReporting(src_address, src_endpoint, cluster_id, attribute, false);
      #endif
    }
  }
}

void ZigbeeGateway::zbWriteAttrResponse(
  uint8_t tsn, esp_zb_zcl_status_t status, uint16_t attribute_id) {
  if ((_write_attr_last_tsn_flag == ZCL_CMD_TSN_SYNC) && (tsn == _write_attr_last_tsn)) {
    _write_attr_status_last_result = status;
    _write_attr_attribute_id_last_result = attribute_id;
    _write_attr_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  }
}

void ZigbeeGateway::zbIASZoneEnrollRequest(
  const esp_zb_zcl_ias_zone_enroll_request_message_t *message) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  esp_zb_zcl_ias_zone_enroll_response_cmd_t enroll_resp_req = {};
  enroll_resp_req.zcl_basic_cmd.dst_endpoint = message->info.src_endpoint;
  enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_short = message->info.src_address.u.short_addr;
  enroll_resp_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  enroll_resp_req.zcl_basic_cmd.src_endpoint = _endpoint;
  enroll_resp_req.enroll_rsp_code = ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS;
  enroll_resp_req.zone_id = 1;
  esp_zb_zcl_ias_zone_enroll_cmd_resp(&enroll_resp_req);
#endif
}

void ZigbeeGateway::zbIASZoneStatusChangeNotification(
  const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {
#if defined(USE_GEMINI_PROTOCOL_NCP)
  G_PktEvtIasStatus pkt = {
    .short_addr      = message->info.src_address.u.short_addr,
    .src_endpoint    = message->info.src_endpoint,
    .cluster_id      = message->info.cluster,
    .zone_status     = message->zone_status,
    .extended_status = message->extended_status,
    .zone_id         = message->zone_id,
    .delay           = message->delay,
    .rssi            = message->info.header.rssi
  };
  GeminiBridge::sendFrame(G_EVT_IAS_STATUS_CHANGE, &pkt, sizeof(pkt));
#else
  if (_on_IAS_zone_status_change_notification)
    _on_IAS_zone_status_change_notification(
      message->info.src_address.u.short_addr, message->info.src_endpoint,
      message->info.cluster, message, nullptr);
#endif
}

void ZigbeeGateway::zbCmdDiscAttrResponse(
  esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
  const esp_zb_zcl_disc_attr_variable_t *variable) {
  if (variable) {
    log_i("Attr Disc - addr 0x%x, ep 0x%x, cl 0x%x, attr 0x%x, type 0x%x",
          src_address.u.short_addr, src_endpoint, cluster_id, variable->attr_id, variable->data_type);
  }
}

void ZigbeeGateway::addBoundDevice(
  zb_device_params_t *device, uint16_t cluster_id, uint8_t count, uint8_t position) {
  zbg_device_params_t *zbg_device = (zbg_device_params_t *)calloc(1, sizeof(zbg_device_params_t));
  if (!zbg_device) {
    free(device);
    return;
  }
  zbg_device->short_addr = device->short_addr;
  memcpy(zbg_device->ieee_addr, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  zbg_device->cluster_id = cluster_id;
  zbg_device->endpoint   = device->endpoint;
  free(device);

#if !defined(USE_GEMINI_PROTOCOL_HOST)
  if (zbg_device->short_addr == 0) 
    zbg_device->short_addr = esp_zb_address_short_by_ieee(zbg_device->ieee_addr);
#endif
  zbg_device->model_id = 0x0000;
  _gateway_devices.push_back(zbg_device);
}

void ZigbeeGateway::clearLocalBindings() {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  GeminiBridge::sendFrame(G_CMD_CLEAR_LOCAL_BINDINGS, nullptr, 0);
#else
  for (auto *bound_device : _gateway_devices) {
    unbindLocalDeviceCluster(bound_device, bound_device->cluster_id);
  }
#endif
}

bool ZigbeeGateway::isDeviceBound(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) {
  zbg_device_params_t zbg_device = {};
  zbg_device.short_addr = short_addr;
  memcpy(zbg_device.ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t));
  return (_on_btc_bound_device && _on_btc_bound_device(&zbg_device, 1, 1));
}

bool ZigbeeGateway::setClusterReporting(
  zbg_device_params_t *device, uint16_t cluster_id, 
  uint16_t attribute_id, uint8_t attribute_type, uint16_t min_interval, 
  uint16_t max_interval, uint16_t delta, bool ack, uint8_t direction,
  uint8_t disable_default_response, uint8_t manuf_specific, uint16_t manuf_code) {
  int16_t report_change = delta;
  return setClusterReportingExt(
    device, cluster_id, attribute_id, attribute_type, min_interval, max_interval,
    &report_change, ack, direction, disable_default_response, manuf_specific, manuf_code);
}

bool ZigbeeGateway::setClusterReportingExt(
  zbg_device_params_t *device, uint16_t cluster_id, uint16_t attribute_id,
  uint8_t attribute_type, uint16_t min_interval, uint16_t max_interval, 
  void *delta, bool ack, uint8_t direction, uint8_t disable_default_response, 
  uint8_t manuf_specific, uint16_t manuf_code) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktConfigReportReq req = {};
  req.device           = *device;
  req.cluster_id       = cluster_id;
  req.attribute_id     = attribute_id;
  req.attribute_type   = attribute_type;
  req.min_interval     = min_interval;
  req.max_interval     = max_interval;
  if (delta) memcpy(req.delta_bytes, delta, 8);
  req.ack              = ack ? 1 : 0;
  req.direction        = direction;
  req.dis_default_resp = disable_default_response;
  req.manuf_specific   = manuf_specific;
  req.manuf_code       = manuf_code;

  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_CONFIG_REPORT, &req, sizeof(req));
    return false;
  }
  uint8_t rsp[2] = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_CONFIG_REPORT, &req, sizeof(req), G_RSP_STATUS, rsp, sizeof(rsp), 2500)) {
    return false;
  }
  _config_report_status_last_result = (esp_zb_zcl_status_t)rsp[1];
  return (rsp[0] != 0);
#else
  esp_zb_zcl_config_report_cmd_t report_cmd = {};
  if (device->short_addr != 0) {
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  
  report_cmd.zcl_basic_cmd.dst_endpoint = device->endpoint;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint;
  report_cmd.clusterID = cluster_id;

  esp_zb_zcl_config_report_record_t records[1] = {};
  records[0].direction         = ESP_ZB_ZCL_REPORT_DIRECTION_SEND;
  records[0].attributeID       = attribute_id;
  records[0].attrType          = attribute_type;
  records[0].min_interval      = min_interval;
  records[0].max_interval      = max_interval;
  records[0].reportable_change = delta;

  report_cmd.record_number    = 1;
  report_cmd.record_field     = &records[0];
  report_cmd.manuf_specific   = manuf_specific;
  report_cmd.dis_default_resp = disable_default_response;
  report_cmd.direction        = direction;
  report_cmd.manuf_code       = manuf_code;

  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  _set_config_last_tsn_flag = ack ? ZCL_CMD_TSN_SYNC : ZCL_CMD_TSN_ASYNC;
  _set_config_last_tsn = esp_zb_zcl_config_report_cmd_req(&report_cmd);
  esp_zb_lock_release();

  if (ack) {
    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      _set_config_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
      return false;
    }
    return true;
  }
  return false;
#endif
}

void ZigbeeGateway::readClusterReportCmd(
  zbg_device_params_t *device, uint16_t cluster_id, uint16_t attribute_id, 
  bool ack, uint8_t direction, uint8_t disable_default_response, 
  uint8_t manuf_specific, uint16_t manuf_code) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktConfigReportReq req = {};
  req.device           = *device;
  req.cluster_id       = cluster_id;
  req.attribute_id     = attribute_id;
  req.ack              = ack ? 1 : 0;
  req.direction        = direction;
  req.dis_default_resp = disable_default_response;
  req.manuf_specific   = manuf_specific;
  req.manuf_code       = manuf_code;
  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_READ_REPORT_CMD, &req, sizeof(req));
  } else {
    uint8_t rsp[2] = {};
    GeminiBridge::sendCommandWaitReply(
      G_CMD_READ_REPORT_CMD, &req, sizeof(req), G_RSP_STATUS, rsp, sizeof(rsp), 10000);
  }
#else
  esp_zb_zcl_report_attr_cmd_t report_cmd = {};
  if (device->short_addr != 0) {
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  report_cmd.zcl_basic_cmd.dst_endpoint = device->endpoint;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint;
  report_cmd.clusterID        = cluster_id;
  report_cmd.attributeID      = attribute_id;
  report_cmd.manuf_specific   = manuf_specific;
  report_cmd.dis_default_resp = disable_default_response;
  report_cmd.direction        = direction;
  report_cmd.manuf_code       = manuf_code;

  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_report_attr_cmd_req(&report_cmd);
  esp_zb_lock_release();

  if (ack) {
    xSemaphoreTake(gt_lock, pdMS_TO_TICKS(ZB_CMD_TIMEOUT));
  }
#endif
}

bool ZigbeeGateway::readClusterReportCfgCmd(
  zbg_device_params_t *device, uint16_t cluster_id, uint16_t attribute_id, 
  bool ack, uint8_t direction, uint8_t disable_default_response, 
  uint8_t manuf_specific, uint16_t manuf_code) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktConfigReportReq req = {};
  req.device           = *device;
  req.cluster_id       = cluster_id;
  req.attribute_id     = attribute_id;
  req.ack              = ack ? 1 : 0;
  req.direction        = direction;
  req.dis_default_resp = disable_default_response;
  req.manuf_specific   = manuf_specific;
  req.manuf_code       = manuf_code;
  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_READ_REPORT_CFG, &req, sizeof(req));
    return false;
  }
  G_PktReadReportCfgRsp rsp = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_READ_REPORT_CFG, &req, sizeof(req), G_RSP_READ_REPORT_CFG, &rsp, sizeof(rsp), 2500)) {
    return false;
  }
  _read_report_config_resp_variable_last_result = rsp.variable;
  return (rsp.success != 0);
#else
  esp_zb_zcl_read_report_config_cmd_t report_cmd = {};
  if (device->short_addr != 0) {
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
   
  report_cmd.zcl_basic_cmd.dst_endpoint = device->endpoint;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint;
  report_cmd.clusterID        = cluster_id;
  report_cmd.manuf_specific   = manuf_specific;
  report_cmd.dis_default_resp = disable_default_response;
  report_cmd.direction        = direction;
  report_cmd.manuf_code       = manuf_code;

  esp_zb_zcl_attribute_record_t records[1] = {};
  report_cmd.record_field  = &records[0];
  report_cmd.record_number = 1;
  records[0].report_direction = 0x0;
  records[0].attributeID      = attribute_id;

  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  _read_config_last_tsn_flag = ack ? ZCL_CMD_TSN_SYNC : ZCL_CMD_TSN_ASYNC;
  _read_config_last_tsn = esp_zb_zcl_read_report_config_cmd_req(&report_cmd);
  esp_zb_lock_release();

  if (ack) {
    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      _read_config_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
      return false;
    }
    return true;
  }
  return false;
#endif
}

void ZigbeeGateway::zbConfigReportResponse(
  uint8_t tsn, esp_zb_zcl_addr_t src_address, uint16_t src_endpoint,
  uint16_t cluster_id, esp_zb_zcl_status_t status, uint8_t direction, 
  uint16_t attribute_id) {
  if ((_set_config_last_tsn_flag == ZCL_CMD_TSN_SYNC) && (tsn == _set_config_last_tsn)) {
    _config_report_status_last_result = status;
    _set_config_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  }
}

void ZigbeeGateway::zbReadReportConfigResponse(
  const esp_zb_zcl_cmd_read_report_config_resp_message_t *message) {
  uint8_t tsn = message->info.header.tsn;
  if ((_read_config_last_tsn_flag == ZCL_CMD_TSN_SYNC) && (tsn == _read_config_last_tsn)) {
    if (message->variables) {
      memcpy(
        &_read_report_config_resp_variable_last_result, message->variables, 
        sizeof(esp_zb_zcl_read_report_config_resp_variable_t));
    }
    _read_config_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  }
}

bool ZigbeeGateway::sendAttributeRead(
  zbg_device_params_t *device, uint16_t cluster_id, uint16_t attribute_id, 
  bool ack, uint8_t direction, uint8_t disable_default_response, 
  uint8_t manuf_specific, uint16_t manuf_code, uint8_t src_endpoint) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktReadAttrReq req = {
    .short_addr       = device->short_addr,
    .src_endpoint     = src_endpoint,
    .dst_endpoint     = device->endpoint,
    .cluster_id       = cluster_id,
    .attribute_id     = attribute_id,
    .ack              = (uint8_t)(ack ? 1 : 0),
    .direction        = direction,
    .dis_default_resp = disable_default_response,
    .manuf_specific   = manuf_specific,
    .manuf_code       = manuf_code
  };
  memcpy(req.ieee_addr, device->ieee_addr, 8);

  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_READ_ATTR, &req, sizeof(req));
    return false;
  }

  G_PktReadAttrRsp rsp = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_READ_ATTR, &req, sizeof(req), G_RSP_READ_ATTR, &rsp, sizeof(rsp), 2500)) {
    return false;
  }
  _read_attr_status_last_result     = (esp_zb_zcl_status_t)rsp.status;
  _read_attr_last_result.id         = rsp.attr_id;
  _read_attr_last_result.data.type  = (esp_zb_zcl_attr_type_t)rsp.attr_type;
  _read_attr_last_result.data.size  = (rsp.data_size <= sizeof(_read_attr_last_result_value_buffer))
                                      ? rsp.data_size : sizeof(_read_attr_last_result_value_buffer);
  memcpy(_read_attr_last_result_value_buffer, rsp.data, _read_attr_last_result.data.size);
  _read_attr_last_result.data.value = _read_attr_last_result_value_buffer;
  return (rsp.success != 0);
#else
  esp_zb_zcl_read_attr_cmd_t read_req = {};
  if (device->short_addr != 0) {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }

  read_req.zcl_basic_cmd.src_endpoint = src_endpoint;
  read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  read_req.clusterID = cluster_id;

  uint16_t attributes[1] = {attribute_id};
  read_req.attr_number      = 1; 
  read_req.attr_field       = &attributes[0];
  read_req.direction        = direction;
  read_req.manuf_specific   = manuf_specific;
  read_req.dis_default_resp = disable_default_response;
  read_req.manuf_code       = manuf_code;

  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  _read_attr_last_tsn_flag = ack ? ZCL_CMD_TSN_SYNC : ZCL_CMD_TSN_ASYNC;
  _read_attr_last_tsn = esp_zb_zcl_read_attr_cmd_req(&read_req);
  esp_zb_lock_release();

  if (ack) {
    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      _read_attr_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
      return false;
    }
    return true;
  }
  return false;
#endif
}

bool ZigbeeGateway::sendAttributeRead(
  uint16_t short_addr, uint8_t dst_endpoint, uint16_t cluster_id, 
  uint16_t attribute_id, bool ack, uint8_t direction,
  uint8_t disable_default_response, uint8_t manuf_specific, 
  uint16_t manuf_code, uint8_t src_endpoint) {
  if (short_addr == 0) {
    log_e("invalid short address!");
    return false;
  }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  return sendAttributeRead(
    &dev, cluster_id, attribute_id, ack, direction,
    disable_default_response, manuf_specific, manuf_code, src_endpoint);
}

void ZigbeeGateway::sendAttributesRead(
  zbg_device_params_t *device, uint16_t cluster_id, uint8_t attr_number, 
  uint16_t *attribute_ids) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktReadAttrMultReq req = {};
  req.short_addr  = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cluster_id   = cluster_id;
  req.attr_number  = (attr_number <= 32) ? attr_number : 32;
  memcpy(req.attr_ids, attribute_ids, req.attr_number * sizeof(uint16_t));
  GeminiBridge::sendFrame(G_CMD_READ_ATTR_MULT, &req, sizeof(req));
#else
  esp_zb_zcl_read_attr_cmd_t read_req = {};
  if (device->short_addr != 0) {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  read_req.zcl_basic_cmd.src_endpoint = _endpoint;
  read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  read_req.clusterID        = cluster_id;
  read_req.attr_number      = attr_number;
  read_req.attr_field       = attribute_ids;
  read_req.direction        = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  read_req.manuf_specific   = 0;
  read_req.dis_default_resp = 1;

  esp_zb_lock_acquire(portMAX_DELAY);
  _read_attr_last_tsn_flag = ZCL_CMD_TSN_ASYNC;
  _read_attr_last_tsn = esp_zb_zcl_read_attr_cmd_req(&read_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendAttributesRead(
  uint16_t short_addr, uint8_t dst_endpoint, uint16_t cluster_id, 
  uint8_t attr_number, uint16_t *attribute_ids) {
  if (short_addr == 0) {
    log_e("invalid short address!");
    return;
  }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendAttributesRead(&dev, cluster_id, attr_number, attribute_ids);
}

bool ZigbeeGateway::sendAttributeWrite(
  zbg_device_params_t *device, uint16_t cluster_id, uint16_t attribute_id, 
  esp_zb_zcl_attr_type_t attribute_type, uint16_t attribute_size, 
  void *attribute_value, bool ack, uint8_t manuf_specific, 
  uint16_t manuf_code, bool disable_default_response, uint8_t src_endpoint) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktWriteAttrReq req = {};
  req.short_addr       = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.src_endpoint     = src_endpoint;
  req.dst_endpoint     = device->endpoint;
  req.cluster_id       = cluster_id;
  req.attribute_id     = attribute_id;
  req.attribute_type   = (uint8_t)attribute_type;
  req.attribute_size   = (attribute_size <= sizeof(req.value)) ? attribute_size : sizeof(req.value);
  req.ack              = ack ? 1 : 0;
  req.manuf_specific   = manuf_specific;
  req.manuf_code       = manuf_code;
  req.dis_default_resp = disable_default_response ? 1 : 0;
  req.is_ext           = 0;
  if (attribute_value && req.attribute_size > 0) {
    memcpy(req.value, attribute_value, req.attribute_size);
  }
  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_WRITE_ATTR, &req, sizeof(req));
    return false;
  }
  G_PktWriteAttrRsp rsp = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_WRITE_ATTR, &req, sizeof(req), G_RSP_WRITE_ATTR, &rsp, sizeof(rsp), 2500)) {
    return false;
  }
  _write_attr_status_last_result       = (esp_zb_zcl_status_t)rsp.status;
  _write_attr_attribute_id_last_result = rsp.attr_id;
  return (rsp.success != 0);
#else
  esp_zb_zcl_write_attr_cmd_t write_req = {};
  esp_zb_zcl_attribute_t attribute_field[1] = {};

  if (device->short_addr != 0) {
    write_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    write_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    write_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(write_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }

  write_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  write_req.zcl_basic_cmd.src_endpoint = src_endpoint;
  write_req.clusterID   = cluster_id;
  write_req.attr_number = 1;
  write_req.attr_field  = &attribute_field[0];

  attribute_field[0].id         = attribute_id;
  attribute_field[0].data.type  = attribute_type;
  attribute_field[0].data.size  = attribute_size;
  attribute_field[0].data.value = attribute_value;

  write_req.manuf_specific   = manuf_specific;
  write_req.dis_default_resp = disable_default_response;
  write_req.direction        = 0;
  write_req.manuf_code       = manuf_code;

  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  _write_attr_last_tsn_flag = ack ? ZCL_CMD_TSN_SYNC : ZCL_CMD_TSN_ASYNC;
  _write_attr_last_tsn = esp_zb_zcl_write_attr_cmd_req(&write_req);
  esp_zb_lock_release();

  if (ack) {
    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      _write_attr_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
      return false;
    }
    return true;
  }
  return false;
#endif
}

bool ZigbeeGateway::sendAttributeWrite(
  uint16_t short_addr, uint8_t dst_endpoint, uint16_t cluster_id, 
  uint16_t attribute_id, esp_zb_zcl_attr_type_t attribute_type, 
  uint16_t attribute_size, void *attribute_value, bool ack, 
  uint8_t manuf_specific, uint16_t manuf_code, bool disable_default_response,
  uint8_t src_endpoint) {
  if (short_addr == 0) {
    log_e("invalid short address!");
    return false;
  }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  return sendAttributeWrite(
    &dev, cluster_id, attribute_id, attribute_type, attribute_size,
    attribute_value, ack, manuf_specific, manuf_code, disable_default_response, src_endpoint);
}

bool ZigbeeGateway::sendAttributeWriteExt(
  zbg_device_params_t *device, uint16_t cluster_id, uint16_t attribute_id, 
  esp_zb_zcl_attr_type_t attribute_type, uint16_t attribute_size, 
  void *attribute_value, bool ack, uint8_t manuf_specific, 
  uint16_t manuf_code, bool disable_default_response, uint16_t profile_id) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktWriteAttrReq req = {};
  req.short_addr       = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint     = device->endpoint;
  req.cluster_id       = cluster_id;
  req.attribute_id     = attribute_id;
  req.attribute_type   = (uint8_t)attribute_type;
  req.attribute_size   = (attribute_size <= sizeof(req.value)) ? attribute_size : sizeof(req.value);
  req.ack              = ack ? 1 : 0;
  req.manuf_specific   = manuf_specific;
  req.manuf_code       = manuf_code;
  req.dis_default_resp = disable_default_response ? 1 : 0;
  req.is_ext           = 1;
  req.profile_id       = profile_id;
  if (attribute_value && req.attribute_size > 0) {
    memcpy(req.value, attribute_value, req.attribute_size);
  }
  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_WRITE_ATTR, &req, sizeof(req));
    return false;
  }
  G_PktWriteAttrRsp rsp = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_WRITE_ATTR, &req, sizeof(req), G_RSP_WRITE_ATTR, &rsp, sizeof(rsp), 2500)) {
    return false;
  }
  _write_attr_status_last_result       = (esp_zb_zcl_status_t)rsp.status;
  _write_attr_attribute_id_last_result = rsp.attr_id;
  return (rsp.success != 0);
#else
  if (attribute_size > 247) return false; // Prevent stack buffer overflow!
  uint8_t payload_data[250];
  memcpy(payload_data, &attribute_id, 2);
  payload_data[2] = attribute_type;
  if (attribute_value && attribute_size > 0) {
    memcpy(payload_data + 3, attribute_value, attribute_size);
  }

  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  _write_attr_last_tsn = _write_ext_current_tsn;
  _write_attr_last_tsn_flag = ack ? ZCL_CMD_TSN_SYNC : ZCL_CMD_TSN_ASYNC;
  sendAPSDEDataRequestCmd(
    device->short_addr, _write_ext_current_tsn, device->endpoint, 
    cluster_id, 0x02, attribute_size + 3, payload_data,  
    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, disable_default_response, 
    manuf_specific, manuf_code, true, profile_id);
  _write_ext_current_tsn++;
  esp_zb_lock_release();

  if (ack) {
    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      _write_attr_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
      return false;
    }
    return true;
  }
  return false;
#endif
}

bool ZigbeeGateway::sendAttributeWriteExt(
  uint16_t short_addr, uint8_t endpoint, uint16_t cluster_id, 
  uint16_t attribute_id, esp_zb_zcl_attr_type_t attribute_type, 
  uint16_t attribute_size, void *attribute_value, bool ack, 
  uint8_t manuf_specific, uint16_t manuf_code, 
  bool disable_default_response, uint16_t profile_id) {
  if (short_addr == 0) {
    log_e("invalid short address!");
    return false;
  }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = endpoint;
  return sendAttributeWriteExt(
    &dev, cluster_id, attribute_id, attribute_type, attribute_size,
    attribute_value, ack, manuf_specific, manuf_code, disable_default_response, profile_id);
}

void ZigbeeGateway::sendIASzoneEnrollResponseCmd(
  zbg_device_params_t *device, uint8_t enroll_rsp_code, uint8_t zone_id) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.param1       = enroll_rsp_code;
  req.param2       = zone_id;
  GeminiBridge::sendFrame(G_CMD_IAS_ENROLL_RSP, &req, sizeof(req));
#else
  esp_zb_zcl_ias_zone_enroll_response_cmd_t enroll_resp_req = {};
  enroll_resp_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  memcpy(enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  enroll_resp_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  enroll_resp_req.zcl_basic_cmd.src_endpoint = _endpoint;
  enroll_resp_req.enroll_rsp_code = enroll_rsp_code; 
  enroll_resp_req.zone_id = zone_id;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_ias_zone_enroll_cmd_resp(&enroll_resp_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendOnOffCmd(zbg_device_params_t *device, bool value) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.flag1        = value ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_ON_OFF, &req, sizeof(req));
#else
  esp_zb_zcl_on_off_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.on_off_cmd_id = value ? ESP_ZB_ZCL_CMD_ON_OFF_ON_ID : ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID;
  
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_on_off_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendOnOffCmd(uint16_t short_addr, uint8_t dst_endpoint, bool value) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendOnOffCmd(&dev, value);
}

void ZigbeeGateway::sendWindowCoveringCmd(
  zbg_device_params_t *device, uint8_t cmd_id, void *cmd_value) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = cmd_id;
  req.flag1        = (cmd_value != nullptr) ? 1 : 0;
  if (cmd_value != nullptr) {
    req.param1 = *((uint8_t *)cmd_value);
  }
  GeminiBridge::sendFrame(G_CMD_WINDOW_COVERING, &req, sizeof(req));
#else
  esp_zb_zcl_window_covering_cluster_send_cmd_req_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.cmd_id = cmd_id;
  cmd_req.value  = cmd_value;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_window_covering_cluster_send_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendWindowCoveringCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t cmd_id, void *cmd_value) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendWindowCoveringCmd(&dev, cmd_id, cmd_value);
}

void ZigbeeGateway::sendAddGroupRequestCmd(
  zbg_device_params_t *device, uint16_t group_id, bool local) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.param1       = group_id;
  req.flag1        = local ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_ADD_GROUP, &req, sizeof(req));
#else
  esp_zb_zcl_groups_add_group_cmd_t cmd_req = {};
  cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = local ? 0 : device->short_addr;
  cmd_req.zcl_basic_cmd.dst_endpoint          = local ? _endpoint : device->endpoint;
  cmd_req.zcl_basic_cmd.src_endpoint          = local ? device->endpoint : _endpoint;
  cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  cmd_req.group_id     = group_id;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_groups_add_group_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendLevelMoveToLevelCmd(
  zbg_device_params_t *device, uint8_t level, uint16_t transition_time, bool withOnOff) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.param1       = level;
  req.param2       = transition_time;
  req.flag1        = withOnOff ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_LEVEL_MOVE, &req, sizeof(req));
#else
  esp_zb_zcl_move_to_level_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.level           = level;
  cmd_req.transition_time = transition_time;
  
  esp_zb_lock_acquire(portMAX_DELAY);
  if (withOnOff)
    esp_zb_zcl_level_move_to_level_with_onoff_cmd_req(&cmd_req);
  else
    esp_zb_zcl_level_move_to_level_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendLevelMoveToLevelCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t level, 
  uint16_t transition_time, bool withOnOff) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendLevelMoveToLevelCmd(&dev, level, transition_time, withOnOff);
}

void ZigbeeGateway::sendColorMoveToHueCmd(
  zbg_device_params_t *device, uint8_t hue, uint8_t direction, uint16_t transition_time) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = G_COLOR_MOVE_HUE;
  req.param1       = hue;
  req.param2       = transition_time;
  req.flag1        = direction;
  GeminiBridge::sendFrame(G_CMD_COLOR_MOVE, &req, sizeof(req));
#else
  esp_zb_zcl_color_move_to_hue_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.hue             = hue;
  cmd_req.direction       = direction;
  cmd_req.transition_time = transition_time;
  
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_hue_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendColorMoveToHueCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t hue, uint8_t direction, uint16_t transition_time) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendColorMoveToHueCmd(&dev, hue, direction, transition_time);
}

void ZigbeeGateway::sendColorMoveToSaturationCmd(
  zbg_device_params_t *device, uint8_t saturation, uint16_t transition_time) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = G_COLOR_MOVE_SAT;
  req.param1       = saturation;
  req.param2       = transition_time;
  GeminiBridge::sendFrame(G_CMD_COLOR_MOVE, &req, sizeof(req));
#else
  esp_zb_zcl_color_move_to_saturation_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.saturation      = saturation;
  cmd_req.transition_time = transition_time;
  
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_saturation_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendColorMoveToSaturationCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t saturation, uint16_t transition_time) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendColorMoveToSaturationCmd(&dev, saturation, transition_time);
}

void ZigbeeGateway::sendColorMoveToHueAndSaturationCmd(
  zbg_device_params_t *device, uint8_t hue, uint8_t saturation, uint16_t transition_time) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = G_COLOR_MOVE_HUE_SAT;
  req.param1       = hue;
  req.param2       = saturation;
  req.param3       = transition_time;
  GeminiBridge::sendFrame(G_CMD_COLOR_MOVE, &req, sizeof(req));
#else
  esp_zb_color_move_to_hue_saturation_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.hue             = hue;
  cmd_req.saturation      = saturation;
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_hue_and_saturation_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendColorMoveToHueAndSaturationCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t hue, uint8_t saturation, uint16_t transition_time) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendColorMoveToHueAndSaturationCmd(&dev, hue, saturation, transition_time);
}

void ZigbeeGateway::sendColorEnhancedMoveToHueAndSaturationCmd(
  zbg_device_params_t *device, uint16_t enhanced_hue, uint8_t saturation, uint16_t transition_time) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = G_COLOR_ENH_MOVE_HUE_SAT;
  req.param1       = enhanced_hue;
  req.param2       = saturation;
  req.param3       = transition_time;
  GeminiBridge::sendFrame(G_CMD_COLOR_MOVE, &req, sizeof(req));
#else
  esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.enhanced_hue    = enhanced_hue;
  cmd_req.saturation      = saturation;
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendColorEnhancedMoveToHueAndSaturationCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint16_t enhanced_hue, uint8_t saturation, uint16_t transition_time) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendColorEnhancedMoveToHueAndSaturationCmd(&dev, enhanced_hue, saturation, transition_time);
}

void ZigbeeGateway::sendColorMoveToColorCmd(
  zbg_device_params_t *device, uint16_t color_x, uint16_t color_y, uint16_t transition_time) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = G_COLOR_MOVE_XY;
  req.param1       = color_x;
  req.param2       = color_y;
  req.param3       = transition_time;
  GeminiBridge::sendFrame(G_CMD_COLOR_MOVE, &req, sizeof(req));
#else
  esp_zb_zcl_color_move_to_color_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.color_x         = color_x;
  cmd_req.color_y         = color_y;
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_color_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendColorMoveToColorCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint16_t color_x, uint16_t color_y, uint16_t transition_time) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendColorMoveToColorCmd(&dev, color_x, color_y, transition_time);
}

void ZigbeeGateway::sendColorMoveToColorTemperatureCmd(
  zbg_device_params_t *device, uint16_t color_temperature, uint16_t transition_time) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.cmd_sub_type = G_COLOR_MOVE_CT;
  req.param1       = color_temperature;
  req.param2       = transition_time;
  GeminiBridge::sendFrame(G_CMD_COLOR_MOVE, &req, sizeof(req));
#else
  esp_zb_zcl_color_move_to_color_temperature_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  cmd_req.color_temperature = color_temperature;
  cmd_req.transition_time   = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_color_temperature_cmd_req(&cmd_req);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::sendColorMoveToColorTemperatureCmd(
  uint16_t short_addr, uint8_t dst_endpoint, uint16_t color_temperature, uint16_t transition_time) {
  if (short_addr == 0) { log_e("invalid short address!"); return; }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  sendColorMoveToColorTemperatureCmd(&dev, color_temperature, transition_time);
}

void ZigbeeGateway::ieee_Cb(
  esp_zb_zdp_status_t zdo_status, esp_zb_zdo_ieee_addr_rsp_t *resp, void *user_ctx) {
  log_i("IEEE Callback");
}

void ZigbeeGateway::sendIEEEAddrReqCmd(zbg_device_params_t *device, bool ack) {
  if (device->short_addr == 0) return;
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr = device->short_addr;
  req.flag1      = ack ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_IEEE_ADDR_REQ, &req, sizeof(req));
#else
  esp_zb_zdo_ieee_addr_req_param_t cmd_req = {};
  cmd_req.dst_nwk_addr     = device->short_addr;
  cmd_req.addr_of_interest = device->short_addr;
  cmd_req.request_type     = 0;
  cmd_req.start_index      = 0;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zdo_ieee_addr_req(&cmd_req, ieee_Cb, nullptr);
  esp_zb_lock_release();
#endif
}

void ZigbeeGateway::leave_Cb(esp_zb_zdp_status_t zdo_status, void *user_ctx) {
  log_i("leave status = %02X", zdo_status);
}

void ZigbeeGateway::sendDeviceLeaveRequest(
  esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr, bool remove_children, bool rejoin) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr = short_addr;
  memcpy(req.ieee_addr, ieee_addr, 8);
  req.param1     = remove_children ? 1 : 0;
  req.flag1      = rejoin ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_LEAVE_REQ, &req, sizeof(req));
#else
  esp_zb_zdo_mgmt_leave_req_param_t cmd_req = {};
  cmd_req.dst_nwk_addr    = short_addr;
  memcpy(cmd_req.device_address, ieee_addr, sizeof(esp_zb_ieee_addr_t));
  cmd_req.remove_children = remove_children ? 1 : 0;
  cmd_req.rejoin          = rejoin ? 1 : 0;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zdo_device_leave_req(&cmd_req, leave_Cb, nullptr);
  esp_zb_lock_release();
#endif
}

/*****************************************************************************/

void ZigbeeGateway::zbOTAUpgradeServerStatus(
  const esp_zb_zcl_ota_upgrade_server_status_message_t *message) {

  if (message->server_status == ESP_ZB_ZCL_OTA_UPGRADE_SERVER_ABORTED || 
      message->server_status == ESP_ZB_ZCL_OTA_UPGRADE_SERVER_END) {
    
    // Fixed null-pointer crash when upgrade_time is NULL on OTA abort!
    uint32_t upg_time = message->upgrade_time ? *(message->upgrade_time) : 0;

    #if defined(USE_GEMINI_PROTOCOL_NCP)
      uint8_t pkt[5];
      memcpy(pkt, &upg_time, 4);
      pkt[4] = message->server_status;
      GeminiBridge::sendFrame(G_EVT_OTA_STATUS, pkt, sizeof(pkt));
    #else
      if (_instance->_on_fill_ota_buffer) {
        _instance->_on_fill_ota_buffer(nullptr, upg_time, message->server_status);
      }
    #endif
  }
}
  
/*****************************************************************************/

bool ZigbeeGateway::zbOTAUpgradeServerQueryImage(
  const esp_zb_zcl_ota_upgrade_server_query_image_message_t *message) {

  if ((message->image_type == s_ota_image_type) && 
      (message->manufacturer_code == s_ota_manuf_code)) {
    return true;
  }
  log_e("OTA image mismatch!");
  return false;
}

/*****************************************************************************/

esp_err_t ZigbeeGateway::zb_ota_next_data_handler(
  esp_zb_ota_zcl_information_t message, uint16_t index, uint8_t size, 
  uint8_t **data) {
  return ESP_OK;
}

/*****************************************************************************/

esp_err_t ZigbeeGateway::sendOTAUpgradeServerNotifyRequest(
  esp_zb_ieee_addr_t ieee_addr, uint8_t *ota_file_start,
  esp_zb_ota_next_data_callback_t esp_zb_ota_next_data_callback) {

  if (!ota_file_start) return ESP_ERR_INVALID_ARG;

#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktOtaNotifyStart req = {};
  memcpy(req.ieee_addr, ieee_addr, 8);
  memcpy(req.header_raw, ota_file_start, sizeof(req.header_raw));

  uint8_t rsp[2] = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_OTA_NOTIFY_START, &req, sizeof(req), G_RSP_STATUS, rsp, sizeof(rsp), 3000)) {
    return ESP_FAIL;
  }
  return (rsp[0] != 0) ? ESP_OK : (esp_err_t)rsp[1];
#else
  const uint8_t *image_info = NULL;
  esp_zb_ota_image_header_t ota_image_header = {};
  esp_zb_ota_file_header_t  ota_file_header  = {};

  uint8_t length = sizeof(esp_zb_ota_image_header_t);
  memcpy(&ota_image_header, ota_file_start, length);

  if (ota_image_header.upgrade_file_id != OTA_UPGRADE_FILE_MAGIC_VALUE ||
      ota_image_header.header_version != OTA_UPGRADE_FILE_HEADER_VERSION) {
    return ESP_ERR_INVALID_ARG;
  }

  ota_file_header.manufacturer_code = ota_image_header.manufacturer_id;
  ota_file_header.image_type        = ota_image_header.image_type;
  ota_file_header.file_version      = ota_image_header.file_version;
  ota_file_header.image_size        = ota_image_header.image_size; 

  if (ota_image_header.field_control & ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER) {
    image_info = (const uint8_t *)(ota_file_start + length);
    ZB_OTA_FILE_HEADER_OPTIONAL(ota_file_header.optional, security_credential_version, length);
    ota_file_header.field_control |= ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER;
  }

  if (ota_image_header.field_control & ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC) {
    image_info = (const uint8_t *)(ota_file_start + length);
    ZB_OTA_FILE_HEADER_OPTIONAL(ota_file_header.optional, upgrade_file_destination, length);
  } else { 
    memcpy(ota_file_header.optional.upgrade_file_destination, ieee_addr, sizeof(esp_zb_ieee_addr_t));
    length += sizeof(esp_zb_ieee_addr_t);
  }
  ota_file_header.field_control |= ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC;
    
  if (ota_image_header.field_control & ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER) {
    image_info = (const uint8_t *)(ota_file_start + length);
    ZB_OTA_FILE_HEADER_OPTIONAL(ota_file_header.optional, minimum_hardware_version, length);
    image_info = (const uint8_t *)(ota_file_start + length);
    ZB_OTA_FILE_HEADER_OPTIONAL(ota_file_header.optional, maximum_hardware_version, length);
    ota_file_header.field_control |= ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER;
  }

  s_ota_image_type         = ota_file_header.image_type;
  s_ota_manuf_code         = ota_file_header.manufacturer_code;
  s_ota_image_size         = ota_image_header.image_size;
  s_ota_image_file_version = ota_image_header.file_version;

  esp_zb_ota_upgrade_server_notify_req_t req = {};
  req.endpoint         = _instance->getEndpoint();
  req.index            = 0;
  req.notify_on        = true;
  req.ota_upgrade_time = 0x0001;
  req.next_data_cb     = zb_ota_next_data_handler;
  memcpy(&req.ota_file_header, &ota_file_header, sizeof(esp_zb_ota_file_header_t));

  return esp_zb_ota_upgrade_server_notify_req(&req);
#endif
}

/*****************************************************************************/

void ZigbeeGateway::simple_descriptor_req_Cb(
  esp_zb_zdp_status_t zdo_status, esp_zb_af_simple_desc_1_1_t *simple_desc, 
  void *user_ctx) {
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS && user_ctx && simple_desc) {
    *((uint32_t *)user_ctx) = simple_desc->app_device_id;
  }
  xSemaphoreGive(gt_lock);
}

void ZigbeeGateway::sendSimpleDescriptorRequestCmd(
  uint16_t addr_of_interest, uint8_t endpoint, void *user_ctx) {
  if (user_ctx) *((uint32_t *)user_ctx) = 0;
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = addr_of_interest;
  req.dst_endpoint = endpoint;
  uint32_t dev_id  = 0;
  if (GeminiBridge::sendCommandWaitReply(
        G_CMD_SIMPLE_DESC_REQ, &req, sizeof(req), G_RSP_SIMPLE_DESC, &dev_id, sizeof(dev_id), 10000)) {
    if (user_ctx) *((uint32_t *)user_ctx) = dev_id;
  }
#else
  esp_zb_zdo_simple_desc_req_param_t cmd_req = {};
  cmd_req.addr_of_interest = addr_of_interest;
  cmd_req.endpoint         = endpoint;

  xSemaphoreTake(gt_lock, 0);
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zdo_simple_desc_req(&cmd_req, simple_descriptor_req_Cb, user_ctx);
  esp_zb_lock_release();

  if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(ZB_CMD_TIMEOUT)) != pdTRUE) {
    log_e("Semaphore timeout while requesting simple descriptor");
  }
#endif
}

/*****************************************************************************/

void ZigbeeGateway::sendDeviceFactoryReset(zbg_device_params_t *device, bool isTuya) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktSimpleCmd req = {};
  req.short_addr   = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint = device->endpoint;
  req.flag1        = isTuya ? 1 : 0;
  GeminiBridge::sendFrame(G_CMD_DEV_FACTORY_RESET, &req, sizeof(req));
#else
  esp_zb_zcl_basic_fact_reset_cmd_t cmd_req = {};
  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_basic_factory_reset_cmd_req(&cmd_req);
  esp_zb_lock_release();
  vTaskDelay(pdMS_TO_TICKS(500));
#endif
}

/*****************************************************************************/

void ZigbeeGateway::zbCmdDefaultResponse(
  uint8_t tsn, int8_t rssi, esp_zb_zcl_addr_t src_address, 
  uint16_t src_endpoint, uint16_t cluster_id, uint8_t resp_to_cmd, 
  esp_zb_zcl_status_t status_code) {

  #if defined(USE_GEMINI_PROTOCOL_NCP)
    uint8_t rssi_pkt[3];
    memcpy(rssi_pkt, &src_address.u.short_addr, 2);
    rssi_pkt[2] = (uint8_t)rssi;
    GeminiBridge::sendFrame(G_EVT_UPDATE_RSSI, rssi_pkt, sizeof(rssi_pkt));
  #else
    if (_on_update_device_last_rssi)
      _on_update_device_last_rssi(src_address.u.short_addr, rssi);
  #endif

  if ((_custom_cmd_last_tsn_flag == ZCL_CMD_TSN_SYNC) && (tsn == _custom_cmd_last_tsn)) {
    _custom_cmd_status_last_result      = status_code;
    _custom_cmd_resp_to_cmd_last_result = resp_to_cmd;
    _custom_cmd_last_tsn_flag           = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  }
}

/*****************************************************************************/

bool ZigbeeGateway::sendCustomClusterCmd(
  zbg_device_params_t *device, int16_t custom_cluster_id, 
  uint16_t custom_command_id, esp_zb_zcl_attr_type_t data_type, 
  uint16_t custom_data_size, uint8_t *custom_data, bool ack, 
  uint8_t direction, uint8_t disable_default_response, uint8_t manuf_specific,
  uint16_t manuf_code) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktCustomClusterReq req = {};
  req.short_addr        = device->short_addr;
  memcpy(req.ieee_addr, device->ieee_addr, 8);
  req.dst_endpoint      = device->endpoint;
  req.custom_cluster_id = custom_cluster_id;
  req.custom_command_id = custom_command_id;
  req.data_type         = (uint8_t)data_type;
  req.data_size         = (custom_data_size <= sizeof(req.data)) ? custom_data_size : sizeof(req.data);
  req.ack               = ack ? 1 : 0;
  req.direction         = direction;
  req.dis_default_resp  = disable_default_response;
  req.manuf_specific    = manuf_specific;
  req.manuf_code        = manuf_code;
  if (custom_data && req.data_size > 0) {
    memcpy(req.data, custom_data, req.data_size);
  }
  if (!ack) {
    GeminiBridge::sendFrame(G_CMD_CUSTOM_CLUSTER_REQ, &req, sizeof(req));
    return false;
  }
  uint8_t rsp[3] = {};
  if (!GeminiBridge::sendCommandWaitReply(
        G_CMD_CUSTOM_CLUSTER_REQ, &req, sizeof(req), G_RSP_CUSTOM_CMD, rsp, sizeof(rsp), 2500)) {
    return false;
  }
  _custom_cmd_status_last_result      = (esp_zb_zcl_status_t)rsp[1];
  _custom_cmd_resp_to_cmd_last_result = rsp[2];
  return (rsp[0] != 0);
#else
  esp_zb_zcl_custom_cluster_cmd_req_t req = {};
  if (device->short_addr != 0) {
    req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  req.zcl_basic_cmd.src_endpoint = _endpoint;
  req.cluster_id       = custom_cluster_id;
  req.profile_id       = ESP_ZB_AF_HA_PROFILE_ID;
  req.direction        = direction;
  req.manuf_specific   = manuf_specific;
  req.dis_default_resp = disable_default_response;
  req.manuf_code       = manuf_code;
  req.custom_cmd_id    = custom_command_id;
  req.data.type        = data_type; 
  req.data.size        = custom_data_size;
  req.data.value       = custom_data;
  
  if (ack) xSemaphoreTake(gt_lock, 0); 

  esp_zb_lock_acquire(portMAX_DELAY);
  _custom_cmd_last_tsn_flag = ack ? ZCL_CMD_TSN_SYNC : ZCL_CMD_TSN_ASYNC;
  _custom_cmd_last_tsn = esp_zb_zcl_custom_cluster_cmd_req(&req);
  esp_zb_lock_release();

  if (ack) {
    if (xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      _custom_cmd_last_tsn_flag = ZCL_CMD_TSN_UNKNOWN;
      return false;
    }
    return true;
  }
  return false;
#endif
}

bool ZigbeeGateway::sendCustomClusterCmd(
  uint16_t short_addr, uint8_t dst_endpoint, int16_t custom_cluster_id, 
  uint16_t custom_command_id, esp_zb_zcl_attr_type_t data_type, 
  uint16_t custom_data_size, uint8_t *custom_data, bool ack, 
  uint8_t direction, uint8_t disable_default_response, uint8_t manuf_specific,
  uint16_t manuf_code) {
  if (short_addr == 0) {
    log_e("invalid short address!");
    return false;
  }
  zbg_device_params_t dev = {};
  dev.short_addr = short_addr;
  dev.endpoint   = dst_endpoint;
  return sendCustomClusterCmd(
    &dev, custom_cluster_id, custom_command_id, data_type,
    custom_data_size, custom_data, ack, direction,
    disable_default_response, manuf_specific, manuf_code);
}

/*****************************************************************************/

void ZigbeeGateway::zbCmdCustomClusterReq(
  uint8_t tsn, esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, 
  uint16_t cluster_id, uint8_t command_id, uint16_t payload_size, 
  uint8_t *payload) {
#if !defined(USE_GEMINI_PROTOCOL_HOST)
  uint16_t short_addr = src_address.u.short_addr;
  esp_zb_ieee_addr_t ieee_addr = {};
  esp_zb_ieee_address_by_short(short_addr, ieee_addr);

  #if defined(USE_GEMINI_PROTOCOL_NCP)
    uint16_t copy_len = (payload_size <= 255) ? payload_size : 255;
    uint8_t pkt_buf[sizeof(G_PktEvtCustomClusterCmd) + 255];
    auto *pkt = reinterpret_cast<G_PktEvtCustomClusterCmd *>(pkt_buf);
    pkt->tsn          = tsn;
    memcpy(pkt->ieee_addr, ieee_addr, 8);
    pkt->short_addr   = short_addr;
    pkt->endpoint_id  = src_endpoint;
    pkt->cluster_id   = cluster_id;
    pkt->command_id   = command_id;
    pkt->payload_size = copy_len;
    if (copy_len > 0 && payload) {
      memcpy(pkt->payload, payload, copy_len);
    }
    GeminiBridge::sendFrame(
      G_EVT_CUSTOM_CLUSTER_CMD, pkt_buf, sizeof(G_PktEvtCustomClusterCmd) + copy_len);
  #else
    if (_on_cmd_custom_cluster_receive)
      _on_cmd_custom_cluster_receive(
        tsn, ieee_addr, short_addr, src_endpoint, cluster_id, 
        command_id, payload_size, payload);
  #endif
#endif
}

/*****************************************************************************/

bool ZigbeeGateway::zbRawCmdHandler(
  uint8_t seq_number, esp_zb_zcl_addr_t source, int8_t rssi, 
  uint8_t src_endpoint, uint8_t dst_endpoint, uint16_t cluster_id, 
  uint8_t cmd_id, uint16_t group_id, bool is_common_command, 
  bool disable_default_response, bool is_manuf_specific, 
  uint16_t manuf_specific, uint8_t buffer_size, uint8_t *buffer) {

#if !defined(USE_GEMINI_PROTOCOL_HOST)
  #if !defined(USE_GEMINI_PROTOCOL_NCP)
    if (_on_update_device_last_rssi)
      _on_update_device_last_rssi(source.u.short_addr, rssi);
  #endif
  
  // 1. Sonoff broken array attribute reporting/read response fix (0x501D / 0x501F)
  if ((cluster_id == SONOFF_CUSTOM_CLUSTER) && ((cmd_id == 0x01) || (cmd_id == 0x0A)) && buffer_size >= 3) {
    uint16_t attribute_id = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    
    if ((attribute_id == 0x501D) || (attribute_id == 0x501F)) {
      if (cmd_id == 0x01 && buffer_size >= 4) {
        esp_zb_zcl_status_t esp_status = (esp_zb_zcl_status_t)buffer[2];
        esp_zb_zcl_attribute_t attribute = {};
        attribute.id         = attribute_id;
        attribute.data.type  = (esp_zb_zcl_attr_type_t)buffer[3];
        attribute.data.size  = buffer_size - 4;
        attribute.data.value = (buffer + 4); 

        zbReadAttrResponse(
          seq_number, rssi, source, src_endpoint, cluster_id, esp_status, &attribute);
      } else if (cmd_id == 0x0A) {
        esp_zb_zcl_attribute_t attribute = {};
        attribute.id         = attribute_id;
        attribute.data.type  = (esp_zb_zcl_attr_type_t)buffer[2];
        attribute.data.size  = buffer_size - 3;
        attribute.data.value = (buffer + 3); 

        #if defined(USE_GEMINI_PROTOCOL_NCP)
          GeminiBridge::forwardAttrReport(
            source.u.short_addr, src_endpoint, cluster_id, &attribute, true, false, rssi);
        #else
          zbProcessAttributeReporting(source, src_endpoint, cluster_id, &attribute, true);
        #endif
      }
      if (!disable_default_response) {
        uint8_t payload_data[2] = { cmd_id, 0x00 };
        sendAPSDEDataRequestCmd(
          source.u.short_addr, seq_number, src_endpoint, cluster_id, 0x0B, 
          2, payload_data, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 0, 0, true, 
          ESP_ZB_AF_HA_PROFILE_ID);
      }
      return true;
    }
  }

  // 2. FAST EXIT for standard ZCL Foundation commands (Report/ReadRsp/WriteRsp/etc.)
  if (is_common_command) {
    #if defined(USE_GEMINI_PROTOCOL_NCP)
      // Update RSSI for standard reports/responses not carrying RSSI in their ZBOSS struct
      if (cmd_id == 0x0A) {
        uint8_t rssi_pkt[3];
        memcpy(rssi_pkt, &source.u.short_addr, 2);
        rssi_pkt[2] = (uint8_t)rssi;
        GeminiBridge::sendFrame(G_EVT_UPDATE_RSSI, rssi_pkt, sizeof(rssi_pkt));
      }
    #endif
    return false;
  }

  // 3. OTA Upgrade Image Block Request (cmd_id == 3)
  if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE) && (cmd_id == 3) && buffer_size >= 14) {
    uint8_t zcl_frame[255];
    uint8_t *p = zcl_frame;

    *p++ = (0x01) | (0 << 2) | (1 << 3) | (1 << 4);
    *p++ = seq_number;
    *p++ = 0x05; // Image Block Response

    // Safe aligned extraction of file_offset and max_data_size
    uint32_t file_offset = 0;
    memcpy(&file_offset, buffer + 9, sizeof(uint32_t));
    uint8_t max_data_size = buffer[13];
    if (max_data_size > 128) max_data_size = 128;

    *p++ = 0x00; // SUCCESS
    memcpy(p, buffer + 1, 13);
    p += 13;

    size_t bytes_read = 0;
    #if defined(USE_GEMINI_PROTOCOL_NCP)
      if (_instance->_on_fill_ota_buffer) {
        bytes_read = _instance->_on_fill_ota_buffer(p, file_offset, max_data_size);
      } else {
        bytes_read = GeminiBridge::requestOtaChunkFromHost(p, file_offset, max_data_size);
      }
    #else
      if (_instance->_on_fill_ota_buffer) {
        bytes_read = _instance->_on_fill_ota_buffer(p, file_offset, max_data_size);
      }
    #endif

    if (bytes_read != max_data_size) {
      *(p - 1) = (uint8_t)bytes_read;
    }
    p += bytes_read;

    uint8_t zcl_len = (uint8_t)(p - zcl_frame);
    esp_zb_apsde_data_req_t aps_req = {};
    aps_req.dst_addr_mode       = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    aps_req.dst_addr.addr_short = source.u.short_addr;
    aps_req.dst_endpoint        = src_endpoint;
    aps_req.profile_id          = ESP_ZB_AF_HA_PROFILE_ID;
    aps_req.cluster_id          = ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE;
    aps_req.src_endpoint        = dst_endpoint;
    aps_req.asdu_length         = zcl_len;
    aps_req.asdu                = zcl_frame;
    aps_req.tx_options          = ESP_ZB_APSDE_TX_OPT_ACK_TX;
    aps_req.use_alias           = false;
    aps_req.radius              = 30;

    esp_zb_aps_data_request(&aps_req);
    return true;
  }

  // 4. OTA Upgrade Query Next Image Request (cmd_id == 1)
  if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE) && (cmd_id == 0x01) && buffer_size >= 9) {
    uint16_t req_manufacturer_code = 0;
    uint16_t req_image_type = 0;
    memcpy(&req_manufacturer_code, buffer + 1, 2);
    memcpy(&req_image_type,        buffer + 3, 2);

    uint8_t zcl_frame[64];
    uint8_t *p = zcl_frame;

    *p++ = (0x01) | (0 << 2) | (1 << 3) | (1 << 4);
    *p++ = seq_number;
    *p++ = 0x02; // Query Next Image Response

    if ((req_image_type == s_ota_image_type) && (req_manufacturer_code == s_ota_manuf_code)) {
      *p++ = ESP_ZB_ZCL_STATUS_SUCCESS;
      memcpy(p, buffer + 1, 4);               p += 4;
      memcpy(p, &s_ota_image_file_version, 4); p += 4;
      memcpy(p, &s_ota_image_size, 4);         p += 4;
    } else {
      *p++ = ESP_ZB_ZCL_STATUS_NO_IMAGE_AVAILABLE;
    }

    uint8_t zcl_len = (uint8_t)(p - zcl_frame);
    esp_zb_apsde_data_req_t aps_req = {};
    aps_req.dst_addr_mode       = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    aps_req.dst_addr.addr_short = source.u.short_addr;
    aps_req.dst_endpoint        = src_endpoint;
    aps_req.profile_id          = ESP_ZB_AF_HA_PROFILE_ID;
    aps_req.cluster_id          = ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE;
    aps_req.src_endpoint        = dst_endpoint;
    aps_req.asdu_length         = zcl_len;
    aps_req.asdu                = zcl_frame;
    aps_req.tx_options          = ESP_ZB_APSDE_TX_OPT_ACK_TX;
    aps_req.use_alias           = false;
    aps_req.radius              = 30;

    esp_zb_aps_data_request(&aps_req);
    return true; 
  }

  // 5. Tuya Private Cluster 0xEF00 Time Sync (cmd_id == 0x24 / 0x25)
  if ((cluster_id == TUYA_PRIVATE_CLUSTER_EF00) && ((cmd_id == 0x24) || (cmd_id == 0x25))) {
    // Fixed union corruption: resolve IEEE address into a separate array!
    esp_zb_ieee_addr_t ieee_addr = {};
    esp_zb_ieee_address_by_short(source.u.short_addr, ieee_addr);

    #if defined(USE_GEMINI_PROTOCOL_NCP)
      uint8_t pkt_buf[sizeof(G_PktEvtCustomClusterCmd) + 255];
      auto *pkt = reinterpret_cast<G_PktEvtCustomClusterCmd *>(pkt_buf);
      pkt->tsn          = seq_number;
      memcpy(pkt->ieee_addr, ieee_addr, 8);
      pkt->short_addr   = source.u.short_addr;
      pkt->endpoint_id  = src_endpoint;
      pkt->cluster_id   = cluster_id;
      pkt->command_id   = cmd_id;
      pkt->payload_size = buffer_size;
      if (buffer_size > 0 && buffer) {
        memcpy(pkt->payload, buffer, buffer_size);
      }
      GeminiBridge::sendFrame(
        G_EVT_CUSTOM_CLUSTER_CMD, pkt_buf, sizeof(G_PktEvtCustomClusterCmd) + buffer_size);
    #else
      if (_on_cmd_custom_cluster_receive)
        _on_cmd_custom_cluster_receive(
          seq_number, ieee_addr, source.u.short_addr, src_endpoint, 
          cluster_id, cmd_id, buffer_size, buffer);
    #endif
    return true;
  }

  // 6. Remote Control / Button Cluster-Specific Commands (ON, OFF, MOVE_TO_LEVEL, STEP, SCENES, etc.)
  #if defined(USE_GEMINI_PROTOCOL_NCP)
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_SCENES        ||
        cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF        ||
        cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL ||
        cluster_id == ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL ||
        cluster_id == ADEO_CUSTOM_CLUSTER                 ||
        cluster_id == IKEA_PRIVATE_CLUSTER                ||
        cluster_id == IKEA_PRIVATE_CLUSTER_2) {

      uint8_t pkt_buf[sizeof(G_PktEvtRawCustomCmd) + 255];
      auto *pkt = reinterpret_cast<G_PktEvtRawCustomCmd *>(pkt_buf);
      pkt->short_addr  = source.u.short_addr;
      pkt->endpoint_id = src_endpoint;
      pkt->cluster_id  = cluster_id;
      pkt->cmd_id      = cmd_id;
      pkt->group_id    = group_id;
      pkt->rssi        = rssi;
      pkt->buffer_size = buffer_size;
      if (buffer_size > 0 && buffer) {
        memcpy(pkt->buffer, buffer, buffer_size);
      }
      GeminiBridge::sendFrame(
        G_EVT_RAW_CUSTOM_CMD, pkt_buf, sizeof(G_PktEvtRawCustomCmd) + buffer_size);
      return true;
    }
    return false;
  #else
    if (_on_custom_cmd_receive)
      return _on_custom_cmd_receive(
        source.u.short_addr, src_endpoint, cluster_id, cmd_id, group_id, 
        buffer_size, buffer);
    return false;
  #endif
#else
  return false;
#endif
}

/*****************************************************************************/

bool ZigbeeGateway::sendAPSDEDataRequestCmd(
  uint16_t short_addr, uint8_t tsn, uint8_t dst_endpoint, 
  uint16_t cluster_id, uint8_t command_id, uint8_t payload_size, 
  uint8_t *payload_data, uint8_t direction, 
  uint8_t disable_default_response, uint8_t manuf_specific, 
  uint16_t manuf_code, bool is_global, uint16_t profile_id) {

#if defined(USE_GEMINI_PROTOCOL_HOST)
  G_PktApsdeRawReq req = {};
  req.short_addr       = short_addr;
  req.tsn              = tsn;
  req.dst_endpoint     = dst_endpoint;
  req.cluster_id       = cluster_id;
  req.command_id       = command_id;
  req.direction        = direction;
  req.dis_default_resp = disable_default_response;
  req.manuf_specific   = manuf_specific;
  req.manuf_code       = manuf_code;
  req.is_global        = is_global ? 1 : 0;
  req.profile_id       = profile_id;
  req.payload_size     = (payload_size <= sizeof(req.payload)) ? payload_size : sizeof(req.payload);
  if (payload_data && req.payload_size > 0) {
    memcpy(req.payload, payload_data, req.payload_size);
  }
  return GeminiBridge::sendFrame(G_CMD_APSDE_RAW_REQ, &req, sizeof(req));
#else
  uint8_t hdr_len = manuf_specific ? 5 : 3;
  if ((uint16_t)payload_size + hdr_len > 255) return false;

  uint8_t zcl_frame[255];
  uint8_t *p = zcl_frame;

  uint8_t frame_type = is_global ? 0 : 1;
  uint8_t frame_control = 
    (frame_type) |          
    (manuf_specific << 2) |
    (direction << 3) |
    (disable_default_response << 4);
      
  *p++ = frame_control;

  if (manuf_specific) {
    memcpy(p, &manuf_code, 2);
    p += 2;
  } 
  *p++ = tsn;
  *p++ = command_id; 

  if (payload_size > 0 && payload_data) {
    memcpy(p, payload_data, payload_size);
    p += payload_size;
  }
  
  uint8_t zcl_len = (uint8_t)(p - zcl_frame);

  esp_zb_apsde_data_req_t aps_req = {};
  aps_req.dst_addr_mode       = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  aps_req.dst_addr.addr_short = short_addr;
  aps_req.dst_endpoint        = dst_endpoint;
  aps_req.profile_id          = profile_id;
  aps_req.cluster_id          = cluster_id;
  aps_req.src_endpoint        = _endpoint;
  aps_req.asdu_length         = zcl_len;
  aps_req.asdu                = zcl_frame;
  aps_req.tx_options          = ESP_ZB_APSDE_TX_OPT_ACK_TX;
  aps_req.use_alias           = false;
  aps_req.radius              = 30;

  esp_err_t err = esp_zb_aps_data_request(&aps_req);
  if (err != ESP_OK) {
    log_e("Failed to send APSDE data req: %d", err);
    return false;
  }
  return true;
#endif
}

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);
#endif // CONFIG_ZB_ENABLED || defined(USE_GEMINI_PROTOCOL)