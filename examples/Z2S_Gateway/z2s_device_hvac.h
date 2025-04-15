#ifndef Z2S_DEVICE_HVAC_H_
#define Z2S_DEVICE_HVAC_H_

#include "z2s_devices_table.h"

#include <supla/actions.h>
#include <supla/events.h>
#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_trv_interface.h>

#define TRV_HEATING_SETPOINT_MSG  0x01
#define TRV_SYSTEM_MODE_MSG       0x02
#define TRV_LOCAL_TEMPERATURE_MSG 0x03
#define TRV_RUNNING_STATE_MSG     0x04


void initZ2SDeviceHvac(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot);

void addZ2SDeviceHvac(ZigbeeGateway * gateway, zbg_device_params_t *device, uint8_t free_slot, uint8_t trv_thermometer_slot);

void msgZ2SDeviceHvac(int16_t channel_number_slot, uint8_t msg_id, int16_t msg_value, signed char rssi);

#endif