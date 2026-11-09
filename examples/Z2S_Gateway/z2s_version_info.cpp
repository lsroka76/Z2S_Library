#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <esp_ota_ops.h>

#include <SuplaDevice.h>

#include "z2s_version_info.h"

/*****************************************************************************/

static constexpr const char GITHUB_HOST[] = "github.com";
static constexpr const char GITHUB_LATEST_PATH[] =
  "/lsroka76/Z2S_Library/releases/latest";

#if CONFIG_IDF_TARGET_ESP32S3
static constexpr const char FIRMWARE_URL[] =
  "https://github.com/lsroka76/Z2S_Library/"
  "releases/latest/download/Z2S_Gateway.S3_Host.8MB.OTA.no_logs.update_only.bin";
#else
static constexpr const char FIRMWARE_URL[] =
  "https://github.com/lsroka76/Z2S_Library/"
  "releases/latest/download/Z2S_Gateway.8MB.OTA.no_logs.update_only.bin";
#endif

static constexpr const char PARSE_ERR_NO_LOCATION[] =
  "Parse error: Redirect Location header not found.";
static constexpr const char PARSE_ERR_BAD_TAG[] =
  "Parse error: Version tag not found in Location header.";
static constexpr const char PARSE_ERR_CONN_FAIL[] =
  "GitHub connection failed.";

/*****************************************************************************/

static TaskHandle_t otaTaskHandle = nullptr;

/*****************************************************************************/

bool isVersionNewer(const char* current, const char* latest) {
  int currentMajor = 0, currentMinor = 0, currentPatch = 0;
  int latestMajor = 0, latestMinor = 0, latestPatch = 0;

  if (sscanf(current, "Z2S-%d.%d.%d", &currentMajor, &currentMinor,
             &currentPatch) != 3) {
    log_e("Failed to parse local version: %s", current);
    return false;
  }

  if (sscanf(latest, "v%d.%d.%d", &latestMajor, &latestMinor,
             &latestPatch) != 3) {
    log_e("Failed to parse GitHub version: %s", latest);
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
    log_e("OTA Error: Insufficient flash. Found %u MB, require 8 MB.",
          flashSize / (1024 * 1024));
    return false;
  }

  const esp_partition_t* update_partition =
    esp_ota_get_next_update_partition(nullptr);

  if (update_partition == nullptr) {
    log_e("OTA Error: No alternate OTA partition found.");
    return false;
  }

  size_t currentSketchSize = ESP.getSketchSize();

  if (update_partition->size < currentSketchSize) {
    log_e("OTA Error: Update partition '%s' is too small (%u bytes).",
          update_partition->label, update_partition->size);
    return false;
  }

  log_i("OTA Check Passed: Flash=%u MB, Target Partition='%s' (%u bytes)",
        flashSize / (1024 * 1024), update_partition->label,
        update_partition->size);

  return true;
}

/*****************************************************************************/

