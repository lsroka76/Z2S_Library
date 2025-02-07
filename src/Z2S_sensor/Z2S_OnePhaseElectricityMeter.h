#ifndef Z2S_ONE_PHASE_ELECTRICITY_METER_H_
#define Z2S_ONE_PHASE_ELECTRICITY_METER_H_


#include <supla/log_wrapper.h>
#include <supla/sensor/one_phase_electricity_meter.h>
#include "ZigbeeGateway.h"

namespace Supla {
namespace Sensor {

class Z2S_OnePhaseElectricityMeter : public OnePhaseElectricityMeter {
 public:
  Z2S_OnePhaseElectricityMeter(ZigbeeGateway *gateway, zb_device_params_t *device, bool active_query = false) : _gateway(gateway) {
	memcpy(&_device, device, sizeof(zb_device_params_t));
	_active_query = active_query;
	if (_active_query) refreshRateSec = 30;

  }

  void onInit() override {
    readValuesFromDevice();
    updateChannelValues();
  }

  virtual void readValuesFromDevice() {
    if (_gateway && Zigbee.started()) {

       if ((_active_query || (emValue.m[0].voltage[0] == 0)) && _gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, true))
         setVoltage(0, (*(uint16_t *)_gateway->getReadAttrLastResult()->data.value) * 100);
       if ((_active_query || (!currentMeasurementAvailable)) && _gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, true))
         setCurrent(0, (*(uint16_t *)_gateway->getReadAttrLastResult()->data.value) * 1);
       if ((_active_query || (!powerActiveMeasurementAvailable)) && _gateway->sendAttributeRead(&_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, true))
         setPowerActive(0, (*(uint16_t *)_gateway->getReadAttrLastResult()->data.value)*10000);
       
	if (_active_query && _gateway->sendAttributeRead(&_device,ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, true)) {
         esp_zb_uint48_t *value = (esp_zb_uint48_t *)_gateway->getReadAttrLastResult()->data.value;
         _supla_int64_t energy = ((_supla_int64_t)value->high << 32) + value->low;
	 setFwdActEnergy(0, energy);
       }
    }
  }

void resetStorage() {
    if (_gateway && Zigbee.started()) {

      _gateway->sendDeviceFactoryReset(&_device);
//_write_mask.low = 0; _write_mask.high = 0;
  //                  _gateway->sendAttributeWrite(&_device,ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,
//ESP_ZB_ZCL_ATTR_TYPE_U48, 6, &_write_mask);

    if (_gateway->sendAttributeRead(&_device,ESP_ZB_ZCL_CLUSTER_ID_METERING, ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, true)) {
         esp_zb_uint48_t *value = (esp_zb_uint48_t *)_gateway->getReadAttrLastResult()->data.value;
         _supla_int64_t energy = ((_supla_int64_t)value->high << 32) + value->low;
	 setFwdActEnergy(0, energy);
       }
     }
  }

 protected:
   ZigbeeGateway 	*_gateway = nullptr;
   zb_device_params_t 	_device;  
   bool                 _active_query = false;
   esp_zb_uint48_t      _write_mask;

};

};  // namespace Sensor
};  // namespace Supla

#endif  //Z2S_ONE_PHASE_ELECTRICITY_METER_H_