#define Z2S_C6_COPROCESSOR_FIRMWARE

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "GeminiProtocol.h"
#include "ZigbeeCore.h"
#include "ZigbeeGateway.h"

#define WIFI_ENABLE     3
#define WIFI_ANT_CONFIG 14

void setup() {
  esp_log_level_set("*", ESP_LOG_INFO);
  Serial.begin(115200);

  pinMode(WIFI_ENABLE, OUTPUT);
  pinMode(WIFI_ANT_CONFIG, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH);

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);
  Zigbee.addEndpoint(&zbGateway);
  Zigbee.setRebootOpenNetwork(0);

  GeminiBridge::initUart();

  log_i("Z2S C6 NCP booted - waiting up to 3s for S3 Host G_CMD_ZB_BEGIN...");

  // S3 reaches Zigbee.begin() at ~1350ms, so it will start C6 via G_CMD_ZB_BEGIN
  // (including honoring erase_nvs if S3 is a fresh installation!)
  uint32_t wait_start = millis();
  while (!Zigbee.started() && (millis() - wait_start < 3000)) {
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  // Fallback if C6 was powered on alone before S3
  if (!Zigbee.started()) {
    log_i("Starting C6 Coordinator from NVRAM...");
    if (Zigbee.begin(ZIGBEE_COORDINATOR, false)) {
      GeminiBridge::sendStackStartedEvt();
      log_i("C6 Zigbee Coordinator started!");
    } else {
      log_e("C6 Zigbee Coordinator failed to start! Rebooting...");
      delay(1000);
      esp_restart();
    }
  }

  disableLoopWDT();
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}