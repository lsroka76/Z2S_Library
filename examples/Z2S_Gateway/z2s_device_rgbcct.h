#ifndef Z2S_DEVICE_RGBCCT_H_
#define Z2S_DEVICE_RGBCCT_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_rgbcct_interface.h>

/*****************************************************************************/

void initZ2SDeviceRGBCCT(
  zbg_device_params_t *device, int16_t channel_number_slot);

/*****************************************************************************/

void addZ2SDeviceRGBCCT(
  zbg_device_params_t *device, uint8_t free_slot, const char *name = nullptr, 
  uint32_t func = 0);

/*****************************************************************************/

void msgZ2SDeviceRGBCCT(
  uint32_t model_id, uint8_t Supla_channel, uint8_t hue, uint8_t saturation, 
  bool state);

#endif