#ifndef Z2S_ELECTRICITY_METER_H_
#define Z2S_ELECTRICITY_METER_H_


#include <supla/log_wrapper.h>
#include <supla/sensor/electricity_meter.h>
#include "ZigbeeGateway.h"

#define CHANNEL_EXTENDED_DATA_TYPE_EM 0x01

typedef struct channel_extended_data_em_s {

  esp_zb_ieee_addr_t  ieee_addr;
  uint64_t            total_forward_active_energy_counter[3];
  uint64_t            total_reverse_active_energy_counter[3];
  uint64_t            total_forward_reactive_energy_counter[3];
  uint64_t            total_reverse_reactive_energy_counter[3];
  uint64_t            total_forward_active_energy_balanced_counter;
  uint64_t            total_reverse_active_energy_balanced_counter;
} __attribute__ ((packed)) channel_extended_data_em_t;


namespace Supla {
namespace Sensor {

class Z2S_ElectricityMeter : public ElectricityMeter {
 public:
  Z2S_ElectricityMeter(ZigbeeGateway *gateway, 
                       zbg_device_params_t *device, 
                      bool isTuya, 
		                  bool active_query = false, 
                      bool one_phase = true) : 
    _gateway(gateway) {
	
    memcpy(&_device, device, sizeof(zbg_device_params_t));
	
    //_active_query = active_query;
    //_isTuya       = isTuya;
	
    if (active_query) 
      setRefreshSecs(30);
	
    if (one_phase) {
	  
      extChannel.setFlag(SUPLA_CHANNEL_FLAG_PHASE2_UNSUPPORTED);
      extChannel.setFlag(SUPLA_CHANNEL_FLAG_PHASE3_UNSUPPORTED);
    }
  }

void setVoltageMultiplier(uint16_t voltage_multiplier, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;

  if (voltage_multiplier > 0)
    _voltage_multiplier = voltage_multiplier;
}

uint16_t getVoltageMultiplier() {

  return _voltage_multiplier;
}

void setVoltageDivisor(uint16_t voltage_divisor, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (voltage_divisor > 0)
    _voltage_divisor = voltage_divisor;
}

uint16_t getVoltageDivisor() {
	
  return _voltage_divisor;
}

void setCurrentMultiplier(uint16_t current_multiplier, bool from_zigbee = true) {

  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (current_multiplier > 0)
    _current_multiplier = current_multiplier;
}

uint16_t getCurrentMultiplier() {

  return _current_multiplier;
}

void setCurrentDivisor(uint16_t current_divisor, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (current_divisor > 0)
    _current_divisor = current_divisor;
}

uint16_t getCurrentDivisor() {
  
  return _current_divisor;
}

void setCurrentMultiplierModifier(uint16_t current_multiplier_modifier) {

  if (current_multiplier_modifier > 0)
    _current_multiplier_modifier = current_multiplier_modifier;
}

uint16_t getCurrentMultiplierModifier() {

  return _current_multiplier_modifier;
}

void setCurrentDivisorModifier(uint16_t current_divisor_modifier) {
  
  if (current_divisor_modifier > 0)
    _current_divisor_modifier = current_divisor_modifier;
}

uint16_t getCurrentDivisorModifier() {
  
  return _current_divisor_modifier;
}

void setActivePowerMultiplier(uint16_t active_power_multiplier, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (active_power_multiplier > 0)
    _active_power_multiplier = active_power_multiplier;
}

uint16_t getActivePowerMultiplier() {
  
  return _active_power_multiplier;
} 

void setActivePowerDivisor(uint16_t active_power_divisor, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (active_power_divisor > 0)  
    _active_power_divisor = active_power_divisor;
}

uint16_t getActivePowerDivisor() {
  
  return _active_power_divisor;
}

void setFreqMultiplier(uint16_t ac_frequency_multiplier, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (ac_frequency_multiplier > 0)
    _ac_frequency_multiplier = ac_frequency_multiplier;
}

uint16_t getFreqMultiplier() {
  
  return _ac_frequency_multiplier;
} 

void setFreqDivisor(uint16_t ac_frequency_divisor, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (ac_frequency_divisor > 0)  
    _ac_frequency_divisor = ac_frequency_divisor;
}

uint16_t getFreqDivisor() {
  
  return _ac_frequency_divisor;
}

void setEnergyMultiplier(uint32_t energy_multiplier, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (energy_multiplier > 0)
    _energy_multiplier = energy_multiplier;
}

uint32_t getEnergyMultiplier() {
  
  return _energy_multiplier;
} 

void setEnergyDivisor(uint16_t energy_divisor, bool from_zigbee = true) {
  
  if (_ignore_zigbee_scaling && from_zigbee)
    return;
  
  if (energy_divisor > 0)  
    _energy_divisor = energy_divisor;
}

uint32_t getEnergyDivisor() {
  
  return _energy_divisor;
}

void setIgnoreZigbeeScaling(bool ignore_zigbee_scaling) {

  _ignore_zigbee_scaling = ignore_zigbee_scaling;
}

bool getIgnoreZigbeeScaling() {

  return _ignore_zigbee_scaling;
}
 
void setVoltage2(int phase, uint32_t voltage) {

  uint32_t em_voltage;

  if ((_voltage_multiplier == 0) || (_voltage_divisor == 0)) 
    em_voltage = voltage * 100;
  else 
    em_voltage = (voltage * _voltage_multiplier * 100) / _voltage_divisor;

  log_d("_voltage_multiplier = %d, _voltage_divisor = %d, em_voltage = %ld", 
        _voltage_multiplier, _voltage_divisor, em_voltage);

  setVoltage(phase, em_voltage);
}

void setCurrent2(int phase, uint32_t current) {

  uint32_t em_current;

  if ((_current_multiplier == 0) || (_current_divisor == 0))
    em_current = current * 1;
  else	
    em_current = (current * _current_multiplier * _current_multiplier_modifier * 1000) / 
                 (_current_divisor * _current_divisor_modifier);

  log_d("_current_multiplier = %d, _current_divisor = %d, em_current = %ld", 
        _current_multiplier, _current_divisor, em_current);

  setCurrent(phase, em_current);
}

void setPowerActive2(int phase, int64_t power) {

  int64_t em_power;
  
  if ((_active_power_multiplier == 0) || (_active_power_divisor == 0))
    em_power = power * 100000;
  else
    em_power = (power * _active_power_multiplier * 100000) / _active_power_divisor;

  log_d("_active_power_multiplier = %d, _active_power_divisor = %d, em_power = %lld", 
        _active_power_multiplier, _active_power_divisor, em_power);

  setPowerActive(phase, em_power);
}

void setFreq2(uint32_t freq) {

  uint32_t em_freq;
  
  if ((_ac_frequency_multiplier == 0) || (_ac_frequency_divisor == 0))
    em_freq = freq * 100;
  else
    em_freq = (freq * _ac_frequency_multiplier * 100) / _ac_frequency_divisor;

  log_d("_ac_frequency_multiplier = %d, _ac_frequency_divisor = %d, em_freq = %ld", 
        _ac_frequency_multiplier, _ac_frequency_divisor, em_freq);

  setFreq(em_freq);
}

void setFwdActEnergy2(int phase, unsigned _supla_int64_t energy) {

  if (energy == 0)
    return;

  if ((_energy_multiplier != 0) && (_energy_divisor != 0))
    energy = (energy * _energy_multiplier * 100000) / _energy_divisor;
  else energy *= 100000;
  
  if (energy > total_forward_active_energy_counter[phase])
    energy -= total_forward_active_energy_counter[phase];
  else
    energy = 0;

  setFwdActEnergy(phase, energy);
}

void setRvrActEnergy2(int phase, unsigned _supla_int64_t energy) {

  if (energy == 0)
    return;

  if ((_energy_multiplier != 0) && (_energy_divisor != 0))
    energy = (energy * _energy_multiplier * 100000) / _energy_divisor;
  else energy *= 100000;
  
  if (energy > total_reverse_active_energy_counter[phase])
    energy -= total_reverse_active_energy_counter[phase];
  else
    energy = 0;

  setRvrActEnergy(phase, energy);
}

void onInit() override {
  
  if (_timeout_enabled)
    getChannel()->setStateOffline();
}

virtual void readValuesFromDevice() {

}

void resetStorage() {

  channel_extended_data_em_t channel_extended_data_em = {};

  memcpy(
    channel_extended_data_em.ieee_addr, _device.ieee_addr, 
    sizeof(esp_zb_ieee_addr_t));

  for (uint8_t i = 0; i <3; i++) {
  
    total_forward_active_energy_counter[i] = 
      emValue.total_forward_active_energy[i];
    channel_extended_data_em.total_forward_active_energy_counter[i] = 
      total_forward_active_energy_counter[i];
    
    total_reverse_active_energy_counter[i] = 
      emValue.total_reverse_active_energy[i];
    channel_extended_data_em.total_reverse_active_energy_counter[i] = 
      total_reverse_active_energy_counter[i];

    total_forward_reactive_energy_counter[i] = 
      emValue.total_forward_reactive_energy[i];
    channel_extended_data_em.total_forward_reactive_energy_counter[i] = 
      total_forward_reactive_energy_counter[i];

    total_reverse_reactive_energy_counter[i] = 
      emValue.total_reverse_reactive_energy[i];
    channel_extended_data_em.total_reverse_reactive_energy_counter[i] = 
      total_reverse_reactive_energy_counter[i];

  }

  total_forward_active_energy_balanced_counter = 
    emValue.total_forward_active_energy_balanced;
  channel_extended_data_em.total_forward_active_energy_balanced_counter = 
    total_forward_active_energy_balanced_counter;

  total_reverse_active_energy_balanced_counter = 
    emValue.total_reverse_active_energy_balanced;
  channel_extended_data_em.total_reverse_active_energy_balanced_counter = 
    total_reverse_active_energy_balanced_counter;

  _gateway->requestDataSave(
    getChannelNumber(), 16, CHANNEL_EXTENDED_DATA_TYPE_EM, 
    (uint8_t *)&channel_extended_data_em);

/*
  _energy_initial_counter = emValue.total_forward_active_energy[0];
  if (_data_counter) {
  
  *_data_counter = _energy_initial_counter;
  _gateway->requestDataSave(0);
  }*/
}


