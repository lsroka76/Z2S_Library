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

#ifndef Z2S_GENERAL_PURPOSE_MEASUREMENT_H_
#define Z2S_GENERAL_PURPOSE_MEASUREMENT_H_

#include <NetworkClient.h>
#include <ESPmDNS.h>

#include <supla/sensor/general_purpose_channel_base.h>

#include "Z2S_common.h"

#define MSINHOUR (60*60*1000)

//extern NetworkClient Z2S_NetworkClient;
//extern IPAddress     Z2S_IPAddress;


namespace Supla {
namespace Sensor {
class Z2S_GeneralPurposeMeasurement : public Supla::Sensor::GeneralPurposeChannelBase,
  public Z2S_Core {
  
public:
    
  Z2S_GeneralPurposeMeasurement(bool rwns_flag = false) : 
    GeneralPurposeChannelBase(nullptr, false), _rwns_flag(rwns_flag) {

    channel.setType(SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT);
    channel.setDefaultFunction(SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT);
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

  void setValue(double value) {
    
    _value = value;

    Refresh();
  }

  double getValue() {

    return _value;
  }

  void iterateAlways() override {
    
    GeneralPurposeChannelBase::iterateAlways();
    
    uint32_t millis_ms = millis();

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
  double _value = NAN;
  bool   _rwns_flag;

  uint32_t _timeout_ms = 0;
  uint32_t _last_timeout_ms = 0;
};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_GENERAL_PURPOSE_MEASUREMENT_H_
