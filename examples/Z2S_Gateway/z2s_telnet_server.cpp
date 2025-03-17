#include "z2s_telnet_server.h"

ESPTelnet telnet;
IPAddress ip;
uint16_t  port = 23;
bool is_Telnet_server = false;

void (*_on_Telnet_cmd)(char *cmd, uint8_t params_number, char **params) = NULL;

char separator_line[] = "--------------------------------------------------------------------------------";

/* ------------------------------------------------- */

// (optional) callback functions for telnet events
void onTelnetConnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" connected");
  
  telnet.println("\n>Welcome " + telnet.getIP());
  telnet.println(">Use quit  to disconnect.");
}

void onTelnetDisconnect(String ip) {
  Serial.print(">- Telnet: ");
  Serial.print(ip);
  Serial.println("> disconnected");
}

void onTelnetReconnect(String ip) {
  Serial.print(">- Telnet: ");
  Serial.print(ip);
  Serial.println("> reconnected");
}

void onTelnetConnectionAttempt(String ip) {
  Serial.print(">- Telnet: ");
  Serial.print(ip);
  Serial.println("> tried to connected");
}

void onTelnetCmd(void (*_callback)(char *cmd, uint8_t params_number, char **params)) {
  _on_Telnet_cmd = _callback;
}

void log_i_telnet(char *log_line, bool toTelnet) {
  log_i("%s", log_line);
  if (toTelnet) {
    telnet.println(separator_line);
    telnet.println(log_line);
    telnet.println(separator_line);
  }
}

void onTelnetInput(String str) {

  str.toUpperCase();

  char *cmd_line = strdup(str.c_str());
  char *cmd_name = NULL;
  char *params[MAX_TELNET_CMD_PARAM_NUMBER];
  uint8_t params_number = 0;

  memset(params, 0, sizeof(params));

  cmd_name = strtok(cmd_line, " ");
  
  if (cmd_name != NULL) {

    for (int i = 0; i < MAX_TELNET_CMD_PARAM_NUMBER; i++) {
      params[i] = strtok(NULL, " ");
      if (params[i] == NULL) break;
      params_number = i+1; 
    }
    if (strcmp(cmd_name, "QUIT") == 0) {
      telnet.println(">disconnecting you...");
      telnet.disconnectClient();
    } else {
      if(_on_Telnet_cmd)
      _on_Telnet_cmd(cmd_name, params_number, params);
    }
  }
  free(cmd_line);
}

/* ------------------------------------------------- */

void setupTelnet() {  
  // passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  telnet.onInputReceived(onTelnetInput);

  Serial.print("- Telnet: ");
  if (telnet.begin(port)) {
    is_Telnet_server = true;
    Serial.println("Telnet running");
  } else {
    is_Telnet_server = false;
    Serial.println("Telnet error.");
  }
}
