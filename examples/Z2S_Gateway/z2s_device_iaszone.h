#ifndef Z2S_DEVICE_IASZONE_H_
#define Z2S_DEVICE_IASZONE_H_

#include "z2s_devices_table.h"
#include <supla/sensor/virtual_binary.h>

void initZ2SDeviceIASzone(int16_t channel_number_slot);

void addZ2SDeviceIASzone(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id = -1, char *name = nullptr, uint32_t func = 0);

void msgZ2SDeviceIASzone(int16_t channel_number_slot, bool state, signed char rssi); 

#endif