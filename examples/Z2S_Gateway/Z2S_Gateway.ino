//#pragma once
#pragma warning(disable: -Wmissing-field-initializers)

#define Z2S_GATEWAY

#include <ZigbeeGateway.h>

#include "esp_coexist.h"

#include <SuplaDevice.h>

#include "priv_auth_data.h"
#include "z2s_devices_database.h"
#include "z2s_devices_table.h"

#include <supla/network/esp_wifi.h>
#include <supla/device/supla_ca_cert.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/clock/clock.h>
#include <supla/actions.h>
#include <supla/control/button.h>
#include <action_handler_with_callbacks.h>

#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include "z2s_version_info.h"
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/network/html/text_cmd_input_parameter.h>
#include <supla/network/html/select_cmd_input_parameter.h>
#include <supla/network/html/select_input_parameter.h>

#include <HTTPUpdateServer.h>

Supla::EspWebServer                       suplaServer;
HTTPUpdateServer                          httpUpdater;

Supla::Html::DeviceInfo                   htmlDeviceInfo(&SuplaDevice);
Supla::Html::CustomDevInfo                htmlCustomDevInfo;
Supla::Html::WifiParameters               htmlWifi;
Supla::Html::ProtocolParameters           htmlProto;

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2
#define CFG_BUTTON_PIN              9  //Boot button for C6/H2
#define WIFI_ENABLE                 3
#define WIFI_ANT_CONFIG             14

#define REFRESH_PERIOD              10 * 1000 //miliseconds

#define USE_WEB_CONFIG_ON_STARTUP

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi; //(SUPLA_WIFI_SSID, SUPLA_WIFI_SSID);
Supla::LittleFsConfig     configSupla (2048);

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

uint32_t refresh_time = 0;
uint8_t refresh_cycle = 0;

bool zbInit = true;
uint8_t write_mask;
uint16_t write_mask_16;
uint32_t write_mask_32;

uint8_t custom_cmd_payload[10]; //TODO - include RAW/STRING


const static char PARAM_CMD1[] = "zigbeestack";
const static char PARAM_CMD2[] = "RMZ2Sdevices";
const static char PARAM_CMD3[] = "UPZ2Sdevices";
const static char PARAM_TXT1[] = "SEDtimeout";

