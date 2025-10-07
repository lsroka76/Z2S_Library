#ifndef Z2S_TELNET_SERVER_H_
#define Z2S_TELNET_SERVER_H_

#include "ESPTelnet.h"

#define MAX_TELNET_CMD_PARAM_NUMBER 12

extern ESPTelnet telnet;
extern bool is_Telnet_server;

void onTelnetCmd(void (*_callback)(char *cmd, uint8_t params_number, char **params));

void log_i_telnet(char *log_line, bool toTelnet = false);

void setupTelnet();

#endif //Z2S_TELNET_SERVER_H_
