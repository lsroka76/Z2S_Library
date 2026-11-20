#ifndef Z2S_LITTLE_FS_H_
#define Z2S_LITTLE_FS_H_

#include <stddef.h>
#include <stdint.h>

/*****************************************************************************/

enum class Z2S_FsLoadResult : uint8_t {
  OK = 0,
  NOT_FOUND,
  CORRUPT,
  IO_ERROR,
  MUTEX_TIMEOUT
};

/*****************************************************************************/

bool Z2S_initLittleFs(bool allow_format = false);

bool Z2S_formatLittleFs();

/*****************************************************************************/

void Z2S_endLittleFs(bool force_end = false);

bool Z2S_lockFs(uint32_t timeout_ms = 2000);
void Z2S_unlockFs();

/*****************************************************************************/

bool Z2S_saveFile(
  const char *z2s_file_name, const uint8_t *z2s_file_data,
  size_t z2s_file_size);

/*****************************************************************************/

bool Z2S_loadFile(
  const char *z2s_file_name, uint8_t *z2s_file_data,
  size_t z2s_file_size);

Z2S_FsLoadResult Z2S_loadFileEx(
  const char *z2s_file_name, uint8_t *z2s_file_data,
  size_t z2s_file_size);

/*****************************************************************************/

bool Z2S_validateFileWithCRC(const char *z2s_file_name);

/*****************************************************************************/

bool Z2S_saveFileWithCRC(
  const char *z2s_file_name, const uint8_t *z2s_file_data,
  size_t z2s_file_size);

bool Z2S_saveFileWithCRCAndBackup(
  const char *z2s_file_name, const uint8_t *z2s_file_data,
  size_t z2s_file_size, const char *backup_file_name);

/*****************************************************************************/

bool Z2S_loadFileWithCRC(
  const char *z2s_file_name, uint8_t *z2s_file_data,
  size_t z2s_file_size);

Z2S_FsLoadResult Z2S_loadFileWithCRCEx(
  const char *z2s_file_name, uint8_t *z2s_file_data,
  size_t z2s_file_size);

/*****************************************************************************/

size_t Z2S_getFileSizeWithCRC(
  const char *z2s_file_name, Z2S_FsLoadResult *status = nullptr);

/*****************************************************************************/

size_t Z2S_loadBufferFromFile(
  const char *z2s_file_name, size_t z2s_file_offset,
  size_t z2s_buffer_size, uint8_t *z2s_buffer_data);

/*****************************************************************************/

bool Z2S_deleteFile(const char *z2s_file_name);

/*****************************************************************************/

bool Z2S_renameFile(const char *z2s_file_name, const char *z2s_new_file_name);

/*****************************************************************************/

int Z2S_getFileSize(
  const char *z2s_file_name, bool init_fs = true, bool full_path = false);

// Returns: 1 = exists, 0 = does not exist, -1 = FS/mutex error
int8_t Z2S_fileExists(const char *z2s_file_name, bool full_path = false);

/*****************************************************************************/

#endif // Z2S_LITTLE_FS_H_