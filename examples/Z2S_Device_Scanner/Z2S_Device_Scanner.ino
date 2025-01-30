#pragma once

#define EP_CLUSTERS_ATTRIBUTES_SCANNER 1

extern "C" { 
#include <zboss_api.h>
#include <zboss_api_nwk.h>
#include <zboss_api_buf.h>
#include <zboss_api_af.h>
#include <zcl/zb_zcl_common.h>
}

#include <ZigbeeGateway.h>

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);



void sz_ias_zone_notification(int status, uint8_t *ieee_addr_64)
{

}

bool zboss_raw_cmd_handler(uint8_t bufid){
  log_i("buufer len 0x%x", zb_buf_len(bufid));
  if (zb_buf_len(bufid) > 0) 
    for (int i = 0; i < zb_buf_len(bufid); i++)
      log_i("0x%x ", *((uint8_t*)(zb_buf_begin(bufid)+i)));
  return false;
}

bool rawCmdHandlerCb(uint8_t bufid) {
    uint8_t buf[zb_buf_len(bufid)];
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);
    log_i("cluster id: 0x%x, command id: %d", cmd_info->cluster_id, cmd_info->cmd_id);
    log_i("src endpoint (0x%x, dst endpoint 0x%x", cmd_info->addr_data.common_data.src_endpoint, cmd_info->addr_data.common_data.dst_endpoint);
    log_i("is common comand %d, disable_default_response %d, is_manuf_specific %d", cmd_info->is_common_command, cmd_info->disable_default_response, cmd_info->is_manuf_specific);
    log_i("manuf_specific 0x%x", cmd_info->manuf_specific);
    memcpy(buf, zb_buf_begin(bufid), sizeof(buf));
    log_i("RAW bufid: %d size: %d", bufid, sizeof(buf));
    for (int i = 0; i < sizeof(buf); ++i) {
        log_i("0x%02X ", buf[i]);
    }
    
    if ((cmd_info->cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (cmd_info->cmd_id == 0xFD)) {
      char button_states[][15]={"pushed", "double tapped", "held"};
      
      log_i("Button nr %d was: %s", cmd_info->addr_data.common_data.dst_endpoint, button_states[buf[0]]); 
      zb_zcl_send_default_handler(bufid, cmd_info, ZB_ZCL_STATUS_SUCCESS);
      return true;
    }
    if ((cmd_info->cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (cmd_info->cmd_id == 0xFC)) {
      char button_states[][15]={"rotated right", "rotated left"};
      
      log_i("Button nr %d was: %s", cmd_info->addr_data.common_data.dst_endpoint, button_states[buf[0]]); 
      zb_zcl_send_default_handler(bufid, cmd_info, ZB_ZCL_STATUS_SUCCESS);
      return true;
    }
    //zb_zcl_send_default_handler(bufid, cmd_info, ZB_ZCL_STATUS_SUCCESS);
    log_i("-------------------------------------------------------------------------------------------");
    return false;
}

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

bool zbInit = true;

void setup() {

  pinMode(BUTTON_PIN, INPUT);

  //  Zigbee

  //zbGateway.onStatusNotification(sz_ias_zone_notification);
  //esp_zb_raw_command_handler_register(rawCmdHandlerCb);
  zbGateway.setManufacturerAndModel("Zigbee2Supla", "Z2S_Device_Scanner");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
      Serial.println("Zigbee failed to start!");
      Serial.println("Rebooting...");
      ESP.restart();
  }
  
  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
}

zb_device_params_t *gateway_device;
zb_device_params_t *joined_device;

char zbd_model_name[64];


uint16_t gateway_devices_list_size = 0;

void loop() {
  
  if (digitalRead(BUTTON_PIN) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 5000) {
        // If key pressed for more than 5 secs, factory reset Zigbee and reboot
        Serial.printf("Resetting Zigbee to factory settings, reboot.\n");
        Zigbee.factoryReset();
      }
    }
    Zigbee.openNetwork(180);
  }
  delay(100);

  if (zbGateway.isNewDeviceJoined()) {

    zbGateway.clearNewDeviceJoined();
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty())
    {
      joined_device = zbGateway.getLastJoinedDevice();
      
      //zbGateway.zbQueryDeviceBasicCluster(joined_device);
      //log_i("manufacturer %s ", zbGateway.readManufacturer(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      //log_i("model %s ", zbGateway.readModel(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      
#ifdef EP_CLUSTERS_ATTRIBUTES_SCANNER        
    
  //zbGateway.sendCustomClusterCmd(joined_device, 0xEF00, 0x03, 0, NULL);
  //zbGateway.sendCustomClusterCmd(joined_device, 0xE001, 0x03, 0, NULL);

    //zbGateway.zbPrintDeviceDiscovery(joined_device);
    
    //zbGateway.sendAttributeRead(joined_device, 0xE001,0xD010, true);
    //zbGateway.sendAttributeRead(joined_device, 0xE001,0xD020, true);
    //zbGateway.sendAttributeRead(joined_device, 0xE001,0xD010, true);

    //zbGateway.sendAttributeRead(joined_device, 0x0006,0x0, true);
    //zbGateway.sendAttributeRead(joined_device, 0x0006,0x8004, true);
    static uint8_t write_mask = 0x01;
    //zbGateway.sendAttributeWrite(joined_device, 0x0006, 0x8004, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask );
    //delay(200);
    //zbGateway.sendAttributeRead(joined_device, 0x0006,0x8004, true);
    //zbGateway.sendAttributeRead(joined_device, 0xE001,0xD011, true);

    for (int i=1; i < 2; i++)
      {
        joined_device->endpoint = i;
        zbGateway.bindDeviceCluster(joined_device,0x0006);
        //zbGateway.bindDeviceCluster(joined_device,0x0005);
        //zbGateway.bindDeviceCluster(joined_device,0x0008);
      }
      
#endif //EP_CLUSTERS_ATTRIBUTES_SCANNER
    }
  }
}