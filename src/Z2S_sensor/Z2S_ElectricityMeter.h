#ifndef Z2S_ELECTRICITY_METER_H_
#define Z2S_ELECTRICITY_METER_H_

/*****************************************************************************/

#include <supla/log_wrapper.h>
#include <supla/sensor/electricity_meter.h>
#include "ZigbeeGateway.h"
#include "TuyaDatapoints.h"
#include "Z2S_common.h"

/*****************************************************************************/

namespace Supla {
namespace Sensor {

class Z2S_ElectricityMeter : public ElectricityMeter, public Z2S_Core {
 public:
  Z2S_ElectricityMeter(bool active_query = false, bool one_phase = true) 
    :  Z2S_Core(this), _one_phase(one_phase) {
	
    if (_one_phase) {
	  
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

void setPowerFactorMultiplier(
  uint16_t power_factor_multiplier, bool from_zigbee = true) {

  if (_ignore_zigbee_scaling && from_zigbee)
    return;

  if (power_factor_multiplier > 0)
    _power_factor_multiplier = power_factor_multiplier;
}

uint16_t getPowerFactorMultiplier() {

  return _power_factor_multiplier;
}

void setPowerFactorDivisor(
  uint16_t power_factor_divisor, bool from_zigbee = true) {

  if (_ignore_zigbee_scaling && from_zigbee)
    return;

  if (power_factor_divisor > 0)
    _power_factor_divisor = power_factor_divisor;
}

uint16_t getPowerFactorDivisor() {

  return _power_factor_divisor;
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

void setEnergyDivisor(uint32_t energy_divisor, bool from_zigbee = true) {
  
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
  if ((phase >= 0) && (phase < MAX_PHASES) &&
      checkChannelUserDataFlags(USER_DATA_FLAG_EM_SW_REVERSE_ENERGY)) {
    integrateSwReverseEnergy(phase, em_power);
  }
}

void setPowerReactive2(int phase, int64_t power) {

  int64_t em_power;
  
  if ((_active_power_multiplier == 0) || (_active_power_divisor == 0))
    em_power = power * 100000;
  else
    em_power = (power * _active_power_multiplier * 100000) / _active_power_divisor;

  log_d("_active_power_multiplier = %d, _active_power_divisor = %d, em_power = %lld", 
        _active_power_multiplier, _active_power_divisor, em_power);

  setPowerReactive(phase, em_power);
}

void setPowerApparent2(int phase, int64_t power) {

  int64_t em_power;
  
  if ((_active_power_multiplier == 0) || (_active_power_divisor == 0))
    em_power = power * 100000;
  else
    em_power = (power * _active_power_multiplier * 100000) / _active_power_divisor;

  log_d("_active_power_multiplier = %d, _active_power_divisor = %d, em_power = %lld", 
        _active_power_multiplier, _active_power_divisor, em_power);

  setPowerApparent(phase, em_power);
}

void setFreq2(uint32_t freq) {

  uint32_t em_freq;
  
  if ((_ac_frequency_multiplier == 0) || (_ac_frequency_divisor == 0))
    em_freq = freq * 100;
  else
    em_freq = (freq * _ac_frequency_multiplier * 100) / _ac_frequency_divisor;

  log_d(
    "_ac_frequency_multiplier = %d, _ac_frequency_divisor = %d, em_freq = %ld", 
    _ac_frequency_multiplier, _ac_frequency_divisor, em_freq);

  setFreq(em_freq);
}

void setPowerFactor2(int phase, int32_t power_factor) {

  int32_t em_power_factor;

  if ((_power_factor_multiplier == 0) || (_power_factor_divisor == 0))
    em_power_factor = power_factor * 1000;
  else
    em_power_factor =
      ((int64_t)power_factor * _power_factor_multiplier * 1000) /
      _power_factor_divisor;

  log_d(
    "_power_factor_multiplier = %d, _power_factor_divisor = %d, "
    "em_power_factor = %ld",
    _power_factor_multiplier, _power_factor_divisor, em_power_factor);

  setPowerFactor(phase, em_power_factor);
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

  log_d(
    "_energy_multiplier = %d, _energy_divisor = %d, energy = %lld", 
    _energy_multiplier, _energy_divisor, energy);

  setFwdActEnergy(phase, energy);
}

void setRvrActEnergy2(int phase, unsigned _supla_int64_t energy) {

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EM_SW_REVERSE_ENERGY))
    return;

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

void setFwdBalancedEnergy2(uint64_t energy) {

  if (energy == 0)
    return;

  if (_one_phase) {

    setFwdActEnergy2(0, energy);
    return;
  } else
    return; //temporary solution/

  if ((_energy_multiplier != 0) && (_energy_divisor != 0))
    energy = (energy * _energy_multiplier * 100000) / _energy_divisor;
  else energy *= 100000;
  
  if (energy > total_forward_active_energy_balanced_counter)
    energy -= total_forward_active_energy_balanced_counter;
  else
    energy = 0;

  setFwdBalancedEnergy(energy);
}

void setRvrBalancedEnergy2(uint64_t energy) {

  if (energy == 0)
    return;

  if (_one_phase) {

    setRvrActEnergy2(0, energy);
    return;
  } else
    return; //temporary solution


  if ((_energy_multiplier != 0) && (_energy_divisor != 0))
    energy = (energy * _energy_multiplier * 100000) / _energy_divisor;
  else energy *= 100000;
  
  if (energy > total_reverse_active_energy_balanced_counter)
    energy -= total_reverse_active_energy_balanced_counter;
  else
    energy = 0;

  setRvrBalancedEnergy(energy);
}

/*****************************************************************************/

void onInit() override {
  
  if (getTimeoutMs())
    getChannel()->setStateOffline();

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EM_SW_REVERSE_ENERGY)) {
    uint8_t phases = _one_phase ? 1 : MAX_PHASES;
    for (uint8_t i = 0; i < phases; i++) {
      setRvrActEnergy(i, total_reverse_active_energy_counter[i]);
    }
    _last_sw_rvr_save_ms = millis();
  }
}

/*****************************************************************************/

virtual void readValuesFromDevice() {

}

/*****************************************************************************/

void resetStorage() {

  bool sw_rvr = checkChannelUserDataFlags(USER_DATA_FLAG_EM_SW_REVERSE_ENERGY);
  channel_extended_data_em_t channel_extended_data_em = {};

  memcpy(
    channel_extended_data_em.ieee_addr, getChannelIeeeAddress(),
    sizeof(esp_zb_ieee_addr_t));

  taskENTER_CRITICAL(&Z2S_globalMutex);
  for (uint8_t i = 0; i < 3; i++) {

    total_forward_active_energy_counter[i] =
      emValue.total_forward_active_energy[i];
    channel_extended_data_em.total_forward_active_energy_counter[i] =
      total_forward_active_energy_counter[i];

    if (sw_rvr) {
      total_reverse_active_energy_counter[i] = 0;
      _rvr_energy_rem[i] = 0;
      _last_power_ms[i]  = 0;
    } else {
      total_reverse_active_energy_counter[i] =
        emValue.total_reverse_active_energy[i];
    }
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

  if (sw_rvr) {
    _sw_rvr_unsaved_delta = 0;
    _last_sw_rvr_save_ms  = millis();
    _sw_rvr_retry_pending = false;
  }
  taskEXIT_CRITICAL(&Z2S_globalMutex);

  if (sw_rvr) {
    uint8_t phases = _one_phase ? 1 : MAX_PHASES;
    for (uint8_t i = 0; i < phases; i++) {
      setRvrActEnergy(i, 0);
    }
  }

  total_forward_active_energy_balanced_counter =
    emValue.total_forward_active_energy_balanced;
  channel_extended_data_em.total_forward_active_energy_balanced_counter =
    total_forward_active_energy_balanced_counter;

  total_reverse_active_energy_balanced_counter =
    emValue.total_reverse_active_energy_balanced;
  channel_extended_data_em.total_reverse_active_energy_balanced_counter =
    total_reverse_active_energy_balanced_counter;

  saveChannelExtendedData(CHANNEL_EXTENDED_DATA_TYPE_EM,
    (uint8_t *)&channel_extended_data_em);
}

/*****************************************************************************/

void clearCounters() {

  channel_extended_data_em_t channel_extended_data_em = {};

  memcpy(
    channel_extended_data_em.ieee_addr, getChannelIeeeAddress(), 
    sizeof(esp_zb_ieee_addr_t));

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EM_SW_REVERSE_ENERGY)) {
    taskENTER_CRITICAL(&Z2S_globalMutex);
    for (uint8_t i = 0; i < 3; i++) {
      channel_extended_data_em.total_reverse_active_energy_counter[i] =
        total_reverse_active_energy_counter[i];
    }
    taskEXIT_CRITICAL(&Z2S_globalMutex);
  }

