/*****************************************************************************/

#include <cstring>
#include <Arduino.h>
#include <Preferences.h>
#include <ZigbeeGateway.h>
#include <SuplaDevice.h>
#include <supla/element.h>

#include "Z2S_common.h"
#include "z2s_little_fs.h"

#include <Z2S_control/Z2S_remote_relay.h>
#include <Z2S_sensor/Z2S_remote_thermometer.h>
#include <Z2S_control/Z2S_local_action_handlers.h>
#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_trv_interface.h>

/*****************************************************************************/

extern Preferences Z2S_GatewayPreferences;

/*****************************************************************************/

uint8_t z2s_elements_index_table[Z2S_ELEMENTS_MAX_NUMBER / 8] = {};

/*****************************************************************************/

size_t mbstrnlen(const char *mb_str, size_t max_bytes) {

	if (mb_str == nullptr)
		return 0;

	size_t mb_counter = 0;
	size_t str_counter = 0;
	while (*(mb_str + str_counter) != '\0') {

		uint8_t next_char = *(mb_str + str_counter);
		if (next_char <= 0x7F)
			str_counter++;
		else 
		if ((next_char >= 0xC0) && (next_char <= 0xDF))
			str_counter += 2;
		else 
		if ((next_char >= 0xE0) && (next_char <= 0xEF))
			str_counter += 3;
		else 
		if ((next_char >= 0xF0) && (next_char <= 0xF7)) 
			str_counter += 4;
	else 
		if ((next_char >= 0xF8) && (next_char <= 0xFB))
			str_counter += 5;
	else 
		if ((next_char >= 0xFC) && (next_char <= 0xFD)) 
			str_counter += 6;
	else 
		if ((next_char >= 0xFE) && (next_char <= 0xFF)) 
			return 0;

	if (str_counter > max_bytes)
		return mb_counter; 
	else
		mb_counter = str_counter;
	}
	return mb_counter;
}

/*****************************************************************************/

bool checkIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index) {

  if (index_position >= max_index)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index = index_position % 8;

  if (*(index_table + byte_index) & (1 << bit_index))
    return true;
  else
    return false;
}

/*****************************************************************************/

bool setIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index) {

  if (index_position >= max_index)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index = index_position % 8;

  *(index_table + byte_index) |= (1 << bit_index);
  return true;
}

/*****************************************************************************/

bool clearIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index) {

  if (index_position >= max_index)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index = index_position % 8;

  *(index_table + byte_index) &= ~(1 << bit_index);
  return true;
}

/*****************************************************************************/

bool Z2S_loadIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name, 
  bool use_new_format, const char *backup_file_name) {

  memset(index_table, 0, table_size);

  bool load_result = false;

  if (use_new_format) {

    load_result = Z2S_loadFileWithCRC(file_name, index_table, table_size);
    
    if(backup_file_name && (!load_result)) {

      log_e(
        "Index table (%s) not found - loading backup file (%s)", 
        file_name, backup_file_name);

      load_result = Z2S_loadFileWithCRC(
        backup_file_name, index_table, table_size);
      
      if (load_result) {

        Z2S_saveFileWithCRC(file_name, index_table, table_size);
      }
    }
  }
  else
    load_result = Z2S_loadFile(file_name, index_table, table_size);

  if (load_result) {

    log_i("Index table (%s) load SUCCESS!", file_name);
    return true;
  } 
  else {

    bool save_result = false;

    if (use_new_format)
      save_result = Z2S_saveFileWithCRC(file_name, index_table, table_size);
    else
      save_result = Z2S_saveFile(file_name, index_table, table_size);
  
    if (save_result) {

      log_i(
        "Index table (%s) not found - writing new one: SUCCESS!", file_name);
      return true;
    } else {
    
      log_e(
        "Index table (%s) not found - writing new one: FAILED!", file_name);
    return false;
    }
  }
}

/*****************************************************************************/

bool Z2S_saveIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name, 
  bool use_new_format, const char *backup_file_name) {

  bool save_result = true;

  if (use_new_format) {

      if (backup_file_name)
        Z2S_renameFile(file_name, backup_file_name);
      save_result = Z2S_saveFileWithCRC(file_name, index_table, table_size);
  }
  else
    save_result = Z2S_saveFile(file_name, index_table, table_size);

  if (save_result) {

    log_i("Saving index table (%s): SUCCESS!", file_name);
    return true;

  } else {
    
    log_i ("Saving index table (%s): FAILED!", file_name);
    return false;
  }
}

