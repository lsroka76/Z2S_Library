//#pragma once
#pragma warning(disable: -Wmissing-field-initializers)

#define Z2S_GATEWAY

#include <ESPmDNS.h>
#include <esp_coexist.h>
#include <esp_heap_caps.h>

#include <ZigbeeGateway.h>
#include <Z2S_custom_actions_events.h>

#include <SuplaDevice.h>
#include <supla/network/esp_wifi.h>
#include <supla/device/supla_ca_cert.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/clock/clock.h>
#include <supla/actions.h>
#include <supla/control/button.h>
#include <supla/device/enter_cfg_mode_after_power_cycle.h>
#include <supla/control/virtual_relay.h>
#include <supla/control/roller_shutter_interface.h>
#include <supla/sensor/general_purpose_measurement.h>
#include <supla/control/hvac_base.h>
#include <supla/device/status_led.h>
#include <supla/device/register_device.h>

#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/status_led_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/button_update.h>
#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/network/html/select_input_parameter.h>

#include <Z2S_control/action_handler_with_callbacks.h>
#include <Z2S_control/Z2S_remote_relay.h>

#include "z2s_devices_database.h"
#include "z2s_devices_table.h"
#include "z2s_device_general_purpose_measurement.h"
#include "z2s_version_info.h"
#include "priv_auth_data.h"
#include "z2s_web_gui.h"
#include "web_gui_templates.h"

#ifdef USE_TELNET_CONSOLE

  #include "z2s_telnet_parser.h"

#endif //USE_TELNET_CONSOLE

#define GATEWAY_ENDPOINT_NUMBER 1

#define BUTTON_PIN                  9  //Boot button for C6/H2
#define CFG_BUTTON_PIN              9  //Boot button for C6/H2
#define WIFI_ENABLE                 3
#define WIFI_ANT_CONFIG             14

#define REFRESH_PERIOD              10 * 1000 //miliseconds

#define TIME_CLUSTER_REFRESH_MS     60 * 1000 //miliseconds

/*static char tpc_send_buffer[8] = {0};
static size_t bufferPos = 0;

static uint8_t tcp_connect_state = 0;

static size_t waitingAck = 0;

static AsyncClient client;*/

//static constexpr char *TEST_GATEWAY_IP PROGMEM = "10.164.38.17";
//static constexpr char *TEST_GATEWAY_IP PROGMEM = "10.164.38.210";
//static constexpr char *TEST_GATEWAY_IP PROGMEM = "192.168.1.70";
//static String TEST_GATEWAY_IP PROGMEM = "192.168.1.36";
//static String TEST_GATEWAY_IP PROGMEM = "192.168.1.70";

static constexpr char *Z2S_TCP_CMD PROGMEM = "Z2SCMD";

/*static uint8_t test_device_ieee_address[8] = 
  {0x84, 0x71, 0x27, 0xFF, 0xFE, 0x98, 0x0E, 0x00};

static uint8_t test_device_ieee_address_2[8] = 
  {0x00, 0x0E, 0x98, 0xFE, 0xFF, 0x27, 0x71, 0x84};

static uint8_t test_device_install_code[18] = 
  {0x08, 0x0A, 0x84, 0x0E, 0xD6, 0x64,
   0x22, 0xD1, 0xC8, 0xB2, 0x39, 0x11,
   0x38, 0x2E, 0x1A, 0x74, 0x4F, 0xDB};*/

static NetworkServer TestServer(REMOTE_RELAY_PORT);
//static NetworkServer ThermometerServer(REMOTE_RELAY_PORT);
static NetworkClient TestClient;
static NetworkClient client2;
//static Supla::Control::Z2S_RemoteRelay *testRemoteRelay;
//Supla::Sensor::VirtualThermometer *TEST_VT;

static bool tcp_server_started = false;

ZigbeeGateway zbGateway = ZigbeeGateway(GATEWAY_ENDPOINT_NUMBER);

Supla::Eeprom             eeprom;
Supla::ESPWifi            wifi;
Supla::LittleFsConfig     configSupla(4160);

Supla::EspWebServer *suplaServer;

ZbPairingManager *zpm = nullptr;

constexpr uint8_t LED_PIN   = 8;
constexpr uint8_t NUM_LEDS  = 1;

uint32_t refresh_time       = 0;
uint8_t refresh_cycle       = 0;

uint32_t _init_devices_ms   = 0;

uint32_t _time_cluster_last_refresh_ms = 0;


int8_t  _enable_gui_on_start  = 1;
uint8_t	_force_config_on_start = 0;
uint8_t _rebuild_Supla_channels_on_start = 0;
uint8_t _use_new_at_model = 1;
int32_t _gui_start_delay      = 0;

bool _initial_gui_check = true;

uint8_t _z2s_security_level    = 0;

bool sendIASNotifications = false;
Supla::Control::VirtualRelay *toggleNotifications = nullptr;



bool _restart_scheduled = false;
bool _forced_config = false;

void initGUI(gui_modes_t mode = minimal_gui_mode) {

  log_i("GUIstarted = %s", Z2S_isGUIStarted() ? "YES" : "NO");
      
  if (!Z2S_isGUIStarted()) {
    if (Supla::Network::IsReady()) {
        
      Z2S_buildWebGUI(mode);
      Z2S_startWebGUI();
      Z2S_startUpdateServer();
      onTuyaCustomClusterReceive(GUI_onTuyaCustomClusterReceive);
    }
  }
}

void supla_callback_bridge(int event, int action) {

  log_i("event(0x%x), action(0x%x)", event, action);
  
  switch (action) {
    case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON: {
      
      sendIASNotifications = true; 
      return;
    } break;
    
    case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF: {
      
      sendIASNotifications = false; 
      return;
    } break;


    case Z2S_SUPLA_ACTION_START_GUI_MINIMAL: {

      initGUI();
      return;
    } break;

    case Z2S_SUPLA_ACTION_START_GUI_STANDARD: {

      initGUI(standard_gui_mode);
      return;
    } break;

    case Z2S_SUPLA_ACTION_DEVICE_STATUS_CHANGE: {

      if (_restart_scheduled)
        return;

      int8_t sd_current_status = SuplaDevice.getCurrentStatus();

      if (sd_current_status == STATUS_INITIALIZED) {

        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_SUPLA_INITIALIZED);
        return;
      }

      if (sd_current_status == STATUS_REGISTERED_AND_READY)
        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_SUPLA_REGISTERED_AND_READY);
  
      if ((!Zigbee.started()) && 
          (sd_current_status == STATUS_REGISTERED_AND_READY)) {
  
        log_i("Starting Zigbee subsystem");
    
        esp_coex_wifi_i154_enable();
  
        if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
          
          log_e("Zigbee failed to start! Rebooting...");

          disableGatewayActions();
          _restart_scheduled = true;
          SuplaDevice.scheduleSoftRestart(1000);
        }
        zbGateway.clearLocalBindings();
        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_STARTED);
        
        refresh_time = 0;
        _init_devices_ms = millis();
        
        #ifdef USE_TELNET_CONSOLE

        setupTelnet();
        onTelnetCmd(Z2S_onTelnetCmd); 

        #endif //USE_TELNET_CONSOLE

        TestServer.begin();
        MDNS.begin(GatewayMDNSLocalName);

        return;
      }
      if (sd_current_status == STATUS_CONFIG_MODE) {

        if (!_forced_config) {//if (Zigbee.started()) {
        
          if (Supla::Storage::ConfigInstance()->setUInt8(
                Z2S_FORCE_CONFIG_ON_START, 1)) {
      	  
            log_i("Supla config mode detected - " //(Zigbee stack active) - "
                  "setting Z2S_FORCE_CONFIG_ON_START flag and restarting!");
            
            Supla::Storage::ConfigInstance()->commit();

            disableGatewayActions();
            _restart_scheduled = true;
            SuplaDevice.scheduleSoftRestart(1000);
          } else
            log_e("Supla config mode detected - "//(Zigbee stack active) - "
                "setting Z2S_FORCE_CONFIG_ON_START flag FAILED!");
          return;
        } 
        
        if (suplaServer == nullptr) {

          
        }
      }
    } break;
  }

  switch (event) {


    case Supla::ON_EVENT_1:
    case Supla::ON_CLICK_1: 

      Zigbee.isNetworkOpen() ? Zigbee.openNetwork(0) : Zigbee.openNetwork(180); 
    break;


    case Supla::ON_EVENT_2:
    case Supla::ON_CLICK_5: {

      initGUI();
    } break; 
  }
}


