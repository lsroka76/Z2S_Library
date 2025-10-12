#ifndef Z2S_DEVICE_ACTION_TRIGGER_H_
#define Z2S_DEVICE_ACTION_TRIGGER_H_

#include "z2s_devices_table.h"

#include <Z2S_control/Z2S_virtual_relay_scene_switch.h>

/*---------------------------------------------------------------------------------------------------------------------------*/

static const char* default_at_name = "Virtual button";

/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceActionTrigger(int16_t channel_number_slot);

/*---------------------------------------------------------------------------------------------------------------------------*/

void addZ2SDeviceActionTrigger(zbg_device_params_t *device, 
                               uint8_t free_slot, 
                               int8_t sub_id = -1, 
                               const char *name = nullptr, 
                               uint32_t func = 0);

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceActionTrigger(int16_t channel_number_slot);


#endif
