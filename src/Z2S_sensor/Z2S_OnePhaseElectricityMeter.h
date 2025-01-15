#ifndef Z2S_ONE_PHASE_ELECTRICITY_METER_H_
#define Z2S_ONE_PHASE_ELECTRICITY_METER_H_


#include <supla/log_wrapper.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include "ZigbeeGateway.h"

namespace Supla {
namespace Sensor {

class Z2S_OnePhaseElectricityMeter : public OnePhaseElectricityMeter {
 public:
  Z2S_OnePhaseElectricityMeter(ZigbeeGateway *Gateway, zb_device_params_t *Device) : Gateway(Gateway) {
      //memcpy(relay_ieee_addr, ieee_addr,8);
	memcpy(&Device, Device, sizeof(zb_device_params_t));

  }

  void onInit() override {
    readValuesFromDevice();
    updateChannelValues();
  }

  virtual void readValuesFromDevice() {
    Gateway->sendAttributeRead(&Device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID);
    Gateway->sendAttributeRead(&Device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID);
    Gateway->sendAttributeRead(&Device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID);
  }

 protected:
   ZigbeeGateway 	*Gateway = nullptr;
   zb_device_params_t 	Device;
//esp_zb_ieee_addr_t relay_ieee_addr;
  
};

};  // namespace Sensor
};  // namespace Supla

#endif  //Z2S_ONE_PHASE_ELECTRICITY_METER_H_