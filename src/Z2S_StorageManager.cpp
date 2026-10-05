#include <Arduino.h>

#include <ZigbeeGateway.h>

#include "Z2S_StorageManager.h"
#include "z2s_little_fs.h"

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
