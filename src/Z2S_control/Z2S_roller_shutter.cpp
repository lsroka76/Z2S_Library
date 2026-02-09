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
#include "TuyaDatapoints.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

#include <supla/time.h>

Supla::Control::Z2S_RollerShutter::Z2S_RollerShutter(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t z2s_function)
  : /*_gateway(gateway),*/ _z2s_function(z2s_function) {

      memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::setZ2SZbDevice(
  z2s_zb_device_params_t *z2s_zb_device) {

    _z2s_zb_device = z2s_zb_device;
  }

/*****************************************************************************/

z2s_zb_device_params_t *Supla::Control::Z2S_RollerShutter::getZ2SZbDevice() {

    return _z2s_zb_device;
}
  
/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::setZ2SChannel(
  z2s_device_params_t *z2s_channel) {

    _z2s_channel = z2s_channel;
  }

/*****************************************************************************/

z2s_device_params_t *Supla::Control::Z2S_RollerShutter::getZ2SChannel() {

    return _z2s_channel;
}
  
/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::onInit() {

  if (_timeout_enabled)
    channel.setStateOffline();
}

void Supla::Control::Z2S_RollerShutter::rsOpen() {

  if (Zigbee.started()) {   
    
    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:

        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_UP_OPEN, nullptr); 
      break;


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT: {
        
        uint8_t lift_percentage = 0;
        
        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE, 
          &lift_percentage); 
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_SHADES_DRIVE_MOTOR_STATE_DP, 0x02);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {


        sendTuyaRequestCmdEnum8(&zbGateway, &_device, MOES_COVER_STATE_DP,0x02);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        sendTuyaRequestCmdEnum8(&zbGateway, &_device, MOES_COVER_STATE_DP, 0x02);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::rsClose() {

  if (Zigbee.started()) {   

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:

        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_DOWN_CLOSE, nullptr); 
      break;


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT: {
        
        uint8_t lift_percentage = 100;

        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE, 
          &lift_percentage); 
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_SHADES_DRIVE_MOTOR_STATE_DP, 0x00);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {

        uint8_t lift_percentage = 100;

        sendTuyaRequestCmdEnum8(&zbGateway, &_device, MOES_COVER_STATE_DP, 0x00);
      } break;
 

      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        sendTuyaRequestCmdEnum8(&zbGateway, &_device, MOES_COVER_STATE_DP, 0x01);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::rsStop() {

  if (Zigbee.started()) {   
    
    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT:

        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_STOP, nullptr); 
      break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {
        
        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_SHADES_DRIVE_MOTOR_STATE_DP, 0x01);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {
        
        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_COVER_STATE_DP, 0x01);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        sendTuyaRequestCmdEnum8(&zbGateway, &_device, MOES_COVER_STATE_DP, 0x00);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::rsMoveToLiftPercentage(
  uint8_t lift_percentage) {

  if (Zigbee.started()) {   

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:

        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE, 
          &lift_percentage);


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT: {

        if (_rs_target_position == 0) {

          if (_rs_current_position > 10) {

            targetPosition = _rs_current_position - 10;
          } else {

            targetPosition = 0;
            _rs_target_position = -1;
          }
        }

        if (_rs_target_position == 100) {

          if (_rs_current_position < 90) {

            targetPosition = _rs_current_position + 10;
          } else {

            targetPosition = 100;
            _rs_target_position = -1;
          }
        }

        log_i("lift_percentage %u\n\r"
              "_rs_target_position %d\n\r"
              "_rs_current_position %d\n\r"
              "newTargetPositionAvailable %d",
              lift_percentage,
              _rs_target_position,
              _rs_current_position,
              newTargetPositionAvailable); 

        
        zbGateway.sendWindowCoveringCmd(
          &_device, ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE, 
          &lift_percentage); 

      }break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, MOES_SHADES_DRIVE_MOTOR_STATE_COVER_POSITION_DP, 
          100 - lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {

        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, MOES_COVER_STATE_COVER_POSITION_DP, 
          lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        if (_z2s_channel)
          _z2s_channel->user_data_flags |= 0x10; //IGNORE_NEXT_MSG
        
        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, MOES_COVER_STATE_COVER_POSITION_DP, 
          lift_percentage);
      } break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::ping() {

  if (Zigbee.started()) {
    
    _fresh_start = false;

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT:

        if (zbGateway.sendAttributeRead(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
          ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID, 
          false)) { /*true))
          if (*(esp_zb_zcl_status_t *)zbGateway.getReadAttrStatusLastResult() == ESP_ZB_ZCL_STATUS_SUCCESS) {
            
            _rs_current_position = *(uint8_t *)zbGateway.getReadAttrLastResult()->data.value;
            setCurrentPosition(_rs_current_position);*/
          }
      break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR:

      break;
    }
  }
}

void Supla::Control::Z2S_RollerShutter::onTimer() {

  if (newTargetPositionAvailable && ((targetPosition == STOP_POSITION) ||
     (targetPosition == STOP_REQUEST))) {
    
    log_i("newTargetPositionAvailable = STOP_POSITION");

    newTargetPositionAvailable = false;
    rsStop();
  }

  if (newTargetPositionAvailable && (targetPosition == MOVE_UP_POSITION)) {
    
    log_i("newTargetPositionAvailable = MOVE_UP_POSITION");

    newTargetPositionAvailable = false;

    if (rsConfig.motorUpsideDown == 2) 
      rsClose();
    else
      rsOpen(); 
  }

  if (newTargetPositionAvailable && (targetPosition == MOVE_DOWN_POSITION)) {
    
    log_i("newTargetPositionAvailable = MOVE_DOWN_POSITION");

    newTargetPositionAvailable = false;

    if (rsConfig.motorUpsideDown == 2) 
      rsOpen();
    else
      rsClose();
  }

  if (newTargetPositionAvailable && (targetPosition >= 0) && (targetPosition <= 100)) {
    
    log_i("newTargetPositionAvailable = %u, reversed = %u", 
          targetPosition, 100 - targetPosition);

    newTargetPositionAvailable = false;

    /*switch (targetPosition) {

      case 0: 
      case 100:
        _rs_target_position = targetPosition; break;

      default:
      break;
    }*/
    
    rsMoveToLiftPercentage(targetPosition);
  }
}

void Supla::Control::Z2S_RollerShutter::iterateAlways() {

  Supla::Control::RollerShutterInterface::iterateAlways();

  /*if (_rs_current_position_changed) {
    _rs_current_position_changed = false;
    if (Zigbee.started()) {   
    
      zbGateway.sendAttributeWrite(
        &_device, 
        ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING, 
        ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID,
        ESP_ZB_ZCL_ATTR_TYPE_U8, 
        1, 
        &_rs_current_position);
    }
  }*/

  //uint32_t current_millis = millis();

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (_keep_alive_enabled && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (true) {
      
      //_last_seen_ms = zbGateway.getZbgDeviceUnitLastSeenMs(_device.short_addr);
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
  if (_timeout_enabled && channel.isStateOnline() && 
      ((millis() - _last_seen_ms) > _timeout_ms)) {

	  log_i("current_millis %u, _last_seen_ms %u", millis(), 
    _last_seen_ms);

    //_last_seen_ms = 
    //  zbGateway.getZbgDeviceUnitLastSeenMs(_device.short_addr);

    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), 
          _last_seen_ms);

    if ((millis() - _last_seen_ms) > _timeout_ms)
      channel.setStateOffline();
  }
}

void Supla::Control::Z2S_RollerShutter::setRSCurrentPosition(
  uint8_t rs_current_position) {
 
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

  /*if (_rs_target_position >= 0)
          newTargetPositionAvailable = true;
  */

  if (_z2s_function == Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER)
    _rs_current_position = 100 - _rs_current_position;
  
  setCurrentPosition(_rs_current_position);
 } else
  log_i("No RS movement detected - ignoring setRSCurrentPosition new value %u", 
        rs_current_position);
}

void Supla::Control::Z2S_RollerShutter::setRSIgnoreMovingDirection(
  bool rs_ignore_moving_direction) {
  
  _rs_ignore_moving_direction = rs_ignore_moving_direction;
  if(_rs_ignore_moving_direction)
    _rs_moving_direction = 0;
  else
    _rs_moving_direction = 1;
}

bool Supla::Control::Z2S_RollerShutter::getRSIgnoreMovingDirection() {
  
  return _rs_ignore_moving_direction;
}

void Supla::Control::Z2S_RollerShutter::setRSMovingDirection(
  uint8_t rs_moving_direction) {

  if (_rs_ignore_moving_direction)
    return;

  switch (_z2s_function) {


    case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT:
      
      //_rs_moving_direction = 0; break;
      _rs_moving_direction = rs_moving_direction; break;
    

    default:
      
      _rs_moving_direction = rs_moving_direction; break;
  }
}

void Supla::Control::Z2S_RollerShutter::Refresh() {

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();
}


void Supla::Control::Z2S_RollerShutter::setKeepAliveSecs(
  uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;

  if (_keep_alive_ms == 0)
    _keep_alive_enabled = false;
  else 
    _keep_alive_enabled = true;
}

void Supla::Control::Z2S_RollerShutter::setTimeoutSecs(
  uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    channel.setStateOnline();
  }
  else
   _timeout_enabled = true;
}

uint32_t Supla::Control::Z2S_RollerShutter::getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t Supla::Control::Z2S_RollerShutter::getTimeoutSecs() {

  return _timeout_ms / 1000;
}

//#endif