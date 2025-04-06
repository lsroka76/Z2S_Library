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

void Supla::Control::Z2S_TRVOutputInterface::setOutputValue(int value) {

  _trv_state = value;
}

bool Supla::Control::Z2S_TRVOutputInterface::isOnOffOnly() const {
  return true;
}