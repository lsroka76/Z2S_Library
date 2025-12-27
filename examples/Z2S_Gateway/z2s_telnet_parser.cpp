#include "z2s_telnet_parser.h"

extern ZigbeeGateway zbGateway;

void printDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  //Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    telnet.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    telnet.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {

      telnet.printf("\n\r- DIR : %s\n\r", file.name());
      
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      
      telnet.printf(
        "- LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n\n\n\r", 
        (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, 
        tmstruct->tm_mday, tmstruct->tm_hour,tmstruct->tm_min, 
        tmstruct->tm_sec);

      if (levels) {
        printDir(fs, file.path(), levels - 1);
      }
    } else {
      
      telnet.printf("-- FILE: %s\n\r-- SIZE: %u\n\r", file.name(), 
        /*strlen(file.name()) > 24 ? "\t" : "\t\t",*/ file.size());
      
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      telnet.printf(
        "-- LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n\n\r", 
          (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, 
          tmstruct->tm_mday, tmstruct->tm_hour,tmstruct->tm_min, 
          tmstruct->tm_sec);
    }
    file = root.openNextFile();
  }
}

void Z2S_nwk_scan_neighbourhood(bool toTelnet = false) {

  char log_line[384];

  esp_zb_nwk_neighbor_info_t nwk_neighbour;
  esp_zb_nwk_info_iterator_t nwk_iterator = 0;

  esp_err_t scan_result = esp_zb_nwk_get_next_neighbor(
    &nwk_iterator, &nwk_neighbour);
  
  if (scan_result == ESP_ERR_NOT_FOUND)
    log_i_telnet2(
      "\033[1mZ2S_nwk_scan_neighbourhood scan empty :-(  \033[22m");

  while (scan_result == ESP_OK) {
    
    sprintf_P(log_line, 
              PSTR("Scan neighbour record number - 0x%x:\n\rIEEE ADDRESS\t\t"
                  "%X:%X:%X:%X:%X:%X:%X:%X\n\rSHORT ADDRESS\t\t0x%x\n\rDEPTH"
                  "\t\t\t0x%x\n\rRX_ON_WHEN_IDLE\t\t0x%x\n\rRELATIONSHIP\t\t"
                  "0x%x\n\rLQI\t\t\t%d\n\rRSSI\t\t\t%d\n\rOUTGOING COST\t\t0x%x\n"
                  "\rAGE\t\t\t0x%x\n\rDEVICE TIMEOUT\t\t%lu\n\rTIMEOUT COUNTER\t\t%lu"), 
        nwk_iterator, 
        nwk_neighbour.ieee_addr[7], 
        nwk_neighbour.ieee_addr[6], 
        nwk_neighbour.ieee_addr[5], 
        nwk_neighbour.ieee_addr[4], 
        nwk_neighbour.ieee_addr[3], 
        nwk_neighbour.ieee_addr[2], 
        nwk_neighbour.ieee_addr[1], 
        nwk_neighbour.ieee_addr[0],
        nwk_neighbour.short_addr, 
        nwk_neighbour.depth, 
        nwk_neighbour.rx_on_when_idle, 
        nwk_neighbour.relationship,
        nwk_neighbour.lqi, 
        nwk_neighbour.rssi, 
        nwk_neighbour.outgoing_cost, 
        nwk_neighbour.age, 
        nwk_neighbour.device_timeout,
        nwk_neighbour.timeout_counter);

    log_i_telnet2(log_line, toTelnet);
    
    int16_t channel_number_slot = Z2S_findChannelNumberSlot(
      nwk_neighbour.ieee_addr, -1, 0, ALL_SUPLA_CHANNEL_TYPES, 
      NO_CUSTOM_CMD_SID);
    
    if (channel_number_slot < 0) {
      
      sprintf(
        log_line, 
        PSTR("Z2S_nwk_scan_neighbourhood - "
             "no channel found for address 0x%x"),
        nwk_neighbour.short_addr);

      log_i_telnet2(log_line, toTelnet);
    } else {
      
      while (channel_number_slot >= 0) {
      auto element = 
        Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

      if (element) 
        element->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(nwk_neighbour.rssi));
        
      channel_number_slot = Z2S_findChannelNumberNextSlot(
        channel_number_slot, nwk_neighbour.ieee_addr, -1, 0, 
        ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
      }
    }  
    
    scan_result = esp_zb_nwk_get_next_neighbor(&nwk_iterator, &nwk_neighbour);
    
  }
  if (scan_result == ESP_ERR_INVALID_ARG)
    log_i_telnet2("Z2S_nwk_scan_neighbourhood error ESP_ERR_INVALID_ARG", 
                  toTelnet);

  if (scan_result == ESP_ERR_NOT_FOUND)
    log_i_telnet2("Z2S_nwk_scan_neighbourhood scan completed", 
                  toTelnet);
}

