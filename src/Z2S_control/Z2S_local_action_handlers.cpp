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


#include "Z2S_local_action_handlers.h"

#define SSLCLIENT_INSECURE_ONLY
#define SSLCLIENT_HALF_DUPLEX

#include <ESP_SSLClient.h>
#include <WiFiClient.h>
//#include <HTTPClient.h>

ESP_SSLClient ssl_client;
WiFiClient basic_client;

//HTTPClient https;

//WiFiClientSecure clientSec;


extern bool force_leave_global_flag;


//using Supla::LocalActionHandler;
//using Supla::LocalActionHandlerWithTrigger;
using namespace Supla;
/*****************************************************************************/

LocalActionHandler::LocalActionHandler() {

};

/*****************************************************************************/

LocalActionHandler::~LocalActionHandler() {

};

/*****************************************************************************/

LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(
  uint8_t pin_logic_operator) : _pin_logic_operator(pin_logic_operator) {

    log_i(
      "_pin_logic_operator %u, pin_logic_operator %u", _pin_logic_operator, 
      pin_logic_operator);
    
    switch (_pin_logic_operator) {


      case PIN_LOGIC_OPERATOR_AND3:
      case PIN_LOGIC_OPERATOR_OR3:

        _pin_c = 0;
      break;

      default:

        _pin_c = 0xFF;
      break;
    }
  };

/*****************************************************************************/

LocalActionHandlerWithTrigger::~LocalActionHandlerWithTrigger() {

};

/*****************************************************************************/

void LocalActionHandlerWithTrigger::handleAction(int event, int action) {
  
  log_i("event %u, action %u", event, action);
  log_i(
    "_pin_a %u, _pin_b %u, _pin_logic_operator %u", _pin_a, _pin_b, 
    _pin_logic_operator);

  bool logic_operation_result = false;

  bool pins_changed = true;

  switch (action) {

    case TURN_ON: {
      
      (_pin_a == 0) ? 
        _pin_a = 1 : (_pin_b == 0) ? 
          _pin_b = 1 : (_pin_c == 0) ? 
            _pin_c = 1 : pins_changed = false;

      /*if (_pin_a == 0) {
        
        _pin_a = 1;
        pins_changed = true;
      } else {

        if (_pin_b == 0) {

          _pin_b = 1;
          pins_changed = true;
        }
      }*/

      log_i(
        "TURN_ON _pin_a %u, _pin_b %u, _pin_c %u, _pin_logic_operator %u, "
        "pins_changed %s", _pin_a, _pin_b, _pin_c, _pin_logic_operator, 
        pins_changed ? "TRUE" : "FALSE");
    } break;

    case TURN_OFF: {

      (_pin_a == 1) ? 
        _pin_a = 0 : (_pin_b == 1) ? 
          _pin_b = 0 : (_pin_c == 1) ? 
            _pin_c = 0 : pins_changed = false;
    
      /*if (_pin_a == 1) {

        _pin_a = 0;
        pins_changed = true;
      } else {

        if (_pin_b == 1) {

          _pin_b = 0;
          pins_changed = true;
        }
      }*/

      log_i(
        "TURN_OFF _pin_a %u, _pin_b %u, _pin_c %u, _pin_logic_operator %u, "
        "pins_changed %s", _pin_a, _pin_b, _pin_c, _pin_logic_operator, 
        pins_changed ? "TRUE" : "FALSE");
    } break;
  }
  
  if  (pins_changed == false)
    return;

  switch (_pin_logic_operator) {


    case PIN_LOGIC_OPERATOR_AND:

      logic_operation_result = _pin_a && _pin_b; break;


    case PIN_LOGIC_OPERATOR_OR:

      logic_operation_result = _pin_a || _pin_b; break;


    case PIN_LOGIC_OPERATOR_XOR:

      logic_operation_result = _pin_a ^ _pin_b; break;

    
    case PIN_LOGIC_OPERATOR_NOT:

      logic_operation_result = !_pin_a; break;

    case PIN_LOGIC_OPERATOR_NAND:

      logic_operation_result = !(_pin_a & _pin_b); break;

    case PIN_LOGIC_OPERATOR_NOR:

      logic_operation_result = !(_pin_a | _pin_b); break;

    case PIN_LOGIC_OPERATOR_AND3:

      logic_operation_result = (_pin_a & _pin_b & _pin_c); break;

    case PIN_LOGIC_OPERATOR_OR3:

      logic_operation_result = (_pin_a | _pin_b | _pin_c); break;
  }
  
  log_i(
    "logic_operation_result %s", logic_operation_result ? "TRUE" : "FALSE");
  
  if (logic_operation_result) {

    if (_postponed_turn_on_ms)
      _pending_postponed_turn_on_ms = millis();
    else
      runAction(ON_TURN_ON);
  }
  else {

    if (_pending_postponed_turn_on_ms)
      _pending_postponed_turn_on_ms = 0;
   runAction(ON_TURN_OFF);
  }
  
  log_i("_postponed_turn_on_ms %lu, _pending_postponed_turn_on_ms %lu", 
        _postponed_turn_on_ms, _pending_postponed_turn_on_ms);
}

