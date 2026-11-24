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

  // Input is read one character per loop and kept after a disconnect: the
  // rest of the last line (e.g. "q" of "quit") would start the next command.
  void clearInput() {
    input = "";
    _iac_state = IAC_NONE;
  }

 protected:
  // Skips telnet commands (IAC ...), e.g. option negotiation sent by clients
  // after connecting: option codes like 0x27 are printable characters and
  // would start the first command.
  void handleInput() override {
    if (_iac_state == IAC_NONE) {
      if (client.peek() != IAC) {
        ESPTelnet::handleInput();
        return;
      }
      client.read();
      _iac_state = IAC_COMMAND;
      return;
    }
    uint8_t c = client.read();
    switch (_iac_state) {
      case IAC_COMMAND:
        if ((c >= WILL) && (c <= DONT))
          _iac_state = IAC_OPTION;
        else if (c == SB)
          _iac_state = IAC_SUBNEGOTIATION;
        else
          _iac_state = IAC_NONE;
        break;
      case IAC_SUBNEGOTIATION:
        if (c == IAC)
          _iac_state = IAC_SUBNEGOTIATION_IAC;
        break;
      case IAC_SUBNEGOTIATION_IAC:
        _iac_state = (c == SE) ? IAC_NONE : IAC_SUBNEGOTIATION;
        break;
      default:
        _iac_state = IAC_NONE;
        break;
    }
  }

 private:
  static const uint8_t IAC = 0xFF;
  static const uint8_t DONT = 0xFE;
  static const uint8_t WILL = 0xFB;
  static const uint8_t SB = 0xFA;
  static const uint8_t SE = 0xF0;

  enum {
    IAC_NONE,
    IAC_COMMAND,
    IAC_OPTION,
    IAC_SUBNEGOTIATION,
    IAC_SUBNEGOTIATION_IAC
  } _iac_state = IAC_NONE;
};

extern Z2S_Telnet telnet;
extern bool is_Telnet_server;

void onTelnetCmd(void (*_callback)(char *cmd, uint8_t params_number, char **params));

void log_i_telnet(char *log_line, bool toTelnet = false);

void setupTelnet();

#endif //Z2S_TELNET_SERVER_H_
