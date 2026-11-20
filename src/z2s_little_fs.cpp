#include <Arduino.h>
#include <LittleFS.h>
#include <esp_rom_crc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <stdio.h>
#include <string.h>

#include "z2s_little_fs.h"

/*****************************************************************************/

// Keep unpacked layout identical to legacy on-disk format (16 bytes on ESP32)
struct FileHeader {
  uint32_t magic;
  uint16_t version;
  uint32_t dataSize;
  uint32_t crc32;
};

static_assert(sizeof(FileHeader) == 16,
  "FileHeader size changed - would break existing LittleFS files");

static constexpr uint32_t FILE_MAGIC              = 0x5344425A; // "ZBDS"
static constexpr uint32_t Z2S_FS_MUTEX_TIMEOUT_MS = 2000;
static constexpr size_t   Z2S_FS_PATH_MAX         = 96;

/*****************************************************************************/

extern SemaphoreHandle_t saveMutex;
extern bool _restart_scheduled;

static bool          s_fs_mounted        = false;
static bool          s_gateway_dir_ready = false;
static volatile bool s_fs_shutdown       = false;

/*****************************************************************************/

class FsMutexGuard {
public:
  explicit FsMutexGuard(uint32_t timeout_ms = Z2S_FS_MUTEX_TIMEOUT_MS) {
    if (s_fs_shutdown) {
      return;
    }
    if (saveMutex == nullptr) {
      _locked    = true;
      _has_mutex = false;
      return;
    }
    if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
      if (s_fs_shutdown) {
        xSemaphoreGive(saveMutex);
        return;
      }
      _locked    = true;
      _has_mutex = true;
    }
  }

  ~FsMutexGuard() {
    if (_locked && _has_mutex && saveMutex != nullptr) {
      xSemaphoreGive(saveMutex);
    }
  }

  bool locked() const { return _locked; }

  FsMutexGuard(const FsMutexGuard &)            = delete;
  FsMutexGuard &operator=(const FsMutexGuard &) = delete;

private:
  bool _locked    = false;
  bool _has_mutex = false;
};

/*****************************************************************************/

static bool buildGatewayPath(
  char *dst, size_t dst_size, const char *file_name,
  const char *suffix = "") {

  if (!dst || dst_size == 0 || !file_name || file_name[0] == '\0' || !suffix) {
    return false;
  }

  int n = snprintf(dst, dst_size, "/z2s_gateway/%s%s", file_name, suffix);
  if (n < 0 || static_cast<size_t>(n) >= dst_size) {
    log_e("Z2S LittleFs: path truncated for \"%s%s\"", file_name, suffix);
    return false;
  }
  return true;
}

/*****************************************************************************/

static bool ensureMountedLocked(bool allow_format = false) {

  if (s_fs_shutdown) {
    log_w("Z2S LittleFs: access blocked (shutdown in progress)");
    return false;
  }

  if (s_fs_mounted) {
    return true;
  }

  bool result = LittleFS.begin(false);

  if (!result) {
    if (allow_format) {
      log_w("Z2S LittleFs: mount failed - explicitly formatting partition");
      if (LittleFS.format()) {
        result = LittleFS.begin(false);
      }
      if (!result) {
        log_e("Z2S LittleFs: failed to mount after format");
      }
    } else {
      log_e(
        "Z2S LittleFs: mount failed (auto-format disabled to protect data)");
    }
  }

  if (result) {
    s_fs_mounted = true;
  }
  return result;
}

/*****************************************************************************/

static bool ensureGatewayDirLocked() {

  if (s_gateway_dir_ready) {
    return true;
  }

  if (LittleFS.exists("/z2s_gateway") || LittleFS.mkdir("/z2s_gateway")) {
    s_gateway_dir_ready = true;
    return true;
  }

  log_e("Z2S LittleFs: failed to create /z2s_gateway directory");
  return false;
}

/*****************************************************************************/

