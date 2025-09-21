
//#pragma once
#pragma warning(disable: -Wmissing-field-initializers)

#define Z2S_GATEWAY

#include <ZigbeeGateway.h>

#include "esp_coexist.h"

#include "priv_auth_data.h"

#include "z2s_web_gui.h"

#include <SuplaDevice.h>

#include "z2s_devices_database.h"
#include "z2s_devices_table.h"
#include "z2s_device_general_purpose_measurement.h"

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
#include <supla/sensor/general_purpose_measurement.h>
#include <supla/device/status_led.h>

#include "z2s_version_info.h"

//#include <Adafruit_NeoPixel.h>

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2
#define CFG_BUTTON_PIN              9  //Boot button for C6/H2
#define WIFI_ENABLE                 3
#define WIFI_ANT_CONFIG             14

#define REFRESH_PERIOD              10 * 1000 //miliseconds

#define TIME_CLUSTER_REFRESH_MS     60 * 1000 //miliseconds

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi;
Supla::LittleFsConfig     configSupla (2048);

constexpr uint8_t LED_PIN   = 8;
constexpr uint8_t NUM_LEDS  = 1;

//Adafruit_NeoPixel rgbLed(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t startTime          = 0;
uint32_t printTime          = 0;
uint32_t zbInit_delay       = 0;

uint32_t refresh_time       = 0;
uint8_t refresh_cycle       = 0;

uint32_t _time_cluster_last_refresh_ms = 0;

uint32_t _status_led_last_refresh_ms = 0;
uint8_t _status_led_mode             = 0;
uint8_t _status_led_last_mode        = 0;
uint8_t _status_led_cycle            = 0;

bool zbInit       = true;
bool GUIstarted   = false;
bool GUIdisabled  = true; //false;

uint8_t  _enable_gui_on_start  = 1;
uint8_t	_force_config_on_start = 0;
uint32_t _gui_start_delay      = 0;

uint8_t _z2s_security_level    = 0;

uint8_t write_mask;
uint16_t write_mask_16;
uint32_t write_mask_32;

bool sendIASNotifications = false;

void Z2S_nwk_scan_neighbourhood(bool toTelnet = false) {

  esp_zb_nwk_neighbor_info_t nwk_neighbour;
  esp_zb_nwk_info_iterator_t nwk_iterator = 0;
  esp_err_t scan_result;
  //esp_zb_lock_acquire(portMAX_DELAY);
  scan_result = esp_zb_nwk_get_next_neighbor(&nwk_iterator, &nwk_neighbour);
  //esp_zb_lock_release();
  char log_line[384];
  
  if (scan_result == ESP_ERR_NOT_FOUND)
    log_i_telnet2("\033[1mZ2S_nwk_scan_neighbourhood scan empty :-(  \033[22m");

  while (scan_result == ESP_OK) {
    sprintf_P(log_line, PSTR("Scan neighbour record number - 0x%x:\n\rIEEE ADDRESS\t\t%X:%X:%X:%X:%X:%X:%X:%X\n\rSHORT ADDRESS\t\t0x%x\n"
                      "\rDEPTH\t\t\t0x%x\n\rRX_ON_WHEN_IDLE\t\t0x%x\n\rRELATIONSHIP\t\t0x%x\n\rLQI\t\t\t%d\n\rRSSI\t\t\t%d\n\rOUTGOING COST\t\t0x%x\n"
                      "\rAGE\t\t\t0x%x\n\rDEVICE TIMEOUT\t\t%lu\n\rTIMEOUT COUNTER\t\t%lu"), 
        nwk_iterator, 
        nwk_neighbour.ieee_addr[7], nwk_neighbour.ieee_addr[6], nwk_neighbour.ieee_addr[5], nwk_neighbour.ieee_addr[4], 
        nwk_neighbour.ieee_addr[3], nwk_neighbour.ieee_addr[2], nwk_neighbour.ieee_addr[1], nwk_neighbour.ieee_addr[0],
        nwk_neighbour.short_addr, nwk_neighbour.depth, nwk_neighbour.rx_on_when_idle, nwk_neighbour.relationship,
        nwk_neighbour.lqi, nwk_neighbour.rssi, nwk_neighbour.outgoing_cost, nwk_neighbour.age, nwk_neighbour.device_timeout,
        nwk_neighbour.timeout_counter);
    log_i_telnet2(log_line, toTelnet);
    
    int16_t channel_number_slot = Z2S_findChannelNumberSlot(nwk_neighbour.ieee_addr, -1, 0, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
    
    if (channel_number_slot < 0) {
      sprintf(log_line, PSTR("Z2S_nwk_scan_neighbourhood - no channel found for address 0x%x"), nwk_neighbour.short_addr);
      log_i_telnet2(log_line, toTelnet);
    }
    else {
      while (channel_number_slot >= 0) {
      auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);
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
    log_i_telnet2("Z2S_nwk_scan_neighbourhood error ESP_ERR_INVALID_ARG", toTelnet);

  if (scan_result == ESP_ERR_NOT_FOUND)
    log_i_telnet2("Z2S_nwk_scan_neighbourhood scan completed", toTelnet);
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

    case 0x4010: {

      if ((!Zigbee.started()) && (SuplaDevice.getCurrentStatus() == STATUS_REGISTERED_AND_READY)) {
  
        log_i("Starting Zigbee subsystem");
    
        esp_coex_wifi_i154_enable();
  
        if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
          
          log_e("Zigbee failed to start! Rebooting...");
          SuplaDevice.scheduleSoftRestart(1000);
        }
      
        refresh_time = 0;
        
        #ifdef USE_TELNET_CONSOLE

        setupTelnet();
        onTelnetCmd(Z2S_onTelnetCmd); 

        #endif //USE_TELNET_CONSOLE
        return;
      }
      if (Zigbee.started() && (SuplaDevice.getCurrentStatus() == STATUS_CONFIG_MODE)) {
        
        if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_FORCE_CONFIG_ON_START, 1)) {
      	  
          log_i("Supla config mode detected (Zigbee stack active) - setting Z2S_FORCE_CONFIG_ON_START flag and restarting!");
          Supla::Storage::ConfigInstance()->commit();
          SuplaDevice.scheduleSoftRestart();
        }
        else
          log_e("Supla config mode detected (Zigbee stack active) - setting Z2S_FORCE_CONFIG_ON_START flag FAILED!");
        return;  
      }
    } break;
  }

  switch (event) {
    case Supla::ON_EVENT_1:
    case Supla::ON_CLICK_1: Zigbee.openNetwork(180); break;
    case Supla::ON_EVENT_2:
    case Supla::ON_CLICK_5: { //Zigbee.factoryReset(); break;
      if (!GUIstarted) {
        if (Supla::Network::IsReady()) {
          GUIstarted = true;
          Z2S_buildWebGUI();
          Z2S_startWebGUI();
          Z2S_startUpdateServer();
          onTuyaCustomClusterReceive(GUI_onTuyaCustomClusterReceive);
        }
      } else {
        //GUIdisabled = true;
        //Z2S_stopWebGUI();
      }
    } break; 
  }
}

