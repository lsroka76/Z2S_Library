//#pragma once
#pragma warning(disable: -Wmissing-field-initializers)

#define Z2S_GATEWAY

#include <cstdlib>
#include <new>
#include "multi_heap.h"

#include <esp_task_wdt.h>
//#include <rtc_wdt.h>

#include <ESPmDNS.h>
#include <esp_coexist.h>
#include <esp_heap_caps.h>

#include <esp_littlefs.h>

//#include <lua.h>
//#include <lualib.h>
//#include <lauxlib.h>

#include <ZigbeeGateway.h>

#include <SuplaDevice.h>
//#include <supla/network/esp_wifi.h>
#include "z2s_esp_wifi.h"
#include <supla/device/supla_ca_cert.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/clock/clock.h>
//#include <supla/actions.h>
#include <supla/control/button.h>
#include <supla/device/enter_cfg_mode_after_power_cycle.h>
#include <supla/control/virtual_relay.h>
#include <supla/control/roller_shutter_interface.h>
#include <supla/sensor/general_purpose_measurement.h>
#include <supla/control/hvac_base.h>
#include <supla/device/status_led.h>
#include <supla/device/register_device.h>
#include <supla/device/auto_update_policy.h>
#include <supla/storage/config.h>

#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/status_led_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/button_update.h>
#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/network/html/select_input_parameter.h>

#include <Z2S_custom_actions_events.h>  
#include <Z2S_control/action_handler_with_callbacks.h>
#include <Z2S_control/Z2S_remote_relay.h>
#include <Z2S_control/Z2S_virtual_relay.h>

#include "z2s_devices_database.h"
#include "z2s_devices_table.h"
#include "z2s_device_general_purpose_measurement.h"
#include "z2s_device_electricity_meter.h"
#include "z2s_device_iaszone.h"
#include "z2s_version_info.h"
#include "priv_auth_data.h"
#include "z2s_web_gui.h"
#include "web_gui_templates.h"
#include "z2s_little_fs.h"
#include "z2s_device_tuya_custom_cluster.h"
#include "task_info.h"

#ifdef USE_TELNET_CONSOLE

  #include "z2s_telnet_parser.h"

#endif //USE_TELNET_CONSOLE

//#define GATEWAY_ENDPOINT_NUMBER 1

#if CONFIG_IDF_TARGET_ESP32S3
  #define BUTTON_PIN                  0  // Boot button for ESP32-S3
  #define CFG_BUTTON_PIN              0  // Boot button for ESP32-S3
#else
  #define BUTTON_PIN                  9  // Boot button for C6/H2
  #define CFG_BUTTON_PIN              9  // Boot button for C6/H2
  #define WIFI_ENABLE                 3
  #define WIFI_ANT_CONFIG             14
#endif

#define REFRESH_PERIOD              30 * 60 * 1000 //miliseconds

#define TIME_CLUSTER_REFRESH_MS     60 * 1000 //miliseconds

#define Z2S_TCP_CMD_SET_REMOTE_RELAY_OFF  0x00
#define Z2S_TCP_CMD_SET_REMOTE_RELAY_ON   0x01

#define Z2S_TCP_CMD_UPDATE_TEMPERATURE    0x10
#define Z2S_TCP_CMD_UPDATE_HUMIDITY       0x11
#define Z2S_TCP_CMD_UPDATE_PRESSURE       0x12

//alignas(8) static uint8_t custom_heap_buffer[64 * 1024]; 
multi_heap_handle_t _z2s_custom_heap = nullptr;
static portMUX_TYPE custom_heap_spinlock = portMUX_INITIALIZER_UNLOCKED;

static constexpr char *Z2S_TCP_CMD PROGMEM = "Z2SCMD";

static NetworkServer TestServer(REMOTE_RELAY_PORT);
static NetworkClient client2;
char   remote_cmd_request[64];

Supla::Eeprom             eeprom;
//Supla::ESPWifi            wifi;
Z2S_ESPWifi                 wifi;
Supla::LittleFsConfig     configSupla(4160);

Supla::EspWebServer *suplaServer;

ZbPairingManager *zpm = nullptr;

constexpr uint8_t LED_PIN = 8;
constexpr uint8_t NUM_LEDS = 1;

uint32_t refresh_time = 0;
uint8_t refresh_cycle = 0;

uint32_t _z2s_iterate_ms = 0;

uint32_t _time_cluster_last_refresh_ms = 0;

int8_t  _enable_gui_on_start  = 1;
uint32_t _gui_build_flags = 0;
int32_t _gui_start_delay = 0;

uint8_t	_force_config_on_start = 0;
uint8_t _rebuild_Supla_channels_on_start = 0;

uint8_t _use_new_at_model = 1;
uint8_t _enable_cloud_ota = 0;

//1.4.81-06/03/26 - 
int32_t _auto_connection_reset_timeout = 0; 

bool _initial_gui_check = true;

uint8_t _z2s_security_level    = 0;

bool sendIASNotifications = false;
Supla::Control::VirtualRelay *toggleNotifications = nullptr;
Supla::Sensor::GeneralPurposeMeasurement *memory_gpm = nullptr;
Supla::Sensor::GeneralPurposeMeasurement *cpu_gpm = nullptr;

bool _restart_scheduled = false;
static bool _forced_config = false;
static bool _start_Zigbee = true;

SemaphoreHandle_t saveMutex;

//lua_State *LuaState = nullptr;

