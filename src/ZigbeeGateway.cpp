#include "ZigbeeGateway.h"
#if SOC_IEEE802154_SUPPORTED 
//&& CONFIG_ZB_ENABLED

// Initialize the static instance of the class
ZigbeeGateway *ZigbeeGateway::_instance = nullptr;

findcb_userdata_t ZigbeeGateway::findcb_userdata;
bool ZigbeeGateway::_last_bind_success = false;
bool ZigbeeGateway::_in_binding = false;
bool ZigbeeGateway::_new_device_joined = false;
uint16_t ZigbeeGateway::_clusters_2_discover = 0;
uint16_t ZigbeeGateway::_attributes_2_discover = 0;
uint16_t ZigbeeGateway::_endpoints_2_bind = 0;
uint16_t ZigbeeGateway::_clusters_2_bind = 0;
uint8_t ZigbeeGateway::_binding_error_retries = 0;
query_basic_cluster_data_t ZigbeeGateway::_last_device_query;
uint8_t ZigbeeGateway::_read_attr_last_tsn = 0;
uint8_t ZigbeeGateway::_read_attr_tsn_list[256];
uint8_t ZigbeeGateway::_custom_cmd_last_tsn = 0;
uint8_t ZigbeeGateway::_custom_cmd_tsn_list[256];
zbg_device_unit_t ZigbeeGateway::zbg_device_units[ZBG_MAX_DEVICES];


esp_zb_zcl_attribute_t ZigbeeGateway::_read_attr_last_result;
//

#define ZB_CMD_TIMEOUT 10000

SemaphoreHandle_t ZigbeeGateway::gt_lock;

ZigbeeGateway::ZigbeeGateway(uint8_t endpoint) : ZigbeeEP(endpoint) {
  _device_id = ESP_ZB_HA_HOME_GATEWAY_DEVICE_ID; 
  _instance = this;  // Set the static pointer to this instance
  
  _new_device_joined = false;
  _last_bind_success = false;

  _clusters_2_discover = 0;
  _attributes_2_discover = 0;

  memset(_read_attr_tsn_list, 0, sizeof(_read_attr_tsn_list));
  memset(_custom_cmd_tsn_list, 0, sizeof(_custom_cmd_tsn_list));
  memset(&_last_device_query, 0, sizeof(query_basic_cluster_data_t));
  
  memset(zbg_device_units, 0, sizeof(zbg_device_units));

  _joined_devices.clear();
  _gateway_devices.clear();

  #if !CONFIG_DISABLE_HAL_LOCKS
  if (!gt_lock) {
    gt_lock = xSemaphoreCreateBinary();
    if (gt_lock == NULL) {
      log_e("Semaphore creation failed");
    }
  }
  #endif


  //use custom config to avoid narrowing error -> must be fixed in zigbee-sdk
  esp_zb_configuration_tool_cfg_t gateway_cfg = ZB_DEFAULT_GATEWAY_CONFIG();

  esp_zb_ias_zone_cluster_cfg_t zone_cfg = {
        .zone_state   = 0,
        .zone_type    = 0x0016,// SS_IAS_ZONE_TYPE_DOOR_WINDOW_HANDLE
        .zone_status  = 0,
        .ias_cie_addr = ESP_ZB_ZCL_ZONE_IAS_CIE_ADDR_DEFAULT,
        .zone_id      = 0xFF,
    };

  esp_zb_on_off_switch_cfg_t switch_cfg = ESP_ZB_DEFAULT_ON_OFF_SWITCH_CONFIG();

  
  esp_zb_attribute_list_t poll_cluster;
  //esp_zb_attribute_list_t tuya_private_cluster0;
  //esp_zb_attribute_list_t tuya_private_cluster1;

  //tuya_private_cluster0.attribute.id = 0xD001;
  //tuya_private_cluster0.cluster_id = 0xE001;
  //tuya_private_cluster0.next = NULL;
  
  esp_zb_on_off_cluster_cfg_t on_off_cfg;
  on_off_cfg.on_off = ESP_ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE;

  esp_zb_time_cluster_cfg_t time_cfg;
  time_cfg.time = 798575471;
  time_cfg.time_status = 3;

  esp_zb_level_cluster_cfg_t level_cfg;
  level_cfg.current_level = ESP_ZB_ZCL_LEVEL_CONTROL_CURRENT_LEVEL_DEFAULT_VALUE;

  esp_zb_groups_cluster_cfg_t groups_cfg;
  groups_cfg.groups_name_support_id = ESP_ZB_ZCL_GROUPS_NAME_SUPPORT_DEFAULT_VALUE;

  esp_zb_scenes_cluster_cfg_t scenes_cfg; 
  scenes_cfg.scenes_count = ESP_ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE;         
  scenes_cfg.current_scene = ESP_ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE;         
  scenes_cfg.current_group = ESP_ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE;         
  scenes_cfg.scene_valid = ESP_ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE;             
  scenes_cfg.name_support = ESP_ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE;

  esp_zb_color_cluster_cfg_t color_control_cfg;
  

  _cluster_list = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&(gateway_cfg.basic_cfg));
  esp_zb_cluster_list_add_basic_cluster(_cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
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

  //esp_zb_cluster_list_add_time_cluster(_cluster_list, esp_zb_time_cluster_create(&time_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

time_t utc_time = 798653565;
uint8_t time_status = 0;
time_t local_time = 798653565;
 esp_zb_attribute_list_t *time_cluster_server = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_TIME);

   /*esp_err_t ret = esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_TIME_ZONE_ID, (void *)&gmt_offset);
   if (ret != ESP_OK) {
     log_e("Failed to add time zone attribute: 0x%x: %s", ret, esp_err_to_name(ret));
   }*/
  esp_err_t ret = esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_TIME_ID, (void *)&utc_time);
  if (ret != ESP_OK) {
    log_e("Failed to add time attribute: 0x%x: %s", ret, esp_err_to_name(ret));
  }
  ret = esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, (void *)&time_status);
  if (ret != ESP_OK) {
    log_e("Failed to add time status attribute: 0x%x: %s", ret, esp_err_to_name(ret));
  }
