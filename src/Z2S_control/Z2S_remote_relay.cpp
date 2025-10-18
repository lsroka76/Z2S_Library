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

#include "Z2S_remote_relay.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

#include <supla/time.h>

Supla::Control::Z2S_RemoteRelay::Z2S_RemoteRelay(
  NetworkClient *remote_gateway, uint8_t remote_Supla_channel) :
    Relay(-1, true, 0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER), 
    _remote_gateway(remote_gateway), 
    _remote_Supla_channel(remote_Supla_channel) {

}

void Supla::Control::Z2S_RemoteRelay::onInit() {
  
  /*uint32_t duration = durationMs;
  if (stateOnInit == STATE_ON_INIT_ON ||
      stateOnInit == STATE_ON_INIT_RESTORED_ON) {
    turnOn(duration);
  } else {
    turnOff(duration);
  }*/
  //if (_timeout_enabled)
  //channel.setStateOffline();
}

void Supla::Control::Z2S_RemoteRelay::turnOn(_supla_int_t duration) {
  
  SUPLA_LOG_INFO(
      "Relay[%d] turn ON (duration %d ms)",
      channel.getChannelNumber(),
      duration);

  durationMs = duration;
  
  if (minimumAllowedDurationMs > 0 && storedTurnOnDurationMs == 0) {
    storedTurnOnDurationMs = durationMs;
  }

  if (keepTurnOnDurationMs || isStaircaseFunction() || isImpulseFunction()) {
    durationMs = storedTurnOnDurationMs;
  }

  if (durationMs != 0) {

    durationTimestamp = millis();
  } else {

    durationTimestamp = 0;
  }

  if ((_remote_gateway) &&
      (_remote_Supla_channel < 0xFF)) {
    if (_remote_gateway_mDNS_name)
      _remote_gateway_ip = MDNS.queryHost(_remote_gateway_mDNS_name);

    _remote_gateway->connect(_remote_gateway_ip, 1234);
    
    _remote_gateway->printf("Z2SCMD%02u%03u%02u\n", 
                            REMOTE_CMD_TURN_ON, 
                            _remote_Supla_channel,
                            0xAB);
    
    String response = _remote_gateway->readStringUntil('\n');
    
    if (response == "OK") {

      state = true;
      channel.setNewValue(state);
      // Schedule save in 5 s after state change
      Supla::Storage::ScheduleSave(5000);
    }
    _remote_gateway->stop();
  } 
}

void Supla::Control::Z2S_RemoteRelay::turnOff(_supla_int_t duration) {
  
  SUPLA_LOG_INFO(
      "Relay[%d] turn OFF (duration %d ms)",
      channel.getChannelNumber(),
      duration);
  
  durationMs = duration;
  
  if (durationMs != 0) {
    
    durationTimestamp = millis();
  } else {
    
    durationTimestamp = 0;
  }

  if ((_remote_gateway) &&
      (_remote_Supla_channel < 0xFF)) {

    if (_remote_gateway_mDNS_name)
      _remote_gateway_ip = MDNS.queryHost(_remote_gateway_mDNS_name);

    _remote_gateway->connect(_remote_gateway_ip, 1234);

    _remote_gateway->printf("Z2SCMD%02u%03u%02u\n", 
                            REMOTE_CMD_TURN_OFF, 
                            _remote_Supla_channel,
                            0xAB);
    
    String response = _remote_gateway->readStringUntil('\n');
    
    if (response == "OK") {

      state = false;
      channel.setNewValue(state);
      // Schedule save in 5 s after state change
      Supla::Storage::ScheduleSave(5000);
    }
    _remote_gateway->stop();
  }
}

void Supla::Control::Z2S_RemoteRelay::ping() {

  
}

void Supla::Control::Z2S_RemoteRelay::iterateAlways() {

  Supla::Control::Relay::iterateAlways();

  /*if (_remote_gateway &&
      _remote_gateway->connected()) {

    if (!channel.isStateOnline()) 
	  channel.setStateOnline();
  } else {

    channel.setStateOffline();
  }*/
}

bool Supla::Control::Z2S_RemoteRelay::isOn() {
  
   return state;
}

void Supla::Control::Z2S_RemoteRelay::Z2S_setOnOff(bool on_off_state) {
  
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();

  log_i("durationMs = %lu, "
        "storedTurnOnDurationMs = %lu, "
        "durationTimestamp = %lu, "
        "keepTurnOnDurationMs = %u",
        durationMs, 
        storedTurnOnDurationMs, 
        durationTimestamp, 
        keepTurnOnDurationMs);

  if (state != on_off_state) {

    if (on_off_state) {
      
      if (minimumAllowedDurationMs > 0 && storedTurnOnDurationMs == 0) {
       storedTurnOnDurationMs = durationMs;
      }

      if (keepTurnOnDurationMs || isStaircaseFunction() || isImpulseFunction()) {
        durationMs = storedTurnOnDurationMs;
      }
      if (durationMs != 0) {

        durationTimestamp = millis();
      } else {

        durationTimestamp = 0;
      }
    } else {
      
      durationMs = 0;
      durationTimestamp = 0;
    }
  }
  
  state = on_off_state;

  channel.setNewValue(state);
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

/*void Supla::Control::Z2S_RemoteRelay::setKeepAliveSecs(uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
  if (_keep_alive_ms == 0)
    _keep_alive_enabled = false;
  else 
    _keep_alive_enabled = true;
}

void Supla::Control::Z2S_RemoteRelay::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    channel.setStateOnline();
  }
  else
   _timeout_enabled = true;
}

uint32_t Supla::Control::Z2S_RemoteRelay::getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t Supla::Control::Z2S_RemoteRelay::getTimeoutSecs() {

  return _timeout_ms / 1000;
}*/

//#endif