/*inline bool is_in_custom_heap(void* ptr) {
    
    uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t start = reinterpret_cast<uintptr_t>(custom_heap_buffer);
    uintptr_t end = start + sizeof(custom_heap_buffer);
    return (p >= start && p < end);
}*/

/*void* operator new(size_t size) {
   
  void* ptr = nullptr;

  // Try allocating from custom heap if registered
  //if ((size < 256) && (_z2s_custom_heap != nullptr)) {
  if (_z2s_custom_heap != nullptr) {
  
    taskENTER_CRITICAL(&custom_heap_spinlock);
    ptr = multi_heap_malloc(_z2s_custom_heap, size);
    taskEXIT_CRITICAL(&custom_heap_spinlock);
  }

  // Fallback to system heap if custom heap is full or not initialized yet
  if (!ptr) {
    ptr = malloc(size);
  }

  if (!ptr) throw std::bad_alloc();
  
  return ptr;
}

void operator delete(void* ptr) noexcept {
    
  if (!ptr) return;

  // Check which heap owns this memory pointer
  if (is_in_custom_heap(ptr)) {

    taskENTER_CRITICAL(&custom_heap_spinlock);
    multi_heap_free(_z2s_custom_heap, ptr);
    taskEXIT_CRITICAL(&custom_heap_spinlock);
  } 
  else {
    free(ptr); // System heap
  }
}
*/

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

      //if (_restart_scheduled)
      //  return;

      int8_t sd_current_status = SuplaDevice.getCurrentStatus();
      log_i("Supla device status changed to %u", sd_current_status);

      if ((sd_current_status == STATUS_UNKNOWN_SERVER_ADDRESS) || 
          (sd_current_status == STATUS_MISSING_CREDENTIALS)) {

            //_forced_config = true;
            _start_Zigbee = false;
          }

      if (sd_current_status == STATUS_SOFTWARE_RESET) {

        disableZ2SNotifications();

        log_i("software reset - stopping Zigbee stack");
  
        Zigbee.stop();
        delay(500);

        log_i("software reset - closing LittleFs");
        
        _restart_scheduled = true;
        Z2S_endLittleFs(true);
        return;
      }

      if (sd_current_status == STATUS_INITIALIZED) {

        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_SUPLA_INITIALIZED);
        
        if (_start_Zigbee && (!Zigbee.started())) {
        
          log_i("Starting Zigbee subsystem");
    
          esp_coex_wifi_i154_enable();
  
          if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
          
            log_e("Zigbee failed to start! Rebooting...");

            disableGatewayActions();
            _restart_scheduled = true;
            SuplaDevice.scheduleSoftRestart(1000);
          }
          esp_coex_wifi_i154_enable();
          zbGateway.clearLocalBindings();
          handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_STARTED);
        
          if (!Z2S_GatewayPreferences.getBool("c6_synced", false)) {
          
            Z2S_GatewayPreferences.putBool("c6_synced", true);
          
            if (Z2S_Devices.empty() && !Zigbee.isFactoryStack()) {
              log_i("Fresh S3 install with non-factory C6 stack -> factory resetting C6!");
              Zigbee.factoryReset();
            }
          }
        }
        return;
      }
      if (sd_current_status == STATUS_REGISTERED_AND_READY) {

        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_SUPLA_REGISTERED_AND_READY);
      
        refresh_time = 0;
        //_init_devices_ms = millis();
        
        #ifdef USE_TELNET_CONSOLE

        setupTelnet();
        onTelnetCmd(Z2S_onTelnetCmd); 

        #endif //USE_TELNET_CONSOLE

        TestServer.begin();
        MDNS.begin(GatewayMDNSLocalName);
        char actualHostname[MDNS_NAME_BUF_LEN]; 
        mdns_hostname_get(actualHostname);
        log_i("MDNS hostname = %s", actualHostname);

        return;
      }
      if (sd_current_status == STATUS_CONFIG_MODE) {

        if (!_forced_config) {
          Z2S_initLittleFs(); // Ensure LittleFS is mounted before saving config!
          if (Supla::Storage::ConfigInstance()->setUInt8(
                Z2S_FORCE_CONFIG_ON_START, 1)) {
      	  
            log_i("Supla config mode detected - "
                  "setting Z2S_FORCE_CONFIG_ON_START flag and restarting!");
            
            Supla::Storage::ConfigInstance()->commit();
            Z2S_endLittleFs(true);
            delay(200);
            esp_restart();
          }
          return;
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
  }
  else {
    
    log_i("Network closed");

    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
    GUI_onZigbeeOpenNetwork(false);
    handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK);
  }
}

