#pragma once

#include <ZigbeeGateway.h>

#include "esp_coexist.h"

#include <SuplaDevice.h>
#include <priv_auth_data.h>

#include <supla/sensor/virtual_binary.h>
#include <supla/network/esp_wifi.h>
#include <supla/storage/eeprom.h>
#include "FS.h"
#include <LittleFS.h>
#include <supla/storage/littlefs_config.h>
#include <supla/sensor/virtual_therm_hygro_meter.h>
#include <Z2S_virtual_relay.h>
#include <supla/device/supla_ca_cert.h>

extern "C" {
#include "esp_littlefs.h"
}

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2

typedef struct z2s_device_params_s {

  bool valid_record;
  uint32_t model_id;
  esp_zb_ieee_addr_t ieee_addr;
  uint8_t endpoint;
  uint16_t cluster_id;
  uint16_t short_addr;
  uint8_t Supla_channel;
  int32_t Supla_channel_type;
} z2s_device_params_t;

static z2s_device_params_t z2s_devices_table[SUPLA_CHANNELMAXCOUNT];


ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi(SUPLA_WIFI_SSID, SUPLA_WIFI_SSID);
Supla::LittleFsConfig     configSupla;

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

const static char   Z2S_DEVICES_COUNT []  PROGMEM = "Z2S_devs_cnt";
const static char   Z2S_OBJECT_NAME [] PROGMEM = "Z2S_object_%d";
const static char   Z2S_DEVICES_TABLE []  PROGMEM = "Z2S_devs_table";
const static char   Z2S_DEVICES_TABLE_SIZE []  PROGMEM = "Z2S_devs_ts";

uint8_t Z2S_getDevicesCount() {
  uint8_t _Z2S_devices_count;
  if (Supla::Storage::ConfigInstance()->getUInt8(Z2S_DEVICES_COUNT, &_Z2S_devices_count))
    return _Z2S_devices_count;
  else
    return 0;
}

uint32_t Z2S_getDevicesTableSize() {
  uint32_t _z2s_devices_table_size;
  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_DEVICES_TABLE_SIZE, &_z2s_devices_table_size))
    return _z2s_devices_table_size;
  else
    return 0;
}

uint8_t Z2S_findFirstFreeDevicesTableSlot() {

  for (uint8_t devices_counter = 0; devices_counter < SUPLA_CHANNELMAXCOUNT; devices_counter++) 
      if (!z2s_devices_table[devices_counter].valid_record)
        return devices_counter;
  return 0xFF;
  
}

uint8_t Z2S_printDevicesTableSlots() {

  for (uint8_t devices_counter = 0; devices_counter < SUPLA_CHANNELMAXCOUNT; devices_counter++) 
      log_i("valid %d, ieee addr %s, model_id 0x%x, endpoint 0x%x, cluster 0x%x, channel %d, channel type %d",
        z2s_devices_table[devices_counter].valid_record,
        z2s_devices_table[devices_counter].ieee_addr,
        z2s_devices_table[devices_counter].model_id,
        z2s_devices_table[devices_counter].endpoint,
        z2s_devices_table[devices_counter].cluster_id,
        z2s_devices_table[devices_counter].Supla_channel,
        z2s_devices_table[devices_counter].Supla_channel_type);
  return 0xFF;
  
}


int16_t Z2S_findChannelNumberSlot(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster) {

  log_i("Z2S_findChannelNumber %d:%d:%d:%d:%d:%d:%d:%d, endopint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  
  for (uint8_t devices_counter = 0; devices_counter < SUPLA_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record)
        if ((memcmp(z2s_devices_table[devices_counter].ieee_addr, ieee_addr,8) == 0) && (z2s_devices_table[devices_counter].endpoint == endpoint)) { 
        //&& (z2s_devices_table[devices_counter].cluster_id = cluster)) {
            return devices_counter;
        }

  }  
  return -1;
}

