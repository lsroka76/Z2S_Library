#ifndef Z2S_LITTLE_FS_H_
#define Z2S_LITTLE_FS_H_

bool Z2S_initLittleFs();
void Z2S_endLittleFs();

bool Z2S_saveFile(const char* z2s_file_name, 
                  const uint8_t* z2s_file_data, 
                  size_t z2s_file_size);

bool Z2S_loadFile(const char* z2s_file_name, 
                  uint8_t* z2s_file_data, 
                  size_t z2s_file_size);

bool Z2S_deleteFile(const char* z2s_file_name);

int Z2S_getFileSize(const char* z2s_file_name);


#endif //Z2S_LITTLE_FS_H_