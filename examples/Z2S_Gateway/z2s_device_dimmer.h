#ifndef Z2S_DEVICE_DIMMER_H_
#define Z2S_DEVICE_DIMMER_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_Tuya_dimmer_bulb.h>
#include <Z2S_control/Z2S_Tuya_dimmer_switch.h>

void initZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel);

void addZ2SDeviceDimmer(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, char *name = nullptr, uint32_t func = 0);


void msgZ2SDeviceTuyaDimmerLightSource(uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                       uint16_t payload_size, uint8_t *payload);

void msgZ2SDeviceDimmer(uint32_t model_id, uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                  uint16_t payload_size, uint8_t *payload);

void msgZ2SDeviceDimmer(uint32_t model_id, uint8_t Supla_channel, int16_t level, bool state);


#endif