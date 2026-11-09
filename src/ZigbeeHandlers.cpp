/* Zigbee Common Functions */

#include "GeminiProtocol.h"

#if CONFIG_ZB_ENABLED && !defined(USE_GEMINI_PROTOCOL_HOST)

extern "C" { 
#include <zboss_api.h>
#include <zboss_api_nwk.h>
#include <zboss_api_buf.h>
#include <zboss_api_af.h>
#include <zcl/zb_zcl_common.h>
#include <zboss_api_zcl.h>
}

#include "ZigbeeCore.h"

// Forward declaration of all implemented handlers
static esp_err_t zb_attribute_set_handler(
  const esp_zb_zcl_set_attr_value_message_t *message);
static esp_err_t zb_attribute_reporting_handler(
  const esp_zb_zcl_report_attr_message_t *message);
static esp_err_t zb_cmd_read_attr_resp_handler(
  const esp_zb_zcl_cmd_read_attr_resp_message_t *message);
static esp_err_t zb_cmd_write_attr_resp_handler(
  const esp_zb_zcl_cmd_write_attr_resp_message_t *message);
static esp_err_t zb_configure_report_resp_handler(
  const esp_zb_zcl_cmd_config_report_resp_message_t *message);
static esp_err_t zb_cmd_read_report_cfg_resp_handler(
  const esp_zb_zcl_cmd_read_report_config_resp_message_t *message);
static esp_err_t zb_cmd_default_resp_handler(
  const esp_zb_zcl_cmd_default_resp_message_t *message);
static esp_err_t zb_cmd_ias_zone_enroll_request_handler(
  const esp_zb_zcl_ias_zone_enroll_request_message_t *message);
static esp_err_t zb_cmd_ias_zone_status_change_handler(
  const esp_zb_zcl_ias_zone_status_change_notification_message_t *message);
static esp_err_t zb_core_cmd_disc_attr_resp_handler(
  esp_zb_zcl_cmd_discover_attributes_resp_message_t *message);
static esp_err_t zb_cmd_custom_cluster_req_handler(
  esp_zb_zcl_custom_cluster_command_message_t *message);
static esp_err_t zb_zcl_group_operation_resp(
  esp_zb_zcl_groups_operate_group_resp_message_t message);
static esp_err_t zb_window_covering_movement_resp_handler(
  const esp_zb_zcl_window_covering_movement_message_t *message);
static esp_err_t zb_ota_upgrade_server_status_handler(
  const esp_zb_zcl_ota_upgrade_server_status_message_t *message);
static esp_err_t zb_ota_upgrade_server_query_image_handler(
  const esp_zb_zcl_ota_upgrade_server_query_image_message_t *message);

static bool zb_raw_cmd_handler(uint8_t bufid);

