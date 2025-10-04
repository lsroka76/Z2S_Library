#ifndef Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_
#define Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_

#include "z2s_devices_table.h"
#include <supla/control/virtual_relay.h>
#include <Z2S_control/Z2S_local_action_handlers.h>

void initZ2SDeviceLocalActionHandler(int16_t channel_number_slot);

bool addZ2SDeviceLocalActionHandler(uint8_t local_channel_type = 1, 
                                    uint32_t local_channel_func = 0);


#endif //Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_