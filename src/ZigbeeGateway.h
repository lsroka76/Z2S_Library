#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
//#if SOC_IEEE802154_SUPPORTED 
//&& CONFIG_ZB_ENABLED
#if CONFIG_ZB_ENABLED

#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"

#define ZB_DEFAULT_GATEWAY_CONFIG()                                                               \
  {                                                                                                  \
    .basic_cfg =                                                                                     \
      {                                                                                              \
        .zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE,                                   \
        .power_source = ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE,                                 \
      },                                                                                             \
    .identify_cfg =                                                                                  \
      {                                                                                              \
        .identify_time = ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE,                            \
      }                                                                                        \
  }

#define ZBG_MAX_DEVICES 0x20 //32

#define TUYA_PRIVATE_CLUSTER_0                                0xE000
#define TUYA_PRIVATE_CLUSTER_1                                0xE001
#define TUYA_PRIVATE_CLUSTER_EF00                             0xEF00

#define TUYA_QUERY_CMD                                        0x03 //TUYA_PRIVATE_CLUSTER_EF00
#define TUYA_SETUP_CMD                                        0xF0 //ESP_ZB_ZCL_CLUSTER_ID_BASIC 

#define IKEA_PRIVATE_CLUSTER                                  0xFC7F
#define IKEA_PRIVATE_CLUSTER_2                                0xFC80

#define SONOFF_CUSTOM_CLUSTER                                 0xFC11
/*TRVZB attributes*/
#define SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID                   0x0000 //BOOL
#define SONOFF_CUSTOM_CLUSTER_OPEN_WINDOW_ID                  0x6000 //BOOL
#define SONOFF_CUSTOM_CLUSTER_FROST_PROTECTION_TEMPERATURE_ID 0x6002 //S16
#define SONOFF_CUSTOM_CLUSTER_IDLE_STEPS_ID                   0x6003 //U16
#define SONOFF_CUSTOM_CLUSTER_CLOSING_STEPS_ID                0x6004 //U16
#define SONOFF_CUSTOM_CLUSTER_VALVE_OPENING_LIMIT_VOLTAGE     0x6005 //U16
#define SONOFF_CUSTOM_CLUSTER_VALVE_CLOSING_LIMIT_VOLTAGE     0x6006 //U16
#define SONOFF_CUSTOM_CLUSTER_VALVE_MOTOR_RUNNING_VOLTAGE     0x6007 //U16
#define SONOFF_CUSTOM_CLUSTER_VALVE_OPENING_DEGREE            0x600B //U8
#define SONOFF_CUSTOM_CLUSTER_VALVE_CLOSING_DEGREE            0x600C //U8
#define SONOFF_CUSTOM_CLUSTER_EXTERNAL_TEMPERATURE_INPUT      0x600D //INT16
#define SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT       0x600E //U8
/*SNZB-04P attributes*/
#define SONOFF_CUSTOM_CLUSTER_TAMPER_ID                       0x2000 //U8
/*SNZB-03P attributes*/
#define SONOFF_CUSTOM_CLUSTER_ILLUMINATION_ID                 0x2001 //U8
/*SWV attributes*/
#define SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID        0x5008
#define SONOFF_CUSTOM_CLUSTER_VOLUME_IRRIGATION_CYCLE_ID      0x5009
#define SONOFF_CUSTOM_CLUSTER_IRRIGATION_CYCLE_MODE_ID        0x5010
/*EM attributes*/
#define SONOFF_CUSTOM_CLUSTER_AC_CURRENT_ID                   0x7004 //UINT32
#define SONOFF_CUSTOM_CLUSTER_AC_VOLTAGE_ID                   0x7005 //UINT32
#define SONOFF_CUSTOM_CLUSTER_AC_POWER_ID                     0x7006 //UINT32
#define SONOFF_CUSTOM_CLUSTER_AC_ENERGY_TODAY_ID              0x7009 //UINT32
#define SONOFF_CUSTOM_CLUSTER_AC_ENERGY_MONTH_ID              0x700A //UINT32
#define SONOFF_CUSTOM_CLUSTER_AC_ENERGY_YESTERDAY_ID          0x700B //UINT32
#define SONOFF_CUSTOM_CLUSTER_OUTLET_CONTROL_PROTECT_ID       0x7007 //U8
/*ZBMINIR2*/
#define SONOFF_CUSTOM_CLUSTER_TURBO_MODE_ID                   0x0012 //S16
/*SNZB-02LD, SNZB-02WD, SNZB-02P*/
#define SONOFF_CUSTOM_CLUSTER_TEMPERATURE_UNITS_ID            0x0007 //U16 //0 Celcius 1 Fahrenheit
#define SONOFF_CUSTOM_CLUSTER_TEMPERATURE_CALIBRATION_ID      0x2003 //S16 -50 +50 0.1 Celcius
#define SONOFF_CUSTOM_CLUSTER_HUMIDITY_CALIBRATION_ID         0x2004 //S16 -50 +50 0.1 %

