#ifndef Z2S_DEVICES_TABLE_H_
#define Z2S_DEVICES_TABLE_H_

#include <ZigbeeGateway.h>
#include "Z2S_Devices_Database.h"

#include <SuplaDevice.h>
#include <supla/sensor/virtual_binary.h>
#include <supla/sensor/virtual_therm_hygro_meter.h>
#include <supla/sensor/one_phase_electricity_meter.h>
//#include <supla/control/hvac_base.h>
#include <supla/control/internal_pin_output.h>
#include <supla/sensor/virtual_thermometer.h>

#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_virtual_relay.h>
#include <Z2S_control/Z2S_Tuya_thermostat.h>
#include <Z2S_sensor/Z2S_OnePhaseElectricityMeter.h>
#include <Z2S_sensor/Z2S_virtual_therm_hygro_meter.h>

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

#define Z2S_CHANNELMAXCOUNT SUPLA_CHANNELMAXCOUNT

extern z2s_device_params_t z2s_devices_table[Z2S_CHANNELMAXCOUNT];

const static char   Z2S_DEVICES_TABLE []  PROGMEM = "Z2S_devs_table";
const static char   Z2S_DEVICES_TABLE_SIZE []  PROGMEM = "Z2S_devs_ts";


uint32_t Z2S_getDevicesTableSize();

uint8_t Z2S_findFirstFreeDevicesTableSlot();

void Z2S_printDevicesTableSlots();

int16_t Z2S_findChannelNumberSlot(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int32_t channel_type );

void Z2S_fillDevicesTableSlot(zb_device_params_t *device, uint8_t slot, uint8_t channel, int32_t channel_type);

bool Z2S_loadDevicesTable();

bool Z2S_saveDevicesTable();

void Z2S_initSuplaChannels(); 

void Z2S_onTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float temperature); 

void Z2S_onHumidityReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float humidity); 

void Z2S_onOnOffReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, bool state); 

void Z2S_onRMSVoltageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t voltage); 

void Z2S_onRMSCurrentReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t current);

void Z2S_onRMSActivePowerReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t active_power);

void Z2S_onBatteryPercentageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t battery_remaining);

void Z2S_onCmdCustomClusterReceive( esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t command_id,
                                    uint16_t payload_size, uint8_t *payload);

void Z2S_onIASzoneStatusChangeNotification(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int iaszone_status);

void Z2S_onBTCBoundDevice(zb_device_params_t *device); 

void Z2S_onBoundDevice(zb_device_params_t *device, bool last_cluster);

void Z2S_addZ2SDevice(zb_device_params_t *device);

#endif