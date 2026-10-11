/*****************************************************************************/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include <esp_ota_ops.h>

#include <SuplaDevice.h>

#include "z2s_version_info.h"

/*****************************************************************************/

const char *GITHUB_API_URL = "https://api.github.com/repos/lsroka76/"
"Z2S_Library/releases/latest";

const char* firmwareUrl = "https://github.com/lsroka76/Z2S_Library/"
"releases/latest/download/Z2S_Gateway.8MB.OTA.no_logs.update_only.bin";

/*****************************************************************************/

TaskHandle_t otaTaskHandle = NULL;

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

static const char *parse_github_ota_version_no_tag_value_error = "Parse "
  "error: Found tag_name but no value.";

static const char *parse_github_ota_version_no_tag_name_error = "Parse error:"
  " tag_name not found in stream.";

static const char *parse_github_ota_version_api_error = "GitHub API failed "
  "with HTTP Code:";

/*****************************************************************************/

bool checkGitHubForUpdate(char *response_buffer, size_t response_buffer_size) {

  WiFiClientSecure client;
  client.setInsecure(); 
  client.setTimeout(5); 

  HTTPClient http;
  http.begin(client, GITHUB_API_URL);
  http.setUserAgent("ZigBee2Supla-Gateway");
    
  int httpCode = http.GET();
  bool updateAvailable = false;

  if (httpCode == HTTP_CODE_OK) {
        
    Stream& stream = http.getStream();
    stream.setTimeout(5000); 
        
    
    if (stream.find("\"tag_name\"")) {
            
      if (stream.find("\"")) {
                
        char latestTag[32] = {0}; 
        size_t len = stream.readBytesUntil(
          '"', latestTag, sizeof(latestTag) - 1);
        latestTag[len] = '\0';
                
        log_i("Current: %s | Latest: %s", Z2S_VERSION, latestTag);
                
        if (isVersionNewer(Z2S_VERSION, latestTag)) {

          if (response_buffer)
          snprintf(
            response_buffer, response_buffer_size, "New GitHub firmware "
            "found:<br>current: %s | latest: %s", Z2S_VERSION, latestTag);

          
          log_i("New firmware release found!");
          updateAvailable = true;
        }
        else {

          if (response_buffer)
          snprintf(
            response_buffer, response_buffer_size, "No new GitHub firmware"
            " found!");
        }
      } 
      else {
        
        if (response_buffer)
          snprintf(
            response_buffer, response_buffer_size, 
            parse_github_ota_version_no_tag_value_error);

        log_e("%s", parse_github_ota_version_no_tag_value_error);
      }
    } 
    else {

      if (response_buffer)
          snprintf(
            response_buffer, response_buffer_size, 
            parse_github_ota_version_no_tag_name_error);
            
      log_e("%s", parse_github_ota_version_no_tag_name_error);
    }
  } 
  else {
    
    if (response_buffer)
          snprintf(
            response_buffer, response_buffer_size, 
            parse_github_ota_version_api_error, httpCode);

    log_e("GitHub API failed with HTTP Code: %d", httpCode);
  }
    
  http.end(); 
    
  return updateAvailable;
}

/*****************************************************************************/

void performBackgroundGitHubOta(void *pvParameters) {

  WiFiClientSecure client;
  client.setInsecure(); 

  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  otaProgressCallback activeOtaProgressCb = (otaProgressCallback)pvParameters;

  if (activeOtaProgressCb) {

    httpUpdate.onProgress(activeOtaProgressCb);
  } 
  else {

    httpUpdate.onProgress([](size_t current, size_t total) {
        
      size_t currentOtaProgress = (current * 100) / total;
        
      if (currentOtaProgress % 10 == 0) {

        log_i("OTA Progress: %02d %%", currentOtaProgress);

      }
    });
  }

  log_i("Starting OTA from GitHub...");

  if (activeOtaProgressCb) 
    activeOtaProgressCb(1, 0);
  
  httpUpdate.rebootOnUpdate(false); 

  t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);

  switch (ret) {


    case HTTP_UPDATE_FAILED: {
        
      if (activeOtaProgressCb) 
        activeOtaProgressCb(2, 0);
      
      log_e(
        "OTA Failed. Error (%d): %s", httpUpdate.getLastError(), 
        httpUpdate.getLastErrorString().c_str());
    } break;


    case HTTP_UPDATE_NO_UPDATES:

      log_i("No updates found.");
    break;
    
    
    case HTTP_UPDATE_OK: {
        
      if (activeOtaProgressCb) 
        activeOtaProgressCb(0, 0);

      log_i("OTA Successful. Rebooting...");
      
      SuplaDevice.scheduleSoftRestart(1000);
    } break;
  }

  log_i("OTA Task finished. Deleting thread.");
  otaTaskHandle = NULL;
  vTaskDelete(NULL);  
}

/*****************************************************************************/

void startBackgroundGitHubOtaTask(otaProgressCallback progressCb) {

  if (otaTaskHandle == NULL) {
            
    if (isHardwareCapableOfOta()) {
                
      xTaskCreate(
        performBackgroundGitHubOta, "GitHub_OTA", 8192, (void *)progressCb, 1, 
        &otaTaskHandle);
                
    } 
    else {
      
      log_e("Aborting OTA due to hardware limits.");
    }
  } 
  else {
    
    log_i("OTA is already running in the background!");
  }
}