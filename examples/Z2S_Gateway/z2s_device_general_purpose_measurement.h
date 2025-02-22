#ifndef Z2S_DEVICE_GENERAL_PURPOSE_MEASUREMENT_H_
#define Z2S_DEVICE_GENERAL_PURPOSE_MEASUREMENT_H_

#include "z2s_devices_table.h"
#include <supla/sensor/general_purpose_measurement.h>

void initZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot);

void addZ2SDeviceGeneralPurposeMeasurement(zbg_device_params_t *device, uint8_t free_slot, 
                                          char *name = nullptr, uint32_t func = 0, char *unit = nullptr);

void msgZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot, double value, signed char rssi); 

#endif