bool Z2S_loadDevicesTable() {

  uint32_t z2s_devices_table_size =  Z2S_getDevicesTableSize(); //3584

  if (z2s_devices_table_size == 0) {

      log_i(" No devices table found, writing empty one with size %d", sizeof(z2s_devices_table));
      
      memset(z2s_devices_table,0,sizeof(z2s_devices_table));
      
      if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
        log_i ("Devices table write failed!");
        return false;
      }
      else { 
        if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_DEVICES_TABLE_SIZE, sizeof(z2s_devices_table))) {
          Supla::Storage::ConfigInstance()->commit();
          return true;
        }
        else { 
          log_i ("Devices table size write failed!");
          return false;
        }
      }
  } else
  {
    if (z2s_devices_table_size != sizeof(z2s_devices_table)) {
      
      log_i("Devices table size mismatch %d <> %d", z2s_devices_table_size, sizeof(z2s_devices_table));
      return false;
    }
    else {
        if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
          log_i ("Devices table load failed!");
          return false;
        } else
          return true;
    }
  }
}

bool Z2S_saveDevicesTable() {

  if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
    log_i ("Devices table write failed!");
    return false;
  }
  else { 
    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_DEVICES_TABLE_SIZE, sizeof(z2s_devices_table)))
      return true;
    else { 
      log_i ("Devices table size write failed!");
      return false;
    }
  }
  Supla::Storage::ConfigInstance()->commit();
}

int16_t Z2S_findChannelNumber(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster) {

  log_i("Z2S_findChannelNumber %d:%d:%d:%d:%d:%d:%d:%d, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  for (uint8_t devices_counter = 0; devices_counter < SUPLA_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record)
        if ((memcmp(z2s_devices_table[devices_counter].ieee_addr, ieee_addr,8) == 0) && (z2s_devices_table[devices_counter].endpoint == endpoint)) {
        //&& (z2s_devices_table[devices_counter].cluster_id = cluster)) {
            return devices_counter;
        }

  }  
  return -1;
}

void Z2S_initSuplaChannels(){

  for (uint8_t devices_counter = 0; devices_counter < SUPLA_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record) 
        switch (z2s_devices_table[devices_counter].Supla_channel_type) {
          case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {
            auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();
            Supla_VirtualThermHygroMeter->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
            break;
          }
          case SUPLA_CHANNELTYPE_BINARYSENSOR: {
            auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
            Supla_VirtualBinary->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
            break;
          }
          case SUPLA_CHANNELTYPE_RELAY: {
            auto Supla_Z2S_Virtual_Relay = new Supla::Control::Z2S_VirtualRelay(&zbGateway,z2s_devices_table[devices_counter].ieee_addr );
            Supla_Z2S_Virtual_Relay->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
            break;
          }
          default: {
            log_i("Can't create channel for %d channel type", z2s_devices_table[devices_counter].Supla_channel_type);
            break;
          }
          
        }
  }  
}

void Z2S_onTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float temperature) {

  log_i("onTemperatureReceive %d:%d:%d:%d:%d:%d:%d:%d, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {

        auto VirtualTHM = reinterpret_cast<Supla::Sensor::VirtualThermHygroMeter *>(element);
        VirtualTHM->setTemp(temperature);
    }
  }
}

void Z2S_onHumidityReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float humidity) {

  log_i("onHumidityReceive %d:%d:%d:%d:%d:%d:%d:%d, endopint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {

        auto VirtualTHM = reinterpret_cast<Supla::Sensor::VirtualThermHygroMeter *>(element);
        VirtualTHM->setHumi(humidity);
    }
  }
}

void Z2S_onOnOffReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, bool state) {

  log_i("onOnOffReceive %d:%d:%d:%d:%d:%d:%d:%d, endopint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {

        auto VirtualOnOff = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
        VirtualOnOff->Z2S_setOnOff(state); 
    }
  }
}

void Z2S_onIASzoneStatusChangeNotification(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int iaszone_status) {
  
int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

        auto Supla_VirtualBinary = reinterpret_cast<Supla::Sensor::VirtualBinary *>(element);
        if (iaszone_status == 0) Supla_VirtualBinary->set();
        else Supla_VirtualBinary->clear();
    }
  }
}


