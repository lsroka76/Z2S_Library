#ifndef Z2S_TELNET_SERVER_H_
#define Z2S_TELNET_SERVER_H_

#include "ESPTelnet.h"

#define MAX_TELNET_CMD_PARAM_NUMBER 12

// ESPTelnetBase::flush() waits for availableForWrite(), which NetworkClient
// doesn't have: every flush ends with a timeout counted as a failed write
// and 3 of them disconnect the client (e.g. a reconnect from the same IP).
// Writes are sent at once, so there is nothing to flush.
class Z2S_Telnet : public ESPTelnet {
 public:
  void flush() override {}
};

extern Z2S_Telnet telnet;
extern bool is_Telnet_server;

void onTelnetCmd(void (*_callback)(char *cmd, uint8_t params_number, char **params));

void log_i_telnet(char *log_line, bool toTelnet = false);

void setupTelnet();

#endif //Z2S_TELNET_SERVER_H_
