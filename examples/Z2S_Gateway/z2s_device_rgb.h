#ifndef Z2S_DEVICE_RGB_H_
#define Z2S_DEVICE_RGB_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_Tuya_rgb_bulb.h>

void initZ2SDeviceRGB(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel);

void addZ2SDeviceRGB(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name = nullptr, uint32_t func = 0);

void msgZ2SDeviceRGB(uint32_t model_id, uint8_t Supla_channel, uint8_t hue, uint8_t saturation, bool state);


#endif