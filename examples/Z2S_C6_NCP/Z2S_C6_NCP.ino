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

  if (Zigbee.begin(ZIGBEE_COORDINATOR, false)) {
    GeminiBridge::sendStackStartedEvt();
    log_i("C6 Zigbee Coordinator started!");
  } else {
    log_e("C6 Zigbee Coordinator failed to start! Rebooting...");
    delay(1000);
    esp_restart();
  }

  disableCore0WDT();
  disableLoopWDT();
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}