#ifndef Z2S_DEVICE_HVAC_H_
#define Z2S_DEVICE_HVAC_H_

#define CURRENT_HEATING_SETPOINT_MSG 0x01
#define TRV_SYSTEM_MODE_MSG          0x02
#define TRV_TURN_OFF_MSG             0x03

#include "z2s_devices_table.h"

#include <supla/actions.h>
#include <supla/events.h>
#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_trv_interface.h>

void initZ2SDeviceHvac(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot);

void addZ2SDeviceHvac(ZigbeeGateway * gateway, zbg_device_params_t *device, uint8_t free_slot, uint8_t trv_thermometer_slot);

void msgZ2SDeviceHvac(int16_t channel_number_slot, uint8_t msg_id, uint16_t msg_value, signed char rssi);

#endif