static bool atomicRenameLocked(const char *src_path, const char *dst_path) {

  if (!LittleFS.exists(src_path)) {

    log_e("Z2S LittleFs: rename source \"%s\" does not exist", src_path);
    
    return false;
  }

  // LittleFS supports atomic overwrite via lfs_rename directly.
  if (LittleFS.rename(src_path, dst_path)) {
    return true;
  }

  // Fallback if VFS layer refuses rename over an existing target.
  if (LittleFS.exists(dst_path)) {
    if (!LittleFS.remove(dst_path)) {
      log_e("Z2S LittleFs: failed to remove \"%s\" before rename", dst_path);
      return false;
    }
    if (LittleFS.rename(src_path, dst_path)) {
      return true;
    }
  }

  log_e("Z2S LittleFs: failed to rename \"%s\" to \"%s\"", src_path, dst_path);
  return false;
}

/*****************************************************************************/

bool Z2S_lockFs(uint32_t timeout_ms) {

  if (s_fs_shutdown) {
    return false;
  }
  if (saveMutex == nullptr) {
    return true;
  }
  if (xSemaphoreTake(saveMutex, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
    if (s_fs_shutdown) {
      xSemaphoreGive(saveMutex);
      return false;
    }
    return true;
  }
  log_e("Z2S LittleFs: Z2S_lockFs timeout!");
  return false;
}

void Z2S_unlockFs() {

  if (saveMutex != nullptr) {
    xSemaphoreGive(saveMutex);
  }
}

/*****************************************************************************/

bool Z2S_initLittleFs(bool allow_format) {

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: init mutex timeout!");
    return false;
  }

  return ensureMountedLocked(allow_format);
}

/*****************************************************************************/

bool Z2S_formatLittleFs() {

  FsMutexGuard guard(5000);
  if (!guard.locked()) {
    log_e("Z2S LittleFs: format mutex timeout!");
    return false;
  }

  if (s_fs_mounted) {
    LittleFS.end();
    s_fs_mounted        = false;
    s_gateway_dir_ready = false;
  }

  if (!LittleFS.format()) {
    log_e("Z2S LittleFs: format failed!");
    return false;
  }

  return ensureMountedLocked(false);
}

/*****************************************************************************/

void Z2S_endLittleFs(bool force_end) {

  if (!force_end) {
    // Mutex lifetime is now managed internally by FsMutexGuard.
    // Kept as a no-op for compatibility with legacy call sites.
    return;
  }

  // Acquire mutex FIRST so any active write completes cleanly before unmount.
  if (saveMutex != nullptr) {
    xSemaphoreTake(saveMutex, portMAX_DELAY);
  }

  s_fs_shutdown       = true;
  s_gateway_dir_ready = false;

  if (s_fs_mounted) {
    LittleFS.end();
    s_fs_mounted = false;
  }

  if (saveMutex != nullptr) {
    xSemaphoreGive(saveMutex);
  }
}

/*****************************************************************************/

bool Z2S_saveFile(
  const char *z2s_file_name, const uint8_t *z2s_file_data,
  size_t z2s_file_size) {

  if (!z2s_file_name || (z2s_file_size > 0 && !z2s_file_data)) {
    return false;
  }

  log_d("Z2S LittleFs: saving file %s (%u B)",
        z2s_file_name, (unsigned)z2s_file_size);

  char file_name[Z2S_FS_PATH_MAX] = {};
  char tmp_name[Z2S_FS_PATH_MAX]  = {};

  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name) ||
      !buildGatewayPath(tmp_name, sizeof(tmp_name), z2s_file_name, ".tmp")) {
    return false;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout saving \"%s\"!", z2s_file_name);
    return false;
  }

  if (!ensureMountedLocked(false) || !ensureGatewayDirLocked()) {
    return false;
  }

  File file = LittleFS.open(tmp_name, "w");
  if (!file) {
    log_e("Z2S LittleFs: failed to open temp file \"%s\"", tmp_name);
    return false;
  }

  size_t written = 0;
  if (z2s_file_size > 0) {
    written = file.write(z2s_file_data, z2s_file_size);
  }

  file.flush();
  file.close();

  if (written != z2s_file_size) {
    log_e("Z2S LittleFs: short write for \"%s\" (%u/%u B)",
          z2s_file_name, (unsigned)written, (unsigned)z2s_file_size);
    LittleFS.remove(tmp_name);
    return false;
  }

  if (!atomicRenameLocked(tmp_name, file_name)) {
    LittleFS.remove(tmp_name);
    return false;
  }

  return true;
}

/*****************************************************************************/

