/*
 * Z2S_C6_NCP.ino
 * Flash this sketch onto the ESP32-C6 when using ESP32-S3 + ESP32-C6 Dual-Chip mode!
 * Arduino IDE Settings for ESP32-C6:
 *   - Board: "ESP32C6 Dev Module"
 *   - Zigbee Mode: "Zigbee ZCZR (coordinator/router)"
 *   - Partition Scheme: "Zigbee ZCZR 4MB with spiffs"
 */

#define Z2S_C6_COPROCESSOR_FIRMWARE
#include "GeminiProtocol.h"
#include "ZigbeeCore.h"
#include "ZigbeeGateway.h"

void setup() {
  Serial.begin(115200);
  log_i("Starting Z2S ESP32-C6 Zigbee Co-Processor (Gemini NCP)...");

  // 1. Initialize 2 Mbps UART Bridge to ESP32-S3
  GeminiBridge::initUart();

  // 2. Register Gateway Endpoint & Start Native Zigbee Coordinator on C6
  Zigbee.addGatewayEndpoint(&zbGateway);
  Zigbee.setRebootOpenNetwork(0);

  if (Zigbee.begin(ZIGBEE_COORDINATOR, false)) {
    log_i("Zigbee Coordinator started on C6! Notifying S3 Host...");
    GeminiBridge::sendFrame(G_EVT_STACK_STARTED, nullptr, 0);
  } else {
    log_e("Failed to start Zigbee Coordinator on C6!");
  }
}

void loop() {
  // All work is handled by the FreeRTOS Zigbee & GeminiBridge tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}