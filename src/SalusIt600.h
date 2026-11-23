#ifndef SALUS_IT600_H_
#define SALUS_IT600_H_

#include <Arduino.h>
#include <time.h>
#include <string.h>
#include "ZigbeeGateway.h"

/*****************************************************************************/

#define SALUS_IT600_SECONDS_1970_TO_2000        946684800UL

#define SALUS_IT600_RECORD_STATUS               0x71
#define SALUS_IT600_RECORD_SETTINGS             0x72
#define SALUS_IT600_STATUS_MIN_LEN              50

#define SALUS_IT600_HOLD_SCHEDULE               0x00
#define SALUS_IT600_HOLD_TEMPORARY              0x01
#define SALUS_IT600_HOLD_PERMANENT              0x02
#define SALUS_IT600_HOLD_AWAY                   0x06
#define SALUS_IT600_HOLD_STANDBY                0x07

#define SALUS_IT600_MODE_COOL                   0x03
#define SALUS_IT600_MODE_HEAT                   0x04

#define SALUS_IT600_KEYPAD_LOCK                 0x41 // 'A'
#define SALUS_IT600_KEYPAD_UNLOCK               0x42 // 'B'

/*****************************************************************************/

inline uint8_t g_salus_it600_tsn    = 0;
inline uint8_t g_salus_it600_handle = 0;

/*****************************************************************************/

inline uint8_t salusIt600NextTsn() {
  return ++g_salus_it600_tsn;
}

inline uint8_t salusIt600NextHandle() {
  // Handles 1..127; thermostat uses 0 and >= 0x80 for unsolicited reports
  g_salus_it600_handle = (g_salus_it600_handle % 0x7F) + 1;
  return g_salus_it600_handle;
}

inline int32_t salusIt600BcdByte(uint8_t b) {
  return ((b >> 4) * 10) + (b & 0x0F);
}

inline int32_t salusIt600BcdTempX100(uint8_t hi, uint8_t lo) {
  int32_t raw = salusIt600BcdByte(hi) * 100 + salusIt600BcdByte(lo);
  return (raw >= 11000) ? (11000 - raw) : raw;
}

/*****************************************************************************/

inline bool sendSalusIt600SyncResponse(
  uint16_t short_addr,
  uint8_t  dst_endpoint = SALUS_IT600_THERMOSTAT_ENDPOINT,
  uint8_t  device_number = SALUS_IT600_THERMOSTAT_DEVICE_NUM) {

  if (short_addr == 0) {
    return false;
  }

  uint8_t payload[15] = {};
  payload[0] = device_number; // 0x20 for thermostats
  payload[1] = 0x00;          // Normal status

  // time(nullptr) is assumed local wall-clock time
  time_t now = time(nullptr);
  uint32_t local_sec_2000 =
    (now > (time_t)SALUS_IT600_SECONDS_1970_TO_2000)
      ? (uint32_t)(now - (time_t)SALUS_IT600_SECONDS_1970_TO_2000)
      : 0;
  memcpy(&payload[2], &local_sec_2000, sizeof(uint32_t));

  payload[6]  = 0x02; // 24-hour clock, no separate DST shift (already in local time)
  payload[7]  = 13;   // UTC timezone index (time bytes already local)
  payload[8]  = 0x00; // DST offset = 0 h
  payload[9]  = 0xFF; // RX1 none
  payload[10] = 0xFF;
  payload[11] = 0xFF; // RX2 none
  payload[12] = 0xFF;
  payload[13] = 0x31; // '1' = cloud/gateway online
  payload[14] = 0x00;

  log_i(
    "Salus iT600: sending 0x81 syncResponse to 0x%04X ep %u (dev=0x%02X, t=%lu)",
    short_addr, dst_endpoint, device_number, (unsigned long)local_sec_2000);

  return zbGateway.sendAPSDEDataRequestCmd(
    short_addr,
    salusIt600NextTsn(),
    dst_endpoint,
    SALUS_IT600_CUSTOM_CLUSTER,
    SALUS_IT600_CMD_SYNC_RESPONSE,
    sizeof(payload),
    payload,
    ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI, // Frame control 0x1D (server->client)
    1,                               // Disable default response
    1,                               // Manufacturer specific
    SALUS_IT600_MANUFACTURER_CODE,
    false,
    ESP_ZB_AF_HA_PROFILE_ID);
}