  void initZigbee() {
	
    if (_gateway && Zigbee.started()) {
      
      uint16_t em_attributes[8] = { 
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID, 
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID,
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID, 
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID,
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID, 
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID,
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MULTIPLIER_ID, 
        ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_DIVISOR_ID };

      uint16_t sm_attributes[2] = { ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, 
                                    ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID };
                                  

      _gateway->sendAttributesRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 8, 
        &em_attributes[0]);

      _gateway->sendAttributesRead(
        &_device, ESP_ZB_ZCL_CLUSTER_ID_METERING, 2, &sm_attributes[0]);
      
      if (_init_ms < 3600000)
        _init_ms *= 2;
    }
  }

  void ping() {

  uint16_t attributes[3] = {  
    ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, 
    ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID,
    ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID  };

  if (_gateway && Zigbee.started()) {

    _gateway->sendAttributesRead(
      &_device, 
      ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 
      3, &attributes[0]);

    _gateway->sendAttributeRead(
      &_device, 
      ESP_ZB_ZCL_CLUSTER_ID_METERING, 
      ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, 
      false);
  }


}

void pong() {

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!getChannel()->isStateOnline()) 
	  getChannel()->setStateOnline();
}


void setKeepAliveSecs(uint32_t keep_alive_secs) {

  _keep_alive_ms = keep_alive_secs * 1000;
  if (_keep_alive_ms == 0) {
    _keep_alive_enabled = false;
    //getChannel()->setStateOnline();
  } else
    _keep_alive_enabled = true;
}

