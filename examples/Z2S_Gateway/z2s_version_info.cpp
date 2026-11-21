#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <esp_ota_ops.h>

#include <SuplaDevice.h>
#include <supla/device/sw_update.h>

#include "z2s_version_info.h"

/*****************************************************************************/

extern uint8_t _enable_cloud_ota;

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

enum class OtaTaskState : uint8_t {
  IDLE,
  RUNNING,
  SUCCESS,
  FAILED,
  NO_UPDATES
};

static volatile OtaTaskState otaTaskState = OtaTaskState::IDLE;
static volatile size_t otaCurrentBytes = 0;
static volatile size_t otaTotalBytes = 0;
static otaProgressCallback s_guiOtaProgressCb = nullptr;
static volatile uint32_t s_guiOtaRequestMs = 0;

void setGitHubOtaProgressCallback(otaProgressCallback progressCb) {
  s_guiOtaProgressCb = progressCb;
}

void markGuiGitHubOtaRequest(bool requested) {
  s_guiOtaRequestMs = requested ? (millis() | 1UL) : 0;
}

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

  static int8_t cachedResult = -1;
  
  if (cachedResult >= 0) {
    return cachedResult == 1;
  }
  
  uint32_t flashSize = ESP.getFlashChipSize();

  if (flashSize < (8 * 1024 * 1024)) {

    log_e("OTA Error: Insufficient flash. Found %u MB, require 8 MB.",
          flashSize / (1024 * 1024));

    cachedResult = 0;
    return false;
  }

  const esp_partition_t* update_partition =
    esp_ota_get_next_update_partition(nullptr);

  if (update_partition == nullptr) {

    log_e("OTA Error: No alternate OTA partition found.");

    cachedResult = 0;
    return false;
  }

  size_t currentSketchSize = ESP.getSketchSize();

  if (update_partition->size < currentSketchSize) {

    log_e("OTA Error: Update partition '%s' is too small (%u bytes).",
          update_partition->label, update_partition->size);
    
    cachedResult = 0;
    return false;
  }

  log_i("OTA Check Passed: Flash=%u MB, Target Partition='%s' (%u bytes)",
        flashSize / (1024 * 1024), update_partition->label,
        update_partition->size);

  cachedResult = 1;
  return true;
}

/*****************************************************************************/

bool checkGitHubForUpdate(
  char *response_buffer, size_t response_buffer_size, char *latest_tag_buffer,
  size_t latest_tag_buffer_size) {
  
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

      if (latest_tag_buffer && latest_tag_buffer_size > 0) {
        
        snprintf(latest_tag_buffer, latest_tag_buffer_size, "%s", latestTag);
      }

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

  int16_t lastLoggedPct = -10;

  httpUpdate.onProgress(
    [activeOtaProgressCb, lastLoggedPct](size_t current, size_t total) mutable {
      otaCurrentBytes = current;
      otaTotalBytes = total;

      if (activeOtaProgressCb) {
        activeOtaProgressCb(current, total);
      } else if (total > 0) {
        int16_t currentOtaProgress =
          static_cast<int16_t>((current * 100) / total);
        if (currentOtaProgress > 100) {
          currentOtaProgress = 100;
        }
        if (currentOtaProgress >= (lastLoggedPct + 10) ||
            (currentOtaProgress == 100 && lastLoggedPct != 100)) {
          lastLoggedPct = currentOtaProgress;
          log_i("OTA Progress: %02d %%", currentOtaProgress);
        }
      }
    });

  log_i("Starting OTA from GitHub: %s", FIRMWARE_URL);

  if (activeOtaProgressCb) {
    activeOtaProgressCb(1, 0);
  }

  t_httpUpdate_return ret = httpUpdate.update(client, FIRMWARE_URL);
  OtaTaskState finalState = OtaTaskState::FAILED;

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      if (activeOtaProgressCb) {
        activeOtaProgressCb(2, 0);
      }
      log_e("OTA Failed. Error (%d): %s",
            httpUpdate.getLastError(),
            httpUpdate.getLastErrorString().c_str());
      finalState = OtaTaskState::FAILED;
      break;

    case HTTP_UPDATE_NO_UPDATES:
      log_i("No updates found.");
      finalState = OtaTaskState::NO_UPDATES;
      break;

    case HTTP_UPDATE_OK:
      if (activeOtaProgressCb) {
        activeOtaProgressCb(0, 0);
      }
      log_i("OTA Successful. Rebooting...");
      finalState = OtaTaskState::SUCCESS;
      if (SuplaDevice.getDeviceMode() != Supla::DEVICE_MODE_SW_UPDATE) {
        SuplaDevice.scheduleSoftRestart(1000);
      }
      break;
  }

  log_i("OTA Task finished. Deleting thread.");
  otaTaskHandle = nullptr;
  otaTaskState = finalState;
  vTaskDelete(nullptr);
}

