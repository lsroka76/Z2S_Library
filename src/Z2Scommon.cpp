/***********************************************************************************/

#include <cstring>

#include <Arduino.h>
#include <ZigbeeGateway.h>
#include <SuplaDevice.h>
#include <supla/element.h>

#include "Z2S_common.h"
#include "z2s_little_fs.h"

#include <Z2S_control/Z2S_remote_relay.h>

/***********************************************************************************/

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
  uint8_t *index_table, size_t table_size, const char *file_name) {

  memset(index_table, 0, table_size);

  if (Z2S_loadFile(file_name, index_table, table_size)) {

    log_i("Index table (%s) load SUCCESS!", file_name);
    return true;
  } 
  else {
  
    if (Z2S_saveFile(file_name, index_table, table_size)) {

      log_i(
        "Index table (%s) not found - writing new one: SUCCESS!", file_name);
      return true;
    } else {
    
      log_i(
        "Index table (%s) not found - writing new one: FAILED!", file_name);
    return false;
    }
  }
}

/*****************************************************************************/

bool Z2S_saveIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name) {

  if (Z2S_saveFile(file_name, index_table, table_size)) {

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
  size_t object_size) {

  char file_name_buffer[50] = {};
  sprintf(file_name_buffer, file_name_prefix, object_index);
  
  if (Z2S_saveFile(file_name_buffer, object_data, object_size)) {

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
  size_t object_size) {

  char file_name_buffer[50] = {};
  
  sprintf(file_name_buffer, file_name_prefix, object_index);
  
  if (Z2S_loadFile(file_name_buffer, object_data, object_size)) {

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

bool Z2S_removeObject(uint16_t object_index, const char *file_name_prefix) {

  char file_name_buffer[50] = {};

  sprintf(file_name_buffer, file_name_prefix, object_index);
  
  if (Z2S_deleteFile(file_name_buffer)) {
    
    log_i("Removing object file %s: SUCCESS", file_name_buffer);
    return true;
  }
  else {

    log_i("Removing object file %s: FAILED", file_name_buffer);
    return false;
  }
}

/***********************************************************************************/

bool Z2S_Core::fillZ2SChannelStruct(z2s_device_params_t& z2s_channel) {

  if (_z2s_channel) {

    memcpy(&z2s_channel, _z2s_channel, sizeof(z2s_device_params_t));
    return true;
  }
  return false;
}

/***********************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByChannelIndex(int16_t channel_index) {

  if (channel_index < 0)
    return nullptr;

  auto core_it = Z2S_Cores.begin();
    while (core_it != Z2S_Cores.end()) {

      if ((*core_it)->_channel_index == channel_index)
        return (*core_it);

      core_it++;
    }
    return nullptr;
}

/***********************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreBySuplaChannelNumber(uint8_t Supla_channel_number) {

  auto core_it = Z2S_Cores.begin();
    while (core_it != Z2S_Cores.end()) {

      if ((*core_it)->_z2s_channel->Supla_channel == Supla_channel_number)
        return (*core_it);

      core_it++;
    }
    return nullptr;
}

/***********************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByZbDeviceId(uint8_t Zb_device_id) {

  auto core_it = Z2S_Cores.begin();
    while (core_it != Z2S_Cores.end()) {

      if ((*core_it)->_z2s_channel->Zb_device_id == Zb_device_id)
        return (*core_it);

      core_it++;
    }
    return nullptr;
}


/***********************************************************************************/

bool Z2S_Core::setMDNSName(const char *mDNS_name) {
  
    if (_z2s_channel && mDNS_name) {

      // cut "mdns://"
      memcpy(_z2s_channel->remote_channel_data.mDNS_name, mDNS_name + 7, 11);
      _z2s_channel->remote_channel_data.mDNS_name[11] = '\0';
      
      if ((getZ2SLocalChannelType() == LOCAL_CHANNEL_TYPE_REMOTE_RELAY) &&
          (_z2s_element->getChannel()->getChannelType() == 
           SUPLA_CHANNELTYPE_RELAY)) {

        auto Supla_Z2S_RemoteRelay = static_cast<
          Supla::Control::Z2S_RemoteRelay *>(_z2s_element);
        Supla_Z2S_RemoteRelay->setRemoteGatewayMDNSName(mDNS_name);  
      }
      return true;
    }
    else
      return false;
  }

  /***********************************************************************************/

  bool Z2S_Core::setRemoteIPAddress(uint32_t remote_ip_address) {
  
    if (_z2s_channel) {

      _z2s_channel->remote_channel_data.remote_ip_address = remote_ip_address;

      if ((getZ2SLocalChannelType() == LOCAL_CHANNEL_TYPE_REMOTE_RELAY) &&
          (_z2s_element->getChannel()->getChannelType() == 
           SUPLA_CHANNELTYPE_RELAY)) {
            
        auto Supla_Z2S_RemoteRelay = static_cast<
          Supla::Control::Z2S_RemoteRelay *>(_z2s_element);
        Supla_Z2S_RemoteRelay->setRemoteGatewayIPAddress(remote_ip_address);  
      }

      return true;
    }
    else
      return false;
  }