bool Z2S_saveFileWithCRCAndBackup(
  const char *z2s_file_name, const uint8_t *z2s_file_data,
  size_t z2s_file_size, const char *backup_file_name) {

  if (!z2s_file_name || (z2s_file_size > 0 && !z2s_file_data)) {
    return false;
  }

  log_d("Z2S LittleFs: saving CRC file %s (%u B)",
        z2s_file_name, (unsigned)z2s_file_size);

  char file_name[Z2S_FS_PATH_MAX] = {};
  char tmp_name[Z2S_FS_PATH_MAX]  = {};
  char bak_name[Z2S_FS_PATH_MAX]  = {};

  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name) ||
      !buildGatewayPath(tmp_name, sizeof(tmp_name), z2s_file_name, ".tmp")) {
    return false;
  }

  bool has_backup = (backup_file_name != nullptr && backup_file_name[0] != '\0');
  if (has_backup &&
      !buildGatewayPath(bak_name, sizeof(bak_name), backup_file_name)) {
    return false;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout saving CRC file \"%s\"!", z2s_file_name);
    return false;
  }

  if (!ensureMountedLocked(false) || !ensureGatewayDirLocked()) {
    return false;
  }

  uint32_t crc32 = 0xFFFFFFFF;
  if (z2s_file_size > 0) {
    crc32 = esp_rom_crc32_le(crc32, z2s_file_data, z2s_file_size);
  }

  FileHeader hdr = {};
  hdr.magic      = FILE_MAGIC;
  hdr.version    = 1;
  hdr.dataSize   = z2s_file_size;
  hdr.crc32      = crc32;

  // 1. Write complete header + payload to temporary file first.
  File file = LittleFS.open(tmp_name, "w");
  if (!file) {
    log_e("Z2S LittleFs: failed to open temp file \"%s\"", tmp_name);
    return false;
  }

  size_t hdr_written = file.write(reinterpret_cast<const uint8_t *>(&hdr),
                                  sizeof(hdr));
  size_t data_written = 0;
  if (hdr_written == sizeof(hdr) && z2s_file_size > 0) {
    data_written = file.write(z2s_file_data, z2s_file_size);
  }

  file.flush();
  file.close();

  if (hdr_written != sizeof(hdr) || data_written != z2s_file_size) {
    log_e("Z2S LittleFs: write error for \"%s\" (hdr %u/%u, data %u/%u)",
          z2s_file_name, (unsigned)hdr_written, (unsigned)sizeof(hdr),
          (unsigned)data_written, (unsigned)z2s_file_size);
    LittleFS.remove(tmp_name);
    return false;
  }

  // 2. Rotate existing primary -> backup only AFTER temp file is committed.
  if (has_backup && LittleFS.exists(file_name)) {
    if (!atomicRenameLocked(file_name, bak_name)) {
      log_w("Z2S LittleFs: failed to rotate \"%s\" to \"%s\"",
            z2s_file_name, backup_file_name);
    }
  }

  // 3. Atomically promote temp file -> primary.
  if (!atomicRenameLocked(tmp_name, file_name)) {
    LittleFS.remove(tmp_name);
    return false;
  }

  return true;
}

/*****************************************************************************/

bool Z2S_saveFileWithCRC(
  const char *z2s_file_name, const uint8_t *z2s_file_data,
  size_t z2s_file_size) {

  return Z2S_saveFileWithCRCAndBackup(
    z2s_file_name, z2s_file_data, z2s_file_size, nullptr);
}

/*****************************************************************************/