#ifdef USE_TELNET_CONSOLE

bool getDeviceByChannelNumber(zbg_device_params_t *device, uint8_t channel_id) {

  int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
  
  if (channel_number_slot >= 0) {

    device->endpoint = z2s_channels_table[channel_number_slot].endpoint;
    device->cluster_id = z2s_channels_table[channel_number_slot].cluster_id;
    memcpy(device->ieee_addr, z2s_channels_table[channel_number_slot].ieee_addr,8);
    device->short_addr = z2s_channels_table[channel_number_slot].short_addr;
    device->model_id = z2s_channels_table[channel_number_slot].model_id;
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
  if (strcmp(Supla_action, PSTR("INCREASE_TEMPERATURE")) == 0)
    return Supla::INCREASE_TEMPERATURE;
  else
  if (strcmp(Supla_action, PSTR("DECREASE_TEMPERATURE")) == 0)
    return Supla::DECREASE_TEMPERATURE;
  else
  if (strcmp(Supla_action, PSTR("TOGGLE_OFF_MANUAL_WEEKLY_SCHEDULE_MODES")) == 0)
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
  
  uint8_t custom_cmd_payload[64]; //TODO - include RAW/STRING 0.8.51 increased 10->64
  uint8_t write_attribute_payload[20];


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
        telnet.printf(PSTR(">Zigbee primary channel: %u\n\r>"), i);
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
        telnet.printf(PSTR(">New Zigbee primary channel(%u) mask(%x) write success! Restarting...\n\r"), zb_primary_channel, (1 << zb_primary_channel));
        Supla::Storage::ConfigInstance()->commit();
        SuplaDevice.scheduleSoftRestart(1000);
        return;
      } else {
        telnet.printf(PSTR(">New Zigbee primary channel(%u) mask(%x) write failed!\n\r"), zb_primary_channel, (1 << zb_primary_channel));
        return;
      }
    } else { 
      telnet.printf(PSTR(">Invalid value for Zigbee primary channel: %u (should be between 11 and 26)\n\r"));
      return;
    }
  } else
  if (strcmp(cmd, "GET-ZIGBEE-TX-POWER") == 0) {

    int8_t zb_tx_power;
    esp_zb_get_tx_power(&zb_tx_power);
    telnet.printf(PSTR(">Zigbee TX power: %d\n\r>"), zb_tx_power);
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
    Z2S_printChannelsTableSlots(true);
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
      z2s_channels_table[channel_number_slot].valid_record = false;
      if (Z2S_saveChannelsTable()) {
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
      Z2S_add_action(action_name, src_channel_id, Supla_action, dst_channel_id, Supla_event, false);
    else
    if ((params_number == 7) && is_valid_action && is_valid_condition) {
      threshold_1       = strtod(*(param+5), nullptr);
      threshold_2       = strtod(*(param+6), nullptr);
      Z2S_add_action(action_name, src_channel_id, Supla_action, dst_channel_id, Supla_condition, true, threshold_1, threshold_2);
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
    uint8_t bit_id = parseZBDeviceFlagsStr(*(param + 2));
    
    int16_t channel_number_slot = Z2S_findTableSlotByChannelNumber(channel_id);
    
    uint8_t zb_device_number_slot = (channel_number_slot >= 0) ? 
      Z2S_findZBDeviceTableSlot(z2s_channels_table[channel_number_slot].ieee_addr) : 0xFF;
    
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
        
        if (Z2S_saveZBDevicesTable()) {
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
      
    if (data_size > sizeof(custom_cmd_payload)) {
      log_e("Custom command data size(%u) to big(max. %u)!",
            data_size, sizeof(custom_cmd_payload));
      return;
    }

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

#endif  //USE_TELNET_CONSOLE

/*zbg_device_params_t test_device = {.model_id = Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A,
.rejoined = true, .ZC_binding = true, .ieee_addr = {0,0,0,0,0,0,0,0}, .endpoint = 1, .cluster_id = 0, 
  .short_addr = 0, .user_data = 0};

static char log_print_buffer[2048];

int spiffs_log_vprintf(const char *fmt, va_list args) {

  int ret = vsnprintf(log_print_buffer, sizeof(log_print_buffer), fmt, args);
  if (ret >= 0) {
    Serial.println("My log function");
    Serial.println(log_print_buffer);
  }
  return ret;
}*/

void enableZ2SNotifications() {

  //  Zigbee Gateway notifications

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onPressureReceive(Z2S_onPressureReceive);
  zbGateway.onIlluminanceReceive(Z2S_onIlluminanceReceive);
  zbGateway.onFlowReceive(Z2S_onFlowReceive);
  zbGateway.onOccupancyReceive(Z2S_onOccupancyReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onElectricalMeasurementReceive(Z2S_onElectricalMeasurementReceive);
  zbGateway.onMultistateInputReceive(Z2S_onMultistateInputReceive);
  zbGateway.onAnalogInputReceive(Z2S_onAnalogInputReceive);
  zbGateway.onMeteringReceive(Z2S_onMeteringReceive);
  zbGateway.onCurrentLevelReceive(Z2S_onCurrentLevelReceive);
  zbGateway.onColorHueReceive(Z2S_onColorHueReceive);
  zbGateway.onColorSaturationReceive(Z2S_onColorSaturationReceive);
  zbGateway.onThermostatTemperaturesReceive(Z2S_onThermostatTemperaturesReceive);
  zbGateway.onThermostatModesReceive(Z2S_onThermostatModesReceive);
  zbGateway.onWindowCoveringReceive(Z2S_onWindowCoveringReceive);
  zbGateway.onSonoffCustomClusterReceive(Z2S_onSonoffCustomClusterReceive);
  zbGateway.onDevelcoCustomClusterReceive(Z2S_onDevelcoCustomClusterReceive);
  zbGateway.onLumiCustomClusterReceive(Z2S_onLumiCustomClusterReceive);
  zbGateway.onBatteryReceive(Z2S_onBatteryReceive);
  zbGateway.onCustomCmdReceive(Z2S_onCustomCmdReceive);
  zbGateway.onCmdCustomClusterReceive(Z2S_onCmdCustomClusterReceive);
  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.onDataSaveRequest(Z2S_onDataSaveRequest);
  zbGateway.onDeviceRejoin(Z2S_onDeviceRejoin);
}

void disableZ2SNotifications() {

//  switch off Zigbee Gateway notifications

  zbGateway.onTemperatureReceive(nullptr);
  zbGateway.onHumidityReceive(nullptr);
  zbGateway.onPressureReceive(nullptr);
  zbGateway.onIlluminanceReceive(nullptr);
  zbGateway.onFlowReceive(nullptr);
  zbGateway.onOccupancyReceive(nullptr);
  zbGateway.onOnOffReceive(nullptr);
  zbGateway.onElectricalMeasurementReceive(nullptr);
  zbGateway.onMultistateInputReceive(nullptr);
  zbGateway.onAnalogInputReceive(nullptr);
  zbGateway.onMeteringReceive(nullptr);
  zbGateway.onCurrentLevelReceive(nullptr);
  zbGateway.onColorHueReceive(nullptr);
  zbGateway.onColorSaturationReceive(nullptr);
  zbGateway.onThermostatTemperaturesReceive(nullptr);
  zbGateway.onThermostatModesReceive(nullptr);
  zbGateway.onWindowCoveringReceive(nullptr);
  zbGateway.onSonoffCustomClusterReceive(nullptr);
  zbGateway.onDevelcoCustomClusterReceive(nullptr);
  zbGateway.onLumiCustomClusterReceive(nullptr);
  zbGateway.onBatteryReceive(nullptr);
  zbGateway.onCustomCmdReceive(nullptr);
  zbGateway.onCmdCustomClusterReceive(nullptr);
  zbGateway.onIASzoneStatusChangeNotification(nullptr);
  zbGateway.onBoundDevice(nullptr);
  zbGateway.onBTCBoundDevice(nullptr);
  zbGateway.onDataSaveRequest(nullptr);
  zbGateway.onDeviceRejoin(nullptr);
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.print(file.name());
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      Serial.printf(
        "  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour,
        tmstruct->tm_min, tmstruct->tm_sec
      );
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      Serial.printf(
        "  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour,
        tmstruct->tm_min, tmstruct->tm_sec
      );
    }
    file = root.openNextFile();
  }
}

Supla::Device::StatusLed statusLed(RGB_BUILTIN, true);

void setup() {
  
  //esp_log_set_vprintf(&spiffs_log_vprintf);

  log_i("setup start");

  //log_d("Total PSRAM: %d", ESP.getPsramSize());
  //log_d("Free PSRAM: %d", ESP.getFreePsram());

  pinMode(BUTTON_PIN, INPUT);

  pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT); (credits @Zibi_007)in
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

  auto toggleNotifications = new Supla::Control::VirtualRelay();
  toggleNotifications->getChannel()->setChannelNumber(110);
  toggleNotifications->setInitialCaption("Gateway sensors notifications");
  toggleNotifications->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
  toggleNotifications->setDefaultStateRestore();

  auto AHwC = new Supla::ActionHandlerWithCallbacks();
  AHwC->setActionHandlerCallback(supla_callback_bridge);

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

  SuplaDevice.addAction(0x4010, AHwC, Supla::ON_DEVICE_STATUS_CHANGE, false);

  LittleFS.begin(false);
  listDir(LittleFS,"/",3);

  if (LittleFS.exists("/supla/Z2S_devs_table")) {

    log_i("/supla/Z2S_devs_table found - moving to /z2s_gateway/channels_table_v2.z2s");
    LittleFS.mkdir("/z2s_gateway");
    LittleFS.rename("/supla/Z2S_devs_table", "/z2s_gateway/channels_table_v2.z2s");
  }

  if (LittleFS.exists("/supla/Z2S_zbd_table")) {

    log_i("/supla/Z2S_zbd_table found - moving to /z2s_gateway/zb_devices_table_v2.z2s");
    LittleFS.mkdir("/z2s_gateway");
    LittleFS.rename("/supla/Z2S_zbd_table", "/z2s_gateway/zb_devices_table_v2.z2s");
  }

  listDir(LittleFS,"/",3);
  LittleFS.end();

  Z2S_loadZBDevicesTable();

  Z2S_loadChannelsTable();

  Z2S_initZBDevices(millis());

  Z2S_initSuplaChannels();

  /*z2s_channel_action_t test_action;

  
  test_action.is_enabled = true;
  test_action.src_Supla_channel = 1;
  test_action.dst_Supla_channel = 2;
  test_action.src_Supla_event = Supla::ON_CLICK_1;
  test_action.dst_Supla_action = Supla::TURN_ON;
  strcpy(test_action.action_name, "Test Action 1");
  strcpy(test_action.action_description, "Short description");
  test_action.is_condition = false;
  test_action.min_value = 12.8;
  test_action.max_value = 1214.56;

  Z2S_saveAction( Z2S_findFreeActionIndex(), test_action);
  strcpy(test_action.action_name, "Test Action 2");
  Z2S_saveAction( Z2S_findFreeActionIndex(), test_action);
  strcpy(test_action.action_name, "Test Action 3");
  Z2S_saveAction( Z2S_findFreeActionIndex(), test_action);
  strcpy(test_action.action_name, "Test Action 4");
  Z2S_saveAction( Z2S_findFreeActionIndex(), test_action);
  strcpy(test_action.action_name, "Test Action 5");
  Z2S_saveAction( Z2S_findFreeActionIndex(), test_action);*/

  Z2S_initSuplaActions();

  enableZ2SNotifications();

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

  if (Supla::Storage::ConfigInstance()->getUInt8(Z2S_ENABLE_GUI_ON_START, &_enable_gui_on_start)) {
    log_i("Z2S_ENABLE_GUI_ON_START = %d", _enable_gui_on_start);
  } else {
    log_i("Z2S_ENABLE_GUI_ON_START not configured - turning on");
    _enable_gui_on_start = 1;
  }

  if (Supla::Storage::ConfigInstance()->getUInt8(Z2S_FORCE_CONFIG_ON_START, &_force_config_on_start)) {
    log_i("Z2S_FORCE_CONFIG_ON_START = %d", _force_config_on_start);
  } else {
    log_i("Z2S_FORCE_CONFIG_ON_START not configured - turning off");
    _force_config_on_start = 0;
  }

  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_GUI_ON_START_DELAY, &_gui_start_delay)) {
    log_i("Z2S_GUI_ON_START_DELAY = %d s", _gui_start_delay);
  } else {
    log_i("Z2S_GUI_ON_START_DELAY not configured - setting to 0 s");
    _gui_start_delay = 0;
  }

  Supla::Storage::ConfigInstance()->getUInt8(PSTR("security_level"), &_z2s_security_level);

  //Supla
  
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  
  SuplaDevice.setName("Zigbee <=> Supla Gateway");
  SuplaDevice.setSwVersion(Z2S_VERSION);
  wifi.enableSSL(ENABLE_SSL);

  SuplaDevice.setAutomaticResetOnConnectionProblem(300); //5 minutes
  SuplaDevice.allowWorkInOfflineMode(2);
  SuplaDevice.begin();      
  
  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
  refresh_time = millis();
}