/*****************************************************************************/

uint16_t Z2S_getIndexTableEntriesNumber(
  uint8_t *index_table, uint16_t max_index) {

  uint16_t entries_number = 0;

  for (uint16_t index = 0; index < max_index; index++)
    if (checkIndexTablePosition(index_table, index, max_index))
      entries_number++;
  
  return entries_number;
}

/*****************************************************************************/

int16_t Z2S_getIndexTablePositionCounter(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index) {

  if (!checkIndexTablePosition(index_table, index_position, max_index))
      return -1;
  
  uint16_t position_counter = 0;

  for (uint16_t index = 0; index < max_index; index++) {
    
    if (index_position == index)
      return position_counter + 1;

    if (checkIndexTablePosition(index_table, index, max_index))
      position_counter++;
  }
  return -1;
}

/*****************************************************************************/

int16_t Z2S_findFreeEntryIndex(uint8_t *index_table, uint16_t max_index) {

  for (uint16_t index = 0; index < max_index; index++)
    if (!checkIndexTablePosition(index_table, index, max_index))
      return index;
  
  return -1; 
}

/*****************************************************************************/

int16_t Z2S_findNextIndexPosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index) {

  for (uint16_t index = index_position; index < max_index; index++)
    if (checkIndexTablePosition(index_table, index, max_index))
      return index;
  
  return -1;
}

/*****************************************************************************/

int16_t Z2S_findPrevIndexPosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index) {

  for (uint16_t index = index_position; index >= 0; index--)
    if (checkIndexTablePosition(index_table, index, max_index))
      return index;
  
  return -1;
}

/*****************************************************************************/

bool Z2S_saveObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data, 
  size_t object_size, bool use_new_format, 
  const char *backup_file_name_prefix) {

  char file_name_buffer[50] = {};
  char backup_file_name_buffer[50] = {};

  sprintf(file_name_buffer, file_name_prefix, object_index);
  if (backup_file_name_prefix)
    sprintf(backup_file_name_buffer, backup_file_name_prefix, object_index);

  bool save_result = false;

  if (use_new_format) {

    if (backup_file_name_prefix)
        Z2S_renameFile(file_name_buffer, backup_file_name_buffer);

    save_result = Z2S_saveFileWithCRC(
      file_name_buffer, object_data, object_size);
  }
  else
    save_result = Z2S_saveFile(
      file_name_buffer, object_data, object_size);
  
  if (save_result) {

    log_i(
      "Saving object in file %s: SUCCESS", file_name_buffer);

   return true;
  } 
  else {

    log_i(
      "Saving object in file %s: FAILED", file_name_buffer);
    return false;
  }
}

/*****************************************************************************/

bool Z2S_loadObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data, 
  size_t object_size, bool use_new_format, 
  const char *backup_file_name_prefix) {

  char file_name_buffer[50] = {};
  char backup_file_name_buffer[50] = {};
  
  sprintf(file_name_buffer, file_name_prefix, object_index);
  if (backup_file_name_prefix)
    sprintf(backup_file_name_buffer, backup_file_name_prefix, object_index);

  bool load_result = false;

  if (use_new_format) {
    
    load_result = Z2S_loadFileWithCRC(
      file_name_buffer, object_data, object_size);

    if(backup_file_name_prefix && (!load_result)) {

      log_e(
        "Object data file (%s) not found - loading backup file (%s)", 
        file_name_buffer, backup_file_name_buffer);

      load_result = Z2S_loadFileWithCRC(
        backup_file_name_buffer, object_data, object_size);
      
      if (load_result) {

        Z2S_saveFileWithCRC(file_name_buffer, object_data, object_size);
      }
    }
  }
  else
    load_result = Z2S_loadFile(
      file_name_buffer, object_data, object_size);
  
  if (load_result) {

    log_i(
      "Loading object from file %s: SUCCESS", file_name_buffer);
   return true;
  } 
  else {

    log_i(
      "Loading object from file %s: FAILED", file_name_buffer);
    return false;
  }
}

/*****************************************************************************/