void Z2S_onBTCBoundDevice(zb_device_params_t *device) {

  log_i("BTC bound device 0x%x on endpoint 0x%x cluster id 0x%x", device->short_addr, device->endpoint, device->cluster_id );
  if (device->cluster_id == ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT) {
    log_i("Trying to wake up device 0x%x on endpoint 0x%x cluster id 0x%x", device->short_addr, device->endpoint, device->cluster_id );
    zbGateway.setClusterReporting(device->short_addr, device->endpoint, device->cluster_id, 
                                  ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID, ESP_ZB_ZCL_ATTR_TYPE_S16, 30, 120, 10);
  } else
  if (device->cluster_id == ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT) {
    log_i("Trying to wake up device 0x%x on endpoint 0x%x cluster id 0x%x", device->short_addr, device->endpoint, device->cluster_id );
    zbGateway.setClusterReporting(device->short_addr, device->endpoint, device->cluster_id, 
                                  ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID, ESP_ZB_ZCL_ATTR_TYPE_U16, 30, 120, 10);
  } else
  if (device->cluster_id == ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG) {
    log_i("Trying to wake up device 0x%x on endpoint 0x%x cluster id 0x%x", device->short_addr, device->endpoint, device->cluster_id );
    zbGateway.setClusterReporting(device->short_addr, device->endpoint, device->cluster_id, 
                                  0x0021, ESP_ZB_ZCL_ATTR_TYPE_U8, 30, 120, 10);
  }
  Z2S_onBoundDevice(device, true);
}

void Z2S_onBoundDevice(zb_device_params_t *device, bool last_cluster) {
  
  
  Z2S_printDevicesTableSlots();

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id);
  

  if (channel_number_slot < 0) {
    log_i("No channel found for address %s, adding new one", device->ieee_addr);
    
    uint8_t first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
    if (first_free_slot == 0xFF) {
        log_i("Devices table full");
        return;
    }
    log_i("model id %d, first free slot %d", device->model_id, first_free_slot);
    
    switch (device->model_id) {
      case 0x0000: break;
      case 0x1000: {
          auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();
          z2s_devices_table[first_free_slot].valid_record = true;
          memcpy(z2s_devices_table[first_free_slot].ieee_addr,device->ieee_addr,8);
          z2s_devices_table[first_free_slot].model_id = device->model_id;
          z2s_devices_table[first_free_slot].endpoint = device->endpoint;
          z2s_devices_table[first_free_slot].cluster_id = device->cluster_id;
          z2s_devices_table[first_free_slot].Supla_channel = Supla_VirtualThermHygroMeter->getChannelNumber();
          z2s_devices_table[first_free_slot].Supla_channel_type = SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR; 
          Z2S_saveDevicesTable();
          Z2S_printDevicesTableSlots();
      } break;
      case 0x2000: {
          auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
          z2s_devices_table[first_free_slot].valid_record = true;
          memcpy(z2s_devices_table[first_free_slot].ieee_addr,device->ieee_addr,8);
          z2s_devices_table[first_free_slot].model_id = device->model_id;
          z2s_devices_table[first_free_slot].endpoint = device->endpoint;
          z2s_devices_table[first_free_slot].cluster_id = device->cluster_id;
          z2s_devices_table[first_free_slot].Supla_channel = Supla_VirtualBinary->getChannelNumber();
          z2s_devices_table[first_free_slot].Supla_channel_type = SUPLA_CHANNELTYPE_BINARYSENSOR; 
          Z2S_saveDevicesTable();
          Z2S_printDevicesTableSlots();
      } break;
      case 0x4000: {
          auto Supla_Z2S_Virtual_Relay = new Supla::Control::Z2S_VirtualRelay(&zbGateway,device->ieee_addr);
          z2s_devices_table[first_free_slot].valid_record = true;
          memcpy(z2s_devices_table[first_free_slot].ieee_addr,device->ieee_addr,8);
          z2s_devices_table[first_free_slot].model_id = device->model_id;
          z2s_devices_table[first_free_slot].endpoint = device->endpoint;
          z2s_devices_table[first_free_slot].cluster_id = device->cluster_id;
          z2s_devices_table[first_free_slot].Supla_channel = Supla_Z2S_Virtual_Relay->getChannelNumber();
          z2s_devices_table[first_free_slot].Supla_channel_type = SUPLA_CHANNELTYPE_RELAY; 
          Z2S_saveDevicesTable();
          Z2S_printDevicesTableSlots();
          break;
      }
    }
  }
  else
  {
    switch (device->model_id) {
      case 0x0000: break;
      case 0x1000: {
          auto Supla_channel = Supla::Channel::GetByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          if (!Supla_channel) {
            auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();
            Supla_VirtualThermHygroMeter->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          }
      } break;
    case 0x2000: {
          auto Supla_channel = Supla::Channel::GetByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          if (!Supla_channel) {
            auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
            Supla_VirtualBinary->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          }
      } break;
    case 0x4000: {
          auto Supla_channel = Supla::Channel::GetByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          if (!Supla_channel) {
            auto Supla_Z2S_Virtual_Relay = new Supla::Control::Z2S_VirtualRelay(&zbGateway, z2s_devices_table[channel_number_slot].ieee_addr);
            Supla_Z2S_Virtual_Relay->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
          break;
      }
    }
  }
}

  //Z2S_findChannelNumber

  
}

