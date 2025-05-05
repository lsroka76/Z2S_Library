#ifndef Z2S_DEVICE_HVAC_H_
#define Z2S_DEVICE_HVAC_H_

#include "z2s_devices_table.h"

#include <supla/actions.h>
#include <supla/events.h>
#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_trv_interface.h>

#define TRV_HEATING_SETPOINT_MSG        0x01
#define TRV_SYSTEM_MODE_MSG             0x02
#define TRV_LOCAL_TEMPERATURE_MSG       0x03
#define TRV_RUNNING_STATE_MSG           0x04
#define TRV_TEMPERATURE_CALIBRATION_MSG 0x05
#define TRV_LOW_BATTERY_MSG             0x06
#define TRV_BATTERY_LEVEL_MSG           0x07
#define TRV_CHILD_LOCK_MSG              0x08
#define TRV_WINDOW_DETECT_MSG           0x09
#define TRV_ANTI_FREEZE_MSG             0x0A
#define TRV_LIMESCALE_PROTECT_MSG       0x0B
#define TRV_SCHEDULE_MODE_MSG           0x0C


void initZ2SDeviceHvac(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot);

void addZ2SDeviceHvac(ZigbeeGateway * gateway, zbg_device_params_t *device, uint8_t free_slot, uint8_t trv_thermometer_slot);

void msgZ2SDeviceHvac(int16_t channel_number_slot, uint8_t msg_id, int32_t msg_value, signed char rssi);

#endif