void enableZ2SNotifications() {

  //  Zigbee Gateway notifications

  log_i("enabling Zigbee Gateway notifications");

  zbGateway.onOpenNetwork(Z2S_onOpenNetwork);

  zbGateway.onTemperatureReceive(Z2S_onTemperatureReceive);
  zbGateway.onHumidityReceive(Z2S_onHumidityReceive);
  zbGateway.onPressureReceive(Z2S_onPressureReceive);
  zbGateway.onConcentrationReceive(Z2S_onConcentrationReceive);
  zbGateway.onIlluminanceReceive(Z2S_onIlluminanceReceive);
  zbGateway.onFlowReceive(Z2S_onFlowReceive);
  zbGateway.onOccupancyReceive(Z2S_onOccupancyReceive);
  zbGateway.onOnOffReceive(Z2S_onOnOffReceive);
  zbGateway.onTimeReceive(Z2S_onTimeReceive);
  zbGateway.onElectricalMeasurementReceive(Z2S_onElectricalMeasurementReceive);
  zbGateway.onBinaryInputReceive(Z2S_onBinaryInputReceive);
  zbGateway.onMultistateInputReceive(Z2S_onMultistateInputReceive);
  zbGateway.onAnalogInputReceive(Z2S_onAnalogInputReceive);
  zbGateway.onMeteringReceive(Z2S_onMeteringReceive);
  zbGateway.onBasicReceive(Z2S_onBasicReceive);
  zbGateway.onCurrentLevelReceive(Z2S_onCurrentLevelReceive);
  zbGateway.onColorControlReceive(Z2S_onColorControlReceive);
  zbGateway.onThermostatReceive(Z2S_onThermostatReceive);
  zbGateway.onThermostatUIReceive(Z2S_onThermostatUIReceive);
  zbGateway.onWindowCoveringReceive(Z2S_onWindowCoveringReceive);
  zbGateway.onDoorLockReceive(Z2S_onDoorLockReceive);
  zbGateway.onSonoffCustomClusterReceive(Z2S_onSonoffCustomClusterReceive);
  zbGateway.onFCXXCustomClusterReceive(Z2S_onFCXXCustomClusterReceive);
  zbGateway.onLumiCustomClusterReceive(Z2S_onLumiCustomClusterReceive);
  zbGateway.onIkeaCustomClusterReceive(Z2S_onIkeaCustomClusterReceive);
  zbGateway.onBatteryReceive(Z2S_onBatteryReceive);
  zbGateway.onCustomCmdReceive(Z2S_onCustomCmdReceive);
  zbGateway.onCmdCustomClusterReceive(Z2S_onCmdCustomClusterReceive);
  zbGateway.onIASzoneStatusChangeNotification(Z2S_onIASzoneStatusChangeNotification);

  zbGateway.onBoundDevice(Z2S_onBoundDevice);
  zbGateway.onBTCBoundDevice(Z2S_onBTCBoundDevice);

  zbGateway.onDeviceUpdate(Z2S_onDeviceUpdate);
  zbGateway.onDeviceAnnce(Z2S_onDeviceAnnce);
  zbGateway.onDeviceRejoin(Z2S_onDeviceRejoin);
  zbGateway.onDeviceLeave(Z2S_onDeviceLeave);
  zbGateway.onUpdateDeviceLastRssi(Z2S_onUpdateDeviceLastRssi);
}