ret = esp_zb_time_cluster_add_attr(time_cluster_server, ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, (void *)&local_time);
  if (ret != ESP_OK) {
    log_e("Failed to add time local time attribute: 0x%x: %s", ret, esp_err_to_name(ret));
  }
  // Add time clusters to cluster list
  ret = esp_zb_cluster_list_add_time_cluster(_cluster_list, time_cluster_server, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  if (ret != ESP_OK) {
    log_e("Failed to add time cluster (server role): 0x%x: %s", ret, esp_err_to_name(ret));
  }


  esp_zb_cluster_list_add_ias_zone_cluster(_cluster_list, esp_zb_ias_zone_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_temperature_meas_cluster(_cluster_list, esp_zb_temperature_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_humidity_meas_cluster(_cluster_list, esp_zb_humidity_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_pressure_meas_cluster(_cluster_list, esp_zb_pressure_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(&on_off_cfg), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_switch_config_cluster(_cluster_list, esp_zb_on_off_switch_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_electrical_meas_cluster(_cluster_list, esp_zb_electrical_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_metering_cluster(_cluster_list, esp_zb_metering_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_illuminance_meas_cluster(_cluster_list, esp_zb_illuminance_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_occupancy_sensing_cluster(_cluster_list, esp_zb_occupancy_sensing_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_thermostat_cluster(_cluster_list, esp_zb_thermostat_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0x0020), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_0),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_1),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_0),ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_1),ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_EF00),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_EF00),ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC80),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC80),ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC7F),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(0xFC7F),ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_attribute_list_t *tyua_on_off_cluster = esp_zb_cluster_list_get_cluster(_cluster_list, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  
  _ep_config = {.endpoint = _endpoint, .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, .app_device_id = ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID, .app_device_version = 0};
}

void ZigbeeGateway::bindCb(esp_zb_zdp_status_t zdo_status, void *user_ctx) {
  
  
  zbg_device_params_t *device = (zbg_device_params_t *)user_ctx;

  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
          
      if (device->ZC_binding) { 
        log_i("ZC has bounded to ZED (0x%x), endpoint (0x%x) cluster (0x%x)", device->short_addr, device->endpoint, device->cluster_id);
      } else 
        log_i("ZED (0x%x), endpoint (0x%x) cluster (0x%x) has bounded to ZC", device->short_addr, device->endpoint, device->cluster_id);
      _is_bound = true;
      _last_bind_success = true;
  } else {
      log_e("Binding failed (0x%x)! Device (0x%x), endpoint (0x%x), cluster (0x%x)", zdo_status, device->short_addr, device->endpoint, device->cluster_id);
      //if (zdo_status == 0x8c) 
      if(_binding_error_retries > 0) {
        _last_bind_success = false;
        --_binding_error_retries;
      }
      else _last_bind_success = true;
  }
  _in_binding = false;
  //free(device);
  log_i("Semaphore give in binding");
  xSemaphoreGive(gt_lock);
}

void ZigbeeGateway::find_Cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx) {
  
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    
    
    esp_zb_zdo_bind_req_param_t bind_req;
    
    zbg_device_params_t *sensor = (zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));
    sensor->endpoint = endpoint;
    sensor->short_addr = addr;
    esp_zb_ieee_address_by_short(sensor->short_addr, sensor->ieee_addr);
    log_d("Sensor found: short address(0x%x), endpoint(%d)", sensor->short_addr, sensor->endpoint);

    _new_device_joined = true;
    _instance->_joined_devices.push_back(sensor); 
  }
}

 void ZigbeeGateway::Z2S_active_ep_req_cb(esp_zb_zdp_status_t zdo_status, uint8_t ep_count, uint8_t *ep_id_list, void *user_ctx) {
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
      
      uint16_t short_addr = *((uint16_t*)(user_ctx));
      esp_zb_zdo_simple_desc_req_param_t cl_cmd_req;

      log_i("Z2S active_ep_req: device address %d, endpoints count %d", short_addr, ep_count);
      
      for (int i = 0; i < ep_count; i++) {
        log_i("Endpont # %d, id %d ", i+1, *(ep_id_list+i));
        cl_cmd_req.addr_of_interest = short_addr;
        cl_cmd_req.endpoint = *(ep_id_list+i);
        if (cl_cmd_req.endpoint != 0xF2) esp_zb_zdo_simple_desc_req(&cl_cmd_req, Z2S_simple_desc_req_cb, user_ctx);
    }
  }
  else log_i("Z2S active_ep_req failed");
}

void ZigbeeGateway::Z2S_simple_desc_req_cb(esp_zb_zdp_status_t zdo_status, esp_zb_af_simple_desc_1_1_t *simple_desc, void *user_ctx) {
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    
    uint16_t short_addr = *((uint16_t*)(user_ctx));
    
    esp_zb_zcl_disc_attr_cmd_t disc_attr_cmd_req;

    log_i("Z2S simple_desc_req: device address %d, endpoint # %d", short_addr, simple_desc->endpoint);
    log_i("Z2S simple_desc_req: in clusters # %d, out clusters # %d", simple_desc->app_input_cluster_count, simple_desc->app_output_cluster_count);
    
    for (int i = 0; i < simple_desc->app_input_cluster_count; i++) {
      
      log_i("In cluster # %d, id %d ", i+1, *(simple_desc->app_cluster_list+i));

      disc_attr_cmd_req.zcl_basic_cmd.dst_endpoint = simple_desc->endpoint;
      disc_attr_cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
      disc_attr_cmd_req.zcl_basic_cmd.src_endpoint = 1;
      disc_attr_cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      disc_attr_cmd_req.cluster_id = *(simple_desc->app_cluster_list+i);
      disc_attr_cmd_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
      disc_attr_cmd_req.start_attr_id = 0;
      disc_attr_cmd_req.max_attr_number = 99;
      
      esp_zb_zcl_disc_attr_cmd_req(&disc_attr_cmd_req);
    }
    
    for (int i = 0; i < simple_desc->app_output_cluster_count; i++) {
      log_i("Out cluster # %d, id %d ", i+1, *(simple_desc->app_cluster_list + simple_desc->app_input_cluster_count + i));

      disc_attr_cmd_req.zcl_basic_cmd.dst_endpoint = simple_desc->endpoint;
      disc_attr_cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
      disc_attr_cmd_req.zcl_basic_cmd.src_endpoint = 1;
      disc_attr_cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      disc_attr_cmd_req.cluster_id = *(simple_desc->app_cluster_list + simple_desc->app_input_cluster_count + i);
      disc_attr_cmd_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
      disc_attr_cmd_req.start_attr_id = 0;
      disc_attr_cmd_req.max_attr_number = 99;
      
      esp_zb_zcl_disc_attr_cmd_req(&disc_attr_cmd_req);
        
    } 
  }
  else log_i("Z2S simple desc failed");
}
uint16_t short_addr_req;

void ZigbeeGateway::zbPrintDeviceDiscovery (zbg_device_params_t * device) {

  
  esp_zb_zdo_active_ep_req_param_t ep_cmd_req;
  
  short_addr_req = device->short_addr; 
  ep_cmd_req.addr_of_interest = short_addr_req; 

  log_i("short_addr_req %d", short_addr_req);      
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zdo_active_ep_req(&ep_cmd_req, Z2S_active_ep_req_cb, &short_addr_req);
  esp_zb_lock_release();
}

