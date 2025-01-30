#pragma once

#define Z2S_GATEWAY

#include <ZigbeeGateway.h>

#include "esp_coexist.h"

#include <SuplaDevice.h>
#include <priv_auth_data.h>
#include "Z2S_Devices_Database.h"
#include "Z2S_Devices_Table.h"

#include <supla/network/esp_wifi.h>
#include <supla/device/supla_ca_cert.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/clock/clock.h>

#undef USE_WEB_INTERFACE
//#define USE_WEB_INTERFACE

#ifdef USE_WEB_INTERFACE

#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/wifi_parameters.h>

Supla::EspWebServer                       suplaServer;

Supla::Html::DeviceInfo                   htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters               htmlWifi;
Supla::Html::ProtocolParameters           htmlProto;

#endif


#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2



ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi; //(SUPLA_WIFI_SSID, SUPLA_WIFI_SSID);
Supla::LittleFsConfig     configSupla;

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

bool zbInit = true;
uint8_t write_mask;

void setup() {
  
  log_i("setup start");

  pinMode(BUTTON_PIN, INPUT);

  eeprom.setStateSavePeriod(5000);

  new Supla::Clock;

  Supla::Storage::Init();

  auto cfg = Supla::Storage::ConfigInstance();

  cfg->commit();

#ifndef USE_WEB_INTERFACE

  log_i("undef webinterface");
  //cfg->setGUID(GUID);
  //cfg->setAuthKey(AUTHKEY);
  cfg->setWiFiSSID(SUPLA_WIFI_SSID);
  cfg->setWiFiPassword(SUPLA_WIFI_PASS);
  cfg->setSuplaServer(SUPLA_SVR);
  cfg->setEmail(SUPLA_EMAIL);

#endif

  Z2S_loadDevicesTable();

  Z2S_initSuplaChannels();

  //  Zigbee Gateway notifications

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onRMSVoltageReceive(Z2S_onRMSVoltageReceive);
  zbGateway.onRMSCurrentReceive(Z2S_onRMSCurrentReceive);
  zbGateway.onRMSActivePowerReceive(Z2S_onRMSActivePowerReceive);
  zbGateway.onBatteryPercentageReceive(Z2S_onBatteryPercentageReceive);
  zbGateway.onOnOffCustomCmdReceive(Z2S_onOnOffCustomCmdReceive);

  zbGateway.onCmdCustomClusterReceive(Z2S_onCmdCustomClusterReceive);

  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  //Supla
  
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  
  SuplaDevice.setName("Zigbee to Supla");
  //wifi.enableSSL(true);

  log_i("before SuplaDevice begin");
  SuplaDevice.begin();      
  
  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
}

zb_device_params_t *gateway_device;
zb_device_params_t *joined_device;

uint8_t counter = 0;
uint8_t tuya_dp_data[10];