bool Z2S_removeObject(
  uint16_t object_index, const char *file_name_prefix, 
  const char *backup_file_name_prefix) {

  char file_name_buffer[50] = {};
  char backup_file_name_buffer[50] = {};

  sprintf(file_name_buffer, file_name_prefix, object_index);
  if (backup_file_name_prefix)
    sprintf(backup_file_name_buffer, backup_file_name_prefix, object_index);
  
  if (Z2S_deleteFile(file_name_buffer)) {

    if (backup_file_name_prefix)
      Z2S_deleteFile(backup_file_name_buffer);
    
    log_i("Removing object file(s) %s: SUCCESS", file_name_buffer);
    return true;
  }
  else {

    log_i("Removing object file %s: FAILED", file_name_buffer);
    return false;
  }
}

/*****************************************************************************/

bool checkElementsIndexTablePosition(uint16_t index_position) {

  return checkIndexTablePosition(
    z2s_elements_index_table, index_position, Z2S_ELEMENTS_MAX_NUMBER);
}

/*****************************************************************************/

bool setElementsIndexTablePosition(uint16_t index_position) {

  return setIndexTablePosition(
    z2s_elements_index_table, index_position, Z2S_ELEMENTS_MAX_NUMBER);
}

/*****************************************************************************/

bool clearElementsIndexTablePosition(uint16_t index_position) {

  return clearIndexTablePosition(
    z2s_elements_index_table, index_position, Z2S_ELEMENTS_MAX_NUMBER);
}

/*****************************************************************************/

bool Z2S_loadElementsIndexTable() {

  return Z2S_loadIndexTable(
    z2s_elements_index_table, sizeof(z2s_elements_index_table), 
    Z2S_ELEMENTS_INDEX_TABLE_V3, true, Z2S_ELEMENTS_INDEX_TABLE_BACKUP_V3);
}

/*****************************************************************************/

bool Z2S_saveElementsIndexTable() {

  return Z2S_saveIndexTable(
    z2s_elements_index_table, sizeof(z2s_elements_index_table), 
    Z2S_ELEMENTS_INDEX_TABLE_V3, true, Z2S_ELEMENTS_INDEX_TABLE_BACKUP_V3);
}

/*****************************************************************************/

uint16_t Z2S_getElementsNumber() {

  return Z2S_getIndexTableEntriesNumber(
    z2s_elements_index_table, Z2S_ELEMENTS_MAX_NUMBER);
}

/****************************************************************************/

int16_t  Z2S_getElementCounter(uint16_t element_position) {

  return Z2S_getIndexTablePositionCounter(
    z2s_elements_index_table, element_position, Z2S_ELEMENTS_MAX_NUMBER);
}
  
/*****************************************************************************/  

int16_t  Z2S_findFreeChannelIndex() {

  return Z2S_findFreeEntryIndex(
    z2s_elements_index_table, Z2S_CHANNELS_MAX_NUMBER);
}

/*****************************************************************************/

int16_t  Z2S_findNextElementPosition(uint16_t element_position) {

  return Z2S_findNextIndexPosition(
    z2s_elements_index_table, element_position, Z2S_ELEMENTS_MAX_NUMBER);
}

/*****************************************************************************/

int16_t  Z2S_findPrevElementPosition(uint16_t element_position) {

return Z2S_findPrevIndexPosition(
    z2s_elements_index_table, element_position, Z2S_ELEMENTS_MAX_NUMBER);
}

/*****************************************************************************/

bool Z2S_saveElement(uint16_t element_index, z2s_device_params_t &element) {

  if (element_index >= Z2S_ELEMENTS_MAX_NUMBER)
    return false;

  if (Z2S_saveObject(element_index, Z2S_ELEMENTS_PREFIX_V3, 
        (uint8_t*) &element, sizeof(z2s_device_params_t), true,
        Z2S_ELEMENTS_BACKUP_PREFIX_V3)) {
    
    setElementsIndexTablePosition(element_index);
    return Z2S_saveElementsIndexTable();
  }
  return false;
}

/*****************************************************************************/

bool Z2S_loadElement(uint16_t element_index, z2s_device_params_t &element) {

  if (element_index >= Z2S_ELEMENTS_MAX_NUMBER)
    return false;

  return Z2S_loadObject(element_index, Z2S_ELEMENTS_PREFIX_V3, 
    (uint8_t*) &element, sizeof(z2s_device_params_t), true,
    Z2S_ELEMENTS_BACKUP_PREFIX_V3);
}

/*****************************************************************************/

bool Z2S_removeElement(uint16_t element_index) {

  if (element_index >= Z2S_ELEMENTS_MAX_NUMBER)
    return false;

   if (Z2S_removeObject(element_index, Z2S_ELEMENTS_PREFIX_V3)) {
  
    clearElementsIndexTablePosition(element_index);
    Z2S_saveElementsIndexTable();
    return true;
  }
  return false;
}