void remoteNeighbourhoodTableCb(
  const esp_zb_zdo_mgmt_lqi_rsp_t *rsp, void *user_ctx) {

  char log_line[384];

  esp_zb_zdo_mgmt_lqi_req_param_t *req = 
    (esp_zb_zdo_mgmt_lqi_req_param_t *)user_ctx;

  esp_zb_zdp_status_t zdo_status = (esp_zb_zdp_status_t)rsp->status;
  
  log_d(
    "Remote neighbourhood table callback for address 0x%04x with status %d", 
     req->dst_addr, zdo_status);

  if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
  
    log_d("Remote neighbourhood table info: total %d, index %d, count %d", 
          rsp->neighbor_table_entries, 
          rsp->start_index, 
          rsp->neighbor_table_list_count);

    if (rsp->neighbor_table_entries == 0) {
      log_d("No remote neighbourhood table entries found");
      return;
    }

    esp_zb_zdo_neighbor_table_list_record_t *nwk_neighbour = 
      rsp->neighbor_table_list;

    for (int i = 0; i < rsp->neighbor_table_list_count; i++) {
    
      sprintf_P(log_line, 
              PSTR("Scan neighbour record number - 0x%x:\n\rIEEE ADDRESS\t\t"
                  "%X:%X:%X:%X:%X:%X:%X:%X\n\rSHORT ADDRESS\t\t0x%x\n\r"
                  "DEVICE TYPE\t\t0x%x\n\rDEPTH\t\t\t0x%x\n\rRX_ON_WHEN_IDLE"
                  "\t\t0x%x\n\rRELATIONSHIP\t\t0x%x\n\rLQI\t\t\t%d\n\r"), 
        rsp->start_index + rsp->neighbor_table_list_count, 
        nwk_neighbour->extended_addr[7], 
        nwk_neighbour->extended_addr[6], 
        nwk_neighbour->extended_addr[5], 
        nwk_neighbour->extended_addr[4], 
        nwk_neighbour->extended_addr[3], 
        nwk_neighbour->extended_addr[2], 
        nwk_neighbour->extended_addr[1], 
        nwk_neighbour->extended_addr[0],
        nwk_neighbour->network_addr, 
        nwk_neighbour->device_type,
        nwk_neighbour->depth, 
        nwk_neighbour->rx_when_idle, 
        nwk_neighbour->relationship,
        nwk_neighbour->lqi);

    log_i_telnet2(log_line, true); //toTelnet);  

      nwk_neighbour++;
    }

    if (rsp->start_index + rsp->neighbor_table_list_count < 
        rsp->neighbor_table_entries) {

      req->start_index = rsp->start_index + rsp->neighbor_table_list_count;
      esp_zb_zdo_mgmt_lqi_req(req, remoteNeighbourhoodTableCb, req);
    }
  }
}


void Z2S_scanRemoteNeighbourhoodTable(
  uint16_t short_address, bool toTelnet = false) {

  esp_zb_zdo_mgmt_lqi_req_param_t req;

  req.start_index = 0;
  req.dst_addr = short_address;

  esp_zb_zdo_mgmt_lqi_req(&req, remoteNeighbourhoodTableCb, (void *)&req);
}

bool getDeviceByChannelNumber(zbg_device_params_t *device, uint8_t channel_id) {

  int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
  
  if (channel_number_slot >= 0) {

    device->endpoint = 
      z2s_channels_table[channel_number_slot].endpoint;

    device->cluster_id = 
    z2s_channels_table[channel_number_slot].cluster_id;
    
    memcpy(device->ieee_addr, 
           z2s_channels_table[channel_number_slot].ieee_addr,
           sizeof(esp_zb_ieee_addr_t));

    device->short_addr = 
      z2s_channels_table[channel_number_slot].short_addr;

    device->model_id = 
      z2s_channels_table[channel_number_slot].model_id;

    telnet.printf(PSTR(">Device %u\n\r>"), device->short_addr);
    
    return true;

  } else {
    
    telnet.printf(PSTR(">Invalid channel number %u\n\r>"), channel_id);
    return false;
  }
}

uint8_t parseAttributeTypeStr(char *attribute_type) {
  
  if (strcmp(attribute_type, "BOOL") == 0)
    return 0x10;
  else
  if (strcmp(attribute_type, "8BITMAP") == 0)
    return 0x18;
  else
  if (strcmp(attribute_type, "16BITMAP") == 0)
    return 0x19;
  else
  if (strcmp(attribute_type, "U8") == 0)
    return 0x20;
  else
  if (strcmp(attribute_type, "U16") == 0)
    return 0x21;
  else
  if (strcmp(attribute_type, "S8") == 0)
    return 0x28;
  else
  if (strcmp(attribute_type, "S16") == 0)
    return 0x29;
  else
  if (strcmp(attribute_type, "ENUM8") == 0)
    return 0x30;
  else
  if (strcmp(attribute_type, "ENUM16") == 0)
    return 0x31;
  else
  if (strcmp(attribute_type, "ARRAY") == 0)
    return 0x48;
  else
  if (strcmp(attribute_type, "SET") == 0)
    return 0x50;
  else
    return strtoul(attribute_type,nullptr, 0);
}

uint16_t parseClusterIdStr(char *cluster_id) {
  
  if (strcmp(cluster_id, "ONOFF") == 0)
    return 0x06;
  else
  if (strcmp(cluster_id, "EM") == 0)
    return 0xB04;
  else
  if (strcmp(cluster_id, "SM") == 0)
    return 0x702;
  else
  if (strcmp(cluster_id, "TEMP") == 0)
    return 0x402;
  else
  if (strcmp(cluster_id, "HUMI") == 0)
    return 0x405;
  else
  if (strcmp(cluster_id, "IAS") == 0)
    return 0x500;
  else
  if (strcmp(cluster_id, "LEVEL") == 0)
    return 0x08;
  else
  if (strcmp(cluster_id, "POWER") == 0)
    return 0x01;
  else
  if (strcmp(cluster_id, "COLOR") == 0)
    return 0x300;
  else
  if (strcmp(cluster_id, "PRESSURE") == 0)
    return 0x403;
  else
  if (strcmp(cluster_id, "POLL") == 0)
    return 0x20;
  else
    return strtoul(cluster_id, nullptr, 0);
}

uint8_t parseTimingsStr(char *cluster_id) {
  
  if (strcmp(cluster_id, "KEEPALIVE") == 0)
    return 0x01;
  else
  if (strcmp(cluster_id, "TIMEOUT") == 0)
    return 0x02;
  else
  if (strcmp(cluster_id, "REFRESH") == 0)
    return 0x04;
  else
  if (strcmp(cluster_id, "AUTOSET") == 0)
    return 0x04;
  else
    return 0x00;
}