void Z2S_nwk_scan_neighbourhood(bool toTelnet = false) {

  esp_zb_nwk_neighbor_info_t nwk_neighbour;
  esp_zb_nwk_info_iterator_t nwk_iterator = 0;
  esp_err_t scan_result;
  //esp_zb_lock_acquire(portMAX_DELAY);
  scan_result = esp_zb_nwk_get_next_neighbor(&nwk_iterator, &nwk_neighbour);
  //esp_zb_lock_release();
  char log_line[384];
  
  if (scan_result == ESP_ERR_NOT_FOUND)
    log_i_telnet("\033[1mZ2S_nwk_scan_neighbourhood scan empty :-(  \033[22m");

  while (scan_result == ESP_OK) {
    sprintf(log_line, "Scan neighbour record number - 0x%x:\n\rIEEE ADDRESS\t\t%X:%X:%X:%X:%X:%X:%X:%X\n\rSHORT ADDRESS\t\t0x%x\n"
                      "\rDEPTH\t\t\t0x%x\n\rRX_ON_WHEN_IDLE\t\t0x%x\n\rRELATIONSHIP\t\t0x%x\n\rLQI\t\t\t%d\n\rRSSI\t\t\t%d\n\rOUTGOING COST\t\t0x%x\n"
                      "\rAGE\t\t\t0x%x\n\rDEVICE TIMEOUT\t\t%lu\n\rTIMEOUT COUNTER\t\t%lu", 
        nwk_iterator, 
        nwk_neighbour.ieee_addr[7], nwk_neighbour.ieee_addr[6], nwk_neighbour.ieee_addr[5], nwk_neighbour.ieee_addr[4], 
        nwk_neighbour.ieee_addr[3], nwk_neighbour.ieee_addr[2], nwk_neighbour.ieee_addr[1], nwk_neighbour.ieee_addr[0],
        nwk_neighbour.short_addr, nwk_neighbour.depth, nwk_neighbour.rx_on_when_idle, nwk_neighbour.relationship,
        nwk_neighbour.lqi, nwk_neighbour.rssi, nwk_neighbour.outgoing_cost, nwk_neighbour.age, nwk_neighbour.device_timeout,
        nwk_neighbour.timeout_counter);
    log_i_telnet(log_line, toTelnet);
    
    int16_t channel_number_slot = Z2S_findChannelNumberSlot(nwk_neighbour.ieee_addr, -1, 0, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
    
    if (channel_number_slot < 0) {
      sprintf(log_line, "Z2S_nwk_scan_neighbourhood - no channel found for address 0x%x", nwk_neighbour.short_addr);
      log_i_telnet(log_line, toTelnet);
    }
    else {
      while (channel_number_slot >= 0) {
      auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
      if (element) 
        element->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(nwk_neighbour.rssi));
        channel_number_slot = Z2S_findChannelNumberNextSlot(channel_number_slot, nwk_neighbour.ieee_addr, -1, 0, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
      }
    }  
    //esp_zb_lock_acquire(portMAX_DELAY);      
    scan_result = esp_zb_nwk_get_next_neighbor(&nwk_iterator, &nwk_neighbour);
    //esp_zb_lock_release();
  }
  if (scan_result == ESP_ERR_INVALID_ARG)
    log_i_telnet("Z2S_nwk_scan_neighbourhood error ESP_ERR_INVALID_ARG", toTelnet);

  if (scan_result == ESP_ERR_NOT_FOUND)
    log_i_telnet("Z2S_nwk_scan_neighbourhood scan completed", toTelnet);
}


void supla_callback_bridge(int event, int action) {
  log_i("supla_callback_bridge - event(0x%x), action(0x%x)", event, action);
  switch (event) {
    case Supla::ON_EVENT_1:
    case Supla::ON_CLICK_1: Zigbee.openNetwork(180); break;
    case Supla::ON_EVENT_2:
    case Supla::ON_CLICK_5: Zigbee.factoryReset(); break;
    case Supla::ON_EVENT_3: 
    case Supla::ON_CLICK_10: Z2S_clearDevicesTable(); break;
    case Supla::ON_EVENT_4: Z2S_nwk_scan_neighbourhood(false); break;
  }
  if ((event >= Supla::ON_EVENT_5) && (event < Supla::ON_EVENT_5 + Z2S_CHANNELMAXCOUNT)) {
    z2s_devices_table[event - Supla::ON_EVENT_5].valid_record = false;
    if (Z2S_saveDevicesTable()) {
      log_i("Device on channel %d removed. Restarting...", z2s_devices_table[event - Supla::ON_EVENT_5].Supla_channel);
      SuplaDevice.scheduleSoftRestart(1000);
    }
  }
  if ((event >= Supla::ON_EVENT_5 + Z2S_CHANNELMAXCOUNT) && (event < Supla::ON_EVENT_5 + 2*Z2S_CHANNELMAXCOUNT)) {
    int32_t timeout = 0;
    Supla::Storage::ConfigInstance()->getInt32(PARAM_TXT1, &timeout);
    log_i("Timeout is %d", timeout);   
    updateTimeout(event - (Supla::ON_EVENT_5 + Z2S_CHANNELMAXCOUNT), timeout);
  }
}

bool getDeviceByChannelNumber(zbg_device_params_t *device, uint8_t channel_id) {

  int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
  
  if (channel_number_slot >= 0) {

    device->endpoint = z2s_devices_table[channel_number_slot].endpoint;
    device->cluster_id = z2s_devices_table[channel_number_slot].cluster_id;
    memcpy(device->ieee_addr, z2s_devices_table[channel_number_slot].ieee_addr,8);
    device->short_addr = z2s_devices_table[channel_number_slot].short_addr;
    device->model_id = z2s_devices_table[channel_number_slot].model_id;
    telnet.printf(">Device %u\n\r>", device->short_addr);
    return true;
  } else {
    telnet.printf(">Invalid channel number %u\n\r>", channel_id);
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
    return strtoul(cluster_id,nullptr, 0);
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


void Z2S_onTelnetCmd(char *cmd, uint8_t params_number, char **param) {
  
  zbg_device_params_t device;

  log_i("cmd: %s, param %s, param %s", cmd, *param != NULL ? *param : "-",*(param+1) != NULL ? *(param+1) : "-" );
  
  if (strcmp(cmd, "OPEN-NETWORK") == 0) {

    uint8_t time = 180;
    if (*(param))
      time = strtoul(*param, nullptr, 0);
    Zigbee.openNetwork(time);
    return;
  } else
  if (strcmp(cmd, "VERSION") == 0) {
    telnet.printf(">Gateway version: %s\n\r>", Z2S_VERSION);
    return;
  } else
  if (strcmp(cmd, "LIST-DEVICES") == 0) {
    Z2S_printDevicesTableSlots(true);
    return;
  } else
  if (strcmp(cmd, "LIST-ZB-DEVICES") == 0) {
    Z2S_printZBDevicesTableSlots(true);
    return;
  } else
  if (strcmp(cmd,"LIST-CHANNELs") == 0) {
    
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
      z2s_devices_table[channel_number_slot].valid_record = false;
      if (Z2S_saveDevicesTable()) {
        log_i("Device on channel %d removed. Restarting...", channel_id);
      SuplaDevice.scheduleSoftRestart(1000);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  } else
  if (strcmp(cmd,"UPDATE-SED-TIMEOUT") == 0) {

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
      telnet.println(">update-device-timings channel \"keepalive\"/\"timeout\"/\"refresh\" time(seconds)");
      return;
    }

    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint8_t selector = parseTimingsStr(*(param + 1));
    uint32_t timings_secs = strtoul(*(param + 2), nullptr, 0);

    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        updateTimeout(channel_number_slot, 0, selector, timings_secs);
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
    }  
    return;
  }  else
  if (strcmp(cmd,"UPDATE-DEVICE-RGB-MODE") == 0) {

    if (params_number < 2)  {
      telnet.println(">update-device-rgb-mode channel \"HS\"/\"XY\"/\"TUYA-HS\"/\"TUYA-XY\"");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint8_t rgb_mode = parseRGBModeStr(*(param + 1));

    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    if (channel_number_slot >= 0) {
        updateRGBMode(channel_number_slot, rgb_mode);
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
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
        z2s_devices_table[channel_number_slot].model_id = device_desc_id;
      if (Z2S_saveDevicesTable()) {
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
        z2s_devices_table[channel_number_slot].sub_id = device_sub_id;
      if (Z2S_saveDevicesTable()) {
        log_i("Device(channel %d) description id changed successfully.", channel_id);
      }
    } else {
      telnet.printf(">Invalid channel number %u\n\r>", channel_id);
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
          z2s_devices_table[channel_number_slot].user_data_1 = param_value; break;
        case 2:
          z2s_devices_table[channel_number_slot].user_data_2 = param_value; break;
        case 3:
          z2s_devices_table[channel_number_slot].user_data_3 = param_value; break;
        case 4:
          z2s_devices_table[channel_number_slot].user_data_4 = param_value; break;
        default:
          telnet.printf(">param_id(%u) should be in range 1...4\n\r>", param_id);

      if (Z2S_saveDevicesTable()) {
        log_i("Device(channel %d) description id changed successfully.", channel_id);
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
  
    Z2S_nwk_scan_neighbourhood(true);
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
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">read-attribute %u %u %u\n\r>", channel_id, cluster_id, attribute_id);
      if (sync) {
        bool result = zbGateway.sendAttributeRead(&device, cluster_id, attribute_id, true); 
        if (result)
          telnet.printf(">Reading attribute successful - data value is 0x%x, data type is 0x%x\n\r>", 
                        *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);
        else
          telnet.printf(">Reading attribute failed\n\r>");
      } else {
        zbGateway.sendAttributeRead(&device, cluster_id, attribute_id, false);
        telnet.println("readAttribute async request sent");
      }
    }
    return;
  } else
  if (strcmp(cmd,"CONFIGURE-REPORTING")== 0) {
    if (params_number < 7)  {
      telnet.println("configure-reporting channel cluster attribute attribute_type min_interval max_interval delta");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t cluster_id = parseClusterIdStr(*(param + 1));
    uint16_t attribute_id = strtoul(*(param + 2),nullptr, 0);
    uint8_t attribute_type = parseAttributeTypeStr(*(param + 3));
    uint16_t min_interval = strtoul(*(param + 4),nullptr, 0);
    uint16_t max_interval = strtoul(*(param + 5),nullptr, 0);
    uint16_t delta = strtoul(*(param + 6),nullptr, 0);
    bool sync = (params_number > 7) ? (strcmp(*(param+7),"ASYNC") == 0 ? false : true) : true;
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">configure-reporting %u 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n\r>", channel_id, cluster_id, attribute_id, attribute_type,
                    min_interval, max_interval, delta);
      if (sync) {
        zbGateway.setClusterReporting(&device, cluster_id, attribute_id, attribute_type, min_interval, max_interval, delta, true); 
        telnet.printf(">Configure reporting sync request sent\n\r");
      } else {
        zbGateway.setClusterReporting(&device, cluster_id, attribute_id, attribute_type, min_interval, max_interval, delta, false); 
        telnet.println("Configure reporting async request sent");
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
  if (strcmp(cmd,"WRITE-ATTRIBUTE")== 0) {
  
    if (params_number < 6)  {
      telnet.println("write-attribute channel cluster attribute attribute_type attribute_size value");
      return;
    }
    uint8_t channel_id = strtoul(*(param), nullptr, 0);
    uint16_t cluster_id = parseClusterIdStr(*(param + 1));
    uint16_t attribute_id = strtoul(*(param + 2),nullptr, 0);
    esp_zb_zcl_attr_type_t attribute_type = (esp_zb_zcl_attr_type_t)parseAttributeTypeStr(*(param + 3));
    uint16_t attribute_size = strtoul(*(param + 4),nullptr, 0);
    //uint32_t value = strtoul(*(param + 5),nullptr,16);
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">write-attribute %u 0x%X 0x%X 0x%X 0x%X\n\r>", channel_id, cluster_id, attribute_id, attribute_type,
                    attribute_size);
      
      void *value;
      
      switch (attribute_size) {
        case 1: {
          write_mask = strtoul(*(param + 5),nullptr, 0);
          value = &write_mask; 
        } break;
        case 2: {
          write_mask_16 = strtoul(*(param + 5),nullptr, 0);
          value = &write_mask_16; 
        }break;
        case 4: {
          write_mask_32 = strtoul(*(param + 5),nullptr, 0);
          value = &write_mask_32; 
        } break;
      }
      zbGateway.sendAttributeWrite(&device, cluster_id, attribute_id, attribute_type, attribute_size, value); 
      telnet.println("Write attribute async request sent");
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
    esp_zb_zcl_attr_type_t data_type = (esp_zb_zcl_attr_type_t)parseAttributeTypeStr(*(param + 3));
    uint16_t data_size = strtoul(*(param + 4),nullptr, 0);
    //uint32_t value = strtoul(*(param + 5),nullptr,16);
    
    if (getDeviceByChannelNumber(&device, channel_id)) {

      telnet.printf(">custom-cmd %u 0x%X 0x%X 0x%X 0x%X\n\r>", channel_id, cluster_id, command_id, data_type,
                    data_size);
      
      char byte_str[3];
      byte_str[2] = '\0';

      memset(custom_cmd_payload, 0, sizeof(custom_cmd_payload));

      for (int i = 0; i < data_size/*strlen(*(param + 5))) / 2*/; i++) {
        memcpy(byte_str,(*(param + 5))  + (i * 2), 2);
        custom_cmd_payload[i] = strtoul(byte_str, nullptr, 16); //here hex base must be explicit
        telnet.printf("%X:", custom_cmd_payload[i]);
      }
      zbGateway.sendCustomClusterCmd(&device, cluster_id, command_id, data_type, data_size, custom_cmd_payload); 
      telnet.println("Custom command async request sent");
    }
    return;
  }
}

void setup() {
  
  log_i("setup start");

  pinMode(BUTTON_PIN, INPUT);

  pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT); (credits @Zibi_007)
  digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control
  delay(100);
  pinMode(WIFI_ANT_CONFIG, OUTPUT); // pinMode(14, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH);
 
  eeprom.setStateSavePeriod(5000);

  new Supla::Clock;

  Supla::Storage::Init();

  auto cfg = Supla::Storage::ConfigInstance();

  cfg->commit();

#ifndef USE_WEB_CONFIG_ON_STARTUP

  log_i("no web config on startup - using hardcoded data");
  //cfg->setGUID(GUID);
  //cfg->setAuthKey(AUTHKEY);
  //cfg->generateGuidAndAuthkey();
  cfg->setWiFiSSID(SUPLA_WIFI_SSID);
  cfg->setWiFiPassword(SUPLA_WIFI_PASS);
  cfg->setSuplaServer(SUPLA_SVR);
  cfg->setEmail(SUPLA_EMAIL);

#endif

  auto selectCmd = new Supla::Html::SelectCmdInputParameter(PARAM_CMD1, "Z2S Commands");
  selectCmd->registerCmd("OPEN ZIGBEE NETWORK (180 SECONDS)", Supla::ON_EVENT_1);
  selectCmd->registerCmd("!RESET ZIGBEE STACK!", Supla::ON_EVENT_2);
  selectCmd->registerCmd("!!CLEAR Z2S TABLE!! (RESET RECOMMENDED)", Supla::ON_EVENT_3);
  selectCmd->registerCmd("NWK SCAN (EXPERIMENTAL)", Supla::ON_EVENT_4);
  
  //selectCmd->registerCmd("TOGGLE", Supla::ON_EVENT_3);

  auto AHwC = new Supla::ActionHandlerWithCallbacks();
  AHwC->setActionHandlerCallback(supla_callback_bridge);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_1, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_2, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_3, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_4, true);

  auto buttonCfg = new Supla::Control::Button(CFG_BUTTON_PIN, true, true);

  buttonCfg->setHoldTime(2000);
  buttonCfg->setMulticlickTime(500);

  buttonCfg->configureAsConfigButton(&SuplaDevice);

  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_1);
  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_5);
  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_10);

  Z2S_loadZBDevicesTable();

  Z2S_loadDevicesTable();

  Z2S_initZBDevices(millis());

  Z2S_initSuplaChannels();

  new Supla::Html::CustomParameter(PARAM_TXT1,"SED Timeout (h)", 0);
  
  auto selectCmd2 = new Supla::Html::SelectCmdInputParameter(PARAM_CMD2, "Remove Z2S Device");
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      char device_removal_cmd[128];
      sprintf(device_removal_cmd, "Remove Z2SDevice [%x:%x:%x:%x:%x:%x:%x:%x] channel # %d",
      z2s_devices_table[devices_counter].ieee_addr[7], z2s_devices_table[devices_counter].ieee_addr[6], z2s_devices_table[devices_counter].ieee_addr[5],
      z2s_devices_table[devices_counter].ieee_addr[4], z2s_devices_table[devices_counter].ieee_addr[3], z2s_devices_table[devices_counter].ieee_addr[2],
      z2s_devices_table[devices_counter].ieee_addr[1], z2s_devices_table[devices_counter].ieee_addr[0], z2s_devices_table[devices_counter].Supla_channel);
      log_i("cmd %s, len %d", device_removal_cmd, strlen(device_removal_cmd));
      selectCmd2->registerCmd(device_removal_cmd, Supla::ON_EVENT_5 + devices_counter);
      selectCmd2->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_5 + devices_counter, true);
    }

  auto selectCmd3 = new Supla::Html::SelectCmdInputParameter(PARAM_CMD3, "Update Z2S Device timeout (h)");
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      char device_removal_cmd[128];
      sprintf(device_removal_cmd, "Update Z2SDevice [%x:%x:%x:%x:%x:%x:%x:%x] channel # %d",
      z2s_devices_table[devices_counter].ieee_addr[7], z2s_devices_table[devices_counter].ieee_addr[6], z2s_devices_table[devices_counter].ieee_addr[5],
      z2s_devices_table[devices_counter].ieee_addr[4], z2s_devices_table[devices_counter].ieee_addr[3], z2s_devices_table[devices_counter].ieee_addr[2],
      z2s_devices_table[devices_counter].ieee_addr[1], z2s_devices_table[devices_counter].ieee_addr[0], z2s_devices_table[devices_counter].Supla_channel);
      log_i("cmd %s, len %d", device_removal_cmd, strlen(device_removal_cmd));
      selectCmd3->registerCmd(device_removal_cmd, Supla::ON_EVENT_5 + Z2S_CHANNELMAXCOUNT + devices_counter);
      selectCmd3->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_5 + Z2S_CHANNELMAXCOUNT + devices_counter, true);
    }
  //  Zigbee Gateway notifications

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onPressureReceive(Z2S_onPressureReceive);
  zbGateway.onIlluminanceReceive(Z2S_onIlluminanceReceive);
  zbGateway.onOccupancyReceive(Z2S_onOccupancyReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onRMSVoltageReceive(Z2S_onRMSVoltageReceive);
  zbGateway.onRMSCurrentReceive(Z2S_onRMSCurrentReceive);
  zbGateway.onRMSActivePowerReceive(Z2S_onRMSActivePowerReceive);
  zbGateway.onCurrentSummationReceive(Z2S_onCurrentSummationReceive);
  zbGateway.onCurrentLevelReceive(Z2S_onCurrentLevelReceive);
  zbGateway.onColorHueReceive(Z2S_onColorHueReceive);
  zbGateway.onColorSaturationReceive(Z2S_onColorSaturationReceive);
  zbGateway.onBatteryPercentageReceive(Z2S_onBatteryPercentageReceive);
  zbGateway.onCustomCmdReceive(Z2S_onCustomCmdReceive);

  zbGateway.onCmdCustomClusterReceive(Z2S_onCmdCustomClusterReceive);

  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.onDataSaveRequest(Z2S_onDataSaveRequest);

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  //Supla
  
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  
  SuplaDevice.setName("Zigbee to Supla Gateway");
  //wifi.enableSSL(true);

  SuplaDevice.begin();      
  
  httpUpdater.setup(suplaServer.getServerPtr(), "/update", "admin", "pass");

  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
  refresh_time = millis();
}

