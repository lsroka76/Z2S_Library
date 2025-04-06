#ifndef Z2S_DEVICE_HVAC_H_
#define Z2S_DEVICE_HVAC_H_

#include "z2s_devices_table.h"

#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_trv_output_interface.h>

void initZ2SDeviceHvac(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t Supla_channel) {
  
}

void addZ2SDeviceHvac(ZigbeeGateway * gateway, zbg_device_params_t *device, uint8_t free_slot) {

}

void msgZ2SDeviceHvac(uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                    uint16_t payload_size, uint8_t *payload, signed char rssi) {
                                      

}

#endif