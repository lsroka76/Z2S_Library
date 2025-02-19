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


#include "Z2S_Tuya_dimmer_switch.h"
#include <supla/log_wrapper.h>


Supla::Control::Z2S_TuyaDimmerSwitch::Z2S_TuyaDimmerSwitch(ZigbeeGateway *gateway, zbg_device_params_t *device, int8_t dimmer_number)
    : _gateway(gateway), _dimmer_number(dimmer_number) {
    memcpy(&_device, device, sizeof(zbg_device_params_t)); 
}

void Supla::Control::Z2S_TuyaDimmerSwitch::turnOn() {

  if (_gateway && Zigbee.started()) {
   log_i("TuyaDimmerSwitch turnON");
    uint16_t _tsn_number = random(0x0000, 0xFFFF); 

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x04 : 0x0A;
    _Tuya_dp_data[3] = 0x04;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x01;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    _tsn_number = random(0x0000, 0xFFFF);
    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x04 : 0x0A;
    _Tuya_dp_data[3] = 0x04;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x02;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    _tsn_number = random(0x0000, 0xFFFF);

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x01 : 0x07;
    _Tuya_dp_data[3] = 0x01;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0xFF;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);


    _tsn_number = random(0x0000, 0xFFFF);

    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x01 : 0x07;
    _Tuya_dp_data[3] = 0x01;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x01;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);

    //channel.setNewValue(true);
 }
}

void Supla::Control::Z2S_TuyaDimmerSwitch::turnOff() {

  if (_gateway && Zigbee.started()) {
    uint16_t _tsn_number = random(0x0000, 0xFFFF); 
    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x01 : 0x07;
    _Tuya_dp_data[3] = 0x01;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x01;
    _Tuya_dp_data[6] = 0x00;

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 7, _Tuya_dp_data, false);
  }
}

void Supla::Control::Z2S_TuyaDimmerSwitch::toggle() {
}

bool Supla::Control::Z2S_TuyaDimmerSwitch::isOn() {
return true;
}

void Supla::Control::Z2S_TuyaDimmerSwitch::sendValueToDevice(uint32_t brightness) {

  if (_gateway && Zigbee.started()) {
    uint16_t _tsn_number = random(0x0000, 0xFFFF); 
    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x02 : 0x08;
    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = 0x00;
    _Tuya_dp_data[7] = 0x00;
    _Tuya_dp_data[8] = 0x00;
    _Tuya_dp_data[9] = map(brightness, 0, 100, 0, 254);

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);

    _tsn_number = random(0x0000, 0xFFFF); 
    _Tuya_dp_data[0] = (_tsn_number & 0xFF00);
    _Tuya_dp_data[1] = (_tsn_number & 0x00FF);
    _Tuya_dp_data[2] = (_dimmer_number == 1) ? 0x02 : 0x08;
    _Tuya_dp_data[3] = 0x02;
    _Tuya_dp_data[4] = 0x00;
    _Tuya_dp_data[5] = 0x04;
    _Tuya_dp_data[6] = 0x00;
    _Tuya_dp_data[7] = 0x00;
    _Tuya_dp_data[8] = 0x00;
    _Tuya_dp_data[9] = 0x00;  

    _gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x01, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
_gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x05, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
_gateway->sendCustomClusterCmd(&_device, TUYA_PRIVATE_CLUSTER_EF00, 0x06, ESP_ZB_ZCL_ATTR_TYPE_SET, 10, _Tuya_dp_data, false);
  }
}


void Supla::Control::Z2S_TuyaDimmerSwitch::setValueOnServer(uint32_t brightness) {

  uint8_t _brightness = map(brightness, 1, 254, 0, 100);
  channel.setNewValue(0, 0, 0, 0, _brightness);
}

void Supla::Control::Z2S_TuyaDimmerSwitch::setStateOnServer(bool state) {
  channel.setNewValue(state);
}