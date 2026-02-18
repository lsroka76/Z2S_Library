#ifndef Z2S_DEVICE_ELECTRICITY_METER_H_
#define Z2S_DEVICE_ELECTRICITY_METER_H_

#include "z2s_devices_table.h"
#include <Z2S_control/Z2S_virtual_relay.h>
#include <Z2S_sensor/Z2S_ElectricityMeter.h>

/*---------------------------------------------------------------------------------------------------------------------------*/

#define Z2S_EM_VOLTAGE_A_SEL           0x0000
#define Z2S_EM_CURRENT_A_SEL           0x0001
#define Z2S_EM_ACTIVE_POWER_A_SEL      0x0002
#define Z2S_EM_ACT_FWD_ENERGY_A_SEL    0x0003
#define Z2S_EM_ACT_RVR_ENERGY_A_SEL    0x0004
#define Z2S_EM_POWER_FACTOR_A_SEL      0x0005
#define Z2S_EM_REACTIVE_POWER_A_SEL    0x0006
#define Z2S_EM_APPARENT_POWER_A_SEL    0x0007

#define Z2S_EM_VOLTAGE_B_SEL           0x0010
#define Z2S_EM_CURRENT_B_SEL           0x0011
#define Z2S_EM_ACTIVE_POWER_B_SEL      0x0012
#define Z2S_EM_ACT_FWD_ENERGY_B_SEL    0x0013
#define Z2S_EM_POWER_FACTOR_B_SEL      0x0014
#define Z2S_EM_ACT_RVR_ENERGY_B_SEL    0x0015
#define Z2S_EM_REACTIVE_POWER_B_SEL    0x0016
#define Z2S_EM_APPARENT_POWER_B_SEL    0x0017

#define Z2S_EM_VOLTAGE_C_SEL           0x0020
#define Z2S_EM_CURRENT_C_SEL           0x0021
#define Z2S_EM_ACTIVE_POWER_C_SEL      0x0022
#define Z2S_EM_ACT_FWD_ENERGY_C_SEL    0x0023
#define Z2S_EM_POWER_FACTOR_C_SEL      0x0024
#define Z2S_EM_ACT_RVR_ENERGY_C_SEL    0x0025
#define Z2S_EM_REACTIVE_POWER_C_SEL    0x0026
#define Z2S_EM_APPARENT_POWER_C_SEL    0x0027

#define Z2S_EM_AC_FREQUENCY            0x0030

#define Z2S_EM_AC_VOLTAGE_MUL_SEL      0x0040
#define Z2S_EM_AC_VOLTAGE_DIV_SEL      0x0041
#define Z2S_EM_AC_CURRENT_MUL_SEL      0x0042
#define Z2S_EM_AC_CURRENT_DIV_SEL      0x0043
#define Z2S_EM_AC_ACTIVE_POWER_MUL_SEL 0x0044
#define Z2S_EM_AC_ACTIVE_POWER_DIV_SEL 0x0045
#define Z2S_EM_AC_FREQUENCY_MUL_SEL    0x0046
#define Z2S_EM_AC_FREQUENCY_DIV_SEL    0x0047

#define Z2S_EM_ACT_FWD_ENERGY_MUL_SEL  0x0048
#define Z2S_EM_ACT_FWD_ENERGY_DIV_SEL  0x0049

/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceElectricityMeter(
  ZigbeeGateway *gateway, zbg_device_params_t *device, 
  int16_t channel_number_slot);

/*---------------------------------------------------------------------------------------------------------------------------*/

void addZ2SDeviceElectricityMeter(
  ZigbeeGateway *gateway, zbg_device_params_t *device, bool isTuya, 
  bool active_query, uint8_t free_slot,int8_t sub_id = -1, 
  bool one_phase = true);

/*---------------------------------------------------------------------------------------------------------------------------*/

void updateZ2SDeviceElectricityMeter(int16_t channel_number_slot);

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceElectricityMeter(
  int16_t channel_number_slot, uint8_t selector, int64_t value);

#endif
