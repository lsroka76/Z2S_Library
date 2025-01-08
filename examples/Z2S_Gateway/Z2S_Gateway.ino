#pragma once

#include <ZigbeeGateway.h>

#include "esp_coexist.h"

//#include <SuplaDevice.h>
//#include <supla/sensor/virtual_binary.h>
//#include <supla/network/esp_wifi.h>

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

//Supla::ESPWifi wifi("xxx", "xxx");

//Supla::Sensor::VirtualBinary *ZBS_TS0203_1;
//Supla::Sensor::VirtualBinary *ZBS_TS0203_2;


void sz_ias_zone_notification(int status, uint8_t *ieee_addr_64)
{
    Serial.println("in sz_ias_zone_nitification");
    Serial.println(status);
    Serial.println(ieee_addr_64[0]);
}

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

bool zbInit = true;

void setup() {

  pinMode(BUTTON_PIN, INPUT);

  /*char GUID[SUPLA_GUID_SIZE] = {0xA4,0x45,0xDF,0xAF,0x7F,0xB1,0x16,0x39,0xB1,0xBB,0xB8,0x48,0x18,0x65,0x81,0xF8};
  char AUTHKEY[SUPLA_AUTHKEY_SIZE] = {0x09,0xB6,0xF1,0x47,0x59,0xD7,0x73,0x5E,0xCE,0x43,0x9C,0x96,0xF1,0x43,0xF2,0xEC};
  
  ZBS_TS0203_1 = new Supla::Sensor::VirtualBinary();
  ZBS_TS0203_1->setDefaultFunction(SUPLA_CHANNELFNC_OPENINGSENSOR_WINDOW);
  ZBS_TS0203_1->set();
  
  ZBS_TS0203_2 = new Supla::Sensor::VirtualBinary();
  ZBS_TS0203_2->setDefaultFunction(SUPLA_CHANNELFNC_OPENINGSENSOR_WINDOW);
  ZBS_TS0203_2->set();
*/
  //  Zigbee

  zbGateway.onStatusNotification(sz_ias_zone_notification);

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  //Supla
  
  //SuplaDevice.begin(GUID,              // Global Unique Identifier 
    //                "svr3.supla.org",  // SUPLA server address
      //              "l.sroka@gmail.com",   // Email address used to login to Supla Cloud
        //            AUTHKEY);          // Authorization key
  
  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
}

zb_device_params_t *gateway_device;
zb_device_params_t *joined_device;

char zbd_model_name[64];

uint16_t gateway_devices_list_size = 0;

void loop() {
  
//  SuplaDevice.iterate();
  // put your main code here, to run repeatedly:

  //if (zbInit && wifi.isReady())
  if (zbInit)
  {
    Serial.println("zbInit");
    
  //  esp_coex_wifi_i154_enable();
  
    if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
      Serial.println("Zigbee failed to start!");
      Serial.println("Rebooting...");
      ESP.restart();
    }
    zbInit = false;
    startTime = millis();
 }
  
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
      strcpy(zbd_model_name,zbGateway.readModel(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      log_i("model %s ", zbd_model_name);

      if ((strcmp(zbd_model_name,"ZigbeeTempSensor") == 0)||
          (strcmp(zbd_model_name,"TS0201") == 0)) {
          esp_zb_lock_acquire(portMAX_DELAY);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT);
          esp_zb_lock_release();
      } else 
      if ((strcmp(zbd_model_name,"TS0203") == 0)||
          (strcmp(zbd_model_name,"TS0202") == 0)) {
          esp_zb_lock_acquire(portMAX_DELAY);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
          esp_zb_lock_release();
          zbGateway.setIASZReporting(joined_device->short_addr, joined_device->endpoint, 10, 20);
      } else 
          if (strcmp(zbd_model_name,"TS0044") == 0) {
          esp_zb_lock_acquire(portMAX_DELAY);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
          esp_zb_lock_release();
      } else
          if (strcmp(zbd_model_name,"TS011F") == 0) {
          esp_zb_lock_acquire(portMAX_DELAY);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT);    
          esp_zb_lock_release();
          }
        else
          if (strcmp(zbd_model_name,"TS0601") == 0) {
          esp_zb_lock_acquire(portMAX_DELAY);
          zbGateway.bindDeviceCluster(joined_device, 61184);
          //zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT);
          //zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT);
          //zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING);
          //zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT);    
          esp_zb_lock_release();
          }
      else
      log_d("Unknown model %s, no binding is possible", zbd_model_name);

    }
  }
}