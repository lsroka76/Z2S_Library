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
#include "TuyaDatapoints.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

#include <supla/time.h>

#include <supla/actions.h>
#include <supla/events.h>


Supla::Control::Z2S_VirtualRelay::Z2S_VirtualRelay(
  ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t z2s_function)
  : Relay(-1, true, RELAY_FLAGS ),
  /*_gateway(gateway),*/ _z2s_function(z2s_function) {

    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

/*****************************************************************************/

void Supla::Control::Z2S_VirtualRelay::setZ2SZbDevice(
  z2s_zb_device_params_t *z2s_zb_device) {

    _z2s_zb_device = z2s_zb_device;
  }

/*****************************************************************************/

z2s_zb_device_params_t *Supla::Control::Z2S_VirtualRelay::getZ2SZbDevice() {

    return _z2s_zb_device;
}
  
/*****************************************************************************/


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
  
  initDone = true;
}

void Supla::Control::Z2S_VirtualRelay::turnOn(_supla_int_t duration) {
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

  if (Zigbee.started()) { 
    
    uint8_t _z2s_function_data[MAX_COMMAND_DATA_SIZE];

    switch (_z2s_function) {

      case Z2S_VIRTUAL_RELAY_FNC_NONE: {
        
        state = true;
        zbGateway.sendOnOffCmd(&_device, state);
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_LUMI_ATTRIBUTE_U8: {

        state = true;
        //zbGateway.sendAttributeWrite(
      //    &_device, LUMI_CUSTOM_CLUSTER,_z2s_function_value_U32,
      //    ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_z2s_function_value_U8, false, 1,
      //LUMI_MANUFACTURER_CODE);
      } break;

/*****************************************************************************/

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

        //log_i("_z2s_function = %u, short addr = 0x%X",_z2s_function, _device.short_addr);
        zbGateway.sendCustomClusterCmd(
          &_device, 0x0502, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 0x05, 
          _z2s_function_data);
        zbGateway.sendOnOffCmd(&_device, state);

        channel.setNewValue(state);

      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_SWITCH: {

        state = true;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, MOES_ALARM_SWITCH_DP, state);
      
        channel.setNewValue(state);

      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_DURATION: {

        state = false;

        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, MOES_ALARM_DURATION_DP, Z2S_incValueU32(
            MOES_ALARM_MELODY_MIN_DURATION, MOES_ALARM_MELODY_MAX_DURATION, 
            10));
        channel.setNewValue(state);

      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_MELODY: {

        state = false;

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_ALARM_MELODY_DP, Z2S_incValueU8(
            MOES_ALARM_MELODY_FIRST_MELODY, MOES_ALARM_MELODY_LAST_MELODY));

        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_VOLUME: {

        state = false;
        
        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_ALARM_MELODY_DP, Z2S_incValueU8(
            MOES_ALARM_VOLUME_LOWEST, MOES_ALARM_VOLUME_HIGHEST));
        
        channel.setNewValue(state);

      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_STATE: {

        state = true;

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, TUYA_PRESENCE_SENSOR_RELAY_SWITCH_STATE_DP, 
          state ? 1 : 0);
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

case Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_MODE: {

        state = true;

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, TUYA_PRESENCE_SENSOR_RELAY_SWITCH_MODE_DP, 
          state ? 1 : 0); //automatic = local mode = 1
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_GIEX_VALVE_MANUAL: {

        state = true;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, GIEX_WATER_VALVE_STATE_DP, state);
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_TUYA_DP_RELAY: {

        state = true;

        uint8_t realy_dp_id = _z2s_function_value_U8;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, realy_dp_id, state);
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_SONOFF_VALVE_PROGRAM: {

        uint16_t attribute_id;

        switch (_z2s_function_value_S8) {
          
          
          case 1: 
            
            attribute_id = SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID; 
          break;


          case 2: 
          
            attribute_id = SONOFF_CUSTOM_CLUSTER_VOLUME_IRRIGATION_CYCLE_ID; 
          break;


          default: 
            
            attribute_id = 0xFFFF; 
          break;
        }

        if (attribute_id < 0xFFFF) {

          _z2s_function_data[0] = 0x0A;
          _z2s_function_data[1] = 0x00;
          _z2s_function_data[2] = _z2s_function_value_U8;
          _z2s_function_data[3] = 0x00;
          _z2s_function_data[4] = (_z2s_function_value_S32 & 0xFF0000) >> 16;
          _z2s_function_data[5] = (_z2s_function_value_S32 & 0xFF00) >> 8;
          _z2s_function_data[6] = _z2s_function_value_S32 & 0xFF;
          _z2s_function_data[7] = 0x00;
          _z2s_function_data[8] = (_z2s_function_value_U32 & 0xFF0000) >> 16;
          _z2s_function_data[9] = (_z2s_function_value_U32 & 0xFF00) >> 8;
          _z2s_function_data[10] = _z2s_function_value_U32 & 0xFF;
		
          zbGateway.sendAttributeWrite(
            &_device, SONOFF_CUSTOM_CLUSTER, attribute_id, 
            ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING, 11, _z2s_function_data, true);
        }
        state = false;
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

  if (Zigbee.started()) { 
    
    uint8_t _z2s_function_data[MAX_COMMAND_DATA_SIZE];
    
    switch (_z2s_function) {

      case Z2S_VIRTUAL_RELAY_FNC_NONE: {
        state = false;
        zbGateway.sendOnOffCmd(&_device, state);
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_LUMI_ATTRIBUTE_U8: {

        state = false;
        //zbGateway.sendAttributeWrite(
      //    &_device, LUMI_CUSTOM_CLUSTER,_z2s_function_value_U32,
      //    ESP_ZB_ZCL_ATTR_TYPE_U8, 1, &_z2s_function_value_S8, false, 1,
      //LUMI_MANUFACTURER_CODE);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_IAS_WD_SILENT_ALARM:
      case Z2S_VIRTUAL_RELAY_FNC_IAS_WD_LOUD_ALARM: {

        state = false;
        
        _z2s_function_data[0] = 0x00; //no warning, no strobe, no siren
        _z2s_function_data[1] = 0x00;
        _z2s_function_data[2] = 0x00; //duration lenght
        _z2s_function_data[3] = 0x00; //strobe duty cycle
        _z2s_function_data[4] = 0x00; //strobe level field

        zbGateway.sendOnOffCmd(&_device, state);
        zbGateway.sendCustomClusterCmd(
          &_device, 0x0502, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 0x05, 
          _z2s_function_data);
        channel.setNewValue(state);

      } break;

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_SWITCH: {

        state = false;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, MOES_ALARM_SWITCH_DP, state);

        channel.setNewValue(state);

      } break;

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_DURATION: {

        state = false;

        sendTuyaRequestCmdValue32(
          &zbGateway, &_device, MOES_ALARM_DURATION_DP, Z2S_decValueU32(
            MOES_ALARM_MELODY_MIN_DURATION, MOES_ALARM_MELODY_MAX_DURATION, 
            10));

        channel.setNewValue(state);
      } break;

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_MELODY: {

        state = false;

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_ALARM_MELODY_DP, Z2S_decValueU8(
            MOES_ALARM_MELODY_FIRST_MELODY, MOES_ALARM_MELODY_LAST_MELODY));

        channel.setNewValue(state);
      } break;

      case Z2S_VIRTUAL_RELAY_FNC_MOES_ALARM_VOLUME: {
        
        state = false;
        
        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, MOES_ALARM_MELODY_DP, Z2S_decValueU8(
            MOES_ALARM_VOLUME_LOWEST, MOES_ALARM_VOLUME_HIGHEST));
        
        channel.setNewValue(state);

      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_STATE: {

        state = false;

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, TUYA_PRESENCE_SENSOR_RELAY_SWITCH_STATE_DP, 
          state ? 1 : 0);
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_PRESENCE_RELAY_MODE: {

        state = false;

        sendTuyaRequestCmdEnum8(
          &zbGateway, &_device, TUYA_PRESENCE_SENSOR_RELAY_SWITCH_MODE_DP, 
          state ? 1 : 0);
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

      case Z2S_VIRTUAL_RELAY_FNC_GIEX_VALVE_MANUAL: {

        state = false;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, GIEX_WATER_VALVE_STATE_DP, state);
        
        channel.setNewValue(state);

      } break;

/*****************************************************************************/
      
      case Z2S_VIRTUAL_RELAY_FNC_TUYA_DP_RELAY: {

        state = false;

        uint8_t realy_dp_id = _z2s_function_value_U8;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, realy_dp_id, state);
        
        channel.setNewValue(state);
      } break;

/*****************************************************************************/

    }  
  }
  // Schedule save in 5 s after state change
  Supla::Storage::ScheduleSave(5000);
}

void Supla::Control::Z2S_VirtualRelay::ping() {

  if (Zigbee.started()) {

    _fresh_start = false;
    
    zbGateway.sendAttributeRead(
      &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
      ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, false);

	switch (_z2s_function) {


      case Z2S_VIRTUAL_RELAY_FNC_TUYA_DP_RELAY: {

        uint8_t realy_dp_id = _z2s_function_value_U8;

        sendTuyaRequestCmdBool(
          &zbGateway, &_device, realy_dp_id, state);
        
      } break;
	}
  }
}

void Supla::Control::Z2S_VirtualRelay::iterateAlways() {

  Supla::Control::Relay::iterateAlways();

  //uint32_t current_millis = millis();

  if (_fresh_start && ((millis() - _last_ping_ms) > 5000))
    ping();

  if (_keep_alive_enabled && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (true) {
      
      if (_z2s_zb_device)
        _last_seen_ms = _z2s_zb_device->last_seen_ms;
      //_last_seen_ms = zbGateway.getZbgDeviceUnitLastSeenMs(_device.short_addr);
      if ((millis() - _last_seen_ms) > _keep_alive_ms) {
      	ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        //if (!channel.isStateOnline()) 
	      channel.setStateOnline();
      }
    }
  }
  if (_timeout_enabled && channel.isStateOnline() && 
      ((millis() - _last_seen_ms) > _timeout_ms)) {
	  
    log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);
    
    //_last_seen_ms = zbGateway.getZbgDeviceUnitLastSeenMs(_device.short_addr);
    
    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), 
          _last_seen_ms);
    
    if ((millis() - _last_seen_ms) > _timeout_ms)
      channel.setStateOffline();
  }
}

