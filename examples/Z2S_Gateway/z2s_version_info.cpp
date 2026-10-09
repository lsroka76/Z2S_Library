/*****************************************************************************/

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include <esp_ota_ops.h>

#include "z2s_version_info.h"

/*****************************************************************************/

const char* GITHUB_API_URL = "https://api.github.com/repos/lsroka76/"
"Z2S_Library/releases/latest";

const char* firmwareUrl = "https://github.com/lsroka76/Z2S_Library/"
"releases/latest/download/Z2S_Gateway.8MB.OTA.no_logs.update_only.bin";

/*****************************************************************************/

bool isVersionNewer(const char* current, const char* latest) {

    int currentMajor = 0, currentMinor = 0, currentPatch = 0;
    int latestMajor = 0, latestMinor = 0, latestPatch = 0;
  
    if (sscanf(current, "Z2S-%d.%d.%d", &currentMajor, &currentMinor, 
          &currentPatch) != 3) {

      log_e("Failed to parse local version.");
      return false;
    }

    if (sscanf(latest, "v%d.%d.%d", &latestMajor, &latestMinor, 
          &latestPatch) != 3) {
        
      log_e("Failed to parse GitHub version.");
      return false; 
    }

    if (latestMajor != currentMajor) return latestMajor > currentMajor;
    if (latestMinor != currentMinor) return latestMinor > currentMinor;
    return latestPatch > currentPatch;
}

/*****************************************************************************/

bool isHardwareCapableOfOta() {
    
  uint32_t flashSize = ESP.getFlashChipSize();
  
  if (flashSize < (8 * 1024 * 1024)) {
    
    log_e(
      "OTA Error: Insufficient flash. Found %u MB, require 8 MB.\n", 
      flashSize / (1024 * 1024));

    return false;
  }

  const esp_partition_t* update_partition = esp_ota_get_next_update_partition(
    NULL);

  if (update_partition == NULL) {
    
    log_e(
      "OTA Error: No alternate OTA partition found. Check your partition"
      " scheme.");
    
    return false;
  }
 
  size_t currentSketchSize = ESP.getSketchSize();
  
  if (update_partition->size < currentSketchSize) {
        
    log_e(
      "OTA Error: Update partition '%s' is too small (%u bytes) to fit a "
      "gateway app.", update_partition->label, update_partition->size);
    
    return false;
  }

  log_e(
    "OTA Check Passed: Flash=%u MB, Target Partition='%s' (%u bytes)\n", 
    flashSize / (1024 * 1024), update_partition->label, 
    update_partition->size);
    
  return true;
}

/*****************************************************************************/

bool checkGitHubForUpdate() {

  WiFiClientSecure client;
  client.setInsecure(); 

  HTTPClient http;
  http.begin(client, GITHUB_API_URL);
  http.setUserAgent("ZigBee2Supla-Gateway");
    
  int httpCode = http.GET();
  bool updateAvailable = false;

  if (httpCode == HTTP_CODE_OK) {
    
    JsonDocument filter;
    filter["tag_name"] = true;
        
    JsonDocument doc;
    DeserializationError error = deserializeJson(
      doc, http.getStream(), DeserializationOption::Filter(filter));

    if (!error) {
            
      const char* latestTag = doc["tag_name"].as<const char*>();
            
      log_i("Current: %s | Latest: %s\n", Z2S_VERSION, latestTag);
            
      if (isVersionNewer(Z2S_VERSION, latestTag)) {
                
        log_i("New firmware release detected.");
            
        updateAvailable = true;
      }
    } 
    else {

      log_e("JSON parse failed: %s\n", error.c_str());
    }
  } 
  else {
        
    log_e("GitHub API failed with HTTP Code: %d\n", httpCode);
  }
    
  http.end();
  return updateAvailable;
}

/*****************************************************************************/

void performGitHubOTA() {

  WiFiClientSecure client;
  client.setInsecure(); 

  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  log_i("Starting OTA from GitHub...");
  
  t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);

  switch (ret) {


    case HTTP_UPDATE_FAILED:
        
      log_e(
        "OTA Failed. Error (%d): %s", httpUpdate.getLastError(), 
        httpUpdate.getLastErrorString().c_str());
    break;


    case HTTP_UPDATE_NO_UPDATES:

      log_i("No updates found.");
    break;
    
    
    case HTTP_UPDATE_OK:
        
      log_i("OTA Successful. Rebooting...");
      ESP.restart();
    break;
  }
}

/*****************************************************************************/
