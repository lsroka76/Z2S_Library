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

#include "Z2S_virtual_thermometer.h"

#define MSINHOUR (60*60*1000)
#define MINUTES_30 1800000
//#define MINUTES_30

#define CONNECTED_THERMOMETERS_FNC_MIN  0x01
#define CONNECTED_THERMOMETERS_FNC_AVG  0x02
#define CONNECTED_THERMOMETERS_FNC_MAX  0x03

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
  public Supla::Sensor::Z2S_VirtualThermometer {
  
public:
    
  Z2S_RemoteThermometer(bool rwns_flag = false) 
  : _rwns_flag(rwns_flag) {

    for(uint8_t connected_thermometers_counter = 0; 
        connected_thermometers_counter < MAX_CONNECTED_THERMOMETERS;
        connected_thermometers_counter++) 
      _connected_thermometers[connected_thermometers_counter].
      connected_thermometer_channel = 0xFF;
  }
  
  void onInit() override {

    channel.setNewValue(getTemp());
  }

  void setRWNSFlag(bool rwns_flag) {

    _rwns_flag = rwns_flag;    
  }
  
  void setTimeoutSecs(uint32_t timeout_secs) {
    
    _timeout_ms = timeout_secs * 1000;
  }


  void setRawValue(double val) {

    log_i("setRawValue");
    temperature = val;
  }

  /*void setTemperature(double val) {
    
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
  }*/


  void setConnectedThermometersFunction(
    uint32_t connected_thermometers_function) {

      _connected_thermometers_function = 
        connected_thermometers_function;
    }
    
    void setConnectedThermometerTimeoutSecs(
      uint32_t connected_thermometer_timeout_secs) {

      if (connected_thermometer_timeout_secs > 0)
        _connected_thermometer_timeout_ms = 
          connected_thermometer_timeout_secs * 1000;
      else
        _connected_thermometer_timeout_ms = MINUTES_30;
    }

  void setConnectedThermometerTemperature(
    uint32_t connected_thermometer_ip_address,
    uint32_t connected_thermometer_channel, 
    int32_t  connected_thermometer_temperature) {

    uint8_t connected_thermometers_free_slot = 0xFF;

    for(uint8_t connected_thermometers_counter = 0; 
        connected_thermometers_counter < MAX_CONNECTED_THERMOMETERS;
        connected_thermometers_counter++) {

      if ((_connected_thermometers[connected_thermometers_counter].
           connected_thermometer_ip_address == 
           connected_thermometer_ip_address) &&
           (_connected_thermometers[connected_thermometers_counter].
           connected_thermometer_channel == 
           connected_thermometer_channel)) {

        _connected_thermometers[connected_thermometers_counter].
        connected_thermometer_temperature = 
          connected_thermometer_temperature;

        _connected_thermometers[connected_thermometers_counter].
        connected_thermometer_last_seen_ms = millis();

        log_i("connected thermometer #%u updated\n\r"
              "IP %s, channel %u\n\r"
              "temperature %ld\n\r",
              connected_thermometers_counter,
              IPAddress(_connected_thermometers[connected_thermometers_counter].
              connected_thermometer_ip_address).toString(),
              _connected_thermometers[connected_thermometers_counter].
              connected_thermometer_channel,
              _connected_thermometers[connected_thermometers_counter].
              connected_thermometer_temperature);
        return;
      }
      if ((_connected_thermometers[connected_thermometers_counter].
           connected_thermometer_channel == 0xFF) &&
          (connected_thermometers_free_slot == 0xFF))
        connected_thermometers_free_slot = connected_thermometers_counter;
    }
    if (connected_thermometers_free_slot < 0xFF) {

      _connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_ip_address = connected_thermometer_ip_address;

      _connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_channel = connected_thermometer_channel;

      _connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_temperature = connected_thermometer_temperature;

      _connected_thermometers[connected_thermometers_free_slot].
      connected_thermometer_last_seen_ms = millis();

      log_i("new connected thermometer registered at%u\n\r"
              "IP %s, channel %u\n\r"
              "temperature %ld\n\r",
              connected_thermometers_free_slot,
              IPAddress(_connected_thermometers[connected_thermometers_free_slot].
              connected_thermometer_ip_address).toString(),
              _connected_thermometers[connected_thermometers_free_slot].
              connected_thermometer_channel,
              _connected_thermometers[connected_thermometers_free_slot].
              connected_thermometer_temperature);
    } else
      log_e("maximum number of connected thermometers - skipping new data");
  }
  
  
  void Refresh() {
    
    _last_timeout_ms = millis();
    channel.setStateOnline();
  }

  void scanConnectedThermometers(uint32_t millis_ms) {

    int32_t connected_thermometers_calculated_temperature = INT32_MIN;
    int32_t special_thermometer_calculated_temperature = INT32_MIN;

    uint8_t connected_thermometers_number = 0; 
    uint8_t connected_thermometer_channel = 0xFF;

    for(uint8_t connected_thermometers_counter = 0; 
        connected_thermometers_counter < MAX_CONNECTED_THERMOMETERS;
        connected_thermometers_counter++) {      

      connected_thermometer_channel =
        _connected_thermometers[connected_thermometers_counter].\
          connected_thermometer_channel;

      log_i("\n\rcounter = %d"
            "\n\rchannel = %d"
            "\n\rtemperature = %d",
            connected_thermometers_counter,
            connected_thermometer_channel,
            _connected_thermometers[connected_thermometers_counter].\
              connected_thermometer_temperature);

      if ((connected_thermometer_channel > 0x7F) &&
          (connected_thermometer_channel < 0xFF)) {

        special_thermometer_calculated_temperature = 
          _connected_thermometers[connected_thermometers_counter].\
            connected_thermometer_temperature;
        break;
      }

      if (connected_thermometer_channel < 0x7F) {

        if ((millis_ms - _connected_thermometers[connected_thermometers_counter].\
                           connected_thermometer_last_seen_ms) > 
            _connected_thermometer_timeout_ms) { //unregister connected thermometer

          log_i("unregistering connected thermometer from IP %s, channel %u",
                IPAddress(_connected_thermometers[connected_thermometers_counter].
                connected_thermometer_ip_address).toString(),
                connected_thermometer_channel
              );

          _connected_thermometers[connected_thermometers_counter].
          connected_thermometer_temperature = INT32_MIN;

          _connected_thermometers[connected_thermometers_counter].
          connected_thermometer_channel = 0xFF;
          break; //skip that thermometer - already unregistered
        }

        switch (_connected_thermometers_function) {


          case CONNECTED_THERMOMETERS_FNC_MIN: {

            connected_thermometers_number = 1;

            if ((connected_thermometers_calculated_temperature == INT32_MIN) ||
                (_connected_thermometers[connected_thermometers_counter].
                connected_thermometer_temperature < 
                connected_thermometers_calculated_temperature))

              connected_thermometers_calculated_temperature = 
                _connected_thermometers[connected_thermometers_counter].
                  connected_thermometer_temperature;
          } break;
          
          
          case CONNECTED_THERMOMETERS_FNC_AVG: {
            
            connected_thermometers_number++;

            if (connected_thermometers_calculated_temperature == INT32_MIN)
              connected_thermometers_calculated_temperature = 
                _connected_thermometers[connected_thermometers_counter].
                  connected_thermometer_temperature;
            else
              connected_thermometers_calculated_temperature += 
                _connected_thermometers[connected_thermometers_counter].
                  connected_thermometer_temperature;
          } break;


          case CONNECTED_THERMOMETERS_FNC_MAX: {

            connected_thermometers_number = 1;

            if (_connected_thermometers[connected_thermometers_counter].
                connected_thermometer_temperature > 
                connected_thermometers_calculated_temperature)

              connected_thermometers_calculated_temperature = 
                _connected_thermometers[connected_thermometers_counter].
                  connected_thermometer_temperature;
          } break;
        } //switch (_connected_thermometers_function)
      }
    }
    if (connected_thermometers_number > 0) {
      
      setTemperature((double)(connected_thermometers_calculated_temperature) / 
               (connected_thermometers_number * 100));
      _connected_thermometers_updated = true;
    } else {
      
      if (_connected_thermometers_updated) {

        _connected_thermometers_updated = false;
        setForcedTemperature(TEMPERATURE_NOT_AVAILABLE);
      } else
      setForcedTemperature(temperature);
      //if (temperature > TEMPERATURE_NOT_AVAILABLE)//(special_thermometer_calculated_temperature > INT32_MIN)
      //setRawValue(temperature); //setRawValue((double)(special_thermometer_calculated_temperature) / 100);
    }
    //else
      //setRawValue(TEMPERATURE_NOT_AVAILABLE);
  }

  void iterateAlways() override {
    
    uint32_t millis_ms = millis();
    
    if (millis_ms - lastReadTime > refreshIntervalMs) {
      
      scanConnectedThermometers(millis_ms);    
      
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
  }
    
 protected:
  bool     _rwns_flag;

  //uint8_t  _remote_Supla_channel = 0xFF;
  //uint32_t _remote_gateway_ip = 0;
  //char *_remote_gateway_mDNS_name = nullptr;

  uint32_t  _connected_thermometers_function = CONNECTED_THERMOMETERS_FNC_AVG;
  connected_thermometers_t _connected_thermometers[MAX_CONNECTED_THERMOMETERS];  

  bool _connected_thermometers_updated = false;

  uint32_t _connected_thermometer_timeout_ms = MINUTES_30; //-> channel refresh?

  uint32_t _timeout_ms = 0; //-> channel timeout
  uint32_t _last_timeout_ms = 0;
};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_REMOTE_THERMOMETER_H_
