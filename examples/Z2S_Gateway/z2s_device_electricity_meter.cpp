#include "z2s_device_electricity_meter.h"

void initZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {
  
  bool _isTuya, _active_query;
  bool _one_phase = true;

  uint16_t current_multiplier_modifier = 1;
  uint16_t current_divisor_modifier    = 1;
  
  uint16_t current_multiplier = 0;
  uint16_t current_divisor    = 0;

  uint16_t voltage_multiplier = 0;
  uint16_t voltage_divisor    = 0;
  
  uint16_t active_power_multiplier = 0;
  uint16_t active_power_divisor    = 0;

  bool ignore_zigbee_scaling = false;

  switch (z2s_channels_table[channel_number_slot].model_id) {
    
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {
      _isTuya = true; _active_query = false; //true;
    } break;

    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER: {
      _isTuya = true; _active_query = false;
    } break;

    case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER: {
      _one_phase = false; 
      voltage_multiplier = 1;
      voltage_divisor    = 10;
    } break;

    case Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER: {
      _one_phase = true;
      voltage_multiplier = 1;
      voltage_divisor    = 10;
    } break;

    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A:
      current_divisor_modifier = 1000; break;

    case Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER: {

      current_multiplier = 1;
      current_divisor    = 1000;

      voltage_multiplier = 1;
      voltage_divisor    = 1000;
  
      active_power_multiplier = 1;
      active_power_divisor    = 1000;

      ignore_zigbee_scaling = true;

    } break;

    case Z2S_DEVICE_DESC_DEVELCO_RELAY_ELECTRICITY_METER: {

      /*current_multiplier = 1;
      current_divisor    = 1000;

      voltage_multiplier = 1;
      voltage_divisor    = 1000;
  
      active_power_multiplier = 1;
      active_power_divisor    = 1000;*/

      ignore_zigbee_scaling = false;

    } break;

    default: {
      _isTuya = false; _active_query = false;
    }
  }

  auto Supla_Z2S_ElectricityMeter = new Supla::Sensor::Z2S_ElectricityMeter(gateway, device, 
                                    &z2s_channels_table[channel_number_slot].data_counter, _isTuya, _active_query, _one_phase);

  Supla_Z2S_ElectricityMeter->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);
  
  Supla_Z2S_ElectricityMeter->setKeepAliveSecs(z2s_channels_table[channel_number_slot].keep_alive_secs);

  Supla_Z2S_ElectricityMeter->setTimeoutSecs(z2s_channels_table[channel_number_slot].timeout_secs);

  Supla_Z2S_ElectricityMeter->setRefreshSecs(z2s_channels_table[channel_number_slot].refresh_secs);

  Supla_Z2S_ElectricityMeter->setEnergyInitialCounter(z2s_channels_table[channel_number_slot].data_counter);

  Supla_Z2S_ElectricityMeter->setIgnoreZigbeeScaling(ignore_zigbee_scaling);

  Supla_Z2S_ElectricityMeter->setCurrentMultiplierModifier(current_multiplier_modifier);

  Supla_Z2S_ElectricityMeter->setCurrentDivisorModifier(current_divisor_modifier);

  Supla_Z2S_ElectricityMeter->setVoltageMultiplier(voltage_multiplier, false);

  Supla_Z2S_ElectricityMeter->setVoltageDivisor(voltage_divisor, false);

  Supla_Z2S_ElectricityMeter->setCurrentMultiplier(current_multiplier, false);

  Supla_Z2S_ElectricityMeter->setCurrentDivisor(current_divisor, false);

  Supla_Z2S_ElectricityMeter->setActivePowerMultiplier(active_power_multiplier, false);

  Supla_Z2S_ElectricityMeter->setActivePowerDivisor(active_power_divisor, false);

}

void addZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zbg_device_params_t *device, bool isTuya, bool active_query, uint8_t free_slot,
                                  int8_t sub_id, bool one_phase) {
  
  auto Supla_Z2S_ElectricityMeter = new Supla::Sensor::Z2S_ElectricityMeter(gateway, device, 0, isTuya, active_query, one_phase);
  
  if (active_query) 
    z2s_channels_table[free_slot].refresh_secs = 30; //active_query replacement 
  
  Z2S_fillChannelsTableSlot(device, free_slot, Supla_Z2S_ElectricityMeter->getChannelNumber(), 
                            SUPLA_CHANNELTYPE_ELECTRICITY_METER, sub_id, "Electricity meter");
}

