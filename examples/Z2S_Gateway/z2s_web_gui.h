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

#endif // Z2S_WEB_GUI_H_