#define ZOSUNG_IR_CONTROL_CUSTOM_CLUSTER                      0xE004
#define ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER                     0xED00

#define ADEO_CUSTOM_CLUSTER                                   0xFE00    

#define PHILIPS_CUSTOM_CLUSTER                                0xFC00
#define PHILIPS_MANUFACTURER_CODE                             0x100B

#define LUMI_CUSTOM_CLUSTER                                   0xFCC0
#define LUMI_MANUFACTURER_CODE                                0x115F

#define LUMI_CUSTOM_CLUSTER_MODE_ID                           0x0009 //U8
#define LUMI_CUSTOM_CLUSTER_ILLUMINANCE_ID                    0x0112 //U32
#define LUMI_CUSTOM_CLUSTER_DISPLAY_UNIT_ID                   0x0114 //U8
#define LUMI_CUSTOM_CLUSTER_AIR_QUALITY_ID                    0x0129 //U8

#define LUMI_CUSTOM_CLUSTER_MODE_ID                           0x0009 //U8

#define LUMI_ATTRIBUTE_BATTERY_ID                             0x01 //ESP_ZB_ZCL_ATTR_TYPE_U16
#define LUMI_ATTRIBUTE_TEMPERATURE_ID                         0x64 //ESP_ZB_ZCL_ATTR_TYPE_S16
#define LUMI_ATTRIBUTE_HUMIDITY_ID                            0x65 //ESP_ZB_ZCL_ATTR_TYPE_U16
#define LUMI_ATTRIBUTE_PRESSURE_ID                            0x66 //ESP_ZB_ZCL_ATTR_TYPE_S32
#define LUMI_ATTRIBUTE_VOC_ID                                 0x66 //ESP_ZB_ZCL_ATTR_TYPE_U16
#define LUMI_ATTRIBUTE_AIR_QUALITY_ID                         0x67 //ESP_ZB_ZCL_ATTR_TYPE_U8

#define DEVELCO_CUSTOM_CLUSTER                                0xFC03
#define DEVELCO_MANUFACTURER_CODE                             0x1015

#define DEVELCO_CUSTOM_CLUSTER_MEASURED_VALUE_ID              0x0000
#define DEVELCO_CUSTOM_CLUSTER_MIN_MEASURED_VALUE_ID          0x0001
#define DEVELCO_CUSTOM_CLUSTER_MAX_MEASURED_VALUE_ID          0x0002
#define DEVELCO_CUSTOM_CLUSTER_RESOLUTION_ID                  0x0003

#define ZCL_CMD_TSN_UNKNOWN 0x00
#define ZCL_CMD_TSN_SYNC    0x01
#define ZCL_CMD_TSN_ASYNC   0x02


#define ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID 0x0021
#define ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID              0x0020

typedef struct findcb_userdata_s {
  uint8_t   _endpoint;
  uint16_t  _cluster_id;
} findcb_userdata_t;