/*****************************************************************************/

void LocalActionHandlerWithTrigger::iterateAlways() {

  uint32_t millis_ms = millis();

  if ((_pending_postponed_turn_on_ms) && 
      ((millis_ms - _pending_postponed_turn_on_ms) > _postponed_turn_on_ms)) {

        log_i(
          "millis_ms %lu, _postponed_turn_on_ms %lu, "
          "_pending_postponed_turn_on_ms %lu", millis_ms, 
          _postponed_turn_on_ms, _pending_postponed_turn_on_ms);

        _pending_postponed_turn_on_ms = 0;
        runAction(ON_TURN_ON);
  }
}

/*****************************************************************************/

LocalActionVirtualButton::LocalActionVirtualButton() {

}

/*****************************************************************************/

LocalActionVirtualButton::~LocalActionVirtualButton() {

}

/*****************************************************************************/

void LocalActionVirtualButton::registerFunction(uint32_t function) {

  _function_flags |= function;
}

/*****************************************************************************/

void LocalActionVirtualButton::unregisterFunction(uint32_t function) {

  _function_flags &= ~function;
}

/*****************************************************************************/

bool LocalActionVirtualButton::hasFunction(uint32_t function) {

  return (_function_flags & function);
}

/*****************************************************************************/

void LocalActionVirtualButton::handleAction(int event, int action) {

}
/*****************************************************************************/

void GatewayEvents::onInit() {

  _cyclic_event_ms = millis();
}

/*****************************************************************************/

void GatewayEvents::setActionHandlerCallback(
  _actionhandler_callback actionhandler_callback) {

    this->actionhandler_callback = actionhandler_callback;
}


/*****************************************************************************/

void GatewayEvents::handleAction(int event, int action) {

  log_i(
    "&&&&&&&&&&&&&&&&&&&&&&&&&&"
    "event %u, action %u", event, action);

  if (_disable_actions)
    return;

  if (action) {

    switch (action) {

      case Z2S_SUPLA_ACTION_START_TIMER: {

        _cyclic_event_enabled = true;
        _cyclic_event_ms = millis();
        _cyclic_event_counter = 0;
      } break;


      case Z2S_SUPLA_ACTION_STOP_TIMER:

        _cyclic_event_enabled = false;
      break;


      case Z2S_SUPLA_ACTION_TURN_ON_LED_WHITE:

        rgbLedWrite(RGB_BUILTIN, 255, 255, 255);
      break;


      case Z2S_SUPLA_ACTION_TURN_OFF_LED:

        rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
      break;


      case Supla::SOFT_RESTART:

        SuplaDevice.scheduleSoftRestart(0);
      break;


      case Supla::ENTER_CONFIG_MODE:

        SuplaDevice.enterConfigMode();
      break;


      case Z2S_SUPLA_ACTION_OPEN_ZIGBEE_NETWORK:

        if (!Zigbee.isNetworkOpen())
          Zigbee.openNetwork(180);
      break;


      case Z2S_SUPLA_ACTION_CLOSE_ZIGBEE_NETWORK:

        if (Zigbee.isNetworkOpen())
          Zigbee.openNetwork(0);
      break;


      case Z2S_SUPLA_ACTION_SET_FORCE_BIND_DEVICE:

        force_leave_global_flag = true;
      break;


      case Z2S_SUPLA_ACTION_CLEAR_FORCE_BIND_DEVICE:

        force_leave_global_flag = false;
      break;


      case Z2S_SUPLA_ACTION_START_GUI_MINIMAL:
      case Z2S_SUPLA_ACTION_START_GUI_STANDARD:

        if (actionhandler_callback != nullptr)  
          actionhandler_callback(event, action);  
      break;


      case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON:

        if (toggleNotifications)
          toggleNotifications->turnOn();
      break;


      case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF:

        if (toggleNotifications)
          toggleNotifications->turnOff();
      break;
    } 
  return;
  }
  
  switch (event) {


    case Supla::ON_DEVICE_STATUS_CHANGE:
    case Z2S_SUPLA_EVENT_ON_SUPLA_INITIALIZED:
    case Z2S_SUPLA_EVENT_ON_SUPLA_REGISTERED_AND_READY:
    case Z2S_SUPLA_EVENT_ON_ZIGBEE_STARTED:
    case Z2S_SUPLA_EVENT_ON_ZIGBEE_OPEN_NETWORK:
    case Z2S_SUPLA_EVENT_ON_ZIGBEE_CLOSE_NETWORK:
    case Z2S_SUPLA_EVENT_ON_GUI_STARTED:
    case Z2S_SUPLA_EVENT_ON_GUI_NOT_STARTED:
      
      runAction(event);
    break;
  }
}