void msgZ2SDeviceElectricityMeter(int16_t channel_number_slot, uint8_t selector, int64_t value, signed char rssi) {

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
        auto Supla_ElectricityMeter = reinterpret_cast<Supla::Sensor::Z2S_ElectricityMeter *>(element);
        Supla_ElectricityMeter->pong();
        log_i("selector 0x%x, value %lld", selector, value);
        
        //Supla_OnePhaseElectricityMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        switch (selector) {
          
          case Z2S_EM_VOLTAGE_A_SEL: Supla_ElectricityMeter->setVoltage2(0, value); break;
          case Z2S_EM_CURRENT_A_SEL: Supla_ElectricityMeter->setCurrent2(0, abs(value)); break;
          case Z2S_EM_ACTIVE_POWER_A_SEL: Supla_ElectricityMeter->setPowerActive2(0, value); break;
          case Z2S_EM_ACT_FWD_ENERGY_A_SEL: Supla_ElectricityMeter->setFwdActEnergy2(0, value); break;
          case Z2S_EM_ACT_RVR_ENERGY_A_SEL: Supla_ElectricityMeter->setRvrActEnergy(0, value * 1000); break;
          case Z2S_EM_POWER_FACTOR_A_SEL: Supla_ElectricityMeter->setPowerFactor(0, value * 1000); break;
          case Z2S_EM_REACTIVE_POWER_A_SEL: Supla_ElectricityMeter->setPowerReactive(0, value * 100000); break;
          
          case Z2S_EM_VOLTAGE_B_SEL: Supla_ElectricityMeter->setVoltage2(1, value); break;
          case Z2S_EM_CURRENT_B_SEL: Supla_ElectricityMeter->setCurrent2(1, abs(value)); break;
          case Z2S_EM_ACTIVE_POWER_B_SEL: Supla_ElectricityMeter->setPowerActive2(1, value); break;
          case Z2S_EM_ACT_FWD_ENERGY_B_SEL: Supla_ElectricityMeter->setFwdActEnergy2(1, value); break;
          case Z2S_EM_ACT_RVR_ENERGY_B_SEL: Supla_ElectricityMeter->setRvrActEnergy(1, value * 1000); break;
          case Z2S_EM_POWER_FACTOR_B_SEL: Supla_ElectricityMeter->setPowerFactor(1, value * 1000); break;

          case Z2S_EM_VOLTAGE_C_SEL: Supla_ElectricityMeter->setVoltage2(2, value); break;
          case Z2S_EM_CURRENT_C_SEL: Supla_ElectricityMeter->setCurrent2(2, abs(value)); break;
          case Z2S_EM_ACTIVE_POWER_C_SEL: Supla_ElectricityMeter->setPowerActive2(2, value); break;
          case Z2S_EM_ACT_FWD_ENERGY_C_SEL: Supla_ElectricityMeter->setFwdActEnergy2(2, value); break;
          case Z2S_EM_ACT_RVR_ENERGY_C_SEL: Supla_ElectricityMeter->setRvrActEnergy(2, value * 1000); break;
          case Z2S_EM_POWER_FACTOR_C_SEL: Supla_ElectricityMeter->setPowerFactor(2, value * 1000); break;

          case Z2S_EM_AC_FREQUENCY: Supla_ElectricityMeter->setFreq2(value); break;

          case Z2S_EM_AC_VOLTAGE_MUL_SEL: Supla_ElectricityMeter->setVoltageMultiplier(value); break;
          case Z2S_EM_AC_VOLTAGE_DIV_SEL: Supla_ElectricityMeter->setVoltageDivisor(value); break;
          case Z2S_EM_AC_CURRENT_MUL_SEL: Supla_ElectricityMeter->setCurrentMultiplier(value); break;
          case Z2S_EM_AC_CURRENT_DIV_SEL: Supla_ElectricityMeter->setCurrentDivisor(value); break;
          case Z2S_EM_AC_ACTIVE_POWER_MUL_SEL:  Supla_ElectricityMeter->setActivePowerMultiplier(value); break;
          case Z2S_EM_AC_ACTIVE_POWER_DIV_SEL:  Supla_ElectricityMeter->setActivePowerDivisor(value); break;
          case Z2S_EM_AC_FREQUENCY_MUL_SEL: Supla_ElectricityMeter->setFreqMultiplier(value); break;
          case Z2S_EM_AC_FREQUENCY_DIV_SEL: Supla_ElectricityMeter->setFreqDivisor(value); break;
          case Z2S_EM_ACT_FWD_ENERGY_MUL_SEL: Supla_ElectricityMeter->setEnergyMultiplier(value); break;
          case Z2S_EM_ACT_FWD_ENERGY_DIV_SEL: Supla_ElectricityMeter->setEnergyDivisor(value); break;

        }
    }
}