  void setTimeoutSecs(uint32_t timeout_secs) {

  _timeout_ms = timeout_secs * 1000;
  if (_timeout_ms == 0) {
    _timeout_enabled = false;
    getChannel()->setStateOnline();
  } else
    _timeout_enabled = true;
}

void setRefreshSecs(uint32_t refresh_secs) {

  _refresh_ms = refresh_secs * 1000;
  if (_refresh_ms == 0) 
    _refresh_enabled = false; 
  else 
    _refresh_enabled = true;
 
}

void setEnergyInitialCounters(
  channel_extended_data_em_t *channel_extended_data_em) {

  for (uint8_t i = 0; i <3; i++) {
  
    total_forward_active_energy_counter[i] = 
      channel_extended_data_em->total_forward_active_energy_counter[i];

    log_i("total_forward_active_energy_counter[%u] = %llu, " 
          "channel_extended_data_em->total_forward_active_energy_counter[%u] = %llu",
          i,
          total_forward_active_energy_counter[i],
          i,
          channel_extended_data_em->total_forward_active_energy_counter[i]);

    total_reverse_active_energy_counter[i] = 
      channel_extended_data_em->total_reverse_active_energy_counter[i];

    total_forward_reactive_energy_counter[i] = 
      channel_extended_data_em->total_forward_reactive_energy_counter[i];

    total_reverse_reactive_energy_counter[i] = 
      channel_extended_data_em->total_reverse_reactive_energy_counter[i];
  }
  
  total_forward_active_energy_balanced_counter = 
    channel_extended_data_em->total_forward_active_energy_balanced_counter;

  total_reverse_active_energy_balanced_counter = 
    channel_extended_data_em->total_reverse_active_energy_balanced_counter;
}

uint32_t getKeepAliveSecs() {

  return _keep_alive_ms / 1000;
}

uint32_t getTimeoutSecs() {

  return _timeout_ms / 1000;
}

uint32_t getRefreshSecs() {

  return _refresh_ms / 1000;
}

/*uint64_t getEnergyInitialCounter() {

  return _energy_initial_counter;
}*/


void iterateAlways() override {

  Supla::Sensor::ElectricityMeter::iterateAlways();

  if (!_ignore_zigbee_scaling) {

    if (_fresh_start && ((millis() - _last_init_ms) > _init_ms)) {
      initZigbee();
      _last_init_ms = millis();
    }
  }

  if ((_refresh_enabled) && ((millis() - _last_refresh_ms) > _refresh_ms)) {
    if (_gateway) {
      
      //_last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
      //if ((millis() - _last_seen_ms) > _keep_alive_ms) {
      	ping();
        _last_ping_ms = millis();
      //} else {
        //_last_ping_ms = _last_seen_ms;
        if ((!getChannel()->isStateOnline()) && ((millis() - _last_seen_ms) < _timeout_ms)) 
	  getChannel()->setStateOnline();
      //}
    }
    _last_refresh_ms = millis();
  } else
  if (_keep_alive_enabled && ((millis() - _last_ping_ms) > _keep_alive_ms)) {
    if (_gateway) {
      
      //_last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
      if ((millis() - _last_seen_ms) > _keep_alive_ms) {
      	ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        if (!getChannel()->isStateOnline()) 
	  getChannel()->setStateOnline();
      }
    }
  }
  if (_timeout_enabled && getChannel()->isStateOnline() && ((millis() - _last_seen_ms) > _timeout_ms)) {
	  log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);
    //_last_seen_ms = _gateway->getZbgDeviceUnitLastSeenMs(_device.short_addr);
    log_i("current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);
    if ((millis() - _last_seen_ms) > _timeout_ms)
      getChannel()->setStateOffline();
  }
}