void Supla::Control::Z2S_VirtualRelay::handleAction(int event, int action) {

  Supla::Control::Relay::handleAction(event, action);

  switch (action) {

    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:

      if (state) 
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
    break;
  }
}


bool Supla::Control::Z2S_VirtualRelay::isOn() {
  
  /*if (Zigbee.started()) {   
    if (zbGateway.sendAttributeRead(
          &_device, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
          ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, true))
      state = *((bool *)zbGateway.getReadAttrLastResult()->data.value);
  }*/
   return state;
}

void Supla::Control::Z2S_VirtualRelay::Z2S_setOnOff(bool on_off_state) {
  
  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!channel.isStateOnline()) 
	  channel.setStateOnline();

  log_i("durationMs = %lu, storedTurnOnDurationMs = %lu, "
        "durationTimestamp = %lu, keepTurnOnDurationMs = %u",
        durationMs, storedTurnOnDurationMs, durationTimestamp, 
        keepTurnOnDurationMs);

  if (state != on_off_state) {

    if (on_off_state) {
      
      if (minimumAllowedDurationMs > 0 && storedTurnOnDurationMs == 0) {
       storedTurnOnDurationMs = durationMs;
      }

      if (keepTurnOnDurationMs || isStaircaseFunction() || 
          isImpulseFunction()) {
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

void Supla::Control::Z2S_VirtualRelay::Z2S_setFunctionValueS8(
  int32_t z2s_function_value_S8) {

  _z2s_function_value_S8 = z2s_function_value_S8;
}

void Supla::Control::Z2S_VirtualRelay::Z2S_setFunctionValueU8(
  int32_t z2s_function_value_U8) {

  _z2s_function_value_U8 = z2s_function_value_U8;
}

void Supla::Control::Z2S_VirtualRelay::Z2S_setFunctionValueS32(
  int32_t z2s_function_value_S32) { 

  _z2s_function_value_S32 = z2s_function_value_S32;
}

void Supla::Control::Z2S_VirtualRelay::Z2S_setFunctionValueU32(
  int32_t z2s_function_value_U32) {

  _z2s_function_value_U32 = z2s_function_value_U32;
}

uint8_t Supla::Control::Z2S_VirtualRelay::Z2S_decValueU8(
  uint8_t lower_limit, uint8_t upper_limit, uint8_t step) {

  if (_z2s_function_value_U8 < UINT8_MAX) {

    _z2s_function_value_U8 = (_z2s_function_value_U8 >= (lower_limit + step)) ? 
      (_z2s_function_value_U8 - step) : upper_limit;
    
    return  _z2s_function_value_U8;
  }
  return lower_limit;
}

uint8_t Supla::Control::Z2S_VirtualRelay::Z2S_incValueU8(
  uint8_t lower_limit, uint8_t upper_limit, uint8_t step) {

  if (_z2s_function_value_U8 < UINT8_MAX) {

    _z2s_function_value_U8 = (_z2s_function_value_U8 <= (upper_limit - step)) ?
      _z2s_function_value_U8 + step : lower_limit;
    
    return _z2s_function_value_U8;
  }
  return lower_limit; 
}

uint32_t Supla::Control::Z2S_VirtualRelay::Z2S_decValueU32(
  uint32_t lower_limit, uint32_t upper_limit, uint32_t step) {

  if (_z2s_function_value_U32 < UINT32_MAX) {

    _z2s_function_value_U32 = (_z2s_function_value_U32 >= (lower_limit + step)) ? 
      (_z2s_function_value_U32 - step) : upper_limit;
    
    return  _z2s_function_value_U32;
  }
  return lower_limit;
}

uint32_t Supla::Control::Z2S_VirtualRelay::Z2S_incValueU32(
  uint32_t lower_limit, uint32_t upper_limit, uint32_t step) {

  if (_z2s_function_value_U32 < UINT32_MAX) {

    _z2s_function_value_U32 = (_z2s_function_value_U32 <= (upper_limit - step)) ?
      _z2s_function_value_U32 + step : lower_limit;
    
    return _z2s_function_value_U32;
  }
  return lower_limit; 
}


void Supla::Control::Z2S_VirtualRelay::setKeepAliveSecs(
  uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
  if (_keep_alive_ms == 0)
    _keep_alive_enabled = false;
  else 
    _keep_alive_enabled = true;
}

void Supla::Control::Z2S_VirtualRelay::setTimeoutSecs(
  uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    channel.setStateOnline();
  }
  else
   _timeout_enabled = true;
}

uint32_t Supla::Control::Z2S_VirtualRelay::getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t Supla::Control::Z2S_VirtualRelay::getTimeoutSecs() {

  return _timeout_ms / 1000;
}

//#endif