Z2S_FsLoadResult Z2S_loadFileEx(
  const char *z2s_file_name, uint8_t *z2s_file_data, size_t z2s_file_size) {

  if (!z2s_file_name || (z2s_file_size > 0 && !z2s_file_data)) {
    return Z2S_FsLoadResult::IO_ERROR;
  }

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    return Z2S_FsLoadResult::IO_ERROR;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout loading \"%s\"!", z2s_file_name);
    return Z2S_FsLoadResult::MUTEX_TIMEOUT;
  }

  if (!ensureMountedLocked(false)) {
    return Z2S_FsLoadResult::IO_ERROR;
  }

  if (!LittleFS.exists(file_name)) {
    return Z2S_FsLoadResult::NOT_FOUND;
  }

  File file = LittleFS.open(file_name, "r");
  if (!file) {
    log_e("Z2S LittleFs: failed to open \"%s\" for read", z2s_file_name);
    return Z2S_FsLoadResult::IO_ERROR;
  }

  size_t file_size = file.size();
  if (file_size != z2s_file_size) {
    log_e("Z2S LittleFs: invalid size for \"%s\" (on disk: %u, expected: %u)",
          z2s_file_name, (unsigned)file_size, (unsigned)z2s_file_size);
    file.close();
    return Z2S_FsLoadResult::CORRUPT;
  }

  size_t bytes_read = 0;
  if (file_size > 0) {
    bytes_read = file.read(z2s_file_data, file_size);
  }
  file.close();

  if (bytes_read != file_size) {
    log_e("Z2S LittleFs: short read for \"%s\" (%u/%u B)",
          z2s_file_name, (unsigned)bytes_read, (unsigned)file_size);
    return Z2S_FsLoadResult::IO_ERROR;
  }

  return Z2S_FsLoadResult::OK;
}

/*****************************************************************************/

bool Z2S_loadFile(
  const char *z2s_file_name, uint8_t *z2s_file_data, size_t z2s_file_size) {

  return Z2S_loadFileEx(z2s_file_name, z2s_file_data, z2s_file_size) ==
         Z2S_FsLoadResult::OK;
}

/*****************************************************************************/

bool Z2S_validateFileWithCRC(const char *z2s_file_name) {

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    return false;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout validating \"%s\"!", z2s_file_name);
    return false;
  }

  if (!ensureMountedLocked(false) || !LittleFS.exists(file_name)) {
    return false;
  }

  File file = LittleFS.open(file_name, "r");
  if (!file) {
    log_e("Z2S LittleFs: failed to open \"%s\" for validation", z2s_file_name);
    return false;
  }

  FileHeader hdr = {};
  if (file.read(reinterpret_cast<uint8_t *>(&hdr), sizeof(hdr)) != sizeof(hdr)) {
    log_e("Z2S LittleFs: failed reading header for \"%s\"", z2s_file_name);
    file.close();
    return false;
  }

  if (hdr.magic != FILE_MAGIC ||
      file.size() != sizeof(FileHeader) + hdr.dataSize) {
    log_e("Z2S LittleFs: invalid header/size for \"%s\"", z2s_file_name);
    file.close();
    return false;
  }

  uint32_t crc32 = 0xFFFFFFFF;
  uint8_t  buf[256];
  size_t   total_read = 0;

  while (file.available()) {
    size_t n = file.read(buf, sizeof(buf));
    if (n == 0) {
      break;
    }
    crc32 = esp_rom_crc32_le(crc32, buf, n);
    total_read += n;
  }

  file.close();
  return (total_read == hdr.dataSize) && (crc32 == hdr.crc32);
}

/*****************************************************************************/