zbg_device_params_t *gateway_device;
zbg_device_params_t *joined_device;

uint8_t counter = 0;
uint8_t tuya_dp_data[10];

zbg_device_params_t test_device = {.model_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
.rejoined = true, .ZC_binding = true, .ieee_addr = {0,0,0,0,0,0,0,0}, .endpoint = 1, .cluster_id = 0, 
  .short_addr = 0, .user_data = 0};

void loop() {
  
  SuplaDevice.iterate();

  if (is_Telnet_server) telnet.loop();
  
  if ((!Zigbee.started()) && SuplaDevice.getCurrentStatus() == STATUS_REGISTERED_AND_READY) {
  
    log_i("Starting Zigbee subsystem");
    
    esp_coex_wifi_i154_enable();
  
    if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
      log_e("Zigbee failed to start! Rebooting...");
      SuplaDevice.scheduleSoftRestart(1000);
    }
    refresh_time = 0;

    SuplaDevice.handleAction(0, Supla::START_LOCAL_WEB_SERVER); //don't start local web server until Zigbee is ready
    setupTelnet();
    onTelnetCmd(Z2S_onTelnetCmd); 
  }
  
  //checking status of AC powered devices
  if (millis() - refresh_time > REFRESH_PERIOD) {

    for ([[maybe_unused]]const auto &device : zbGateway.getGatewayDevices()) {       

      if (refresh_cycle % 12 == 0) {//print every 120 seconds - only for debug purposes 
        log_i("Device on endpoint(0x%x), short address(0x%x), model id(0x%x), cluster id(0x%x), rejoined(%s)", 
              device->endpoint, device->short_addr, device->model_id, device->cluster_id, device->rejoined ? "YES" : "NO");
        log_i("Gateway version: %s", Z2S_VERSION);
      }
      if (refresh_cycle % 6 == 0)
        log_i("getZbgDeviceUnitLastSeenMs %d, current millis %d", zbGateway.getZbgDeviceUnitLastSeenMs(device->short_addr), millis()); 

    }
    if (!zbGateway.getGatewayDevices().empty()) {
      refresh_time = millis();
      refresh_cycle = (refresh_cycle + 1) % 12;
    }
  }

  if (zbGateway.isNewDeviceJoined()) {

    zbGateway.clearNewDeviceJoined();
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty())
    {
      joined_device = zbGateway.getLastJoinedDevice();
      
      //do some Tuya vodoo - just in case Tuya device is paired
      
      zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);
      
      if ((strlen(zbGateway.getQueryBasicClusterData()->zcl_model_name) == 0) ||
          (strlen(zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name) == 0))
        if (!zbGateway.zbQueryDeviceBasicCluster(joined_device)) {
        
          log_i("Error while pairing - cann't read manufacturer id. Gateway will restart, try to pair device once again!");
          SuplaDevice.scheduleSoftRestart(0);
        }
      write_mask = 0x13;
      zbGateway.sendAttributeWrite(joined_device, 0x0000, 0xffde, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask); //Tuya black magic continues
      write_mask = 0x1;
      zbGateway.sendAttributeWrite(joined_device, 0xfcc0, 0x0009, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, 1, 0x115f); //Lumi magic

      uint16_t devices_list_table_size = sizeof(Z2S_DEVICES_LIST)/sizeof(Z2S_DEVICES_LIST[0]);
      uint16_t devices_desc_table_size = sizeof(Z2S_DEVICES_DESC)/sizeof(Z2S_DEVICES_DESC[0]);
      bool device_recognized = false;

          for (int i = 0; i < devices_list_table_size; i++) { 
            
            if ((strcmp(zbGateway.getQueryBasicClusterData()->zcl_model_name, Z2S_DEVICES_LIST[i].model_name) == 0) &&
            (strcmp(zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name, Z2S_DEVICES_LIST[i].manufacturer_name) == 0)) {
              log_i(  "LIST matched %s::%s, entry # %d, endpoints # %d, endpoints 0x%x::0x%x,0x%x::0x%x,0x%x::0x%x,0x%x::0x%x",
                      Z2S_DEVICES_LIST[i].manufacturer_name, Z2S_DEVICES_LIST[i].model_name, i, 
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints_count,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[0].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[0].z2s_device_desc_id,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[1].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[1].z2s_device_desc_id,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[2].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[2].z2s_device_desc_id,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[3].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[3].z2s_device_desc_id );
  
              for (int j = 0; j < Z2S_DEVICES_LIST[i].z2s_device_endpoints_count; j++) {
              
                uint8_t endpoint_id = ( Z2S_DEVICES_LIST[i].z2s_device_endpoints_count == 1) ? 
                                        1 : Z2S_DEVICES_LIST[i].z2s_device_endpoints[j].endpoint_id; 
                                        
                uint32_t z2s_device_desc_id = ( Z2S_DEVICES_LIST[i].z2s_device_endpoints_count == 1) ?
                                                Z2S_DEVICES_LIST[i].z2s_device_desc_id :
                                                Z2S_DEVICES_LIST[i].z2s_device_endpoints[j].z2s_device_desc_id; 

                for (int k = 0; k < devices_desc_table_size; k++) {

                  if ( z2s_device_desc_id == Z2S_DEVICES_DESC[k].z2s_device_desc_id) {
                  log_i("DESC matched 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, endpoint 0x%x ",
                        Z2S_DEVICES_DESC[k].z2s_device_desc_id,   
                        Z2S_DEVICES_DESC[k].z2s_device_clusters_count,
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[0],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[1],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[2],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[3],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[4],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[5],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[6],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[7],
                        endpoint_id);

                        device_recognized = true;

                        joined_device->endpoint = endpoint_id;
                        joined_device->model_id = Z2S_DEVICES_DESC[k].z2s_device_desc_id;
                        
                        for (int m = 0; m < Z2S_DEVICES_DESC[k].z2s_device_clusters_count; m++)
                          zbGateway.bindDeviceCluster(joined_device, Z2S_DEVICES_DESC[k].z2s_device_clusters[m]);
                        if (endpoint_id == 1)
                          Z2S_addZBDeviceTableSlot(joined_device->ieee_addr,
                                                   joined_device->short_addr,
                                                   zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                                   zbGateway.getQueryBasicClusterData()->zcl_model_name,
                                                   Z2S_DEVICES_LIST[i].z2s_device_endpoints_count,
                                                   Z2S_DEVICES_LIST[i].z2s_device_desc_id,
                                                   zbGateway.getQueryBasicClusterData()->zcl_power_source_id);

                        switch (joined_device->model_id) {
                          case Z2S_DEVICE_DESC_TUYA_SWITCH_4X3: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                          } break;
                          
                          case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_1_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_1_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_1_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_2_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_2_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_2_HELD_SID);
                           } break;

                          case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_ROTATE_LEFT_SID);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                          } break;

                           case Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                          } break;

                          case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1:
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_PLAY_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_NEXT_TRACK_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_PREV_TRACK_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2:
                          case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3: 
                          case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
                            Z2S_addZ2SDevice(joined_device, TUYA_DOUBLE_DIMMER_SWITCH_1_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_DOUBLE_DIMMER_SWITCH_2_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_PRESENCE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_MOTION_STATE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR: {
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID);
                          } break;
                          /*case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_2_SID);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID);
                          }*/
                          default: Z2S_addZ2SDevice(joined_device, NO_CUSTOM_CMD_SID); 
                        }
                  }  
                  /*else 
                  log_i("DESC checking 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, endpoint %d ",
                        Z2S_DEVICES_DESC[k].z2s_device_desc_id,   
                        Z2S_DEVICES_DESC[k].z2s_device_clusters_count,
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[0],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[1],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[2],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[3], 
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[4],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[5],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[6],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[7],
                        endpoint_id);*/        
                  }
              }
              //here we can configure reporting and restart ESP32
              //zbGateway.sendDeviceFactoryReset(joined_device);
              switch (Z2S_DEVICES_LIST[i].z2s_device_desc_id) { //(joined_device->model_id) {

                case 0x0000: break;      
                
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1: {
                } break;

                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
                case Z2S_DEVICE_DESC_IKEA_RGBW_BULB: {
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID, true))
                    log_i("Color control caps 0x%x, type 0x%x", *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE100, true)) {
                    esp_zb_uint48_t *value = (esp_zb_uint48_t *)zbGateway.getReadAttrLastResult()->data.value;
                    log_i("Color full data 0x%x::0x%x, type 0x%x", value->low, value->high, zbGateway.getReadAttrLastResult()->data.type);
                  }
                    
                } break;

                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1: 
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER: {
                  /*zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 5, 3600, 5, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 5, 3600, 50, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 300, 5, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING,  
                                                ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, ESP_ZB_ZCL_ATTR_TYPE_U48, 5, 3600, 2, true);
                  */
                  
                } [[fallthrough]];//break;
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {
                  /*zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, false);
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, false);
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, false);
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, false);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID, true))
                    log_i("AC voltage multiplier 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID, true))
                    log_i("AC divisor 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID, true))
                    log_i("AC current multiplier 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID, true))
                    log_i("AC current divisor 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID, true))
                    log_i("AC power multiplier 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID, true))
                    log_i("AC power divisor 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                      ESP_ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, true))
                    log_i("Summation formating 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                      ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, true)) {
                    log_i("Metering multiplier 0x%x:0x%x", 
                          ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->low, ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->high);
                  }
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                      ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID, true))
                    log_i("Metering divisor 0x%x:0x%x", 
                          ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->low, ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->high);*/
                  //relay restore mode on startup
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x4003, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); //Tuya special
                }; break;
                
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: {
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true))
                      log_i("IAS_ZONE attribute has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, 
                            *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 0x0021, false);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                                ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 900, 1, false);
                } break;

                 case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
                 case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
                 case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {

                    joined_device->endpoint = 0x01;
                    if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,0x8004, true))
                      log_i("Tuya custom attribute 0x8004 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                    write_mask = 0x01; //Tuya switch mode, 0x00 - scenes
                    zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8004, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,0x8004, true))
                      log_i("Tuya custom attribute has 0x8004 been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                 } break;

                 case Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH:
                 case Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH:
                 case Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH: {
                   joined_device->endpoint = 0x01;
                    write_mask = 0xFF;
                    zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x4003, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    write_mask = 0x02;
                    zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); //Tuya special
                    for(int n = 0; n < Z2S_DEVICES_LIST[i].z2s_device_endpoints_count; n++) {
                      joined_device->endpoint = ( Z2S_DEVICES_LIST[i].z2s_device_endpoints_count == 1) ? 
                                                  1 : Z2S_DEVICES_LIST[i].z2s_device_endpoints[n].endpoint_id;;
                      //zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD010, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); 
                      //zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD030, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    }
                 } break;

                 case Z2S_DEVICE_DESC_RELAY_1: {
                    
                    joined_device->endpoint = 0x01;
                    write_mask = 0xFF;
                    zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x4003, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    write_mask = 0x02;
                    zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); //Tuya special
                 } break;
                 
                 case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
                 case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR: 
                 case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
                  zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL); break;

              }
              SuplaDevice.scheduleSoftRestart(5000);
            }   
            //else log_i("LIST checking %s::%s, entry # %d",Z2S_DEVICES_LIST[i].manufacturer_name, Z2S_DEVICES_LIST[i].model_name, i);
          }
      if (!device_recognized) log_d(  "Unknown model %s::%s, no binding is possible", zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                     zbGateway.getQueryBasicClusterData()->zcl_model_name);
    }
  }
}