/*****************************************************************************/

void GatewayEvents::iterateAlways() {

  uint32_t millis_ms = millis();

  if (!_cyclic_event_enabled)
    return;

  if (millis_ms - _cyclic_event_ms < 5000)
    return;

  _cyclic_event_ms = millis_ms;

  _cyclic_event_counter = (_cyclic_event_counter + 1) % 17280; //24H

  if ((_cyclic_event_counter % 1) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_5_SECONDS);

  if ((_cyclic_event_counter % 6) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_30_SECONDS);

  if ((_cyclic_event_counter % 12) == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_60_SECONDS);

  if (_cyclic_event_counter == 0)
    runAction(Z2S_SUPLA_EVENT_ON_EVERY_24_HOURS);
}


/*****************************************************************************/


Supla::Control::LocalActionTrigger::LocalActionTrigger() {

}

/*****************************************************************************/

Supla::Control::LocalActionTrigger::~LocalActionTrigger() {

}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::setHoldMs(uint32_t hold_ms) {

  _hold_ms = hold_ms;
}

/*****************************************************************************/

uint32_t Supla::Control::LocalActionTrigger::getHoldMs() {

  return _hold_ms;
}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::handleAction(int event, int action) {

  Supla::Control::ActionTrigger::handleAction(event, action);

  uint32_t local_event = actionTriggerCapToButtonEvent(
    getActionTriggerCap(action));

  log_i("event %u, action %u, local event %u", event, action, local_event);

  //Supla::LocalAction::
  runAction(local_event);
}

/*****************************************************************************/

void Supla::Control::LocalActionTrigger::iterateAlways() {

  if (_hold_ms && ((millis() - _last_hold_ms) > _hold_ms)) {

    log_i("hold repeat");
    _last_hold_ms = millis();
    runAction(Supla::ON_HOLD);
  }
}

/*****************************************************************************/


Supla::Control::LocalVirtualRelay::LocalVirtualRelay(_supla_int_t functions):
 VirtualRelay(functions) {

}

/*****************************************************************************/

Supla::Control::LocalVirtualRelay::~LocalVirtualRelay() {

}

/*****************************************************************************/

void Supla::Control::LocalVirtualRelay::handleAction(int event, int action) {

  Supla::Control::VirtualRelay::handleAction(event, action);

  log_i("event %u, action %u", event, action);

  switch (action) {

    
    case Z2S_SUPLA_ACTION_TURN_OFF_RELAY_5_SECONDS:

      turnOff(5000);
    break;


    case Z2S_SUPLA_ACTION_RESEND_RELAY_STATE:

      if (state) 
        runAction(Supla::ON_TURN_ON);
      else
        runAction(Supla::ON_TURN_OFF);
    break;


    case Z2S_SUPLA_ACTION_COPY_RELAY_ON_STATE:

      if (state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_ON_STATE);
    break;


    case Z2S_SUPLA_ACTION_COPY_RELAY_OFF_STATE:

      if (!state) 
        runAction(Z2S_SUPLA_EVENT_ON_COPY_OFF_STATE);
    break;


    case Z2S_SUPLA_ACTION_SET_RELAY_STATE_DISABLED:

      channel.setStateOffline();
    break;
  }
}

