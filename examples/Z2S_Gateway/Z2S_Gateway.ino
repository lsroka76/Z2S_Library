//#pragma once

#define Z2S_GATEWAY

#include <ZigbeeGateway.h>

#include "esp_coexist.h"

#include <SuplaDevice.h>

#include "priv_auth_data.h"
#include "z2s_devices_database.h"
#include "z2s_devices_table.h"

#include <supla/network/esp_wifi.h>
#include <supla/device/supla_ca_cert.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/clock/clock.h>
#include <supla/actions.h>
#include <supla/control/button.h>
#include <action_handler_with_callbacks.h>

#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include "z2s_version_info.h"
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/network/html/text_cmd_input_parameter.h>
#include <supla/network/html/select_cmd_input_parameter.h>

#include <HTTPUpdateServer.h>

Supla::EspWebServer                       suplaServer;
HTTPUpdateServer                          httpUpdater;

Supla::Html::DeviceInfo                   htmlDeviceInfo(&SuplaDevice);
Supla::Html::CustomDevInfo                htmlCustomDevInfo;
Supla::Html::WifiParameters               htmlWifi;
Supla::Html::ProtocolParameters           htmlProto;

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2
#define CFG_BUTTON_PIN              9  //Boot button for C6/H2
#define WIFI_ENABLE                 3
#define WIFI_ANT_CONFIG             14

#define REFRESH_PERIOD              60 * 1000 //miliseconds

#define USE_WEB_CONFIG_ON_STARTUP

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi; //(SUPLA_WIFI_SSID, SUPLA_WIFI_SSID);
Supla::LittleFsConfig     configSupla (2048);

uint32_t startTime = 0;
uint32_t printTime = 0;
uint32_t zbInit_delay = 0;

uint32_t refresh_time = 0;

bool zbInit = true;
uint8_t write_mask;
uint16_t write_mask_16;

const static char PARAM_CMD1[] = "zigbeestack";
const static char PARAM_CMD2[] = "Z2S devices";

void supla_callback_bridge(int event, int action) {
  log_i("supla_callback_bridge - event(0x%x), action(0x%x)", event, action);
  switch (event) {
    case Supla::ON_EVENT_1:
    case Supla::ON_CLICK_1: Zigbee.openNetwork(180); break;
    case Supla::ON_EVENT_2:
    case Supla::ON_CLICK_5: Zigbee.factoryReset(); break;
    case Supla::ON_EVENT_3: 
    case Supla::ON_CLICK_10: Z2S_clearDevicesTable(); break;
  }
  if (event >= Supla::ON_EVENT_4) {
    z2s_devices_table[event - Supla::ON_EVENT_4].valid_record = false;
    if (Z2S_saveDevicesTable()) {
      log_i("Device on channel %d removed. Restarting...", z2s_devices_table[event - Supla::ON_EVENT_4].Supla_channel);
      SuplaDevice.scheduleSoftRestart(1000);
    }
  }
}

void setup() {
  
  log_i("setup start");

  pinMode(BUTTON_PIN, INPUT);

  pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT); (credits @Zibi_007)
  digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control
  delay(100);
  pinMode(WIFI_ANT_CONFIG, OUTPUT); // pinMode(14, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH);
 
  eeprom.setStateSavePeriod(5000);

  new Supla::Clock;

  Supla::Storage::Init();

  auto cfg = Supla::Storage::ConfigInstance();

  cfg->commit();

#ifndef USE_WEB_CONFIG_ON_STARTUP

  log_i("no web config on startup - using hardcoded data");
  //cfg->setGUID(GUID);
  //cfg->setAuthKey(AUTHKEY);
  //cfg->generateGuidAndAuthkey();
  cfg->setWiFiSSID(SUPLA_WIFI_SSID);
  cfg->setWiFiPassword(SUPLA_WIFI_PASS);
  cfg->setSuplaServer(SUPLA_SVR);
  cfg->setEmail(SUPLA_EMAIL);