/*****************************************************************************/

inline bool sendSalusIt600BCommand(
  uint16_t short_addr, uint8_t dst_endpoint,
  const char code[3], const uint8_t *args, uint8_t args_len) {

  if (short_addr == 0 || !code || args_len > 60) {
    return false;
  }

  uint8_t payload[64];
  payload[0] = salusIt600NextHandle();
  payload[1] = (uint8_t)code[0];
  payload[2] = (uint8_t)code[1];
  payload[3] = (uint8_t)code[2];

  if (args && args_len > 0) {
    memcpy(&payload[4], args, args_len);
  }

  log_i(
    "Salus iT600: sending %c%c%c (handle=%u, len=%u) to 0x%04X ep %u",
    code[0], code[1], code[2], payload[0], args_len, short_addr, dst_endpoint);

  return zbGateway.sendAPSDEDataRequestCmd(
    short_addr,
    salusIt600NextTsn(),
    dst_endpoint,
    SALUS_IT600_CUSTOM_CLUSTER,
    SALUS_IT600_CMD_WRITE,
    (uint8_t)(4 + args_len),
    payload,
    ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI, // Frame control 0x1D
    1,
    1,
    SALUS_IT600_MANUFACTURER_CODE,
    false,
    ESP_ZB_AF_HA_PROFILE_ID);
}

/*****************************************************************************/

inline bool sendSalusIt600Setpoint(
  uint16_t short_addr, uint8_t dst_endpoint, int32_t setpoint_x100) {

  if (setpoint_x100 < 500)  setpoint_x100 = 500;
  if (setpoint_x100 > 4000) setpoint_x100 = 4000;

  // Round to nearest 0.5 C step + 0x20 offset (5.0C=0x2A, 21.0C=0x4A, 35.0C=0x66)
  uint8_t sp_byte = (uint8_t)(((setpoint_x100 + 25) / 50) + 0x20);
  return sendSalusIt600BCommand(short_addr, dst_endpoint, "B34", &sp_byte, 1);
}

inline bool sendSalusIt600SystemMode(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t mode_on) {

  if (mode_on == 0) {
    uint8_t arg = SALUS_IT600_HOLD_STANDBY;
    return sendSalusIt600BCommand(short_addr, dst_endpoint, "B20", &arg, 1);
  }

  uint8_t hold_arg = SALUS_IT600_HOLD_PERMANENT;
  bool ok = sendSalusIt600BCommand(short_addr, dst_endpoint, "B20", &hold_arg, 1);
  delay(100);
  uint8_t heat_arg = SALUS_IT600_MODE_HEAT;
  return sendSalusIt600BCommand(short_addr, dst_endpoint, "B48", &heat_arg, 1) && ok;
}

inline bool sendSalusIt600ScheduleMode(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t schedule_on) {

  uint8_t arg = schedule_on ? SALUS_IT600_HOLD_SCHEDULE : SALUS_IT600_HOLD_PERMANENT;
  return sendSalusIt600BCommand(short_addr, dst_endpoint, "B20", &arg, 1);
}

inline bool sendSalusIt600ChildLock(
  uint16_t short_addr, uint8_t dst_endpoint, uint8_t lock_on) {

  uint8_t args[13] = {};
  args[2] = lock_on ? SALUS_IT600_KEYPAD_LOCK : SALUS_IT600_KEYPAD_UNLOCK;
  return sendSalusIt600BCommand(short_addr, dst_endpoint, "B03", args, sizeof(args));
}

/*****************************************************************************/

#endif // SALUS_IT600_H_