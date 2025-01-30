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

//#ifdef Z2S_GATEWAY

#ifndef SRC_SUPLA_CONTROL_Z2S_TUYA_THERMOSTAT_H_
#define SRC_SUPLA_CONTROL_Z2S_TUYA_THERMOSTAT_H_

#include <supla/actions.h>
#include <supla/action_handler.h>
#include <supla/local_action.h>
#include <supla/events.h>
#include <supla/control/hvac_base.h>
#include <supla/control/output_interface.h>
#include <supla/sensor/virtual_thermometer.h>

#include "ZigbeeGateway.h"

#define TUYA_TRV_CS_001	0x001


namespace Supla {
namespace Control {
class Z2S_TuyaThermostat : public ActionHandler, public OutputInterface, public Supla::Sensor::VirtualThermometer {
 public:
  Z2S_TuyaThermostat() : ActionHandler(), OutputInterface(), VirtualThermometer() {}
  ~Z2S_TuyaThermostat() {}

  void handleAction(int event, int action);

  int getOutputValue() const {return 1;}
  void setOutputValue(int value) override {log_i("setOutputValue 0x%x", value);}
  bool isOnOffOnly() const override {return true;}
    
  void setZigbeeDevice(ZigbeeGateway *Gateway, zb_device_params_t *thermostat_device, uint8_t commands_set);
  void setHvac(HvacBase *Hvac) {this->Hvac = Hvac;}
  void setTemperatureSetpoint(uint16_t setpoint);
  void setOnOff(bool on_off);
  void setChildlock(bool childlock);
  void queryData();

 private:
  uint16_t tsn = 0;
  ZigbeeGateway *Gateway = nullptr;
  HvacBase *Hvac;
  //esp_zb_ieee_addr_t thermostat_ieee_addr;
  zb_device_params_t thermostat_device;
  uint8_t commands_set = 0x00;  
  uint8_t payload_buffer[10];
};

};  // namespace Control
};  // namespace Supla

#endif  // SRC_SUPLA_CONTROL_Z2S_TUYA_THERMOSTAT_H_
//#endif  // #ifdef Z2S_GATEWAY