/*****************************************************************************/
/*****************************************************************************/

bool Z2S_Core::fillZ2SChannelStruct(z2s_device_params_t& z2s_channel) {

  memcpy(&z2s_channel, &_z2s_channel, sizeof(z2s_device_params_t));
  return true;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByChannelIndex(int16_t channel_index) {

  if (channel_index < 0)
    return nullptr;

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByChannelNumber(uint8_t channel_number) {

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByZbDeviceId(uint8_t Zb_device_id) {

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Zb_device_id == Zb_device_id)
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/


Supla::Element *Z2S_Core::getZ2SElementByChannelIndex(int16_t channel_index) {

  if (channel_index < 0)
    return nullptr;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core->_z2s_element;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Supla::Element *Z2S_Core::getZ2SElementByChannelNumber(
  uint8_t channel_number) {

  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER) 
    return &GatewayEventsInstance;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core->_z2s_element;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Supla::Element *Z2S_Core::getZ2SElementByZbDeviceId(uint8_t Zb_device_id) {

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Zb_device_id == Zb_device_id)
      return z2s_core->_z2s_element;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

uint8_t Z2S_Core::updateZ2SChannelsByZbDeviceId(
  uint8_t Zb_device_id, uint8_t rssi_percentage, uint8_t battery_level) {

  uint8_t channels_number = 0;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Zb_device_id == Zb_device_id) {     

      Supla::Channel *channel = z2s_core->_z2s_element->getChannel();

      if (channel) {

        channel->setStateOnline();
        channel->setBridgeSignalStrength(rssi_percentage);
        if ((battery_level < 0xFF) &&
          ((z2s_core->_z2s_channel.user_data_flags &
            USER_DATA_FLAG_IGNORE_CHANNEL_BATTERY_LEVEL) == 0)) 

          channel->setBatteryLevel(battery_level);
      }
      else
        {

        }
      channels_number++;
    }
    core_it++;
  }
  return channels_number;
}

/*****************************************************************************/

uint8_t Z2S_Core::countZ2SChannelsByZbDeviceId(uint8_t Zb_device_id) {

  uint8_t channels_number = 0;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    if ((*core_it)->_z2s_channel.Zb_device_id == Zb_device_id) {
      
      channels_number++;
    }
    core_it++;
  }
  return channels_number;
}

/*****************************************************************************/

int16_t Z2S_Core::getZ2SChannelIndexByChannelNumber(uint8_t channel_number) {

  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER)
    return GATEWAY_EVENTS_CHANNEL_INDEX;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core->_channel_index;

    core_it++;
  }
  return -1;
}

/*****************************************************************************/

uint8_t Z2S_Core::getZ2SChannelNumberByChannelIndex(int16_t channel_index) {

  if (channel_index == GATEWAY_EVENTS_CHANNEL_INDEX)
     return GATEWAY_EVENTS_CHANNEL_NUMBER;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core->_z2s_channel.Supla_channel;

    core_it++;
  }
  return 0xFF;
}

/*****************************************************************************/

const char *Z2S_Core::getZ2SChannelNameByChannelNumber(
  uint8_t channel_number) {
    
  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER)
    return gateway_events_channel_name;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core->_z2s_channel.Supla_channel_name;

    core_it++;
  }
  return "missing channel name!";
}

/*****************************************************************************/

const char *Z2S_Core::getZ2SChannelNameByChannelIndex(int16_t channel_index) {
    
  if (channel_index == GATEWAY_EVENTS_CHANNEL_INDEX)
    return gateway_events_channel_name;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core->_z2s_channel.Supla_channel_name;

    core_it++;
  }
  return "missing channel name!";
}

/*****************************************************************************/

Supla::Control::SwitchBotRelay *Z2S_Core::getSwitchBotRelayInstance(
  int16_t channel_index, uint8_t channel_number) {

  auto core_it = Z2S_Cores.begin();

  bool core_found = false;
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (channel_index < 0) {

      if ((z2s_core->_z2s_channel.Supla_channel == channel_number) &&
          ((z2s_core->_z2s_channel.local_channel_type == 
            LOCAL_CHANNEL_TYPE_SWITCHBOT)))
       return static_cast<
        Supla::Control::SwitchBotRelay *>(z2s_core->_z2s_element);    
    }
    else {

      if ((z2s_core->_channel_index == channel_index) &&
          ((z2s_core->_z2s_channel.local_channel_type == 
            LOCAL_CHANNEL_TYPE_SWITCHBOT)))
        return static_cast<
          Supla::Control::SwitchBotRelay *>(z2s_core->_z2s_element);    
    }
    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

void Z2S_Core::updateZ2SCoresShortAddress(
  uint16_t prev_addr, uint16_t new_addr){

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.short_addr == prev_addr) {

      z2s_core->_z2s_channel.short_addr = new_addr;
      z2s_core->saveChannelData();
    }

    core_it++;
  }
}