typedef struct query_basic_cluster_data_s {
  char zcl_manufacturer_name[32];
  uint8_t zcl_version_id;
  uint8_t zcl_application_version_id;
  uint8_t zcl_power_source_id;
  char zcl_model_name[32];
  char software_build_ID[16];
} query_basic_cluster_data_t;

typedef struct zbg_device_params_s {
  uint32_t model_id;
  bool rejoined;
  bool ZC_binding;
  bool rejoin_after_leave;
  esp_zb_ieee_addr_t ieee_addr;
  uint8_t endpoint;
  uint16_t cluster_id;
  uint16_t short_addr;
  uint32_t user_data;
} zbg_device_params_t;

typedef struct zbg_device_unit_s {
  uint8_t record_id;
  esp_zb_ieee_addr_t ieee_addr;
  uint16_t short_addr;
  uint32_t model_id;
  uint32_t last_seen_ms;
  uint32_t keep_alive_ms;
  uint32_t timeout_ms;
  int8_t last_rssi;
} zbg_device_unit_t;

class ZigbeeGateway : public ZigbeeEP {
public:
  ZigbeeGateway(uint8_t endpoint);
  ~ZigbeeGateway() {};

  std::list<zbg_device_params_t *> getGatewayDevices() const {
    return _gateway_devices;
  }
  void addGatewayDevice(zbg_device_params_t * device){ 
    if (device) _gateway_devices.push_back(device);
  }
  std::list<zbg_device_params_t *> getJoinedDevices() const {
    return _joined_devices;
  }
  zbg_device_params_t * getLastJoinedDevice() { 
    zbg_device_params_t *last_joined_device = NULL;
    if (!_joined_devices.empty()) {
      last_joined_device = _joined_devices.back();
      _joined_devices.pop_back(); 
    }
    return last_joined_device;
  }
  void printGatewayDevices();
  void printJoinedDevices();
  static bool isNewDeviceJoined() {
    return _new_device_joined;
  }
  static void clearNewDeviceJoined() {
    _new_device_joined = false;
  }
  /*static void setEndpoints2Bind(uint8_t endpoints_count) {
    _endpoints_2_bind = endpoints_count;
  }
  static void setClusters2Bind(uint16_t clusters_count) {
    _clusters_2_bind = clusters_count;
  }*/

  static void setActivePairing(bool active_pairing) {

    _active_pairing = active_pairing;
  }

  static bool getActivePairing() {

    return _active_pairing;
  }

  static query_basic_cluster_data_t * getQueryBasicClusterData() {
    return &_last_device_query;
  }
  static esp_zb_zcl_attribute_t * getReadAttrLastResult() {
    return &_read_attr_last_result;
  }
  static esp_zb_zcl_read_report_config_resp_variable_t * getReportConfigRespVariableLastResult() {
    return &_read_report_config_resp_variable_last_result;
  }

  static esp_zb_zcl_status_t * getReadAttrStatusLastResult() {
    return &_read_attr_status_last_result;
  }

  static esp_zb_zcl_status_t * getConfigReportStatusLastResult() {
    return &_config_report_status_last_result;
  }

  static esp_zb_zcl_status_t * getWriteAttrStatusLastResult() {
    return &_write_attr_status_last_result;
  }

  static esp_zb_zcl_status_t * getCustomCmdStatusLastResult() {
    return &_custom_cmd_status_last_result;
  }

  static uint16_t * getWriteAttrAttributeIdLastResult() {
    return &_write_attr_attribute_id_last_result;
  }

  static uint8_t * getCustomCmdRespToCmdLastResult() {
    return &_custom_cmd_resp_to_cmd_last_result;
  }

  void zbPrintDeviceDiscovery (zbg_device_params_t * device);

  static void bindDeviceCluster(zbg_device_params_t *,
                                int16_t cluster_id, 
                                uint8_t groupcast_flag = 0);

  static uint32_t getZbgDeviceUnitLastSeenMs(uint16_t short_addr);
  static int8_t getZbgDeviceUnitLastRssi(uint16_t short_addr);

