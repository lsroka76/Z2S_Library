#ifndef Z2S_DEVICE_VIRTUAL_RELAY_H_
#define Z2S_DEVICE_VIRTUAL_RELAY_H_

#include "z2s_devices_table.h"

#include <Z2S_control/Z2S_virtual_relay.h>
#include <Z2S_control/Z2S_roller_shutter.h>

#define RS_CURRENT_POSITION_LIFT_PERCENTAGE_MSG 0x01
#define RS_MOVING_DIRECTION_MSG                 0x02

#define VRV_U8_ID  0x01
#define VRV_S8_ID  0x02
#define VRV_U32_ID 0x03
#define VRV_S32_ID 0x04

static const char *default_relay_name = "Relay";
static const char *default_rs_name = "Roller shutter";

void initZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot);

void addZ2SDeviceVirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
                              int8_t sub_id = -1, char *name = nullptr, uint32_t func = 0);

void msgZ2SDeviceVirtualRelay(int16_t channel_number_slot, bool state);

void msgZ2SDeviceVirtualRelayValue(int16_t channel_number_slot, uint8_t value_id, uint32_t value);

void msgZ2SDeviceRollerShutter(int16_t channel_number_slot, uint8_t msg_id, uint16_t msg_value);

#endif