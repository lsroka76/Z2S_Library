/* Class of Zigbee Temperature sensor endpoint inherited from common EP class */

#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_IEEE802154_SUPPORTED 
//&& CONFIG_ZB_ENABLED

#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"

//define the thermostat configuration to avoid narrowing conversion issue in zigbee-sdk
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

typedef struct findcb_userdata_s {
  uint8_t   _endpoint;
  uint16_t  _cluster_id;
} findcb_userdata_t;

class ZigbeeGateway : public ZigbeeEP {
public:
  ZigbeeGateway(uint8_t endpoint);
  ~ZigbeeGateway();

  std::list<zb_device_params_t *> getGatewayDevices() const {
    return _gateway_devices;
  }

  void addGatewayDevice(zb_device_params_t * device){
    
    if (device) _gateway_devices.push_back(device);
  }

  std::list<zb_device_params_t *> getJoinedDevices() const {
    
    return _joined_devices;
  }
  
  zb_device_params_t * getLastJoinedDevice() {
    
    zb_device_params_t *last_joined_device = NULL;
    
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

  static void clearNewDeviceJoined(){
    _new_device_joined = false;
  }

void zbPrintDeviceDiscovery (zb_device_params_t * device);

static void bindDeviceCluster(zb_device_params_t *,int16_t cluster_id );

void setIASZReporting(uint16_t short_addr, uint16_t endpoint, uint16_t min_interval, uint16_t max_interval);

  void onStatusNotification(void (*callback)(int, uint8_t *)) {
    _on_status_notification = callback;
  }
  void onConfigRecieve(void (*callback)(float, float, float)) {
    _on_config_recieve = callback;
  }

private:
  // save instance of the class in order to use it in static functions
  static ZigbeeGateway *_instance;

  static findcb_userdata_t findcb_userdata;
  static bool _last_bind_success;
  static bool _in_binding;
  static bool _new_device_joined;

  static uint16_t _clusters_2_discover;
  static uint16_t _attributes_2_discover; 

  void (*_on_status_notification)(int, uint8_t *);
  void (*_on_config_recieve)(float, float, float);
 
  void findEndpoint(esp_zb_zdo_match_desc_req_param_t *cmd_req);

  static void bindCb(esp_zb_zdp_status_t zdo_status, void *user_ctx);
  static void find_Cb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx);

  static void Z2S_active_ep_req_cb(esp_zb_zdp_status_t zdo_status, uint8_t ep_count, uint8_t *ep_id_list, void *user_ctx);
  static void Z2S_simple_desc_req_cb(esp_zb_zdp_status_t zdo_status, esp_zb_af_simple_desc_1_1_t *simple_desc, void *user_ctx);

  void zbAttributeRead(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, const esp_zb_zcl_attribute_t *attribute) override;
  void zbIASZoneStatusChangeNotification(const esp_zb_zcl_ias_zone_status_change_notification_message_t *message) override;
  void zbCmdDiscAttrResponse(esp_zb_zcl_addr_t src_address, uint16_t src_endpoint, uint16_t cluster_id, 
                            const esp_zb_zcl_disc_attr_variable_t *variable) override;
  
  void addBoundDevice(zb_device_params_t *device) override;
protected:

  static SemaphoreHandle_t gt_lock;
  std::list<zb_device_params_t *> _joined_devices;
  std::list<zb_device_params_t *> _gateway_devices;
};

#endif  //SOC_IEEE802154_SUPPORTED && CONFIG_ZB_ENABLED
