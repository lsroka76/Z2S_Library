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
    
  Z2S_VirtualThermHygroMeter(bool rwns_flag = false) : _rwns_flag(rwns_flag) 
  {

  }

  void setRWNSFlag(bool rwns_flag) {

    _rwns_flag = rwns_flag;    
  }
  
  void setTimeoutSecs(uint32_t timeout_secs) {
    
    _timeout_ms = timeout_secs * 1000;
  }

  void Refresh() {
    _last_timeout_ms = millis();
    channel.setStateOnline();
  }

  void setTemperature(double val) {
    
    log_i("temperature = %f4.2", val);
    _forced_temperature = false;
    temperature = val;

    channel.setNewValue(temperature, getHumi());
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
    }


    if (_timeout_ms) {
      
      uint32_t _zb_device_last_seen_ms = getZbDeviceLastSeenMs();
      
      if (_zb_device_last_seen_ms > _last_timeout_ms) {

        _last_timeout_ms = _zb_device_last_seen_ms;
        channel.setStateOnline();
      }
      
      if ((millis_ms - _last_timeout_ms) > _timeout_ms) {
      
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

  uint32_t _timeout_ms = 0;
  uint32_t _last_timeout_ms = 0;
};

class Z2S_SNZB02DR2ThermHygroMeter : 
  public Supla::Sensor::Z2S_VirtualThermHygroMeter {
  
public:
    
  Z2S_SNZB02DR2ThermHygroMeter(bool rwns_flag = false) 
    : Z2S_VirtualThermHygroMeter(rwns_flag) 
  {
  }

  void setResentSecs(uint32_t resent_secs) {
    
    _resent_ms = resent_secs * 1000;
  }

  void setSonoffExternalTemperature(int16_t sonoff_external_temperature) {

    _sonoff_external_temperature = sonoff_external_temperature;
    _last_resent_ms = 0;
  }

  void setSonoffExternalHumidity(uint16_t sonoff_external_humidity) {

    _sonoff_external_humidity = sonoff_external_humidity;
    _last_resent_ms = 0;
  }

  void updateSNZB02DR2ExtValues() {

    if (!Zigbee.started())
      return;

    uint8_t temperature_selector = 1;

    zbg_device_params_t device = {};

    device.endpoint = 0x01;
    device.cluster_id = SONOFF_CUSTOM_CLUSTER;
    memcpy(
      device.ieee_addr, _z2s_channel->ieee_addr, sizeof(esp_zb_ieee_addr_t));
    device.short_addr = _z2s_channel->short_addr;
    device.model_id = _z2s_channel->model_id;

    zbGateway.sendAttributeWrite(
      &device, SONOFF_CUSTOM_CLUSTER, 
      SONOFF_CUSTOM_CLUSTER_TEMPERATURE_SENSOR_SELECT, 
      ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &temperature_selector);

  
    if (_sonoff_external_temperature > INT16_MIN)
      zbGateway.sendAttributeWrite(
        &device, SONOFF_CUSTOM_CLUSTER, 
        SONOFF_CUSTOM_CLUSTER_EXTERNAL_TEMPERATURE_INPUT, 
        ESP_ZB_ZCL_ATTR_TYPE_S16, 2, &_sonoff_external_temperature);
  
    if (_sonoff_external_humidity < UINT16_MAX)
      zbGateway.sendAttributeWrite(
        &device, SONOFF_CUSTOM_CLUSTER, 
        SONOFF_CUSTOM_CLUSTER_EXTERNAL_HUMIDITY_INPUT, 
        ESP_ZB_ZCL_ATTR_TYPE_U16, 2, &_sonoff_external_humidity);
  }

  void iterateAlways() override {

    VirtualThermHygroMeter::iterateAlways();

    if (_resent_ms && (millis() - _last_resent_ms > _resent_ms)) {
      
      updateSNZB02DR2ExtValues();
      _last_resent_ms = millis();
    }
       
  }

  uint32_t getID() override {
    
    return TH_ID_SNZB_02DR2;
  }
    
 protected:

  int16_t  _sonoff_external_temperature = INT16_MIN;
  uint16_t  _sonoff_external_humidity = UINT16_MAX;
  uint32_t  _resent_ms = 0;
  uint32_t  _last_resent_ms = 0;
};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_
