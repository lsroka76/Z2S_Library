#include <LittleFS.h>
#include <esp_rom_crc.h>

#include "z2s_little_fs.h"

/*****************************************************************************/

struct FileHeader {
  uint32_t magic;
  uint16_t version;
  uint32_t dataSize;
  uint32_t crc32;
};

static constexpr uint32_t FILE_MAGIC = 0x5344425A; // "ZBDS"

/*****************************************************************************/

extern SemaphoreHandle_t saveMutex;

extern bool _restart_scheduled;

/*****************************************************************************/

bool Z2S_initLittleFs() {

  //if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    bool result = LittleFS.begin();
  
    if (!result) {
    
      log_w("Z2S LittleFs: formatting partition");
    
      LittleFS.format();
    
      result = LittleFS.begin();
      if (!result) {
      
        log_e("Z2S LittleFs: failed to mount and to format partition");
      }
    }
    return result;
  }
  
/*****************************************************************************/

void Z2S_endLittleFs(bool force_end) {

  if (force_end) {

    LittleFS.end();
    xSemaphoreTake(saveMutex, portMAX_DELAY);
  }
  else
    xSemaphoreGive(saveMutex);
}

/*****************************************************************************/

bool Z2S_saveFile(
  const char* z2s_file_name, const uint8_t* z2s_file_data, 
  size_t z2s_file_size) {

  log_d("Z2S LittleFs: saving file %s", z2s_file_name);

  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
    
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);

  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    LittleFS.mkdir("/z2s_gateway");

    File file = LittleFS.open(file_name, "w");
    
    if (!file) {
    
      log_e(
        "Z2S LittleFs: failed to open file \"%s\" for read/write", 
        z2s_file_name);
      
      Z2S_endLittleFs();
      return false;
    }

    file.write(z2s_file_data, z2s_file_size);
    file.close();
    Z2S_endLittleFs();
    return true;
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

bool Z2S_saveFileWithCRC(
  const char* z2s_file_name, const uint8_t* z2s_file_data, 
  size_t z2s_file_size) {

  log_d("Z2S LittleFs: saving file %s - new format", z2s_file_name);

  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
    
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);

  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    LittleFS.mkdir("/z2s_gateway");

    uint32_t crc32 = esp_rom_crc32_le(
      0xFFFFFFFF, z2s_file_data, z2s_file_size);

    FileHeader hdr;
    hdr.magic = FILE_MAGIC;
    hdr.version = 1;
    hdr.dataSize = z2s_file_size;
    hdr.crc32 = crc32;

    File file = LittleFS.open(file_name, "w");
    
    if (!file) {
    
      log_e(
        "Z2S LittleFs: failed to open file \"%s\" for read/write!", 
        z2s_file_name);
      
      Z2S_endLittleFs();
      return false;
    }

    if (file.write((uint8_t*)&hdr, sizeof(hdr)) != sizeof(hdr)) {

      log_e(
        "Z2S LittleFs: error while writing header for file \"%s\"!", 
        z2s_file_name);
      
      file.close();
      Z2S_endLittleFs();
      return false;
    }

    if (file.write(z2s_file_data, z2s_file_size) != z2s_file_size) {

      log_e(
        "Z2S LittleFs: error while writing data for file \"%s\"!", 
        z2s_file_name);

      file.close();
      Z2S_endLittleFs();
      return false;
    }

    file.close();
    Z2S_endLittleFs();
    return true;
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

bool Z2S_loadFile(
  const char* z2s_file_name, uint8_t* z2s_file_data, size_t z2s_file_size) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);

  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
  
    File file = LittleFS.open(file_name, "r");
  
    if (!file) {

      log_e(
        "Z2S LittleFs: failed to open file \"%s\" for read", z2s_file_name);
      Z2S_endLittleFs();
      return false;
    }
  
    size_t file_size = file.size();

    if (file_size > z2s_file_size) {

      log_e("Z2S LittleFs: file size exceeds buffer size!");

      file.close();
      Z2S_endLittleFs();
      return false;
    }
  
    if (file_size != z2s_file_size) {

      log_e("Z2S LittleFs: invalid file size");

      file.close();
      Z2S_endLittleFs();
      return false;
    }

    int bytesRead = file.read(z2s_file_data, file_size);

    file.close();
    Z2S_endLittleFs();
    return bytesRead == file_size;
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

bool Z2S_validateFileWithCRC(const char* z2s_file_name) {

  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);

  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
  
    File file = LittleFS.open(file_name, "r");

    if (!file) {

      log_e(
        "Z2S LittleFs: failed to open file \"%s\" for read!", z2s_file_name);
      
      Z2S_endLittleFs();
      return false;
    }

    FileHeader hdr;

    if (file.read((uint8_t*)&hdr, sizeof(hdr)) != sizeof(hdr)) {

      log_e(
        "Z2S LittleFs: error while reading header for file \"%s\"!", 
        z2s_file_name);

      file.close();
      Z2S_endLittleFs();
      return false;
    }

    if (hdr.magic != FILE_MAGIC) {

      log_e(
        "Z2S LittleFs: invalid header magic for file \"%s\"!", 
        z2s_file_name);

      file.close();
      Z2S_endLittleFs();
      return false;
    }

    uint32_t crc32 = 0xFFFFFFFF;

    uint8_t buf[256];

    while (file.available()) {
        size_t n = file.read(buf, sizeof(buf));
        crc32 = esp_rom_crc32_le(crc32, buf, n);
    }

    file.close();
    Z2S_endLittleFs();

    return crc32 == hdr.crc32;
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

