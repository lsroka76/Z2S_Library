#ifndef Z2S_DEVICE_ELECTRICITY_METER_H_
#define Z2S_DEVICE_ELECTRICITY_METER_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_virtual_relay.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include <Z2S_sensor/Z2S_OnePhaseElectricityMeter.h>

#define Z2S_EM_VOLTAGE_SEL        0x0000
#define Z2S_EM_CURRENT_SEL        0x0001
#define Z2S_EM_ACTIVE_POWER_SEL   0x0002
#define Z2S_ACT_FWD_ENERGY_SEL    0x0003


void initZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot);

void addZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zbg_device_params_t *device, bool isTuya, bool active_query, uint8_t free_slot);

void msgZ2SDeviceElectricityMeter(int16_t channel_number_slot, uint8_t selector, uint64_t value, signed char rssi);

#endif