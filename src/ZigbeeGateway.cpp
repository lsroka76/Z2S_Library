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

uint16_t ZigbeeGateway::_clusters_2_bind = 0;
//


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

  
  /*esp_zb_attribute_list_t Tuya_custom;
  
  Tuya_custom.attribute = 0x00;
  Tuya_custom.cluster_id = 0xEF00;
  Tuya_custom.next = NULL;*/
  
  _cluster_list = esp_zb_zcl_cluster_list_create();
  esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&(gateway_cfg.basic_cfg));
  esp_zb_cluster_list_add_basic_cluster(_cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_basic_cluster(_cluster_list, esp_zb_basic_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_identify_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_identify_cluster(_cluster_list, esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_power_config_cluster(_cluster_list, esp_zb_power_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);

  esp_zb_cluster_list_add_ias_zone_cluster(_cluster_list, esp_zb_ias_zone_cluster_create(&zone_cfg), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_temperature_meas_cluster(_cluster_list, esp_zb_temperature_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_humidity_meas_cluster(_cluster_list, esp_zb_humidity_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  esp_zb_cluster_list_add_on_off_cluster(_cluster_list, esp_zb_on_off_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_on_off_switch_config_cluster(_cluster_list, esp_zb_on_off_switch_config_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_electrical_meas_cluster(_cluster_list, esp_zb_electrical_meas_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_metering_cluster(_cluster_list, esp_zb_metering_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  esp_zb_cluster_list_add_thermostat_cluster(_cluster_list, esp_zb_thermostat_cluster_create(NULL), ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  
  //esp_zb_cluster_list_add_custom_cluster(_cluster_list, &Tuya_custom,ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE);
  //esp_zb_cluster_list_add_custom_cluster(_cluster_list, &Tuya_custom,ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
  
  _ep_config = {.endpoint = _endpoint, .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID, .app_device_id = ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID, .app_device_version = 0};
}

void ZigbeeGateway::bindCb(esp_zb_zdp_status_t zdo_status, void *user_ctx) {
  
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    
    if (user_ctx) {
      
      zb_device_params_t *sensor = (zb_device_params_t *)user_ctx;
      
      _instance->_gateway_devices.push_back(sensor);
      
      if (_instance->_clusters_2_bind > 0) --_instance->_clusters_2_bind;
      
      if (_instance->_clusters_2_bind == 0 && _instance->_on_bound_device)
          _instance->_on_bound_device (sensor, true);

      log_v("Binding success (ZC side)");
    } else
      log_v("Binding success (ED side");
    
      _is_bound = true;
      _last_bind_success = true;
  } else {
    log_e("Binding failed!");
    _last_bind_success = false;
  }
  _in_binding = false;
}

void ZigbeeGateway::find_Cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx) {
  
  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
    
    
    esp_zb_zdo_bind_req_param_t bind_req;
    
    /* Store the information of the remote device */
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

  esp_zb_zdo_bind_req_param_t bind_req;
    
  bind_req.req_dst_addr = device->short_addr;
  log_d("Request sensor to bind us");

  /* populate the src information of the binding */
  memcpy(bind_req.src_address, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.src_endp = device->endpoint;
  bind_req.cluster_id = cluster_id; 
    
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  esp_zb_get_long_address(bind_req.dst_address_u.addr_long);
  bind_req.dst_endp = _instance->getEndpoint(); 
    
  esp_zb_zdo_device_bind_req(&bind_req, bindCb, NULL);

  bind_req.req_dst_addr = esp_zb_get_short_address();

  esp_zb_get_long_address(bind_req.src_address);
  bind_req.src_endp = _instance->getEndpoint();
  bind_req.cluster_id = cluster_id;
    
  bind_req.dst_addr_mode = ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED;
  memcpy(bind_req.dst_address_u.addr_long, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  bind_req.dst_endp = device->endpoint;

  device->cluster_id = cluster_id;
  
  esp_zb_zdo_device_bind_req(&bind_req, bindCb, (void *)device);

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
      log_i("zbAttributeRead temperature measurement %f",((float)value)/100);
      log_i("zbAttributeRead %d:%d:%d:%d:%d:%d:%d:%d, endpoint 0x%x", src_address.u.ieee_addr[7], src_address.u.ieee_addr[6], src_address.u.ieee_addr[5], 
      src_address.u.ieee_addr[4], src_address.u.ieee_addr[3],src_address.u.ieee_addr[2], src_address.u.ieee_addr[1], src_address.u.ieee_addr[0], src_endpoint);
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
    if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) 
    {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead electrical measurement RMS voltage %d",value);
      //if (_on_on_off_receive)
        //_on_on_off_receive(src_address.u.ieee_addr, value);
      } else log_i("zbAttributeRead electrical measurement cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else
    if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_METERING) {
    if (attribute->id == ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID && attribute->data.type == ESP_ZB_ZCL_ATTR_TYPE_U16) 
    {
      uint16_t value = attribute->data.value ? *(uint16_t *)attribute->data.value : 0;
      log_i("zbAttributeRead electrical measurement RMS voltage %d",value);
      //if (_on_on_off_receive)
        //_on_on_off_receive(src_address.u.ieee_addr, value);
      } else log_i("zbAttributeRead electrical measurement cluster (0x%x), attribute id (0x%x), attribute data type (0x%x)", cluster_id, attribute->id, attribute->data.type);
    } else log_i("zbAttributeRead from (0x%x), endpoint (%d), cluster (0x%x), attribute id (0x%x), attribute data type (0x%x) ", 
        src_address.u.short_addr, src_endpoint, cluster_id, attribute->id, attribute->data.type);
}

void ZigbeeGateway::zbIASZoneStatusChangeNotification(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {


  esp_zb_zcl_cmd_info_t info = message->info;
  esp_zb_zcl_addr_t src_address = info.src_address;
  esp_zb_ieee_address_by_short(info.src_address.u.short_addr,info.src_address.u.ieee_addr);
  //log_i("short address %d, src id %d ", src_address.u.short_addr, src_address.u.src_id);

  //char ieee_addr[9];
  //uint8_t ieee_addr_64[8];

  //ieee_addr[8] = '\0';

  //esp_zb_ieee_address_by_short(src_address.u.short_addr, ieee_addr_64);

  //log_i("ieee addr %d:%d:%d:%d:%d:%d:%d:%d ",ieee_addr_64[7],ieee_addr_64[6],ieee_addr_64[5],ieee_addr_64[4],
  //  ieee_addr_64[3],ieee_addr_64[2],ieee_addr_64[1],ieee_addr_64[0]);
  //log_i("zone status %d ", message->zone_status);
  if (_on_IAS_zone_status_change_notification)
    _on_IAS_zone_status_change_notification(info.src_address.u.ieee_addr, info.src_endpoint, info.cluster, message->zone_status);  

}

void ZigbeeGateway::zbCmdDiscAttrResponse(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                                          const esp_zb_zcl_disc_attr_variable_t *variable) {
  
  if (variable) {
  log_i("Attribute Discovery Message - device address (0x%x), source endpoint (0x%x), source cluster (0x%x), attribute id (0x%x), data type (0x%x)", 
        src_address.u.short_addr, src_endpoint, cluster_id, variable->attr_id, variable->data_type);
  } //else xSemaphoreGive(gt_lock);
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

void ZigbeeGateway::setIASZReporting(uint16_t short_addr, uint16_t endpoint, uint16_t min_interval, uint16_t max_interval) {
  
  esp_zb_zcl_config_report_cmd_t report_cmd;
  
  report_cmd.zcl_basic_cmd.dst_endpoint = endpoint;
  report_cmd.zcl_basic_cmd.dst_addr_u.addr_short = short_addr;
  //report_cmd.zcl_basic_cmd.src_endpoint = 1;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      
  //report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
  report_cmd.zcl_basic_cmd.src_endpoint = _instance->getEndpoint();
  report_cmd.clusterID = ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE;

  int16_t report_change = 1;
  esp_zb_zcl_config_report_record_t records[] = {
    {
      .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, //0x00, //ESP_ZB_ZCL_REPORT_DIRECTION_SEND,
      .attributeID = 0,
      .attrType = ESP_ZB_ZCL_ATTR_TYPE_S16,
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
  //report_cmd.zcl_basic_cmd.src_endpoint = 1;
  report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;
      
  //report_cmd.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
  report_cmd.zcl_basic_cmd.src_endpoint = _instance->getEndpoint();
  report_cmd.clusterID = cluster_id;

  int16_t report_change = delta;
  esp_zb_zcl_config_report_record_t records[] = {
    {
      .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, //0x00, //ESP_ZB_ZCL_REPORT_DIRECTION_SEND,
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

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