  bool zbQueryDeviceBasicCluster(zbg_device_params_t * device, 
                                 bool single_attribute = false, 
                                 uint16_t attribute_id = 0x0);

  //void zbReadBasicCluster(const esp_zb_zcl_attribute_t *attribute) override;
  void zbReadBasicCluster(esp_zb_zcl_addr_t src_address, 
                          uint16_t src_endpoint, 
                          uint16_t cluster_id, 
                          esp_zb_zcl_attribute_t *attribute) override;

  bool setClusterReporting(zbg_device_params_t * device, 
                           uint16_t cluster_id, 
                           uint16_t attribute_id, 
                           uint8_t attribute_type,
                           uint16_t min_interval, 
                           uint16_t max_interval, 
                           uint16_t delta, bool ack);

  void readClusterReportCmd(zbg_device_params_t * device, 
                            uint16_t cluster_id, 
                            uint16_t attribute_id, 
                            bool ack);

  bool readClusterReportCfgCmd(zbg_device_params_t * device, 
                               uint16_t cluster_id, 
                               uint16_t attribute_id, 
                               bool ack);

  bool sendAttributeRead(zbg_device_params_t * device, 
                         int16_t cluster_id, 
                         uint16_t attribute_id, 
                         bool ack = false, 
                         uint8_t direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
                         uint8_t disable_default_response = 1, 
                         uint8_t manuf_specific = 0, 
                         uint16_t manuf_code = 0);

  void sendAttributesRead(zbg_device_params_t * device, 
                          int16_t cluster_id, 
                          uint8_t attr_number, 
                          uint16_t *attribute_ids);

  bool sendAttributeWrite(zbg_device_params_t * device, 
                          int16_t cluster_id, 
                          uint16_t attribute_id, 
                          esp_zb_zcl_attr_type_t attribute_type, 
                          uint16_t attribute_size, 
                          void *attribute_value, 
                          bool ack = false, 
                          uint8_t manuf_specific = 0, 
                          uint16_t manuf_code = 0);

  void sendIASzoneEnrollResponseCmd(zbg_device_params_t *device, 
                                    uint8_t enroll_rsp_code, 
                                    uint8_t zone_id);

  void sendOnOffCmd(zbg_device_params_t *device, bool value); 

  void sendLevelMoveToLevelCmd(zbg_device_params_t *device, 
                               uint8_t level, 
                               uint16_t transition_time);

  void sendColorMoveToHueCmd(zbg_device_params_t *device, 
                             uint8_t hue, uint8_t directon, 
                             uint16_t transition_time);

  void sendColorMoveToSaturationCmd(zbg_device_params_t *device, 
                                    uint8_t saturation, 
                                    uint16_t transition_time);

  void sendColorMoveToHueAndSaturationCmd(zbg_device_params_t *device, 
                                          uint8_t hue, 
                                          uint8_t saturation, 
                                          uint16_t transition_time);

  void sendColorEnhancedMoveToHueAndSaturationCmd(zbg_device_params_t *device, 
                                                  uint16_t enhanced_hue, 
                                                  uint8_t saturation, 
                                                  uint16_t transition_time);

  void sendColorMoveToColorCmd(zbg_device_params_t *device, 
                               uint16_t color_x, 
                               uint16_t color_y, 
                               uint16_t transition_time);

  void sendColorMoveToColorTemperatureCmd(zbg_device_params_t *device, 
                                          uint16_t color_temperature, 
                                          uint16_t transition_time);

  void sendWindowCoveringCmd(zbg_device_params_t *device, 
                             uint8_t cmd_id, 
                             void *cmd_value);

  void sendIEEEAddrReqCmd(zbg_device_params_t *device, bool ack);

  void sendDeviceFactoryReset(zbg_device_params_t *device, bool isTuya = false);