void Z2S_onOpenNetwork(uint8_t permit_duration) {

  if (permit_duration) {

    log_i("Network open for %u s", permit_duration);

    rgbLedWrite(RGB_BUILTIN, 0, 255, 0);
    GUI_onZigbeeOpenNetwork(true);
    handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_OPEN_NETWORK);

    /*if (zpm) {

      zpm->setSrpc(SuplaDevice.getSrpcLayer());

      zpm->notifySrpcAboutParingEnd(
        SUPLA_CALCFG_PAIRINGRESULT_ONGOING, nullptr);
    }*/
  }
  else {
    
    log_i("Network closed");

    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
    GUI_onZigbeeOpenNetwork(false);
    handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK);

    /*if (zpm) {

      zpm->setSrpc(SuplaDevice.getSrpcLayer());
      
      zpm->notifySrpcAboutParingEnd(
        SUPLA_CALCFG_PAIRINGRESULT_NO_NEW_DEVICE_FOUND, nullptr);
    }*/
  }
}

void enableZ2SNotifications() {

  //  Zigbee Gateway notifications

  zbGateway.onOpenNetwork(Z2S_onOpenNetwork);

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onPressureReceive(Z2S_onPressureReceive);
  zbGateway.onPM25Receive(Z2S_onPM25Receive);
  zbGateway.onIlluminanceReceive(Z2S_onIlluminanceReceive);
  zbGateway.onFlowReceive(Z2S_onFlowReceive);
  zbGateway.onOccupancyReceive(Z2S_onOccupancyReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onElectricalMeasurementReceive(Z2S_onElectricalMeasurementReceive);
  zbGateway.onMultistateInputReceive(Z2S_onMultistateInputReceive);
  zbGateway.onAnalogInputReceive(Z2S_onAnalogInputReceive);
  zbGateway.onMeteringReceive(Z2S_onMeteringReceive);
  zbGateway.onBasicReceive(Z2S_onBasicReceive);
  zbGateway.onCurrentLevelReceive(Z2S_onCurrentLevelReceive);
  zbGateway.onColorHueReceive(Z2S_onColorHueReceive);
  zbGateway.onColorSaturationReceive(Z2S_onColorSaturationReceive);
  zbGateway.onThermostatTemperaturesReceive(Z2S_onThermostatTemperaturesReceive);
  zbGateway.onThermostatModesReceive(Z2S_onThermostatModesReceive);
  zbGateway.onWindowCoveringReceive(Z2S_onWindowCoveringReceive);
  zbGateway.onSonoffCustomClusterReceive(Z2S_onSonoffCustomClusterReceive);
  zbGateway.onDevelcoCustomClusterReceive(Z2S_onDevelcoCustomClusterReceive);
  zbGateway.onLumiCustomClusterReceive(Z2S_onLumiCustomClusterReceive);
  zbGateway.onIkeaCustomClusterReceive(Z2S_onIkeaCustomClusterReceive);
  zbGateway.onBatteryReceive(Z2S_onBatteryReceive);
  zbGateway.onCustomCmdReceive(Z2S_onCustomCmdReceive);
  zbGateway.onCmdCustomClusterReceive(Z2S_onCmdCustomClusterReceive);
  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.onDataSaveRequest(Z2S_onDataSaveRequest);
  zbGateway.onDeviceRejoin(Z2S_onDeviceRejoin);
  zbGateway.onDeviceLeave(Z2S_onDeviceLeave);
  zbGateway.onUpdateDeviceLastRssi(Z2S_onUpdateDeviceLastRssi);
}

void disableZ2SNotifications() {

//  switch off Zigbee Gateway notifications

  zbGateway.onTemperatureReceive(nullptr);
  zbGateway.onHumidityReceive(nullptr);
  zbGateway.onPressureReceive(nullptr);
  zbGateway.onPM25Receive(nullptr);
  zbGateway.onIlluminanceReceive(nullptr);
  zbGateway.onFlowReceive(nullptr);
  zbGateway.onOccupancyReceive(nullptr);
  zbGateway.onOnOffReceive(nullptr);
  zbGateway.onElectricalMeasurementReceive(nullptr);
  zbGateway.onMultistateInputReceive(nullptr);
  zbGateway.onAnalogInputReceive(nullptr);
  zbGateway.onMeteringReceive(nullptr);
  zbGateway.onBasicReceive(nullptr);
  zbGateway.onCurrentLevelReceive(nullptr);
  zbGateway.onColorHueReceive(nullptr);
  zbGateway.onColorSaturationReceive(nullptr);
  zbGateway.onThermostatTemperaturesReceive(nullptr);
  zbGateway.onThermostatModesReceive(nullptr);
  zbGateway.onWindowCoveringReceive(nullptr);
  zbGateway.onSonoffCustomClusterReceive(nullptr);
  zbGateway.onDevelcoCustomClusterReceive(nullptr);
  zbGateway.onLumiCustomClusterReceive(nullptr);
  zbGateway.onIkeaCustomClusterReceive(nullptr);
  zbGateway.onBatteryReceive(nullptr);
  zbGateway.onCustomCmdReceive(nullptr);
  zbGateway.onCmdCustomClusterReceive(nullptr);
  zbGateway.onIASzoneStatusChangeNotification(nullptr);
  zbGateway.onBoundDevice(nullptr);
  zbGateway.onBTCBoundDevice(nullptr);
  zbGateway.onDataSaveRequest(nullptr);
  zbGateway.onDeviceRejoin(nullptr);
  zbGateway.onDeviceLeave(nullptr);
  zbGateway.onUpdateDeviceLastRssi(nullptr);
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.print(file.name());
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      
      Serial.printf(
        "LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", 
        (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, 
        tmstruct->tm_mday, tmstruct->tm_hour,
        tmstruct->tm_min, tmstruct->tm_sec);

      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t = file.getLastWrite();
      struct tm *tmstruct = localtime(&t);
      Serial.printf(
        " LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", 
          (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, 
          tmstruct->tm_mday, tmstruct->tm_hour,
        tmstruct->tm_min, tmstruct->tm_sec);
    }
    file = root.openNextFile();
  }
}

Supla::Device::StatusLed statusLed(RGB_BUILTIN, true);

void setup() {
  
  //esp_log_set_vprintf(&spiffs_log_vprintf);

  log_i("setup start");

  printSizeOfClasses();
  uint32_t sanity_check_time_ms = millis();
  uint16_t devices_list_table_size = sizeof(Z2S_DEVICES_LIST)/
    sizeof(Z2S_DEVICES_LIST[0]);
  
  uint8_t match = 0;

  for(uint16_t i = 0; i < devices_list_table_size; i++) {
    match = 0;
    for (uint16_t j = 0; j < devices_list_table_size; j++) {
      
      if ((Z2S_DEVICES_LIST[i].z2s_device_uid > 0) &&
          (Z2S_DEVICES_LIST[i].z2s_device_uid == Z2S_DEVICES_LIST[j].z2s_device_uid))
        match++;
      if (match > 1) {

        log_e("Critical! Duplicate found %s", Z2S_DEVICES_LIST[i].manufacturer_name);
        break;
      }
    }
  }
  sanity_check_time_ms = millis() - sanity_check_time_ms;
  log_i("Sanity check time %lu", sanity_check_time_ms);
  //log_d("Total PSRAM: %d", ESP.getPsramSize());
  //log_d("Free PSRAM: %d", ESP.getFreePsram());

  pinMode(BUTTON_PIN, INPUT);

  pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT); (credits @Zibi_007)in
  pinMode(WIFI_ANT_CONFIG, OUTPUT); // pinMode(14, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH);
 
  eeprom.setStateSavePeriod(5000);

  new Supla::Clock;

  Supla::Storage::Init();

  Supla::Storage::Instance()->enableChannelNumbers();

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

  toggleNotifications = new Supla::Control::VirtualRelay();
  toggleNotifications->getChannel()->setChannelNumber(110);
  toggleNotifications->setInitialCaption("Gateway sensors notifications");
  toggleNotifications->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
  toggleNotifications->setDefaultStateRestore();
  
  auto AHwC = new Supla::ActionHandlerWithCallbacks();
  AHwC->setActionHandlerCallback(supla_callback_bridge);

  toggleNotifications->addAction(
    Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON, AHwC, Supla::ON_TURN_ON, false);
  toggleNotifications->addAction(
    Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF, AHwC, Supla::ON_TURN_OFF, false);

  auto buttonCfg = new Supla::Control::Button(CFG_BUTTON_PIN, true, true);

  buttonCfg->setHoldTime(2000);
  buttonCfg->setMulticlickTime(500);

  buttonCfg->configureAsConfigButton(&SuplaDevice);

   new Supla::Device::EnterCfgModeAfterPowerCycle(10000, 3, true);

  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_1);
  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_5);
  buttonCfg->addAction(Supla::TURN_ON, AHwC, Supla::ON_CLICK_10);

  SuplaDevice.addAction(
    Z2S_SUPLA_ACTION_DEVICE_STATUS_CHANGE, AHwC, 
    Supla::ON_DEVICE_STATUS_CHANGE, false);

  LittleFS.begin(false);
  listDir(LittleFS,"/",3);

  if (LittleFS.exists("/supla/Z2S_devs_table")) {

    log_i("/supla/Z2S_devs_table found - moving to /z2s_gateway/channels_table_v2.z2s");
    LittleFS.mkdir("/z2s_gateway");
    LittleFS.rename("/supla/Z2S_devs_table", "/z2s_gateway/channels_table_v2.z2s");
  }

  if (LittleFS.exists("/supla/Z2S_zbd_table")) {

    log_i("/supla/Z2S_zbd_table found - moving to /z2s_gateway/zb_devices_table_v2.z2s");
    LittleFS.mkdir("/z2s_gateway");
    LittleFS.rename("/supla/Z2S_zbd_table", "/z2s_gateway/zb_devices_table_v2.z2s");
  }

  listDir(LittleFS,"/",3);
  LittleFS.end();

  Z2S_loadZbDevicesTable();

  Z2S_initZbDevices(millis());

  Z2S_loadChannelsTable();

  Z2S_initSuplaChannels();

  if (Supla::Storage::ConfigInstance()->getUInt8(
        Z2S_REBUILD_CHANNELS_ON_START, &_rebuild_Supla_channels_on_start)) {
                                                  
    log_i(
      "Z2S_REBUILD_CHANNELS_ON_START = %d", _rebuild_Supla_channels_on_start);

  } else {

    log_i("Z2S_REBUILD_CHANNELS_ON_START not configured - turning off");
    _rebuild_Supla_channels_on_start = 0;
  }

  if (_rebuild_Supla_channels_on_start) {

    _rebuild_Supla_channels_on_start = 0;

    if (Supla::Storage::ConfigInstance()->setUInt8(
          Z2S_REBUILD_CHANNELS_ON_START, _rebuild_Supla_channels_on_start))
      Supla::Storage::ConfigInstance()->commit();

    log_i("rebuild Supla Channels action triggered!");  

    Z2S_rebuildSuplaChannels();
  }

  setGatewayEventHandler(supla_callback_bridge);
  handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK);
  handleGatewayEvent(Z2S_SUPLA_EVENT_ON_GUI_NOT_STARTED);

  Z2S_initSuplaActions();

  enableZ2SNotifications();

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addGatewayEndpoint(&zbGateway);
  //Zigbee.addEndpoint(&zbGateway); //???

  uint32_t zb_primary_channel_mask;

  if (Supla::Storage::ConfigInstance()->getUInt32(
    Z2S_ZIGBEE_PRIMARY_CHANNEL, &zb_primary_channel_mask)) {

    log_i("Zigbee primary channel mask (0x%x) loaded successfully", 
          zb_primary_channel_mask);

    Zigbee.setPrimaryChannelMask(zb_primary_channel_mask);
  }
  //Disable pairing after boot 1.2.2-15/01/26
  Zigbee.setRebootOpenNetwork(0);

  if (Supla::Storage::ConfigInstance()->getInt8(
    Z2S_ENABLE_GUI_ON_START_V2, &_enable_gui_on_start)) {

    log_i("Z2S_ENABLE_GUI_ON_START_V2 = %i", _enable_gui_on_start);
  } else {

    log_i(
      "Z2S_ENABLE_GUI_ON_START_V2 not configured - setting to minimal_gui");

    _enable_gui_on_start = 1;
    Supla::Storage::ConfigInstance()->eraseKey(Z2S_ENABLE_GUI_ON_START_V2);
    Supla::Storage::ConfigInstance()->commit();
    Supla::Storage::ConfigInstance()->setInt8(Z2S_ENABLE_GUI_ON_START_V2, 
		  _enable_gui_on_start);
		Supla::Storage::ConfigInstance()->commit();
  }
  
  if (Supla::Storage::ConfigInstance()->getUInt8(
    Z2S_FORCE_CONFIG_ON_START, &_force_config_on_start)) {

    log_i("Z2S_FORCE_CONFIG_ON_START = %d", _force_config_on_start);

    if (_force_config_on_start)
      _forced_config = true;
  } else {

    log_i("Z2S_FORCE_CONFIG_ON_START not configured - turning off");

    _force_config_on_start = 0;
  }

  if (Supla::Storage::ConfigInstance()->getInt32(
    Z2S_GUI_ON_START_DELAY_V2, &_gui_start_delay)) {

    log_i("Z2S_GUI_ON_START_DELAY_V2 = %i s", _gui_start_delay);
  } else {

    log_i("Z2S_GUI_ON_START_DELAY_V2 not configured - setting to 10 s");

    _gui_start_delay = 10;
    Supla::Storage::ConfigInstance()->eraseKey(Z2S_GUI_ON_START_DELAY_V2);
    Supla::Storage::ConfigInstance()->commit();
    Supla::Storage::ConfigInstance()->setInt32(Z2S_GUI_ON_START_DELAY_V2, 
		  _gui_start_delay);
		Supla::Storage::ConfigInstance()->commit();
    
  }

  if (Supla::Storage::ConfigInstance()->
      getString(Z2S_GATEWAY_MDNS_LOCAL_NAME, GatewayMDNSLocalName, 12)) {

    log_i("Z2S_GATEWAY_MDNS_LOCAL_NAME = %s", GatewayMDNSLocalName);
  } else {

    log_i("Z2S_GATEWAY_MDNS_LOCAL_NAME not configured - using default");
    //GatewayMDNSLocalName = "Z2S_gateway";
  }

  if (Supla::Storage::ConfigInstance()->getUInt8(
        Z2S_USE_NEW_AT_MODEL, &_use_new_at_model)) {
                                                  
    log_i(
      "Z2S_USE_NEW_AT_MODEL = %d", _use_new_at_model);

  } else {

    log_i("Z2S_USE_NEW_AT_MODEL not configured - turning on");
    _use_new_at_model = 1;
  }
  
  Supla::Storage::ConfigInstance()->getUInt8(
    PSTR("security_level"), &_z2s_security_level);
  
  Supla::Storage::ConfigInstance()->setUInt8(Z2S_FILES_STRUCTURE_VERSION, 2);
  
  auto zbcr = new ZbConflictResolver();
  SuplaDevice.setChannelConflictResolver(zbcr);
  SuplaDevice.addFlags(
    SUPLA_DEVICE_FLAG_BLOCK_ADDING_CHANNELS_AFTER_DELETION);

  auto zdc = new ZbDevicesConfigurator();

  zpm = new ZbPairingManager();

  SuplaDevice.addFlags(SUPLA_DEVICE_FLAG_CALCFG_SUBDEVICE_PAIRING);
  SuplaDevice.setSubdevicePairingHandler(zpm);

  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  
  SuplaDevice.setName("Zigbee <=> Supla Gateway");
  SuplaDevice.setSwVersion(Z2S_VERSION);
  wifi.enableSSL(ENABLE_SSL);

  SuplaDevice.setAutomaticResetOnConnectionProblem(300); //5 minutes
  //SuplaDevice.allowWorkInOfflineMode(2);
  SuplaDevice.setInitialMode(Supla::InitialMode::StartInCfgMode);

  if ((_force_config_on_start) || 
      (Supla::RegisterDevice::isEmailEmpty())) {

    SuplaDevice.setCustomHostnamePrefix("SUPLA-ZIGBEE-GATE");

    suplaServer = new Supla::EspWebServer;
    new Supla::Html::DeviceInfo(&SuplaDevice);
    new Supla::Html::WifiParameters;
    new Supla::Html::ProtocolParameters;
    new Supla::Html::StatusLedParameters;
    new Supla::Html::ButtonUpdate(suplaServer);

    auto selectGUIMode = new Supla::Html::SelectInputParameter(
      Z2S_ENABLE_GUI_ON_START_V2, "SELECT GUI START MODE");

    selectGUIMode->setBaseTypeBitCount(8);  

    for (int8_t modes_counter = no_gui_mode; 
			 modes_counter < gui_modes_number; modes_counter++) {

      selectGUIMode->registerValue(
        GUI_MODE_OPTIONS[modes_counter], modes_counter);
    }

    auto inputGUIDelay = new Supla::Html::CustomParameter(
      Z2S_GUI_ON_START_DELAY_V2, " GUI START DELAY (S)", 30);

  }

  SuplaDevice.begin(28);      
  
  refresh_time = millis();


   /*client.onDisconnect([](void *arg, AsyncClient *client) {
    Serial.printf("Disconnected.\n");
    tcp_connect_state = 0;
  });

  // register a callback when an error occurs
  client.onError([](void *arg, AsyncClient *client, int8_t error) {
    Serial.printf("Error: %s\n", client->errorToString(error));
  });

  // register a callback when data arrives, to accumulate it
  client.onData([](void *arg, AsyncClient *client, void *data, size_t len) {
    Serial.printf("Received %u bytes...\n", len);
    Serial.write((uint8_t *)data, len);
  });

  // register a callback when we are connected
  client.onConnect([](void *arg, AsyncClient *client) {
    Serial.printf("Connected!\n");
    tcp_connect_state = 2;
  });

  client.onAck([](void *arg, AsyncClient *client, size_t len, uint32_t time) {
    Serial.printf("Acked %u bytes in %" PRIu32 " ms\n", len, time);
    assert(waitingAck >= len);
    waitingAck -= len;
  });

  client.setRxTimeout(20000);
  client.setNoDelay(true);
*/
}

