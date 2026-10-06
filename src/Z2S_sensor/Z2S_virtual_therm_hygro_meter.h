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

#include <supla/sensor/virtual_therm_hygro_meter.h>

#define TH_ID_SNZB_02DR2        0x1000

namespace Supla {
namespace Sensor {
class Z2S_VirtualThermHygroMeter : 
  public Supla::Sensor::VirtualThermHygroMeter, public Z2S_Core {
  
public:
    
  Z2S_VirtualThermHygroMeter(bool rwns_flag = false) 
    : Z2S_Core(this), _rwns_flag(rwns_flag) {
  }

  void setRWNSFlag(bool rwns_flag) {

    _rwns_flag = rwns_flag;    
  }
  

  void Refresh() {

    _last_timeout_ms = millis();
    channel.setStateOnline();
  }

  void setTemperature(double val) {
    
    log_i("temperature = %f4.2", val);
    _forced_temperature = false;
    temperature = val;

    lastReadTime = 0;

    //channel.setNewValue(temperature, getHumi());
    Refresh();
  }

  void setHumidity(double val) {
    
    log_i("humidity = %f4.2", val);
    humidity = val;

    lastReadTime = 0;

    //channel.setNewValue(temperature, getHumi());
    Refresh();
  }

  void setForcedTemperature(double val) {
    
    log_i("temperature = %f4.2", val);
    _forced_temperature = true;
    temperature = val;

    Refresh();
  }

  bool isForcedTemperature() {

    return _forced_temperature;
  }

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
    
 protected:
  bool     _rwns_flag;
  bool     _forced_temperature = false;

  uint32_t _last_timeout_ms = 0;
};

/*****************************************************************************/
//#include <stdarg.h>
//#include <stdint.h>
//#include <math.h>

/**
 * Builds the ZCL Array Write Attribute frame for eWeLink remote sensor data.
 *
 * @param buffer Pointer to the destination byte array (must be at least 32 bytes).
 * @param count Number of sensor arguments provided:
 *              1 = Temperature only
 *              2 = Temperature + Humidity
 *              3 = Temperature + Humidity + Pressure
 * @param ... Floating-point values (float or double) in order: temp (°C), humi (%), press (hPa).
 * @return Total bytes written to buffer.
 */

uint16_t build_ewelink_payload(uint8_t *buffer, uint8_t count, ...) {
    
    if (!buffer || count == 0 || count > 3) 
        return 0;
    

    
    buffer[0] = ESP_ZB_ZCL_ATTR_TYPE_U8;

    // buffer[1] and buffer[2] reserved for Array Element Count (uint16_le)

    // 2. Inner eWeLink Header
    buffer[3] = 0x01; // Prefix byte 0
    buffer[4] = 0x01; // Prefix byte 1
    buffer[5] = 0x00; // Prefix byte 2
    buffer[6] = 0x03; // Prefix byte 3

    // buffer[7] reserved for Inner Length L (1 + item_bytes)
    buffer[8] = count; 

    uint16_t idx = 9; // Start index for sensor item blocks

    va_list args;
    va_start(args, count);

    // --- 1. Temperature (always present if count >= 1) ---
    if (count >= 1) {
        
      double temp = va_arg(args, double);
      int16_t scaled = (int16_t)round(temp * 100.0);

      buffer[idx++] = 0x00; // Type: Temperature
      buffer[idx++] = 0x00; // Sensor ID: 0
      buffer[idx++] = 0x01; // State: Enabled
      buffer[idx++] = 0x02; // Value Length: 2 bytes

      buffer[idx++] = (uint8_t)(scaled & 0xFF);        
      buffer[idx++] = (uint8_t)((scaled >> 8) & 0xFF); 
    }

    // --- 2. Humidity (present if count >= 2) ---
    if (count >= 2) {

      double humi = va_arg(args, double);
      uint16_t scaled = (uint16_t)round(humi * 100.0);

      buffer[idx++] = 0x01; // Type: Humidity
      buffer[idx++] = 0x00; // Sensor ID: 0
      buffer[idx++] = 0x01; // State: Enabled
      buffer[idx++] = 0x02; // Value Length: 2 bytes

      buffer[idx++] = (uint8_t)(scaled & 0xFF);        
      buffer[idx++] = (uint8_t)((scaled >> 8) & 0xFF); 
    }

    // --- 3. Pressure (present if count >= 3) ---
    if (count >= 3) {

      double press = va_arg(args, double);
      int32_t scaled = (int32_t)round(press * 100.0);

      buffer[idx++] = 0x02; // Type: Pressure
      buffer[idx++] = 0x00; // Sensor ID: 0
      buffer[idx++] = 0x01; // State: Enabled
      buffer[idx++] = 0x04; // Value Length: 4 bytes

      buffer[idx++] = (uint8_t)(scaled & 0xFF);
      buffer[idx++] = (uint8_t)((scaled >> 8) & 0xFF);
      buffer[idx++] = (uint8_t)((scaled >> 16) & 0xFF);
      buffer[idx++] = (uint8_t)((scaled >> 24) & 0xFF);
    }

    va_end(args);

    // Calculate length headers
    uint16_t total_item_bytes = idx - 9;
    buffer[7] = (uint8_t)(1 + total_item_bytes); // eWeLink Inner Length L

    uint16_t inner_payload_len = idx - 3;
    buffer[1] = (uint8_t)(inner_payload_len & 0xFF);       
    buffer[2] = (uint8_t)((inner_payload_len >> 8) & 0xFF);

    return idx;
}

/*****************************************************************************/
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

  void updateSNZB02DR2ExtValues() {

    if (!Zigbee.started())
      return;

    uint8_t temperature_selector = 1;

    zbGateway.sendAttributeWrite(
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
        ESP_ZB_ZCL_ATTR_TYPE_U16, 2, &_sonoff_external_humidity);
  }

/*****************************************************************************/

  void iterateAlways() override {

    VirtualThermHygroMeter::iterateAlways();

    if (getResentMs() && (millis() - _last_resent_ms > getResentMs())) {
      
      updateSNZB02DR2ExtValues();
      _last_resent_ms = millis();
    }       
  }

/*****************************************************************************/
  
 protected:

  int16_t  _sonoff_external_temperature = INT16_MIN;
  uint16_t  _sonoff_external_humidity = UINT16_MAX;

  uint32_t  _last_resent_ms = 0;
};

/*****************************************************************************/
/*****************************************************************************/

};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_
