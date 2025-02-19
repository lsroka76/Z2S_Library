#ifndef Z2S_DEVICE_RGBW_LIGHT_SOURCE_H_
#define Z2S_DEVICE_RGBW_LIGHT_SOURCE_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_rgbw_light_source.h>


void initZ2SDeviceRGBWLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel);

void addZ2SDeviceRGBWLightSource(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name = nullptr, uint32_t func = 0);

void msgZ2SDeviceRGBWLightSource(uint8_t Supla_channel, uint8_t id, uint8_t value, bool state = true, signed char rssi = 0);

#endif