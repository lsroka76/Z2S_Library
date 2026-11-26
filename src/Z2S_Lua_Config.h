#pragma once

/*
 * Enable/disable explicitly via compiler flags (-DZ2S_ENABLE_LUA=1 or 0),
 * or auto-enable only on ESP32-S3 builds with PSRAM configured.
 * 4MB / ESP32-C6 builds default cleanly to 0 (zero Flash/RAM cost).
 */
#ifndef Z2S_ENABLE_LUA
  #if defined(CONFIG_IDF_TARGET_ESP32S3) && (defined(BOARD_HAS_PSRAM) || defined(CONFIG_SPIRAM))
    #define Z2S_ENABLE_LUA 1
  #else
    #define Z2S_ENABLE_LUA 0
  #endif
#endif