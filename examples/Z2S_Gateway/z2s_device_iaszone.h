#ifndef Z2S_DEVICE_IASZONE_H_
#define Z2S_DEVICE_IASZONE_H_

#include "z2s_devices_table.h"
#include <supla/sensor/virtual_binary.h>

void initZ2SDeviceIASzone(uint8_t Supla_channel);

void addZ2SDeviceIASzone(zbg_device_params_t *device, uint8_t free_slot);

void msgZ2SDeviceIASzone(uint8_t Supla_channel, int zone_status); 

#endif