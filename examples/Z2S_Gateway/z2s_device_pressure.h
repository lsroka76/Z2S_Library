#ifndef Z2S_DEVICE_PRESSURE_H_
#define Z2S_DEVICE_PRESSURE_H_

#include "z2s_devices_table.h"
#include "virtual_pressure.h"

void initZ2SDevicePressure(int16_t channel_number_slot);

void addZ2SDevicePressure(zbg_device_params_t *device, uint8_t free_slot);

void msgZ2SDevicePressure(int16_t channel_number_slot, double pressure);

void msgZ2SDevicePressureBatteryLevel(int16_t channel_number_slot, uint8_t battery_level); 

#endif