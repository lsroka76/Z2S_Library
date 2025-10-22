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

#ifndef Z2S_REMOTE_THERMOMETER_H_
#define Z2S_REMOTE_THERMOMETER_H_

#include <NetworkClient.h>
#include <ESPmDNS.h>

#include <supla/sensor/virtual_thermometer.h>

#define MSINHOUR (60*60*1000)
#define MINUTES_30 1800000

#define MAX_CONNECTED_THERMOMETERS 8

typedef struct connected_thermometers_s {

  uint32_t  connected_thermometer_ip_address;
  uint32_t  connected_thermometer_channel;
  int32_t   connected_thermometer_temperature;
  uint32_t  connected_thermometer_last_seen_ms;
} connected_thermometers_t;

extern NetworkClient Z2S_NetworkClient;
extern IPAddress     Z2S_IPAddress;


namespace Supla {
namespace Sensor {
class Z2S_RemoteThermometer : 
  public Supla::Sensor::VirtualThermometer {
  
public:
    
  Z2S_RemoteThermometer(bool rwns_flag = false) 
  : _rwns_flag(rwns_flag) {

    for(uint8_t connected_thermometers_counter = 0; 
        connected_thermometers_counter < MAX_CONNECTED_THERMOMETERS;
        connected_thermometers_counter++) 
      connected_thermometers[connected_thermometers_counter].
      connected_thermometer_channel = 0xFF;
  }
  
  void onInit() override {

  }


  void setRWNSFlag(bool rwns_flag) {

    _rwns_flag = rwns_flag;    
  }
  
  void setTimeoutSecs(uint32_t timeout_secs) {
    
    _timeout_ms = timeout_secs * 1000;
  }

  void setConnectedThermometerTemperature(
    uint32_t connected_thermometer_ip_address,
    uint32_t connected_thermometer_channel, 
    int32_t  connected_thermometer_temperature) {

    uint8_t connected_thermometers_free_slot = 0xFF;

    for(uint8_t connected_thermometers_counter = 0; 
        connected_thermometers_counter < MAX_CONNECTED_THERMOMETERS;
        connected_thermometers_counter++) {

      if ((connected_thermometers[connected_thermometers_counter].
           connected_thermometer_ip_address == 
           connected_thermometer_ip_address) &&
           (connected_thermometers[connected_thermometers_counter].
           connected_thermometer_channel == 
           connected_thermometer_channel)) {

        connected_thermometers[connected_thermometers_counter].
        connected_thermometer_temperature = 
          connected_thermometer_temperature;

        connected_thermometers[connected_thermometers_counter].
        connected_thermometer_last_seen_ms = millis();

        log_i("connected thermometer #%u updated\n\r"
              "IP %s, channel %u\n\r"
              "temperature %ld\n\r",
              connected_thermometers_counter,
              IPAddress(connected_thermometers[connected_thermometers_counter].
              connected_thermometer_ip_address).toString(),
              connected_thermometers[connected_thermometers_counter].
              connected_thermometer_channel,
              connected_thermometers[connected_thermometers_counter].
              connected_thermometer_temperature);
        return;
      }
      if ((connected_thermometers[connected_thermometers_counter].
           connected_thermometer_channel == 0xFF) &&
          (connected_thermometers_free_slot == 0xFF))
        connected_thermometers_free_slot = connected_thermometers_counter;
    }
    if (connected_thermometers_free_slot < 0xFF) {

      connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_ip_address = connected_thermometer_ip_address;

      connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_channel = connected_thermometer_channel;

      connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_temperature = connected_thermometer_temperature;

      connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_last_seen_ms = millis();

      log_i("new connected thermometer registered at%u\n\r"
              "IP %s, channel %u\n\r"
              "temperature %ld\n\r",
              connected_thermometers_free_slot,
              IPAddress(connected_thermometers[connected_thermometers_free_slot].
              connected_thermometer_ip_address).toString(),
              connected_thermometers[connected_thermometers_free_slot].
              connected_thermometer_channel,
              connected_thermometers[connected_thermometers_free_slot].
              connected_thermometer_temperature);
    } else
      log_e("maximum number of connected thermometers - skipping new data");
  }
  
  
  void Refresh() {
    
    _last_timeout_ms = millis();
    channel.setStateOnline();
  }

  void iterateAlways() override {
    
    uint32_t millis_ms = millis();
    
    if (millis_ms - lastReadTime > refreshIntervalMs) {
      
      lastReadTime = millis_ms;
      channel.setNewValue(getTemp());

    }

    if ((_timeout_ms > 0) && 
        (millis_ms - _last_timeout_ms > _timeout_ms)) {
      
      _last_timeout_ms = millis_ms;

      if (_rwns_flag) 
        channel.setStateOfflineRemoteWakeupNotSupported();
      else
        channel.setStateOffline();
    }

    int32_t connected_thermometers_calculated_temperature = 0;
    uint8_t connected_thermometers_number = 0; 

    for(uint8_t connected_thermometers_counter = 0; 
        connected_thermometers_counter < MAX_CONNECTED_THERMOMETERS;
        connected_thermometers_counter++) {

      if (connected_thermometers[connected_thermometers_counter].
           connected_thermometer_channel < 0xFF) {

        if ((millis_ms - connected_thermometers[connected_thermometers_counter].
           connected_thermometer_last_seen_ms) > 30000) { //MINUTES_30) { //unregister connected thermometer

          log_i("unregistering connected thermometer from IP %s, channel %u",
                IPAddress(connected_thermometers[connected_thermometers_counter].
                connected_thermometer_ip_address).toString(),
                connected_thermometers[connected_thermometers_counter].
                connected_thermometer_channel
              );

          connected_thermometers[connected_thermometers_counter].
          connected_thermometer_temperature = INT32_MIN;

          connected_thermometers[connected_thermometers_counter].
          connected_thermometer_channel = 0xFF;
          break; //skip that thermometer - already unregistered
        }

        connected_thermometers_number++;
        connected_thermometers_calculated_temperature += 
        connected_thermometers[connected_thermometers_counter].
        connected_thermometer_temperature;
      }
    }
    if (connected_thermometers_number > 0) 
      setValue(connected_thermometers_calculated_temperature / 
               (connected_thermometers_number * 100));
  }
    
 protected:
  bool     _rwns_flag;

  //uint8_t  _remote_Supla_channel = 0xFF;
  //uint32_t _remote_gateway_ip = 0;
  //char *_remote_gateway_mDNS_name = nullptr;

  connected_thermometers_t connected_thermometers[MAX_CONNECTED_THERMOMETERS];  

  uint32_t _timeout_ms = 0;
  uint32_t _last_timeout_ms = 0;
};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_REMOTE_THERMOMETER_H_
