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
  
  esp_zb_on_off_cluster_cfg_t on_off_cluster;
  on_off_cluster.on_off = ESP_ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE;

  esp_zb_time_cluster_cfg_t time_cluster;
  time_cluster.time = ESP_ZB_ZCL_TIME_TIME_DEFAULT_VALUE;
  time_cluster.time_status = ESP_ZB_ZCL_TIME_TIME_STATUS_DEFAULT_VALUE;

  _cluster_list = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&(gateway_cfg.basic_cfg));
  esp_zb_cluster_list_add_basic_cluster(_cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_power_config_cluster(_cluster_list, esp_zb_power_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_scenes_cluster(_cluster_list, esp_zb_scenes_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_groups_cluster(_cluster_list, esp_zb_groups_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);

  esp_zb_cluster_list_add_time_cluster(_cluster_list, esp_zb_time_cluster_create(&time_cluster), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

  esp_zb_cluster_list_add_ias_zone_cluster(_cluster_list, esp_zb_ias_zone_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_temperature_meas_cluster(_cluster_list, esp_zb_temperature_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_humidity_meas_cluster(_cluster_list, esp_zb_humidity_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(&on_off_cluster), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_switch_config_cluster(_cluster_list, esp_zb_on_off_switch_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_electrical_meas_cluster(_cluster_list, esp_zb_electrical_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_metering_cluster(_cluster_list, esp_zb_metering_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_illuminance_meas_cluster(_cluster_list, esp_zb_illuminance_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_thermostat_cluster(_cluster_list, esp_zb_thermostat_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  
  //esp_zb_cluster_list_add_custom_cluster(_cluster_list, &poll_cluster,ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_0),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_1),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_custom_cluster(_cluster_list, esp_zb_zcl_attr_list_create(TUYA_PRIVATE_CLUSTER_EF00),ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  
  _ep_config = {.endpoint = _endpoint, .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, .app_device_id = ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID, .app_device_version = 0};
}

void ZigbeeGateway::bindCb(esp_zb_zdp_status_t zdo_status, void *user_ctx) {
  
  
  zb_device_params_t *device = (zb_device_params_t *)user_ctx;

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
  /*if (_instance->_clusters_2_bind > 0) --_instance->_clusters_2_bind;
  if (_instance->_clusters_2_bind == 0)
    {
      _instance->_gateway_devices.push_back(device);
      if(_instance->_on_bound_device)
        _instance->_on_bound_device (device, true);
    }*/
}

void ZigbeeGateway::find_Cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx) {
  
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    
    
    esp_zb_zdo_bind_req_param_t bind_req;
    
    zb_device_params_t *sensor = (zb_device_params_t *)malloc(sizeof(zb_device_params_t));
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
        esp_zb_zdo_simple_desc_req(&cl_cmd_req, Z2S_simple_desc_req_cb, user_ctx);
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

void ZigbeeGateway::zbPrintDeviceDiscovery (zb_device_params_t * device) {

  
  esp_zb_zdo_active_ep_req_param_t ep_cmd_req;
  
  short_addr_req = device->short_addr; 
  ep_cmd_req.addr_of_interest = short_addr_req; 

  log_i("short_addr_req %d", short_addr_req);      
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zdo_active_ep_req(&ep_cmd_req, Z2S_active_ep_req_cb, &short_addr_req);
  esp_zb_lock_release();
}

void ZigbeeGateway::bindDeviceCluster(zb_device_params_t * device,int16_t cluster_id) {

  /*if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE) {
    esp_zb_ieee_addr_t addr;
    esp_zb_get_long_address(addr);
    _instance->sendAttributeWrite(device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                       ESP_ZB_ZCL_ATTR_TYPE_U64,sizeof(esp_zb_ieee_addr_t),addr);
    _instance->sendIASzoneEnrollResponseCmd(device, ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS, 120);
  }*/
  {
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

    zb_device_params_t *bind_device =(zb_device_params_t *)malloc(sizeof(zb_device_params_t));
    memcpy(bind_device, device, sizeof(zb_device_params_t));

    _last_bind_success = false;
    _binding_error_retries = 5;

    while (!_last_bind_success){

      esp_zb_lock_acquire(portMAX_DELAY);
      esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)bind_device);
      esp_zb_lock_release();
    
      if (xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Semaphore error while binding");
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
    
    device->ZC_binding = true;
    device->cluster_id = cluster_id;

    bind_device =(zb_device_params_t *)malloc(sizeof(zb_device_params_t));
    memcpy(bind_device, device, sizeof(zb_device_params_t));

    log_d("Requesting ZC to bind ZED (0x%x), endpoint (0x%x), cluster_id (0x%x)", device->short_addr, device->endpoint, device->cluster_id);

    _last_bind_success = false;
    _binding_error_retries = 5;

    while (!_last_bind_success) {

      esp_zb_lock_acquire(portMAX_DELAY);
      esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)bind_device);
      esp_zb_lock_release();
    
      if (xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Semaphore error while binding");
      }
    }
    free(bind_device);
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

void ZigbeeGateway::zbAttributeRead(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute) {
  
  esp_zb_ieee_address_by_short(src_address.u.short_addr,src_address.u.ieee_addr);

  if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_S16) {
      int16_t value = attribute->data.value ? *(int16_t *)attribute->data.value : 0;
      //log_i("zbAttributeRead temperature measurement %f",((float)value)/100);
      //log_i("zbAttributeRead %d:%d:%d:%d:%d:%d:%d:%d, endpoint 0x%x", src_address.u.ieee_addr[7], src_address.u.ieee_addr[6], src_address.u.ieee_addr[5], 
      //src_address.u.ieee_addr[4], src_address.u.ieee_addr[3],src_address.u.ieee_addr[2], src_address.u.ieee_addr[1], src_address.u.ieee_addr[0], src_endpoint);
      if (_on_temperature_receive)
        _on_temperature_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, ((float)value)/100);
      } else log_i("zbAttributeRead temperature cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
  if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) 
    {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead humidity measurement %f",((float)value)/100);
      if (_on_humidity_receive)
        _on_humidity_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, ((float)value)/100);
      } else log_i("zbAttributeRead humidity cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) 
    {
      bool value = attribute->data.value ? *(bool *)attribute->data.value : 0;
      log_i("zbAttributeRead on/off report %s",value ? "ON" : "OFF");
      if (_on_on_off_receive)
        _on_on_off_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value);
      } else log_i("zbAttributeRead on/off cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value :  0;
      log_i("zbAttributeRead electrical measurement RMS voltage %d",value);
      if (_on_rms_voltage_receive)
        _on_rms_voltage_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value);
      } else
      if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead electrical measurement RMS current %d",value);
      if (_on_rms_current_receive)
        _on_rms_current_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value);
      } else
      if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead electrical measurement RMS active power %d",value);
      if (_on_rms_active_power_receive)
        _on_rms_active_power_receive(src_address.u.ieee_addr, src_endpoint, cluster_id, value);
      } else log_i("zbAttributeRead electrical measurement cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_METERING) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) 
    {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead electrical measurement RMS voltage %d",value);
      //if (_on_on_off_receive)
        //_on_on_off_receive(src_address.u.ieee_addr, value);
      } else log_i("zbAttributeRead metering cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG) {
    if (attribute->id == 0x0021 /*ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID*/ && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U8) 
    {
      uint8_t value = attribute->data.value ? *(uint8_t *)attribute->data.value : 0;
      log_i("zbAttributeRead power config battery percentage remaining %d",value);
      if (_on_battery_percentage_receive)
        _on_battery_percentage_receive(src_address.u.ieee_addr, src_endpoint,cluster_id, value /2);
      } else log_i("zbAttributeRead power config cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    }
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID /*ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID*/ 
        && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_16BITMAP) {

      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead IAS zone status %d",value);
      if (_on_IAS_zone_status_change_notification)
        _on_IAS_zone_status_change_notification(src_address.u.ieee_addr, src_endpoint, cluster_id, value);
      } else log_i("zbAttributeRead IAS zone cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else log_i("zbAttributeRead from (0x%x), endpoint (%d), cluster (0x%x), attribute id (0x%x), attribute data type (0x%x) ", 
        src_address.u.short_addr, src_endpoint, cluster_id, attribute->id, attribute->data.type);
}

void ZigbeeGateway::zbIASZoneStatusChangeNotification(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {

  esp_zb_zcl_cmd_info_t info = message->info;
  esp_zb_ieee_address_by_short(info.src_address.u.short_addr, info.src_address.u.ieee_addr);
  
  if (_on_IAS_zone_status_change_notification)
    _on_IAS_zone_status_change_notification(info.src_address.u.ieee_addr, info.src_endpoint, message->info.cluster, message->zone_status);  

}

void ZigbeeGateway::zbCmdDiscAttrResponse(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                                          const esp_zb_zcl_disc_attr_variable_t *variable) {
  
  if (variable) {
  log_i("Attribute Discovery Message - device address (0x%x), source endpoint (0x%x), source cluster (0x%x), attribute id (0x%x), data type (0x%x)", 
        src_address.u.short_addr, src_endpoint, cluster_id, variable->attr_id, variable->data_type);
  }
}

void ZigbeeGateway::addBoundDevice(zb_device_params_t *device){
    
    device->short_addr = esp_zb_address_short_by_ieee(device->ieee_addr);
    device->model_id = 0x0000;
    
    _gateway_devices.push_back(device);

    if (_on_btc_bound_device)
      _on_btc_bound_device (device);
}

bool ZigbeeGateway::isDeviceBound(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) {

	for (std::list<zb_device_params_t *>::iterator bound_device = _gateway_devices.begin(); bound_device != _gateway_devices.end(); ++bound_device) {
              if (((*bound_device)->short_addr == short_addr) || (memcmp((*bound_device)->ieee_addr, ieee_addr, 8) == 0)) return true;
	}
	return false;
		
}

void ZigbeeGateway::setIASzoneReporting(uint16_t short_addr, uint16_t endpoint, uint16_t min_interval, uint16_t max_interval) {
  
  esp_zb_zcl_config_report_cmd_t report_cmd;
  
  report_cmd.zcl_basic_cmd.dst_endpoint = endpoint;
  report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint; //_instance->getEndpoint();
  report_cmd.clusterID = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE;

  int16_t report_change = 1;
  esp_zb_zcl_config_report_record_t records[] = {
    {
      .direction = ESP_ZB_ZCL_REPORT_DIRECTION_SEND, //0x00, //ESP_ZB_ZCL_REPORT_DIRECTION_SEND,ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV
      .attributeID =  ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID,
      .attrType = ESP_ZB_ZCL_ATTR_TYPE_16BITMAP,
      .min_interval = min_interval,
      .max_interval = max_interval,
      .reportable_change = &report_change,
    },
  };
  report_cmd.record_number = ZB_ARRAY_LENTH(records);
  report_cmd.record_field = records;
  
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_config_report_cmd_req(&report_cmd);
  esp_zb_lock_release();
}

void ZigbeeGateway::setClusterReporting(uint16_t short_addr, uint16_t endpoint, uint16_t cluster_id, uint16_t attribute_id, uint8_t attribute_type,
                                        uint16_t min_interval, uint16_t max_interval, uint16_t delta) {
  
  esp_zb_zcl_config_report_cmd_t report_cmd;
  
  report_cmd.zcl_basic_cmd.dst_endpoint = endpoint;
  report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint; // _instance->getEndpoint();
  report_cmd.clusterID = cluster_id;

  int16_t report_change = delta;
  esp_zb_zcl_config_report_record_t records[] = {
    {
      .direction = ESP_ZB_ZCL_REPORT_DIRECTION_SEND, //0x00, //ESP_ZB_ZCL_REPORT_DIRECTION_SEND,
      .attributeID = attribute_id,
      .attrType = attribute_type, //ESP_ZB_ZCL_ATTR_TYPE_S16,
      .min_interval = min_interval,
      .max_interval = max_interval,
      .reportable_change = &report_change,
    },
  };
  report_cmd.record_number = ZB_ARRAY_LENTH(records);
  report_cmd.record_field = records;

  
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_config_report_cmd_req(&report_cmd);
  esp_zb_lock_release();
}

void ZigbeeGateway::setClusterReporting(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster_id, uint16_t attribute_id, uint8_t attribute_type,
                                        uint16_t min_interval, uint16_t max_interval, uint16_t delta) {
  
  esp_zb_zcl_config_report_cmd_t report_cmd;
  
  report_cmd.zcl_basic_cmd.dst_endpoint = endpoint;
  memcpy(report_cmd.zcl_basic_cmd.dst_addr_u.addr_long, ieee_addr, sizeof(esp_zb_ieee_addr_t));
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
  report_cmd.zcl_basic_cmd.src_endpoint = _endpoint; // _instance->getEndpoint();
  report_cmd.clusterID = cluster_id;

  int16_t report_change = delta;
  esp_zb_zcl_config_report_record_t records[] = {
    {
      .direction = ESP_ZB_ZCL_REPORT_DIRECTION_SEND, //0x00, //ESP_ZB_ZCL_REPORT_DIRECTION_SEND,
      .attributeID = attribute_id,
      .attrType = attribute_type, //ESP_ZB_ZCL_ATTR_TYPE_S16,
      .min_interval = min_interval,
      .max_interval = max_interval,
      .reportable_change = &report_change,
    },
  };
  report_cmd.record_number = ZB_ARRAY_LENTH(records);
  report_cmd.record_field = records;

  
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_config_report_cmd_req(&report_cmd);
  esp_zb_lock_release();
}


void ZigbeeGateway::sendAttributeRead(zb_device_params_t * device, int16_t cluster_id, uint16_t attribute_id) {

    esp_zb_zcl_read_attr_cmd_t read_req;

    read_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
    read_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    memcpy(read_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    read_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    read_req.zcl_basic_cmd.src_endpoint = _endpoint;
    read_req.clusterID = cluster_id;

    uint16_t attributes = attribute_id;
    read_req.attr_number = 1; //ZB_ARRAY_LENTH(attributes);
    read_req.attr_field = &attributes;

    log_i("Sending 'read attribute' command");
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_read_attr_cmd_req(&read_req);
    esp_zb_lock_release();
}

void ZigbeeGateway::sendAttributeWrite( zb_device_params_t * device, int16_t cluster_id, uint16_t attribute_id,
                                        esp_zb_zcl_attr_type_t attribute_type, uint16_t attribute_size, void *attribute_value) {

    esp_zb_zcl_write_attr_cmd_t write_req;
    esp_zb_zcl_attribute_t attribute_field;
    esp_zb_zcl_attribute_data_t attribute_data;

    write_req.zcl_basic_cmd.dst_endpoint = device->endpoint;
    write_req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
    memcpy(write_req.zcl_basic_cmd.dst_addr_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    write_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
    write_req.zcl_basic_cmd.src_endpoint = _endpoint;
    write_req.clusterID = cluster_id;
    write_req.attr_number = 1;
    write_req.attr_field = &attribute_field;

    attribute_field.id = attribute_id;
    attribute_field.data.type = attribute_type;
    attribute_field.data.size = attribute_size;
    attribute_field.data.value = attribute_value;

    log_i("Sending 'write attribute' command");
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_write_attr_cmd_req(&write_req);
    esp_zb_lock_release();
}

void ZigbeeGateway::sendIASzoneEnrollResponseCmd(zb_device_params_t *device, uint8_t enroll_rsp_code, uint8_t zone_id){

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
}


void ZigbeeGateway::setOnOffCluster(esp_zb_ieee_addr_t ieee_addr, bool value) {

    esp_zb_zcl_on_off_cmd_t cmd_req;
    
    cmd_req.zcl_basic_cmd.src_endpoint = _endpoint;
    cmd_req.zcl_basic_cmd.dst_endpoint = 1;
    cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    cmd_req.on_off_cmd_id = value ? ESP_ZB_ZCL_CMD_ON_OFF_ON_ID : ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID;
    memcpy(cmd_req.zcl_basic_cmd.dst_addr_u.addr_long, ieee_addr, sizeof(esp_zb_ieee_addr_t));
    log_v(
      "Sending ON/OFF command to ieee address %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", ieee_addr[7], ieee_addr[6], ieee_addr[5],
      ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]
    );
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_on_off_cmd_req(&cmd_req);
    esp_zb_lock_release();
}

void ZigbeeGateway::zbCmdDefaultResponse( esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, uint8_t resp_to_cmd, esp_zb_zcl_status_t status_code) {
  if ((cluster_id == TUYA_PRIVATE_CLUSTER_EF00) && (resp_to_cmd = 0x00))
    xSemaphoreGive(gt_lock);
}

void ZigbeeGateway::sendCustomClusterCmd(zb_device_params_t * device, int16_t custom_cluster_id, uint16_t custom_command_id, uint16_t custom_data_size, uint8_t *custom_data ) {
  
  esp_zb_zcl_custom_cluster_cmd_req_t req;

  req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  req.zcl_basic_cmd.src_endpoint = _endpoint;
  req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req.cluster_id = custom_cluster_id;
  req.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  //req.manuf_specific = 1;
  req.dis_defalut_resp = 0;
  req.custom_cmd_id = custom_command_id;
  req.data.type = ESP_ZB_ZCL_ATTR_TYPE_SET;
  req.data.size = custom_data_size;
  req.data.value = custom_data;
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_custom_cluster_cmd_req(&req);
  esp_zb_lock_release();
}

void ZigbeeGateway::sendCustomClusterCmdAck(zb_device_params_t * device, int16_t custom_cluster_id, uint16_t custom_command_id, uint16_t custom_data_size, uint8_t *custom_data ) {
  
  esp_zb_zcl_custom_cluster_cmd_req_t req;

  req.zcl_basic_cmd.dst_addr_u.addr_short = device->short_addr;
  req.zcl_basic_cmd.dst_endpoint = device->endpoint;
  req.zcl_basic_cmd.src_endpoint = _endpoint;
  req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
  req.cluster_id = custom_cluster_id;
  req.profile_id = ESP_ZB_AF_HA_PROFILE_ID;
  req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;
  //req.manuf_specific = 1;
  req.dis_defalut_resp = 0;
  req.custom_cmd_id = custom_command_id;
  req.data.type = ESP_ZB_ZCL_ATTR_TYPE_SET;
  req.data.size = custom_data_size;
  req.data.value = custom_data;
  esp_zb_lock_acquire(portMAX_DELAY);
  esp_zb_zcl_custom_cluster_cmd_req(&req);
  esp_zb_lock_release();
  if (xSemaphoreTake(gt_lock, ZB_CMD_TIMEOUT) != pdTRUE) {
      log_e("Semaphore error while custom command acknowledge");
      }
}

void ZigbeeGateway::zbCmdCustomClusterReq(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id,uint8_t command_id, uint16_t payload_size, uint8_t *payload) {

  esp_zb_zcl_cmd_info_t info;
  esp_zb_ieee_address_by_short(src_address.u.short_addr, info.src_address.u.ieee_addr);


if (_on_cmd_custom_cluster_receive)
  _on_cmd_custom_cluster_receive(info.src_address.u.ieee_addr, src_endpoint, cluster_id, command_id, payload_size, payload);
}
#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