void setup() {

  
  
  
  pinMode(BUTTON_PIN, INPUT);

  eeprom.setStateSavePeriod(5000);

  LittleFS.end();

  Supla::Storage::Init();

  auto cfg = Supla::Storage::ConfigInstance();

  cfg->commit();

  cfg->setGUID(GUID);
  cfg->setAuthKey(AUTHKEY);
  cfg->setWiFiSSID(SUPLA_WIFI_SSID);
  cfg->setWiFiPassword(SUPLA_WIFI_PASS);
  cfg->setSuplaServer(SUPLA_SVR);
  cfg->setEmail(SUPLA_EMAIL);

  Z2S_loadDevicesTable();

  Z2S_initSuplaChannels();

  //  Zigbee

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway);

  //Open network for 180 seconds after boot
  Zigbee.setRebootOpenNetwork(180);

  //Supla
  
  //SuplaDevice.setStatusFuncImpl(&status_func);

  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  
  SuplaDevice.setName("Zigbee to Supla");
  //wifi.enableSSL(true);

  SuplaDevice.begin(GUID,              // Global Unique Identifier 
                    SUPLA_SVR,  // SUPLA server address
                    SUPLA_EMAIL,   // Email address used to login to Supla Cloud
                    AUTHKEY);          // Authorization key
  
  startTime = millis();
  printTime = millis();
  zbInit_delay = millis();
}

zb_device_params_t *gateway_device;
zb_device_params_t *joined_device;

char zbd_model_name[64];

uint16_t gateway_devices_list_size = 0;
size_t total, used;
esp_err_t esp_error;

void loop() {
  
  SuplaDevice.iterate();

  if (millis() - printTime) {

    printTime = millis();
    //log_i("Little FS total %d, used %d",LittleFS.totalBytes(), LittleFS.usedBytes());
    
    esp_error = esp_littlefs_info("spiffs", &total, &used);

    //log_i("ESP Little FS total %d, used %d, error code %d",total, used, esp_error);
  }
  if (zbInit && wifi.isReady())
  //if (zbInit)
  {
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
      
      log_i("manufacturer %s ", zbGateway.readManufacturer(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      strcpy(zbd_model_name,zbGateway.readModel(joined_device->endpoint, joined_device->short_addr, joined_device->ieee_addr));
      log_i("model %s ", zbd_model_name);
      zbGateway.setClusters2Bind(1);

      if (strcmp(zbd_model_name,"TS0201") == 0) {
          esp_zb_lock_acquire(portMAX_DELAY);
          joined_device->model_id = 0x1000; // Tuya TS0201 temperature&humidity sensor
          zbGateway.setClusters2Bind(4);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT);
          esp_zb_lock_release();
      } else 
      if ((strcmp(zbd_model_name,"TS0203") == 0)||
          (strcmp(zbd_model_name,"TS0202") == 0)||
          (strcmp(zbd_model_name,"TS0205") == 0)) {
          esp_zb_lock_acquire(portMAX_DELAY);
          joined_device->model_id = 0x2000; // Tuya IAS sensor
          zbGateway.setClusters2Bind(2);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
          esp_zb_lock_release();
          //zbGateway.setIASZReporting(joined_device->short_addr, joined_device->endpoint, 10, 20);
      } else 
          if (strcmp(zbd_model_name,"TS0044") == 0) {
          esp_zb_lock_acquire(portMAX_DELAY);
          zbGateway.bindDeviceCluster(joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF);
          esp_zb_lock_release();
      } else
          if (strcmp(zbd_model_name,"TS011F") == 0) {
          esp_zb_lock_acquire(portMAX_DELAY);
          joined_device->model_id = 0x4000; // Tuya wall socket
          zbGateway.setClusters2Bind(3);
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