Z2S_FsLoadResult Z2S_loadFileWithCRCEx(
  const char *z2s_file_name, uint8_t *z2s_file_data, size_t z2s_file_size) {

  if (!z2s_file_name || (z2s_file_size > 0 && !z2s_file_data)) {
    return Z2S_FsLoadResult::IO_ERROR;
  }

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    return Z2S_FsLoadResult::IO_ERROR;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout loading CRC file \"%s\"!", z2s_file_name);
    return Z2S_FsLoadResult::MUTEX_TIMEOUT;
  }

  if (!ensureMountedLocked(false)) {
    return Z2S_FsLoadResult::IO_ERROR;
  }

  if (!LittleFS.exists(file_name)) {
    return Z2S_FsLoadResult::NOT_FOUND;
  }

  File file = LittleFS.open(file_name, "r");
  if (!file) {
    log_e("Z2S LittleFs: failed to open \"%s\" for read", z2s_file_name);
    return Z2S_FsLoadResult::IO_ERROR;
  }

  size_t disk_size = file.size();
  if (disk_size < sizeof(FileHeader)) {
    log_e("Z2S LittleFs: truncated file \"%s\" (%u B)",
          z2s_file_name, (unsigned)disk_size);
    file.close();
    return Z2S_FsLoadResult::CORRUPT;
  }

  FileHeader hdr = {};
  if (file.read(reinterpret_cast<uint8_t *>(&hdr), sizeof(hdr)) != sizeof(hdr)) {
    log_e("Z2S LittleFs: error reading header for \"%s\"", z2s_file_name);
    file.close();
    return Z2S_FsLoadResult::IO_ERROR;
  }

  if (hdr.magic != FILE_MAGIC) {
    log_e("Z2S LittleFs: invalid magic (0x%08lX) for \"%s\"",
          (unsigned long)hdr.magic, z2s_file_name);
    file.close();
    return Z2S_FsLoadResult::CORRUPT;
  }

  if (hdr.dataSize != z2s_file_size ||
      disk_size != sizeof(FileHeader) + hdr.dataSize) {
    log_e("Z2S LittleFs: size mismatch for \"%s\" (hdr=%u, req=%u, disk=%u)",
          z2s_file_name, (unsigned)hdr.dataSize,
          (unsigned)z2s_file_size, (unsigned)disk_size);
    file.close();
    return Z2S_FsLoadResult::CORRUPT;
  }

  size_t bytes_read = 0;
  if (hdr.dataSize > 0) {
    bytes_read = file.read(z2s_file_data, hdr.dataSize);
  }
  file.close();

  if (bytes_read != hdr.dataSize) {
    log_e("Z2S LittleFs: error reading data from \"%s\" (%u/%u B)",
          z2s_file_name, (unsigned)bytes_read, (unsigned)hdr.dataSize);
    return Z2S_FsLoadResult::CORRUPT;
  }

  uint32_t crc32 = 0xFFFFFFFF;
  if (hdr.dataSize > 0) {
    crc32 = esp_rom_crc32_le(crc32, z2s_file_data, hdr.dataSize);
  }

  if (crc32 != hdr.crc32) {
    log_e("Z2S LittleFs: CRC mismatch for \"%s\" (calc=0x%08lX, hdr=0x%08lX)",
          z2s_file_name, (unsigned long)crc32, (unsigned long)hdr.crc32);
    return Z2S_FsLoadResult::CORRUPT;
  }

  return Z2S_FsLoadResult::OK;
}

/*****************************************************************************/

bool Z2S_loadFileWithCRC(
  const char *z2s_file_name, uint8_t *z2s_file_data, size_t z2s_file_size) {

  return Z2S_loadFileWithCRCEx(z2s_file_name, z2s_file_data, z2s_file_size) ==
         Z2S_FsLoadResult::OK;
}

/*****************************************************************************/

size_t Z2S_getFileSizeWithCRC(
  const char *z2s_file_name, Z2S_FsLoadResult *status) {

  auto set_status = [&](Z2S_FsLoadResult res) {
    if (status) {
      *status = res;
    }
  };

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    set_status(Z2S_FsLoadResult::IO_ERROR);
    return 0;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout getting CRC size for \"%s\"!",
          z2s_file_name);
    set_status(Z2S_FsLoadResult::MUTEX_TIMEOUT);
    return 0;
  }

  if (!ensureMountedLocked(false)) {
    set_status(Z2S_FsLoadResult::IO_ERROR);
    return 0;
  }

  if (!LittleFS.exists(file_name)) {
    set_status(Z2S_FsLoadResult::NOT_FOUND);
    return 0;
  }

  File file = LittleFS.open(file_name, "r");
  if (!file) {
    log_e("Z2S LittleFs: failed to open \"%s\" for read", z2s_file_name);
    set_status(Z2S_FsLoadResult::IO_ERROR);
    return 0;
  }

  size_t disk_size = file.size();
  FileHeader hdr   = {};

  if (disk_size < sizeof(FileHeader) ||
      file.read(reinterpret_cast<uint8_t *>(&hdr), sizeof(hdr)) != sizeof(hdr)) {
    log_e("Z2S LittleFs: error reading header for \"%s\"", z2s_file_name);
    file.close();
    set_status(Z2S_FsLoadResult::CORRUPT);
    return 0;
  }

  file.close();

  if (hdr.magic != FILE_MAGIC ||
      disk_size != sizeof(FileHeader) + hdr.dataSize) {
    log_e("Z2S LittleFs: invalid header/size for \"%s\"", z2s_file_name);
    set_status(Z2S_FsLoadResult::CORRUPT);
    return 0;
  }

  set_status(Z2S_FsLoadResult::OK);
  return hdr.dataSize;
}

/*****************************************************************************/