zbg_device_params_t *gateway_device;
zbg_device_params_t *joined_device;

uint8_t counter = 0;
uint8_t tuya_dp_data[10];


void loop() {

  if (_force_config_on_start == 1) {
    
    _force_config_on_start = 0;
    
    if (Supla::Storage::ConfigInstance()->setUInt8(Z2S_FORCE_CONFIG_ON_START, _force_config_on_start))
      Supla::Storage::ConfigInstance()->commit();

    SuplaDevice.enterConfigMode();
  }

  if ((!GUIstarted) && SuplaDevice.getCurrentStatus() == STATUS_CONFIG_MODE) {
    GUIstarted = true;
    Z2S_startWebGUIConfig();
    Z2S_startUpdateServer();
  } 

  if ((!GUIstarted) && (_enable_gui_on_start == 1) && Zigbee.started() && (SuplaDevice.uptime.getUptime() > _gui_start_delay)) {// SuplaDevice.getCurrentStatus() == STATUS_REGISTERED_AND_READY) {
    GUIstarted = true;
    Z2S_buildWebGUI();  
    Z2S_startWebGUI();
    Z2S_startUpdateServer();
    onTuyaCustomClusterReceive(GUI_onTuyaCustomClusterReceive);
  }
 
  SuplaDevice.iterate();

#ifdef USE_TELNET_CONSOLE

  if (is_Telnet_server) telnet.loop();

#endif //USE_TELNET_CONSOLE
  
  //
  if (Zigbee.started() && (millis() - _time_cluster_last_refresh_ms > TIME_CLUSTER_REFRESH_MS)) {

    uint32_t new_utc_time = time(NULL) - 946684800;
    uint32_t new_local_time = time(NULL) - 946684800;
    uint8_t time_status = 0x02;
        
    log_i("New UTC time %lu, new local time %lu", new_utc_time, new_local_time);
    
    esp_zb_lock_acquire(portMAX_DELAY);
    esp_zb_zcl_set_attribute_val(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                 ESP_ZB_ZCL_ATTR_TIME_TIME_ID, &new_utc_time, false);
    esp_zb_zcl_set_attribute_val(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                 ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, &time_status, false);
    esp_zb_zcl_set_attribute_val(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                 ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, &new_local_time, false);
    esp_zb_lock_release();
    
    uint32_t utc_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                                         ESP_ZB_ZCL_ATTR_TIME_TIME_ID)->data_p);
    uint32_t local_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                                           ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID)->data_p);
    uint8_t time_status_attribute = (*(uint8_t *)esp_zb_zcl_get_attribute(1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                                           ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID)->data_p);

    log_i("Local Time Cluster Time status attribute %u, UTC time attribute %lu, local time attribute %lu", time_status_attribute, utc_time_attribute, local_time_attribute);
    

    if (GUIstarted)
      Z2S_updateWebGUI();

    _time_cluster_last_refresh_ms = millis();
  }

  if (millis() - _status_led_last_refresh_ms > 1000) {

    /*log_i( "Memory information: Flash chip real size:%u B, Free Sketch Space:%u B, "
						"Free Heap:%u, Minimal Free Heap:%u B, "
						"HeapSize:%u B, MaxAllocHeap:%u B, "
						"Supla uptime:%lu s", 
						ESP.getFlashChipSize(), ESP.getFreeSketchSpace(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(),
						ESP.getMaxAllocHeap(), SuplaDevice.uptime.getUptime());
    */
    _status_led_last_mode = _status_led_mode;
    _status_led_last_refresh_ms = millis();
    
    if (Zigbee.isNetworkOpen())
      _status_led_mode = 1;
    else
      _status_led_mode = 0;

    if(_status_led_mode != _status_led_last_mode) {
      if (_status_led_mode == 1)
        //rgbLed.setPixelColor(0, rgbLed.Color(255, 215, 0));
        rgbLedWrite(RGB_BUILTIN, 0, 255, 0);  // Green
      else
      if (_status_led_mode == 0)
        //rgbLed.setPixelColor(0, rgbLed.Color(0, 0, 0));
        rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // Green
      
      //rgbLed.show();
    }

    if (GUIstarted)
      Z2S_loopWebGUI();

  }

  if (millis() - refresh_time > REFRESH_PERIOD) {

    
    for ([[maybe_unused]]const auto &device : zbGateway.getGatewayDevices()) {       

      if (refresh_cycle % 12 == 0) {//print every 120 seconds - only for debug purposes 
        /*log_i("Device on endpoint(0x%x), short address(0x%x), model id(0x%x), cluster id(0x%x), rejoined(%s)", 
              device->endpoint, device->short_addr, device->model_id, device->cluster_id, device->rejoined ? "YES" : "NO");
        log_i("Gateway version: %s", Z2S_VERSION);*/
        //int8_t zb_tx_power;
        //esp_zb_get_tx_power(&zb_tx_power);
        //log_i("Zigbee TX power: %d", zb_tx_power);
        //Z2S_updateWebGUI();
      /*  if (Test_GeneralPurposeMeasurement) {
          char display_buffer[15] = {};
          char test_gpm_buf[15];
          sprintf(test_gpm_buf, "%llu", time(NULL));
          //Test_GeneralPurposeMeasurement->setNoSpaceBeforeValue(0, true);
          //Test_GeneralPurposeMeasurement->setNoSpaceAfterValue(0, true);
          //Test_GeneralPurposeMeasurement->setValue(0.2800);
          //Test_GeneralPurposeMeasurement->setUnitBeforeValue("0123456789ABCD", true);
          //sprintf(test_gpm_buf, "%llu", millis());
          //Test_GeneralPurposeMeasurement->setUnitAfterValue("EF0123456789AB", true);*/
          /*display_buffer[0] = random(0,2) + '0';
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 1, 1, display_buffer);
          sprintf(display_buffer, "%02u", 1 + random(0,18));
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 3, 2, display_buffer);
          sprintf(display_buffer, "%u", random(0,3));
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 6, 1, display_buffer);
          sprintf(display_buffer, "%04u", random(0,1801));
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 8, 4, display_buffer);*/
          //sprintf(display_buffer, "A%1uM%02uV%1uD%04u", random(0,2), 1 + random(0,18), random(0,3), random(0,1801));
          //Test_GeneralPurposeMeasurement->setValue(random(0,10));
          //Test_GeneralPurposeMeasurement->setUnitBeforeValue(display_buffer, true);

        //}
      }
      if (refresh_cycle % 12 == 0) {
        /*log_i("getZbgDeviceUnitLastSeenMs %d, current millis %d", zbGateway.getZbgDeviceUnitLastSeenMs(device->short_addr), millis());
        log_i( "Memory information: Flash chip real size:%u B, Free Sketch Space:%u B, "
						"Free Heap:%u, Minimal Free Heap:%u B, "
						"HeapSize:%u B, MaxAllocHeap:%u B, "
						"Supla uptime:%lu s", 
						ESP.getFlashChipSize(), ESP.getFreeSketchSpace(), ESP.getFreeHeap(), ESP.getMinFreeHeap(), ESP.getHeapSize(),
						ESP.getMaxAllocHeap(), SuplaDevice.uptime.getUptime());*/
      }
    }
    if (!zbGateway.getGatewayDevices().empty()) {
      refresh_time = millis();
      refresh_cycle = (refresh_cycle + 1) % 12;
    }
  }

  if (zbGateway.isNewDeviceJoined()) {

    disableZ2SNotifications();
    //zbGateway.setActivePairing(true); //TODO - rethink that idea
    
    zbGateway.clearNewDeviceJoined();
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty())
    {
      joined_device = zbGateway.getLastJoinedDevice();
      
      //rgbLed.setPixelColor(0, rgbLed.Color(0, 128, 128));
      //rgbLed.show();
      rgbLedWrite(RGB_BUILTIN, 0, 128, 128);  // Green

      Z2S_stopWebGUI();


      //do some Tuya vodoo - just in case Tuya device is paired
      
      //zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);
      
      bool _basic_cluster_query_success = false;

      if ((strlen(zbGateway.getQueryBasicClusterData()->zcl_model_name) == 0) ||
          (strlen(zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name) == 0)) {
      
        for (uint8_t query_idx = 0; query_idx < 5; query_idx++) {

          _basic_cluster_query_success =  zbGateway.zbQueryDeviceBasicCluster(joined_device);
          if (_basic_cluster_query_success)
            break; 
        }
      }
      else _basic_cluster_query_success = true;

      if (!_basic_cluster_query_success) {
        
          log_i("Error while pairing - cann't read manufacturer id (5x). Gateway will restart, try to pair device once again!");
          SuplaDevice.scheduleSoftRestart(0);
        } 
      //write_mask = 0x13;
      //zbGateway.sendAttributeWrite(joined_device, 0x0000, 0xffde, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask); //Tuya black magic continues
      //write_mask = 0x1;
      //zbGateway.sendAttributeWrite(joined_device, 0xfcc0, 0x0009, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, 1, 0x115F); //Lumi magic

      uint16_t devices_list_table_size = sizeof(Z2S_DEVICES_LIST)/sizeof(Z2S_DEVICES_LIST[0]);
      uint16_t devices_desc_table_size = sizeof(Z2S_DEVICES_DESC)/sizeof(Z2S_DEVICES_DESC[0]);
      bool device_recognized = false;

          for (int devices_list_counter = 0; devices_list_counter < devices_list_table_size; devices_list_counter++) { 
            
            if ((strcmp(zbGateway.getQueryBasicClusterData()->zcl_model_name, Z2S_DEVICES_LIST[devices_list_counter].model_name) == 0) &&
            (strcmp(zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name, Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name) == 0)) {
              log_i("LIST matched %s::%s, entry # %d, endpoints # %d, endpoints 0x%x::0x%x,0x%x::0x%x,0x%x::0x%x,0x%x::0x%x",
                    Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name, Z2S_DEVICES_LIST[devices_list_counter].model_name, devices_list_counter, 
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count,
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[0].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[0].z2s_device_desc_id,
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[1].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[1].z2s_device_desc_id,
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[2].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[2].z2s_device_desc_id,
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[3].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[3].z2s_device_desc_id);
  
              for (int endpoint_counter = 0; endpoint_counter < Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count; endpoint_counter++) {
              
                uint8_t endpoint_id = ( Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count == 1) ? 
                                        1 : Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[endpoint_counter].endpoint_id; 
                                        
                uint32_t z2s_device_desc_id = ( Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count == 1) ?
                                                Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id :
                                                Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[endpoint_counter].z2s_device_desc_id; 

                for (int devices_desc_counter = 0; devices_desc_counter < devices_desc_table_size; devices_desc_counter++) {

                  if ( z2s_device_desc_id == Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_desc_id) {
                  log_i("DESC matched 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, endpoint 0x%x ",
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_desc_id,   
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters_count,
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[0],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[1],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[2],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[3],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[4],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[5],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[6],
                        Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[7],
                        endpoint_id);

                        device_recognized = true;

                        joined_device->endpoint = endpoint_id;
                        joined_device->model_id = Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_desc_id;
                        
                        if (endpoint_counter == 0) { 

                          if (Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_config_flags & 
                              Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT) {

                              log_i("Tuya magic");

                              uint16_t tuya_init_attributes[6] = { ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
                                                                   ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 
                                                                   ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, 
                                                                   ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
                                                                   ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 
                                                                   0xFFFE };
                              zbGateway.sendAttributesRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 6, tuya_init_attributes);  
                            }

                          if (Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_config_flags & 
                              Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_SETUP) {

                              log_i("Tuya setup");

                              zbGateway.sendCustomClusterCmd(joined_device, 
                                                             ESP_ZB_ZCL_CLUSTER_ID_BASIC, 
                                                             TUYA_SETUP_CMD, 
                                                             ESP_ZB_ZCL_ATTR_TYPE_SET, 
                                                             0, nullptr, false, 
                                                             ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 0, 0); //disable default response, no manufacurer code  
                            }
                        }

                        //before bind

                        switch (z2s_device_desc_id) {

                          case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2: {

                            write_mask_16 = 0x000B;
                            //joined_device->endpoint = 2;
                            /*if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0031, true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                                1, 1, PHILIPS_MANUFACTURER_CODE)) {

                              uint16_t philips_0031 = *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value;
                              log_i("Philips basic cluster attribute 0x0031 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, philips_0031);
                              if (philips_0031 != write_mask_16)
                              write_mask_16 = 0x0000;
                            }*/
                            zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0031, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 
                                                         2, &write_mask_16, true, 1, PHILIPS_MANUFACTURER_CODE);
                          } break;
                        }

                        for (int clusters_bind_counter = 0; 
                              clusters_bind_counter < Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters_count; 
                              clusters_bind_counter++)
                          zbGateway.bindDeviceCluster(joined_device, 
                                                      Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[clusters_bind_counter]);

                        if (endpoint_counter == 0) {//(endpoint_id == 1)
                          
                          uint8_t zb_device_slot = Z2S_addZBDeviceTableSlot(joined_device->ieee_addr,
                                                                            joined_device->short_addr,
                                                                            zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                                                            zbGateway.getQueryBasicClusterData()->zcl_model_name,
                                                                            Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count,
                                                                            Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id,
                                                                            zbGateway.getQueryBasicClusterData()->zcl_power_source_id);
                          if ((zb_device_slot < 0xFF) &&
                              (Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_config_flags & 
                               Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY))
                            Z2S_setZBDeviceFlags(zb_device_slot, ZBD_USER_DATA_FLAG_TUYA_QUERY_AFTER_REJOIN);
                          
                        }

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

                          case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2: {
                            
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_ON_PRESS_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_ON_PRESS_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_ON_HOLD_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_ON_HOLD_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_UP_PRESS_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_UP_PRESS_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_UP_HOLD_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_UP_HOLD_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_DOWN_PRESS_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_DOWN_PRESS_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_DOWN_HOLD_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_DOWN_HOLD_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_OFF_PRESS_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_OFF_PRESS_RELEASE_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_OFF_HOLD_SID);
                            Z2S_addZ2SDevice(joined_device, PHILIPS_HUE_DIMMER_SWITCH_OFF_HOLD_RELEASE_SID);
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

                          case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_PRESENCE_SID, "PRESENCE", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_TEMPHUMIDITY_SID, "T&H");
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID, "ILLUMINANCE",
                                             SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx");
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_RAIN_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_RAIN_SENSOR_ILLUMINANCE_SID);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_ENERGY_SID, 
                                             "ENERGY", 0, "kWh");
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_PRODUCED_ENERGY_SID,
                                             "PRODUCED ENERGY", 0, "kWh");
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_POWER_SID, 
                                             "POWER", 0, "W");
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_POWER_FACTOR_SID, 
                                             "POWER FACTOR");   
                            Z2S_addZ2SDevice(joined_device, TUYA_3PHASES_ELECTRICITY_METER_SID);
                          } break;

                          case Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER: {
                            
                            Z2S_addZ2SDevice(joined_device, SONOFF_ON_OFF_SID, "SWITCH", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, SONOFF_ELECTRICITY_METER_SID, 
                                             "EM");
                            Z2S_addZ2SDevice(joined_device, SONOFF_ELECTRICITY_METER_ENERGY_TODAY_SID, 
                                             "ENERGY (TODAY)", 0, "kWh");
                            Z2S_addZ2SDevice(joined_device, SONOFF_ELECTRICITY_METER_ENERGY_MONTH_SID, 
                                             "ENERGY (MONTH)", 0, "kWh");   
                            Z2S_addZ2SDevice(joined_device, SONOFF_ELECTRICITY_METER_ENERGY_YESTERDAY_SID, 
                                             "ENERGY (YESTERDAY)", 0, "kWh");
                          } break;

                          case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, ADEO_SMART_PIRTH_SENSOR_TEMPHUMI_SID, 
                                             "TH");
                            Z2S_addZ2SDevice(joined_device, ADEO_SMART_PIRTH_SENSOR_ILLUMINANCE_SID, 
                                             "ILLUMINANCE", 0, "lx");
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID, 
                                             "ALARM_1", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID, 
                                             "TAMPER", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);  
                          } break;

                          case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_1_SID, 
                                             "CONTACT", 
                                             SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_ALARM_2_SID, 
                                             "VIBRATION", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, IAS_ZONE_TAMPER_SID, 
                                             "TAMPER", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
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

                          case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_VIBRATION_SENSOR_VIBRATION_SID, "VIBRATION", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, TUYA_VIBRATION_SENSOR_CONTACT_SID,  "CONTACT", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR_PIR_SID, 
                                             "PIR", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR_IL_SID,  
                                             "ILLUMINANCE", SUPLA_CHANNELFNC_OPENINGSENSOR_DOOR);
                          } break;

                          case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, LUMI_MOTION_SENSOR_OCCUPANCY_SID,"OCCUPANCY", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, LUMI_MOTION_SENSOR_ILLUMINANCE_SID, "ILLUMINANCE", 0, "lx");
                          } break;

                          case Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, SONOFF_PIR_SENSOR_OCCUPANCY_SID,"OCCUPANCY", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, SONOFF_PIR_SENSOR_ILLUMINANCE_SID, "ILLUMINANCE", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                          } break;

                          case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE: {
                            
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_ON_OFF_SID,"VALVE ON/OFF (MANUAL)", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_RUN_PROGRAM_SID,"RUN SAVED PROGRAM", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                                             
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_FLOW_SID, "FLOW", 0, "m³/h");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_MODE_SID, "MODE", 0, "");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_CYCLE_NUMBER_SID, "CYCLE #", 0, "");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_CYCLES_COUNT_SID, "TOTAL CYCLES", 0, "");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_TIME_SID, "CYCLE TIME", 0, "s");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_PAUSE_SID, "CYCLE PAUSE", 0, "s");
                            Z2S_addZ2SDevice(joined_device, SONOFF_SMART_VALVE_VOLUME_SID, "CYCLE VOLUME", 0, "l");
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM: {
                            
                            Z2S_addZ2SDevice(joined_device, IAS_WD_SILENT_ALARM_SID, "SILENT ALARM", 
                                            SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, IAS_WD_LOUD_ALARM_SID, "LOUD ALARM", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                          } break;

                          case Z2S_DEVICE_DESC_MOES_ALARM: {
                            
                            Z2S_addZ2SDevice(joined_device, MOES_ALARM_SWITCH_SID, "ALARM SWITCH", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, MOES_ALARM_MELODY_SID, "ALARM MELODY", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, MOES_ALARM_VOLUME_SID, "ALARM VOLUME", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, MOES_ALARM_DURATION_SID, "ALARM DURATION", 
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, MOES_ALARM_DISPLAY_SID, "ALARM DISPLAY", 
                                             SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT);

                          } break;

                          case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_CO_SID,"CARBON MONOXIDE", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_CO_CONC_SID, "CO CONCENTRATION", 0, "ppm");
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_SELF_TEST_SID, "SELF TEST RESULT", 0, "1..3");
                            Z2S_addZ2SDevice(joined_device, TUYA_CO_DETECTOR_SILENCE_SID, "SILENCE");
                          } break;

                          case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_GAS_DETECTOR_GAS_SID,"GAS DETECTED", 
                                             SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
                            Z2S_addZ2SDevice(joined_device, TUYA_GAS_DETECTOR_GAS_CONC_SID, "GAS CONCENTRATION", 0, "%LEL");
                            Z2S_addZ2SDevice(joined_device, TUYA_GAS_DETECTOR_SELF_TEST_RESULT_SID, "SELF TEST RESULT", 0, "1..3");
                            Z2S_addZ2SDevice(joined_device, TUYA_GAS_DETECTOR_PREHEAT_SID, "PREHEAT");
                          } break;
                          
                          case Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, TUYA_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID,"TEMPHUMIDITY");
                            Z2S_addZ2SDevice(joined_device, TUYA_AIR_QUALITY_SENSOR_CO2_SID, "CO2", 0, "ppm");
                            Z2S_addZ2SDevice(joined_device, TUYA_AIR_QUALITY_SENSOR_VOC_SID, "VOC", 0, "ppm");
                            Z2S_addZ2SDevice(joined_device, TUYA_AIR_QUALITY_SENSOR_FA_SID, "FORMALDEHYDE", 0, "mg/m³");
                          } break;

                          case Z2S_DEVICE_DESC_DEVELCO_AIR_QUALITY_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, DEVELCO_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID, "TEMPHUMIDITY");
                            Z2S_addZ2SDevice(joined_device, DEVELCO_AIR_QUALITY_SENSOR_VOC_SID, "VOC", 0, "ppb");
                          } break;

                          case Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR: {
                            
                            Z2S_addZ2SDevice(joined_device, LUMI_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID, "T/H");
                            Z2S_addZ2SDevice(joined_device, LUMI_AIR_QUALITY_SENSOR_VOC_SID, "VOC", 0, "ppb");
                            Z2S_addZ2SDevice(joined_device, LUMI_AIR_QUALITY_SENSOR_AIR_QUALITY_SID, "AIR QUALITY", 0, "1-5");
                          } break;
                          
                          case Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP: {

                            Z2S_addZ2SDevice(joined_device, Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP_TEMP_SID, "TEMP");
                            Z2S_addZ2SDevice(joined_device, Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP_SWITCH_SID, "SWITCH",
                                             SUPLA_CHANNELFNC_POWERSWITCH);
                            Z2S_addZ2SDevice(joined_device, Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP_EM_SID);
                            
                          }break;

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

              if (hasTuyaCustomCluster(Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id))
                zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_QUERY_CMD, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);

              switch (Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id) { //(joined_device->model_id) {

                case 0x0000: break;     

                case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR:
                case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: 
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR:
                case Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR: 
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2:
                case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM:
                case Z2S_DEVICE_DESC_DEVELCO_IAS_ZONE_TEMP_SENSOR:
{
                  esp_zb_ieee_addr_t gateway_ieee_addr;
                  memset(gateway_ieee_addr, 0, sizeof(esp_zb_ieee_addr_t));
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                                               ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, sizeof(esp_zb_ieee_addr_t), &gateway_ieee_addr);
                  esp_zb_get_long_address(gateway_ieee_addr);
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                                               ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, sizeof(esp_zb_ieee_addr_t), &gateway_ieee_addr);

                } break;
                
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL:
		case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR: {
                } break;

                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
                case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
                case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
                case Z2S_DEVICE_DESC_RGBW_BULB_XY:
                case Z2S_DEVICE_DESC_DIMMER_BULB:
                case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:
                case Z2S_DEVICE_DESC_IKEA_WW_BULB:
                case Z2S_DEVICE_DESC_PHILIPS_WW_BULB:
                case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY: {

                  /*if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID, true))
                    log_i("Color control caps 0x%x, type 0x%x", *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, 0xE100, true)) {
                    esp_zb_uint48_t *value = (esp_zb_uint48_t *)zbGateway.getReadAttrLastResult()->data.value;
                    log_i("Color full data 0x%x::0x%x, type 0x%x", value->low, value->high, zbGateway.getReadAttrLastResult()->data.type);
                  }*/
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;

                case  Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER: {

                  joined_device->endpoint = Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[0].endpoint_id; //2
                  
                  bool sync_cmd = true;
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, sync_cmd);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 0, 232, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 0, 50, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 0, 10, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 0, 1, sync_cmd);

                    zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U48, 0, 0, 1, sync_cmd);

                } break; //[[fallthrough]];

                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1: 
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A: 
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {

                   /*bool sync_cmd = true;
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MULTIPLIER_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_DIVISOR_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U24, 0, 3600, 1, sync_cmd);
                   zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 3600, 1, sync_cmd);*/
                  
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
                  for(int n = 0; n < Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count; n++) {
                    joined_device->endpoint = ( Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count == 1) ? 
                                                1 : Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[n].endpoint_id;
                      //zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD010, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); 
                      //zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD030, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                  }
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;

                case Z2S_DEVICE_DESC_RELAY_1:
                case Z2S_DEVICE_DESC_TUYA_RELAY: {
                    
                  joined_device->endpoint = 0x01;
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x4003, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); //Tuya special
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;
                   
                case Z2S_DEVICE_DESC_SONOFF_TRVZB: {
                  
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 1200, 10, false);                 
                } break;
                
                case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH: {

                  /*write_mask_16 = 0x000B;
                  joined_device->endpoint = 2;
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0031, true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                      1, 1, PHILIPS_MANUFACTURER_CODE)) {

                    uint16_t philips_0031 = *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value;
                    log_i("Philips basic cluster attribute 0x0031 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, philips_0031);
                    if (philips_0031 != write_mask_16)
                      write_mask_16 = 0x0000;
                  }
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0031, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 
                                               2, &write_mask_16, true, 1, PHILIPS_MANUFACTURER_CODE);*/
                  /*if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0034, true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                      1, 1, PHILIPS_MANUFACTURER_CODE)) {

                    uint16_t philips_0034 = *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value;
                    log_i("Philips basic cluster attribute 0x0034 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, philips_0034);
                  }*/
                } break;

                case Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR:
                case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR:
                case Z2S_DEVICE_DESC_LUMI_SWITCH:
                case Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO:
                case Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR:
                case Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR: {

                  write_mask = 0x01;
                  joined_device->endpoint = 1;
                  zbGateway.sendAttributeWrite(joined_device, LUMI_CUSTOM_CLUSTER, LUMI_CUSTOM_CLUSTER_MODE_ID, ESP_ZB_ZCL_ATTR_TYPE_U8, 
                                               1, &write_mask, true, 1, LUMI_MANUFACTURER_CODE);
                  zbGateway.sendAttributeRead(joined_device, LUMI_CUSTOM_CLUSTER, 0x0148, true, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                  1, 1, LUMI_MANUFACTURER_CODE);
                } break;
              }

              SuplaDevice.scheduleSoftRestart(5000);
            }   
            //else log_i("LIST checking %s::%s, entry # %d",Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name, Z2S_DEVICES_LIST[devices_list_counter].model_name, devices_list_counter);
          }
      if (!device_recognized) {
        log_d("Unknown model %s::%s, no binding is possible", zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                     zbGateway.getQueryBasicClusterData()->zcl_model_name);
        //rgbLed.setPixelColor(0, rgbLed.Color(255, 0, 0));
        //rgbLed.show();
        rgbLedWrite(RGB_BUILTIN, 255, 0, 0);  // Green
        delay(1000);
        enableZ2SNotifications();
        zbGateway.setActivePairing(false);

        Z2S_startWebGUI();
        GUI_onLastBindingFailure(true);

      }
    }
  }
}