void disableZ2SNotifications() {

//  switch off Zigbee Gateway notifications
  log_i("disabling Zigbee Gateway notifications");

  zbGateway.onTemperatureReceive(nullptr);
  zbGateway.onHumidityReceive(nullptr);
  zbGateway.onPressureReceive(nullptr);
  zbGateway.onConcentrationReceive(nullptr);
  zbGateway.onIlluminanceReceive(nullptr);
  zbGateway.onFlowReceive(nullptr);
  zbGateway.onOccupancyReceive(nullptr);
  zbGateway.onOnOffReceive(nullptr);
  zbGateway.onTimeReceive(nullptr);
  zbGateway.onElectricalMeasurementReceive(nullptr);
  zbGateway.onBinaryInputReceive(nullptr);
  zbGateway.onMultistateInputReceive(nullptr);
  zbGateway.onAnalogInputReceive(nullptr);
  zbGateway.onMeteringReceive(nullptr);
  zbGateway.onBasicReceive(nullptr);
  zbGateway.onCurrentLevelReceive(nullptr);
  zbGateway.onColorControlReceive(nullptr);
  zbGateway.onThermostatReceive(nullptr);
  zbGateway.onThermostatUIReceive(nullptr);
  zbGateway.onWindowCoveringReceive(nullptr);
  zbGateway.onDoorLockReceive(nullptr);
  zbGateway.onSonoffCustomClusterReceive(nullptr);
  zbGateway.onFCXXCustomClusterReceive(nullptr);
  zbGateway.onLumiCustomClusterReceive(nullptr);
  zbGateway.onIkeaCustomClusterReceive(nullptr);
  zbGateway.onBatteryReceive(nullptr);
  zbGateway.onCustomCmdReceive(nullptr);
  zbGateway.onCmdCustomClusterReceive(nullptr);
  zbGateway.onIASzoneStatusChangeNotification(nullptr);
  zbGateway.onBoundDevice(nullptr);
  zbGateway.onBTCBoundDevice(nullptr);
  zbGateway.onDeviceUpdate(nullptr);
  zbGateway.onDeviceAnnce(nullptr);
  zbGateway.onDeviceRejoin(nullptr);
  zbGateway.onDeviceLeave(nullptr);
  zbGateway.onUpdateDeviceLastRssi(nullptr);
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {

  log_i("Listing directory: %s", dirname);

  File root = fs.open(dirname);
  if (!root || !root.isDirectory()) {
    log_e("Failed to open directory: %s", dirname);
    if (root) root.close();
    return;
  }

  File file = root.openNextFile();
  while (file) {
    bool isDir = file.isDirectory();
    time_t t = file.getLastWrite();
    struct tm tmstruct = {};
    localtime_r(&t, &tmstruct);

    if (isDir) {
      
      char subPath[64];
      strncpy(subPath, file.path(), sizeof(subPath) - 1);
      subPath[sizeof(subPath) - 1] = '\0';

      log_i(
        "  DIR : %s  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d",
        file.name(), tmstruct.tm_year + 1900, tmstruct.tm_mon + 1, 
        tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);

      file.close();

      if (levels > 0) {
        listDir(fs, subPath, levels - 1);
      }
    } 
    else {

      log_i(
        "  FILE: %-28s SIZE: %6u  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d",
        file.name(), (unsigned)file.size(), tmstruct.tm_year + 1900, 
        tmstruct.tm_mon + 1, tmstruct.tm_mday, tmstruct.tm_hour, 
        tmstruct.tm_min, tmstruct.tm_sec);

      file.close(); 
    }

    file = root.openNextFile();
  }
  root.close(); 
}

Supla::Device::StatusLed statusLed(RGB_BUILTIN, true);

/*****************************************************************************/

static uint32_t parse_uint(const char *s, size_t len) {

  char tmp[16];

  if (len >= sizeof(tmp))
    return 0;

  memcpy(tmp, s, len);
  tmp[len] = '\0';

  return strtoul(tmp, nullptr, 10);
}

/*****************************************************************************/

static int32_t parse_int(const char *s, size_t len) {

  char tmp[16];

  if (len >= sizeof(tmp))
    return 0;

  memcpy(tmp, s, len);
  tmp[len] = '\0';

  return strtol(tmp, nullptr, 10);
}

/*****************************************************************************/

void setup() {

  esp_log_level_set("*", ESP_LOG_INFO);

  log_i("Z2S GATEWAY setup has just started!");

  setupCpuMonitoring();

  saveMutex = xSemaphoreCreateMutex();

  printSizeOfClasses();

  //log_d("Total PSRAM: %d", ESP.getPsramSize());
  //log_d("Free PSRAM: %d", ESP.getFreePsram());

  pinMode(BUTTON_PIN, INPUT);

#if !CONFIG_IDF_TARGET_ESP32S3
  pinMode(WIFI_ENABLE, OUTPUT);
  pinMode(WIFI_ANT_CONFIG, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH);
#endif

#if CONFIG_IDF_TARGET_ESP32S3
  
  esp_vfs_littlefs_conf_t lfs_conf = {
    .base_path              = "/littlefs",
    .partition_label        = "spiffs",
    .format_if_mount_failed = false,
    .read_only              = false,
    .dont_mount             = false,
    .grow_on_mount          = true
  };

  if (esp_vfs_littlefs_register(&lfs_conf) == ESP_OK) {
    
    log_i("LittleFS mounted & grown to S3 partition size successfully");
    esp_vfs_littlefs_unregister(lfs_conf.partition_label);
  }
#endif
 
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

  Z2S_GatewayPreferences.begin("Z2S_GATEWAY", false);
  toggleNotifications = new Supla::Control::VirtualRelay();
  toggleNotifications->getChannel()->setChannelNumber(110);

  toggleNotifications->setInitialCaption("Gateway sensors notifications");
  toggleNotifications->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
  toggleNotifications->setDefaultStateRestore();

  memory_gpm = new Supla::Sensor::GeneralPurposeMeasurement();
  memory_gpm->setInitialCaption("Maximum Allocatable Heap Block");
  memory_gpm->setDefaultUnitAfterValue("B");
  memory_gpm->getChannel()->setChannelNumber(127);

  cpu_gpm = new Supla::Sensor::GeneralPurposeMeasurement();
  cpu_gpm->setInitialCaption("CPU usage");
  cpu_gpm->setDefaultUnitAfterValue("%");
  cpu_gpm->getChannel()->setChannelNumber(126);

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

  Z2S_initLittleFs();
  //listDir(LittleFS,"/",3);

  File verfile = LittleFS.open("/version.dat", "w");

  char ver_buffer[30];
  sprintf(ver_buffer, "SV-%s", Z2S_VERSION);

  verfile.println(ver_buffer);

  verfile.close();

  if (LittleFS.exists("/supla/Z2S_devs_table")) {

    log_i(
      "/supla/Z2S_devs_table found - moving to "
      "/z2s_gateway/channels_table_v2.z2s");

    LittleFS.mkdir("/z2s_gateway");
    LittleFS.rename(
      "/supla/Z2S_devs_table", "/z2s_gateway/channels_table_v2.z2s");
  }

  if (LittleFS.exists("/supla/Z2S_zbd_table")) {

    log_i(
      "/supla/Z2S_zbd_table found - moving to "
      "/z2s_gateway/zb_devices_table_v2.z2s");

    LittleFS.mkdir("/z2s_gateway");
    LittleFS.rename(
      "/supla/Z2S_zbd_table", "/z2s_gateway/zb_devices_table_v2.z2s");
  }

  //listDir(LittleFS,"/",3);
  
  esp_zb_ota_image_header_t esp_zb_ota_image_header;
  
  size_t ota_file_size = Z2S_getFileSize(
    "/zigbee_ota/zigbee_ota_file.ota", false, true);
  
  log_i("OTA file size %lu", ota_file_size);
  
  size_t bytes_read = Z2S_loadBufferFromFile(
    "/zigbee_ota/zigbee_ota_file.ota", 0, sizeof(esp_zb_ota_image_header_t), 
    (uint8_t *)&esp_zb_ota_image_header);

  log_i(
    "\n\rupgrade_file_id = 0x%08X, header_version = 0x%04X, header_length = %u"
    "\n\rfield_control = %u, manufacturer_id = 0x%04X, image_type = 0x%04X\n\r"
    "file_version = 0x%08X, stack_version = %u, image_size = %lu\n\r", 
    esp_zb_ota_image_header.upgrade_file_id, 
    esp_zb_ota_image_header.header_version, 
    esp_zb_ota_image_header.header_length, 
    esp_zb_ota_image_header.field_control, 
    esp_zb_ota_image_header.manufacturer_id, 
    esp_zb_ota_image_header.image_type, 
    esp_zb_ota_image_header.file_version, 
    esp_zb_ota_image_header.stack_version, 
    esp_zb_ota_image_header.image_size);

  if (Supla::Storage::ConfigInstance()->getUInt8(
    Z2S_FORCE_CONFIG_ON_START, &_force_config_on_start)) {

    log_i(
      "Z2S_FORCE_CONFIG_ON_START = %d", _force_config_on_start);

    if (_force_config_on_start)
      _forced_config = true;
  } else {

    log_i("Z2S_FORCE_CONFIG_ON_START not configured - turning off");

    _force_config_on_start = 0;
  }

  if (_forced_config) {

  }
  else {

    Z2S_initWebGUI();

    if (Z2S_loadZbDevicesTable()) {

      Z2S_initZbDevices(millis());

      Z2S_printZbDevicesTableSlots(false);

      if (Z2S_loadChannelsTable()) {

        Z2S_initSuplaChannels();
      
        setGatewayEventHandler(supla_callback_bridge);
        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK);
        handleGatewayEvent(Z2S_SUPLA_EVENT_ON_GUI_NOT_STARTED);

        Z2S_initSuplaActions();
        Z2S_initPushoverMessages();
        irCodesStorage.loadIndexTable();

      }
    }
    else {

      _force_config_on_start = 1;
      _forced_config = true;
    }
  }

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

    //Z2S_rebuildSuplaChannels();
  }

  enableZ2SNotifications();

  zbGateway.setManufacturerAndModel("Supla", "Z2SGateway");
  zbGateway.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbGateway); 

  uint32_t zb_primary_channel_mask;

  if (Supla::Storage::ConfigInstance()->getUInt32(
    Z2S_ZIGBEE_PRIMARY_CHANNEL, &zb_primary_channel_mask)) {

    log_i("Zigbee primary channel mask (0x%08X) loaded successfully", 
          zb_primary_channel_mask);

    Zigbee.setPrimaryChannelMask(zb_primary_channel_mask);
  }
  //Disable pairing after boot 1.2.2-15/01/26
  Zigbee.setRebootOpenNetwork(0);


  if (Supla::Storage::ConfigInstance()->getInt8(
    Z2S_ENABLE_GUI_ON_START_V2, &_enable_gui_on_start)) {

    log_i("Z2S_ENABLE_GUI_ON_START_V2 = %i", _enable_gui_on_start);
  } 
  else {

    log_i(
      "Z2S_ENABLE_GUI_ON_START_V2 not configured - setting to minimal_gui");

    _enable_gui_on_start = 1;
    Supla::Storage::ConfigInstance()->eraseKey(Z2S_ENABLE_GUI_ON_START_V2);
    Supla::Storage::ConfigInstance()->commit();
    Supla::Storage::ConfigInstance()->setInt8(Z2S_ENABLE_GUI_ON_START_V2, 
		  _enable_gui_on_start);
		Supla::Storage::ConfigInstance()->commit();
  }

  if (Supla::Storage::ConfigInstance()->getUInt32(
    Z2S_GUI_BUILD_FLAGS_V2, &_gui_build_flags)) {

    log_i("Z2S_GUI_BUILD_FLAGS_V2 = 0x%04X", _gui_build_flags);
  }
  if (_gui_build_flags == 0) {

    Supla::Storage::ConfigInstance()->eraseKey(Z2S_GUI_BUILD_FLAGS_V2);
    Supla::Storage::ConfigInstance()->commit();

    _gui_build_flags = Z2S_getGUIBuildControlFlags(
      (gui_modes_t)_enable_gui_on_start);
    if (_gui_build_flags == 0)
      _gui_build_flags = 1;

    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_GUI_BUILD_FLAGS_V2, 
		      _gui_build_flags)) {

      Supla::Storage::ConfigInstance()->commit();

      log_i(
      "Z2S_GUI_BUILD_FLAGS_V2 not configured - updating to 0x%04X from "
      "Z2S_ENABLE_GUI_ON_START_V2 0x%u", _gui_build_flags, 
      _enable_gui_on_start);
    }
    else
      log_e("Error updating Z2S_GUI_BUILD_FLAGS_V2");
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

  if (Supla::Storage::ConfigInstance()->getInt32(
    Z2S_AUTO_CONNECTION_RESET_TIMEOUT, &_auto_connection_reset_timeout)) {

    log_i(
      "Z2S_AUTO_CONNECTION_RESET_TIMEOUT = %i s", 
      _auto_connection_reset_timeout);
  } else {

    log_i(
      "Z2S_AUTO_CONNECTION_RESET_TIMEOUT not configured - switching OFF");

    _auto_connection_reset_timeout = 0;
    Supla::Storage::ConfigInstance()->setInt32(
      Z2S_AUTO_CONNECTION_RESET_TIMEOUT, _auto_connection_reset_timeout);
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

  } 
  else {

    log_i("Z2S_USE_NEW_AT_MODEL not configured - turning on");
    _use_new_at_model = 1;
  }

  if (Supla::Storage::ConfigInstance()->getUInt8(
        Z2S_ENABLE_CLOUD_OTA, &_enable_cloud_ota)) {
    
    log_i("Z2S_ENABLE_CLOUD_OTA = %d", _enable_cloud_ota);
  } 
  else {
    
    log_i("Z2S_ENABLE_CLOUD_OTA not configured - turning off");
    _enable_cloud_ota = 0;
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

  SuplaDevice.setAutomaticResetOnConnectionProblem(
    _auto_connection_reset_timeout); 
    
  //SuplaDevice.allowWorkInOfflineMode(2);
  SuplaDevice.setInitialMode(Supla::InitialMode::StartInCfgMode);

  if (_force_config_on_start) {

    log_i("starting Supla WebServer");
    _start_Zigbee = false;

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

  if (isHardwareCapableOfOta() && _enable_cloud_ota) {
    
    SuplaDevice.setAutomaticFirmwareUpdateSupported(true);
  }

  SuplaDevice.begin(28);      

  if (cfg && cfg->getAutoUpdatePolicy() != 
          Supla::AutoUpdatePolicy::Disabled) {
    
    cfg->setAutoUpdatePolicy(Supla::AutoUpdatePolicy::Disabled);
    cfg->commit();
  }

  
  refresh_time = millis();

  /*zbg_device_params_t test_joined_device = {};

  test_joined_device.model_id = Z2S_DEVICE_DESC_IAS_ZONE_SENSOR;
  esp_zb_ieee_addr_t ieee_addr_test = {0x00,0x15,0x8D,0x00,0x02,0x5F,0x1E,0xC5};

  const char *manuf = "eWeLink"; //"AwoX"; //"Paulmann Licht GmbH";
  const char *model = "DS01"; //"TLSR82xx"; //"CCT-I";
  test_joined_device.zb_device_id = Z2S_addZbDeviceTableSlot(
    ieee_addr_test, 33600, manuf, model, 1, 
    Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_5F_WXKG12LM, 0);

    test_joined_device.endpoint = 1;
    test_joined_device.model_id = Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_5F_WXKG12LM;
    
    Z2S_buildSuplaChannels(&test_joined_device, 1);*/

    /*test_joined_device.endpoint = 38;
    test_joined_device.model_id = Z2S_DEVICE_DESC_TEMPERATURE_SENSOR;
    
    Z2S_addZ2SDevice(&test_joined_device);*/

#if !CONFIG_IDF_TARGET_ESP32S3
  disableCore0WDT();
  if (Zigbee.getZigbeeTaskHandle() != NULL) {
    esp_task_wdt_delete(Zigbee.getZigbeeTaskHandle());
  }
#endif
  disableLoopWDT();

  /*LuaState = luaL_newstate();
  //luaL_openlibs(LuaState);
  lua_pushcfunction(LuaState, luaopen_base); lua_call(LuaState, 0, 0); 
  lua_pushcfunction(LuaState, luaopen_math); lua_call(LuaState, 0, 0); 
  lua_pushcfunction(LuaState, luaopen_string); lua_call(LuaState, 0, 0); 
  lua_pushcfunction(LuaState, luaopen_table); lua_call(LuaState, 0, 0);*/
}

zbg_device_params_t *gateway_device;
zbg_device_params_t *joined_device;

bool once_test = true;

size_t getArduinoLoopTaskStackSize(void) {

  return 8192;
}

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

  
  if (_initial_gui_check && (!Z2S_isGUIStarted()) && 
      ((_enable_gui_on_start != no_gui_mode) || _gui_build_flags) && 
      Zigbee.started() && 
      (SuplaDevice.uptime.getUptime() > _gui_start_delay)) {

    _initial_gui_check = false;
    
    Z2S_buildWebGUI((gui_modes_t)_enable_gui_on_start, _gui_build_flags);  
    Z2S_startWebGUI();
    Z2S_startUpdateServer();
    onTuyaCustomClusterReceive(GUI_onTuyaCustomClusterReceive);
  }
 
  SuplaDevice.iterate();
  
  vTaskDelay(pdMS_TO_TICKS(10));

  if (Zigbee.started() && (z2s_millis() - _z2s_iterate_ms > 5000))
    _z2s_iterate_ms = Z2S_iterateSuplaChannels(_z2s_iterate_ms);

#ifdef USE_TELNET_CONSOLE

  if (is_Telnet_server) telnet.loop();

#endif //USE_TELNET_CONSOLE


  client2 = TestServer.available();

  if (client2.connected()) {
     
    size_t request_length = client2.readBytesUntil(
      '\n', remote_cmd_request, sizeof(remote_cmd_request) - 1);

    remote_cmd_request[request_length] = '\0';

    char *cmd = strstr(remote_cmd_request, Z2S_TCP_CMD);

    if (cmd == nullptr) {

      client2.stop();
    }
    else {

      IPAddress ip = client2.remoteIP();

      client2.print("OK\n");
      client2.stop();

      const char *p = cmd + strlen(Z2S_TCP_CMD);

      uint8_t cmd_id  = parse_uint(p + 0, 2);
      uint8_t cmd_dst_channel = parse_uint(p + 2, 3);  
    
      switch (cmd_id) {


        case Z2S_TCP_CMD_SET_REMOTE_RELAY_OFF:
        case Z2S_TCP_CMD_SET_REMOTE_RELAY_ON: { //Relay On/Off

          log_i(
            "\n\rZ2S TCP ON/OFF command received id = %u\n\rfor channel = %u",
            cmd_id, cmd_dst_channel);

          setRemoteRelay(cmd_dst_channel, cmd_id);
        } break;


        case Z2S_TCP_CMD_UPDATE_TEMPERATURE:
        case Z2S_TCP_CMD_UPDATE_HUMIDITY:
        case Z2S_TCP_CMD_UPDATE_PRESSURE: {

          uint32_t src_channel = parse_uint(p + 5, 3);
          int32_t cmd_thermometer_value = parse_int (p + 8, 8);

          auto value_type = cmd_id - Z2S_TCP_CMD_UPDATE_TEMPERATURE + 1;

          log_i(
            "Z2S TCP sensor command id=%u src=%u dst=%u value=%ld", cmd_id,
            src_channel, cmd_dst_channel, cmd_thermometer_value);

          Z2S_Core::updateRemoteThermometer(
            cmd_dst_channel, ip, src_channel, value_type, 
            cmd_thermometer_value);
        } break;
      }   
    }
  }

  if (Z2S_isGUIStarted())
    Z2S_loopWebGUI();

  if (Zigbee.started() && 
     (millis() - _time_cluster_last_refresh_ms > TIME_CLUSTER_REFRESH_MS)) {

    uint32_t new_utc_time = time(NULL) - 946684800;
    uint32_t new_local_time = time(NULL) - 946684800;
    uint8_t time_status = 0x02;
        
    log_i(
      "New UTC time %lu, new local time %lu", new_utc_time, new_local_time);
    
    //log_i("now attempting to check heap integrity #1");
    //heap_caps_check_integrity_all(true);

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
    
    uint32_t utc_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_TIME_ID)->data_p);

    uint32_t local_time_attribute = (*(uint32_t *)esp_zb_zcl_get_attribute(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID)->data_p);

    uint8_t time_status_attribute = (*(uint8_t *)esp_zb_zcl_get_attribute(
      1, ESP_ZB_ZCL_CLUSTER_ID_TIME, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, 
      ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID)->data_p);

    esp_zb_lock_release();

    log_i(
      "\n\rLocal Time Cluster Time status attribute %u"
      "\n\rUTC time attribute %lu\n\rlocal time attribute %lu", 
      time_status_attribute, utc_time_attribute, local_time_attribute);

    for (Z2S_Device *device : Z2S_Devices) {

      if (device && (device->checkDeviceUserDataFlags(
            ZBD_USER_DATA_FLAG_TUYA_FORCE_TIME_SYNC))) {

        esp_zb_lock_acquire(portMAX_DELAY);

        processTuyaCustomCluster(
          1, device->getDeviceShortAddress(), 1, TUYA_MCU_SYNC_TIME, 0, 
          nullptr);

        esp_zb_lock_release();
      }
    }

    if (Z2S_isGUIStarted())
      Z2S_updateWebGUI();

    memory_gpm->setValue(ESP.getMaxAllocHeap());
    cpu_gpm->setValue(GatewayTaskInfo.getCpuUsage());

    _time_cluster_last_refresh_ms = millis();

    
  }

  /*if (LuaState) {

    if (luaL_dostring(LuaState, "print('Hello from Lua')") != LUA_OK) {
        
      Serial.println(lua_tostring(LuaState, -1));
      lua_pop(LuaState, 1);
    }
    lua_gc(LuaState, LUA_GCSTEP, 1);
  }*/

  if (millis() - refresh_time > REFRESH_PERIOD) {

    for (Z2S_Device *device : Z2S_Devices) {
      if (device && 
          (device->getDeviceDescId() == 
            Z2S_DEVICE_DESC_TUYA_SMART_POOL_SENSOR)) {

        zbg_device_params_t dev_params = {};

        dev_params.endpoint = 1;
        dev_params.short_addr = device->getDeviceShortAddress();

        sendTuyaRequestCmdValue32(
          &zbGateway, &dev_params, TUYA_SMART_POOL_SENSOR_BACKLIGHT_DP, 1, 
          true);

        delay(2000); //== vTaskDelay

        sendTuyaRequestCmdValue32(
          &zbGateway, &dev_params, TUYA_SMART_POOL_SENSOR_BACKLIGHT_DP, 0, 
          true);
      }
    }
    refresh_time = millis();
  }

  //bool _basic_cluster_query_success = false;

  /*if (once_test) {

    once_test = false;
    goto test_label;
  }*/

  if ((!zbGateway.getJoinedDevices().empty()) && 
      (zbGateway.isNewDeviceJoined())) {

    //disableZ2SNotifications();
    
    zbGateway.printJoinedDevices();

    while (!zbGateway.getJoinedDevices().empty()) {

      joined_device = zbGateway.getLastJoinedDevice();

      rgbLedWrite(RGB_BUILTIN, 0, 0, 255);  // Blue

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

        log_e(
          "Error while pairing - can't read manufacturer id (5x). "
          "Try to pair device once again!");

        free(joined_device);
        joined_device = nullptr;
        zbGateway.clearNewDeviceJoined();
        //SuplaDevice.scheduleSoftRestart(1000);
        break;
      }
     
      log_i("sizeof(Z2S_DEVICES_LIST) = %u", sizeof(Z2S_DEVICES_LIST));

      bool device_recognized = false;
      bool restart_required  = true;

      int32_t devices_list_idx = Z2S_findDeviceListIdxByManufacturerAndModel(
        zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
        zbGateway.getQueryBasicClusterData()->zcl_model_name);

      if (devices_list_idx >= 0) {
        const auto &z2s_device_list = Z2S_DEVICES_LIST[devices_list_idx];

        log_i(
          "LIST matched %s::%s, entry # %ld, endpoints # %u",
          z2s_device_list.manufacturer_name, z2s_device_list.model_name,
          devices_list_idx, z2s_device_list.z2s_device_endpoints_count);

        uint8_t first_endpoint_id = 1;
        bool first_valid_ep_found = false;
        joined_device->zb_device_id = 0xFF;

        for (uint8_t endpoint_counter = 0;
             endpoint_counter < z2s_device_list.z2s_device_endpoints_count;
             endpoint_counter++) {

          z2s_resolved_endpoint_t resolved_ep = {};
          if (!Z2S_resolveDeviceEndpointByIndex(
                devices_list_idx, endpoint_counter, resolved_ep)) {
            continue;
          }

          if (resolved_ep.desc_id == Z2S_DEVICE_DESC_NULL) {
            continue;
          }

          if (!first_valid_ep_found) {
            first_endpoint_id = resolved_ep.endpoint_id;
            first_valid_ep_found = true;
          }

          log_i(
            "Processing endpoint #%u: ep_id=0x%02X, desc_id=0x%04lX",
            endpoint_counter, resolved_ep.endpoint_id, resolved_ep.desc_id);

          const z2s_device_desc_t *z2s_device_desc =
            Z2S_getDeviceDescById(resolved_ep.desc_id);

          if (z2s_device_desc != nullptr) {
            log_i(
              "DESC matched 0x%x, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, "
              "endpoint 0x%x",
              z2s_device_desc->z2s_device_desc_id,
              z2s_device_desc->z2s_device_clusters_count,
              z2s_device_desc->z2s_device_clusters[0],
              z2s_device_desc->z2s_device_clusters[1],
              z2s_device_desc->z2s_device_clusters[2],
              z2s_device_desc->z2s_device_clusters[3],
              z2s_device_desc->z2s_device_clusters[4],
              resolved_ep.endpoint_id);

            device_recognized = true;

            joined_device->endpoint = resolved_ep.endpoint_id;
            joined_device->model_id = z2s_device_desc->z2s_device_desc_id;

            Z2S_initDeviceEndpointBeforeBind(
              joined_device, z2s_device_list, *z2s_device_desc,
              endpoint_counter);

            Z2S_bindDeviceEndpointClusters(joined_device, z2s_device_desc);

            if (joined_device->zb_device_id == 0xFF) {
              restart_required = false;

              Z2S_Device *z2s_device = Z2S_addZbDeviceTableSlot(
                joined_device->ieee_addr, joined_device->short_addr,
                zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
                zbGateway.getQueryBasicClusterData()->zcl_model_name,
                z2s_device_list.z2s_device_endpoints_count,
                z2s_device_list.z2s_device_desc_id,
                zbGateway.getQueryBasicClusterData()->zcl_power_source_id);

              if (z2s_device) {
                joined_device->zb_device_id = z2s_device->getDeviceIndex();
              } else {
                log_e(
                  "Failed to allocate Z2S_Device slot - aborting "
                  "channel creation!");

                joined_device->zb_device_id = 0xFF;
                device_recognized = false;
                break;
              }
            }

            if (joined_device->zb_device_id != 0xFF) {
              Z2S_buildSuplaChannels(joined_device, endpoint_counter);
            }
          }

          Z2S_configureEndpointStandardReporting(joined_device, resolved_ep);
        } // endpoint_counter loop

        if (device_recognized && joined_device->zb_device_id != 0xFF) {
          Z2S_configureDeviceSpecificReporting(
            joined_device, z2s_device_list.z2s_device_desc_id,
            first_endpoint_id);

          if (zpm->getState() == 2) {
            char found_device_name[128];
            snprintf(
              found_device_name, sizeof(found_device_name), "%s::%s",
              zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
              zbGateway.getQueryBasicClusterData()->zcl_model_name);

            zpm->notifySrpcAboutParingEnd(
              SUPLA_CALCFG_PAIRINGRESULT_SUCCESS, found_device_name);
          }

          if (restart_required) {
            if (joined_device != nullptr) {
              free(joined_device);
              joined_device = nullptr;
            }
            SuplaDevice.scheduleSoftRestart(10000);
          } else {
            if (Zigbee.isNetworkOpen())
              rgbLedWrite(RGB_BUILTIN, 0, 255, 0);
            else
              rgbLedWrite(RGB_BUILTIN, 0, 0, 0);

            zbGateway.clearQueryBasicClusterData();
            zbGateway.clearNewDeviceJoined();

            if (joined_device != nullptr) {
              free(joined_device);
              joined_device = nullptr;
            }

            if (Z2S_isGUIStarted()) {
              sortZbDevicesAndChannelsSelectors();
            }

            Supla::Network::DisconnectProtocols();
          }
        }
      }
      if (!device_recognized) {

        log_d(
          "Unknown model %s::%s, no binding is possible", 
          zbGateway.getQueryBasicClusterData()->zcl_manufacturer_name,
          zbGateway.getQueryBasicClusterData()->zcl_model_name);
        
        Z2S_printZbDevicesTableSlots();

        Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByIeeeAddress(
          joined_device->ieee_addr);

        if (z2s_device) {
          
          log_i(
            "Removing incomplete ZigBee device#%u", 
            z2s_device->getDeviceIndex());

          Z2S_removeZbDevice(z2s_device->getDeviceIndex());
        
          Z2S_printZbDevicesTableSlots();
        }

        if (Zigbee.started())
          zbGateway.sendDeviceLeaveRequest(
            joined_device->ieee_addr, joined_device->short_addr, false, false);

        rgbLedWrite(RGB_BUILTIN, 255, 0, 0);  // Red
        delay(1000);
        //enableZ2SNotifications();
                
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
        zbGateway.clearQueryBasicClusterData();
        zbGateway.clearNewDeviceJoined();

        if (joined_device != nullptr) {
          free(joined_device);
          joined_device = nullptr;
        }
      }
    }
  }
}