bool Z2S_loadFileWithCRC(
  const char* z2s_file_name, uint8_t* z2s_file_data, size_t z2s_file_size) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);

  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    File file = LittleFS.open(file_name, "r");
  
    if (!file) {

      log_e(
        "Z2S LittleFs: failed to open file \"%s\" for read", z2s_file_name);

      Z2S_endLittleFs();
      return false;
    }

    FileHeader hdr;

    if (file.read((uint8_t*)&hdr, sizeof(hdr)) != sizeof(hdr)) {

      log_e(
        "Z2S LittleFs: error while reading header for file \"%s\"!", 
        z2s_file_name);
    
      file.close();
      Z2S_endLittleFs();
      return false;
    }

    if (hdr.magic != FILE_MAGIC) {

      log_e(
        "Z2S LittleFs: invalid header magic for file \"%s\"!", 
        z2s_file_name);

      file.close();
      Z2S_endLittleFs();
      return false;
    }

    if (hdr.dataSize != z2s_file_size) {

      log_e(
        "Z2S LittleFs: invalid header size for file \"%s\" load request !"
        "(header: %u, requested: %u)", z2s_file_name, hdr.dataSize, 
        z2s_file_size);

      file.close();
      Z2S_endLittleFs();
      return false;
    }

    if (file.read(z2s_file_data, hdr.dataSize) != hdr.dataSize) {

      log_e(
        "Z2S LittleFs: error while reading data from file \"%s\"!",
        z2s_file_size);

      file.close();
      Z2S_endLittleFs();
      return false;
    }


    file.close();
    Z2S_endLittleFs();

    uint32_t crc32 = 0xFFFFFFFF;
    crc32 = esp_rom_crc32_le(crc32, z2s_file_data, hdr.dataSize);

    return (crc32 == hdr.crc32);
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

size_t Z2S_loadBufferFromFile(
  const char* z2s_file_name, size_t z2s_file_offset, size_t z2s_buffer_size,
  uint8_t* z2s_buffer_data) {

  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {  

    File file = LittleFS.open(z2s_file_name, "r");
  
    if (!file) {

      log_e(
        "Z2S LittleFs: failed to open file \"%s\" for read", z2s_file_name);
        Z2S_endLittleFs();
      return 0;
    }
  
    size_t file_size = file.size();

    if (z2s_file_offset >= file_size) {

      log_e("Z2S LittleFs: file offset exceeds file size!");

      file.close();
      Z2S_endLittleFs();
      return 0;
    }

    if (file.seek(z2s_file_offset, SeekSet)) {

      size_t bytesRead = file.read(z2s_buffer_data, z2s_buffer_size);
      file.close();
      Z2S_endLittleFs();
      return bytesRead;
    }
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

bool Z2S_deleteFile(const char* z2s_file_name) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  
  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    bool result = LittleFS.remove(file_name);
 
    if (!result) {

      log_e("Z2S LittleFs: failed to remove file \"%s\"", z2s_file_name);
    }
    Z2S_endLittleFs();
    return result;
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

bool Z2S_renameFile(const char *z2s_file_name, const char *z2s_new_file_name) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  char new_file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  snprintf(
    new_file_name, sizeof(new_file_name), "/z2s_gateway/%s", 
    z2s_new_file_name);
  
  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    bool rename_result = true;
    
    if (LittleFS.exists(new_file_name))
      rename_result = LittleFS.remove(new_file_name);
 
    if (!rename_result) {

      log_e("Z2S LittleFs: failed to remove file \"%s\"", z2s_new_file_name);
    }
    else
      rename_result =  LittleFS.rename(file_name, new_file_name);

    if (!rename_result) {

      log_e(
        "Z2S LittleFs: failed to rename file \"%s\" to \"%s\"", z2s_file_name, 
        z2s_new_file_name);
    }
    
    Z2S_endLittleFs();
    return rename_result;
  }
  else {

    log_e("xSemaphoreTake failed!");
    return false;
  }
}

/*****************************************************************************/

int Z2S_getFileSize(const char* z2s_file_name, bool init_fs, bool full_path) {
  
  if (init_fs && (!Z2S_initLittleFs())) {
    return 0;
  }

  char file_name[50] = {};
  
  if (full_path)
    snprintf(file_name, sizeof(file_name), "%s", z2s_file_name);
  else
    snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  
  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(500)) == pdTRUE) {

    File file = LittleFS.open(file_name, "r");
  
    if (!file) {

      log_e("Z2S LittleFs: failed to open file \"%s\"", file_name);

      //if (init_fs)
      Z2S_endLittleFs();
      return 0;
    }

    int file_size = file.size();

    file.close();
    //if (init_fs)
    Z2S_endLittleFs();
  
    return file_size;
  }
  else {
    
    log_e("xSemaphoreTake failed!");
    return false;
  }
}