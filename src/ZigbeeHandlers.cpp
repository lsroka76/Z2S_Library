/* Zigbee Common Functions */
#include "ZigbeeCore.h"
#include "Arduino.h"

#if SOC_IEEE802154_SUPPORTED 
//&& CONFIG_ZB_ENABLED

// forward declaration of all implemented handlers
static esp_err_t zb_attribute_set_handler(const esp_zb_zcl_set_attr_value_message_t *message);
static esp_err_t zb_attribute_reporting_handler(const esp_zb_zcl_report_attr_message_t *message);
static esp_err_t zb_cmd_read_attr_resp_handler(const esp_zb_zcl_cmd_read_attr_resp_message_t *message);
static esp_err_t zb_configure_report_resp_handler(const esp_zb_zcl_cmd_config_report_resp_message_t *message);
static esp_err_t zb_cmd_default_resp_handler(const esp_zb_zcl_cmd_default_resp_message_t *message);
static esp_err_t zb_cmd_ias_zone_status_change_handler(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message);
static esp_err_t zb_core_cmd_disc_attr_resp_handler(esp_zb_zcl_cmd_discover_attributes_resp_message_t *message);
static esp_err_t zb_cmd_custom_cluster_req_handler(esp_zb_zcl_custom_cluster_command_message_t *message);

// Zigbee action handlers
[[maybe_unused]]
static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
  esp_err_t ret = ESP_OK;
log_i("Receive Zigbee action(0x%x) callback", callback_id);
switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:         ret = zb_attribute_set_handler((esp_zb_zcl_set_attr_value_message_t *)message); break;
    case ESP_ZB_CORE_REPORT_ATTR_CB_ID:            ret = zb_attribute_reporting_handler((esp_zb_zcl_report_attr_message_t *)message); break;
    case ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID:     ret = zb_cmd_read_attr_resp_handler((esp_zb_zcl_cmd_read_attr_resp_message_t *)message); break;
    case ESP_ZB_CORE_CMD_REPORT_CONFIG_RESP_CB_ID: ret = zb_configure_report_resp_handler((esp_zb_zcl_cmd_config_report_resp_message_t *)message); break;
    case ESP_ZB_CORE_CMD_DEFAULT_RESP_CB_ID:       ret = zb_cmd_default_resp_handler((esp_zb_zcl_cmd_default_resp_message_t *)message); break;
    case ESP_ZB_CORE_CMD_DISC_ATTR_RESP_CB_ID:	   ret = zb_core_cmd_disc_attr_resp_handler((esp_zb_zcl_cmd_discover_attributes_resp_message_t *)message); break;	
    case ESP_ZB_CORE_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID: ret = zb_cmd_ias_zone_status_change_handler((esp_zb_zcl_ias_zone_status_change_notification_message_t *)message); break;
    case ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_REQ_CB_ID: ret = zb_cmd_custom_cluster_req_handler((esp_zb_zcl_custom_cluster_command_message_t *)message); break;
    case ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_RESP_CB_ID: ret = zb_cmd_custom_cluster_req_handler((esp_zb_zcl_custom_cluster_command_message_t *)message); break;
    default:                                       log_w("Receive unhandled Zigbee action(0x%x) callback", callback_id); break;
  }
  return ret;
}

static esp_err_t zb_attribute_set_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
   if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  log_v(
    "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster, message->attribute.id,
    message->attribute.data.size
  );

  // List through all Zigbee EPs and call the callback function, with the message
  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->getEndpoint()) {
      if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY) {
        (*it)->zbIdentify(message);  //method zbIdentify implemented in the common EP class
      } else {
        (*it)->zbAttributeSet(message);  //method zbAttributeSet must be implemented in specific EP class
      }
    }
  }
  return ESP_OK;
}

static esp_err_t zb_attribute_reporting_handler(const esp_zb_zcl_report_attr_message_t *message) {
   if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->status);
    return ESP_ERR_INVALID_ARG;
  }
  log_v(
    "Received report from address(0x%x) src endpoint(%d) to dst endpoint(%d) cluster(0x%x)", message->src_address.u.short_addr, message->src_endpoint,
    message->dst_endpoint, message->cluster
  );
  // List through all Zigbee EPs and call the callback function, with the message
  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->dst_endpoint == (*it)->getEndpoint()) {
      (*it)->zbAttributeRead(message->src_address, message->src_endpoint, message->cluster, &message->attribute);  //method zbAttributeRead must be implemented in specific EP class
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_read_attr_resp_handler(const esp_zb_zcl_cmd_read_attr_resp_message_t *message) {
   if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  log_v(
    "Read attribute response: from address(0x%x) src endpoint(%d) to dst endpoint(%d) cluster(0x%x)", message->info.src_address.u.short_addr,
    message->info.src_endpoint, message->info.dst_endpoint, message->info.cluster
  );

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->getEndpoint()) {
      esp_zb_zcl_read_attr_resp_variable_t *variable = message->variables;
      while (variable) {
        log_v(
          "Read attribute response: status(%d), cluster(0x%x), attribute(0x%x), type(0x%x), value(%d)", variable->status, message->info.cluster,
          variable->attribute.id, variable->attribute.data.type, variable->attribute.data.value ? *(uint8_t *)variable->attribute.data.value : 0
        );
        if (variable->status == ESP_ZB_ZCL_STATUS_SUCCESS) {
          if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_BASIC) {
            (*it)->zbReadBasicCluster(&variable->attribute);  //method zbReadBasicCluster implemented in the common EP class
          } else {
            (*it)->zbAttributeRead(message->info.src_address, message->info.src_endpoint, message->info.cluster, &variable->attribute);  //method zbAttributeRead must be implemented in specific EP class
          }
        }
        variable = variable->next;
      }
    }
  }
  return ESP_OK;
}

