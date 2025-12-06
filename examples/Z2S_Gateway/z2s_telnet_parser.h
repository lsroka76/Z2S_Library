#ifndef Z2S_TELNET_PARSER_H_
#define Z2S_TELNET_PARSER_H_

#include <SuplaDevice.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/channels/channel.h>
#include <supla/actions.h>
#include <supla/events.h>

#include "z2s_devices_table.h"
#include "z2s_version_info.h"
#include "z2s_web_gui.h"


extern bool GUIstarted;

void Z2S_onTelnetCmd(
  char *cmd, uint8_t params_number, char **param);

#endif Z2S_TELNET_PARSER_H_