/*****************************************************************************/

void startBackgroundGitHubOtaTask(otaProgressCallback progressCb) {
  if (otaTaskHandle == nullptr) {
    if (isHardwareCapableOfOta()) {
      otaCurrentBytes = 0;
      otaTotalBytes = 0;
      otaTaskState = OtaTaskState::RUNNING;

      BaseType_t res = xTaskCreate(
        performBackgroundGitHubOta,
        "GitHub_OTA",
        8192,
        reinterpret_cast<void *>(progressCb),
        1,
        &otaTaskHandle);

      if (res != pdPASS) {
        otaTaskHandle = nullptr;
        otaTaskState = OtaTaskState::FAILED;
        log_e("Failed to create OTA task (Out of RAM)!");
      }
    } else {
      otaTaskState = OtaTaskState::FAILED;
      log_e("Aborting OTA due to hardware limits.");
    }
  } else {
    log_i("OTA is already running in the background!");
  }
}
/*****************************************************************************/

#if defined(USE_GEMINI_PROTOCOL_HOST)
#include "GeminiProtocol.h"

static constexpr const char NCP_FIRMWARE_URL[] =
  "https://github.com/lsroka76/Z2S_Library/"
  "releases/latest/download/Z2S_C6_NCP.4MB.no_logs.update_only.bin";

void performBackgroundGitHubNcpOta(void *pvParameters) {
  otaProgressCallback activeOtaProgressCb =
    reinterpret_cast<otaProgressCallback>(pvParameters);

  WiFiClientSecure client;
  client.setInsecure();
  client.setHandshakeTimeout(10);

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(10000);

  log_i("Starting C6 NCP OTA from GitHub: %s", NCP_FIRMWARE_URL);
  if (activeOtaProgressCb) activeOtaProgressCb(1, 0);

  bool success = false;
  if (http.begin(client, NCP_FIRMWARE_URL)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      int totalLen = http.getSize();
      WiFiClient *stream = http.getStreamPtr();

      if (GeminiBridge::hostNcpOtaBegin()) {
        uint8_t buf[256];
        int writtenTotal = 0;
        bool writeOk = true;

        while (http.connected() && (totalLen < 0 || writtenTotal < totalLen)) {
          size_t avail = stream->available();
          if (avail > 0) {
            size_t toRead = (avail > sizeof(buf)) ? sizeof(buf) : avail;
            int readBytes = stream->readBytes(buf, toRead);
            if (readBytes > 0) {
              if (!GeminiBridge::hostNcpOtaWrite(buf, readBytes)) {
                writeOk = false;
                break;
              }
              writtenTotal += readBytes;
              if (activeOtaProgressCb && totalLen > 0) {
                activeOtaProgressCb(writtenTotal, totalLen);
              }
            }
          } else {
            vTaskDelay(pdMS_TO_TICKS(5));
          }
        }

        if (writeOk && GeminiBridge::hostNcpOtaEnd()) {
          success = true;
        }
      }
    }
    http.end();
  }

  if (success) {
    if (activeOtaProgressCb) activeOtaProgressCb(0, 0);
    log_i("C6 NCP GitHub OTA Successful! Rebooting S3...");
    SuplaDevice.scheduleSoftRestart(1000);
  } else {
    if (activeOtaProgressCb) activeOtaProgressCb(2, 0);
    log_e("C6 NCP GitHub OTA Failed!");
  }

  otaTaskHandle = nullptr;
  vTaskDelete(nullptr);
}
#endif