void loop() {
  
  SuplaDevice.iterate();

  /*if (millis() - printTime > 10000) {
    //Zigbee.scanNetworks();
    if (zbGateway.getGatewayDevices().size() > 0 ) {
      if (esp_zb_is_started()) {//&& esp_zb_lock_acquire(portMAX_DELAY)) {
        zb_device_params_t *gt_device = zbGateway.getGatewayDevices().front();
	      log_i("short address before 0x%x",gt_device->short_addr);
        gt_device->short_addr = esp_zb_address_short_by_ieee(gt_device->ieee_addr);
        log_i("short address after 0x%x",gt_device->short_addr);
        if (counter == 0) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x65;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x01;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        //if (counter == 1) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x66;
          tuya_dp_data[3] = 0x02;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x04;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 0x00;
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 10, tuya_dp_data);
        //}
        if (counter == 2) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x65;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x00;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        if (counter == 3) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x6C;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x01;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        if (counter == 4) {          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x6C;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x02;
          
          //zbGateway.sendCustomClusterCmd(gt_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, tuya_dp_data);
        }
        counter++; if(counter > 4) counter = 0;
        //zbGateway.sendAttributeWrite(gt_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
          //                          ESP_ZB_ZCL_ATTR_TYPE_U64,8, gt_device->ieee_addr);
        //zbGateway.sendIASzoneEnrollResponseCmd(gt_device, ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS, 120);
        //gt_device->short_addr = 0xFFFF;
        //zbGateway.sendAttributeRead(gt_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, true);
        //zbGateway.setClusterReporting(  gt_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
          //                 ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 60, 1, true);
      }
   //esp_zb_lock_release();
  
    }
    printTime = millis();
  }*/

  //if (zbInit && wifi.isReady()) {
    if (zbInit && SuplaDevice.getCurrentStatus() == STATUS_REGISTERED_AND_READY) {
  
    Serial.println("zbInit");
    
    esp_coex_wifi_i154_enable();
  
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
      zbGateway.zbQueryDeviceBasicCluster(joined_device);
      
      uint16_t devices_list_table_size = sizeof(Z2S_DEVICES_LIST)/sizeof(Z2S_DEVICES_LIST[0]);
         uint16_t devices_desc_table_size = sizeof(Z2S_DEVICES_DESC)/sizeof(Z2S_DEVICES_DESC[0]);
      bool device_recognized = false;

          for (int i = 0; i < devices_list_table_size; i++) {
            
            if ((strcmp(zbGateway.getQueryBasicClusterData()->zcl_model_name, Z2S_DEVICES_LIST[i].model_name) == 0) &&
            (strcmp(zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name, Z2S_DEVICES_LIST[i].manufacturer_name) == 0)) {
              log_i(  "LIST matched %s::%s, entry # %d, endpoints # %d, endpoints 0x%x::0x%x,0x%x::0x%x,0x%x::0x%x,0x%x::0x%x",
                      Z2S_DEVICES_LIST[i].manufacturer_name, Z2S_DEVICES_LIST[i].model_name, i, 
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints_count,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[0].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[0].z2s_device_desc_id,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[1].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[1].z2s_device_desc_id,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[2].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[2].z2s_device_desc_id,
                      Z2S_DEVICES_LIST[i].z2s_device_endpoints[3].endpoint_id, Z2S_DEVICES_LIST[i].z2s_device_endpoints[3].z2s_device_desc_id );
  
              for (int j = 0; j < Z2S_DEVICES_LIST[i].z2s_device_endpoints_count; j++) {
              
                uint8_t endpoint_id = ( Z2S_DEVICES_LIST[i].z2s_device_endpoints_count == 1) ? 
                                        1 : Z2S_DEVICES_LIST[i].z2s_device_endpoints[j].endpoint_id; 
                                        
                uint32_t z2s_device_desc_id = ( Z2S_DEVICES_LIST[i].z2s_device_endpoints_count == 1) ?
                                                Z2S_DEVICES_LIST[i].z2s_device_desc_id :
                                                Z2S_DEVICES_LIST[i].z2s_device_endpoints[j].z2s_device_desc_id; 

                for (int k = 0; k < devices_desc_table_size; k++) {

                  if ( z2s_device_desc_id == Z2S_DEVICES_DESC[k].z2s_device_desc_id) {
                  log_i("DESC matched 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, endpoint 0x%x ",
                        Z2S_DEVICES_DESC[k].z2s_device_desc_id,   
                        Z2S_DEVICES_DESC[k].z2s_device_clusters_count,
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[0],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[1],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[2],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[3],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[4],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[5],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[6],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[7],
                        endpoint_id);

                        device_recognized = true;

                        joined_device->endpoint = endpoint_id;
                        joined_device->model_id = Z2S_DEVICES_DESC[k].z2s_device_desc_id;
                        
                        if(joined_device->model_id == Z2S_DEVICE_DESC_SWITCH_4X3)
                           {
                            Z2S_addZ2SDevice(joined_device,0);
                            Z2S_addZ2SDevice(joined_device,1);
                            Z2S_addZ2SDevice(joined_device,2);
                           }
                        else Z2S_addZ2SDevice(joined_device,-1);


                        //case Z2S_DEVICE_DESC_ON_OFF: {
                            //zbGateway.sendAttributeRead(joined_device, 0x0006,0x8001, true);
                            //zbGateway.sendAttributeRead(joined_device, 0x0006,0x8002, true);
                            //zbGateway.sendAttributeRead(joined_device, 0x0006,0x5000, true);
                            //zbGateway.sendAttributeRead(joined_device, 0x0006,0x8001, true);
                            //write_mask = 0x01;
                            //zbGateway.sendAttributeWrite(joined_device, 0x0006, 0x8004, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask );
                            //zbGateway.sendAttributeRead(joined_device, 0x0006,0x8004, true);
                          //} break;

                        for (int m = 0; m < Z2S_DEVICES_DESC[k].z2s_device_clusters_count; m++)
                          zbGateway.bindDeviceCluster(joined_device, Z2S_DEVICES_DESC[k].z2s_device_clusters[m]);
                        
                        switch (joined_device->model_id) {
                          case 0x0000: break;
                          case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                          case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1: {
                          } break;
                          case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: {
                            //log_i("Trying to configure cluster reporting on device (0x%x), endpoint (0x%x)", joined_device->short_addr, joined_device->endpoint);
                            //zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true);      
                            //zbGateway.readClusterReportCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true);
                            //zbGateway.readClusterReportCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 0x0021, true);
                            //zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                                        //  ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 60, 1, true);
                            //zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
                                                        //  0x0021, //ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID, 
                                 //                         ESP_ZB_ZCL_ATTR_TYPE_U8, 0, 4*60*60, 1, true);
                          } break;
                        }
                  }  
                  else 
                  log_i("DESC checking 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, endpoint %d ",
                        Z2S_DEVICES_DESC[k].z2s_device_desc_id,   
                        Z2S_DEVICES_DESC[k].z2s_device_clusters_count,
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[0],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[1],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[2],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[3], 
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[4],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[5],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[6],
                        Z2S_DEVICES_DESC[k].z2s_device_clusters[7],
                        endpoint_id);        
                  }
              }
              //here we can configure reporting and restart ESP32
              switch (joined_device->model_id) {
                case 0x0000: break;
      
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1: {
                } break;
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: {
                  /*log_i("Trying to configure cluster reporting on device (0x%x), endpoint (0x%x)", joined_device->short_addr, joined_device->endpoint);
                  zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true);      
                  zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 0x0021, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                                ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 60, 1, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 
                                                0x0021, //ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID, 
                                                ESP_ZB_ZCL_ATTR_TYPE_U8, 0, 4*60*60, 1, true);

                  */
                  //esp_zb_ieee_addr_t addr;
                  //esp_zb_get_long_address(addr);
                  //zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                  //     ESP_ZB_ZCL_ATTR_TYPE_U64,sizeof(esp_zb_ieee_addr_t),&addr);
                  //delay(200);
                  //zbGateway.sendIASzoneEnrollResponseCmd(joined_device, ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS, 120);
                  //delay(200);
                  //zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true);
                  
                  
                } break;
              }
              //zbGateway.setClusterReporting( joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                //                        ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 30, 300, 1);
      
              SuplaDevice.scheduleSoftRestart(5000);
            }   
            else log_i("LIST checking %s::%s, entry # %d",Z2S_DEVICES_LIST[i].manufacturer_name, Z2S_DEVICES_LIST[i].model_name, i);
          }
      if (!device_recognized) log_d(  "Unknown model %s::%s, no binding is possible", zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                     zbGateway.getQueryBasicClusterData()->zcl_model_name);
    }
  }
}