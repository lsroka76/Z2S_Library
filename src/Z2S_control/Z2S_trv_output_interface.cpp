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

#include "Z2S_trv_output_interface.h"

Supla::Control::Z2S_TRVOutputInterface::Z2S_TRVOutputInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t trv_mode) : _gateway(gateway), _trv_mode(trv_mode) {
    memcpy(&_device, device, sizeof(zbg_device_params_t));     
}

 int Supla::Control::Z2S_TRVOutputInterface::getOutputValue() const {

  return _trv_state;
 }

void Supla::Control::Z2S_TRVOutputInterface::sendOnOff(bool state) {

  if (_gateway && Zigbee.started()) {
    log_i("Z2S_TRVOutputInterface::sendOnOff to %d", state);

    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    switch(_trv_mode) {
      case 1: {
        _Tuya_dp_data[2] = 0x65; 
        _Tuya_dp_data[3] = 0x01;
      } break;
      case 2: {
        _Tuya_dp_data[2] = 0x02; 
        _Tuya_dp_data[3] = 0x04;
      } break;
      case 3: {
        _Tuya_dp_data[2] = 0x01; 
        _Tuya_dp_data[3] = 0x04;
      } break;
    }
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x01;
    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    delay(200);
    
    _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    switch(_trv_mode) {
      case 1: _Tuya_dp_data[2] = 0x67; break;
      case 2: _Tuya_dp_data[2] = 0x04; break;
      case 3: _Tuya_dp_data[2] = 0x02; break;
    }
    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = 0x00;
    _Tuya_dp_data[7] = 0x00;
    if (state) {
      _Tuya_dp_data[8] = 0x01;
      _Tuya_dp_data[9] = 0x2C;
    } else {
      _Tuya_dp_data[8] = 0x00;
      _Tuya_dp_data[9] = 0x32;
    }
    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TRVOutputInterface::setOutputValue(int value) {

  _trv_state = value;
  sendOnOff((value == 1));
}

bool Supla::Control::Z2S_TRVOutputInterface::isOnOffOnly() const {
  return true;
}

void Supla::Control::Z2S_TRVOutputInterface::iterateAlways() {

  if (millis() - _last_refresh_ms > _refresh_ms) {
    _last_refresh_ms = millis();
    //sendOnOff((_trv_state == 1));
  }
}