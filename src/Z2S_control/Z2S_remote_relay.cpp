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

#include <Z2S_custom_actions_events.h>
#include <supla/events.h>

#include <supla/time.h>

Supla::Control::Z2S_RemoteRelay::Z2S_RemoteRelay(
  NetworkClient *remote_gateway, uint8_t remote_Supla_channel) 
  : Relay(-1, true, 0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER), 
    Z2S_Core(this) {
}

void Supla::Control::Z2S_RemoteRelay::onInit() {
  
  initDone = true;
}

bool Supla::Control::Z2S_RemoteRelay::connectRemoteGateway() {

  log_i("getMDNSName() = %s", getMDNSName());
  
  if (getSuplaRemoteChannel() < 0xFF) {

    if(getRemoteIPAddress() == 0) {

      if (getMDNSName())
        setRemoteIPAddress(MDNS.queryHost(getMDNSName(), 500));
      else
        return false;

      if(getRemoteIPAddress() == 0)
        return false;
    }

    if (!Z2S_NetworkClient.connect(getRemoteIPAddress(), REMOTE_RELAY_PORT, 500)) {

      if (getMDNSName())
        setRemoteIPAddress(MDNS.queryHost(getMDNSName(), 500));
      else
        return false;

      if (!Z2S_NetworkClient.connect(getRemoteIPAddress(), REMOTE_RELAY_PORT, 500))
      return false;
    } else   
      return true;
  }
  return false;
}

void Supla::Control::Z2S_RemoteRelay::postponeTurnOnOff(bool pending_state) {

  if ((_retries_number > 0) &&
      (_current_retry == 0)) {

    _current_retry = _retries_number;
    _retry_time_ms = millis();
    _pending_state = pending_state;
  }
}

void Supla::Control::Z2S_RemoteRelay::turnOn(_supla_int_t duration) {
  
  SUPLA_LOG_INFO(
      "Relay[%d] turn ON (duration %d ms)",channel.getChannelNumber(),
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

  if (connectRemoteGateway()) {

    Z2S_NetworkClient.printf(
      "Z2SCMD%02u%03u%08ld\n", REMOTE_CMD_TURN_ON, getSuplaRemoteChannel(),
      -12567);
    
    String response = Z2S_NetworkClient.readStringUntil('\n');
    if (response == "OK") {

      _current_retry = 0;
      state = true;
      channel.setNewValue(state);
      // Schedule save in 5 s after state change
      Supla::Storage::ScheduleSave(5000);

      Z2S_NetworkClient.stop();
      return;
    } 
    Z2S_NetworkClient.stop();
  }
  postponeTurnOnOff(true); 
}

void Supla::Control::Z2S_RemoteRelay::turnOff(_supla_int_t duration) {
  
  SUPLA_LOG_INFO(
      "Relay[%d] turn OFF (duration %d ms)", channel.getChannelNumber(),
      duration);
  
  durationMs = duration;
  
  if (durationMs != 0) {
    
    durationTimestamp = millis();
  } else {
    
    durationTimestamp = 0;
  }

  if (connectRemoteGateway()) {

    Z2S_NetworkClient.printf(
      "Z2SCMD%02u%03u%08ld\n", REMOTE_CMD_TURN_OFF, getSuplaRemoteChannel(),
      0xAB);
    
    String response = Z2S_NetworkClient.readStringUntil('\n');
    
    if (response == "OK") {

      _current_retry = 0;
      state = false;
      channel.setNewValue(state);
      // Schedule save in 5 s after state change
      Supla::Storage::ScheduleSave(5000);

      Z2S_NetworkClient.stop();
      return;
    }
    Z2S_NetworkClient.stop();
  }

  postponeTurnOnOff(false);
}

void Supla::Control::Z2S_RemoteRelay::ping() {

  
}

void Supla::Control::Z2S_RemoteRelay::iterateAlways() {

  Supla::Control::Relay::iterateAlways();

  uint32_t millis_ms = millis();

  if ((_current_retry > 0) &&
      (millis_ms - _retry_time_ms > _retry_pause_ms)) {

    log_i("_current_retry %u, _retry_time_ms %lu, millis_ms %lu",
          _current_retry, _retry_time_ms, millis_ms);

    _retry_time_ms = millis_ms;
    _current_retry--;
    
    if (_pending_state)
      turnOn();
    else 
      turnOff();
    }
}

void Supla::Control::Z2S_RemoteRelay::handleAction(int event, int action)  {

  Supla::Control::Relay::handleAction(event, action);

  log_i("event %u, action %u", event, action);

  switch (action) {


    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:

      if (state) 
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
    break;


    case Z2S_SUPLA_ACTION_RESYNC_REMOTE_RELAY:

      if (state) 
        turnOn();
      else
        turnOff();
    break;


    case Z2S_SUPLA_ACTION_COPY_ON_STATE:

      if (state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
    break;


    case Z2S_SUPLA_ACTION_COPY_OFF_STATE:

      if (!state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
    break;


    case Z2S_SUPLA_ACTION_SET_RELAY_STATE_DISABLED:

      channel.setStateOffline();
    break;
  }  
}

bool Supla::Control::Z2S_RemoteRelay::isOn() {
  
   return state;
}

void Supla::Control::Z2S_RemoteRelay::Z2S_setOnOff(bool on_off_state) {
  
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


//#endif
