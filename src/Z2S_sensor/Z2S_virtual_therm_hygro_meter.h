/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef Z2S_VIRTUAL_THERM_HYGRO_METER_H_
#define Z2S_VIRTUAL_THERM_HYGRO_METER_H_

#include "ZigbeeGateway.h"
#include "Z2S_common.h"
#include "z2s_devices_database.h"

#include <supla/sensor/virtual_therm_hygro_meter.h>

/*****************************************************************************/
/*                    Z2S_VirtualThermHygroMeter                             */
/*****************************************************************************/

namespace Supla {
namespace Sensor {

/*****************************************************************************/

class Z2S_VirtualThermHygroMeter : 
  public Supla::Sensor::VirtualThermHygroMeter, public Z2S_Core {
  
public:

/*****************************************************************************/

  Z2S_VirtualThermHygroMeter(bool rwns_flag = false) 
    : Z2S_Core(this), _rwns_flag(rwns_flag) {
  }

/*****************************************************************************/

  void setRWNSFlag(bool rwns_flag) {

    _rwns_flag = rwns_flag;    
  }
  
/*****************************************************************************/

  void Refresh() {

    _last_timeout_ms = millis();
    channel.setStateOnline();
  }

/*****************************************************************************/

  void setTemperature(double val) {
    
    log_i("temperature = %f4.2", val);
    _forced_temperature = false;
    temperature = val;

    lastReadTime = 0;

    //channel.setNewValue(temperature, getHumi());
    Refresh();
  }

/*****************************************************************************/

  void setHumidity(double val) {
    
    log_i("humidity = %f4.2", val);
    humidity = val;

    lastReadTime = 0;

    //channel.setNewValue(temperature, getHumi());
    Refresh();
  }

/*****************************************************************************/

  void setForcedTemperature(double val) {
    
    log_i("temperature = %f4.2", val);
    _forced_temperature = true;
    temperature = val;

    Refresh();
  }

/*****************************************************************************/

  bool isForcedTemperature() {

    return _forced_temperature;
  }

/*****************************************************************************/

  void iterateAlways() override {

    uint32_t millis_ms = millis();

    if (millis_ms - lastReadTime > refreshIntervalMs) {

      lastReadTime = millis_ms;
      channel.setNewValue(getTemp(), getHumi());

      if (checkChannelUserDataFlags(
            USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE)) {

        resendTemperatureHumidityValue(
          RTH_VALUE_TYPE_TEMPERATURE, channel.getValueDoubleFirst() * 100);

        resendTemperatureHumidityValue(
          RTH_VALUE_TYPE_HUMIDITY, channel.getValueDoubleSecond() * 100);
      }
    }


    if (getTimeoutMs()) {
      
      uint32_t _zb_device_last_seen_ms = getZbDeviceLastSeenMs();
      
      if (_zb_device_last_seen_ms > _last_timeout_ms) {

        _last_timeout_ms = _zb_device_last_seen_ms;
        channel.setStateOnline();
      }
      
      if ((millis_ms - _last_timeout_ms) > getTimeoutMs()) {
      
        _last_timeout_ms = millis_ms;

        if (_rwns_flag) 
          channel.setStateOfflineRemoteWakeupNotSupported();
        else
          channel.setStateOffline();
      }
    }
  }
    
/*****************************************************************************/

 protected:
  bool     _rwns_flag;
  bool     _forced_temperature = false;

  uint32_t _last_timeout_ms = 0;
};

/*****************************************************************************/
/*****************************************************************************/

// Sensor Types
#define SNZB_TYPE_TEMPERATURE   0x00
#define SNZB_TYPE_HUMIDITY      0x01

// Source IDs
#define SNZB_SOURCE_1           0x00
#define SNZB_SOURCE_2           0x01

// Sensor States
#define SNZB_STATE_UNBOUND      0x00
#define SNZB_STATE_ONLINE       0x01
#define SNZB_STATE_OFFLINE      0x02
#define SNZB_STATE_RESTORED     0x03

/*****************************************************************************/

/**
 * Encodes SNZB-02DR2 payload into a raw uint8_t buffer.
 * 
 * @param buf       Output raw byte array buffer (minimum 30 bytes recommended).
 * @param max_len   Maximum capacity of the buffer.
 * @param count     Number of items being passed (1 to 4).
 * @param ...       Item parameters as tuples of:
 *                  (uint32_t type, uint32_t id, int32_t state, in32_t value)
 *                
 * 
 * @return          Total bytes written to buffer (0 on error).
 */

/*****************************************************************************/

inline size_t build_snzb02dr2_payload_va(
  uint8_t *buf, size_t max_len, uint8_t count, ...) {

  if (!buf || count == 0 || count > 4) return 0;

  // Safety check: header (9 bytes) + max 6 bytes per item
  if (max_len < (size_t)(9 + (count * 6))) return 0;

  size_t offset = 9; // Reserve 6 bytes for header

  va_list args;
  va_start(args, count);

  for (uint8_t i = 0; i < count; i++) {
      
    uint8_t type  = va_arg(args, uint32_t);
    uint8_t id    = va_arg(args, uint32_t);
    uint8_t state = va_arg(args, uint32_t);
    int32_t value = va_arg(args, int32_t);

    buf[offset++] = type;
    buf[offset++] = id;
    buf[offset++] = state;

    if (state == SNZB_STATE_ONLINE || state == SNZB_STATE_RESTORED) {

      buf[offset++] = 0x02; // Value length = 2 bytes

      if (type == SNZB_TYPE_TEMPERATURE) {

        buf[offset++] = value & 0xFF;
        buf[offset++] = (value >> 8) & 0xFF;
      } 
      else {
        
        buf[offset++] = (value & 0xFF);
        buf[offset++] = ((value >> 8) & 0xFF);
      }
    } 
    else {
      
      buf[offset++] = 0x00; // No value payload attached
    }
  }
  va_end(args);

  // Header assembly
  buf[0] = ESP_ZB_ZCL_ATTR_TYPE_U8;
  buf[1] = (offset - 3) & 0xFF;
  buf[2] = 0x00;
  buf[3] = 0x01;
  buf[4] = 0x01;
  buf[5] = 0x00;
  buf[6] = 0x03;
  buf[7] = (uint8_t)(offset - 8); // TLV Length
  buf[8] = count;

  return offset; // Return length of generated payload
}

/*****************************************************************************/

#define SONOFF_FLAG_TEMPERATURE  (1 << 0)
#define SONOFF_FLAG_HUMIDITY     (1 << 1)
#define SONOFF_FLAG_PRESSURE     (1 << 2)


/**
 * Builds the ZCL Array Write Attribute frame for Sonoff remote sensor data.
 *
 * @param buffer Pointer to the destination byte array (must be at least 32 bytes).
 * @param count Number of sensor arguments provided:
 *              1 = Temperature only
 *              2 = Temperature + Humidity
 *              3 = Temperature + Humidity + Pressure
 * @param ... Floating-point values (float or double) in order: temp (°C), humi (%), press (hPa).
 * @return Total bytes written to buffer.
 */

/*****************************************************************************/

inline uint16_t build_sonoff_payload(
  uint8_t *buffer, uint8_t flags, int16_t temperature, uint16_t humidity, 
  int32_t pressure) {

  if (!buffer || flags == 0) return 0;

  // Count how many sensors are active
  uint8_t item_count = 0;
  
  if (flags & SONOFF_FLAG_TEMPERATURE) item_count++;
  if (flags & SONOFF_FLAG_HUMIDITY) item_count++;    
  if (flags & SONOFF_FLAG_PRESSURE) item_count++;    
  
  buffer[0] = ESP_ZB_ZCL_ATTR_TYPE_U8;

  // buffer[1] and buffer[2] reserved for Array Element Count (uint16_le)

  // 2. Inner eWeLink Header
  buffer[3] = 0x01; // Prefix byte 0
  buffer[4] = 0x01; // Prefix byte 1
  buffer[5] = 0x00; // Prefix byte 2
  buffer[6] = 0x03; // Prefix byte 3

  // buffer[7] reserved for Inner Length L (1 + item_bytes)
  buffer[8] = item_count; 

  uint16_t idx = 9; // Start index for sensor item blocks


  if (flags & SONOFF_FLAG_TEMPERATURE) {

    buffer[idx++] = 0x00; // Type: Temperature
    buffer[idx++] = 0x00; // Sensor ID: 0
    buffer[idx++] = 0x01; // State: Enabled
    buffer[idx++] = 0x02; // Value Length: 2 bytes

    buffer[idx++] = temperature & 0xFF;        
    buffer[idx++] = (temperature >> 8) & 0xFF; 
  }

  if (flags & SONOFF_FLAG_HUMIDITY) {

    buffer[idx++] = 0x01; // Type: Humidity
    buffer[idx++] = 0x00; // Sensor ID: 0
    buffer[idx++] = 0x01; // State: Enabled
    buffer[idx++] = 0x02; // Value Length: 2 bytes

    buffer[idx++] = humidity & 0xFF;     
    buffer[idx++] = (humidity >> 8) & 0xFF;
  }

  if (flags & SONOFF_FLAG_PRESSURE) {

    buffer[idx++] = 0x02; // Type: Pressure
    buffer[idx++] = 0x00; // Sensor ID: 0
    buffer[idx++] = 0x01; // State: Enabled
    buffer[idx++] = 0x04; // Value Length: 4 bytes

    buffer[idx++] = pressure & 0xFF;
    buffer[idx++] = (pressure >> 8) & 0xFF;
    buffer[idx++] = (pressure >> 16) & 0xFF;
    buffer[idx++] = (pressure >> 24) & 0xFF;
  }

  // Calculate length headers
  uint16_t total_item_bytes = idx - 9;
  buffer[7] = 1 + total_item_bytes; // eWeLink Inner Length L

  uint16_t inner_payload_len = idx - 3;
  buffer[1] = inner_payload_len & 0xFF;       
  buffer[2] = (inner_payload_len >> 8) & 0xFF;

  return idx;
}

inline bool is_valid_temperature(int32_t val) {
  return ((val >= -4000) && (val <= 8500));
}

inline bool is_valid_humidity(int32_t val) {
  return ((val >= 0) && (val <= 10000));
}

/*****************************************************************************/

class Z2S_SNZB02DR2ThermHygroMeter : 
  public Supla::Sensor::Z2S_VirtualThermHygroMeter {
  
public:

/*****************************************************************************/

  Z2S_SNZB02DR2ThermHygroMeter(bool rwns_flag = false) 
    : Z2S_VirtualThermHygroMeter(rwns_flag) 
  {
  }

/*****************************************************************************/

  void setSonoffExternalTemperature(int16_t sonoff_external_temperature) {

    _sonoff_external_temperature = sonoff_external_temperature;
    _last_resent_ms = 0;
  }

/*****************************************************************************/

  void setSonoffExternalHumidity(uint16_t sonoff_external_humidity) {

    _sonoff_external_humidity = sonoff_external_humidity;
    _last_resent_ms = 0;
  }

/*****************************************************************************/

  void setSonoffExternalTemperature2(int16_t sonoff_external_temperature) {

    _sonoff_external_temperature_2 = sonoff_external_temperature;
    _last_resent_ms = 0;
  }

/*****************************************************************************/

  void setSonoffExternalHumidity2(uint16_t sonoff_external_humidity) {

    _sonoff_external_humidity_2 = sonoff_external_humidity;
    _last_resent_ms = 0;
  }

/*****************************************************************************/

  void updateSNZB02DR2ExtValues() {

    if (!Zigbee.started())
      return;

    uint8_t temperature_selector = 1;

    switch (getChannelModelId()) {


      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL_EXT: {

        
        uint8_t sonoff_buffer[64] = {};
        size_t payload_size = 0;

        uint32_t temperature_state_1 = SNZB_STATE_UNBOUND;
        uint32_t temperature_state_2 = SNZB_STATE_UNBOUND;

        uint32_t humidity_state_1 = SNZB_STATE_UNBOUND;
        uint32_t humidity_state_2 = SNZB_STATE_UNBOUND;

        if (is_valid_temperature(_sonoff_external_temperature))
          temperature_state_1 = SNZB_STATE_ONLINE;

        if (is_valid_temperature(_sonoff_external_temperature_2))
          temperature_state_2 = SNZB_STATE_ONLINE;

        if (is_valid_temperature(_sonoff_external_humidity))
          humidity_state_1 = SNZB_STATE_ONLINE;

        if (is_valid_temperature(_sonoff_external_humidity_2))
          humidity_state_2 = SNZB_STATE_ONLINE;

        payload_size = build_snzb02dr2_payload_va(
          sonoff_buffer, sizeof(sonoff_buffer), 4, SNZB_TYPE_TEMPERATURE, 
          SNZB_SOURCE_1, temperature_state_1, _sonoff_external_temperature, 
          SNZB_TYPE_HUMIDITY, SNZB_SOURCE_1, humidity_state_1, 
          _sonoff_external_humidity, SNZB_TYPE_TEMPERATURE, SNZB_SOURCE_2,
          temperature_state_2, _sonoff_external_temperature_2, SNZB_TYPE_HUMIDITY, 
          SNZB_SOURCE_2, humidity_state_2, _sonoff_external_humidity_2);
            
        if (payload_size) {

          uint8_t temperature_selector = 1;

          zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, SONOFF_CUSTOM_CLUSTER, 
          SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT, 
          ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);

          
          zbGateway.sendAttributeWriteExt(
            _short_addr, _endpoint, SONOFF_CUSTOM_CLUSTER, 
            SONOFF_CUSTOM_CLUSTER_REMOTE_SENSOR_DATA, 
            ESP_ZB_ZCL_ATTR_TYPE_ARRAY, payload_size, sonoff_buffer, true, 1,
            SONOFF_MANUFACTURER_CODE);
        }
      
        /*zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, SONOFF_CUSTOM_CLUSTER, 
          SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT, 
          ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);

  
      if (_sonoff_external_temperature > INT16_MIN)
        zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, SONOFF_CUSTOM_CLUSTER, 
          SONOFF_CUSTOM_CLUSTER_EXTERNAL_TEMPERATURE_INPUT, 
          ESP_ZB_ZCL_ATTR_TYPE_S16, 2, &_sonoff_external_temperature);
  
      if (_sonoff_external_humidity < UINT16_MAX)
        zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, SONOFF_CUSTOM_CLUSTER, 
          SONOFF_CUSTOM_CLUSTER_EXTERNAL_HUMIDITY_INPUT, 
          ESP_ZB_ZCL_ATTR_TYPE_U16, 2, &_sonoff_external_humidity);*/
      } break;


      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL_EXT2: {

      uint8_t sonoff_flags = 0;

      if (_sonoff_external_temperature > INT16_MIN) 
        sonoff_flags |= SONOFF_FLAG_TEMPERATURE;

      if (_sonoff_external_humidity < UINT16_MAX)
        sonoff_flags |= SONOFF_FLAG_HUMIDITY;

      if (sonoff_flags) {

        uint8_t sonoff_buffer[32] = {};

        build_sonoff_payload(
          sonoff_buffer, sonoff_flags, _sonoff_external_temperature, 
          _sonoff_external_humidity, 0);

        zbGateway.sendAttributeWriteExt(
          _short_addr, _endpoint, SONOFF_CUSTOM_CLUSTER, 
          SONOFF_CUSTOM_CLUSTER_REMOTE_SENSOR_DATA, ESP_ZB_ZCL_ATTR_TYPE_ARRAY,
          sizeof(sonoff_buffer), sonoff_buffer, true);
      }
    } break;
  }
}

/*****************************************************************************/

  void iterateAlways() override {

    Z2S_VirtualThermHygroMeter::iterateAlways();

    if (getResentMs() && (millis() - _last_resent_ms > getResentMs())) {
      
      updateSNZB02DR2ExtValues();
      _last_resent_ms = millis();
    }       
  }

/*****************************************************************************/
  
 protected:

  int16_t  _sonoff_external_temperature = INT16_MIN;
  uint16_t  _sonoff_external_humidity = UINT16_MAX;

  int16_t  _sonoff_external_temperature_2 = INT16_MIN;
  uint16_t  _sonoff_external_humidity_2 = UINT16_MAX;

  uint32_t  _last_resent_ms = 0;
};

/*****************************************************************************/
/*****************************************************************************/

};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_