uint8_t parseRGBModeStr(char *rgb_mode) {
  
  if (strcmp(rgb_mode, "HS") == 0)
    return 0x01;
  else
  if (strcmp(rgb_mode, "XY") == 0)
    return 0x03;
  else
  if (strcmp(rgb_mode, "TUYA-HS") == 0)
    return 0x11;
  else
  if (strcmp(rgb_mode, "TUYA-XY") == 0)
    return 0x13;
  else
    return 0x00;
}

uint16_t parseSuplaEventStr(char *Supla_event) {
  
  if (strcmp(Supla_event, "ON_TURN_ON") == 0)
    return Supla::ON_TURN_ON;
  else
  if (strcmp(Supla_event, "ON_TURN_OFF") == 0)
    return Supla::ON_TURN_OFF;
  else
  if (strcmp(Supla_event, "ON_CHANGE") == 0)
    return Supla::ON_CHANGE;
  else
    return 0xFFFF;
}

uint16_t parseSuplaActionStr(char *Supla_action) {
  
  if (strcmp(Supla_action, "TURN_ON") == 0)
    return Supla::TURN_ON;
  else
  if (strcmp(Supla_action, "TURN_OFF") == 0)
    return Supla::TURN_OFF;
  else
  if (strcmp(Supla_action, "TOGGLE") == 0)
    return Supla::TOGGLE;
  else
  if (strcmp(Supla_action, "SET") == 0)
    return Supla::SET;
  else
  if (strcmp(Supla_action, "CLEAR") == 0)
    return Supla::CLEAR;
  else
  if (strcmp(Supla_action, 
      PSTR("INCREASE_TEMPERATURE")) == 0)
    return Supla::INCREASE_TEMPERATURE;
  else
  if (strcmp(Supla_action, 
      PSTR("DECREASE_TEMPERATURE")) == 0)
    return Supla::DECREASE_TEMPERATURE;
  else
  if (strcmp(Supla_action, 
      PSTR("TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES")) == 0)
    return Supla::TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES;
  else
  if (strcmp(Supla_action, "DIM_W") == 0)
    return Supla::DIM_W;
  else
  if (strcmp(Supla_action, "BRIGHTEN_W") == 0)
    return Supla::BRIGHTEN_W;
  else
    return 0xFFFF;
}

uint16_t parseSuplaConditionStr(char *Supla_condition) {
  
  if (strcmp(Supla_condition, "ON_LESS") == 0)
    return Supla::ON_LESS;
  else
  if (strcmp(Supla_condition, "ON_LESS_EQ") == 0)
    return Supla::ON_LESS_EQ;
  else
  if (strcmp(Supla_condition, "ON_GREATER") == 0)
    return Supla::ON_GREATER;
  else
  if (strcmp(Supla_condition, "ON_GREATER_EQ") == 0)
    return Supla::ON_GREATER_EQ;
  else
  if (strcmp(Supla_condition, "ON_BETWEEN") == 0)
    return Supla::ON_BETWEEN;
  else
  if (strcmp(Supla_condition, "ON_BETWEEN_EQ") == 0)
    return Supla::ON_BETWEEN_EQ;
  else
  if (strcmp(Supla_condition, "ON_EQUAL") == 0)
    return Supla::ON_EQUAL;
  else
    return 0xFFFF;
}

uint16_t parseBasicClusterAttributeStr(char *attribute) {
  
  if (strcmp(attribute, "MANUFACTURER_NAME") == 0)
    return ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID;
  else
  if (strcmp(attribute, "MODEL_ID") == 0)
    return ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID;
  else
  if (strcmp(attribute, "POWER_SOURCE") == 0)
    return ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID;
  else
  if (strcmp(attribute, "SW_BUILD_ID") == 0)
    return ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID;
  else
    return 0xFFFF;
}

uint8_t parseDeviceFlagsStr(char *device_flag) {
  
  if (strcmp(device_flag, "FLAG_DISABLE_NOTIFICATIONS") == 0)
    return USER_DATA_FLAG_DISABLE_NOTIFICATIONS;
  else
  if (strcmp(device_flag, "FLAG_TRV_AUTO_TO_SCHEDULE") == 0)
    return USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE;
  else
  if (strcmp(device_flag, "FLAG_IAS_MSG_DISABLED") == 0)
    return USER_DATA_FLAG_MSG_DISABLED;
  else
    return strtoul(device_flag, nullptr, 0);
}

uint8_t parseZbDeviceFlagsStr(char *zbdevice_flag) {
  
  if (strcmp(zbdevice_flag, 
      "ZBD_FLAG_DISABLE_BATTERY_MSG") == 0)
    return ZBD_USER_DATA_FLAG_DISABLE_BATTERY_MSG;
  else
  if (strcmp(zbdevice_flag, 
      "ZBD_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG") == 0)
    return ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG;
  else
  if (strcmp(zbdevice_flag, 
      "ZBD_FLAG_DISABLE_BATTERY_VOLTAGE_MSG") == 0)
    return ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG;
  else
    return strtoul(zbdevice_flag, nullptr, 0);
}

