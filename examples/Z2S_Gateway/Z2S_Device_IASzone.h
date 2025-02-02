#ifndef Z2S_DEVICE_IASZONE_H_
#define Z2S_DEVICE_IASZONE_H_

#include "Z2S_Devices_Table.h"
#include <supla/sensor/virtual_binary.h>

void initZ2SDeviceIASzone(uint8_t Supla_channel);

void addZ2SDeviceIASzone(zb_device_params_t *device, uint8_t free_slot);

void msgZ2SDeviceIASzone(uint8_t Supla_channel, int zone_status); 

#endif