/*****************************************************************************/

void startBackgroundGitHubNcpOtaTask(otaProgressCallback progressCb) {
#if defined(USE_GEMINI_PROTOCOL_HOST)
  if (otaTaskHandle == nullptr) {
    BaseType_t res = xTaskCreate(
      performBackgroundGitHubNcpOta,
      "GitHub_NCP_OTA",
      8192,
      reinterpret_cast<void *>(progressCb),
      1,
      &otaTaskHandle);
    if (res != pdPASS) {
      otaTaskHandle = nullptr;
      log_e("Failed to create NCP OTA task!");
    }
  }
#endif
}

/*****************************************************************************/

class Z2sGitHubSwUpdate : public Supla::Device::SwUpdate {
 public:
  Z2sGitHubSwUpdate(SuplaDeviceClass *sdc,
                    const char *newUrl,
                    Supla::SwUpdateMode mode)
      : Supla::Device::SwUpdate(sdc, newUrl, mode) {}

  void iterate() override {
    if (!isStarted()) {
      return;
    }

    if (mode == Supla::SwUpdateMode::PeriodicCheckAndUpdate) {
      finished = true;
      abort = true;
      retryAllowed = false;
      return;
    }

    // 1. Supla Cloud "Check for update" button
    if (mode == Supla::SwUpdateMode::OnlyCheck) {
      if (!_enable_cloud_ota) {
        if (sdc) {
          sdc->addLastStateLog("SW update: Cloud OTA disabled in Web GUI");
        }
        notifyFinished(Supla::Device::SwUpdateResult::UP_TO_DATE);
        finished = true;
        abort = true;
        retryAllowed = false;
        return;
      }

      if (!isHardwareCapableOfOta()) {
        if (sdc) {
          sdc->addLastStateLog("SW update: hardware not capable of OTA");
        }
        notifyFinished(Supla::Device::SwUpdateResult::UP_TO_DATE);
        finished = true;
        abort = true;
        retryAllowed = false;
        return;
      }

      char latestTag[32] = {};
      if (checkGitHubForUpdate(nullptr, 0, latestTag, sizeof(latestTag)) &&
          latestTag[0] != '\0') {
        size_t verLen = strlen(latestTag) + 1;
        newVersion = new char[verLen];
        if (newVersion) {
          snprintf(newVersion, verLen, "%s", latestTag);
        }

        char releaseUrl[128] = {};
        int written = snprintf(
          releaseUrl, sizeof(releaseUrl),
          "https://%s/lsroka76/Z2S_Library/releases/tag/%s",
          GITHUB_HOST, latestTag);

        if (written > 0 &&
            static_cast<size_t>(written + 1) < SUPLA_URL_PATH_MAXSIZE) {
          changelogUrl = new char[written + 1];
          if (changelogUrl) {
            snprintf(changelogUrl, written + 1, "%s", releaseUrl);
          }
        }

        notifyFinished(true);
      } else {
        notifyFinished(Supla::Device::SwUpdateResult::UP_TO_DATE);
      }

      finished = true;
      abort = true;
      retryAllowed = false;
      return;
    }

    // 2. Supla Cloud or GUI "Start firmware update"
    if (mode == Supla::SwUpdateMode::CheckAndUpdate) {
      if (!taskStarted) {
        uint32_t reqMs = s_guiOtaRequestMs;
        s_guiOtaRequestMs = 0;
        bool fromGui = (reqMs != 0) && ((uint32_t)(millis() - reqMs) < 5000);

        if (!_enable_cloud_ota && !fromGui) {
          if (sdc) {
            sdc->addLastStateLog("SW update: Cloud OTA disabled in Web GUI");
          }
          notifyFinished(false, "Cloud OTA disabled in Web GUI");
          finished = true;
          abort = true;
          retryAllowed = false;
          return;
        }

        if (!isHardwareCapableOfOta()) {
          if (sdc) {
            sdc->addLastStateLog("SW update: hardware not capable of OTA");
          }
          if (s_guiOtaProgressCb) s_guiOtaProgressCb(2, 0);
          notifyFinished(false, "Hardware not capable of OTA");
          finished = true;
          abort = true;
          retryAllowed = false;
          return;
        }

        if (s_guiOtaProgressCb) s_guiOtaProgressCb(1, 0);

        // Cloud clicks check version first; GUI already checked via Check button
        if (!fromGui) {
          char latestTag[32] = {};
          if (!checkGitHubForUpdate(nullptr, 0, latestTag, sizeof(latestTag))) {
            if (sdc) {
              sdc->addLastStateLog("SW update: firmware is already up to date");
            }
            if (s_guiOtaProgressCb) s_guiOtaProgressCb(2, 0);
            notifyFinished(Supla::Device::SwUpdateResult::UP_TO_DATE);
            finished = true;
            abort = true;
            retryAllowed = false;
            return;
          }
        }

        startBackgroundGitHubOtaTask(nullptr);
        if (otaTaskState != OtaTaskState::RUNNING) {
          if (sdc) {
            sdc->addLastStateLog("SW update: failed to start GitHub OTA task");
          }
          if (s_guiOtaProgressCb) s_guiOtaProgressCb(2, 0);
          notifyFinished(false, "Failed to start OTA task");
          finished = true;
          abort = true;
          retryAllowed = false;
          return;
        }

        taskStarted = true;
        lastReportedBytes = SIZE_MAX;
        notifyProgress(0, 0);
        return;
      }

      size_t cur = otaCurrentBytes;
      size_t total = otaTotalBytes;
      if (cur != lastReportedBytes) {
        lastReportedBytes = cur;
        if (s_guiOtaProgressCb && total > 0) {
          s_guiOtaProgressCb(cur, total);
        }
        notifyProgress(static_cast<uint32_t>(cur),
                       static_cast<uint32_t>(total));
      }

      OtaTaskState state = otaTaskState;
      if (state == OtaTaskState::RUNNING) {
        return;
      }

      otaTaskState = OtaTaskState::IDLE;

      if (state == OtaTaskState::SUCCESS) {
        if (s_guiOtaProgressCb) s_guiOtaProgressCb(0, 0);
        notifyProgress(static_cast<uint32_t>(total),
                       static_cast<uint32_t>(total));
        notifyFinished(true);
        finished = true;
        abort = false;
      } else {
        const char *reason = (state == OtaTaskState::NO_UPDATES)
                                 ? "SW update: no updates found"
                                 : "SW update: GitHub OTA failed";
        if (sdc) {
          sdc->addLastStateLog(reason);
        }
        if (s_guiOtaProgressCb) s_guiOtaProgressCb(2, 0);
        notifyFinished(false, reason);
        finished = true;
        abort = true;
        retryAllowed = false;
      }
      return;
    }
  }

 private:
  bool taskStarted = false;
  size_t lastReportedBytes = 0;
};

Supla::Device::SwUpdate *Supla::Device::SwUpdate::Create(
    SuplaDeviceClass *sdc, const char *newUrl, Supla::SwUpdateMode mode) {
  return new Z2sGitHubSwUpdate(sdc, newUrl, mode);
}