#endif

  auto selectCmd = new Supla::Html::SelectCmdInputParameter(PARAM_CMD1, "Z2S Commands");
  selectCmd->registerCmd("OPEN ZIGBEE NETWORK (180 SECONDS)", Supla::ON_EVENT_1);
  selectCmd->registerCmd("!RESET ZIGBEE STACK!", Supla::ON_EVENT_2);
  selectCmd->registerCmd("!!CLEAR Z2S TABLE!! (RESET RECOMMENDED)", Supla::ON_EVENT_3);
  
  //selectCmd->registerCmd("TOGGLE", Supla::ON_EVENT_3);

  auto AHwC = new Supla::ActionHandlerWithCallbacks();
  AHwC->setActionHandlerCallback(supla_callback_bridge);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_1, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_2, true);
  selectCmd->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_3, true);

  auto buttonCfg = new Supla::Control::Button(CFG_BUTTON_PIN, true, true);

  buttonCfg->setHoldTime(2000);
  buttonCfg->setMulticlickTime(500);

  buttonCfg->configureAsConfigButton(&SuplaDevice);

  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_1);
  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_5);
  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_10);

  
  Z2S_loadDevicesTable();

  Z2S_initSuplaChannels();

  auto selectCmd2 = new Supla::Html::SelectCmdInputParameter(PARAM_CMD2, "Z2S Devices");
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      char device_removal_cmd[128];
      sprintf(device_removal_cmd, "Remove Z2SDevice [%x:%x:%x:%x:%x:%x:%x:%x] channel # %d",
      z2s_devices_table[devices_counter].ieee_addr[7], z2s_devices_table[devices_counter].ieee_addr[6], z2s_devices_table[devices_counter].ieee_addr[5],
      z2s_devices_table[devices_counter].ieee_addr[4], z2s_devices_table[devices_counter].ieee_addr[3], z2s_devices_table[devices_counter].ieee_addr[2],
      z2s_devices_table[devices_counter].ieee_addr[1], z2s_devices_table[devices_counter].ieee_addr[0], z2s_devices_table[devices_counter].Supla_channel);
      log_i("cmd %s, len %d", device_removal_cmd, strlen(device_removal_cmd));
      selectCmd2->registerCmd(device_removal_cmd, Supla::ON_EVENT_4 + devices_counter);
      selectCmd2->addAction(Supla::TURN_ON, AHwC, Supla::ON_EVENT_4 + devices_counter, true);
    }
  //  Zigbee Gateway notifications

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onIlluminanceReceive(Z2S_onIlluminanceReceive);
  zbGateway.onOccupancyReceive(Z2S_onOccupancyReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onRMSVoltageReceive(Z2S_onRMSVoltageReceive);
  zbGateway.onRMSCurrentReceive(Z2S_onRMSCurrentReceive);
  zbGateway.onRMSActivePowerReceive(Z2S_onRMSActivePowerReceive);
  zbGateway.onCurrentSummationReceive(Z2S_onCurrentSummationReceive);
  zbGateway.onCurrentLevelReceive(Z2S_onCurrentLevelReceive);
  zbGateway.onColorHueReceive(Z2S_onColorHueReceive);
  zbGateway.onColorSaturationReceive(Z2S_onColorSaturationReceive);
  zbGateway.onBatteryPercentageReceive(Z2S_onBatteryPercentageReceive);
  zbGateway.onCustomCmdReceive(Z2S_onCustomCmdReceive);

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
  
  SuplaDevice.setName("Zigbee to Supla Gateway");
  //wifi.enableSSL(true);

  SuplaDevice.begin();      
  
  httpUpdater.setup(suplaServer.getServerPtr(), "/update", "admin", "pass");

  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
  refresh_time = millis();
}

zbg_device_params_t *gateway_device;
zbg_device_params_t *joined_device;

uint8_t counter = 0;
uint8_t tuya_dp_data[10];

