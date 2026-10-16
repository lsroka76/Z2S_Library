#ifndef Z2S_VERSION_INFO_H_
#define Z2S_VERSION_INFO_H_ 

/*****************************************************************************/

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include <esp_ota_ops.h>

/*****************************************************************************/

#define Z2S_VERSION "Z2S-1.6.88-16/10/26"

/*****************************************************************************/

typedef void (*otaProgressCallback)(size_t, size_t);

/*****************************************************************************/

bool isVersionNewer(const char* current, const char* latest);

/*****************************************************************************/

bool isHardwareCapableOfOta();    

/*****************************************************************************/

bool checkGitHubForUpdate(
  char *response_buffer = nullptr, size_t response_buffer_size = 0);

/*****************************************************************************/

void startBackgroundGitHubOtaTask(otaProgressCallback progressCb = nullptr);

/*****************************************************************************/

#endif  