  bool sendCustomClusterCmd(zbg_device_params_t * device, 
                            int16_t custom_cluster_id, 
                            uint16_t custom_command_id, 
                            esp_zb_zcl_attr_type_t data_type, 
                            uint16_t custom_data_size, 
                            uint8_t *custom_data, 
                            bool ack = false, 
                            uint8_t direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV, 
                            uint8_t disable_default_response = 0, 
                            uint8_t manuf_specific = 0, 
                            uint16_t manuf_code = 0);

  void requestDataSave(uint8_t Supla_channel,
                       uint8_t data_save_mode, 
                       uint8_t extended_data_type = 0, 
                       uint8_t *extended_data = nullptr) {

    if (_on_data_save_request)
      _on_data_save_request(Supla_channel, 
                            data_save_mode, 
                            extended_data_type, 
                            extended_data);
  }

  void onIASzoneStatusChangeNotification (void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, int)) {
    _on_IAS_zone_status_change_notification = callback;
  }
  void onTemperatureReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, float)) {
    _on_temperature_receive = callback; 
  }
  void onHumidityReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, float)) {
    _on_humidity_receive = callback;
  }
  void onPressureReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, float)) {
    _on_pressure_receive = callback;
  }
  void onIlluminanceReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t)) {
    _on_illuminance_receive = callback;
  }
  void onFlowReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t)) {
    _on_flow_receive = callback;
  }
  void onOccupancyReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t)) {
    _on_occupancy_receive = callback;
  }
  void onOnOffReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, bool)) {
    _on_on_off_receive = callback;
  }
  void onElectricalMeasurementReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, 
                                      const esp_zb_zcl_attribute_t *)) {
    _on_electrical_measurement_receive = callback;
  }
  void onMultistateInputReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, 
                                      const esp_zb_zcl_attribute_t *)) {
    _on_multistate_input_receive = callback;
  }
  void onAnalogInputReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, 
                            const esp_zb_zcl_attribute_t *)) {
    _on_analog_input_receive = callback;
  }
  void onMeteringReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, 
                         const esp_zb_zcl_attribute_t *)) {
    _on_metering_receive = callback;
  }
  void onBasicReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, 
                         const esp_zb_zcl_attribute_t *)) {
    _on_basic_receive = callback;
  }
  void onCurrentSummationReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint64_t)) {
    _on_current_summation_receive = callback;
  }
  void onBatteryReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, uint8_t)) {
    _on_battery_receive = callback;
  }
  void onCurrentLevelReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t)) {
    _on_current_level_receive = callback;
  }
  void onColorHueReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t)) {
    _on_color_hue_receive = callback;
  }
  void onColorSaturationReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t)) {
    _on_color_saturation_receive = callback;
  }
  void onColorTemperatureReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t)) {
    _on_color_temperature_receive = callback;
  }
  void onThermostatTemperaturesReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, int16_t)) {
    _on_thermostat_temperatures_receive = callback;
  }
  void onThermostatModesReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, uint8_t)) {
    _on_thermostat_modes_receive = callback;
  }
  void onWindowCoveringReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, uint16_t)) {
    _on_window_covering_receive = callback;
  }
  void onSonoffCustomClusterReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *)) {
    _on_sonoff_custom_cluster_receive = callback;
  }
  void onDevelcoCustomClusterReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *)) {
    _on_develco_custom_cluster_receive = callback;
  }
  void onLumiCustomClusterReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *)) {
    _on_lumi_custom_cluster_receive = callback;
  }
  void onOnOffCustomCmdReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint8_t, uint8_t)) {
    _on_on_off_custom_cmd_receive = callback;
  }
  void onCustomCmdReceive(bool (*callback)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t *)) {
    _on_custom_cmd_receive = callback;
  }
  void onCmdCustomClusterReceive(void (*callback)(esp_zb_ieee_addr_t ieee_addr, 
                                                  uint16_t endpoint_id, 
                                                  uint16_t cluster_id, 
                                                  uint8_t command_id, 
                                                  uint16_t payload_size, 
                                                  uint8_t *payload)) {
    _on_cmd_custom_cluster_receive = callback;
  }
  void onBoundDevice(void (*callback)(zbg_device_params_t *, bool)) {
    _on_bound_device = callback;
  }
  void onBTCBoundDevice(void (*callback)(zbg_device_params_t *, uint8_t count, uint8_t position)) {
    _on_btc_bound_device = callback;
  }
  void onDataSaveRequest(void (*callback)(uint8_t Supla_channel, 
                                          uint8_t data_save_mode,  
                                          uint8_t extended_data_type, 
                                          uint8_t *extended_data)) {
    _on_data_save_request = callback;
  }
  void onDeviceRejoin(void (*callback)(uint16_t, esp_zb_ieee_addr_t)) {
    _on_device_rejoin = callback;
  }

