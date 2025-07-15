#ifndef Z2S_WEB_GUI_H_
#define Z2S_WEB_GUI_H_

#include <ESPUI.h>
#include <ESPmDNS.h>
#include "ESPAsyncHTTPUpdateServer.h"

void Z2S_startWebGUIConfig();

void Z2S_buildWebGUI();

void Z2S_startWebGUI();

void Z2S_stopWebGUI();

void Z2S_updateWebGUI();

void Z2S_loopWebGUI();

void Z2S_startUpdateServer();

void GUI_onLastBindingFailure(bool binding_failed);

void GUI_onTuyaCustomClusterReceive(uint8_t command_id, uint16_t payload_size, uint8_t * payload_data);

#endif // Z2S_WEB_GUI_H_