#ifndef Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_
#define Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_

#include <SuplaDevice.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/control/virtual_relay.h>
#include <supla/control/hvac_base.h>
#include <supla/sensor/virtual_binary.h>
#include <Z2S_control/Z2S_local_action_handlers.h>
#include <Z2S_control/Z2S_remote_relay.h>
#include <Z2S_control/Z2S_virtual_relay.h>
#include <Z2S_control/virtual_output_interface.h>
#include <Z2S_sensor/Z2S_remote_thermometer.h>

#include "z2s_devices_table.h"

static constexpr char *ACTION_HANDLERS_DEFAULT_NAMES[] PROGMEM = {

  "LOCAL ACTION HANDLER",
  "AND GATE",
  "OR GATE",
  "NOT GATE",
  "XOR GATE",
  "NAND GATE",
  "NOR GATE",
  "AND GATE (3 INPUTS)",
  "OR GATE (3 INPUTS)",
  "NOP GATE"
};

static constexpr char* CONNECTED_THERMOMETERS_FUNCTION_NAMES[] PROGMEM = {

"CONNECTED_THERMOMETERS_FNC_MINIMUM",
"CONNECTED_THERMOMETERS_FNC_AVERAGE",
"CONNECTED_THERMOMETERS_FNC_MAXIMUM"
};

/*****************************************************************************/

const char* getZ2SDeviceLocalActionHandlerTypeName(
  int16_t channel_number_slot);

const char* getZ2SDeviceLocalActionHandlerTypeName(
  z2s_device_params_t* _z2s_channel);

/*****************************************************************************/

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(
  int16_t channel_number_slot);

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(
  z2s_device_params_t* _z2s_channel);

/*****************************************************************************/

void initZ2SDeviceLocalActionHandler(int16_t channel_number_slot);

void initZ2SDeviceLocalActionHandler(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel);
  
/*****************************************************************************/

void initZ2SDeviceGatewayEvents(int16_t channel_number_slot);

void initZ2SDeviceGatewayEvents(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel);

/*****************************************************************************/

bool addZ2SDeviceGatewayEvents(int16_t channel_number_slot);

bool addZ2SDeviceLocalActionHandler(
  uint8_t local_channel_type = LOCAL_CHANNEL_TYPE_ACTION_HANDLER, 
  uint32_t local_channel_func = SUPLA_CHANNELFNC_NONE,
  uint8_t logic_operator = PIN_LOGIC_OPERATOR_NONE);


#endif //Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_