bool ZigbeeGateway::zbQueryDeviceBasicCluster(zbg_device_params_t * device) {
  
  esp_zb_zcl_read_attr_cmd_t read_req;

  if (device->short_addr != 0) {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }

  read_req.zcl_basic_cmd.src_endpoint = _endpoint;
  read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  read_req.clusterID = ESP_ZB_ZCL_CLUSTER_ID_BASIC;

  /*uint16_t attributes[7] = {  ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 
                              ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 
                              0xFFFE};*/

  uint16_t attributes[2] = {  ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID };
  
  
  /*uint16_t attributes[6] = {  ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 
                              ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 
                              0xFFFE};*/
  
  for (int attribute_number = 0; attribute_number < 2; attribute_number++) {
    
    read_req.attr_number = 1; //ZB_ARRAY_LENTH(attributes);
    read_req.attr_field = &attributes[attribute_number];

    log_i("Query basic cluster attribute 0x%x", *read_req.attr_field);

    esp_zb_lock_acquire(portMAX_DELAY);
    uint8_t basic_tsn = esp_zb_zcl_read_attr_cmd_req(&read_req);
    esp_zb_lock_release();
    delay(500);
    log_i("basic tsn 0x%x", basic_tsn);

    //Wait for response or timeout
    if (xSemaphoreTake(gt_lock, /*pdMS_TO_TICKS(10000)*/ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Error while querying basic cluster attribute 0x%x", attributes[attribute_number]);
      if (attributes[attribute_number] == ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID) return false;
    }
  }
  /*read_req.attr_number = 1; //ZB_ARRAY_LENTH(attributes);
  read_req.attr_field = &attributes[6];

  log_i("Query basic cluster attribute 0x%x", *read_req.attr_field);

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_read_attr_cmd_req(&read_req);
  esp_zb_lock_release();
  delay(500);*/
  return true; 
}

//void ZigbeeGateway::zbReadBasicCluster(const esp_zb_zcl_attribute_t *attribute) {
void ZigbeeGateway::zbReadBasicCluster(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, esp_zb_zcl_attribute_t *attribute) {
  /* Basic cluster attributes */
  if (attribute->id == ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING && attribute->data.value) {
    zbstring_t *zbstr = (zbstring_t *)attribute->data.value;
    memcpy(_last_device_query.zcl_manufacturer_name, zbstr->data, zbstr->len);
    _last_device_query.zcl_manufacturer_name[zbstr->len] = '\0';
    log_i("Peer Manufacturer is \"%s\"", _last_device_query.zcl_manufacturer_name);
    xSemaphoreGive(gt_lock);
  }
  if (attribute->id == ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING && attribute->data.value) {
    zbstring_t *zbstr = (zbstring_t *)attribute->data.value;
    memcpy(_last_device_query.zcl_model_name, zbstr->data, zbstr->len);
    _last_device_query.zcl_model_name[zbstr->len] = '\0';
    log_i("Peer Model is \"%s\"", _last_device_query.zcl_model_name);
    xSemaphoreGive(gt_lock);
  }
  if (attribute->id == ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8 && attribute->data.value) {
   
    _last_device_query.zcl_version_id = *((uint8_t*)attribute->data.value);
    log_i("ZCL version id is \"%d\"", _last_device_query.zcl_version_id);
    xSemaphoreGive(gt_lock);
  }
  if (attribute->id == ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8 && attribute->data.value) {
   
    _last_device_query.zcl_application_version_id = *((uint8_t*)attribute->data.value);
    log_i("ZCL application version id is \"%d\"", _last_device_query.zcl_application_version_id);
    xSemaphoreGive(gt_lock);
  }
  if (attribute->id == ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM && attribute->data.value) {
   
    _last_device_query.zcl_power_source_id = *((uint8_t*)attribute->data.value);
    log_i("ZCL power source id is \"%d\"", _last_device_query.zcl_power_source_id);
    xSemaphoreGive(gt_lock);
  }
  if (attribute->id == 0xFFFE) { // && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8 && attribute->data.value) {
    log_i("Attribute 0xFFFE, data type 0x%x", attribute->data.type);
    //_last_device_query.zcl_power_source_id = *((uint8_t*)attribute->data.value);
    //log_i("ZCL power source id is \"%d\"", _last_device_query.zcl_power_source_id;
    xSemaphoreGive(gt_lock);
  }
  if (attribute->id == 0xFFE2 && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8 && attribute->data.value) {
    uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
    log_i("Tuya 0xFFE2 attribute report value",value);
    //if (_on_battery_percentage_receive)
    //  _on_battery_percentage_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value / 2);
  }
}

void ZigbeeGateway::bindDeviceCluster(zbg_device_params_t * device,int16_t cluster_id) {

  esp_zb_zdo_bind_req_param_t bind_req;
    
  bind_req.req_dst_addr = device->short_addr;

  /* populate the src information of the binding */
  memcpy(bind_req.src_address, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.src_endp = device->endpoint;
  bind_req.cluster_id = cluster_id; 
  
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  esp_zb_get_long_address(bind_req.dst_address_u.addr_long);
  bind_req.dst_endp = _instance->getEndpoint(); 
    
  device->ZC_binding = false;
  device->cluster_id = cluster_id;

  log_d("Requesting ZED (0x%x), endpoint (0x%x), cluster_id (0x%x) to bind ZC", device->short_addr, device->endpoint, device->cluster_id);

  zbg_device_params_t *bind_device =(zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));
  memcpy(bind_device, device, sizeof(zbg_device_params_t));

  _last_bind_success = false;
  _binding_error_retries = 5;

  while (!_last_bind_success){

    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)bind_device);
    esp_zb_lock_release();
    delay(200);

    if (xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
    log_e("Semaphore timeout while binding");
    }
  }
  free(bind_device);

  bind_req.req_dst_addr = esp_zb_get_short_address();

  esp_zb_get_long_address(bind_req.src_address);
  bind_req.src_endp = _instance->getEndpoint();
  bind_req.cluster_id = cluster_id;
    
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  memcpy(bind_req.dst_address_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.dst_endp = device->endpoint;
    
  /*memcpy(bind_req.src_address, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.src_endp = device->endpoint;
  bind_req.cluster_id = cluster_id; 
  
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  esp_zb_get_long_address(bind_req.dst_address_u.addr_long);
  bind_req.dst_endp = _instance->getEndpoint();*/ 

  device->ZC_binding = true;
  device->cluster_id = cluster_id;

  bind_device =(zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));
  memcpy(bind_device, device, sizeof(zbg_device_params_t));

  log_d("Requesting ZC to bind ZED (0x%x), endpoint (0x%x), cluster_id (0x%x)", device->short_addr, device->endpoint, device->cluster_id);

  _last_bind_success = false;
  _binding_error_retries = 5;

  while (!_last_bind_success) {

    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)bind_device);
    esp_zb_lock_release();
    delay(200);

    if (xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
    log_e("Semaphore timeout while binding");
    }
  }
  free(bind_device);
}

void ZigbeeGateway::zbDeviceAnnce(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) {
  
  zbg_device_params_t *device = (zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));
  device->endpoint = 1;
  device->short_addr = short_addr;
  log_d("zbDeviceAnnce short address (0x%x), ieee_addr (0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x)", short_addr,
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
  memcpy(device->ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t));
  log_d("zbDeviceAnnce joined device short address (0x%x), ieee_addr (0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x)", device->short_addr,
        device->ieee_addr[7], device->ieee_addr[6], device->ieee_addr[5], device->ieee_addr[4], device->ieee_addr[3], device->ieee_addr[2], device->ieee_addr[1], device->ieee_addr[0]);

  esp_zb_zcl_read_attr_cmd_t read_req;

  if (device->short_addr != 0) {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }

  read_req.zcl_basic_cmd.src_endpoint = _endpoint;
  read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  read_req.clusterID = ESP_ZB_ZCL_CLUSTER_ID_BASIC;

  uint16_t attributes[6] = {  ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 
                              ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 
                              0xFFFE};
    
    read_req.attr_number = 6; //ZB_ARRAY_LENTH(attributes);
    read_req.attr_field = &attributes[0];

    log_i("Tuya magic last hope");

    esp_zb_lock_acquire(portMAX_DELAY);
    uint8_t basic_tsn = esp_zb_zcl_read_attr_cmd_req(&read_req);
    esp_zb_lock_release();
    delay(200);

  _new_device_joined = true;
  _instance->_joined_devices.push_back(device);
}

