#ifndef Z2S_WEB_GUI_H_
#define Z2S_WEB_GUI_H_

#define ESPASYNCHTTPUPDATESERVER_MODE 0

#include <ESPUI.h>
#include <ESPmDNS.h>
#include "ESPAsyncHTTPUpdateServer.h"

#define MAX_ZIGBEE_PAYLOAD_SIZE         255

#define PAYLOAD_ERROR_MSG_HELPER(MSG, MSG2) MSG # MSG2
#define PAYLOAD_ERROR_MSG(MSG, MSG2) PAYLOAD_ERROR_MSG_HELPER(MSG, MSG2)

#define GUI_COMMANDS_POOL_SIZE 32

inline const char* rootCACertificatePushover = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
"MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2Vyd  SW5jMRkwFwYDVQQLExB3d3cuZGlnaWNlcnQuY29t\n" \
"MSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBHMjCCASIwDQYJKoZIhvcN\n" \
"AQEBBQADggEPADCCAQoCggEBALStS1dknq5kB82RKf97p3R0W79ZAn0SGrYj4R7J\n" \
"5Hh1kF88ZkZ40a/9D7gKAt4Yw6D4T139hXF1B1S8k7y8f6q8D/iHIsyJvUxlR6B2\n" \
"8x8o1V583sM2D0P3t4R5u6t5r3xU5h9fGqSInE7v68sW1s8E6y6jVvI5f2e8D6Xh\n" \
"E8fR3T4F01f8nI/u4U/Y6C3P1b6s7WqI4G6P2G4r/6M5W16hE0S1E6fS7n1K8V8x\n" \
"0C5S6mU3n9Y8zYJ5lW6X3V8P3I7hWv3f6Y5sXW8D3xS6n3f6Y5sXW8D3xS6n3f6Y\n" \
"5sXW8D3xS6n3f6Y5sXW8D3xS6n3f6Y5sXW8D3xS6n3f6Y5sXW8D3xS6n3f6Y5sXW\n" \
"8D3xS6n3f6Y5sXW8D3xS6n3f6Y5sXW8D3xS6n3f6Y5sXW8D3xS6n3f6Y5sXW8D3x\n" \
"-----END CERTIFICATE-----\n";


typedef enum gui_modes {

  no_gui_mode = 0,
  minimal_gui_mode = 1,
  standard_gui_mode = 2,
  extended_gui_mode = 3,
  full_gui_mode = 4,
  developer_gui_mode = 5,
  supla_gui_mode = 6,
  gateway_ca_mode = 7,
  gateway_tcc_mode = 8,
  gateway_ca_tcc_mode = 9,
  gateway_devices_channels_mode = 10,
  gateway_no_local_actions_mode = 11,
  full_ad_gui_mode = 12,
  full_sb_gui_mode = 13,
  standard_ad_gui_mode = 14,
  gui_modes_number
} gui_modes_t;

enum gui_commands {

  gui_cmd_none,
  gui_cmd_sort_channels_selectors,
  gui_cmd_sort_zbdevices_selectors,
  gui_cmd_sort_zbdevices_and_channels_selectors,
  gui_cmd_sort_attributes_selectors,
  gui_cmd_sort_attribute_types_selectors,
  gui_cmd_sort_attribute_values_selectors,
  gui_cmd_sort_Tuya_datapoints_selectors,
  gui_cmd_cluster_callback,
  gui_cmd_add_logic_gate,
  gui_cmd_add_virtual_relay,
  gui_cmd_add_virtual_binary,
  gui_cmd_add_virtual_hvac,
  gui_cmd_add_remote_relay,
  gui_cmd_add_remote_thermometer,
  gui_cmd_add_virtual_thermhygrometer,
  gui_cmd_add_switchbot_1x,
  gui_cmd_add_switchbot_2x,
  gui_cmd_save_switchbot_data,
  gui_cmd_add_local_relay,
  gui_cmd_remove_local_channel,
  gui_cmd_edit_channel_data,
  gui_cmd_update_channel_info,
  gui_cmd_update_zbdevice_info
};

class Z2S_Core;

size_t mbstrnlen(const char *mb_str, size_t max_bytes);

bool Z2S_isGUIStarted();
bool Z2S_isGUIBuilt();

void Z2S_initWebGUI();

void Z2S_buildWebGUI(
  gui_modes_t mode = minimal_gui_mode, uint32_t gui_custom_flags = 0);

void Z2S_startWebGUI();

void Z2S_stopWebGUI();

void Z2S_updateWebGUI();

void Z2S_loopWebGUI();

void Z2S_startUpdateServer();

void GUI_onLastBindingFailure(bool binding_failed);

void GUI_onTuyaCustomClusterReceive(
  uint8_t command_id, uint16_t payload_size, uint8_t * payload_data);

void GUI_onZigbeeOpenNetwork(bool is_network_open);

void sortZbDevicesSelectors(int32_t selected_value = -1);

void sortChannelsSelectors(int32_t selected_value = -1);

void sortZbDevicesAndChannelsSelectors();


#endif // Z2S_WEB_GUI_H_