#ifndef Z2S_DEVICE_ACTION_TRIGGER_H_
#define Z2S_DEVICE_ACTION_TRIGGER_H_

#include "z2s_devices_table.h"
#include <supla/control/action_trigger.h>
#include <supla/actions.h>
#include <Z2S_control/Z2S_virtual_relay_scene_switch.h>
#include <Z2S_control/Z2S_local_action_handlers.h>

typedef struct virtual_button_data_s {

  uint32_t button_id;
  uint32_t button_action_trigger_flag;
  uint32_t button_action_id;
  const char *button_action_trigger_name;

} virtual_button_data_t;

/*****************************************************************************/

static const char* default_at_name = "Virtual button";

/*****************************************************************************/

void initZ2SDeviceActionTrigger(int16_t channel_number_slot);

/*****************************************************************************/

void initZ2SDeviceActionTriggerV2(int16_t channel_number_slot);

/*****************************************************************************/

void addZ2SDeviceActionTrigger(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id = -1, 
  const char *name = nullptr, uint32_t func = 0);

/*****************************************************************************/

void addZ2SDeviceActionTriggerV2(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id = -1, 
  const char *name = nullptr, uint32_t func = 0);

/*****************************************************************************/

bool getVirtualButtonNumber(
  virtual_button_data_t &virtual_button_data, uint8_t endpoint_id, 
  uint16_t cluster_id, uint32_t model_id, int8_t sub_id);

/*****************************************************************************/

void msgZ2SDeviceActionTrigger(int16_t channel_number_slot);

/*****************************************************************************/

void msgZ2SDeviceActionTriggerV2(int16_t channel_number_slot, int8_t sub_id);


/*****************************************************************************/

#endif