void loop() {
  
  SuplaDevice.iterate();

  if ((!Zigbee.started()) && SuplaDevice.getCurrentStatus() == STATUS_REGISTERED_AND_READY) {
  
    log_i("Starting Zigbee subsystem");
    
    esp_coex_wifi_i154_enable();
  
    if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
      log_e("Zigbee failed to start! Rebooting...");
      SuplaDevice.scheduleSoftRestart(1000);
    }
    refresh_time = 0;

    SuplaDevice.handleAction(0, Supla::START_LOCAL_WEB_SERVER); //don't start local web server until Zigbee is ready
  }
  
  //checking status of AC powered devices
  if (millis() - refresh_time > REFRESH_PERIOD) {
    for ([[maybe_unused]]
      const auto &device : zbGateway.getGatewayDevices()) {       
      log_i("Device on endpoint(0x%x), short address(0x%x), model id(0x%x), rejoined(%s)", device->endpoint, device->short_addr, device->model_id,
            device->rejoined ? "YES" : "NO");

      if ((device->rejoined) && (device->model_id == Z2S_DEVICE_DESC_TUYA_HVAC_6567C)) {
        zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x03;
          tuya_dp_data[2] = 0x65; 
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0X01;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data);
          delay(3000);
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x04;
          tuya_dp_data[2] = 0x6C;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x02; 
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data); 
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x05;
          tuya_dp_data[2] = 0x28; 
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x00;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data);
         /* tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x06;
          tuya_dp_data[2] = 0x14; //TUYA_6567C_SCHEDULE_SET_DP;
          tuya_dp_data[3] = 0x01;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x01;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data);
*/
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x07;
          tuya_dp_data[2] = 0x67; //TUYA_6567C_LOCAL_TEMPERATURE_DP;//TUYA_6567C_CURRENT_HEATING_SETPOINT_DP;
          tuya_dp_data[3] = 0x02;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x04;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x01;
          tuya_dp_data[9] = 0x2C; //random(15, 24) * 10;
          
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, tuya_dp_data);
          
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x05;
          tuya_dp_data[2] = 0x1B; //TUYA_6567C_LOCAL_TEMPERATURE_DP;//TUYA_6567C_CURRENT_HEATING_SETPOINT_DP;
          tuya_dp_data[3] = 0x02;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x04;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 0x00;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, tuya_dp_data);
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x08;
          tuya_dp_data[2] = 0x6D; //TUYA_6567C_LOCAL_TEMPERATURE_DP;//TUYA_6567C_CURRENT_HEATING_SETPOINT_DP;
          tuya_dp_data[3] = 0x04;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x00;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 0x00;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data);
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x09;
          tuya_dp_data[2] = 0x6D; //TUYA_6567C_LOCAL_TEMPERATURE_DP;//TUYA_6567C_CURRENT_HEATING_SETPOINT_DP;
          tuya_dp_data[3] = 0x04;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x01;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 50;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data);
          tuya_dp_data[0] = 0x00;
          tuya_dp_data[1] = 0x0A;
          tuya_dp_data[2] = 0x6D; //TUYA_6567C_LOCAL_TEMPERATURE_DP;//TUYA_6567C_CURRENT_HEATING_SETPOINT_DP;
          tuya_dp_data[3] = 0x04;
          tuya_dp_data[4] = 0x00;
          tuya_dp_data[5] = 0x01;
          tuya_dp_data[6] = 0x02;
          tuya_dp_data[7] = 0x00;
          tuya_dp_data[8] = 0x00;
          tuya_dp_data[9] = 100;
          zbGateway.sendCustomClusterCmd(device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, tuya_dp_data);
      }
      if ((device->model_id >= Z2S_DEVICE_DESC_LIGHT_SOURCE) && (device->model_id < Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F)) {//TODO change it to some kind of function
        bool is_online = zbGateway.sendAttributeRead(device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true); 
        int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
        if (channel_number_slot < 0)
          log_i("No channel found for address %s", device->ieee_addr);
        else
        while (channel_number_slot >= 0) {
          auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          if (element) { 
            if (is_online) {
              zbGateway.sendAttributeRead(device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
              element->getChannel()->setOnline();
            }
            else element->getChannel()->setOffline();
          }
          channel_number_slot = Z2S_findChannelNumberNextSlot(channel_number_slot, device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
        }  
      }
    }
    refresh_time = millis();
  }

  if (zbGateway.isNewDeviceJoined()) {

    zbGateway.clearNewDeviceJoined();
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty())
    {
      joined_device = zbGateway.getLastJoinedDevice();
      
      //do some Tuya vodoo - just in case Tuya device is paired
      
      zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL);
      if (!zbGateway.zbQueryDeviceBasicCluster(joined_device))
      {
        log_i("Error while pairing - cann't read manufacturer id. Gateway will restart, try to pair device once again!");
        SuplaDevice.scheduleSoftRestart(0);
      }
      write_mask = 0x13;
      zbGateway.sendAttributeWrite(joined_device, 0x0000, 0xffde, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask); //Tuya black magic continues
      write_mask = 0x1;
      zbGateway.sendAttributeWrite(joined_device, 0xfcc0, 0x0009, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, 1, 0x115f); //Lumi magic

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
                        
                        for (int m = 0; m < Z2S_DEVICES_DESC[k].z2s_device_clusters_count; m++)
                          zbGateway.bindDeviceCluster(joined_device, Z2S_DEVICES_DESC[k].z2s_device_clusters[m]);

                        switch (joined_device->model_id) {
                          case Z2S_DEVICE_DESC_TUYA_SWITCH_4X3: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                           } break; 
                          case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_ROTATE_LEFT_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2:
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_PLAY_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_NEXT_TRACK_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_PREV_TRACK_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID);
                          } break;
                          case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3: {
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID);
                            Z2S_addZ2SDevice(joined_device, IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH: {
                            Z2S_addZ2SDevice(joined_device, TUYA_DOUBLE_DIMMER_SWITCH_1_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_DOUBLE_DIMMER_SWITCH_2_SID);
                          } break;
                          case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_PRESENCE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_MOTION_STATE_SID);
                            Z2S_addZ2SDevice(joined_device, TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID);
                          } break;
                          default: Z2S_addZ2SDevice(joined_device, NO_CUSTOM_CMD_SID); 
                        }
                  }  
                  /*else 
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
                        endpoint_id);*/        
                  }
              }
              //here we can configure reporting and restart ESP32
              //zbGateway.sendDeviceFactoryReset(joined_device);
              switch (joined_device->model_id) {
                case 0x0000: break;      
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1: {
                } break;
                /*case Z2S_DEVICE_DESC_RGBW_LIGHT_SOURCE: {
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID, true))
                    log_i("Color control caps 0x%x, type 0x%x", *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value, zbGateway.getReadAttrLastResult()->data.type);
                    
                } break;*/
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1: {
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 300, 5, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 300, 10, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 0, 300, 5, true);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING,  
                                                ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, ESP_ZB_ZCL_ATTR_TYPE_U48, 0, 300, 1, true);
                  
                  
                } [[fallthrough]];//break;
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, false);
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, false);
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, false);
                  zbGateway.readClusterReportCfgCmd(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, false);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID, true))
                    log_i("AC voltage multiplier 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID, true))
                    log_i("AC divisor 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID, true))
                    log_i("AC current multiplier 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID, true))
                    log_i("AC current divisor 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID, true))
                    log_i("AC power multiplier 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                      ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID, true))
                    log_i("AC power divisor 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                      ESP_ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, true))
                    log_i("Summation formating 0x%x", *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                      ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, true)) {
                    log_i("Metering multiplier 0x%x:0x%x", 
                          ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->low, ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->high);
                  }
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                      ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID, true))
                    log_i("Metering divisor 0x%x:0x%x", 
                          ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->low, ((esp_zb_uint24_t *)zbGateway.getReadAttrLastResult()->data.value)->high);
                  //relay restore mode on startup
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x4003, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); //Tuya special
                    
                }; break;
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: {
                  if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE,ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, true))
                      log_i("IAS_ZONE attribute has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, 
                            *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                  zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG, 0x0021, false);
                  zbGateway.setClusterReporting(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                                ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 900, 1, false);
                } break;
                 case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
                 case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
                 case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {
                    joined_device->endpoint = 0x01;
                    if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,0x8004, true))
                      log_i("Tuya custom attribute 0x8004 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                    write_mask = 0x01;
                    zbGateway.sendAttributeWrite(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 0x8004, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    if (zbGateway.sendAttributeRead(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,0x8004, true))
                      log_i("Tuya custom attribute has 0x8004 been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                 } break;
                 case Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH_1:
                 {
                    /*joined_device->endpoint = 0x01;
                    if (zbGateway.sendAttributeRead(joined_device, 0xE001,0xD020, true))
                      log_i("Tuya custom attribute 0xD020 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                    write_mask = 0x01;
                    zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD020, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    if (zbGateway.sendAttributeRead(joined_device, 0xE001,0xD020, true))
                      log_i("Tuya custom attribute has 0xD020 been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                    joined_device->endpoint = 0x02;
                    if (zbGateway.sendAttributeRead(joined_device, 0xE001,0xD020, true))
                      log_i("Tuya custom attribute 0xD020 has been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                    write_mask = 0x01;
                    zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD020, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                    if (zbGateway.sendAttributeRead(joined_device, 0xE001,0xD020, true))
                      log_i("Tuya custom attribute has 0xD020 been read id 0x%x, value 0x%x", zbGateway.getReadAttrLastResult()->id, *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                    */
                 } break;
                 case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
                 case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR: 
                 case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
                  zbGateway.sendCustomClusterCmd(joined_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, ESP_ZB_ZCL_ATTR_TYPE_SET, 0, NULL); break;

              }
              SuplaDevice.scheduleSoftRestart(5000);
            }   
            //else log_i("LIST checking %s::%s, entry # %d",Z2S_DEVICES_LIST[i].manufacturer_name, Z2S_DEVICES_LIST[i].model_name, i);
          }
      if (!device_recognized) log_d(  "Unknown model %s::%s, no binding is possible", zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                                     zbGateway.getQueryBasicClusterData()->zcl_model_name);
    }
  }
}