// Zigbee action handlers
[[maybe_unused]]
static esp_err_t zb_action_handler(
  esp_zb_core_action_callback_id_t callback_id, const void *message) {
  
  esp_err_t ret = ESP_OK;
  log_v("Receive Zigbee action(0x%x) callback", callback_id);
  
  switch (callback_id) {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:         
      ret = zb_attribute_set_handler(
        (const esp_zb_zcl_set_attr_value_message_t *)message); 
      break;

    case ESP_ZB_CORE_REPORT_ATTR_CB_ID:            
      ret = zb_attribute_reporting_handler(
        (const esp_zb_zcl_report_attr_message_t *)message); 
      break;

    case ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID:        
      ret = zb_cmd_read_attr_resp_handler(
        (const esp_zb_zcl_cmd_read_attr_resp_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_WRITE_ATTR_RESP_CB_ID:       
      ret = zb_cmd_write_attr_resp_handler(
        (const esp_zb_zcl_cmd_write_attr_resp_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_REPORT_CONFIG_RESP_CB_ID:    
      ret = zb_configure_report_resp_handler(
        (const esp_zb_zcl_cmd_config_report_resp_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_READ_REPORT_CFG_RESP_CB_ID:  
      ret = zb_cmd_read_report_cfg_resp_handler(
        (const esp_zb_zcl_cmd_read_report_config_resp_message_t *)message);
      break;
    
    case ESP_ZB_CORE_CMD_DEFAULT_RESP_CB_ID:          
      ret = zb_cmd_default_resp_handler(
        (const esp_zb_zcl_cmd_default_resp_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_DISC_ATTR_RESP_CB_ID:	      
      ret = zb_core_cmd_disc_attr_resp_handler(
        (esp_zb_zcl_cmd_discover_attributes_resp_message_t *)message); 
      break;	
    
    case ESP_ZB_CORE_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID:
      ret = zb_cmd_ias_zone_enroll_request_handler(
        (const esp_zb_zcl_ias_zone_enroll_request_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID: 
      ret = zb_cmd_ias_zone_status_change_handler(
        (const esp_zb_zcl_ias_zone_status_change_notification_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_REQ_CB_ID:    
    case ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_RESP_CB_ID:   
      ret = zb_cmd_custom_cluster_req_handler(
        (esp_zb_zcl_custom_cluster_command_message_t *)message); 
      break;
    
    case ESP_ZB_CORE_CMD_OPERATE_GROUP_RESP_CB_ID:
      ret = zb_zcl_group_operation_resp(
        *(const esp_zb_zcl_groups_operate_group_resp_message_t *)message);
      break;
    
    case ESP_ZB_CORE_WINDOW_COVERING_MOVEMENT_CB_ID:
      ret = zb_window_covering_movement_resp_handler(
        (const esp_zb_zcl_window_covering_movement_message_t *)message);
      break;

    case ESP_ZB_CORE_OTA_UPGRADE_SRV_STATUS_CB_ID:
      ret = zb_ota_upgrade_server_status_handler(
        (const esp_zb_zcl_ota_upgrade_server_status_message_t *)message);
      break;

    case ESP_ZB_CORE_OTA_UPGRADE_SRV_QUERY_IMAGE_CB_ID:
      ret = zb_ota_upgrade_server_query_image_handler(
        (const esp_zb_zcl_ota_upgrade_server_query_image_message_t *)message);
      break;

    default:                                       
      log_w("Receive unhandled Zigbee action(0x%x) callback", callback_id); 
      break;
  }
  return ret;
}

static bool zb_raw_cmd_handler(uint8_t bufid) {
  uint8_t raw_len = (zb_buf_len(bufid) <= 255) ? (uint8_t)zb_buf_len(bufid) : 255;
  uint8_t buf[255];
  
  zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);

  log_v("cluster id: 0x%x, command id: %d, common: %d", 
        cmd_info->cluster_id, cmd_info->cmd_id, cmd_info->is_common_command);

  uint16_t dst_addr = cmd_info->addr_data.common_data.dst_addr;
  uint16_t group_id = 0x0000;

  if (dst_addr >= 0xFFF8) { // Broadcast NWK address (0xFFFD for RxOnWhenIdle)
    uint8_t *zcl_payload = (uint8_t *)zb_buf_begin(bufid);
    uint8_t zcl_hdr_len = cmd_info->is_manuf_specific ? 5 : 3;
    uint8_t *aps_hdr_end = zcl_payload - zcl_hdr_len;
    uint8_t aps_fc = *(aps_hdr_end - 9);

    if ((aps_fc & 0x0C) == 0x0C) { // Delivery Mode bits [3:2] == 0b11 (Group Addressing)
      group_id = (uint16_t)(*(aps_hdr_end - 8)) | ((uint16_t)(*(aps_hdr_end - 7)) << 8);
      log_i("APS Groupcast detected! Group ID: 0x%04X", group_id);
    }
  }

  if (raw_len > 0) {
    memcpy(buf, zb_buf_begin(bufid), raw_len);
  }
  
  esp_zb_zcl_addr_t esp_zb_zcl_address = {};
  esp_zb_zcl_address.addr_type    = ESP_ZB_ZCL_ADDR_TYPE_SHORT;
  esp_zb_zcl_address.u.short_addr = cmd_info->addr_data.common_data.source.u.short_addr;
  
  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if ((*it)->zbRawCmdHandler(
      cmd_info->seq_number, esp_zb_zcl_address, cmd_info->rssi, 
      cmd_info->addr_data.common_data.src_endpoint, 
      cmd_info->addr_data.common_data.dst_endpoint, 
      cmd_info->cluster_id, cmd_info->cmd_id, group_id, 
      cmd_info->is_common_command, cmd_info->disable_default_response, 
      cmd_info->is_manuf_specific, cmd_info->manuf_specific, raw_len, 
      buf)) {

      zb_zcl_send_default_handler(bufid, cmd_info, ZB_ZCL_STATUS_SUCCESS);
      log_v("raw command processed");
      return true;
    } else {
      return false;
    }
  }
  return false;
}

static esp_err_t zb_attribute_set_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY) {
      (*it)->zbIdentify(message);
    } else {
      (*it)->zbAttributeSet(message);
    }
  }
  return ESP_OK;
}

static esp_err_t zb_attribute_reporting_handler(const esp_zb_zcl_report_attr_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if (message->cluster == ESP_ZB_ZCL_CLUSTER_ID_BASIC) {
      (*it)->zbReadBasicCluster(
        message->src_address, message->src_endpoint, message->cluster, 
        (esp_zb_zcl_attribute_t *)&message->attribute);
    } else {
      (*it)->zbAttributeReporting(
        message->src_address, message->src_endpoint, message->cluster, 
        &message->attribute);
    }  
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_read_attr_resp_handler(const esp_zb_zcl_cmd_read_attr_resp_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    esp_zb_zcl_read_attr_resp_variable_t *variable = message->variables;
    while (variable) {
      if (message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_BASIC) {
        (*it)->zbReadBasicCluster(
          message->info.src_address, message->info.src_endpoint, 
          message->info.cluster, &variable->attribute);
      } else {
        (*it)->zbReadAttrResponse(
          message->info.header.tsn, message->info.header.rssi,
          message->info.src_address, message->info.src_endpoint, 
          message->info.cluster, variable->status, &variable->attribute); 
      }
      variable = variable->next;
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_write_attr_resp_handler(const esp_zb_zcl_cmd_write_attr_resp_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    esp_zb_zcl_write_attr_resp_variable_t *variable = message->variables;
    while (variable) {
      (*it)->zbWriteAttrResponse(message->info.header.tsn, variable->status, variable->attribute_id); 
      variable = variable->next;
    }
  }
  return ESP_OK;
}

static esp_err_t zb_configure_report_resp_handler(const esp_zb_zcl_cmd_config_report_resp_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    esp_zb_zcl_config_report_resp_variable_t *variable = message->variables;
    while (variable) {
      (*it)->zbConfigReportResponse(
        message->info.header.tsn, message->info.src_address, message->info.src_endpoint,
        message->info.cluster, variable->status, variable->direction, variable->attribute_id);
      variable = variable->next;  
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_read_report_cfg_resp_handler(const esp_zb_zcl_cmd_read_report_config_resp_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    esp_zb_zcl_read_report_config_resp_variable_t *variable = message->variables;
    while (variable) {
      (*it)->zbReadReportConfigResponse(message);
      variable = variable->next;
    }
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_default_resp_handler(const esp_zb_zcl_cmd_default_resp_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    (*it)->zbCmdDefaultResponse(
      message->info.header.tsn, message->info.header.rssi, 
      message->info.src_address, message->info.src_endpoint, 
      message->info.cluster, message->resp_to_cmd, message->status_code);
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_ias_zone_enroll_request_handler(const esp_zb_zcl_ias_zone_enroll_request_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    (*it)->zbIASZoneEnrollRequest(message);
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_ias_zone_status_change_handler(
  const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) {

  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    (*it)->zbIASZoneStatusChangeNotification(message);
  }
  return ESP_OK;
}

static esp_err_t zb_core_cmd_disc_attr_resp_handler(esp_zb_zcl_cmd_discover_attributes_resp_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    esp_zb_zcl_disc_attr_variable_t *variable = message->variables;
    while (variable) {
      (*it)->zbCmdDiscAttrResponse(message->info.src_address, message->info.src_endpoint, message->info.cluster, variable);
      variable = variable->next;  
    }
    (*it)->zbCmdDiscAttrResponse(message->info.src_address, message->info.src_endpoint, message->info.cluster, NULL);
  }
  return ESP_OK;
}

static esp_err_t zb_cmd_custom_cluster_req_handler(esp_zb_zcl_custom_cluster_command_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    (*it)->zbCmdCustomClusterReq(
      message->info.header.tsn,
      message->info.src_address, message->info.src_endpoint, 
      message->info.cluster, message->info.command.id, message->data.size, 
      (uint8_t *)message->data.value);
  }
  return ESP_OK;
}

static esp_err_t zb_zcl_group_operation_resp(esp_zb_zcl_groups_operate_group_resp_message_t message) {
  if (message.info.status == ESP_ZB_ZCL_STATUS_SUCCESS) {
    switch (message.info.command.id) {
      case ESP_ZB_ZCL_CMD_GROUPS_ADD_GROUP:
        log_i("Succeed in adding endpoint: %d to group: %d", message.info.src_endpoint, message.group_id);
        break;
      case ESP_ZB_ZCL_CMD_GROUPS_REMOVE_GROUP:
        log_i("Succeed in removing endpoint: %d from group: %d", message.info.src_endpoint, message.group_id);
        break;
      default:
        break;
    }
  }
  return ESP_OK;
}

static esp_err_t zb_window_covering_movement_resp_handler(
  const esp_zb_zcl_window_covering_movement_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_ERR_INVALID_ARG;
  return ESP_OK;
}

static esp_err_t zb_ota_upgrade_server_status_handler(
  const esp_zb_zcl_ota_upgrade_server_status_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_FAIL;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    (*it)->zbOTAUpgradeServerStatus(message);
  }
  return ESP_OK;
}

static esp_err_t zb_ota_upgrade_server_query_image_handler(
  const esp_zb_zcl_ota_upgrade_server_query_image_message_t *message) {
  if (!message) return ESP_FAIL;
  if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) return ESP_FAIL;

  for (std::list<ZigbeeEP *>::iterator it = Zigbee.ep_objects.begin(); it != Zigbee.ep_objects.end(); ++it) {
    if ((*it)->zbOTAUpgradeServerQueryImage(message))
      return ESP_OK;
    else
      return ESP_ERR_NOT_FOUND;
  }
  return ESP_ERR_NOT_FOUND; // Fixed missing return statement!
}

#endif // CONFIG_ZB_ENABLED && !defined(USE_GEMINI_PROTOCOL_HOST)