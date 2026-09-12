#ifndef Z2S_LITTLE_FS_H_
#define Z2S_LITTLE_FS_H_

/*****************************************************************************/

bool Z2S_initLittleFs();

/*****************************************************************************/

void Z2S_endLittleFs(bool force_end = false);

/*****************************************************************************/

bool Z2S_saveFile(
  const char *z2s_file_name, const uint8_t *z2s_file_data, 
  size_t z2s_file_size);

/*****************************************************************************/

bool Z2S_loadFile(
  const char *z2s_file_name, uint8_t *z2s_file_data, 
  size_t z2s_file_size);

/*****************************************************************************/

bool Z2S_validateFileWithCRC(const char *z2s_file_name);

/*****************************************************************************/

bool Z2S_saveFileWithCRC(
  const char *z2s_file_name, const uint8_t *z2s_file_data, 
  size_t z2s_file_size);

/*****************************************************************************/

bool Z2S_loadFileWithCRC(
  const char *z2s_file_name, uint8_t *z2s_file_data, 
  size_t z2s_file_size);

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

/*****************************************************************************/

#endif //Z2S_LITTLE_FS_H_