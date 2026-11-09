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

  // Start UART Bridge and wait for S3 Host to send G_CMD_ZB_BEGIN!
  GeminiBridge::initUart();
  log_i("Z2S C6 NCP ready - waiting for S3 Host G_CMD_ZB_BEGIN...");

  disableCore0WDT();
  disableLoopWDT();
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}