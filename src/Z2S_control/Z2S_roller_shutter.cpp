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

#include "Z2S_roller_shutter.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

#include <supla/time.h>

Supla::Control::Z2S_RollerShutter::Z2S_RollerShutter(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t z2s_function)
  : _gateway(gateway), _z2s_function(z2s_function) {
      memcpy(&_device, device, sizeof(zbg_device_params_t));     
      }
      
      void Supla::Control::Z2S_RollerShutter::onInit() {
        /*uint32_t duration = durationMs;
        ::Z2S_RollerShutter(ZigbeeGateway *gateway, zbg_device_params_t *device, _supla_int_t functions)
  : Relay(-1, true, functions), _gateway(gateway){
    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

void Supla::Control::Z2S_RollerShutter::onInit() {
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

void Supla::Control::Z2S_RollerShutter::rsOpen() {

  if (_gateway && Zigbee.started()) {   
    
    switch (_z2s_function) {

      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
        _gateway->sendWindowCoveringCmd(&_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_UP_OPEN, NULL); break;

      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        uint8_t MOES_SHADES_DRIVE_MOTOR_OPEN_CMD[] = { 00, 00, 01, 04, 00, 01, 02 };

        uint16_t _tsn_number = random(0x0000, 0xFFFF); 

        MOES_SHADES_DRIVE_MOTOR_OPEN_CMD[0] = (_tsn_number & 0xFF00);
        MOES_SHADES_DRIVE_MOTOR_OPEN_CMD[1] = (_tsn_number & 0x00FF);

        _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, MOES_SHADES_DRIVE_MOTOR_OPEN_CMD, false);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::rsClose() {

  if (_gateway && Zigbee.started()) {   

    switch (_z2s_function) {

      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
        _gateway->sendWindowCoveringCmd(&_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_DOWN_CLOSE, NULL); break;

      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        uint8_t MOES_SHADES_DRIVE_MOTOR_CLOSE_CMD[] = { 00, 00, 01, 04, 00, 01, 00 };

        uint16_t _tsn_number = random(0x0000, 0xFFFF); 

        MOES_SHADES_DRIVE_MOTOR_CLOSE_CMD[0] = (_tsn_number & 0xFF00);
        MOES_SHADES_DRIVE_MOTOR_CLOSE_CMD[1] = (_tsn_number & 0x00FF);

        _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, MOES_SHADES_DRIVE_MOTOR_CLOSE_CMD, false);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::rsStop() {

  if (_gateway && Zigbee.started()) {   
    
    switch (_z2s_function) {

      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
        _gateway->sendWindowCoveringCmd(&_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_STOP, NULL); break;

      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        uint8_t MOES_SHADES_DRIVE_MOTOR_STOP_CMD[] = { 00, 00, 01, 04, 00, 01, 01 };

        uint16_t _tsn_number = random(0x0000, 0xFFFF); 

        MOES_SHADES_DRIVE_MOTOR_STOP_CMD[0] = (_tsn_number & 0xFF00);
        MOES_SHADES_DRIVE_MOTOR_STOP_CMD[1] = (_tsn_number & 0x00FF);

        _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, MOES_SHADES_DRIVE_MOTOR_STOP_CMD, false);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::rsMoveToLiftPercentage(uint8_t lift_percentage) {

  if (_gateway && Zigbee.started()) {   

    switch (_z2s_function) {

      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
        _gateway->sendWindowCoveringCmd(&_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE, &lift_percentage); break;

      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        uint8_t MOES_SHADES_DRIVE_MOTOR_POSITION_PERCENTAGE_CMD[] = { 00, 00, 02, 02, 00, 04, 00, 00, 00, 00 };

        uint16_t _tsn_number = random(0x0000, 0xFFFF); 

        MOES_SHADES_DRIVE_MOTOR_POSITION_PERCENTAGE_CMD[0] = (_tsn_number & 0xFF00);
        MOES_SHADES_DRIVE_MOTOR_POSITION_PERCENTAGE_CMD[1] = (_tsn_number & 0x00FF);
        MOES_SHADES_DRIVE_MOTOR_POSITION_PERCENTAGE_CMD[9] = lift_percentage;

        _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, MOES_SHADES_DRIVE_MOTOR_POSITION_PERCENTAGE_CMD, false);
      } break;

    }
  }
}

void Supla::Control::Z2S_RollerShutter::ping() {

  if (_gateway && Zigbee.started()) {
    
    _fresh_start = false;

    switch (_z2s_function) {

      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
        _gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID, false); break;

      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR:
      break;

    }
  }
}

void Supla::Control::Z2S_RollerShutter::onTimer() {

  if (newTargetPositionAvailable && (targetPosition == STOP_POSITION)) {
    log_i("newTargetPositionAvailable = STOP_POSITION");
    newTargetPositionAvailable = false;
    rsStop();
  }
  if (newTargetPositionAvailable && (targetPosition == MOVE_UP_POSITION)) {
    log_i("newTargetPositionAvailable = MOVE_UP_POSITION");
    newTargetPositionAvailable = false;
    rsClose();
  }
  if (newTargetPositionAvailable && (targetPosition == MOVE_DOWN_POSITION)) {
    log_i("newTargetPositionAvailable = MOVE_DOWN_POSITION");
    newTargetPositionAvailable = false;
    rsOpen();
  }
  if (newTargetPositionAvailable && (targetPosition >= 0) && (targetPosition <= 100)) {
    log_i("newTargetPositionAvailable = %u, reversed = %u", targetPosition, 100 - targetPosition);
    newTargetPositionAvailable = false;
    rsMoveToLiftPercentage(targetPosition);

  }

}

void Supla::Control::Z2S_RollerShutter::iterateAlways() {

  Supla::Control::RollerShutterInterface::iterateAlways();

  /*if (_rs_current_position_changed) {
    _rs_current_position_changed = false;
    if (_gateway && Zigbee.started()) {   
    
      _gateway->sendAttributeWrite(&_device, ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID,
                                ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_rs_current_position);
    }
  }*/

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

void Supla::Control::Z2S_RollerShutter::setRSCurrentPosition(uint8_t rs_current_position) {
 
  /*if ((_rs_current_position == 100) && (_rs_current_position == 0))
    _rs_current_position_changed = true;
  else
  if ((_rs_current_position == 0) && (_rs_current_position == 100))
    _rs_current_position_changed = true;
  else {
    _rs_current_position = rs_current_position;
    setCurrentPosition(_rs_current_position);  
  }
  return;

  if (_rs_current_position != rs_current_position)
    _rs_current_position_changed = true;

  _rs_current_position = rs_current_position;
  setCurrentPosition(_rs_current_position);

  return;*/

 if (_rs_moving_direction != 1) {
  _rs_current_position = rs_current_position;
  setCurrentPosition(_rs_current_position);
 }
 else
  log_i("No RS movement detected - ignoring setRSCurrentPosition new value %u", rs_current_position);
}

void Supla::Control::Z2S_RollerShutter::setRSMovingDirection(uint8_t rs_moving_direction) {

  _rs_moving_direction = rs_moving_direction;
}

void Supla::Control::Z2S_RollerShutter::setKeepAliveSecs(uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
  if (_keep_alive_ms == 0)
    _keep_alive_enabled = false;
  else 
    _keep_alive_enabled = true;
}

void Supla::Control::Z2S_RollerShutter::setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    channel.setStateOnline();
  }
  else
   _timeout_enabled = true;
}

uint32_t Supla::Control::Z2S_RollerShutter::getKeepAliveSecs() {

  return _keep_alive_ms * 1000;
}

uint32_t Supla::Control::Z2S_RollerShutter::getTimeoutSecs() {

  return _timeout_ms * 1000;
}

//#endif
