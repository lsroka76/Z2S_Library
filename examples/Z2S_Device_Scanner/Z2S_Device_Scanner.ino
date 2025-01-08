#pragma once

#define EP_CLUSTERS_ATTRIBUTES_SCANNER 1

#include <ZigbeeGateway.h>

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);



void sz_ias_zone_notification(int status, uint8_t *ieee_addr_64)
{

}

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

bool zbInit = true;

void setup() {

  pinMode(BUTTON_PIN, INPUT);

  //  Zigbee

  zbGateway.onStatusNotification(sz_ias_zone_notification);

  zbGateway.setManufacturerAndModel("Zigbee2Supla", "Z2S_Device_Scanner");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
      Serial.println("Zigbee failed to start!");
      Serial.println("Rebooting...");
      ESP.restart();
  }
  
  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
}

zb_device_params_t *gateway_device;
zb_device_params_t *joined_device;

char zbd_model_name[64];


uint16_t gateway_devices_list_size = 0;

void loop() {
  
  if (digitalRead(BUTTON_PIN) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 5000) {
        // If key pressed for more than 5 secs, factory reset Zigbee and reboot
        Serial.printf("Resetting Zigbee to factory settings, reboot.\n");
        Zigbee.factoryReset();
      }
    }
    Zigbee.openNetwork(180);
  }
  delay(100);

  if (zbGateway.isNewDeviceJoined()) {

    zbGateway.clearNewDeviceJoined();
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty())
    {
      joined_device = zbGateway.getLastJoinedDevice();
      
      log_i("manufacturer %s ", zbGateway.readManufacturer(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      log_i("model %s ", zbGateway.readModel(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      
#ifdef EP_CLUSTERS_ATTRIBUTES_SCANNER        
    
    zbGateway.zbPrintDeviceDiscovery(joined_device);
      
#endif //EP_CLUSTERS_ATTRIBUTES_SCANNER
    }
  }
}