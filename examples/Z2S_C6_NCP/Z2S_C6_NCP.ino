/*
 * Z2S_C6_NCP.ino
 * ESP32-C6 Zigbee Co-Processor (NCP) Firmware for Z2S Gateway (ESP32-S3 Host + ESP32-C6 NCP)
 */

#define Z2S_C6_COPROCESSOR_FIRMWARE

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "GeminiProtocol.h"
#include "ZigbeeCore.h"
#include "ZigbeeGateway.h"

#define WIFI_ENABLE     3
#define WIFI_ANT_CONFIG 14

#define Z2S_NCP_VERSION "NCP-1.3.0-25/11/26"

void setup() {
  esp_log_level_set("*", ESP_LOG_INFO);
  Serial.begin(115200);

  // 1. Configure C6 RF antenna switch pins (same as standalone Z2S_Gateway)
  pinMode(WIFI_ENABLE, OUTPUT);
  pinMode(WIFI_ANT_CONFIG, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH);

  // 2. Configure Zigbee Gateway endpoint & attributes on the C6
  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);
  Zigbee.addEndpoint(&zbGateway);
  Zigbee.setRebootOpenNetwork(0);

  // 3. Start 2 Mbps UART Bridge to ESP32-S3 Host immediately
  GeminiBridge::setNcpVersion(Z2S_NCP_VERSION);
  GeminiBridge::initUart();

  log_i("Z2S C6 NCP booted - waiting up to 2s for S3 Host G_CMD_ZB_BEGIN...");

  // 4. Wait up to 2 seconds for S3 Host to send G_CMD_ZB_BEGIN (with S3's channel_mask)
  uint32_t wait_start = millis();
  while (!Zigbee.started() && (millis() - wait_start < 2000)) {
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  // 5. Fallback / sync: If S3 hasn't sent G_CMD_ZB_BEGIN yet, start coordinator from C6 NVRAM.
  //    Serialized with G_CMD_ZB_BEGIN so both tasks can never call Zigbee.begin() concurrently.
  if (!GeminiBridge::ncpEnsureZigbeeStarted()) {
    log_e("C6 Zigbee Coordinator failed to start! Rebooting...");
    delay(1000);
    esp_restart();
  }

  // 6. Disable Task Watchdogs on C6 (same as standalone Z2S_Gateway)
  disableCore0WDT();
  disableLoopWDT();
  
  if (Zigbee.getZigbeeTaskHandle() != NULL) {
    esp_task_wdt_delete(Zigbee.getZigbeeTaskHandle());
  }
}

void loop() {
  // All Zigbee & UART Bridge work runs in dedicated FreeRTOS tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}