void ZigbeeGateway::zbDeviceLeave(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr, uint8_t rejoin) {
  for (std::list<zbg_device_params_t *>::iterator bound_device = _gateway_devices.begin(); bound_device != _gateway_devices.end(); ++bound_device) {
    if (((*bound_device)->short_addr == short_addr) || (memcmp((*bound_device)->ieee_addr, ieee_addr, 8) == 0)) {
      (*bound_device)->rejoin_after_leave = true;
    }
	}
}

void ZigbeeGateway::findEndpoint(esp_zb_zdo_match_desc_req_param_t *param) {
   
  uint16_t cluster_list[] = {ESP_ZB_ZCL_CLUSTER_ID_BASIC,ESP_ZB_ZCL_CLUSTER_ID_BASIC};
    
  param->profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  param->num_in_clusters = 1;
  param->num_out_clusters = 1;
  param->cluster_list = cluster_list;
  findcb_userdata._endpoint = _endpoint;
  findcb_userdata._cluster_id = ESP_ZB_ZCL_CLUSTER_ID_BASIC;
  
  esp_zb_zdo_match_cluster(param, find_Cb, &findcb_userdata);
}

void ZigbeeGateway::printGatewayDevices() {
  log_i("gateway devices:");
  for ([[maybe_unused]]
       const auto &device : _gateway_devices) {
    log_i("Device on endpoint %d, short address: 0x%x", device->endpoint, device->short_addr);
    //print_ieee_addr(device->ieee_addr);
  }
}

void ZigbeeGateway::printJoinedDevices() {
  log_i("joined devices:");
  for ([[maybe_unused]]
       const auto &device : _joined_devices) {
    log_i("Device on endpoint %d, short address: 0x%x", device->endpoint, device->short_addr);
    //print_ieee_addr(device->ieee_addr);
  }
}

uint32_t ZigbeeGateway::getZbgDeviceUnitLastSeenMs(uint16_t short_addr){

for (uint8_t i = 0; i < ZBG_MAX_DEVICES; i++) {
    if ((zbg_device_units[i].record_id > 0) && (zbg_device_units[i].short_addr == short_addr)) {
      return zbg_device_units[i].last_seen_ms;
    }
  }
  return 0;
}

void ZigbeeGateway::updateZbgDeviceUnitLastSeenMs(uint16_t short_addr) {

//log_i("Inside updateZbgDeviceUnitLastSeenMs with short address 0x%x", short_addr);
for (uint8_t i = 0; i < ZBG_MAX_DEVICES; i++) {
    if ((zbg_device_units[i].record_id > 0) && (zbg_device_units[i].short_addr == short_addr)) {
      zbg_device_units[i].last_seen_ms = millis();
      //log_i("updateZbgDeviceUnitLastSeenMs addr 0x%x vs 0x%x, ms %u",zbg_device_units[i].short_addr, short_addr, zbg_device_units[i].last_seen_ms);
      break;
    }
  }
}

