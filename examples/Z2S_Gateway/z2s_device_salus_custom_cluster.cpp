#include "z2s_device_salus_custom_cluster.h"
#include "z2s_device_temphumidity.h"
#include "z2s_device_hvac.h"

/*****************************************************************************/

static constexpr uint8_t SALUS_IT600_BATTERY_STEPS[6] PROGMEM = {
  0, 10, 25, 50, 75, 100
};

/*****************************************************************************/

void processSalusIt600CustomCluster(
  uint8_t tsn, esp_zb_ieee_addr_t ieee_addr, uint16_t short_addr,
  uint16_t endpoint, uint8_t command_id, uint16_t payload_size,
  uint8_t *payload) {

  (void)tsn;
  (void)ieee_addr;

  log_i(
    "Salus iT600 RX: addr=0x%04X ep=%u cmd=0x%02X len=%u",
    short_addr, endpoint, command_id, payload_size);

  switch (command_id) {

    case SALUS_IT600_CMD_SYNC_REQUEST: {
      if (payload && payload_size >= 7) {
        uint32_t fw_ver = 0;
        memcpy(&fw_ver, &payload[3], sizeof(uint32_t));
        log_i(
          "Salus iT600 syncRequest: dev_num=0x%02X dev_type=0x%02X fw=0x%08lX",
          payload[0], payload[1], (unsigned long)fw_ver);
      }
      sendSalusIt600SyncResponse(short_addr, (uint8_t)endpoint);
    } break;

    case SALUS_IT600_CMD_RECORD: {
      if (!payload || payload_size < 2) {
        return;
      }

      if (payload[0] != SALUS_IT600_RECORD_STATUS) {
        log_i(
          "Salus iT600 ignoring non-status record 0x%02X (len=%u)",
          payload[0], payload_size);
        return;
      }

      if (payload_size < SALUS_IT600_STATUS_MIN_LEN) {
        log_w("Salus iT600 short 0x71 status record (%u bytes)", payload_size);
        return;
      }

      Supla::Element *th_element = Z2S_findZ2SElement(
        short_addr, endpoint, SALUS_IT600_CUSTOM_CLUSTER,
        SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, NO_CUSTOM_CMD_SID);

      Supla::Element *hvac_element = Z2S_findZ2SElement(
        short_addr, endpoint, SALUS_IT600_CUSTOM_CLUSTER,
        SUPLA_CHANNELTYPE_HVAC, NO_CUSTOM_CMD_SID);

      uint8_t hold_type     = payload[3];
      uint8_t system_mode   = payload[15];
      uint8_t running_state = (payload[17] == 1) ? 1 : 0;
      uint8_t child_lock    = (payload[23] == 1) ? 1 : 0;
      uint8_t battery_idx   = payload[49];

      bool is_temp_placeholder = (payload[4] == 0x12 && payload[5] == 0x34);
      bool valid_local_temp =
        !is_temp_placeholder &&
        salusIt600IsValidBcd(payload[4]) &&
        salusIt600IsValidBcd(payload[5]);
      bool valid_setpoint =
        salusIt600IsValidBcd(payload[9]) &&
        salusIt600IsValidBcd(payload[10]);
      bool valid_humidity = salusIt600IsValidBcd(payload[24]);

      int32_t local_temp_x100 = valid_local_temp
                                  ? salusIt600BcdTempX100(payload[4], payload[5])
                                  : INT32_MIN;
      int32_t setpoint_x100   = valid_setpoint
                                  ? salusIt600BcdTempX100(payload[9], payload[10])
                                  : INT32_MIN;
      uint8_t humidity        = valid_humidity
                                  ? (uint8_t)salusIt600BcdByte(payload[24])
                                  : 0xFF;

      log_i(
        "Salus 0x71: hold=%u mode=%u run=%u lock=%u hum=%u%% temp=%ld sp=%ld bat_idx=%u",
        hold_type, system_mode, running_state, child_lock, humidity,
        (long)local_temp_x100, (long)setpoint_x100, battery_idx);

      if (th_element) {
        if (valid_local_temp) {
          msgZ2SDeviceTempHumidityTemp(th_element, (float)local_temp_x100 / 100.0f);
        }
        if (valid_humidity && humidity <= 100) {
          msgZ2SDeviceTempHumidityHumi(th_element, (float)humidity);
        }
      }

      if (hvac_element) {
        // 1. Apply mode before setpoint
        if (hold_type == SALUS_IT600_HOLD_STANDBY) {
          msgZ2SDeviceHvac(hvac_element, TRV_SYSTEM_MODE_MSG, 0);
        } else if ((hold_type == SALUS_IT600_HOLD_SCHEDULE) ||
                   (hold_type == SALUS_IT600_HOLD_TEMPORARY)) {
          msgZ2SDeviceHvac(hvac_element, TRV_SCHEDULE_MODE_MSG, 2);
        } else {
          msgZ2SDeviceHvac(hvac_element, TRV_SYSTEM_MODE_MSG, 1);
        }

        // 2. Active setpoint (only in manual/away heat mode; skip standby & schedule transitions)
        if (valid_setpoint &&
            (system_mode != SALUS_IT600_MODE_COOL) &&
            (hold_type != SALUS_IT600_HOLD_STANDBY) &&
            (hold_type != SALUS_IT600_HOLD_SCHEDULE) &&
            (hold_type != SALUS_IT600_HOLD_TEMPORARY)) {
          msgZ2SDeviceHvac(hvac_element, TRV_HEATING_SETPOINT_MSG, setpoint_x100);
        }

        // 3. Running state, child lock, local temperature
        msgZ2SDeviceHvac(hvac_element, TRV_RUNNING_STATE_MSG, running_state);
        msgZ2SDeviceHvac(hvac_element, TRV_CHILD_LOCK_MSG, child_lock);

        if (valid_local_temp) {
          msgZ2SDeviceHvac(hvac_element, TRV_LOCAL_TEMPERATURE_MSG, local_temp_x100);
        }
      }

      // Battery percentage (SQ610RFNH only; SQ610NH is mains-powered)
      // Scoped lock released before updateSuplaBatteryLevel() acquires Z2S_DevicesLock itself
      bool is_rf_battery_model = false;
      {
        Z2S_DevicesLock lock;
        Z2S_Device *z2s_device = Z2S_Device::getZ2SDeviceByShortAddress(short_addr);
        is_rf_battery_model =
          (z2s_device != nullptr) &&
          (z2s_device->getDeviceDescId() == Z2S_DEVICE_DESC_SALUS_SQ610RFNH);
      }
      if (is_rf_battery_model && (battery_idx < 6)) {
        updateSuplaBatteryLevel(
          short_addr, ZBD_BATTERY_LEVEL_MSG,
          SALUS_IT600_BATTERY_STEPS[battery_idx], false);
      }
    } break;

    case SALUS_IT600_CMD_ECHO: {
      if (payload && payload_size >= 4) {
        log_i(
          "Salus iT600 echo: handle=%u cmd=%c%c%c len=%u",
          payload[0], payload[1], payload[2], payload[3], payload_size);
      }
      Supla::Element *hvac_element = Z2S_findZ2SElement(
        short_addr, endpoint, SALUS_IT600_CUSTOM_CLUSTER,
        SUPLA_CHANNELTYPE_HVAC, NO_CUSTOM_CMD_SID);
      if (hvac_element) {
        static_cast<Supla::Control::Z2S_TRVInterface *>(hvac_element)->refreshTimeout();
      }
    } break;

    default:
      log_i("Salus iT600 unhandled cmd 0x%02X (len=%u)", command_id, payload_size);
    break;
  }
}