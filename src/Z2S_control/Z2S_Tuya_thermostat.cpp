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

#include "Z2S_Tuya_thermostat.h"

//#include <supla/log_wrapper.h>

//#include <supla/time.h>


void Supla::Control::Z2S_TuyaThermostat::handleAction(int event, int action) {
(void)(event);
log_i("Z2S_TuyaThermostat::handleAction 0x%x 0x%x", event, action);
  switch (action) {
    case TURN_ON: {
	//if (Hvac && Hvac->isThermostatDisabled())
	  //setOnOff(false);
	//else
	  //setOnOff(true);

	if (Hvac)
	{
	  log_i("setTemperatureSetpoint %d", Hvac->getTemperatureSetpointHeat()/10);
	  setTemperatureSetpoint(Hvac->getTemperatureSetpointHeat());
	}		
      
    } break;
    case TURN_OFF: {
      
    } break;   
  }
}

void Supla::Control::Z2S_TuyaThermostat::setZigbeeDevice(ZigbeeGateway *Gateway, zb_device_params_t *thermostat_device, uint8_t commands_set) {
  this->Gateway = Gateway;
  memcpy(&this->thermostat_device, thermostat_device, sizeof(zb_device_params_t));
  this->commands_set = commands_set;
}

void Supla::Control::Z2S_TuyaThermostat::setTemperatureSetpoint(uint16_t setpoint) {
  if (Gateway && Zigbee.started()) {
	
	thermostat_device.short_addr = esp_zb_address_short_by_ieee(thermostat_device.ieee_addr);
	log_i("thermostat_device.short_addr 0x%x", thermostat_device.short_addr);
    	setpoint = setpoint/10;
	payload_buffer[0] = 0x00; //tsn & 0xFF00; payload_buffer[1] = tsn & 0x00FF;
	payload_buffer[1] = 0x03; //tsn & 0x00FF; 
	payload_buffer[2] = 0x67; 
	payload_buffer[3] = 0x02; 
	payload_buffer[4] = 0x00;
	payload_buffer[5] = 0x04; 
	payload_buffer[6] = 0x00;
	payload_buffer[7] = 0x00;
	payload_buffer[8] = setpoint & 0xFF00;
	payload_buffer[9] = setpoint & 0xFF;
	log_i("payload_buffer[8] 0x%x, payload_buffer[9] 0x%x", payload_buffer[8], payload_buffer[9]);
	
	tsn++;    
     Gateway->sendCustomClusterCmd(&thermostat_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 10, payload_buffer, true);
  }
}

void Supla::Control::Z2S_TuyaThermostat::setOnOff(bool on_off) {
if (Gateway && Zigbee.started())
  {
     	thermostat_device.short_addr = esp_zb_address_short_by_ieee(thermostat_device.ieee_addr);
	log_i("thermostat_device.short_addr 0x%x", thermostat_device.short_addr);
	
	payload_buffer[0] = 0x00; //tsn & 0xFF00; payload_buffer[1] = tsn & 0x00FF;
	payload_buffer[1] = 0x03; //tsn & 0x00FF; 
	payload_buffer[2] = 0x65; payload_buffer[3] = 0x01; payload_buffer[4] = 0x00;
	payload_buffer[5] = 0x01; 
	if (on_off) payload_buffer[6] = 0x01;
	else payload_buffer[6] = 0x00;
	tsn++;
	Gateway->sendCustomClusterCmd(&thermostat_device, TUYA_PRIVATE_CLUSTER_EF00, 0x00, 7, payload_buffer, true);
  }
}

void Supla::Control::Z2S_TuyaThermostat::setChildlock(bool childlock) {}

void Supla::Control::Z2S_TuyaThermostat::queryData() {

if (Gateway && Zigbee.started())
    Gateway->sendCustomClusterCmd(&thermostat_device, TUYA_PRIVATE_CLUSTER_EF00, 0x03, 0, payload_buffer);
}

//#endif