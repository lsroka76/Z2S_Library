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

/*****************************************************************************/

Supla::Control::Z2S_RollerShutter::Z2S_RollerShutter(uint8_t z2s_function)
  : Z2S_Core(this), _z2s_function(z2s_function) {

}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::setZ2SFunction(uint8_t z2s_function) {

  _z2s_function = z2s_function;
}

/*****************************************************************************/

uint8_t Supla::Control::Z2S_RollerShutter::getZ2SFunction() {

  return _z2s_function;
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::onInit() {

  Supla::Control::RollerShutterInterface::onInit();

  if (getTimeoutMs())
    channel.setStateOffline();
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::schedulePostCmdPing() {

  _rs_post_cmd_ping_accept_ms = 0;

  if (getRollerShutterPostCmdPingDelayS() > 0) {
    uint32_t now = millis();
    _rs_post_cmd_ping_ms = now ? now : 1;
  } else {
    _rs_post_cmd_ping_ms = 0;
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::rsOpen() {

  _rs_lumi_stop_read_ms = 0;

  if (Zigbee.started()) {

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint, ESP_ZB_ZCL_CMD_WINDOW_COVERING_UP_OPEN,
          nullptr);
      break;


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT: {

        uint8_t lift_percentage = 0;

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint,
          ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE,
          &lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_SHADES_DRIVE_MOTOR_STATE_DP, 0x02);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_COVER_STATE_DP, 0x02);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_COVER_STATE_DP, 0x02);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_TUYA_MB60L_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, TUYA_MB60L_SMART_BLINDS_MOTOR_COVER_STATE_DP,
          0x00);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_LUMI_ANALOG_MULTISTATE: {

        float lift_cmd = 100;

        zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT, 0x55,
          ESP_ZB_ZCL_ATTR_TYPE_SINGLE, 4, &lift_cmd, false, 0, 0);
      } break;
    }
    schedulePostCmdPing();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::rsClose() {

  _rs_lumi_stop_read_ms = 0;

  if (Zigbee.started()) {

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint, ESP_ZB_ZCL_CMD_WINDOW_COVERING_DOWN_CLOSE,
          nullptr);
      break;


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT: {

        uint8_t lift_percentage = 100;

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint,
          ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE,
          &lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_SHADES_DRIVE_MOTOR_STATE_DP, 0x00);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_COVER_STATE_DP, 0x00);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_COVER_STATE_DP, 0x01);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_TUYA_MB60L_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, TUYA_MB60L_SMART_BLINDS_MOTOR_COVER_STATE_DP,
          0x02);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_LUMI_ANALOG_MULTISTATE: {

        float lift_cmd = 0;

        zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT, 0x55,
          ESP_ZB_ZCL_ATTR_TYPE_SINGLE, 4, &lift_cmd, false, 0, 0);
      } break;
    }
    schedulePostCmdPing();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::rsStop() {

  if (Zigbee.started()) {

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT:

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint, ESP_ZB_ZCL_CMD_WINDOW_COVERING_STOP,
          nullptr);
      break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_SHADES_DRIVE_MOTOR_STATE_DP, 0x01);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_COVER_STATE_DP, 0x01);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, MOES_COVER_STATE_DP, 0x00);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_TUYA_MB60L_COVER: {

        sendTuyaRequestCmdEnum8(
          _short_addr, _endpoint, TUYA_MB60L_SMART_BLINDS_MOTOR_COVER_STATE_DP,
          0x01);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_LUMI_ANALOG_MULTISTATE: {

        uint16_t lift_cmd = 2;

        zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_MULTI_OUTPUT, 0x55,
          ESP_ZB_ZCL_ATTR_TYPE_U16, 2, &lift_cmd, false, 0, 0);

        uint32_t now = millis();
        _rs_lumi_stop_read_ms = now ? now : 1;
      } break;
    }
    schedulePostCmdPing();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::rsMoveToLiftPercentage(
  uint8_t lift_percentage) {

  _rs_lumi_stop_read_ms = 0;

  if (Zigbee.started()) {

    if (rsConfig.buttonsUpsideDown == 2)
      lift_percentage = 100 - lift_percentage;

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint,
          ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE,
          &lift_percentage);
      break;


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT: {

        zbGateway.sendWindowCoveringCmd(
          _short_addr, _endpoint,
          ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE,
          &lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR: {

        sendTuyaRequestCmdValue32(
          _short_addr, _endpoint,
          MOES_SHADES_DRIVE_MOTOR_STATE_COVER_POSITION_DP,
          100 - lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER: {

        sendTuyaRequestCmdValue32(
          _short_addr, _endpoint, MOES_COVER_STATE_COVER_POSITION_DP,
          lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER: {

        setChannelUserDataFlags(USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG);
        setIgnoreNextMsgCounter(2);

        rsStop();

        sendTuyaRequestCmdValue32(
          _short_addr, _endpoint, MOES_COVER_STATE_COVER_POSITION_DP,
          lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_TUYA_MB60L_COVER: {

        sendTuyaRequestCmdValue32(
          _short_addr, _endpoint,
          TUYA_MB60L_SMART_BLINDS_MOTOR_COVER_POSITION_DP,
          100 - lift_percentage);
      } break;


      case Z2S_ROLLER_SHUTTER_FNC_LUMI_ANALOG_MULTISTATE: {

        float lift_float = 100 - lift_percentage;
        zbGateway.sendAttributeWrite(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT, 0x55,
          ESP_ZB_ZCL_ATTR_TYPE_SINGLE, 4, &lift_float, false, 0, 0);
      } break;
    }
    schedulePostCmdPing();
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::ping() {

  if (Zigbee.started()) {

    _fresh_start = false;

    switch (_z2s_function) {


      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER:
      case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT:

        zbGateway.sendAttributeRead(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,
          ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID,
          false);
      break;


      case Z2S_ROLLER_SHUTTER_FNC_MOES_SHADES_DRIVE_MOTOR:
      case Z2S_ROLLER_SHUTTER_FNC_MOES_COVER:
      case Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER:
      case Z2S_ROLLER_SHUTTER_FNC_TUYA_MB60L_COVER:

        sendTuyaQueryCmd(_short_addr, _endpoint);
      break;


      case Z2S_ROLLER_SHUTTER_FNC_LUMI_ANALOG_MULTISTATE:

        zbGateway.sendAttributeRead(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT, 0x55,
          false);
      break;
    }
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::processTargetPosition() {

  if (!newTargetPositionAvailable)
    return;

  uint32_t now_ms = millis();
  if ((_last_cmd_ms != 0) &&
      ((uint32_t)(now_ms - _last_cmd_ms) < Z2S_RS_MIN_CMD_GAP_MS)) {
    return;
  }

  int8_t cmd = targetPosition;
  newTargetPositionAvailable = false;
  _last_cmd_ms = now_ms ? now_ms : 1;

  if ((cmd == STOP_POSITION) || (cmd == STOP_REQUEST)) {

    log_i("newTargetPositionAvailable = STOP_POSITION");

    currentDirection = Directions::STOP_DIR;
    rsStop();
  } else if (cmd == MOVE_UP_POSITION) {

    log_i("newTargetPositionAvailable = MOVE_UP_POSITION");

    currentDirection = Directions::UP_DIR;

    if (rsConfig.motorUpsideDown == 2)
      rsClose();
    else
      rsOpen();
  } else if (cmd == MOVE_DOWN_POSITION) {

    log_i("newTargetPositionAvailable = MOVE_DOWN_POSITION");

    currentDirection = Directions::DOWN_DIR;

    if (rsConfig.motorUpsideDown == 2)
      rsOpen();
    else
      rsClose();
  } else if ((cmd >= 0) && (cmd <= 100)) {

    log_i("newTargetPositionAvailable = %d, reversed = %d", cmd, 100 - cmd);

    int cur = getCurrentPosition();
    if ((cmd == 0) || ((cur >= 0) && (cmd < cur))) {
      currentDirection = Directions::UP_DIR;
    } else if ((cmd == 100) || ((cur >= 0) && (cmd > cur))) {
      currentDirection = Directions::DOWN_DIR;
    } else {
      currentDirection = Directions::STOP_DIR;
    }

    rsMoveToLiftPercentage(static_cast<uint8_t>(cmd));
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::iterateAlways() {

  Supla::Control::RollerShutterInterface::iterateAlways();

  processTargetPosition();

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (getKeepAliveMs() && ((millis() - _last_ping_ms) > getKeepAliveMs())) {
    if (true) {

      _last_seen_ms = getZbDeviceLastSeenMs();

      if ((millis() - _last_seen_ms) > getKeepAliveMs()) {
        ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        if (!channel.isStateOnline())
          channel.setStateOnline();
      }
    }
  }

  if (getTimeoutMs() && channel.isStateOnline() &&
      ((millis() - _last_seen_ms) > getTimeoutMs())) {

    log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);

    _last_seen_ms = getZbDeviceLastSeenMs();

    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(),
          _last_seen_ms);

    if ((millis() - _last_seen_ms) > getTimeoutMs())
      channel.setStateOffline();
  }

  if (_rs_action_timeout_ms &&
      (millis() - _rs_action_timeout_ms > getRefreshMs())) {

    _rs_action_timeout_ms = 0;
    stop();
  }

  if (_rs_lumi_stop_read_ms) {
    uint32_t now_ms = millis();
    if ((uint32_t)(now_ms - _rs_lumi_stop_read_ms) >=
        Z2S_RS_LUMI_STOP_READ_MS) {
      _rs_lumi_stop_read_ms       = 0;
      _rs_post_cmd_ping_accept_ms = now_ms ? now_ms : 1;
      if (Zigbee.started()) {
        zbGateway.sendAttributeRead(
          _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT, 0x55,
          false);
        _last_ping_ms = now_ms;
      }
    }
  }

  uint32_t armed_ms = _rs_post_cmd_ping_ms;
  if (armed_ms) {
    uint32_t delay_ms = getRollerShutterPostCmdPingDelayMs();
    uint32_t now_ms   = millis();
    if (delay_ms == 0) {
      _rs_post_cmd_ping_ms = 0;
    } else if ((uint32_t)(now_ms - armed_ms) >= delay_ms) {
      _rs_post_cmd_ping_ms        = 0;
      _rs_post_cmd_ping_accept_ms = now_ms ? now_ms : 1;
      ping();
      _last_ping_ms = now_ms;
    }
  }

  uint32_t accept_ms = _rs_post_cmd_ping_accept_ms;
  if (accept_ms && ((uint32_t)(millis() - accept_ms) >= 5000UL)) {
    _rs_post_cmd_ping_accept_ms = 0;
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::setRSCurrentPosition(
  uint8_t rs_current_position) {

  uint32_t accept_ms = _rs_post_cmd_ping_accept_ms;
  bool accept_ping_reply =
    (accept_ms != 0) && ((uint32_t)(millis() - accept_ms) < 5000UL);

  if ((_rs_moving_direction != 1) || _rs_ignore_moving_direction ||
      accept_ping_reply) {

    _rs_post_cmd_ping_ms        = 0;
    _rs_post_cmd_ping_accept_ms = 0;
    _rs_current_position        = rs_current_position;

    if (_z2s_function == Z2S_ROLLER_SHUTTER_FNC_CURRYSMARTER_COVER)
      _rs_current_position = 100 - _rs_current_position;

    if (rsConfig.buttonsUpsideDown == 2)
      _rs_current_position = 100 - _rs_current_position;

    setCurrentPosition(_rs_current_position);

    if (((currentDirection == Directions::UP_DIR) &&
         (_rs_current_position == 0)) ||
        ((currentDirection == Directions::DOWN_DIR) &&
         (_rs_current_position == 100)) ||
        ((targetPosition >= 0) &&
         (_rs_current_position == targetPosition))) {
      currentDirection = Directions::STOP_DIR;
    }
  }
  else {

    log_i(
      "No RS movement detected - ignoring setRSCurrentPosition new value %u",
      rs_current_position);
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::setRSIgnoreMovingDirection(
  bool rs_ignore_moving_direction) {

  _rs_ignore_moving_direction = rs_ignore_moving_direction;
}

/*****************************************************************************/

bool Supla::Control::Z2S_RollerShutter::getRSIgnoreMovingDirection() {

  return _rs_ignore_moving_direction;
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::setRSMovingDirection(
  uint8_t rs_moving_direction) {

  log_i("_rs_ignore_moving_direction %u", _rs_ignore_moving_direction);

  switch (_z2s_function) {


    case Z2S_ROLLER_SHUTTER_FNC_WINDOW_COVERING_CLUSTER_ALT:

      _rs_moving_direction = rs_moving_direction;
    break;


    default:

      _rs_moving_direction = rs_moving_direction;
    break;
  }

  if (_rs_moving_direction == 1) {
    currentDirection = Directions::STOP_DIR;
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::handleAction(int event, int action) {

  Supla::Control::RollerShutterInterface::handleAction(event, action);

  switch (action) {


    case Z2S_SUPLA_ACTION_OPEN_WITH_TIMEOUT: {

      _rs_action_timeout_ms = millis();
      open();
    } break;


    case Z2S_SUPLA_ACTION_CLOSE_WITH_TIMEOUT: {

      _rs_action_timeout_ms = millis();
      close();
    } break;
  }
}

/*****************************************************************************/

void Supla::Control::Z2S_RollerShutter::Refresh() {

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;

  if (!channel.isStateOnline())
    channel.setStateOnline();
}

/*****************************************************************************/