/*****************************************************************************/

Supla::Control::Z2S_RemoteRelay *Z2S_Core::getRemoteRelayPtr() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_REMOTE_RELAY)
    return static_cast<
      Supla::Control::Z2S_RemoteRelay *>(_z2s_element);
  else
    return nullptr;
}

/*****************************************************************************/

Supla::Control::HvacBaseEE *Z2S_Core::getHvacPtr() {

  if (_z2s_channel.Supla_channel_type == SUPLA_CHANNELTYPE_HVAC) {

    Supla::Control::Z2S_TRVInterface *Supla_Z2S_TRVInterface = static_cast<
      Supla::Control::Z2S_TRVInterface *>(_z2s_element);
      
    return Supla_Z2S_TRVInterface->getTRVHvac();
  }
  return nullptr;
}

/*****************************************************************************/

bool Z2S_Core::setMDNSName(const char *mDNS_name) {
  
  if (mDNS_name) {

    // cut "mdns://"
    memcpy(_z2s_channel.remote_channel_data.mDNS_name, mDNS_name + 7, 11);
      _z2s_channel.remote_channel_data.mDNS_name[11] = '\0';
      
    return saveChannelData();
  }
  else
    return false;
}

/*****************************************************************************/

bool Z2S_Core::setRemoteIPAddress(uint32_t remote_ip_address) {
  
  _z2s_channel.remote_channel_data.remote_ip_address = remote_ip_address;
            
  return saveChannelData();
}

/*****************************************************************************/

bool Z2S_Core::setSuplaRemoteChannel(uint8_t Supla_remote_channel) {

  _z2s_channel.Supla_remote_channel = Supla_remote_channel;
                
  return saveChannelData();
}

/*****************************************************************************/

bool Z2S_Core::isInSeconds() {

  switch (_z2s_channel.Supla_channel_type) {


    case SUPLA_CHANNELTYPE_ACTIONTRIGGER:
      return false;


    default:
      return true;
  }
}

/*****************************************************************************/

bool Z2S_Core::initZ2SChannelExtendedDataCounter() {

  

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len > 0) &&
        (extended_data_counter_str_len < 8))
      return false;
  }
  sprintf(
    _z2s_channel.extended_data_counter, "EDC_%03U", _channel_index);

  return setChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER, true);
}

/*****************************************************************************/

bool Z2S_Core::setChannelExtendedDataCounter(uint64_t extended_data_counter) {

  
  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len == 0) &&
        (extended_data_counter_str_len == 8))
      return false;

    return Z2S_GatewayPreferences.putULong64(
      _z2s_channel.extended_data_counter, extended_data_counter);
  } 
  else {

    _z2s_channel.data_counter = extended_data_counter;
    return saveChannelData();
  }
}

/*****************************************************************************/

const char *Z2S_Core::getChannelExtendedDataCounterKey() {


  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
        _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len == 0) &&
        (extended_data_counter_str_len == 8))
      return invalid_extended_data_counter_key;

    return (const char*)_z2s_channel.extended_data_counter;
  }
  return no_extended_data_counter_key;
}

/*****************************************************************************/

bool Z2S_Core::removeChannelExtendedDataCounter() {

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len > 0) &&
        (extended_data_counter_str_len < 8))
      Z2S_GatewayPreferences.remove(_z2s_channel.extended_data_counter);

    return clearChannelUserDataFlags(
      USER_DATA_FLAG_EXTENDED_DATA_COUNTER, true);
  }
  return false;
}

/*****************************************************************************/

uint64_t Z2S_Core::getChannelExtendedDataCounter() {

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len == 0) &&
        (extended_data_counter_str_len == 8))
      return 0;
        
    return Z2S_GatewayPreferences.getULong64(
      _z2s_channel.extended_data_counter);
  }
  else
    return _z2s_channel.data_counter;
}

/*****************************************************************************/