#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include <ZigbeeGateway.h>

#include "Z2S_StorageManager.h"
#include "z2s_little_fs.h"

/*****************************************************************************/

static constexpr size_t Z2S_OBJ_NAME_MAX = 80;

/*****************************************************************************/

bool checkIndexTablePosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index) {

  if (index_position >= max_index)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index  = index_position % 8;

  return (*(index_table + byte_index) & (1 << bit_index)) != 0;
}

/*****************************************************************************/

bool setIndexTablePosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index) {

  if (index_position >= max_index)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index  = index_position % 8;

  *(index_table + byte_index) |= (1 << bit_index);
  return true;
}

/*****************************************************************************/

bool clearIndexTablePosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index) {

  if (index_position >= max_index)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index  = index_position % 8;

  *(index_table + byte_index) &= ~(1 << bit_index);
  return true;
}

/*****************************************************************************/

bool Z2S_loadIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name,
  bool use_new_format, const char *backup_file_name) {

  memset(index_table, 0, table_size);

  bool load_result                = false;
  Z2S_FsLoadResult primary_status = Z2S_FsLoadResult::IO_ERROR;
  Z2S_FsLoadResult backup_status  = Z2S_FsLoadResult::NOT_FOUND;

  if (use_new_format) {
    size_t primary_size = Z2S_getFileSizeWithCRC(file_name, &primary_status);

    if (primary_status == Z2S_FsLoadResult::OK) {
      if (primary_size > 0 && primary_size <= table_size) {
        primary_status = Z2S_loadFileWithCRCEx(
          file_name, index_table, primary_size);
        load_result = (primary_status == Z2S_FsLoadResult::OK);
      } else {
        log_e("Index table (%s) size %u out of bounds (max %u)",
              file_name, (unsigned)primary_size, (unsigned)table_size);
        primary_status = Z2S_FsLoadResult::CORRUPT;
      }
    }

    if (backup_file_name && !load_result) {
      log_w("Index table (%s) load failed (status=%u) - trying backup (%s)",
            file_name, (unsigned)primary_status, backup_file_name);

      size_t backup_size = Z2S_getFileSizeWithCRC(
        backup_file_name, &backup_status);

      if (backup_status == Z2S_FsLoadResult::OK) {
        if (backup_size > 0 && backup_size <= table_size) {
          backup_status = Z2S_loadFileWithCRCEx(
            backup_file_name, index_table, backup_size);
          load_result = (backup_status == Z2S_FsLoadResult::OK);
        } else {
          log_e("Backup index table (%s) size %u out of bounds (max %u)",
                backup_file_name, (unsigned)backup_size, (unsigned)table_size);
          backup_status = Z2S_FsLoadResult::CORRUPT;
        }
      }

      if (load_result) {
        log_i("Index table recovered from backup (%s) - restoring (%s)",
              backup_file_name, file_name);
        Z2S_saveFileWithCRC(file_name, index_table, table_size);
      }
    }
  } else {
    primary_status = Z2S_loadFileEx(file_name, index_table, table_size);
    load_result    = (primary_status == Z2S_FsLoadResult::OK);
  }

  if (load_result) {
    log_i("Index table (%s) load SUCCESS!", file_name);
    return true;
  }

  // Only create a brand-new empty table when neither primary nor backup exists.
  // Never overwrite existing data on mutex timeout, I/O error, or corruption.
  bool genuinely_missing =
    (primary_status == Z2S_FsLoadResult::NOT_FOUND) &&
    (!use_new_format || !backup_file_name ||
     backup_status == Z2S_FsLoadResult::NOT_FOUND);

  if (!genuinely_missing) {
    log_e("Index table (%s) FAILED (primary=%u, backup=%u) - "
          "refusing to overwrite with blank table!",
          file_name, (unsigned)primary_status, (unsigned)backup_status);
    return false;
  }

  bool save_result = use_new_format
    ? Z2S_saveFileWithCRC(file_name, index_table, table_size)
    : Z2S_saveFile(file_name, index_table, table_size);

  if (save_result) {
    log_i("Index table (%s) not found - writing new one: SUCCESS!", file_name);
    return true;
  }

  log_e("Index table (%s) not found - writing new one: FAILED!", file_name);
  return false;
}

/*****************************************************************************/

