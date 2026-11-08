#define Z2S_C6_COPROCESSOR_FIRMWARE
#include "GeminiProtocol.h"
#include "ZigbeeCore.h"
#include "ZigbeeGateway.h"

void setup() {

  // 1. Start UART immediately (~120 ms after power-on) so we never miss S3's packet
  Zigbee.addGatewayEndpoint(&zbGateway);
  GeminiBridge::initUart();

  Serial.begin(115200);
  log_i("Z2S C6 NCP ready - waiting up to 2s for S3 Host G_CMD_ZB_BEGIN...");

  // 2. Wait up to 2 seconds for S3 to send G_CMD_ZB_BEGIN (with S3's channel_mask)
  uint32_t wait_start = millis();
  while (!Zigbee.started() && (millis() - wait_start < 2000)) {
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  // 3. Fallback: If C6 was powered on before S3, start coordinator from C6 NVRAM.
  //    When S3 boots later and sends G_CMD_ZB_BEGIN, C6 will reply immediately!
  if (!Zigbee.started()) {
    log_i("Starting C6 Coordinator from NVRAM (S3 will sync when it boots)...");
    Zigbee.setRebootOpenNetwork(0);
    if (Zigbee.begin(ZIGBEE_COORDINATOR, false)) {
      GeminiBridge::sendFrame(G_EVT_STACK_STARTED, nullptr, 0);
    }
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}