#include <LittleFS.h>
#include "z2s_little_fs.h"

//static const char* _TEMP_FILE_NAME[] PROGMEM = "/z2s_gateway/save_file.tmp";

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
  /*else {

    log_e("Z2S LittleFs: saveMutex locked!");
    return false;
  }
  
  return true;
}*/

/*****************************************************************************/

void Z2S_endLittleFs() {

  //LittleFS.end();
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