bool Z2S_saveIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name,
  bool use_new_format, const char *backup_file_name) {

  bool save_result = use_new_format
    ? Z2S_saveFileWithCRCAndBackup(
        file_name, index_table, table_size, backup_file_name)
    : Z2S_saveFile(file_name, index_table, table_size);

  if (save_result) {
    log_i("Saving index table (%s): SUCCESS!", file_name);
    return true;
  }

  log_e("Saving index table (%s): FAILED!", file_name);
  return false;
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

int16_t Z2S_getIndexTableCounterPosition(
  uint8_t *index_table, uint16_t counter, uint16_t max_index) {

  if (counter == 0)
    return -1;

  uint16_t current_counter = 0;

  for (uint16_t index = 0; index < max_index; index++) {
    if (checkIndexTablePosition(index_table, index, max_index)) {
      current_counter++;

      if (current_counter == counter) {
        return index;
      }
    }
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

  if (max_index == 0)
    return -1;

  int32_t start_idx = (index_position >= max_index)
                        ? (max_index - 1)
                        : index_position;

  for (int32_t index = start_idx; index >= 0; index--) {
    if (checkIndexTablePosition(index_table, (uint16_t)index, max_index))
      return (int16_t)index;
  }
  return -1;
}

/*****************************************************************************/

bool Z2S_saveObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data,
  size_t object_size, bool use_new_format,
  const char *backup_file_name_prefix) {

  char file_name_buffer[Z2S_OBJ_NAME_MAX]        = {};
  char backup_file_name_buffer[Z2S_OBJ_NAME_MAX] = {};

  int n = snprintf(
    file_name_buffer, sizeof(file_name_buffer), file_name_prefix, object_index);
  if (n < 0 || static_cast<size_t>(n) >= sizeof(file_name_buffer)) {
    log_e("Object filename truncated for index %u", object_index);
    return false;
  }

  const char *backup_name_ptr = nullptr;
  if (backup_file_name_prefix) {
    int nb = snprintf(
      backup_file_name_buffer, sizeof(backup_file_name_buffer),
      backup_file_name_prefix, object_index);
    if (nb < 0 || static_cast<size_t>(nb) >= sizeof(backup_file_name_buffer)) {
      log_e("Backup object filename truncated for index %u", object_index);
      return false;
    }
    backup_name_ptr = backup_file_name_buffer;
  }

  bool save_result = use_new_format
    ? Z2S_saveFileWithCRCAndBackup(
        file_name_buffer, object_data, object_size, backup_name_ptr)
    : Z2S_saveFile(file_name_buffer, object_data, object_size);

  if (save_result) {
    log_i("Saving object in file %s: SUCCESS", file_name_buffer);
    return true;
  }

  log_e("Saving object in file %s: FAILED", file_name_buffer);
  return false;
}

/*****************************************************************************/

bool Z2S_loadObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data,
  size_t object_size, bool use_new_format,
  const char *backup_file_name_prefix) {

  char file_name_buffer[Z2S_OBJ_NAME_MAX]        = {};
  char backup_file_name_buffer[Z2S_OBJ_NAME_MAX] = {};

  int n = snprintf(
    file_name_buffer, sizeof(file_name_buffer), file_name_prefix, object_index);
  if (n < 0 || static_cast<size_t>(n) >= sizeof(file_name_buffer)) {
    return false;
  }

  bool has_backup = false;
  if (backup_file_name_prefix) {
    int nb = snprintf(
      backup_file_name_buffer, sizeof(backup_file_name_buffer),
      backup_file_name_prefix, object_index);
    if (nb >= 0 && static_cast<size_t>(nb) < sizeof(backup_file_name_buffer)) {
      has_backup = true;
    }
  }

  bool load_result = false;

  if (use_new_format) {
    Z2S_FsLoadResult res = Z2S_loadFileWithCRCEx(
      file_name_buffer, object_data, object_size);
    load_result = (res == Z2S_FsLoadResult::OK);

    if (has_backup && !load_result &&
        res != Z2S_FsLoadResult::MUTEX_TIMEOUT) {
      log_w("Object file (%s) failed (status=%u) - loading backup (%s)",
            file_name_buffer, (unsigned)res, backup_file_name_buffer);

      Z2S_FsLoadResult bak_res = Z2S_loadFileWithCRCEx(
        backup_file_name_buffer, object_data, object_size);
      load_result = (bak_res == Z2S_FsLoadResult::OK);

      if (load_result) {
        Z2S_saveFileWithCRC(file_name_buffer, object_data, object_size);
      }
    }
  } else {
    load_result = Z2S_loadFile(file_name_buffer, object_data, object_size);
  }

  if (load_result) {
    log_i("Loading object from file %s: SUCCESS", file_name_buffer);
    return true;
  }

  log_e("Loading object from file %s: FAILED", file_name_buffer);
  return false;
}

/*****************************************************************************/

bool Z2S_removeObject(
  uint16_t object_index, const char *file_name_prefix,
  const char *backup_file_name_prefix) {

  char file_name_buffer[Z2S_OBJ_NAME_MAX]        = {};
  char backup_file_name_buffer[Z2S_OBJ_NAME_MAX] = {};

  int n = snprintf(
    file_name_buffer, sizeof(file_name_buffer), file_name_prefix, object_index);
  if (n < 0 || static_cast<size_t>(n) >= sizeof(file_name_buffer)) {
    return false;
  }

  bool primary_ok = true;
  int8_t primary_exists = Z2S_fileExists(file_name_buffer);
  if (primary_exists < 0) {
    primary_ok = false;
  } else if (primary_exists == 1) {
    primary_ok = Z2S_deleteFile(file_name_buffer);
  }

  // Always clean up any leftover .bak file even if primary was already gone.
  bool backup_ok = true;
  if (backup_file_name_prefix) {
    int nb = snprintf(
      backup_file_name_buffer, sizeof(backup_file_name_buffer),
      backup_file_name_prefix, object_index);
    if (nb >= 0 && static_cast<size_t>(nb) < sizeof(backup_file_name_buffer)) {
      int8_t bak_exists = Z2S_fileExists(backup_file_name_buffer);
      if (bak_exists < 0) {
        backup_ok = false;
      } else if (bak_exists == 1) {
        backup_ok = Z2S_deleteFile(backup_file_name_buffer);
      }
    }
  }

  if (primary_ok && backup_ok) {
    log_i("Removing object file(s) %s: SUCCESS", file_name_buffer);
    return true;
  }

  log_e("Removing object file(s) %s: FAILED", file_name_buffer);
  return false;
}