void Z2S_onTelnetCmd(char *cmd, uint8_t params_number, char **param) {
  
  zbg_device_params_t device;

  uint8_t write_mask;
  uint16_t write_mask_16;
  uint32_t write_mask_32;
  
  uint8_t custom_cmd_payload[64]; //TODO - include RAW/STRING 0.8.51 increased 10->64
  uint8_t write_attribute_payload[20];


  log_i("cmd: %s, param %s, param %s", cmd, *param != NULL ? *param : "-",
        *(param+1) != NULL ? *(param+1) : "-" );
  
  if (strcmp(cmd, "OPEN-NETWORK") == 0) {

    uint8_t time = 180;
    if (*(param))
      time = strtoul(*param, nullptr, 0);
    Zigbee.openNetwork(time);
    return;
  } else 
  if (strcmp(cmd, "GET-ZIGBEE-PRIMARY-CHANNEL") == 0) {

    uint32_t zb_primary_channel = esp_zb_get_primary_network_channel_set();
    for (uint8_t i = 11; i <= 26; i++) {
      if (zb_primary_channel & (1 << i))
        telnet.printf(PSTR("\n\rZigbee primary channel: %u\n\r>"), i);
    }
    return;
  } else
  if (strcmp(cmd, "SET-ZIGBEE-PRIMARY-CHANNEL") == 0) {
    
    if (params_number < 1)  {
      telnet.printf(
        "\n\rset-zigbee-primary-channel zigbee_channel(11-26)\n\r>");
      return;
    }
    
    uint32_t zb_primary_channel = strtoul(*param, nullptr, 0);

    if ((zb_primary_channel >= 11) && (zb_primary_channel <= 26)) {

      if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_ZIGBEE_PRIMARY_CHANNEL, 
          (1 << zb_primary_channel))) {
        
        telnet.printf(
          PSTR(
            "\n\rNew Zigbee primary channel(%u) mask(%x) "
            "write success! Restarting...\n\r>"), 
          zb_primary_channel, (1 << zb_primary_channel));

        Supla::Storage::ConfigInstance()->commit();
        SuplaDevice.scheduleSoftRestart(1000);
        return;
      } else {
        telnet.printf(
          PSTR(
            "\n\rNew Zigbee primary channel(%u) mask(%x) write failed!\n\r>"), 
          zb_primary_channel, (1 << zb_primary_channel));
        return;
      }
    } else { 
      telnet.printf(
        PSTR(
          "\n\rInvalid value for Zigbee primary channel: %u "
          "(should be between 11 and 26)\n\r>"));
      return;
    }
  } else
  if (strcmp(cmd, "GET-ZIGBEE-TX-POWER") == 0) {

    int8_t zb_tx_power;
    esp_zb_get_tx_power(&zb_tx_power);
    telnet.printf(PSTR("\n\rZigbee TX power: %d\n\r>"), zb_tx_power);
    return;
  } else
  if (strcmp(cmd, "SET-ZIGBEE-TX-POWER") == 0) {

    int8_t zb_tx_power = 20;
    if (*(param))
      zb_tx_power = strtoul(*param, nullptr, 0);
    if ((zb_tx_power <= 20) && (zb_tx_power>= -24))
      esp_zb_set_tx_power(zb_tx_power);
    else
      telnet.printf("\n\rInvalid TX power value for SET-ZIGBEE-TX-POWER: "
                    "%d (should be between -24 and +20)\n\r>", zb_tx_power);
    return;
  } else
  if (strcmp(cmd, "VERSION") == 0) {

    telnet.printf("\n\r>Gateway version: %s\n\r>", Z2S_VERSION);
    return;
  } else
  if (strcmp(cmd, "TASKS") == 0) {

    if ((params_number > 0) && (strcmp(*(param), "RESET"))) {

      telnet.printf("\n\r>RESETTING TASKS STATS - not yet implemented\n\r");
      //vTaskResetState();
    } else {
      
      telnet.printf("\n\r>TASKS CPU USAGE\n\r");
      printTaskInfo(true);
      telnet.printf("\n\r>");
    }
    return;
  } else
  if (strcmp(cmd, "DIR") == 0) {

    telnet.printf("\n\r>FILE SYSTEM\n\r");
    LittleFS.begin(false);
    printDir(LittleFS,"/",3);
    LittleFS.end();
    telnet.printf("--------------------\n\r>");
    return;
  } else
  if (strcmp(cmd, "TIME") == 0) {

    time_t now = time(nullptr);
    telnet.printf("\n\r>Gateway current time: %s\n\r>", ctime(&now));
    return;
  } else
  if (strcmp(cmd, "RESTART") == 0) {

    uint32_t restart_ms = 1000;

    if (params_number > 0)
      restart_ms = strtoul(*(param), nullptr, 0);

    telnet.printf("\n\rRestarting gateway in %u ms!\n\r", restart_ms);

    Supla::Storage::ConfigInstance()->commit();
    SuplaDevice.scheduleSoftRestart(restart_ms);
    return;
  } else
  if (strcmp(cmd, "START-GUI") == 0) {

    if (GUIstarted)
      telnet.printf("GUI already started!");
    else {

      gui_modes_t gui_mode = minimal_gui_mode;
      uint32_t gui_custom_flags = 0x0000;

      GUIstarted = true;

      if (params_number > 1)
        gui_custom_flags = strtoul(*(param + 1), nullptr, 0);
      if (params_number == 1)  
        gui_mode = (gui_modes_t)strtoul(*(param), nullptr, 0);

      telnet.printf("\n\rStarting GUI! Mode %u, flags %04x\n\r", gui_mode, 
        gui_custom_flags);

      Z2S_buildWebGUI(gui_mode, gui_custom_flags);
      Z2S_startWebGUI();
      Z2S_startUpdateServer();
      onTuyaCustomClusterReceive(GUI_onTuyaCustomClusterReceive);
    }
    return;
  } else
  if (strcmp(cmd, "MEMORY") == 0) {
  
    telnet.printf(
      "\n\rMemory information:"
      "\n\rFlash chip real size: %u B"
      "\n\rFree Sketch Space: %u B"
      "\n\rHeapSize: %u B"
			"\n\rFree Heap: %u B"
      "\n\rMaxAllocHeap: %u B"
      "\n\rMinimal Free Heap: %u B"
			"\n\rSupla uptime: %lu s\n\r\n\r>", 
			ESP.getFlashChipSize(), 
      ESP.getFreeSketchSpace(), 
      ESP.getHeapSize(),
      ESP.getFreeHeap(), 
      ESP.getMaxAllocHeap(),
      ESP.getMinFreeHeap(), 
      SuplaDevice.uptime.getUptime());
    return;
  } else
  if ((strcmp(cmd, "LIST-DEVICES") == 0) ||
      (strcmp(cmd, "LIST-CHANNELS") == 0)) {
    Z2S_printChannelsTableSlots(true);
    return;
  } else
  if (strcmp(cmd, "LIST-ZB-DEVICES") == 0) {
    Z2S_printZbDevicesTableSlots(true);
    return;
  } else
  if (strcmp(cmd,"REMOVE-CHANNEL") == 0) {

    if (params_number < 1)  {
      telnet.println(">remove-channel channel");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
      
      if (Z2S_removeChannel(channel_number_slot)) {
        log_i("Supla channel #%d removed. Restarting...", channel_id);
      SuplaDevice.scheduleSoftRestart(1000);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-SED-TIMEOUT") == 0) {

    telnet.println(">command update-sed-timeout depreciated");
    return;

    if (params_number < 2)  {
      telnet.println(">update-sed-timeout channel timeout(h)");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint8_t timeout = strtoul(*(param + 1), nullptr, 0);
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        updateTimeout(channel_number_slot, timeout);
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-DEVICE-TIMINGS") == 0) {

    if (params_number < 3)  {
      telnet.printf(
        "\n\rupdate-device-timings channel \"keepalive\"/\"timeout\"/"
        "\"refresh\"/\"autoset\" time(seconds)");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint8_t selector = parseTimingsStr(*(param + 1));
    uint32_t timings_secs = strtoul(*(param + 2), nullptr, 0);

    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        updateTimeout(channel_number_slot, 0, selector, timings_secs);
    } else {
      telnet.printf("\n\rInvalid channel number %u\n\r>", channel_id);
    }  
    return;
  }  else
  if (strcmp(cmd,"UPDATE-DEVICE-RGB-MODE") == 0) {

    if (params_number < 2)  {
      telnet.println(
        ">update-device-rgb-mode channel \"HS\"/\"XY\"/\"TUYA-HS\"/"
        "\"TUYA-XY\"");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint8_t rgb_mode = parseRGBModeStr(*(param + 1));

    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        updateRGBMode(channel_number_slot, rgb_mode);
    } else {
      telnet.printf("\n\rInvalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"SET-TEMP") == 0) {

    if (params_number < 2)  {
      telnet.println(">set-temp channel temperature*100");
      return;
    }
    uint8_t channel_id  = strtoul(*(param), nullptr, 0);
    int32_t temperature = strtoul(*(param+1), nullptr, 0);

    
    if (channel_id >= 0) {
        updateDeviceTemperature(channel_id, temperature);
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"ADD-ACTION") == 0) {

    if ((params_number != 5) && (params_number != 7))  {
      telnet.printf(
        "\n\radd-action action_name src_channel Supla_action dst_channel"
        "Supla_event");
      telnet.printf("\n\ror");
      telnet.printf(
        "\n\radd-action action_name src_channel Supla_action "
        "dst_channel Supla_condition threshold1 threshold2");
      return;
    }

    char *action_name        = *(param);
    uint8_t src_channel_id   = strtoul(*(param+1), nullptr, 0);
    uint8_t dst_channel_id   = strtoul(*(param+3), nullptr, 0);
    uint16_t Supla_event     = parseSuplaEventStr(*(param+4));
    uint16_t Supla_action    = parseSuplaActionStr(*(param+2));
    uint16_t Supla_condition = parseSuplaConditionStr(*(param+4));
    bool is_valid_action     = Supla_action < 0xFFFF ? true : false;
    bool is_valid_event      = Supla_event < 0xFFFF ? true : false;
    bool is_valid_condition  = Supla_condition < 0xFFFF ? true : false;
    double threshold_1;      
    double threshold_2; 
    
    if (is_valid_action && is_valid_event)
      Z2S_add_action(
        action_name, src_channel_id, Supla_action, dst_channel_id, 
        Supla_event, false);
    else
    if ((params_number == 7) && is_valid_action && is_valid_condition) {
      threshold_1       = strtod(*(param+5), nullptr);
      threshold_2       = strtod(*(param+6), nullptr);
      Z2S_add_action(
        action_name, src_channel_id, Supla_action, dst_channel_id, 
        Supla_condition, true, threshold_1, threshold_2);
    }
    return;
  } else
  if (strcmp(cmd,"UPDATE-DEVICE-DESC") == 0) {

    if (params_number < 2)  {
      telnet.println(">update-device-desc channel device_desc_id");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint32_t device_desc_id = strtoul(*(param + 1), nullptr, 0);
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        z2s_channels_table[channel_number_slot].model_id = device_desc_id;
      if (Z2S_saveChannelsTable()) {
        log_i("Device(channel %d) description id changed successfully.", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-DEVICE-SID") == 0) {

    if (params_number < 2)  {
      telnet.println(">update-device-sid channel device_sub_id");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    int8_t device_sub_id = strtoul(*(param + 1), nullptr, 0);
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        z2s_channels_table[channel_number_slot].sub_id = device_sub_id;
      if (Z2S_saveChannelsTable()) {
        log_i("Device(channel %d) sub id changed successfully.", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-CHANNEL-TYPE") == 0) {

    if (params_number < 2)  {
      telnet.println(">update-channel-type channel Supla_channel_type");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint32_t channel_type = strtoul(*(param + 1), nullptr, 0);
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        z2s_channels_table[channel_number_slot].Supla_channel_type = channel_type;
      if (Z2S_saveChannelsTable()) {
        log_i("Channel (%02u) type changed successfully.", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-CHANNEL-FUNC") == 0) {

    if (params_number < 2)  {
      telnet.println(">update-channel-type channel Supla_channel_func");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint32_t channel_func = strtoul(*(param + 1), nullptr, 0);
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        z2s_channels_table[channel_number_slot].Supla_channel_func = channel_func;
      if (Z2S_saveChannelsTable()) {
        log_i("Channel (%02u) function changed successfully.", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-DEVICE-FLAGS") == 0) {

    if (params_number < 3)  {
      telnet.println(">update-device-flags channel \"set\"/\"clear\" flag_bit");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    bool flag_set = (strcmp(*(param + 1), "SET") == 0);
    bool flag_clear = (strcmp(*(param + 1), "CLEAR") == 0);
    uint8_t bit_id = parseDeviceFlagsStr(*(param + 2));
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        if (flag_set)
          z2s_channels_table[channel_number_slot].user_data_flags |= (1 << bit_id);
        if (flag_clear)
          z2s_channels_table[channel_number_slot].user_data_flags &= ~(1 << bit_id);
      if (Z2S_saveChannelsTable()) {
        telnet.printf(">Device(channel %d) flags changed successfully.\n\r>", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
    if (strcmp(cmd,"UPDATE-ZB-DEVICE-FLAGS") == 0) {

    if (params_number < 3)  {
      telnet.println(">update-zb-device-flags channel \"set\"/\"clear\" flag_bit");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    bool flag_set = (strcmp(*(param + 1), "SET") == 0);
    bool flag_clear = (strcmp(*(param + 1), "CLEAR") == 0);
    uint8_t bit_id = parseZbDeviceFlagsStr(*(param + 2));
    
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    uint8_t zb_device_number_slot = (channel_number_slot >= 0) ? 
      Z2S_findZbDeviceTableSlot(z2s_channels_table[channel_number_slot].ieee_addr) : 0xFF;
    
    if (zb_device_number_slot < 0xFF) {
        if (flag_set)
          z2s_zb_devices_table[zb_device_number_slot].user_data_flags |= (1 << bit_id);
        if (flag_clear)
          z2s_zb_devices_table[zb_device_number_slot].user_data_flags &= ~(1 << bit_id);
      if (Z2S_saveZbDevicesTable()) {
        telnet.printf("ZB Device(channel %d) global flags changed successfully.\n\r>", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
    } else
    if (strcmp(cmd,"UPDATE-ZB-DEVICE-UID") == 0) {

    if (params_number < 2)  {
      telnet.println(">update-zb-device-uid zb_device_slot uid");
      return;
    }

    uint8_t device_slot = strtoul(*(param), nullptr, 0);
    uint32_t device_new_uid = strtoul(*(param + 1), nullptr, 0);
    
    if ((device_slot >= 0) && (device_slot < Z2S_ZB_DEVICES_MAX_NUMBER)) {

      if (z2s_zb_devices_table[device_slot].record_id >= 2) {

        z2s_zb_devices_table[device_slot].device_uid = device_new_uid;
        
        if (Z2S_saveZbDevicesTable()) {
          telnet.printf("ZB Device #%02u UID changed successfully to %lu!\n\r>", device_slot, device_new_uid);
        } else {
          telnet.printf("Error saving ZB Devices Table (device #%02u)!\n\r>", device_slot);
        }
      } else {
        telnet.printf("Error - ZB Device #%02u not in V2 data format!\n\r>", device_slot);
      }
    } else {
      telnet.printf("Error - zb_device_slot (%u) not in range [0..%u]\n\r>", device_slot, Z2S_ZB_DEVICES_MAX_NUMBER);
    }
    return;
  } else
  if (strcmp(cmd,"UPDATE-DEVICE-PARAMS") == 0) {

    if (params_number < 3)  {
      telnet.println(">update-device-params channel param_id param_value");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    int8_t param_id = strtoul(*(param + 1), nullptr, 0);
    int32_t param_value = strtoul(*(param + 2), nullptr, 0);
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
      switch (param_id) {
        case 1:
          z2s_channels_table[channel_number_slot].user_data_1 = param_value; break;
        case 2:
          z2s_channels_table[channel_number_slot].user_data_2 = param_value; break;
        case 3:
          z2s_channels_table[channel_number_slot].user_data_3 = param_value; break;
        case 4:
          z2s_channels_table[channel_number_slot].user_data_4 = param_value; break;
        default:
          telnet.printf(">param_id(%u) should be in range 1...4\n\r>", param_id); break;
      }
      if (Z2S_saveChannelsTable()) {
        log_i("Device(channel %d) user data daved successfully.", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd, "RESET-ZIGBEE-STACK") == 0) {

    Zigbee.factoryReset();
    return;
  } else
  if (strcmp(cmd,"NWK-SCAN")== 0) {
    if (params_number < 1)
      Z2S_nwk_scan_neighbourhood(true);
    else 
      Z2S_scanRemoteNeighbourhoodTable(strtoul(*(param), nullptr, 0));
    return;
  } else
  if (strcmp(cmd,"READ-ATTRIBUTE")== 0) {
    if (params_number < 3)  {
      telnet.println(">read-attribute channel cluster attribute");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t cluster_id = parseClusterIdStr(*(param + 1));
    uint16_t attribute_id = strtoul(*(param + 2),nullptr, 0);
    bool sync = (params_number > 3) ? (strcmp(*(param + 3),"ASYNC") == 0 ? false : true) : true;
    
    uint8_t disable_default_response = 1;
    uint8_t manuf_specific = 0;
    uint16_t manuf_code = 0;
    
    uint8_t direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV;

    if (params_number >= 5)
      direction = strtoul(*(param + 4),nullptr, 0);

    if (params_number >= 6)
      disable_default_response = strtoul(*(param + 5),nullptr, 0);

    if (params_number >= 7)
      manuf_specific = strtoul(*(param + 6),nullptr, 0);

    if (params_number >= 8)
      manuf_code = strtoul(*(param + 7),nullptr, 0);


    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">read-attribute %u %u %u\n\r>", channel_id, cluster_id, attribute_id);
      if (sync) {
        bool result = zbGateway.sendAttributeRead(
          &device, cluster_id, attribute_id, true, direction, 
          disable_default_response, manuf_specific, manuf_code); 
        if (result) {
          if (zbGateway.getReadAttrLastResult()->data.type == ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) {
            /*zbstring_t *zbstr = (zbstring_t *)zbGateway.getReadAttrLastResult()->data.value;
            if (zbstr->len == 0) {
              telnet.printf(">Reading attribute successful - data type is char string (0x42), string is empty");
              return;  
            }
            char *str_buf = (char *)malloc(zbstr->len + 1);
            memcpy(str_buf, zbstr->data, zbstr->len);
            *(str_buf + zbstr->len) = '\0';
            telnet.printf(">Reading attribute successful - data type is char string (0x42), string length is 0x%u, string data is %s\n\r>", 
                        zbstr->len, zbstr->data);
            free(str_buf);*/
            telnet.printf(">Reading attribute successful - data type is 0x%x, data size is %u\n\r>Data = ", zbGateway.getReadAttrLastResult()->data.type,
                          zbGateway.getReadAttrLastResult()->data.size);
            for (int8_t i = 0; i < zbGateway.getReadAttrLastResult()->data.size; i++)
              telnet.printf("0x%x, ", *(((uint8_t *)zbGateway.getReadAttrLastResult()->data.value) + i));
            telnet.printf("\n\r");
          } else
            telnet.printf(">Reading attribute successful - data value is 0x%x, data type is 0x%x\n\r>", 
                        *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);
        } else
          telnet.printf("\n\rReading attribute failed\n\r>");
      } else {
        zbGateway.sendAttributeRead(&device, cluster_id, attribute_id, false, direction);
        telnet.printf("\n\rreadAttribute async request sent\n\r>");
      }
    }
    return;
  } else
  if (strcmp(cmd,"CONFIGURE-REPORTING")== 0) {
    if (params_number < 7)  {
      telnet.printf(
        "\n\rconfigure-reporting channel cluster attribute attribute_type "
        "min_interval max_interval delta\n\r>");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t cluster_id = parseClusterIdStr(*(param + 1));
    uint16_t attribute_id = strtoul(*(param + 2),nullptr, 0);
    uint8_t attribute_type = parseAttributeTypeStr(*(param + 3));
    uint16_t min_interval = strtoul(*(param + 4),nullptr, 0);
    uint16_t max_interval = strtoul(*(param + 5),nullptr, 0);
    uint16_t delta = strtoul(*(param + 6),nullptr, 0);
    bool sync = (params_number > 7) ? 
      (strcmp(*(param + 7),"ASYNC") == 0 ? false : true) : true;
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(
        "\n\rconfigure-reporting %u 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n\r>", 
        channel_id, cluster_id, attribute_id, attribute_type,min_interval,
        max_interval, delta);

      if (sync) {
        zbGateway.setClusterReporting(
          &device, cluster_id, attribute_id, attribute_type, min_interval,
          max_interval, delta, true); 
        telnet.printf("\n\rConfigure reporting sync request sent\n\r>");
      } else {
        zbGateway.setClusterReporting(
          &device, cluster_id, attribute_id, attribute_type, min_interval, 
          max_interval, delta, false); 
        
        telnet.printf("\n\rConfigure reporting async request sent\n\r>");
      }
    }
    return;
  } else
  if (strcmp(cmd,"DEVICE-DISCOVERY")== 0) {
    if (params_number < 1)  {
      telnet.println("device-discovery channel");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">device-discovery %u 0x%X\n\r>", channel_id, device.short_addr);
      zbGateway.zbPrintDeviceDiscovery(&device); 
    }
    return;
  } else
  if (strcmp(cmd,"QUERY-DEVICE-INFO")== 0) {
    if (params_number < 2)  {
      telnet.println("query-device-info channel \"manufacturer_name\"/\"model_id\"/\"power_source\"/\"sw_build_id\"");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t attribute_id = parseBasicClusterAttributeStr(*(param + 1));
    
    if (attribute_id == 0xFFFF) {
      telnet.printf(">query-device-info error - unrecognized attribute %s\n\r", *(param + 1));
      return;
    }

    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">query-device-info channel:%u, short address: 0x%X\n\r>", channel_id, device.short_addr);
      
      if (zbGateway.zbQueryDeviceBasicCluster(&device, true, attribute_id)) {
        switch (attribute_id) {
          

          case ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID:
            
            telnet.printf(">Device 0x%X manufacturer name: %s\n\r", 
                          device.short_addr, 
                          zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name); break;


          case ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID:
            
            telnet.printf(">Device 0x%X model name: %s\n\r", 
                          device.short_addr, 
                          zbGateway.getQueryBasicClusterData()->zcl_model_name); break;


          case ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID:
            
            telnet.printf(">Device 0x%X power source: 0x%x\n\r", 
                          device.short_addr, 
                          zbGateway.getQueryBasicClusterData()->zcl_power_source_id); break;


          case ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID:
            
            telnet.printf(">Device 0x%X firmware build: %s\n\r", 
                          device.short_addr, 
                          zbGateway.getQueryBasicClusterData()->software_build_ID); break;
        }
      } else

        telnet.println("read-device-firmware failed :-(");
    }
    return;
  } else
  if (strcmp(cmd,"WRITE-ATTRIBUTE")== 0) {
  
    if (params_number < 6)  {
      telnet.printf(
        "\n\rwrite-attribute channel cluster attribute attribute_type "
        "attribute_size value\n\r>");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t cluster_id = parseClusterIdStr(*(param + 1));
    uint16_t attribute_id = strtoul(*(param + 2),nullptr, 0);
    esp_zb_zcl_attr_type_t attribute_type = (esp_zb_zcl_attr_type_t)parseAttributeTypeStr(*(param + 3));
    uint16_t attribute_size = strtoul(*(param + 4),nullptr, 0);
    uint8_t manuf_specific = 0;
    uint16_t manuf_code = 0;
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">write-attribute %u 0x%X 0x%X 0x%X 0x%X\n\r>", channel_id, cluster_id, attribute_id, attribute_type,
                    attribute_size);
      
      if (params_number == 8) {
        manuf_specific = strtoul(*(param + 6),nullptr, 0);
        manuf_code = strtoul(*(param + 7),nullptr, 0);
      }

      void *value = nullptr;
      
      if ((attribute_type >= ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) && (attribute_type <= ESP_ZB_ZCL_ATTR_TYPE_BAG))  {
        
        char byte_str[3];
        byte_str[2] = '\0';

        memset(write_attribute_payload, 0, sizeof(write_attribute_payload));

        for (int i = 0; i < attribute_size/*strlen(*(param + 5))) / 2*/; i++) {
          memcpy(byte_str,(*(param + 5))  + (i * 2), 2);
          write_attribute_payload[i] = strtoul(byte_str, nullptr, 16); //here hex base must be explicit
          telnet.printf("%X:", write_attribute_payload[i]);
        }
        value = write_attribute_payload;
      } else {
        switch (attribute_size) {
          case 1: {
            write_mask = strtoul(*(param + 5),nullptr, 0);
            value = &write_mask; 
          } break;
          case 2: {
            write_mask_16 = strtoul(*(param + 5),nullptr, 0);
            value = &write_mask_16; 
          } break;
          case 4: {
            write_mask_32 = strtoul(*(param + 5),nullptr, 0);
            value = &write_mask_32; 
          } break;
        }
      }
      if (value)
        zbGateway.sendAttributeWrite(
          &device, cluster_id, attribute_id, attribute_type, attribute_size, 
          value, manuf_specific, manuf_code); 

      telnet.printf("\n\rWrite attribute async request sent\n\r>");
    } 
    return;
  } else
  if (strcmp(cmd,"CUSTOM-CMD")== 0) {
  
    if (params_number < 6)  {
      telnet.println("custom-cmd channel cluster command data_type data_size value");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t cluster_id = parseClusterIdStr(*(param + 1));
    uint16_t command_id = strtoul(*(param + 2),nullptr, 0);
    esp_zb_zcl_attr_type_t data_type = 
      (esp_zb_zcl_attr_type_t)parseAttributeTypeStr(*(param + 3));
    uint16_t data_size = strtoul(*(param + 4),nullptr, 0);
    uint8_t direction = 0;
    uint8_t disable_default_response = 0;
    uint8_t manuf_specific = 0;
    uint8_t manuf_code = 0;
    
    bool sync = (params_number >= 7) ? 
                (strcmp(*(param + 6),"ASYNC") == 0 ? false : true) : false;

    if (params_number >= 8) 
      direction = strtoul(*(param + 7),nullptr, 0);

    if (params_number >= 9)
      disable_default_response = strtoul(*(param + 8),nullptr, 0);

    if (params_number >= 10)
      manuf_specific = strtoul(*(param + 9),nullptr, 0);

    if (params_number >= 11)
      manuf_code = strtoul(*(param + 10),nullptr, 0);
      
    if (data_size > sizeof(custom_cmd_payload)) {
      log_e("Custom command data size(%u) to big(max. %u)!",
            data_size, sizeof(custom_cmd_payload));
      return;
    }

    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">custom-cmd %u 0x%X 0x%X 0x%X 0x%X\n\r>", 
                    channel_id, cluster_id, command_id, data_type,
                    data_size);
      
      char byte_str[3];
      byte_str[2] = '\0';

      memset(custom_cmd_payload, 0, sizeof(custom_cmd_payload));

      for (int i = 0; i < data_size/*strlen(*(param + 5))) / 2*/; i++) {
        memcpy(byte_str,(*(param + 5))  + (i * 2), 2);
        custom_cmd_payload[i] = strtoul(byte_str, nullptr, 16); //here hex base must be explicit
        telnet.printf("%X:", custom_cmd_payload[i]);
      }
      zbGateway.sendCustomClusterCmd(
        &device, cluster_id, command_id, data_type, data_size, 
        custom_cmd_payload, sync, direction, disable_default_response, 
        manuf_specific, manuf_code); 

      if (!sync) telnet.printf("Custom command async request sent\n\r>");
    }
    return;
  } else
  if (strcmp(cmd,"READ-RSSI")== 0) {
  
    if (params_number < 1)  {
      telnet.println("read-rssi channel");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    if (getDeviceByChannelNumber(&device, channel_id)) {

      if (zbGateway.sendCustomClusterCmd(
			  &device, 0x0003, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, nullptr, 
        true))
			  telnet.printf("\n\rdevice: %4X, rssi = %d\n\r>", device.short_addr, 
        zbGateway.getZbgDeviceUnitLastRssi(device.short_addr));
      else
        telnet.printf("\n\rdevice: %4X, rssi query failed!\n\r>", 
          device.short_addr);
    }
  } else
    telnet.printf("\n\rUnrecognized command!\n\r>");
}