private:
  // save instance of the class in order to use it in static functions
  static ZigbeeGateway *_instance;

  static volatile bool _active_pairing;

  //static volatile bool GatewayReady;

  static findcb_userdata_t findcb_userdata;
  static volatile bool _last_bind_success;
  static volatile bool _in_binding;
  static volatile bool _new_device_joined;

  static volatile uint8_t _binding_error_retries;

  //static volatile uint16_t _clusters_2_discover;
  //static volatile uint16_t _attributes_2_discover;

  //static volatile uint16_t _endpoints_2_bind;
  //static volatile uint16_t _clusters_2_bind;

  static query_basic_cluster_data_t _last_device_query;
  static esp_zb_zcl_attribute_t _read_attr_last_result;
  static esp_zb_zcl_read_report_config_resp_variable_t _read_report_config_resp_variable_last_result;
  static esp_zb_zcl_status_t _config_report_status_last_result;
  static esp_zb_zcl_status_t _read_attr_status_last_result;
  static esp_zb_zcl_status_t _write_attr_status_last_result;
  static uint16_t _write_attr_attribute_id_last_result;
  static esp_zb_zcl_status_t _custom_cmd_status_last_result;
  static uint8_t _custom_cmd_resp_to_cmd_last_result;


  static volatile uint8_t _read_attr_last_tsn;
  static volatile uint8_t _read_attr_tsn_list[256];
  static volatile uint8_t _custom_cmd_last_tsn;
  static volatile uint8_t _custom_cmd_last_tsn_flag;
  static volatile uint8_t _set_config_last_tsn;
  static volatile uint8_t _set_config_last_tsn_flag;
  static volatile uint8_t _read_config_last_tsn;
  static volatile uint8_t _read_config_last_tsn_flag;
  static volatile uint8_t _write_attr_last_tsn;
  static volatile uint8_t _write_attr_last_tsn_flag;
  
  //static volatile uint8_t _custom_cmd_tsn_list[256];
  
  //static bool _read_attr_async;
  //static bool enable_attribute_reporting 

  static zbg_device_unit_t zbg_device_units[ZBG_MAX_DEVICES];

  void (*_on_IAS_zone_status_change_notification)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, int);
  void (*_on_temperature_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, float);
  void (*_on_humidity_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, float);
  void (*_on_pressure_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, float);
  void (*_on_illuminance_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t);
  void (*_on_flow_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t);
  void (*_on_occupancy_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t);
  void (*_on_on_off_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, bool);
  void (*_on_electrical_measurement_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_multistate_input_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_analog_input_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_metering_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_basic_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_current_summation_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint64_t);
  void (*_on_battery_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, uint8_t);
  void (*_on_current_level_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t);
  void (*_on_color_hue_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t);
  void (*_on_color_saturation_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t);
  void (*_on_color_temperature_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t);
  void (*_on_thermostat_temperatures_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, int16_t);
  void (*_on_thermostat_modes_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, uint8_t);
  void (*_on_window_covering_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint16_t, uint16_t);
  void (*_on_sonoff_custom_cluster_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_develco_custom_cluster_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_lumi_custom_cluster_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, const esp_zb_zcl_attribute_t *);
  void (*_on_on_off_custom_cmd_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint8_t, uint8_t);
  bool (*_on_custom_cmd_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t, uint8_t, uint8_t *);

  void (*_on_cmd_custom_cluster_receive)(esp_zb_ieee_addr_t ieee_addr, uint16_t, uint16_t, uint8_t, uint16_t, uint8_t *);

  void (*_on_bound_device)(zbg_device_params_t *, bool);
  void (*_on_btc_bound_device)(zbg_device_params_t *, uint8_t count, uint8_t position);

  void (*_on_data_save_request)(uint8_t , uint8_t, uint8_t, uint8_t *);

  void (*_on_device_rejoin)(uint16_t, esp_zb_ieee_addr_t);

  void findEndpoint(esp_zb_zdo_match_desc_req_param_t *cmd_req);

  bool zbRawCmdHandler( esp_zb_zcl_addr_t source, uint8_t src_endpoint, uint8_t dst_endpoint, uint16_t cluster_id, uint8_t cmd_id, 
                                bool is_common_command, bool disable_default_response, bool is_manuf_specific, uint16_t manuf_specific,
                                uint8_t buffer_size, uint8_t *buffer);

  static void bindCb(esp_zb_zdp_status_t zdo_status, void *user_ctx);
  static void find_Cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx);
  static void ieee_Cb(esp_zb_zdp_status_t zdo_status, esp_zb_zdo_ieee_addr_rsp_t *resp, void *user_ctx);

  static void Z2S_active_ep_req_cb(esp_zb_zdp_status_t zdo_status, uint8_t ep_count, uint8_t *ep_id_list, void *user_ctx);
  static void Z2S_simple_desc_req_cb(esp_zb_zdp_status_t zdo_status, esp_zb_af_simple_desc_1_1_t *simple_desc, void *user_ctx);

  void zbAttributeReporting(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                            const esp_zb_zcl_attribute_t *attribute) override;
  void zbReadAttrResponse(uint8_t tsn, esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                          esp_zb_zcl_status_t status, const esp_zb_zcl_attribute_t *attribute) override;
  void zbWriteAttrResponse(uint8_t tsn, esp_zb_zcl_status_t status, uint16_t attribute_id) override;
  void zbIASZoneEnrollRequest(const esp_zb_zcl_ias_zone_enroll_request_message_t *message) override;
  void zbIASZoneStatusChangeNotification(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) override;
  void zbCmdDiscAttrResponse(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                            const esp_zb_zcl_disc_attr_variable_t *variable) override;
  void zbCmdCustomClusterReq(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, 
                             uint16_t cluster_id,uint8_t command_id, 
                             uint16_t payload_size, uint8_t *payload) override;
  void zbConfigReportResponse(uint8_t tsn, esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, 
                              uint16_t cluster_id, esp_zb_zcl_status_t status, uint8_t direction, 
                             uint16_t attribute_id) override;
  void zbReadReportConfigResponse(const esp_zb_zcl_cmd_read_report_config_resp_message_t *message) override;
  void zbCmdDefaultResponse(uint8_t tsn, int8_t rssi, esp_zb_zcl_addr_t src_address, 
                            uint16_t src_endpoint, uint16_t cluster_id, uint8_t resp_to_cmd, 
                            esp_zb_zcl_status_t status_code) override;

  void zbDeviceAnnce(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) override;
  void zbDeviceRejoin(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) override;
  void zbDeviceLeave(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr, uint8_t rejoin) override;

  void addBoundDevice(zb_device_params_t *device, uint16_t cluster_id, uint8_t count, uint8_t position) override;
  bool isDeviceBound(uint16_t short_addr, esp_zb_ieee_addr_t ieee_addr) override;

  static void updateZbgDeviceUnitLastSeenMs(uint16_t short_addr);
  static void updateZbgDeviceUnitLastRssi(uint16_t short_addr, int8_t rssi);

protected:

  static SemaphoreHandle_t gt_lock;

  std::list<zbg_device_params_t *> _joined_devices;
  std::list<zbg_device_params_t *> _gateway_devices;
};

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