zbg_device_params_t *gateway_device;
zbg_device_params_t *joined_device;


void loop() {

  uint8_t  write_mask     = 0x00;
  uint16_t write_mask_16  = 0x0000;

  if (_force_config_on_start == 1) {
    
    _force_config_on_start = 0;
    
    if (Supla::Storage::ConfigInstance()->setUInt8(
      Z2S_FORCE_CONFIG_ON_START, _force_config_on_start))
      Supla::Storage::ConfigInstance()->commit();

    _forced_config = true;
    SuplaDevice.enterConfigMode();
  }

  /*if ((!Z2S_isGUIStarted()) && 
      SuplaDevice.getCurrentStatus() == STATUS_CONFIG_MODE) {
  
    Z2S_startWebGUIConfig();
    Z2S_startUpdateServer();
  }*/ 

  /*if (do_once) {

    if ((SuplaDevice.getCurrentStatus() == STATUS_REGISTERED_AND_READY) &&
        (zpm->getState() == 2)) {

      //do_once = false;

      char unknown_device_name[128];
        sprintf(
          unknown_device_name,"%s::%s", 
          "MANUFACTURER NAME",
          "MODEL NAME");
        zpm->notifySrpcAboutParingEnd(
          SUPLA_CALCFG_PAIRINGRESULT_SUCCESS,
          //SUPLA_CALCFG_PAIRINGRESULT_DEVICE_NOT_SUPPORTED,
          unknown_device_name);

      zbg_device_params_t test_joined_device = {};

      test_joined_device.model_id = Z2S_DEVICE_DESC_TUYA_SWITCH_4X3;

      for (uint8_t i = 1; i < 5; i++) {

        test_joined_device.endpoint = i;

        Z2S_addZ2SDevice(&test_joined_device, 
          TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID);

        Z2S_addZ2SDevice(
          &test_joined_device, TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID);

        Z2S_addZ2SDevice(&test_joined_device, TUYA_CUSTOM_CMD_BUTTON_HELD_SID);
      }
    }
  }*/
  
  if (_initial_gui_check && (!Z2S_isGUIStarted()) && 
      (_enable_gui_on_start != no_gui_mode) && 
      Zigbee.started() && 
      (SuplaDevice.uptime.getUptime() > _gui_start_delay)) {

    _initial_gui_check = false;
    
    Z2S_buildWebGUI((gui_modes_t)_enable_gui_on_start);  
    Z2S_startWebGUI();
    Z2S_startUpdateServer();
    onTuyaCustomClusterReceive(GUI_onTuyaCustomClusterReceive);
  }
 
  SuplaDevice.iterate();

#ifdef USE_TELNET_CONSOLE

  if (is_Telnet_server) telnet.loop();

#endif //USE_TELNET_CONSOLE


client2 = TestServer.available();

if (client2 && client2.connected()) {
     
     String request = client2.readStringUntil('\n');
     Serial.println(request);
     int16_t cmd_pos = request.indexOf(Z2S_TCP_CMD);


     if (cmd_pos >= 0 ) {

      Serial.printf("cmd pos %u\n\r", cmd_pos);
      String helper;
      helper.reserve(64);
      
      helper = request.substring(cmd_pos + 6, cmd_pos + 8);
      //Serial.println(helper);
      uint8_t cmd_id = helper.toInt();
      
      helper = request.substring(cmd_pos + 8, cmd_pos + 11);
      uint8_t cmd_dst_channel = helper.toInt();      
    
      switch (cmd_id) {


        case 0x00:
        case 0x01: { //Relay On/Off

          log_i("\n\rZ2S TCP ON/OFF command received id = %u"
                "\n\rfor channel = %u",
                cmd_id,
                cmd_dst_channel);

          setRemoteRelay(cmd_dst_channel, cmd_id);
        } break;


        case  0x10: { //remote thermometer

          helper = request.substring(cmd_pos + 11, cmd_pos + 14);
          uint32_t cmd_src_channel = helper.toInt();
          
          helper = request.substring(cmd_pos + 14, cmd_pos + 22);
          int32_t cmd_thermometer_value = helper.toInt();

          log_i("\n\rZ2S TCP thermometer command received id = %u"
                "\n\rremote IP = %s"
                "\n\rfrom channel = %u"
                "\n\rto local channel = %u"
                "\n\rwith value = %lu",
                cmd_id,
                client2.remoteIP().toString(),
                cmd_src_channel,
                cmd_dst_channel,
                cmd_thermometer_value);

          updateRemoteThermometer(
            cmd_dst_channel, client2.remoteIP(), cmd_src_channel, 
            cmd_thermometer_value);
        } break;
      }      
      client2.print("OK\n");
     }
     client2.stop();
   }

if (Z2S_isGUIStarted())
      Z2S_loopWebGUI();

  
  if (Zigbee.started() && 
     (millis() - _time_cluster_last_refresh_ms > TIME_CLUSTER_REFRESH_MS)) {

    uint32_t new_utc_time = time(NULL) - 946684800;
    uint32_t new_local_time = time(NULL) - 946684800;
    uint8_t time_status = 0x02;
        
    log_i("New UTC time %lu, new local time %lu", 
          new_utc_time, 
          new_local_time);
    
    esp_zb_lock_acquire(portMAX_DELAY);

    esp_zb_zcl_set_attribute_val(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_TIME_ID, &new_utc_time, false);

    esp_zb_zcl_set_attribute_val(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, &time_status, false);

    esp_zb_zcl_set_attribute_val(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, &new_local_time, false);

    esp_zb_lock_release();
    
    uint32_t utc_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_TIME_ID)->data_p);

    uint32_t local_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID)->data_p);

    uint8_t time_status_attribute = (*(uint8_t *)esp_zb_zcl_get_attribute(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID)->data_p);

    log_i("\n\rLocal Time Cluster Time status attribute %u"
          "\n\rUTC time attribute %lu\n\rlocal time attribute %lu", 
          time_status_attribute, utc_time_attribute, local_time_attribute);

    /*for (uint8_t channels_counter = 0; 
             channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++)
      if (Z2S_getChannelZbDevicePtr(channels_counter))
        log_i(
          "DEVICE: %s, LAST SEEN (S) %lu", 
          Z2S_getChannelZbDevicePtr(channels_counter)->device_local_name,
          Z2S_getChannelZbDevicePtr(channels_counter)->last_seen_ms / 1000);*/

    

    if (Z2S_isGUIStarted())
      Z2S_updateWebGUI();

    _time_cluster_last_refresh_ms = millis();
  }

  if (millis() - refresh_time > REFRESH_PERIOD) {

    
    if (refresh_cycle == 30) {

      log_i(
        "\n\rMemory information:\n\rFlash chip real size: %u B"
        "\n\rFree Sketch Space: %u B\n\rHeapSize: %u B\n\rFree Heap: %u B"
        "\n\rMaxAllocHeap: %u B\n\rMinimal Free Heap: %u B"
				"\n\rSupla uptime: %lu s", ESP.getFlashChipSize(), 
        ESP.getFreeSketchSpace(), ESP.getHeapSize(), ESP.getFreeHeap(), 
        ESP.getMaxAllocHeap(), ESP.getMinFreeHeap(),  
        SuplaDevice.uptime.getUptime());

      printTaskInfo();
    }
    for ([[maybe_unused]]const auto &device : zbGateway.getGatewayDevices()) {       

      if (refresh_cycle % 12 == 0) {//print every 120 seconds - only for debug purposes 
        //uint32_t app_device_id = 0;
        //zbGateway.sendSimpleDescriptorRequestCmd(device->short_addr, 1, &app_device_id);
        //log_i("app_device_id %u", app_device_id);
        /*log_i("Device on endpoint(0x%x), short address(0x%x), model id(0x%x), cluster id(0x%x), rejoined(%s)", 
              device->endpoint, device->short_addr, device->model_id, device->cluster_id, device->rejoined ? "YES" : "NO");
        log_i("Gateway version: %s", Z2S_VERSION);*/
        //int8_t zb_tx_power;
        //esp_zb_get_tx_power(&zb_tx_power);
        //log_i("Zigbee TX power: %d", zb_tx_power);
        //Z2S_updateWebGUI();
      /*  if (Test_GeneralPurposeMeasurement) {
          char display_buffer[15] = {};
          char test_gpm_buf[15];
          sprintf(test_gpm_buf, "%llu", time(NULL));
          //Test_GeneralPurposeMeasurement->setNoSpaceBeforeValue(0, true);
          //Test_GeneralPurposeMeasurement->setNoSpaceAfterValue(0, true);
          //Test_GeneralPurposeMeasurement->setValue(0.2800);
          //Test_GeneralPurposeMeasurement->setUnitBeforeValue("0123456789ABCD", true);
          //sprintf(test_gpm_buf, "%llu", millis());
          //Test_GeneralPurposeMeasurement->setUnitAfterValue("EF0123456789AB", true);*/
          /*display_buffer[0] = random(0,2) + '0';
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 1, 1, display_buffer);
          sprintf(display_buffer, "%02u", 1 + random(0,18));
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 3, 2, display_buffer);
          sprintf(display_buffer, "%u", random(0,3));
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 6, 1, display_buffer);
          sprintf(display_buffer, "%04u", random(0,1801));
          msgZ2SDeviceGeneralPurposeMeasurementDisplay(0, 8, 4, display_buffer);*/
          //sprintf(display_buffer, "A%1uM%02uV%1uD%04u", random(0,2), 1 + random(0,18), random(0,3), random(0,1801));
          //Test_GeneralPurposeMeasurement->setValue(random(0,10));
          //Test_GeneralPurposeMeasurement->setUnitBeforeValue(display_buffer, true);

        //}
      }

    if (refresh_cycle % 12 == 0) {

      //heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
      //heap_caps_dump_all();
  
    }
   
        /*switch (tcp_connect_state) {
          
          case 0: {
            
            Serial.printf("Connecting...\n");
            if (!client.connect("192.168.1.36", 1234)) {
            
              Serial.printf("Failed to connect!\n");
              delay(1000);  // to not flood logs
            } else {
              tcp_connect_state = 1;
            } break;
          }

          case 1: {

            Serial.printf("Still connecting...\n");
            delay(500);  // to not flood logs
          } break;

          case 2: {

            tpc_send_buffer[0] = 'A';
            tpc_send_buffer[1] = 'B';
            tpc_send_buffer[2] = 0;
            client.write(tpc_send_buffer, 3);
          }
        }*/
      //}
      if (refresh_cycle % 3 == 0) {
        //log_i("getZbgDeviceUnitLastSeenMs %d, current millis %d", zbGateway.getZbgDeviceUnitLastSeenMs(device->short_addr), millis());
        /*uint32_t random_temperature = (21 + random(-5, 5)) * 100;
        uint8_t random_src_channel = 1 + random(0,127);
        if (TestClient.connect(MDNS.queryHost("bramka_36"), 1234)) {

          TestClient.printf("Z2SCMD%02u%03u%03u%08ld\n", 
                            0x10, 
                            5,
                            random_src_channel,
                            random_temperature);
    
    
          String response = TestClient.readStringUntil('\n');
          if (response == "OK") 
            log_i("Temperature sent");
        
          TestClient.stop();
        }*/
      }
    //}
    }
    
    if (!zbGateway.getGatewayDevices().empty()) {
      refresh_time = millis();
      refresh_cycle = (refresh_cycle + 1) % 31;
    }
  }

  if ((!zbGateway.getJoinedDevices().empty()) && 
      (zbGateway.isNewDeviceJoined())) {

    disableZ2SNotifications();
    //zbGateway.setActivePairing(true); //TODO - rethink that idea
    
    //zbGateway.clearNewDeviceJoined();
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty())
    {
      joined_device = zbGateway.getLastJoinedDevice();
      
      //rgbLed.setPixelColor(0, rgbLed.Color(0, 128, 128));
      //rgbLed.show();
      rgbLedWrite(RGB_BUILTIN, 0, 0, 255);  // Blue

      Z2S_stopWebGUI();

      if (zpm->getState() == 2)
        zpm->notifySrpcAboutParingEnd(
        SUPLA_CALCFG_PAIRINGRESULT_ONGOING, nullptr);

      bool _basic_cluster_query_success = false;

      if ((strlen(
            zbGateway.getQueryBasicClusterData()->zcl_model_name) == 0) ||
          (strlen(
            zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name) == 0)) {
      
        for (uint8_t query_idx = 0; query_idx < 5; query_idx++) {

          _basic_cluster_query_success =  
            zbGateway.zbQueryDeviceBasicCluster(joined_device);
          if (_basic_cluster_query_success)
            break; 
        }
      }
      else _basic_cluster_query_success = true;

      if (!_basic_cluster_query_success) {
        
          log_i("Error while pairing - cann't read manufacturer id (5x). "
                "Gateway will restart, try to pair device once again!");
          SuplaDevice.scheduleSoftRestart(1000);
        } 
      //write_mask = 0x13;
      //zbGateway.sendAttributeWrite(joined_device, 0x0000, 0xffde, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask); //Tuya black magic continues
      //write_mask = 0x1;
      //zbGateway.sendAttributeWrite(joined_device, 0xfcc0, 0x0009, ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, 1, 0x115F); //Lumi magic

      uint16_t devices_list_table_size = 
        sizeof(Z2S_DEVICES_LIST)/sizeof(Z2S_DEVICES_LIST[0]);

      uint16_t devices_desc_table_size = 
        sizeof(Z2S_DEVICES_DESC)/sizeof(Z2S_DEVICES_DESC[0]);

      bool device_recognized = false;

          for (uint32_t devices_list_counter = 0; 
               devices_list_counter < devices_list_table_size; 
               devices_list_counter++) { 
            
            if ((strcmp(zbGateway.getQueryBasicClusterData()->zcl_model_name, 
                  Z2S_DEVICES_LIST[devices_list_counter].model_name) == 0) &&
                (strcmp(zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name, 
                  Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name) == 0)) {

              log_i("LIST matched %s::%s, entry # %d, endpoints # %d, "
                    "endpoints 0x%x::0x%x,0x%x::0x%x,0x%x::0x%x,0x%x::0x%x",
                    Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name, 
                    Z2S_DEVICES_LIST[devices_list_counter].model_name, 
                    devices_list_counter, 
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints_count,
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[0].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[0].z2s_device_desc_id,
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[1].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[1].z2s_device_desc_id,
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[2].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[2].z2s_device_desc_id,
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[3].endpoint_id, 
                    Z2S_DEVICES_LIST[devices_list_counter].\
                      z2s_device_endpoints[3].z2s_device_desc_id);
  
              for (uint8_t endpoint_counter = 0; 
                   endpoint_counter < Z2S_DEVICES_LIST[devices_list_counter].\
                    z2s_device_endpoints_count; 
                   endpoint_counter++) {
              
                uint8_t endpoint_id = 
                  (Z2S_DEVICES_LIST[devices_list_counter].\
                    z2s_device_endpoints_count == 1) ? 
                  1 : Z2S_DEVICES_LIST[devices_list_counter].\
                    z2s_device_endpoints[endpoint_counter].endpoint_id; 
                                        
                uint32_t z2s_device_desc_id = 
                  (Z2S_DEVICES_LIST[devices_list_counter].\
                    z2s_device_endpoints_count == 1) ?
                  Z2S_DEVICES_LIST[devices_list_counter].\
                    z2s_device_desc_id :
                  Z2S_DEVICES_LIST[devices_list_counter].\
                  z2s_device_endpoints[endpoint_counter].z2s_device_desc_id; 

                for (uint32_t devices_desc_counter = 0; 
                     devices_desc_counter < devices_desc_table_size; 
                     devices_desc_counter++) {

                  if (z2s_device_desc_id == 
                        Z2S_DEVICES_DESC[devices_desc_counter].\
                          z2s_device_desc_id) {
                    log_i("DESC matched 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, endpoint 0x%x ",
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_desc_id,   
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters_count,
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[0],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[1],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[2],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[3],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[4],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[5],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[6],
                          Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters[7],
                          endpoint_id);

                    device_recognized = true;

                    joined_device->endpoint = endpoint_id;
                    joined_device->model_id = 
                      Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_desc_id;
                        
                    if (endpoint_counter == 0) { 

                      if (Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_config_flags & 
                          Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_INIT) {

                        log_i("Tuya magic");

                        uint16_t tuya_init_attributes[6] = { 
                          ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,
                          ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, 
                          ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, 
                          ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,
                          ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, 
                          0xFFFE };

                        zbGateway.sendAttributesRead(
                          joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 6, 
                          tuya_init_attributes);  
                      }

                      if (Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_config_flags & 
                          Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_SETUP) {

                        log_i("Tuya setup");

                        zbGateway.sendCustomClusterCmd(
                          joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 
                          TUYA_SETUP_CMD, ESP_ZB_ZCL_ATTR_TYPE_SET, 
                          0, nullptr, false, ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                          1, 0, 0); //disable default response, no manufacurer code  
                      }

                      if (Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_config_flags & 
                          Z2S_DEVICE_DESC_CONFIG_FLAG_LUMI_INIT) {

                        log_i("LUMI magic");

                        write_mask = 0x01;
                        //joined_device->endpoint = 1;
                        zbGateway.sendAttributeWrite(
                          joined_device, LUMI_CUSTOM_CLUSTER, 
                          LUMI_CUSTOM_CLUSTER_MODE_ID, 
                          ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, true, 1, 
                          LUMI_MANUFACTURER_CODE);
                      }
                    }

                    //before bind

                    switch (z2s_device_desc_id) {

                      case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2: {

                        write_mask_16 = 0x000B;
                            //joined_device->endpoint = 2;
                            /*if (zbGateway.sendAttributeRead(joined_device, 
                                                              ESP_ZB_ZCL_CLUSTER_ID_BASIC, 
                                                              0x0031, 
                                                              true, 
                                                              ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                                                              1, 
                                                              1, 
                                                              PHILIPS_MANUFACTURER_CODE)) {

                              uint16_t philips_0031 = *(uint16_t *)zbGateway.getReadAttrLastResult()->data.value;
                              log_i("Philips basic cluster attribute 0x0031 has been read id 0x%x, value 0x%x", 
                                    zbGateway.getReadAttrLastResult()->id, 
                                    philips_0031);
                              if (philips_0031 != write_mask_16)
                              write_mask_16 = 0x0000;
                            }*/
                        zbGateway.sendAttributeWrite(
                          joined_device, ESP_ZB_ZCL_CLUSTER_ID_BASIC, 0x0031, 
                          ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 2, &write_mask_16, 
                          true, 1, PHILIPS_MANUFACTURER_CODE);
                      } break;
                    }

                    for (int clusters_bind_counter = 0; 
                         clusters_bind_counter < Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_clusters_count; 
                         clusters_bind_counter++) {
                      
                      if (Z2S_DEVICES_DESC[devices_desc_counter].\
                            z2s_device_config_flags &
                          Z2S_DEVICE_DESC_CONFIG_FLAG_RESERVED_5)
                        zbGateway.bindDeviceCluster(
                          joined_device, 
                          Z2S_DEVICES_DESC[devices_desc_counter].\
                            z2s_device_clusters[clusters_bind_counter], 3);  
                      
                      zbGateway.bindDeviceCluster(
                        joined_device, 
                        Z2S_DEVICES_DESC[devices_desc_counter].\
                          z2s_device_clusters[clusters_bind_counter]);
                    }

                    if (endpoint_counter == 0) {//(endpoint_id == 1)
                          
                      uint8_t zb_device_slot = Z2S_addZbDeviceTableSlot(
                        joined_device->ieee_addr,joined_device->short_addr,
                        zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                        zbGateway.getQueryBasicClusterData()->zcl_model_name,
                        Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count,
                        Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id,
                        zbGateway.getQueryBasicClusterData()->zcl_power_source_id);

                      if (zb_device_slot < 0xFF) {

                        Z2S_syncZbDeviceDescFlags(
                          devices_desc_counter, zb_device_slot);
                        Z2S_clearZbDeviceFlags(
                          zb_device_slot, ZBD_USER_DATA_FLAG_BINDING_REQUIRED);
                      }
                    }

                    Z2S_buildSuplaChannels(joined_device, endpoint_counter);
                  }          
                }
              }
              //here we can configure reporting and restart ESP32

              if (hasTuyaCustomCluster(
                    Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id))
                zbGateway.sendCustomClusterCmd(
                  joined_device, TUYA_PRIVATE_CLUSTER_EF00, TUYA_QUERY_CMD, 
                  ESP_ZB_ZCL_ATTR_TYPE_NULL, 0, nullptr);

              switch (Z2S_DEVICES_LIST[devices_list_counter].z2s_device_desc_id) { //(joined_device->model_id) {

                case 0x0000: break;     

                case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR:
                case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: 
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR:
                case Z2S_DEVICE_DESC_TUYA_IAS_ZONE_SENSOR: 
                case Z2S_DEVICE_DESC_TUYA_IAS_ZONE_1_B_SENSOR:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B:
                case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1:
                case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_2:
                case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM:
                case Z2S_DEVICE_DESC_DEVELCO_IAS_ZONE_TEMP_SENSOR:
{
                  esp_zb_ieee_addr_t gateway_ieee_addr;

                  memset(gateway_ieee_addr, 0, sizeof(esp_zb_ieee_addr_t));

                  zbGateway.sendAttributeWrite(joined_device, 
                                              ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                              ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                                              ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, 
                                              sizeof(esp_zb_ieee_addr_t), 
                                              &gateway_ieee_addr);

                  esp_zb_get_long_address(gateway_ieee_addr);

                  zbGateway.sendAttributeWrite(joined_device, 
                                               ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE, 
                                               ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,
                                               ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR, 
                                               sizeof(esp_zb_ieee_addr_t), 
                                               &gateway_ieee_addr);

                } break;
                

                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
                case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL:
		            case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR: {
                
                } break;


                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
                case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
                case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
                case Z2S_DEVICE_DESC_PHILIPS_RGBW_BULB:
                case Z2S_DEVICE_DESC_RGBW_BULB_XY:
                case Z2S_DEVICE_DESC_DIMMER_BULB:
                case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0:
                case Z2S_DEVICE_DESC_IKEA_WW_BULB:
                case Z2S_DEVICE_DESC_IKEA_WS_BULB:
                case Z2S_DEVICE_DESC_PHILIPS_WW_BULB:
                case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY:
                case Z2S_DEVICE_DESC_TUYA_DIMMER_CT_BULB: {

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;


                case  Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER: {

                  joined_device->endpoint = Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[0].endpoint_id; //2
                  
                  bool sync_cmd = true;
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, sync_cmd);

                  zbGateway.setClusterReporting(joined_device, 
                                                ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_ID,
                                                ESP_ZB_ZCL_ATTR_TYPE_U16, 
                                                0, 0, 232, 
                                                sync_cmd);

                   zbGateway.setClusterReporting(joined_device, 
                                                 ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                 ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID,
                                                 ESP_ZB_ZCL_ATTR_TYPE_U16, 
                                                0, 0, 50, 
                                                sync_cmd);

                   zbGateway.setClusterReporting(joined_device, 
                                                 ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                 ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID,
                                                 ESP_ZB_ZCL_ATTR_TYPE_U16, 
                                                 0, 0, 10, 
                                                 sync_cmd);

                   zbGateway.setClusterReporting(joined_device, 
                                                 ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
                                                 ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID,
                                                  ESP_ZB_ZCL_ATTR_TYPE_S16, 
                                                  0, 0, 1, 
                                                  sync_cmd);

                    zbGateway.setClusterReporting(joined_device, 
                                                  ESP_ZB_ZCL_CLUSTER_ID_METERING, 
                                                  ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,
                                                  ESP_ZB_ZCL_ATTR_TYPE_U48, 
                                                  0, 0, 1, 
                                                  sync_cmd);

                } break; //[[fallthrough]];


                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1: 
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A: 
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
                case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
                case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {

                  //relay restore mode on startup
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(joined_device, 
                                               ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                               ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF, 
                                               ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 
                                               1, 
                                               &write_mask);

                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(joined_device, 
                                               ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                               0x8002, 
                                               ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 
                                               1, 
                                               &write_mask); //Tuya special
                }; break;
                

                case Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB20:
                case Z2S_DEVICE_DESC_LIVARNO_DIMMER_SWITCH_FB21: {

                  zbGateway.sendAddGroupRequestCmd(joined_device, 0x4E20, true);
                  zbGateway.sendAddGroupRequestCmd(joined_device, 0x4E20, false);
                } break;

                /*case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3: {

                  zbGateway.sendAddGroupRequestCmd(joined_device, 0, true);
                  zbGateway.sendAddGroupRequestCmd(joined_device, 0, false);
                } break;*/

    
    
                case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
                case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
                case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {
                //case Z2S_DEVICE_DESC_LIVARNO_SWITCH_DIMMER_FB20:
                //case Z2S_DEVICE_DESC_LIVARNO_SWITCH_DIMMER_FB21: {

                  joined_device->endpoint = 0x01;
                  if (zbGateway.sendAttributeRead(joined_device, 
                                                  ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                                                  0x8004, 
                                                  true))
                    log_i("Tuya custom attribute 0x8004 has been read id 0x%x, value 0x%x", 
                          zbGateway.getReadAttrLastResult()->id, 
                          *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);

                  write_mask = 0x01; //Tuya switch mode, 0x00 - scenes
                  zbGateway.sendAttributeWrite(joined_device, 
                                               ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                               0x8004, 
                                               ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 
                                               1, 
                                               &write_mask);

                  if (zbGateway.sendAttributeRead(joined_device,  
                                                  ESP_ZB_ZCL_CLUSTER_ID_ON_OFF,
                                                  0x8004, 
                                                  true))
                    log_i("Tuya custom attribute has 0x8004 been read id 0x%x, value 0x%x", 
                          zbGateway.getReadAttrLastResult()->id, 
                          *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value);
                } break;

                case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1: //Z2S_DEVICE_DESC_TUYA_1GANG_SWITCH:
                case Z2S_DEVICE_DESC_TUYA_2GANG_SWITCH:
                case Z2S_DEVICE_DESC_TUYA_3GANG_SWITCH:
                case Z2S_DEVICE_DESC_TUYA_4GANG_SWITCH: {
                  
                  joined_device->endpoint = 0x01;
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);

                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); 
                    //Tuya special

                  for(int n = 0; 
                      n < Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count; 
                      n++) {

                    joined_device->endpoint = 
                      ( Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints_count == 1) ? 
                        1 : Z2S_DEVICES_LIST[devices_list_counter].z2s_device_endpoints[n].endpoint_id;

                      //zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD010, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); 
                      //zbGateway.sendAttributeWrite(joined_device, 0xE001, 0xD030, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);
                  }
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;

                case Z2S_DEVICE_DESC_RELAY_1:
                case Z2S_DEVICE_DESC_TUYA_RELAY: {
                    
                  joined_device->endpoint = 0x01;
                  write_mask = 0xFF;
                  zbGateway.sendAttributeWrite(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask);

                  write_mask = 0x02;
                  zbGateway.sendAttributeWrite(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    0x8002, ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 1, &write_mask); 
                    //Tuya special

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                    ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_BOOL, 0, 300, 1, false);
                } break;


                case Z2S_DEVICE_DESC_SONOFF_TRVZB: {
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 1200, 10, false);                 
                } break;
                

                case Z2S_DEVICE_DESC_BOSCH_BTHRA: {  //no data from thermostat without reporting
                  
                  uint32_t write_mask_32 = 1200;
                  zbGateway.sendAttributeWrite(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL,
                    0x0000, ESP_ZB_ZCL_ATTR_TYPE_U32, 4, &write_mask_32);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 30, 900, 20, false);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 65000, 1, false);
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 0, 65000, 0, false);
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    BOSCH_HEATING_DEMAND_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 0, 65000, 0, false,
                    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 
                    BOSCH_MANUFACTURER_CODE);               

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    BOSCH_TRV_OPERATING_MODE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 0, 65000, 0, false,
                    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 
                    BOSCH_MANUFACTURER_CODE);
                } break;


                case Z2S_DEVICE_DESC_EUROTRONIC_SPZB0001: {  
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 3600, 10, false);
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 3600, 10, false);
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 0, 3600, 10, false);      
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    EUROTRONIC_HOST_FLAGS_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_U24, 0, 3600, 1, false,
                    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 
                    EUROTRONIC_MANUFACTURER_CODE);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    EUROTRONIC_CURRENT_HEATING_SETPOINT_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 3600, 25, false,
                    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 
                    EUROTRONIC_MANUFACTURER_CODE);         
                } break;


                case Z2s_DEVICE_DESC_LUMI_TRV: {  
                  
                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 3600, 10, false);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_S16, 0, 3600, 10, false);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM, 10, 3600, 0, false);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_16BITMAP, 0, 3600, 0, false);

                  zbGateway.setClusterReporting(
                    joined_device, ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT, 
                    ESP_ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_U8, 10, 3600, 10, false);

                  write_mask = 0;
                  zbGateway.sendAttributeWrite(
                    joined_device, LUMI_CUSTOM_CLUSTER,
                    LUMI_CUSTOM_CLUSTER_TRV_WINDOW_DETECTION_ID,
                    ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &write_mask, false, 1, 
                    LUMI_MANUFACTURER_CODE);
                } break;


                case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH: {

                  
                } break;

                //case Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR:
                case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR:
                case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR_2:
                case Z2S_DEVICE_DESC_LUMI_SWITCH:
                case Z2S_DEVICE_DESC_LUMI_DOUBLE_SWITCH:
                case Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO:
                //case Z2S_DEVICE_DESC_LUMI_TEMPHUMIPRESSURE_SENSOR:
                case Z2S_DEVICE_DESC_LUMI_AIR_QUALITY_SENSOR: {

                  write_mask = 0x01;
                  joined_device->endpoint = 1;
                  zbGateway.sendAttributeWrite(
                    joined_device, LUMI_CUSTOM_CLUSTER, 
                    LUMI_CUSTOM_CLUSTER_MODE_ID, ESP_ZB_ZCL_ATTR_TYPE_U8, 
                    1, &write_mask, true, 1, LUMI_MANUFACTURER_CODE);

                  zbGateway.sendAttributeRead(
                    joined_device, LUMI_CUSTOM_CLUSTER, 0x0148, true, 
                    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 1, 1, 
                    LUMI_MANUFACTURER_CODE);
                } break;
              }

              if (zpm->getState() == 2) {
                
                char found_device_name[128];
              
                sprintf(
                  found_device_name,"%s::%s", 
                  zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                  zbGateway.getQueryBasicClusterData()->zcl_model_name);

                zpm->notifySrpcAboutParingEnd(
                  SUPLA_CALCFG_PAIRINGRESULT_SUCCESS, found_device_name);
              }

              SuplaDevice.scheduleSoftRestart(10000);
              break;
            }   
            /*else log_i("LIST checking %s::%s, entry # %d",
                          Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name, 
                          Z2S_DEVICES_LIST[devices_list_counter].model_name, 
                          devices_list_counter);*/
          }
      if (!device_recognized) {
        log_d(
          "Unknown model %s::%s, no binding is possible", 
          zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
          zbGateway.getQueryBasicClusterData()->zcl_model_name);
        
        rgbLedWrite(RGB_BUILTIN, 255, 0, 0);  // Red
        delay(1000);
        enableZ2SNotifications();
        zbGateway.setActivePairing(false);

        Z2S_startWebGUI();
        GUI_onLastBindingFailure(true);

        if (zpm->getState() == 2) {

          char unknown_device_name[128];

          sprintf(
            unknown_device_name,"%s::%s", 
            zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
            zbGateway.getQueryBasicClusterData()->zcl_model_name);

          zpm->notifySrpcAboutParingEnd(
            SUPLA_CALCFG_PAIRINGRESULT_DEVICE_NOT_SUPPORTED,
            unknown_device_name);
        }
      }
    }
  }
}
