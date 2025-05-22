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

#include "Z2S_virtual_relay.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

#include <supla/time.h>

Supla::Control::Z2S_VirtualRelay::Z2S_VirtualRelay(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t z2s_function)
  : Relay(-1, true, 0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER), _gateway(gateway), _z2s_function(z2s_function) {
    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Supla::Control::Z2S_VirtualRelay::onInit() {
  /*uint32_t duration = durationMs;
  if (stateOnInit == STATE_ON_INIT_ON ||
      stateOnInit == STATE_ON_INIT_RESTORED_ON) {
    turnOn(duration);
  } else {
    turnOff(duration);
  }*/
  if (_timeout_enabled)
    channel.setStateOffline();
}

void Supla::Control::Z2S_VirtualRelay::turnOn(_supla_int_t duration) {
  SUPLA_LOG_INFO(
      "Relay[%d] turn ON (duration %d ms)",
      channel.getChannelNumber(),
      duration);
  durationMs = duration;
  if (keepTurnOnDurationMs) {
    durationMs = storedTurnOnDurationMs;
  }
  if (durationMs != 0) {
    durationTimestamp = millis();
  } else {
    durationTimestamp = 0;
  }

  if (_gateway && Zigbee.started()) { 
    switch (_z2s_function) {

      case Z2S_VIRTUAL_RELAY_FNC_NONE: {
        
        state = true;
        _gateway->sendOnOffCmd(&_device, state);
        channel.setNewValue(state);
      } break;

      case Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM:
      case Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM: {
        state = true;
        
        if (_z2s_function == Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM)
          _z2s_function_data[0] = 0x25; // emergency + strobe + no sound
        else
          _z2s_function_data[0] = 0x37; // emergency + strobe + very high level sound
        _z2s_function_data[1] = 0xFF;
        _z2s_function_data[2] = 0xFE; //max duration lenght
        _z2s_function_data[3] = 0x32; //strobe duty cycle 50/50
        _z2s_function_data[4] = 0x01; //strobe level field

        _gateway->sendCustomClusterCmd(&_device, 0x0502, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 0x05, _z2s_function_data);
        _gateway->sendOnOffCmd(&_device, state);

        channel.setNewValue(state);

      } break;
    }  
  }
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void Supla::Control::Z2S_VirtualRelay::turnOff(_supla_int_t duration) {
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
  if (_gateway && Zigbee.started()) { 
    switch (_z2s_function) {

      case Z2S_VIRTUAL_RELAY_FNC_NONE: {
        state = false;
        _gateway->sendOnOffCmd(&_device, state);
        channel.setNewValue(state);
      } break;

      case Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM:
      case Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM: {

        state = false;
        
        _z2s_function_data[0] = 0x00; //no warning, no strobe, no siren
        _z2s_function_data[1] = 0x00;
        _z2s_function_data[2] = 0x00; //duration lenght
        _z2s_function_data[3] = 0x00; //strobe duty cycle
        _z2s_function_data[4] = 0x00; //strobe level field

        _gateway->sendOnOffCmd(&_device, state);
        _gateway->sendCustomClusterCmd(&_device, 0x0502, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 0x05, _z2s_function_data);
        channel.setNewValue(state);

      } break;
    }  
  }
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void Supla::Control::Z2S_VirtualRelay::ping() {

  if (_gateway && Zigbee.started()) {
    _fresh_start = false;
    _gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);
  }
}

void Supla::Control::Z2S_VirtualRelay::iterateAlways() {

  Supla::Control::Relay::iterateAlways();

  //uint32_t current_millis = millis();

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (_keep_alive_enabled && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (_gateway) {
      
      _last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
      if ((millis() - _last_seen_ms) > _keep_alive_ms) {
      	ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        if (!channel.isStateOnline()) 
	  channel.setStateOnline();
      }
    }
  }
  if (_timeout_enabled && channel.isStateOnline() && ((millis() - _last_seen_ms) > _timeout_ms)) {
	  log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);
    _last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);
    if ((millis() - _last_seen_ms) > _timeout_ms)
      channel.setStateOffline();
  }
}

bool Supla::Control::Z2S_VirtualRelay::isOn() {
  
  if (_gateway && Zigbee.started()) {   
     if (_gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true))
         state = *((bool *)_gateway->getReadAttrLastResult()->data.value);
  }
   return state;
}

void Supla::Control::Z2S_VirtualRelay::Z2S_setOnOff(bool on_off_state) {
  
  state = on_off_state;

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();

  channel.setNewValue(state);
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void Supla::Control::Z2S_VirtualRelay::setKeepAliveSecs(uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
  if (_keep_alive_ms == 0)
    _keep_alive_enabled = false;
  else 
    _keep_alive_enabled = true;
}

void Supla::Control::Z2S_VirtualRelay::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    channel.setStateOnline();
  }
  else
   _timeout_enabled = true;
}

uint32_t Supla::Control::Z2S_VirtualRelay::getKeepAliveSecs() {

  return _keep_alive_ms * 1000;
}

uint32_t Supla::Control::Z2S_VirtualRelay::getTimeoutSecs() {

  return _timeout_ms * 1000;
}

//#endif
