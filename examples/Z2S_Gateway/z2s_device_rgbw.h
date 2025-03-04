#ifndef Z2S_DEVICE_RGBW_H_
#define Z2S_DEVICE_RGBW_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_Tuya_rgbw_bulb.h>
//#include <Z2S_control/Z2S_Tuya_hue_dimmer_bulb.h>

void initZ2SDeviceRGBW(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel);

void addZ2SDeviceRGBW(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name = nullptr, uint32_t func = 0);

void msgZ2SDeviceRGBW(uint32_t model_id, uint8_t Supla_channel, uint8_t hue, uint8_t saturation, int16_t level, bool state, signed char rssi);


#endif