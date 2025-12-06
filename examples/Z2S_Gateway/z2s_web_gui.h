#ifndef Z2S_WEB_GUI_H_
#define Z2S_WEB_GUI_H_

#include <ESPUI.h>
#include <ESPmDNS.h>
#include "ESPAsyncHTTPUpdateServer.h"

typedef enum gui_modes {

  no_gui_mode = 0,
  minimal_gui_mode = 1,
  standard_gui_mode = 2,
  extended_gui_mode = 3,
  full_gui_mode = 4,
  developer_gui_mode = 5,
  supla_gui_mode = 6,
  gui_modes_number
} gui_modes_t;

void Z2S_startWebGUIConfig();

void Z2S_buildWebGUI(
  gui_modes_t mode = minimal_gui_mode, uint32_t gui_custom_flags = 0);

void Z2S_startWebGUI();

void Z2S_reloadWebGUI();

void Z2S_stopWebGUI();

void Z2S_updateWebGUI();

void Z2S_loopWebGUI();

void Z2S_startUpdateServer();

void GUI_onLastBindingFailure(bool binding_failed);

void GUI_onTuyaCustomClusterReceive(uint8_t command_id, 
                                    uint16_t payload_size, 
                                    uint8_t * payload_data);

void GUI_onZigbeeOpenNetwork(bool is_network_open);

#endif // Z2S_WEB_GUI_H_