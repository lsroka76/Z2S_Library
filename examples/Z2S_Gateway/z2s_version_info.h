#ifndef Z2S_VERSION_INFO_H_
#define Z2S_VERSION_INFO_H_ 

/*****************************************************************************/

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include <esp_ota_ops.h>

/*****************************************************************************/

#define Z2S_VERSION "Z2S-1.6.72-10/10/26"

/*****************************************************************************/

bool isVersionNewer(const char* current, const char* latest);

/*****************************************************************************/

bool isHardwareCapableOfOta();    

/*****************************************************************************/

bool checkGitHubForUpdate();

/*****************************************************************************/

void performGitHubOTA();

/*****************************************************************************/

#endif  
