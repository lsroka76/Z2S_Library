#ifndef Z2S_DEVICE_GENERAL_PURPOSE_MEASUREMENT_H_
#define Z2S_DEVICE_GENERAL_PURPOSE_MEASUREMENT_H_

#include "z2s_devices_table.h"
#include <supla/sensor/general_purpose_measurement.h>

#define ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE         0x00
#define ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE  0x01
#define ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_MOTION_STATE 0x02
#define ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_PPM          0x03

void initZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot);

void addZ2SDeviceGeneralPurposeMeasurement(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id = -1,
                                          char *name = nullptr, uint32_t func = 0, char *unit = nullptr);

void msgZ2SDeviceGeneralPurposeMeasurement(int16_t channel_number_slot, uint8_t function, double value, signed char rssi); 

#endif