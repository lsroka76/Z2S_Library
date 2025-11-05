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

#include <supla/sensor/virtual_therm_hygro_meter.h>

namespace Supla {
namespace Sensor {
class Z2S_VirtualThermHygroMeter : public Supla::Sensor::VirtualThermHygroMeter {
  
public:
    
  Z2S_VirtualThermHygroMeter(bool rwns_flag = false) 
  : _rwns_flag(rwns_flag) {}


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

    if (millis() - lastReadTime > refreshIntervalMs) {
      lastReadTime = millis();
      channel.setNewValue(getTemp(), getHumi());
    }

    if ((_timeout_ms > 0) && (millis() - _last_timeout_ms > _timeout_ms)) {
      
      _last_timeout_ms = millis();

      if (_rwns_flag) 
        channel.setStateOfflineRemoteWakeupNotSupported();
      else
        channel.setStateOffline();
    }
  }


    
 protected:
  bool     _rwns_flag;
  bool     _forced_temperature = false;

  uint32_t _timeout_ms = 0;
  uint32_t _last_timeout_ms = 0;
};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_