  saveChannelExtendedData(CHANNEL_EXTENDED_DATA_TYPE_EM, 
    (uint8_t *)&channel_extended_data_em);

  log_i("EM channel extended data successfully UPDATED (counters zeroed)");
}

/*****************************************************************************/

  void initZigbee() {
	
    if (Zigbee.started()) {
      
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
                                  

      zbGateway.sendAttributesRead(
        _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 8, 
        &em_attributes[0]);

      zbGateway.sendAttributesRead(
        _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_METERING, 2, &sm_attributes[0]);
      
      if (_init_ms < 3600000)
        _init_ms *= 2;
    }
  }

  void ping() {

  uint16_t attributes[3] = {  
    ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, 
    ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID,
    ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID  };

  if (Zigbee.started()) {

    zbGateway.sendAttributesRead(
      _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, 3, 
      &attributes[0]);

    zbGateway.sendAttributeRead(
      _short_addr, _endpoint, ESP_ZB_ZCL_CLUSTER_ID_METERING, 
      ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, 
      false);

	  switch (getZbDeviceModelId()) {
      
      
      case 0x4550:
      case 0x4551:
      
        sendTuyaRequestCmdEnum8(_short_addr, _endpoint, 0x23, 1);
      break;
    }
  }
}

void pong() {

  _last_ping_ms = millis();
  _last_seen_ms = _last_ping_ms;
  
  if (!getChannel()->isStateOnline()) 
	  getChannel()->setStateOnline();
}

void setEnergyInitialCounters(
  channel_extended_data_em_t *channel_extended_data_em) {

  for (uint8_t i = 0; i <3; i++) {
  
    total_forward_active_energy_counter[i] = 
      channel_extended_data_em->total_forward_active_energy_counter[i];

    log_i(
      "total_forward_active_energy_counter[%u] = %llu, " 
      "channel_extended_data_em->total_forward_active_energy_counter[%u]"
      " = %llu", i, total_forward_active_energy_counter[i], i,
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

/*****************************************************************************/

void iterateAlways() override {

  Supla::Sensor::ElectricityMeter::iterateAlways();

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EM_SW_REVERSE_ENERGY)) {
    uint64_t unsaved   = 0;
    uint32_t last_save = 0;
    bool retry_pending = false;

    taskENTER_CRITICAL(&Z2S_globalMutex);
    unsaved       = _sw_rvr_unsaved_delta;
    last_save     = _last_sw_rvr_save_ms;
    retry_pending = _sw_rvr_retry_pending;
    taskEXIT_CRITICAL(&Z2S_globalMutex);

    if (unsaved > 0) {
      uint32_t elapsed = millis() - last_save;
      bool should_save = retry_pending
        ? (elapsed >= SW_RVR_RETRY_BACKOFF_MS)
        : ((elapsed >= SW_RVR_SAVE_INTERVAL_MS) ||
           ((unsaved >= SW_RVR_SAVE_DELTA_UNITS) &&
            (elapsed >= SW_RVR_RETRY_BACKOFF_MS)));

      if (should_save) {
        saveSwReverseEnergyCounters();
      }
    }
  }

  if (!_ignore_zigbee_scaling) {

    if (_fresh_start && ((millis() - _last_init_ms) > _init_ms)) {
      initZigbee();
      _last_init_ms = millis();
    }
  }

  if ((getRefreshMs()) && ((millis() - _last_refresh_ms) > getRefreshMs())) {
    if (true) {

      _last_seen_ms = getZbDeviceLastSeenMs();
      //if ((millis() - _last_seen_ms) > getKeepAliveMs()) {
        ping();
        _last_ping_ms = millis();
      //} else {
        //_last_ping_ms = _last_seen_ms;
        if ((!getChannel()->isStateOnline()) && ((millis() - _last_seen_ms) < getTimeoutMs()))
          getChannel()->setStateOnline();
      //}
    }
    _last_refresh_ms = millis();
  } else
  if (getKeepAliveMs() && ((millis() - _last_ping_ms) > getKeepAliveMs())) {
    if (true) {

      _last_seen_ms = getZbDeviceLastSeenMs();

      if ((millis() - _last_seen_ms) > getKeepAliveMs()) {
        ping();
        _last_ping_ms = millis();
      } else {
        _last_ping_ms = _last_seen_ms;
        if (!getChannel()->isStateOnline())
          getChannel()->setStateOnline();
      }
    }
  }
  if (getTimeoutMs() && getChannel()->isStateOnline() &&
     ((millis() - _last_seen_ms) > getTimeoutMs())) {

    log_i("current_millis %u, _last_seen_ms %u", millis(), _last_seen_ms);

    _last_seen_ms = getZbDeviceLastSeenMs();

    log_i(
      "current_millis %u, _last_seen_ms(updated) %u", millis(), _last_seen_ms);

    if ((millis() - _last_seen_ms) > getTimeoutMs())
      getChannel()->setStateOffline();
  }
}

static constexpr uint32_t SW_RVR_SAVE_INTERVAL_MS = 1800000UL; // 30 min
static constexpr uint32_t SW_RVR_RETRY_BACKOFF_MS = 60000UL;   // 60 s
static constexpr uint32_t SW_RVR_MAX_DT_MS        = 120000UL;  // 2 min cap
static constexpr uint64_t SW_RVR_SAVE_DELTA_UNITS = 100000ULL; // 1.0 kWh

void onSwReverseEnergyFlagChanged(bool enabled) {

  if (!enabled) {
    uint64_t unsaved = 0;
    taskENTER_CRITICAL(&Z2S_globalMutex);
    unsaved = _sw_rvr_unsaved_delta;
    taskEXIT_CRITICAL(&Z2S_globalMutex);

    if (unsaved > 0) {
      saveSwReverseEnergyCounters();
    }
  }

  uint64_t rvr_snapshot[MAX_PHASES] = {};

  taskENTER_CRITICAL(&Z2S_globalMutex);
  for (uint8_t i = 0; i < MAX_PHASES; i++) {
    _last_power_ms[i]  = 0;
    _rvr_energy_rem[i] = 0;
    rvr_snapshot[i]    = total_reverse_active_energy_counter[i];
  }
  _sw_rvr_unsaved_delta = 0;
  _last_sw_rvr_save_ms  = millis();
  _sw_rvr_retry_pending = false;
  taskEXIT_CRITICAL(&Z2S_globalMutex);

  if (enabled) {
    uint8_t phases = _one_phase ? 1 : MAX_PHASES;
    for (uint8_t i = 0; i < phases; i++) {
      setRvrActEnergy(i, rvr_snapshot[i]);
    }
    valueChanged = true;
  } else {
    emValue.measured_values &= ~EM_VAR_REVERSE_ACTIVE_ENERGY;
    valueChanged = true;
  }
}

void integrateSwReverseEnergy(int phase, int64_t em_power) {

  uint32_t now_ms = millis();
  if (now_ms == 0) now_ms = 1;

  uint64_t new_rvr_energy = 0;
  bool updated = false;

  taskENTER_CRITICAL(&Z2S_globalMutex);
  uint32_t prev_ms = _last_power_ms[phase];
  _last_power_ms[phase] = now_ms;

  if ((em_power < 0) && (prev_ms != 0)) {
    uint32_t dt_ms = now_ms - prev_ms;
    if ((dt_ms > 0) && (dt_ms <= SW_RVR_MAX_DT_MS)) {
      // 1 Supla unit (0.00001 kWh) = 3,600,000,000 * (0.00001 W) * ms
      uint64_t acc   = static_cast<uint64_t>(-em_power) * dt_ms +
                       _rvr_energy_rem[phase];
      uint64_t delta = acc / 3600000000ULL;
      _rvr_energy_rem[phase] = acc % 3600000000ULL;

      if (delta > 0) {
        total_reverse_active_energy_counter[phase] += delta;
        _sw_rvr_unsaved_delta += delta;
        new_rvr_energy = total_reverse_active_energy_counter[phase];
        updated = true;
      }
    }
  }
  taskEXIT_CRITICAL(&Z2S_globalMutex);

  if (updated) {
    setRvrActEnergy(phase, new_rvr_energy);
  }
}

void saveSwReverseEnergyCounters() {

  channel_extended_data_em_t channel_extended_data_em = {};

  memcpy(
    channel_extended_data_em.ieee_addr, getChannelIeeeAddress(),
    sizeof(esp_zb_ieee_addr_t));

  uint64_t saved_delta = 0;

  taskENTER_CRITICAL(&Z2S_globalMutex);
  for (uint8_t i = 0; i < MAX_PHASES; i++) {
    channel_extended_data_em.total_forward_active_energy_counter[i] =
      total_forward_active_energy_counter[i];
    channel_extended_data_em.total_reverse_active_energy_counter[i] =
      total_reverse_active_energy_counter[i];
    channel_extended_data_em.total_forward_reactive_energy_counter[i] =
      total_forward_reactive_energy_counter[i];
    channel_extended_data_em.total_reverse_reactive_energy_counter[i] =
      total_reverse_reactive_energy_counter[i];
  }
  channel_extended_data_em.total_forward_active_energy_balanced_counter =
    total_forward_active_energy_balanced_counter;
  channel_extended_data_em.total_reverse_active_energy_balanced_counter =
    total_reverse_active_energy_balanced_counter;
  saved_delta = _sw_rvr_unsaved_delta;
  taskEXIT_CRITICAL(&Z2S_globalMutex);

  if (saveChannelExtendedData(
        CHANNEL_EXTENDED_DATA_TYPE_EM,
        (uint8_t *)&channel_extended_data_em)) {

    taskENTER_CRITICAL(&Z2S_globalMutex);
    _sw_rvr_unsaved_delta =
      (_sw_rvr_unsaved_delta > saved_delta)
        ? (_sw_rvr_unsaved_delta - saved_delta)
        : 0;
    _last_sw_rvr_save_ms  = millis();
    _sw_rvr_retry_pending = false;
    taskEXIT_CRITICAL(&Z2S_globalMutex);

    log_i("SW reverse energy counters saved to LittleFS");
  } else {

    taskENTER_CRITICAL(&Z2S_globalMutex);
    _last_sw_rvr_save_ms  = millis();
    _sw_rvr_retry_pending = true;
    taskEXIT_CRITICAL(&Z2S_globalMutex);

    log_e("SW reverse energy counters save FAILED - retrying in 60 s");
  }
}

  protected:

    bool _fresh_start	          = true;
    bool _one_phase             = true;
    bool _ignore_zigbee_scaling = false;
    bool _sw_rvr_retry_pending  = false;
    

    uint16_t _voltage_multiplier      = 0;
    uint16_t _voltage_divisor 	      = 0;
    uint16_t _current_multiplier      = 0;
    uint16_t _current_divisor         = 0;
    uint16_t _active_power_multiplier = 0;
    uint16_t _active_power_divisor    = 0;

    uint16_t _ac_frequency_multiplier = 0;
    uint16_t _ac_frequency_divisor    = 0;

    uint16_t _power_factor_multiplier = 0;
    uint16_t _power_factor_divisor    = 0;

    uint32_t _energy_multiplier       = 0;
    uint32_t _energy_divisor          = 0;

    uint16_t _current_multiplier_modifier = 1;
    uint16_t _current_divisor_modifier    = 1;

    uint64_t total_forward_active_energy_counter[3]          = {};
    uint64_t total_reverse_active_energy_counter[3]          = {};
    uint64_t total_forward_reactive_energy_counter[3]        = {};
    uint64_t total_reverse_reactive_energy_counter[3]        = {};
    uint64_t total_forward_active_energy_balanced_counter    = 0;
    uint64_t total_reverse_active_energy_balanced_counter    = 0;

    uint64_t _rvr_energy_rem[MAX_PHASES]                     = {};
    uint64_t _sw_rvr_unsaved_delta                           = 0;
    uint32_t _last_power_ms[MAX_PHASES]                      = {};
    uint32_t _last_sw_rvr_save_ms                            = 0;
    
    uint32_t _init_ms       = 30000;
    
    uint32_t _last_ping_ms    = 0;
    uint32_t _last_seen_ms    = 0;
    uint32_t _last_refresh_ms = 0;
    uint32_t _last_init_ms    = 0;
};

};  // namespace Sensor
};  // namespace Supla

#endif  //Z2S_ELECTRICITY_METER_H_