void ZigbeeGateway::zbAttributeReporting(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute, signed char  rssi) {
  
  uint16_t short_addr = src_address.u.short_addr;
  //log_i(" zbAttributeReporting with short address 0x%x", short_addr);
  esp_zb_ieee_address_by_short(short_addr,src_address.u.ieee_addr);
  //log_i("Calling updateZbgDeviceUnitLastSeenMs with short address 0x%x", short_addr);
  _instance->updateZbgDeviceUnitLastSeenMs(short_addr);

  if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_S16) {
      int16_t value = attribute->data.value ? *(int16_t *)attribute->data.value : 0;
      log_i("zbAttributeReporting temperature measurement %f",((float)value)/100);
      if (_on_temperature_receive)
        _on_temperature_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, ((float)value)/100, rssi);
      } else log_i("zbAttributeReporting temperature cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
  if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) 
    {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeReporting humidity measurement %f",((float)value)/100);
      if (_on_humidity_receive)
        _on_humidity_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, ((float)value)/100, rssi);
      } else log_i("zbAttributeReporting humidity cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
  } else 
  if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_S16) 
    {
      int16_t value = attribute->data.value ? *(int16_t *)attribute->data.value : 0;
      log_i("zbAttributeReporting pressure measurement %f",((float)value));
      if (_on_pressure_receive)
        _on_pressure_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, ((float)value), rssi);
      } else log_i("zbAttributeReporting humidity cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
  } else 
  if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeReporting illuminance measurement 0x%x", value);
      if (_on_illuminance_receive)
        _on_illuminance_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else log_i("zbAttributeReporting illuminance measurement cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_8BITMAP) {
      uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
      log_i("zbAttributeReporting occupancy sensing 0x%x", value);
      if (_on_occupancy_receive)
        _on_occupancy_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else log_i("zbAttributeReporting occupancy sensing cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && ((attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) || (attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8)))
      {
        bool value = attribute->data.value ? *(bool *)attribute->data.value : 0;
        log_i("zbAttributeReporting on/off report %s",value ? "ON" : "OFF");
        if (_on_on_off_receive)
          _on_on_off_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else log_i("zbAttributeReporting on/off cluster (0x%x), attribute id (0x%x), attribute data type (0x%x), attribute value (0x%x)", 
                    cluster_id, attribute->id, attribute->data.type, *(uint8_t *)attribute->data.value);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
        uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value :  0;
        log_i("zbAttributeReporting electrical measurement RMS voltage %d",value);
        if (_on_rms_voltage_receive)
          _on_rms_voltage_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else
      if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
        uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting electrical measurement RMS current %d",value);
        if (_on_rms_current_receive)
          _on_rms_current_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else
      if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID) { //&& attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
        uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting electrical measurement RMS active power %d, data type 0x%x",value, attribute->data.type);
        if (_on_rms_active_power_receive)
          _on_rms_active_power_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else log_i("zbAttributeReporting electrical measurement cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_METERING) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U48) {
        esp_zb_uint48_t *value; 
        if (attribute->data.value) value = (esp_zb_uint48_t *)attribute->data.value ;
        log_i("zbAttributeReporting metering cluster current summation delivered %d:%d",value->high, value->low);
        uint64_t value64 = (((uint64_t)value->high) << 32) + value->low;
        if (_on_current_summation_receive)
          _on_current_summation_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value64, rssi);
      } else log_i("zbAttributeReporting metering cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) { 
        uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting level control cluster current level 0x%x",value);
        if (_on_current_level_receive)
          _on_current_level_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else log_i("zbAttributeReporting level control cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) {
        uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting color control cluster hue 0x%x",value);
        if (_on_color_hue_receive)
          _on_color_hue_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else 
      if (attribute->id == ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) {
        uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting color control cluster saturation 0x%x",value);
        if (_on_color_saturation_receive)
          _on_color_saturation_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
      } else
      if (attribute->id == ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
        uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting color control cluster saturation 0x%x",value);
        if (_on_color_temperature_receive)
          _on_color_temperature_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);

      } else log_i("zbAttributeReporting color control cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) 
      {
        uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting power config battery percentage remaining %d",value);
        if (_on_battery_percentage_receive)
          _on_battery_percentage_receive(src_address.u.ieee_addr, src_endpoint,cluster_id, value /2);
      } else
      if (attribute->id == ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) 
      {
        uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting power config battery voltage %d",value);
        if (_on_battery_percentage_receive)
          _on_battery_percentage_receive(src_address.u.ieee_addr, src_endpoint,cluster_id, 100 - ((33 - value)*20));
      }
      else log_i("zbAttributeReporting power config cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_16BITMAP) {
        uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting IAS zone status %d",value);
        if (_on_IAS_zone_status_change_notification)
          _on_IAS_zone_status_change_notification(src_address.u.ieee_addr, src_endpoint, cluster_id, value, rssi);
        } else log_i("zbAttributeReporting IAS zone cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
      } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT) {
      if (attribute->id == ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_S16) {
        int16_t value = attribute->data.value ? *(int16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting thermostat local temperature %d",value);
        if (_on_thermostat_temperatures_receive)
          _on_thermostat_temperatures_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, attribute->id, value, rssi);
        } else
        if (attribute->id == ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_S16) {
        int16_t value = attribute->data.value ? *(int16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting thermostat occupied heating setpoint %d",value);
        if (_on_thermostat_temperatures_receive)
          _on_thermostat_temperatures_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, attribute->id, value, rssi);
        } else
        if (attribute->id == ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_S8) {
        int8_t value = attribute->data.value ? *(int8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting thermostat local temperature calibration %d",value);
        if (_on_thermostat_temperatures_receive)
          _on_thermostat_temperatures_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, attribute->id, value, rssi);
        } else
        if (attribute->id == ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM) {
        uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting thermostat system mode %d",value);
        if (_on_thermostat_modes_receive)
          _on_thermostat_modes_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, attribute->id, value, rssi);
        } else
        if (attribute->id == ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_16BITMAP) {
        uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
        log_i("zbAttributeReporting thermostat running state %d",value);
        if (_on_thermostat_modes_receive)
          _on_thermostat_modes_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, attribute->id, value, rssi);
        } else log_i("zbAttributeReporting thermostat cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
      } else log_i("zbAttributeReporting from (0x%x), endpoint (%d), cluster (0x%x), attribute id (0x%x), attribute data type (0x%x) ", 
        src_address.u.short_addr, src_endpoint, cluster_id, attribute->id, attribute->data.type);
}

void ZigbeeGateway::zbReadAttrResponse(uint8_t tsn, esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute, signed char  rssi) {
  
  //esp_zb_ieee_address_by_short(src_address.u.short_addr,src_address.u.ieee_addr);
  if ((_read_attr_tsn_list[tsn] == ZCL_CMD_TSN_SYNC) && (tsn == _read_attr_last_tsn))
  {
    updateZbgDeviceUnitLastSeenMs(src_address.u.short_addr);
    log_i("zbReadAttrResponse sync read, tsn matched");
    memcpy(&_read_attr_last_result, attribute, sizeof(const esp_zb_zcl_attribute_t));
    log_i("check 0x%x vs 0x%x", _read_attr_last_result.id, attribute->id);
    delay(200);
    _read_attr_tsn_list[tsn] = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  }
  else 
  {
    log_i("zbReadAttrResponse async read, tsn 0x%x[0x%x]", tsn, _read_attr_tsn_list[tsn]);
    zbAttributeReporting(src_address, src_endpoint, cluster_id, attribute, rssi);
  }
  //zbAttributeReporting(src_address, src_endpoint, cluster_id, attribute);
  
}

void ZigbeeGateway::zbIASZoneEnrollRequest(const esp_zb_zcl_ias_zone_enroll_request_message_t *message) {
  
  esp_zb_zcl_ias_zone_enroll_response_cmd_t enroll_resp_req;

  enroll_resp_req.zcl_basic_cmd.dst_endpoint = message->info.src_endpoint;
  enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_short = message->info.src_address.u.short_addr;
  //memcpy(enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  enroll_resp_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  enroll_resp_req.zcl_basic_cmd.src_endpoint = _endpoint;

  enroll_resp_req.enroll_rsp_code = ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS;
  enroll_resp_req.zone_id = 1;

  log_i("Sending 'ias zone enroll resp' command");
  //esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_ias_zone_enroll_cmd_resp(&enroll_resp_req);
  //esp_zb_lock_release();
  //delay(200);
}

void ZigbeeGateway::zbIASZoneStatusChangeNotification(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {

  esp_zb_zcl_cmd_info_t info = message->info;

  updateZbgDeviceUnitLastSeenMs(info.src_address.u.short_addr);

  esp_zb_ieee_address_by_short(info.src_address.u.short_addr, info.src_address.u.ieee_addr);
  

  if (_on_IAS_zone_status_change_notification)
    _on_IAS_zone_status_change_notification(info.src_address.u.ieee_addr, info.src_endpoint, message->info.cluster, message->zone_status, message->info.header.rssi);  
}

void ZigbeeGateway::zbCmdDiscAttrResponse(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                                          const esp_zb_zcl_disc_attr_variable_t *variable) {
  
  if (variable) {
  log_i("Attribute Discovery Message - device address (0x%x), source endpoint (0x%x), source cluster (0x%x), attribute id (0x%x), data type (0x%x)", 
        src_address.u.short_addr, src_endpoint, cluster_id, variable->attr_id, variable->data_type);
  }
}

void ZigbeeGateway::addBoundDevice(zb_device_params_t *device, uint16_t cluster_id) {
    
    zbg_device_params_t *zbg_device = (zbg_device_params_t *)calloc(1, sizeof(zbg_device_params_t));
    zbg_device->short_addr = device->short_addr;
    memcpy(zbg_device->ieee_addr, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    zbg_device->cluster_id = cluster_id;
    zbg_device->endpoint = device->endpoint;

    free(device);

    if (zbg_device->short_addr == 0) 
      zbg_device->short_addr = esp_zb_address_short_by_ieee(zbg_device->ieee_addr);
    
    zbg_device->model_id = 0x0000;
    zbg_device->rejoined = false;
    zbg_device->rejoin_after_leave = false;
    
    if (_on_btc_bound_device)
      _on_btc_bound_device (zbg_device);

    _gateway_devices.push_back(zbg_device);

    uint8_t first_free_record = 0xFF;
    bool zgb_device_unit_found = false;

    for (uint8_t i = 0; i < ZBG_MAX_DEVICES; i ++) {
      
      if ((zbg_device_units[i].record_id == 0) && (i < first_free_record))
        first_free_record = i;

      if ((zbg_device_units[i].record_id > 0) &&
           (memcmp(zbg_device_units[i].ieee_addr, zbg_device->ieee_addr, sizeof(esp_zb_ieee_addr_t)) == 0)) {

        log_i("Zigbee device unit already registered, last short address 0x%x, current short address 0x%x", zbg_device_units[i].short_addr, zbg_device->short_addr);
        zgb_device_unit_found = true;
        if (zbg_device_units[i].short_addr != zbg_device->short_addr)
          zbg_device_units[i].short_addr = zbg_device->short_addr;
        break;
      }
    }
    if (!zgb_device_unit_found) {
      if (first_free_record < ZBG_MAX_DEVICES) {
        zbg_device_units[first_free_record].record_id = 1;
        memcpy(zbg_device_units[first_free_record].ieee_addr, zbg_device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
        zbg_device_units[first_free_record].short_addr = zbg_device->short_addr;
        zbg_device_units[first_free_record].model_id = zbg_device->model_id;
        zbg_device_units[first_free_record].last_seen_ms = 0; //millis();
        log_i("New Zigbee device unit registered - IEEE address %x:%x:%x:%x:%x:%x:%x:%x, short address 0x%x, model id 0x%x, last seen (ms) %lu",
              zbg_device_units[first_free_record].ieee_addr[7], zbg_device_units[first_free_record].ieee_addr[6], zbg_device_units[first_free_record].ieee_addr[5],
              zbg_device_units[first_free_record].ieee_addr[4], zbg_device_units[first_free_record].ieee_addr[3], zbg_device_units[first_free_record].ieee_addr[2],
              zbg_device_units[first_free_record].ieee_addr[1], zbg_device_units[first_free_record].ieee_addr[0], zbg_device_units[first_free_record].short_addr,
              zbg_device_units[first_free_record].model_id, zbg_device_units[first_free_record].last_seen_ms);
      } else
        log_e("Zigbee device units table full - can't register new one!");
    }
}

bool ZigbeeGateway::isDeviceBound(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) {

	for (std::list<zbg_device_params_t *>::iterator bound_device = _gateway_devices.begin(); bound_device != _gateway_devices.end(); ++bound_device) {
              if (((*bound_device)->short_addr == short_addr) || (memcmp((*bound_device)->ieee_addr, ieee_addr, 8) == 0)) {
                (*bound_device)->rejoined = true;
                if ((*bound_device)->rejoin_after_leave)
                  return false;
                else
                return true;
              }
	}
	return false;
		
}

void ZigbeeGateway::setClusterReporting(zbg_device_params_t * device, uint16_t cluster_id, uint16_t attribute_id, uint8_t attribute_type,
                                        uint16_t min_interval, uint16_t max_interval, uint16_t delta, bool ack) {
  
  esp_zb_zcl_config_report_cmd_t report_cmd;
  
  if (device->short_addr != 0) {
      report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  //report_cmd.dis_defalut_resp = 0;   
  report_cmd.zcl_basic_cmd.dst_endpoint = device->endpoint;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint;
  report_cmd.clusterID = cluster_id;

  int16_t report_change = delta;
  esp_zb_zcl_config_report_record_t records[1];// = {
    //{
      records[0].direction = ESP_ZB_ZCL_REPORT_DIRECTION_SEND, //0x00, //ESP_ZB_ZCL_REPORT_DIRECTION_SEND,
      records[0].attributeID = attribute_id,
      records[0].attrType = attribute_type, //ESP_ZB_ZCL_ATTR_TYPE_S16,
      records[0].min_interval = min_interval,
      records[0].max_interval = max_interval,
      records[0].reportable_change = &report_change,
    //}
  //};
  report_cmd.record_number = 1;//ZB_ARRAY_LENTH(records);
  report_cmd.record_field = &records[0];

  report_cmd.manuf_specific = 0;
  report_cmd.dis_defalut_resp = 0;
  report_cmd.direction = 0;
  report_cmd.manuf_code = 0;


  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_config_report_cmd_req(&report_cmd);
  esp_zb_lock_release();
  delay(200);

  if (ack && xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Semaphore timeout configuring attribute reporting 0x%x - device 0x%x, endpoint 0x%x, cluster 0x%x", attribute_id, device->short_addr, device->endpoint, cluster_id);
    }
}

void ZigbeeGateway::readClusterReportCmd(zbg_device_params_t * device, uint16_t cluster_id, uint16_t attribute_id, bool ack) {
  
  esp_zb_zcl_report_attr_cmd_t report_cmd;

  if (device->short_addr != 0) {
      report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  //report_cmd.dis_defalut_resp = 0;   
  report_cmd.zcl_basic_cmd.dst_endpoint = device->endpoint;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint;
  report_cmd.clusterID = cluster_id;
  report_cmd.attributeID = attribute_id;

  report_cmd.manuf_specific = 0;
  report_cmd.dis_defalut_resp = 0;
  report_cmd.direction = 0;
  report_cmd.manuf_code = 0;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_report_attr_cmd_req(&report_cmd);
  esp_zb_lock_release();
  delay(200);

  if (ack && xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Semaphore timeout read attribute report 0x%x - device 0x%x, endpoint 0x%x, cluster 0x%x", attribute_id, device->short_addr, device->endpoint, cluster_id);
    }
}

void ZigbeeGateway::readClusterReportCfgCmd(zbg_device_params_t * device, uint16_t cluster_id, uint16_t attribute_id, bool ack) {
  
  esp_zb_zcl_read_report_config_cmd_t report_cmd;
  
  if (device->short_addr != 0) {
      report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  //report_cmd.dis_defalut_resp = 0;   
  report_cmd.zcl_basic_cmd.dst_endpoint = device->endpoint;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint;
  report_cmd.clusterID = cluster_id;

  report_cmd.manuf_specific = 0;
  report_cmd.dis_defalut_resp = 0;
  report_cmd.direction = 0;
  report_cmd.manuf_code = 0;

  esp_zb_zcl_attribute_record_t records[1];
  
  report_cmd.record_field = &records[0];
  report_cmd.record_number = 1;

  records[0].report_direction = 0x0;
  records[0].attributeID = attribute_id;

  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_read_report_config_cmd_req(&report_cmd);
  esp_zb_lock_release();
  delay(200);

  if (ack && xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Semaphore timeout read attribute report 0x%x - device 0x%x, endpoint 0x%x, cluster 0x%x", attribute_id, device->short_addr, device->endpoint, cluster_id);
    }
}

void ZigbeeGateway::zbConfigReportResponse(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, esp_zb_zcl_status_t status, uint8_t direction, 
                             uint16_t attribute_id) {

  updateZbgDeviceUnitLastSeenMs(src_address.u.short_addr);
  xSemaphoreGive(gt_lock);
}


bool ZigbeeGateway::sendAttributeRead(zbg_device_params_t * device, int16_t cluster_id, uint16_t attribute_id, bool ack) {

    esp_zb_zcl_read_attr_cmd_t read_req;

    if (device->short_addr != 0) {
      read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      //else read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;
      read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    }

    read_req.zcl_basic_cmd.src_endpoint = _endpoint;
    read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

    read_req.clusterID = cluster_id;

    uint16_t attributes[1] = {attribute_id};
    read_req.attr_number = 1; //ZB_ARRAY_LENTH(attributes);
    read_req.attr_field = &attributes[0];

    read_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
    read_req.manuf_specific = 0;
    read_req.dis_defalut_resp = 1;

    log_i("Sending 'read attribute' command");
    esp_zb_lock_acquire(portMAX_DELAY);
    _read_attr_last_tsn = esp_zb_zcl_read_attr_cmd_req(&read_req);
    esp_zb_lock_release();
    
    if (ack) 
      _read_attr_tsn_list[_read_attr_last_tsn] = ZCL_CMD_TSN_SYNC;
    else 
      _read_attr_tsn_list[_read_attr_last_tsn] = ZCL_CMD_TSN_ASYNC;
    
    delay(200);

    if (ack && xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
      log_e("Semaphore timeout reading attribute 0x%x - device 0x%x, endpoint 0x%x, cluster 0x%x", attribute_id, device->short_addr, device->endpoint, cluster_id);
      return false;
    } else return true;
}

void ZigbeeGateway::sendAttributesRead(zbg_device_params_t * device, int16_t cluster_id, uint8_t attr_number, uint16_t *attribute_ids) {
  
  esp_zb_zcl_read_attr_cmd_t read_req;

  if (device->short_addr != 0) {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    //else read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT;
    read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }

  read_req.zcl_basic_cmd.src_endpoint = _endpoint;
  read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

  read_req.clusterID = cluster_id;

  read_req.attr_number = attr_number; //1; //ZB_ARRAY_LENTH(attributes);
  read_req.attr_field = attribute_ids;

  read_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  read_req.manuf_specific = 0;
  read_req.dis_defalut_resp = 1;

  log_i("Sending 'read attribute' command");
  esp_zb_lock_acquire(portMAX_DELAY);
    _read_attr_last_tsn = esp_zb_zcl_read_attr_cmd_req(&read_req);
  esp_zb_lock_release();
  
  _read_attr_tsn_list[_read_attr_last_tsn] = ZCL_CMD_TSN_ASYNC;
    
  delay(200);
}

void ZigbeeGateway::sendAttributeWrite( zbg_device_params_t * device, int16_t cluster_id, uint16_t attribute_id,
                                        esp_zb_zcl_attr_type_t attribute_type, uint16_t attribute_size, void *attribute_value, uint8_t manuf_specific, uint16_t manuf_code) {

    esp_zb_zcl_write_attr_cmd_t write_req;
    esp_zb_zcl_attribute_t attribute_field[1];
    esp_zb_zcl_attribute_data_t attribute_data;

    if (device->short_addr != 0) {
      write_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      write_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      write_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(write_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    }

    write_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
    write_req.zcl_basic_cmd.src_endpoint = _endpoint;
    write_req.clusterID = cluster_id;
    write_req.attr_number = 1;
    write_req.attr_field = &attribute_field[0];

    attribute_field[0].id = attribute_id;
    attribute_field[0].data.type = attribute_type;
    attribute_field[0].data.size = attribute_size;
    attribute_field[0].data.value = attribute_value;

    write_req.manuf_specific = manuf_specific;
    write_req.dis_defalut_resp = 0;
    write_req.direction = 0;
    write_req.manuf_code = manuf_code;

    log_i("Sending 'write attribute' command - id (0x%x), type (0x%x), size (0x%x), value (0x%x)",
    (*((esp_zb_zcl_attribute_t*)write_req.attr_field)).id, (*((esp_zb_zcl_attribute_t*)write_req.attr_field)).data.type,
    (*((esp_zb_zcl_attribute_t*)write_req.attr_field)).data.size,*((uint8_t*)((*((esp_zb_zcl_attribute_t*)write_req.attr_field)).data.value)));
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_write_attr_cmd_req(&write_req);
    esp_zb_lock_release();
    delay(500);
}

void ZigbeeGateway::sendIASzoneEnrollResponseCmd(zbg_device_params_t *device, uint8_t enroll_rsp_code, uint8_t zone_id){

  esp_zb_zcl_ias_zone_enroll_response_cmd_t enroll_resp_req;

  enroll_resp_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  memcpy(enroll_resp_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  enroll_resp_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  enroll_resp_req.zcl_basic_cmd.src_endpoint = _endpoint;

  enroll_resp_req.enroll_rsp_code = enroll_rsp_code; //ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS;
  enroll_resp_req.zone_id = zone_id;

  log_i("Sending 'ias zone enroll resp' command");
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_ias_zone_enroll_cmd_resp(&enroll_resp_req);
  esp_zb_lock_release();
  delay(200);  
}


void ZigbeeGateway::sendOnOffCmd(zbg_device_params_t *device, bool value) {

    esp_zb_zcl_on_off_cmd_t cmd_req;
    
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
    delay(200);
}

void ZigbeeGateway::sendLevelMoveToLevelCmd(zbg_device_params_t *device, uint8_t level, uint16_t transition_time) {
  
    esp_zb_zcl_move_to_level_cmd_t cmd_req;

    if (device->short_addr != 0) {
      cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    }
    cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
    cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

    cmd_req.level = level;
    cmd_req.transition_time = transition_time;
    
    /*else if (command_id == ESP_ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_WITH_ON_OFF) {
      esp_zb_zcl_level_move_cmd_t cmd_req;
    else if (command_id == ESP_ZB_ZCL_CMD_LEVEL_CONTROL_STEP_WITH_ON_OFF) { 
      esp_zb_zcl_level_step_cmd_t cmd_req;*/

    esp_zb_lock_acquire(portMAX_DELAY);
    //esp_zb_zcl_level_move_to_level_with_onoff_cmd_req(&cmd_req);
    esp_zb_zcl_level_move_to_level_cmd_req(&cmd_req);
    esp_zb_lock_release();
    delay(200);


}

void ZigbeeGateway::sendColorMoveToHueCmd(zbg_device_params_t *device, uint8_t hue, uint8_t direction, uint16_t transition_time) {

    esp_zb_zcl_color_move_to_hue_cmd_t cmd_req;

    if (device->short_addr != 0) {
      cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    }
    cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
    cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

    cmd_req.hue = hue;
    cmd_req.direction = direction;
    cmd_req.transition_time = transition_time;
    
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_color_move_to_hue_cmd_req(&cmd_req);
    esp_zb_lock_release();
    delay(200);
}

void ZigbeeGateway::sendColorMoveToSaturationCmd(zbg_device_params_t *device, uint8_t saturation, uint16_t transition_time) {

    esp_zb_zcl_color_move_to_saturation_cmd_t cmd_req;

    if (device->short_addr != 0) {
      cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    }
    cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
    cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

    cmd_req.saturation = saturation;
    cmd_req.transition_time = transition_time;
    
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_color_move_to_saturation_cmd_req(&cmd_req);
    esp_zb_lock_release();
    delay(200);
} 

void ZigbeeGateway::sendColorMoveToHueAndSaturationCmd(zbg_device_params_t *device, uint8_t hue, uint8_t saturation, uint16_t transition_time) {
  
  esp_zb_color_move_to_hue_saturation_cmd_t cmd_req;

  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

  cmd_req.hue = hue;
  cmd_req.saturation = saturation;
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_hue_and_saturation_cmd_req(&cmd_req);
  esp_zb_lock_release();
  delay(200);
}

void ZigbeeGateway::sendColorEnhancedMoveToHueAndSaturationCmd(zbg_device_params_t *device, uint16_t enhanced_hue, uint8_t saturation, uint16_t transition_time) {
  
  esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_t cmd_req;

  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

  cmd_req.enhanced_hue = enhanced_hue;
  cmd_req.saturation = saturation;
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_req(&cmd_req);
  esp_zb_lock_release();
  delay(200);
}

void ZigbeeGateway::sendColorMoveToColorCmd(zbg_device_params_t *device, uint16_t color_x, uint16_t color_y, uint16_t transition_time) {

  esp_zb_zcl_color_move_to_color_cmd_t cmd_req;

  if (device->short_addr != 0) {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  } else {
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  }
  cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
  cmd_req.zcl_basic_cmd.dst_endpoint = device->endpoint;

  cmd_req.color_x = color_x;
  cmd_req.color_y = color_y;
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_color_cmd_req(&cmd_req);
  esp_zb_lock_release();
  delay(200);
} 

void ZigbeeGateway::sendColorMoveToColorTemperatureCmd(zbg_device_params_t *device, uint16_t color_temperature, uint16_t transition_time) {
  
  esp_zb_zcl_color_move_to_color_temperature_cmd_t cmd_req;

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
  cmd_req.transition_time = transition_time;
    
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_color_move_to_color_temperature_cmd_req(&cmd_req);
  esp_zb_lock_release();
  delay(200);
}

void ZigbeeGateway::sendDeviceFactoryReset(zbg_device_params_t *device, bool isTuya) {

    /*if (isTuya) {
      uint8_t tuya_dp_data[15];
      memset(tuya_dp_data, 0, sizeof(tuya_dp_data));
      sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_1, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_U32, 4,  tuya_dp_data);

          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x11;
          tuya_dp_data[3] = 0x02;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x04;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 0x00;
                sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_1, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_SET, 10,  tuya_dp_data);

          tuya_dp_data[0] = 0x11;
          tuya_dp_data[1] = 0x02;
          tuya_dp_data[2] = 0x00;
          tuya_dp_data[3] = 0x04;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x00;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
                sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_1, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_SET, 8,  tuya_dp_data);
                      memset(tuya_dp_data, 0, sizeof(tuya_dp_data));
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x11;
          tuya_dp_data[3] = 0x02;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x09;
                sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_1, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_SET, 15,  tuya_dp_data);
          tuya_dp_data[0] = 0x11;
          tuya_dp_data[1] = 0x02;
          tuya_dp_data[2] = 0x00;
          tuya_dp_data[3] = 0x09;
          
                sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_1, 0xE1, ESP_ZB_ZCL_ATTR_TYPE_SET, 13,  tuya_dp_data);
    
    }
    else {
      sendCustomClusterCmd(device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);*/
  esp_zb_zcl_basic_fact_reset_cmd_t cmd_req;
    
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
  delay(500); 
}


void ZigbeeGateway::zbCmdDefaultResponse( uint8_t tsn, int8_t rssi, esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, uint8_t resp_to_cmd, esp_zb_zcl_status_t status_code) {
  //if ((cluster_id == TUYA_PRIVATE_CLUSTER_EF00) && (resp_to_cmd = 0x00))
    //xSemaphoreGive(gt_lock);

  updateZbgDeviceUnitLastSeenMs(src_address.u.short_addr);

  if ((_custom_cmd_tsn_list[tsn] == ZCL_CMD_TSN_SYNC) && (tsn == _custom_cmd_last_tsn))
  {
    log_i("zbCustomCmd default response sync, tsn matched");
    //memcpy(&_read_attr_last_result, attribute, sizeof(const esp_zb_zcl_attribute_t));
    //log_i("check 0x%x vs 0x%x", _read_attr_last_result.id, attribute->id);
    delay(200);
    _custom_cmd_tsn_list[tsn] = ZCL_CMD_TSN_UNKNOWN;
    xSemaphoreGive(gt_lock);  
  }
  else 
  {
    log_i("zbCustomCmd default response async, tsn 0x%x[0x%x]", tsn, _custom_cmd_tsn_list[tsn]);
  }
  log_i("custom cmd rssi %d", rssi);
}

void ZigbeeGateway::sendCustomClusterCmd( zbg_device_params_t * device, int16_t custom_cluster_id, uint16_t custom_command_id, esp_zb_zcl_attr_type_t data_type, 
                                          uint16_t custom_data_size, uint8_t *custom_data, bool ack, uint8_t direction ) {
  
  esp_zb_zcl_custom_cluster_cmd_req_t req;

  if (device->short_addr != 0) {
      req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    } else {
      req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
      memcpy(req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    }
  req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  req.zcl_basic_cmd.src_endpoint = _endpoint;
  req.cluster_id = custom_cluster_id;
  req.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  req.direction = direction;
  req.manuf_specific = 0;
  req.dis_defalut_resp = 0;
  req.manuf_code = 0;
  req.custom_cmd_id = custom_command_id;
  req.data.type = data_type; //ESP_ZB_ZCL_ATTR_TYPE_U8;//ESP_ZB_ZCL_ATTR_TYPE_SET;
  req.data.size = custom_data_size;
  req.data.value = custom_data;
  
  esp_zb_lock_acquire(portMAX_DELAY);
  _custom_cmd_last_tsn = esp_zb_zcl_custom_cluster_cmd_req(&req);
  esp_zb_lock_release();
  
  delay(200);

  if (ack) 
    _custom_cmd_tsn_list[_custom_cmd_last_tsn] = ZCL_CMD_TSN_SYNC;
  else 
    _custom_cmd_tsn_list[_custom_cmd_last_tsn] = ZCL_CMD_TSN_ASYNC;
  delay(200);

  if (ack && xSemaphoreTake(gt_lock, pdMS_TO_TICKS(2000)) != pdTRUE) {
  //if (ack && xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
    log_e("Semaphore timeout while sending custom command");
  }  
}

void ZigbeeGateway::zbCmdCustomClusterReq(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id,uint8_t command_id, uint16_t payload_size, uint8_t *payload) {

  updateZbgDeviceUnitLastSeenMs(src_address.u.short_addr);

  esp_zb_zcl_cmd_info_t info;
  esp_zb_ieee_address_by_short(src_address.u.short_addr, info.src_address.u.ieee_addr);


  if (_on_cmd_custom_cluster_receive)
    _on_cmd_custom_cluster_receive(info.src_address.u.ieee_addr, src_endpoint, cluster_id, command_id, payload_size, payload, info.header.rssi);
}

bool ZigbeeGateway::zbRawCmdHandler( esp_zb_zcl_addr_t source, uint8_t src_endpoint, uint8_t dst_endpoint, uint16_t cluster_id, uint8_t cmd_id, 
                                bool is_common_command, bool disable_default_response, bool is_manuf_specific, uint16_t manuf_specific,
                                uint8_t buffer_size, uint8_t *buffer, signed char  rssi) {
  
    updateZbgDeviceUnitLastSeenMs(source.u.short_addr);

    if (_on_custom_cmd_receive)
      return _on_custom_cmd_receive(source.u.ieee_addr, src_endpoint, cluster_id, cmd_id, buffer_size, buffer, rssi);
    else return false;
}

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