static esp_err_t zb_configure_report_resp_handler(const esp_zb_zcl_cmd_config_report_resp_message_t *message) {
   if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  esp_zb_zcl_config_report_resp_variable_t *variable = message->variables;
  while (variable) {
    log_v(
      "Configure report response: status(%d), cluster(0x%x), direction(0x%x), attribute(0x%x)", variable->status, message->info.cluster, variable->direction,
      variable->attribute_id
    );
    variable = variable->next;
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_default_resp_handler(const esp_zb_zcl_cmd_default_resp_message_t *message) {
  if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  log_v(
    "Received default response: from address(0x%x), src_endpoint(%d) to dst_endpoint(%d), cluster(0x%x) for command (0x%x) with status 0x%x",
    message->info.src_address.u.short_addr, message->info.src_endpoint, message->info.dst_endpoint, message->info.cluster, message->resp_to_cmd, message->status_code
  );
  log_v("command id (%d), direction (%d), is common (%d)", message->info.command.id, message->info.command.direction, message->info.command.is_common);  
  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->getEndpoint()) {
        
	      (*it)->zbCmdDefaultResponse( message->info.src_address, message->info.src_endpoint, message->info.cluster, message->resp_to_cmd, message->status_code);
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_ias_zone_status_change_handler(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {
   if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  log_v(
    "IAS Zone Status Notification: from address(0x%x) src endpoint(%d) to dst endpoint(%d) cluster(0x%x), zone status (0x%x), extended status (0x%x), zone id (0x%x), delay (0x%x)", 
    message->info.src_address.u.short_addr, message->info.src_endpoint, message->info.dst_endpoint, message->info.cluster, message->zone_status, message->extended_status,
    message->zone_id, message->delay); 

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->getEndpoint()) {
        
	      (*it)->zbIASZoneStatusChangeNotification(message);
	
    }
  }
  return ESP_OK;
}

static esp_err_t zb_core_cmd_disc_attr_resp_handler(esp_zb_zcl_cmd_discover_attributes_resp_message_t *message) {
   if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->getEndpoint()) {
    
    esp_zb_zcl_disc_attr_variable_t *variable = message->variables;
    
    while (variable) {
     //log_i("Attribute Discovery Message - device address %d, source endpoint %d, source cluster %d, attribute id %d, data type %d", message->info.src_address.u.short_addr, message->info.src_endpoint, message->info.cluster, variable->attr_id, variable->data_type);
     (*it)->zbCmdDiscAttrResponse(message->info.src_address, message->info.src_endpoint, message->info.cluster, variable);
     variable = variable->next;  
     }
     (*it)->zbCmdDiscAttrResponse(message->info.src_address, message->info.src_endpoint, message->info.cluster, NULL);
    }
   }
  return ESP_OK;
}

static esp_err_t zb_cmd_custom_cluster_req_handler(esp_zb_zcl_custom_cluster_command_message_t *message) {
  if (!message) {
    log_e("Empty message");
    return ESP_FAIL;
  }
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
    log_e("Received message: error status(%d)", message->info.status);
    return ESP_ERR_INVALID_ARG;
  }
  //log_i("Receive custom command: %d from address 0x%04hx", message->info.command.id, message->info.src_address.u.short_addr);
  //log_i("Attribute 0x%x", message->data.type);
  //log_i("Payload size: %d", message->data.size);
  //for (uint8_t i = 0; i < message->data.size; i++)
  //  log_i("Payload [0x%x] = 0x%x", i, *(((uint8_t *)(message->data.value)) + i));
  
  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.dst_endpoint == (*it)->getEndpoint()) {
        
	      (*it)->zbCmdCustomClusterReq( message->info.src_address, message->info.src_endpoint, message->info.cluster, message->info.command.id,
                                      message->data.size, (uint8_t*) message->data.value);
	
    }
  }
  return ESP_OK;
}
#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
