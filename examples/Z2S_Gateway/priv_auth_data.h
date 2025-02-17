#ifndef priv_auth_data_
#define priv_auth_data_
#include <supla-common/proto.h>
static char SUPLA_WIFI_SSID[20]="";
static char SUPLA_WIFI_PASS[50]="";
static char SUPLA_EMAIL[50]="";
static char SUPLA_SVR[50]="";

static char GUID[SUPLA_GUID_SIZE] = {0xA4};
static char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x09};
static int ZG_OPEN_NETWORK=180;
static int ZG_SCENE_SWITCH_DEBOUNCE_TIME_MS=1500;
#endif // priv_auth_data_