/*****************************************************************************/

Supla::Control::SwitchBotRelay::SwitchBotRelay(uint8_t device_type_id):
 Relay(-1, true, 0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER) {

  _device_type_id = device_type_id;
}

/*****************************************************************************/

void Supla::Control::SwitchBotRelay::onInit() {

  initDone = true;
  //clientSec.setInsecure();
  ssl_client.setInsecure();
  ssl_client.setBufferSizes(1024 /* rx */, 512 /* tx */);
  ssl_client.setDebugLevel(3);
  ssl_client.setSessionTimeout(120);
  ssl_client.setClient(&basic_client);
}

/*****************************************************************************/

void Supla::Control::SwitchBotRelay::turnOn(_supla_int_t duration) {

  
  /*static const char *test_host = "api.pushover.net";

				int16_t connection_code = ssl_client.connect(test_host, 443);
				log_i("connection_code = %d", connection_code);
				if (!connection_code) {
        
					Serial.println("Connection failed!");
    		} else {

    			// Prepare the POST body
    			String url = "/1/messages.json";
    			String postData = "token=" + String("a7haupab8v7r3mo146s4b3838nx81a") +
                      "&user=" + String("uneu9f8jhxt18sm6t4so87ev9xn7i5") +
                      "&message=TEST MESSAGE";

    			// Send HTTP request
    			ssl_client.print(
						String("POST ") + url + " HTTP/1.1\r\n" +
            "Host: api.pushover.net\r\n" +
            "Content-Type: application/x-www-form-urlencoded\r\n" +
            "Content-Length: " + postData.length() + "\r\n" +
            "Connection: close\r\n\r\n" +
            postData);

    			Serial.println("Notification sent!");
				}
				ssl_client.stop();

  if (_device_type_id == SB_DEVICE_TYPE_ON_OFF_ID) {

    _state = true;
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    Supla::Storage::ScheduleSave(5000);
  }
  
  return;*/
  
  char sb_url[128];
    
  //HTTPClient https;

  //sprintf(sb_url, sb_url_template, _sb_device_id.c_str());
  
  sprintf(sb_url, sb_device_path, _sb_device_id.c_str());
  if (ssl_client.connect(sb_host, 443)) {
        
    Serial.println(" ok");
    Serial.println("Send POST request...");
    Serial.println(sb_url);

    ssl_client.print(sb_url);
    ssl_client.print("Host: api.switch-bot.com\r\n");
    ssl_client.print("Content-Type: application/json\r\n");
    ssl_client.print("Authorization: ");
    ssl_client.print(_sb_token);
    ssl_client.print("\r\n");
    ssl_client.print("Content-Length: ");
    ssl_client.print(_json_payload.length());
    ssl_client.print("\r\n\r\n");
    ssl_client.print(_json_payload);
  }
  ssl_client.stop();

  /*if (!https.connected())
    https.begin(ssl_client, sb_url);

	https.addHeader("Content-Type", "application/json");
	https.addHeader("Authorization", _sb_token);

	int httpResponseCode = https.POST(_json_payload);

  if (httpResponseCode == HTTP_CODE_OK)
    log_i("HTTP_CODE_OK, response: %s", https.getString().c_str());
  else
    log_e("HTTP error: %s", https.errorToString(httpResponseCode).c_str());

  ssl_client.stop();
  //clientSec.stop();
  //https.end();*/

  if (_device_type_id == SB_DEVICE_TYPE_ON_OFF_ID) {

    _state = true;
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    Supla::Storage::ScheduleSave(5000);
  }
}

/*****************************************************************************/