size_t Z2S_loadBufferFromFile(
  const char *z2s_file_name, size_t z2s_file_offset, size_t z2s_buffer_size,
  uint8_t *z2s_buffer_data) {

  if (!z2s_file_name || !z2s_buffer_data || z2s_buffer_size == 0) {
    return 0;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout reading buffer from \"%s\"!",
          z2s_file_name);
    return 0;
  }

  if (!ensureMountedLocked(false) || !LittleFS.exists(z2s_file_name)) {
    return 0;
  }

  File file = LittleFS.open(z2s_file_name, "r");
  if (!file) {
    log_e("Z2S LittleFs: failed to open \"%s\" for read", z2s_file_name);
    return 0;
  }

  size_t file_size = file.size();
  if (z2s_file_offset >= file_size) {
    log_e("Z2S LittleFs: offset %u exceeds file size %u for \"%s\"",
          (unsigned)z2s_file_offset, (unsigned)file_size, z2s_file_name);
    file.close();
    return 0;
  }

  size_t bytes_read = 0;
  if (file.seek(z2s_file_offset, SeekSet)) {
    bytes_read = file.read(z2s_buffer_data, z2s_buffer_size);
  } else {
    log_e("Z2S LittleFs: seek failed at %u for \"%s\"",
          (unsigned)z2s_file_offset, z2s_file_name);
  }

  file.close();
  return bytes_read;
}

/*****************************************************************************/

bool Z2S_deleteFile(const char *z2s_file_name) {

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    return false;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout deleting \"%s\"!", z2s_file_name);
    return false;
  }

  if (!ensureMountedLocked(false)) {
    return false;
  }

  if (!LittleFS.exists(file_name)) {
    return false;
  }

  bool result = LittleFS.remove(file_name);
  if (!result) {
    log_e("Z2S LittleFs: failed to remove \"%s\"", z2s_file_name);
  }
  return result;
}

/*****************************************************************************/

bool Z2S_renameFile(const char *z2s_file_name, const char *z2s_new_file_name) {

  char file_name[Z2S_FS_PATH_MAX]     = {};
  char new_file_name[Z2S_FS_PATH_MAX] = {};

  if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name) ||
      !buildGatewayPath(new_file_name, sizeof(new_file_name), z2s_new_file_name)) {
    return false;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout renaming \"%s\"!", z2s_file_name);
    return false;
  }

  if (!ensureMountedLocked(false)) {
    return false;
  }

  return atomicRenameLocked(file_name, new_file_name);
}

/*****************************************************************************/

int Z2S_getFileSize(const char *z2s_file_name, bool init_fs, bool full_path) {

  (void)init_fs;

  if (!z2s_file_name || z2s_file_name[0] == '\0') {
    return 0;
  }

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (full_path) {
    int n = snprintf(file_name, sizeof(file_name), "%s", z2s_file_name);
    if (n < 0 || static_cast<size_t>(n) >= sizeof(file_name)) {
      return 0;
    }
  } else if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    return 0;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    log_e("Z2S LittleFs: mutex timeout getting size of \"%s\"!", file_name);
    return 0;
  }

  if (!ensureMountedLocked(false) || !LittleFS.exists(file_name)) {
    return 0;
  }

  File file = LittleFS.open(file_name, "r");
  if (!file) {
    log_e("Z2S LittleFs: failed to open \"%s\"", file_name);
    return 0;
  }

  int file_size = static_cast<int>(file.size());
  file.close();
  return file_size;
}

/*****************************************************************************/

int8_t Z2S_fileExists(const char *z2s_file_name, bool full_path) {

  if (!z2s_file_name || z2s_file_name[0] == '\0') {
    return -1;
  }

  char file_name[Z2S_FS_PATH_MAX] = {};
  if (full_path) {
    int n = snprintf(file_name, sizeof(file_name), "%s", z2s_file_name);
    if (n < 0 || static_cast<size_t>(n) >= sizeof(file_name)) {
      return -1;
    }
  } else if (!buildGatewayPath(file_name, sizeof(file_name), z2s_file_name)) {
    return -1;
  }

  FsMutexGuard guard;
  if (!guard.locked()) {
    return -1;
  }

  if (!ensureMountedLocked(false)) {
    return -1;
  }

  return LittleFS.exists(file_name) ? 1 : 0;
}