#ifndef Z2S_DEVICE_VIRTUAL_VALVE_H_
#define Z2S_DEVICE_VIRTUAL_VALVE_H_

#include "z2s_devices_table.h"

#include <Z2S_control/Z2S_virtual_valve.h>

/*****************************************************************************/

void initZ2SDeviceVirtualValve(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot);

/*****************************************************************************/

void initZ2SDeviceVirtualValve(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel);

/*****************************************************************************/

void addZ2SDeviceVirtualValve(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t free_slot, 
  int8_t sub_id = -1, const char *name = nullptr, uint32_t func = 0);

/*****************************************************************************/

//void msgZ2SDeviceVirtualValve(int16_t channel_number_slot, bool state);

/*****************************************************************************/

void msgZ2SDeviceVirtualValve(Supla::Element* element, bool state);

/*****************************************************************************/
#endif