bool checkGitHubForUpdate(char *response_buffer, size_t response_buffer_size) {
  WiFiClientSecure client;
  client.setInsecure();
  client.setHandshakeTimeout(5); // Handshake timeout is in SECONDS (5s)
  client.setTimeout(5000);       // Stream/read timeout is in MILLISECONDS (5000ms)

  bool updateAvailable = false;

  if (!client.connect(GITHUB_HOST, 443)) {
    if (response_buffer) {
      snprintf(response_buffer, response_buffer_size, "%s", PARSE_ERR_CONN_FAIL);
    }
    log_e("%s", PARSE_ERR_CONN_FAIL);
    return false;
  }

  // Request /releases/latest without following redirects.
  // GitHub responds with a tiny HTTP 302 Found containing /releases/tag/vX.Y.Z
  client.printf(
    "GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: ZigBee2Supla-Gateway\r\n"
    "Connection: close\r\n\r\n",
    GITHUB_LATEST_PATH, GITHUB_HOST);

  // Wait up to 5000 ms for the first response bytes while yielding to FreeRTOS
  uint32_t startMs = millis();
  while (client.connected() && !client.available() && (millis() - startMs < 5000)) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  // GitHub replies with:
  // Location: https://github.com/lsroka76/Z2S_Library/releases/tag/vX.Y.Z
  if (client.find("/releases/tag/")) {
    char latestTag[32] = {0};
    size_t len = client.readBytesUntil('\r', latestTag, sizeof(latestTag) - 1);
    latestTag[len] = '\0';

    // Strip any trailing quote (if matched in body) or whitespace/newline
    for (size_t i = 0; i < len; ++i) {
      if (latestTag[i] == '"' || latestTag[i] == '\n' || latestTag[i] == ' ') {
        latestTag[i] = '\0';
        len = i;
        break;
      }
    }

    if (len > 0) {
      log_i("Current: %s | Latest: %s", Z2S_VERSION, latestTag);

      if (isVersionNewer(Z2S_VERSION, latestTag)) {
        if (response_buffer) {
          snprintf(
            response_buffer, response_buffer_size,
            "New GitHub firmware found:<br>current: %s | latest: %s",
            Z2S_VERSION, latestTag);
        }
        log_i("New firmware release found!");
        updateAvailable = true;
      } else {
        if (response_buffer) {
          snprintf(
            response_buffer, response_buffer_size,
            "No new GitHub firmware found! (latest: %s)", latestTag);
        }
      }
    } else {
      if (response_buffer) {
        snprintf(response_buffer, response_buffer_size, "%s", PARSE_ERR_BAD_TAG);
      }
      log_e("%s", PARSE_ERR_BAD_TAG);
    }
  } else {
    if (response_buffer) {
      snprintf(response_buffer, response_buffer_size, "%s", PARSE_ERR_NO_LOCATION);
    }
    log_e("%s", PARSE_ERR_NO_LOCATION);
  }

  client.stop();
  return updateAvailable;
}

/*****************************************************************************/

void performBackgroundGitHubOta(void *pvParameters) {
  WiFiClientSecure client;
  client.setInsecure();
  client.setHandshakeTimeout(10);

  httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  httpUpdate.rebootOnUpdate(false);

  otaProgressCallback activeOtaProgressCb =
    reinterpret_cast<otaProgressCallback>(pvParameters);

  if (activeOtaProgressCb) {
    httpUpdate.onProgress(activeOtaProgressCb);
  } else {
    httpUpdate.onProgress([](size_t current, size_t total) {
      if (total == 0) return;

      static size_t lastLoggedPct = 0xFF;
      size_t currentOtaProgress = (current * 100) / total;

      if ((currentOtaProgress % 10 == 0) && (currentOtaProgress != lastLoggedPct)) {
        lastLoggedPct = currentOtaProgress;
        log_i("OTA Progress: %02u %%", static_cast<unsigned>(currentOtaProgress));
      }
    });
  }

  log_i("Starting OTA from GitHub: %s", FIRMWARE_URL);

  if (activeOtaProgressCb) {
    activeOtaProgressCb(1, 0);
  }

  t_httpUpdate_return ret = httpUpdate.update(client, FIRMWARE_URL);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      if (activeOtaProgressCb) {
        activeOtaProgressCb(2, 0);
      }
      log_e("OTA Failed. Error (%d): %s",
            httpUpdate.getLastError(),
            httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      log_i("No updates found.");
      break;

    case HTTP_UPDATE_OK:
      if (activeOtaProgressCb) {
        activeOtaProgressCb(0, 0);
      }
      log_i("OTA Successful. Rebooting...");
      SuplaDevice.scheduleSoftRestart(1000);
      break;
  }

  log_i("OTA Task finished. Deleting thread.");
  otaTaskHandle = nullptr;
  vTaskDelete(nullptr);
}

/*****************************************************************************/

void startBackgroundGitHubOtaTask(otaProgressCallback progressCb) {
  if (otaTaskHandle == nullptr) {
    if (isHardwareCapableOfOta()) {
      BaseType_t res = xTaskCreate(
        performBackgroundGitHubOta,
        "GitHub_OTA",
        8192,
        reinterpret_cast<void *>(progressCb),
        1,
        &otaTaskHandle);

      if (res != pdPASS) {
        otaTaskHandle = nullptr;
        log_e("Failed to create OTA task (Out of RAM)!");
      }
    } else {
      log_e("Aborting OTA due to hardware limits.");
    }
  } else {
    log_i("OTA is already running in the background!");
  }
}