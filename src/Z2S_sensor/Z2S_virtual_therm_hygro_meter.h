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

#ifndef Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_
#define Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_

#include <supla/sensor/therm_hygro_meter.h>
#include <ZigbeeGateway.h>

namespace Supla {
namespace Sensor {
class Z2S_VirtualThermHygroMeter : public Supla::Sensor::ThermHygroMeter {
 public:
    Z2S_VirtualThermHygroMeter(ZigbeeGateway *Gateway, zb_device_params_t *Device) : Gateway(Gateway) {
      memcpy(&Device, Device, sizeof(zb_device_params_t));
  }


    double getTemp() override {
      if (esp_zb_is_started() && esp_zb_lock_acquire(portMAX_DELAY)) {
	Device.short_addr = esp_zb_address_short_by_ieee(Device.ieee_addr);
        Gateway->sendAttributeRead(&Device, ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT, ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID);
        esp_zb_lock_release();
    }
    return temperature;
  }

  double getHumi() override {
    if (esp_zb_is_started() && esp_zb_lock_acquire(portMAX_DELAY)) {
      Device.short_addr = esp_zb_address_short_by_ieee(Device.ieee_addr);
      Gateway->sendAttributeRead(&Device, ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT, ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID); 
      esp_zb_lock_release();
    }
    return humidity;
  }

  void setTemp(double val) {
    temperature = val;
  }

  void setHumi(double val) {
    humidity = val;
  }

 protected:
  double temperature = TEMPERATURE_NOT_AVAILABLE;
  double humidity = HUMIDITY_NOT_AVAILABLE;

  ZigbeeGateway 	*Gateway = nullptr;
  zb_device_params_t 	Device;

};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_SRC_SUPLA_SENSOR_VIRTUAL_THERM_HYGRO_METER_H_