  protected:
    ZigbeeGateway 	     *_gateway = nullptr;
    zbg_device_params_t  _device;  
    //uint64_t		         *_data_counter = nullptr;
    //channel_extended_data_em_t *_ext_data = nullptr;
    //bool                 _active_query  = false;
    //esp_zb_uint48_t      _write_mask;
    //bool                 _isTuya = false;

    bool _fresh_start	        = true;

    bool _ignore_zigbee_scaling = false;

    //uint8_t _em_scaling_mask          = 0xFF; //0x3F; //bits 0-5 on
    //uint8_t _sm_scaling_mask          = 0x03; // bits 0-1 on

    uint16_t _voltage_multiplier      = 0;
    uint16_t _voltage_divisor 	      = 0;
    uint16_t _current_multiplier      = 0;
    uint16_t _current_divisor         = 0;
    uint16_t _active_power_multiplier = 0;
    uint16_t _active_power_divisor    = 0;

    uint16_t _ac_frequency_multiplier = 0;
    uint16_t _ac_frequency_divisor    = 0;

    uint32_t _energy_multiplier       = 0;
    uint32_t _energy_divisor          = 0;

    uint16_t _current_multiplier_modifier = 1;
    uint16_t _current_divisor_modifier    = 1;

    bool _keep_alive_enabled = true;
    bool _timeout_enabled    = true;
    bool _refresh_enabled    = true;

    uint64_t  total_forward_active_energy_counter[3];
    uint64_t  total_reverse_active_energy_counter[3];
    uint64_t  total_forward_reactive_energy_counter[3];
    uint64_t  total_reverse_reactive_energy_counter[3];
    uint64_t  total_forward_active_energy_balanced_counter;
    uint64_t  total_reverse_active_energy_balanced_counter;
    
    uint32_t _refresh_ms    = 30000;
    uint32_t _keep_alive_ms = 30000;
    uint32_t _timeout_ms    = 60000;
    uint32_t _init_ms       = 30000;
    
    uint32_t _last_ping_ms    = 0;
    uint32_t _last_seen_ms    = 0;
    uint32_t _last_refresh_ms = 0;
    uint32_t _last_init_ms    = 0;
};

};  // namespace Sensor
};  // namespace Supla

#endif  //Z2S_ELECTRICITY_METER_H_