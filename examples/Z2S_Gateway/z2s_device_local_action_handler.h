#ifndef Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_
#define Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_

#include "z2s_devices_table.h"

#include <SuplaDevice.h>
#include <supla/storage/eeprom.h>
#include <supla/storage/littlefs_config.h>
#include <supla/control/virtual_relay.h>
#include <supla/sensor/virtual_binary.h>
#include <Z2S_control/Z2S_local_action_handlers.h>
#include <Z2S_control/Z2S_remote_relay.h>
#include <Z2S_sensor/Z2S_remote_thermometer.h>


#define LOCAL_CHANNEL_TYPE_ACTION_HANDLER       0x01
#define LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY        0x02
#define LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY       0x03

#define LOCAL_CHANNEL_TYPE_REMOTE_RELAY         0x10
#define LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER   0x11

#define REMOTE_ADDRESS_TYPE_LOCAL               0x00
#define REMOTE_ADDRESS_TYPE_IP4                 0x01
#define REMOTE_ADDRESS_TYPE_MDNS                0x02

static constexpr char *ACTION_HANDLERS_DEFAULT_NAMES[] PROGMEM = {

  "LOCAL ACTION HANDLER",
  "AND GATE",
  "OR GATE",
  "NOT GATE",
  "XOR GATE",
  "NAND GATE",
  "NOR GATE",
  "AND GATE (3 INPUTS)",
  "OR GATE (3 INPUTS)"
};

const char* getZ2SDeviceLocalActionHandlerTypeName(int16_t channel_number_slot);

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(int16_t channel_number_slot);

void initZ2SDeviceLocalActionHandler(int16_t channel_number_slot);

bool addZ2SDeviceLocalActionHandler(uint8_t local_channel_type = LOCAL_CHANNEL_TYPE_ACTION_HANDLER, 
                                    uint32_t local_channel_func = SUPLA_CHANNELFNC_NONE,
                                    uint8_t logic_operator = PIN_LOGIC_OPERATOR_NONE);


#endif //Z2S_DEVICE_LOCAL_ACTION_HANDLERS_H_