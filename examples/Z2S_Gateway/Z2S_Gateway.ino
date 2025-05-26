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
#include <supla/device/enter_cfg_mode_after_power_cycle.h>
#include <action_handler_with_callbacks.h>

#include <supla/control/virtual_relay.h>

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

#include <Adafruit_NeoPixel.h>

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

#define TIME_CLUSTER_REFRESH_MS     60 * 1000 //miliseconds

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi; //(SUPLA_WIFI_SSID, SUPLA_WIFI_SSID);
Supla::LittleFsConfig     configSupla (2048);

constexpr uint8_t LED_PIN = 8;
constexpr uint8_t NUM_LEDS = 1;

Adafruit_NeoPixel rgbLed(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

uint32_t refresh_time = 0;
uint8_t refresh_cycle = 0;

uint32_t _time_cluster_last_refresh_ms = 0;

uint32_t _status_led_last_refresh_ms = 0;
uint8_t _status_led_mode = 0;
uint8_t _status_led_last_mode = 0;
uint8_t _status_led_cycle = 0;

bool zbInit = true;
uint8_t write_mask;
uint16_t write_mask_16;
uint32_t write_mask_32;

uint8_t custom_cmd_payload[10]; //TODO - include RAW/STRING
uint8_t write_attribute_payload[20];


bool sendIASNotifications = false;

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
  switch (action) {
    case 0x4000: {
      sendIASNotifications = true; 
      return;
    } break;
    case 0x4001: {
      sendIASNotifications = false; 
      return;
    } break;
  }
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
  if (strcmp(Supla_action, "INCREASE_TEMPERATURE") == 0)
    return Supla::INCREASE_TEMPERATURE;
  else
  if (strcmp(Supla_action, "DECREASE_TEMPERATURE") == 0)
    return Supla::DECREASE_TEMPERATURE;
  else
  if (strcmp(Supla_action, "TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES") == 0)
    return Supla::TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES;
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

uint8_t parseZBDeviceFlagsStr(char *zbdevice_flag) {
  
  if (strcmp(zbdevice_flag, "ZBD_FLAG_DISABLE_BATTERY_MSG") == 0)
    return ZBD_USER_DATA_FLAG_DISABLE_BATTERY_MSG;
  else
  if (strcmp(zbdevice_flag, "ZBD_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG") == 0)
    return ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG;
  else
  if (strcmp(zbdevice_flag, "ZBD_FLAG_DISABLE_BATTERY_VOLTAGE_MSG") == 0)
    return ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG;
  else
    return strtoul(zbdevice_flag, nullptr, 0);
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
  if (strcmp(cmd, "GET-ZIGBEE-PRIMARY-CHANNEL") == 0) {

    uint32_t zb_primary_channel = esp_zb_get_primary_network_channel_set();
    for (uint8_t i = 11; i <= 26; i++) {
      if (zb_primary_channel & (1 << i))
        telnet.printf(">Zigbee primary channel: %u\n\r>", i);
    }
    return;
  } else
  if (strcmp(cmd, "SET-ZIGBEE-PRIMARY-CHANNEL") == 0) {
    
    if (params_number < 1)  {
      telnet.println(">set-zigbee-primary-channel zigbee_channel(11-26)");
      return;
    }
    
    uint32_t zb_primary_channel = strtoul(*param, nullptr, 0);

    if ((zb_primary_channel >= 11) && (zb_primary_channel <= 26)) {

      if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_ZIGBEE_PRIMARY_CHANNEL, (1 << zb_primary_channel))) {
        telnet.printf(">New Zigbee primary channel(%u) mask(%x) write success! Restarting...\n\r", zb_primary_channel, (1 << zb_primary_channel));
        Supla::Storage::ConfigInstance()->commit();
        SuplaDevice.scheduleSoftRestart(1000);
        return;
      } else {
        telnet.printf(">New Zigbee primary channel(%u) mask(%x) write failed!\n\r", zb_primary_channel, (1 << zb_primary_channel));
        return;
      }
    } else { 
      telnet.printf(">Invalid value for Zigbee primary channel: %u (should be between 11 and 26)\n\r");
      return;
    }
  } else
  if (strcmp(cmd, "GET-ZIGBEE-TX-POWER") == 0) {

    int8_t zb_tx_power;
    esp_zb_get_tx_power(&zb_tx_power);
    telnet.printf(">Zigbee TX power: %d\n\r>", zb_tx_power);
    return;
  } else
  if (strcmp(cmd, "SET-ZIGBEE-TX-POWER") == 0) {

    int8_t zb_tx_power = 20;
    if (*(param))
      zb_tx_power = strtoul(*param, nullptr, 0);
    if ((zb_tx_power <= 20) && (zb_tx_power>= -24))
      esp_zb_set_tx_power(zb_tx_power);
    else
      telnet.printf(">Invalid TX power value for SET-ZIGBEE-TX-POWER: %d (should be between -24 and +20)\n\r>", zb_tx_power);
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
      telnet.println(">update-device-timings channel \"keepalive\"/\"timeout\"/\"refresh\"/\"autoset\" time(seconds)");
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
      telnet.println(">add-action action_name src_channel Supla_action dst_channel Supla_event");
      telnet.println(">or");
      telnet.println(">add-action action_name src_channel Supla_action dst_channel Supla_condition threshold1 threshold2");
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
      z2s_add_action(action_name, src_channel_id, Supla_action, dst_channel_id, Supla_event, false);
    else
    if ((params_number == 7) && is_valid_action && is_valid_condition) {
      threshold_1       = strtod(*(param+5), nullptr);
      threshold_2       = strtod(*(param+6), nullptr);
      z2s_add_action(action_name, src_channel_id, Supla_action, dst_channel_id, Supla_condition, true, threshold_1, threshold_2);
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
        log_i("Device(channel %d) sub id changed successfully.", channel_id);
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
          z2s_devices_table[channel_number_slot].user_data_flags |= (1 << bit_id);
        if (flag_clear)
          z2s_devices_table[channel_number_slot].user_data_flags &= ~(1 << bit_id);
      if (Z2S_saveDevicesTable()) {
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
    uint8_t bit_id = parseZBDeviceFlagsStr(*(param + 2));
    
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    uint8_t zb_device_number_slot = (channel_number_slot >= 0) ? 
      Z2S_findZBDeviceTableSlot(z2s_zb_devices_table[channel_number_slot].ieee_addr) : 0xFF;
    
    if (zb_device_number_slot < 0xFF) {
        if (flag_set)
          z2s_zb_devices_table[zb_device_number_slot].user_data_flags |= (1 << bit_id);
        if (flag_clear)
          z2s_zb_devices_table[zb_device_number_slot].user_data_flags &= ~(1 << bit_id);
      if (Z2S_saveZBDevicesTable()) {
        telnet.printf("ZB Device(channel %d) global flags changed successfully.\n\r>", channel_id);
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
          telnet.printf(">param_id(%u) should be in range 1...4\n\r>", param_id); break;
      }
      if (Z2S_saveDevicesTable()) {
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
        bool result = zbGateway.sendAttributeRead(&device, cluster_id, attribute_id, true, direction, disable_default_response, manuf_specific, manuf_code); 
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
          telnet.printf(">Reading attribute failed\n\r>");
      } else {
        zbGateway.sendAttributeRead(&device, cluster_id, attribute_id, false, direction);
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
    bool sync = (params_number > 7) ? (strcmp(*(param + 7),"ASYNC") == 0 ? false : true) : true;
    
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
            telnet.printf(">Device 0x%X manufacturer name: %s\n\r", device.short_addr, zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name); break;
          case ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID:
            telnet.printf(">Device 0x%X model name: %s\n\r", device.short_addr, zbGateway.getQueryBasicClusterData()->zcl_model_name); break;
          case ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID:
            telnet.printf(">Device 0x%X power source: 0x%x\n\r", device.short_addr, zbGateway.getQueryBasicClusterData()->zcl_power_source_id); break;
          case ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID:
            telnet.printf(">Device 0x%X firmware build: %s\n\r", device.short_addr, zbGateway.getQueryBasicClusterData()->software_build_ID); break;
        }
      } else
        telnet.println("read-device-firmware failed :-(");
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
        zbGateway.sendAttributeWrite(&device, cluster_id, attribute_id, attribute_type, attribute_size, value, manuf_specific, manuf_code); 
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
    uint8_t direction = 0;
    uint8_t disable_default_response = 0;
    uint8_t manuf_specific = 0;
    uint8_t manuf_code = 0;
    
    bool sync = (params_number >= 7) ? (strcmp(*(param + 6),"ASYNC") == 0 ? false : true) : false;
    if (params_number >= 8) 
      direction = strtoul(*(param + 7),nullptr, 0);
    if (params_number >= 9)
      disable_default_response = strtoul(*(param + 8),nullptr, 0);
    if (params_number >= 10)
      manuf_specific = strtoul(*(param + 9),nullptr, 0);
    if (params_number >= 11)
      manuf_code = strtoul(*(param + 10),nullptr, 0);
      
    
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
      zbGateway.sendCustomClusterCmd(&device, cluster_id, command_id, data_type, data_size, custom_cmd_payload, sync, direction, 
                                     disable_default_response, manuf_specific, manuf_code); 
      if (!sync) telnet.println("Custom command async request sent");
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
 
  rgbLed.begin(); 
  rgbLed.show(); 

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

  auto toggleNotifications = new Supla::Control::VirtualRelay();
  toggleNotifications->getChannel()->setChannelNumber(110);
  toggleNotifications->setInitialCaption("IAS ZONE NOTIFICATIONS");
  toggleNotifications->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
  toggleNotifications->setDefaultStateRestore();

  auto AHwC = new Supla::ActionHandlerWithCallbacks();
  AHwC->setActionHandlerCallback(supla_callback_bridge);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_1, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_2, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_3, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_4, true);

  toggleNotifications->addAction(0x4000, AHwC, Supla::ON_TURN_ON, false);
  toggleNotifications->addAction(0x4001, AHwC, Supla::ON_TURN_OFF, false);

  auto buttonCfg = new Supla::Control::Button(CFG_BUTTON_PIN, true, true);

  buttonCfg->setHoldTime(2000);
  buttonCfg->setMulticlickTime(500);

  buttonCfg->configureAsConfigButton(&SuplaDevice);

   new Supla::Device::EnterCfgModeAfterPowerCycle(10000, 3, true);

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
  zbGateway.onFlowReceive(Z2S_onFlowReceive);
  zbGateway.onOccupancyReceive(Z2S_onOccupancyReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onRMSVoltageReceive(Z2S_onRMSVoltageReceive);
  zbGateway.onRMSCurrentReceive(Z2S_onRMSCurrentReceive);
  zbGateway.onRMSActivePowerReceive(Z2S_onRMSActivePowerReceive);
  zbGateway.onCurrentSummationReceive(Z2S_onCurrentSummationReceive);
  zbGateway.onCurrentLevelReceive(Z2S_onCurrentLevelReceive);
  zbGateway.onColorHueReceive(Z2S_onColorHueReceive);
  zbGateway.onColorSaturationReceive(Z2S_onColorSaturationReceive);
  zbGateway.onThermostatTemperaturesReceive(Z2S_onThermostatTemperaturesReceive);
  zbGateway.onThermostatModesReceive(Z2S_onThermostatModesReceive);
  zbGateway.onWindowCoveringReceive(Z2S_onWindowCoveringReceive);
  zbGateway.onSonoffCustomClusterReceive(Z2S_onSonoffCustomClusterReceive);
  zbGateway.onBatteryReceive(Z2S_onBatteryReceive);
  zbGateway.onCustomCmdReceive(Z2S_onCustomCmdReceive);
  zbGateway.onCmdCustomClusterReceive(Z2S_onCmdCustomClusterReceive);
  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.onDataSaveRequest(Z2S_onDataSaveRequest);

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  uint32_t zb_primary_channel_mask;

  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_ZIGBEE_PRIMARY_CHANNEL, &zb_primary_channel_mask)) {
    log_i("Zigbee primary channel mask (0x%x) loaded successfully", zb_primary_channel_mask);
    Zigbee.setPrimaryChannelMask(zb_primary_channel_mask);
  }
  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  //Supla
  
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  
  SuplaDevice.setName("Zigbee to Supla Gateway");
  //wifi.enableSSL(true);

  SuplaDevice.setAutomaticResetOnConnectionProblem(300); //5 minutes

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
  
  //
  if (Zigbee.started() && (millis() - _time_cluster_last_refresh_ms > TIME_CLUSTER_REFRESH_MS)) {

    uint32_t new_utc_time = time(NULL) - 946684800;
    uint32_t new_local_time = time(NULL) - 946684800;
        
    log_i("New UTC time %lu, new local time %lu", new_utc_time, new_local_time);
    
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_set_attribute_val(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                 ESP_ZB_ZCL_ATTR_TIME_TIME_ID, &new_utc_time, false);
    esp_zb_zcl_set_attribute_val(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                 ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, &new_local_time, false);
    esp_zb_lock_release();
    
    uint32_t utc_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                                         ESP_ZB_ZCL_ATTR_TIME_TIME_ID)->data_p);
    uint32_t local_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                                           ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID)->data_p);

    log_i("Local Time Cluster UTC time attribute %lu, local time attribute %lu", utc_time_attribute, local_time_attribute);
    
    _time_cluster_last_refresh_ms = millis();
  }

  if (millis() - _status_led_last_refresh_ms > 1000) {

    _status_led_last_mode = _status_led_mode;
    
    if (Zigbee.isNetworkOpen())
      _status_led_mode = 1;
    else
      _status_led_mode = 0;

    if(_status_led_mode != _status_led_last_mode) {
      if (_status_led_mode == 1)
        rgbLed.setPixelColor(0, rgbLed.Color(0, 128, 0));
      else
      if (_status_led_mode == 0)
        rgbLed.setPixelColor(0, rgbLed.Color(0, 0, 0));
      
      rgbLed.show();
    }
  }

  if (millis() - refresh_time > REFRESH_PERIOD) {

    
    for ([[maybe_unused]]const auto &device : zbGateway.getGatewayDevices()) {       

      if (refresh_cycle % 12 == 0) {//print every 120 seconds - only for debug purposes 
        log_i("Device on endpoint(0x%x), short address(0x%x), model id(0x%x), cluster id(0x%x), rejoined(%s)", 
              device->endpoint, device->short_addr, device->model_id, device->cluster_id, device->rejoined ? "YES" : "NO");
        log_i("Gateway version: %s", Z2S_VERSION);
        int8_t zb_tx_power;
        esp_zb_get_tx_power(&zb_tx_power);
        log_i("Zigbee TX power: %d", zb_tx_power);
      }
      if (refresh_cycle % 6 == 0) {
        log_i("getZbgDeviceUnitLastSeenMs %d, current millis %d", zbGateway.getZbgDeviceUnitLastSeenMs(device->short_addr), millis());
      }
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
      
      rgbLed.setPixelColor(0, rgbLed.Color(0, 128, 128));
      rgbLed.show();

      //do some Tuya vodoo - just in case Tuya device is paired
      
      //zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);
      
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

                          case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5: {
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_PRESENCE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR: {
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER: {
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_ENERGY_SID, "ENERGY", 0, "kWh");
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_SID, "PRODUCED ENERGY", 0, "kWh");
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_POWER_SID, "POWER", 0, "W");
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_SID, "POWER FACTOR");   
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_SID);
                          } break;

                          case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR: {
                            Z2S_addZ2SDevice(joined_device, ADEO_SMART_PIRTH_SENSOR_TEMPHUMI_SID, "TH");
                            Z2S_addZ2SDevice(joined_device, ADEO_SMART_PIRTH_SENSOR_ILLUMINANCE_SID, "ILLUMINANCE", 0, "lx");
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID, "ALARM_1", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID, "TAMPER", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);  
                          } break;

                          case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: {
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID, "CONTACT", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_2_SID, "VIBRATION", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID, "TAMPER", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                          } break;

                          case Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F: {
                            Z2S_addZ2SDevice(joined_device, IAS_ACE_ARM_CMD_SID, "ARM", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ACE_PANIC_CMD_SID, "PANIC", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ACE_2_CMD_SID, "2 ?", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ACE_3_CMD_SID, "3 ?", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);

                          } break;

                          case Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F: {
                            Z2S_addZ2SDevice(joined_device, ADEO_CUSTOM_CMD_BUTTON_PRESSED_SID, "PRESSED");
                            Z2S_addZ2SDevice(joined_device, ADEO_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID, "DOUBLE PRESSED");
                            Z2S_addZ2SDevice(joined_device, ADEO_CUSTOM_CMD_BUTTON_HELD_SID, "HELD");
                          } break;

                          case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID,"CONTACT", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID, "TAMPER", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_LOW_BATTERY_SID, "LOW BATTERY", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                          } break;

                          case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID,"CONTACT", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                            Z2S_addZ2SDevice(joined_device, SONOFF_CUSTOM_CLUSTER_TAMPER_SID, "TAMPER", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_LOW_BATTERY_SID, "LOW BATTERY", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                          } break;

                          case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID, "ALARM1", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_2_SID, "ALARM2", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID,  "TAMPER", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                          } break;

                          case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID, "ALARM1", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_LOW_BATTERY_SID,  "LOW BATTERY", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                          } break;

                          case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, LUMI_MOTION_SENSOR_OCCUPANCY_SID,"OCCUPANCY", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, LUMI_MOTION_SENSOR_ILLUMINANCE_SID, "ILLUMINANCE", 0, "lx");
                          } break;

                          case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE: {
                            
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_ON_OFF_SID,"VALVE", SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_FLOW_SID, "FLOW", 0, "m³/h");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_MODE_SID, "MODE", 0, "");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_CYCLE_NUMBER_SID, "CYCLE #", 0, "");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_CYCLES_COUNT_SID, "TOTAL CYCLES", 0, "");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_TIME_SID, "CYCLE TIME", 0, "s");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_PAUSE_SID, "CYCLE PAUSE", 0, "s");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_VOLUME_SID, "CYCLE VOLUME", 0, "l");
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_WD_SILENT_ALARM_SID, "SILENT ALARM", SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, IAS_WD_LOUD_ALARM_SID, "LOUD ALARM", SUPLA_CHANNELFNC_POWERSWITCH);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_CO_SID,"CARBON MONOXIDE", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_CO_CONC_SID, "CO CONCENTRATION", 0, "ppm");
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_SELF_TEST_SID, "SELF TEST RESULT", 0, "1..3");
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_SILENCE_SID, "SILENCE");
                          } break;

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

                case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR:
                case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: 
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2:
{
                  esp_zb_ieee_addr_t gateway_ieee_addr;
                  memset(gateway_ieee_addr, 0, sizeof(esp_zb_ieee_addr_t));
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                                               ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, 8,&gateway_ieee_addr);
                  esp_zb_get_long_address(gateway_ieee_addr);
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                                               ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, 8,&gateway_ieee_addr);

                } break;
                
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL: {
                } break;

                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
                case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
                case Z2S_DEVICE_DESC_RGBW_BULB_XY:
                case Z2S_DEVICE_DESC_DIMMER_BULB:
                case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:
                case Z2S_DEVICE_DESC_IKEA_WW_BULB:
                case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
 {
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID, true))
                    log_i("Color control caps 0x%x, type 0x%x", *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE100, true)) {
                    esp_zb_uint48_t *value = (esp_zb_uint48_t *)zbGateway.getReadAttrLastResult()->data.value;
                    log_i("Color full data 0x%x::0x%x, type 0x%x", value->low, value->high, zbGateway.getReadAttrLastResult()->data.type);
                    zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
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
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
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
                
                /*case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: {
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true))
                      log_i("IAS_ZONE attribute has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, 
                            *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 0x0021, false);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                                ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 900, 1, false);
                } break;
                */
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

                case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1: //Z2S_DEVICE_DESC_TUYA_1GANG_SWITCH:
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
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;

                case Z2S_DEVICE_DESC_RELAY_1: {
                    
                  joined_device->endpoint = 0x01;
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x4003, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); //Tuya special
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;
                 
                case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
                case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1:
                case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR: 
                case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
                case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
                case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5:
                case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR:
                case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR:
                case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3:
                case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH:
                case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL:
                case Z2S_DEVICE_DESC_TS0601_TRV_ME167:
                case Z2S_DEVICE_DESC_TS0601_TRV_BECA:
                case Z2S_DEVICE_DESC_TS0601_TRV_MOES:
                case Z2S_DEVICE_DESC_TS0601_TRV_TRV601:
                case Z2S_DEVICE_DESC_TS0601_TRV_TRV603:
                  zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL); break;

                  
                case Z2S_DEVICE_DESC_SONOFF_TRVZB: {
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 1200, 10, false); 
                
                } break;
              }

              SuplaDevice.scheduleSoftRestart(5000);
            }   
            //else log_i("LIST checking %s::%s, entry # %d",Z2S_DEVICES_LIST[i].manufacturer_name, Z2S_DEVICES_LIST[i].model_name, i);
          }
      if (!device_recognized) {
        log_d(  "Unknown model %s::%s, no binding is possible", zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                     zbGateway.getQueryBasicClusterData()->zcl_model_name);
        rgbLed.setPixelColor(0, rgbLed.Color(255, 0, 0));
        rgbLed.show();
        delay(1000);
      }
    }
  }
}
