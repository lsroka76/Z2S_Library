#include <LittleFS.h>
#include "z2s_little_fs.h"

bool Z2S_initLittleFs() {

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

bool Z2S_saveFile(const char* z2s_file_name, const uint8_t* z2s_file_data, size_t z2s_file_size) {

  log_d("Z2S LittleFs: saving file %s", z2s_file_name);
  if (!Z2S_initLittleFs()) {
    return false;
  }

  LittleFS.mkdir("/z2s_gateway");

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  
  File file = LittleFS.open(file_name, "w");
  
  if (!file) {
  
    log_e("Z2S LittleFs: failed to open file \"%s\" for write", z2s_file_name);
    LittleFS.end();
    return false;
  }

  file.write(z2s_file_data, z2s_file_size);
  file.close();
  LittleFS.end();
  return true;
}

bool Z2S_loadFile(const char* z2s_file_name, uint8_t* z2s_file_data, size_t z2s_file_size) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  
  File file = LittleFS.open(file_name, "r");
  
  if (!file) {

    log_e("Z2S LittleFs: failed to open file \"%s\" for read", z2s_file_name);
    LittleFS.end();
    return false;
  }
  
  size_t file_size = file.size();

  if (file_size > z2s_file_size) {

    log_e("Z2S LittleFs: file size exceeds buffer size!");
    file.close();
    LittleFS.end();
    return false;
  }

  int bytesRead = file.read(z2s_file_data, file_size);

  file.close();
  LittleFS.end();
  return bytesRead == file_size;
}

bool Z2S_deleteFile(const char* z2s_file_name) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  
  bool result = LittleFS.remove(file_name);
 
  if (!result) {
    log_e("Z2S LittleFs: failed to remove file \"%s\"", z2s_file_name);
  }
  LittleFS.end();
  return result;
}

int Z2S_getFileSize(const char* z2s_file_name) {
  
  if (!Z2S_initLittleFs()) {
    return false;
  }

  char file_name[50] = {};
  
  snprintf(file_name, sizeof(file_name), "/z2s_gateway/%s", z2s_file_name);
  
  File file = LittleFS.open(file_name, "r");
  
  if (!file) {

    log_e("Z2S LittleFs: failed to open file \"%s\"", z2s_file_name);
    LittleFS.end();
    return false;
  }

  int file_size = file.size();

  file.close();
  LittleFS.end();
  return file_size;
}