void Supla::Control::SwitchBotRelay::turnOff(_supla_int_t duration) {

  if (_device_type_id == SB_DEVICE_TYPE_ON_OFF_ID) {

  char sb_url[128];

  sprintf(sb_url, sb_device_path, _sb_device_id.c_str());
  
  if (ssl_client.connect(sb_host, 443)) {
        
    Serial.println(" ok");
    Serial.println("Send POST request...");
    Serial.println(sb_url);

    ssl_client.print(sb_url);
    ssl_client.print("Host: api.switch-bot.com\r\n");
    ssl_client.print("Content-Type: application/json\r\n");
    ssl_client.print("Authorization: ");
    ssl_client.print(_sb_token);
    ssl_client.print("\r\n");
    ssl_client.print("Content-Length: ");
    ssl_client.print(_json_payload_2.length());
    ssl_client.print("\r\n\r\n");
    ssl_client.print(_json_payload_2);
  }
  ssl_client.stop();  

    /*char sb_url[128];
    
    //HTTPClient https;

    sprintf(sb_url, sb_url_template, _sb_device_id.c_str());
    
    if (!https.connected())
      https.begin(clientSec, sb_url);
	  
	  https.addHeader("Content-Type", "application/json");
	  https.addHeader("Authorization", _sb_token);

	  int httpResponseCode = https.POST(_json_payload_2);

    if (httpResponseCode == HTTP_CODE_OK)
      log_i("HTTP_CODE_OK, response: %s", https.getString().c_str());
    else
      log_e("HTTP error: %s", https.errorToString(httpResponseCode).c_str());

    //https.end();
    clientSec.stop();*/
    
    _state = false;
    channel.setNewValue(_state);
    // Schedule save in 5 s after state change
    Supla::Storage::ScheduleSave(5000);
  }
}

/*****************************************************************************/

bool Supla::Control::SwitchBotRelay::updateSwitchBotData(
  channel_extended_data_sb_t &channel_extended_data_sb, uint8_t direction) {

    if (direction == SB_UPDATE_DATA_LOAD_DIR) {

      _sb_device_id = channel_extended_data_sb.ble_mac_address;
      _sb_token = channel_extended_data_sb.token;
      _json_payload = channel_extended_data_sb.json_payload;
      _json_payload_2 = channel_extended_data_sb.json_payload_2;
      return true;
    }
    if (direction == SB_UPDATE_DATA_SAVE_DIR) {

      strncpy(
        channel_extended_data_sb.ble_mac_address, _sb_device_id.c_str(),
        sizeof(channel_extended_data_sb.ble_mac_address));
      strncpy(
        channel_extended_data_sb.token, _sb_token.c_str(),
        sizeof(channel_extended_data_sb.token));
      strncpy(
        channel_extended_data_sb.json_payload, _json_payload.c_str(),
        sizeof(channel_extended_data_sb.json_payload));
      strncpy(
        channel_extended_data_sb.json_payload_2, _json_payload_2.c_str(),
        sizeof(channel_extended_data_sb.json_payload_2));
      return true;
    }
    return false;
  }

/*****************************************************************************/

Supla::Sensor::LocalVirtualBinary::LocalVirtualBinary(
  bool keepStateInStorage) : VirtualBinary(keepStateInStorage) {

};

/*****************************************************************************/

Supla::Sensor::LocalVirtualBinary::~LocalVirtualBinary() {

};

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::handleAction(int event, int action) {

  Supla::Sensor::VirtualBinary::handleAction(event, action);

  if (!_notification_registered)
    return;

  switch (action) {
    
    
    case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_ON:

      enableNotifications();
    break;


    case Z2S_SUPLA_ACTION_IAS_NOTIFICATIONS_OFF:

      disableNotifications();
    break;
    
    
    case SET:

      if (_auto_clear_ms)
        _last_clear_ms = millis();
    [[fallthrough]];
    case CLEAR:
    case TOGGLE: 
      
      sendNotification();
    break;
  }
}  

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::iterateAlways() {

  Supla::Sensor::VirtualBinary::iterateAlways();

  if (_auto_clear_ms && _last_clear_ms && 
      ((millis() - _last_clear_ms) > _auto_clear_ms)) {

    _last_clear_ms = 0;
    clear();
  }
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::registerNotification() {

  _notification_registered = true;
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::enableNotifications() {

  _notifications_enabled = true;
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::disableNotifications() {

  _notifications_enabled = false;
}

/*****************************************************************************/

void Supla::Sensor::LocalVirtualBinary::sendNotification() {

  if (!sendIASNotifications)
    return;

  Supla::Notification::SendF(
    channel.getChannelNumber(), "","State changed - now is %s", 
    getValue() ? "ON" : "OFF");
}

/*****************************************************************************/