#include "z2s_device_electricity_meter.h"

/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, 
                                   zbg_device_params_t *device, 
                                   int16_t channel_number_slot) {
  
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

  uint16_t ac_frequency_multiplier = 0;
  uint16_t ac_frequency_divisor = 0;

  uint32_t  energy_multiplier = 0;
  uint32_t  energy_divisor = 0;

  bool ignore_zigbee_scaling = false;

  switch (z2s_channels_table[channel_number_slot].model_id) {
    
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {

      _isTuya = true; 
      _active_query = false; //true;
    } break;


    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER: {

      _isTuya = true; 
      _active_query = false;

      if (strcmp(Z2S_getZbDeviceManufacturerName(
                  z2s_channels_table[channel_number_slot].ZB_device_id),
                  "_TZ3000_ww6drja5") == 0) {

        energy_multiplier = 1;
        energy_divisor  = 100;
      
        ignore_zigbee_scaling = true;
      }
    } break;

    case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER: {

      _one_phase = false; 

      voltage_multiplier = 1;
      voltage_divisor    = 10;

      ac_frequency_multiplier = 1;
      ac_frequency_divisor = 100;
      
      energy_multiplier = 1;
      energy_divisor  = 100;
      
      ignore_zigbee_scaling = true;
    } break;

    case Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER: {

      _one_phase = true;

      voltage_multiplier = 1;
      voltage_divisor    = 10;

      ac_frequency_multiplier = 1;
      ac_frequency_divisor = 100;

      energy_multiplier = 1;
      energy_divisor  = 100;
      
      ignore_zigbee_scaling = true;
    } break;


    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A: {

      //current_divisor_modifier = 1000; break;
      current_multiplier = 1;
      current_divisor    = 1000;

      energy_multiplier = 1;
      energy_divisor  = 100;

      ignore_zigbee_scaling = true;

      if (strcmp(Z2S_getZbDeviceManufacturerName(
                  z2s_channels_table[channel_number_slot].ZB_device_id),
                  "_TZ3000_kqvb5akv") == 0) {

        energy_multiplier = 1;
        energy_divisor  = 10000;
      }
    } break;


    case Z2S_DEVICE_DESC_SONOFF_RELAY_ELECTRICITY_METER: {

      current_multiplier = 1;
      current_divisor    = 1000;

      voltage_multiplier = 1;
      voltage_divisor    = 1000;
  
      active_power_multiplier = 1;
      active_power_divisor    = 1000;

      energy_multiplier = 1;
      energy_divisor  = 1000;

      ignore_zigbee_scaling = true;

    } break;

    case Z2S_DEVICE_DESC_TUYA_DIN_BREAKER_EM_TEMP: {

      current_multiplier = 1;
      current_divisor    = 1000;

      voltage_multiplier = 1;
      voltage_divisor    = 1;
  
      active_power_multiplier = 1;
      active_power_divisor    = 1;

      energy_multiplier = 1;
      energy_divisor  = 100;

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
      _isTuya = false; 
      _active_query = false;
    }
  }

  auto Supla_Z2S_ElectricityMeter = 
    new Supla::Sensor::Z2S_ElectricityMeter(gateway, 
                                            device, 
                                            _isTuya, 
                                            _active_query, 
                                            _one_phase);

  Supla_Z2S_ElectricityMeter->getChannel()->setChannelNumber(
    z2s_channels_table[channel_number_slot].Supla_channel);
  
  Supla_Z2S_ElectricityMeter->setKeepAliveSecs(
      z2s_channels_table[channel_number_slot].keep_alive_secs);

  Supla_Z2S_ElectricityMeter->setTimeoutSecs(
      z2s_channels_table[channel_number_slot].timeout_secs);

  Supla_Z2S_ElectricityMeter->setRefreshSecs(
      z2s_channels_table[channel_number_slot].refresh_secs);


  channel_extended_data_em_t channel_extended_data_em = {};
  
  if (Z2S_loadChannelExtendedData(channel_number_slot, 
                                  CHANNEL_EXTENDED_DATA_TYPE_EM,
                                  (uint8_t*)&channel_extended_data_em)) {

    char ieee_addr_str[24] = {};

    ieee_addr_to_str(ieee_addr_str, channel_extended_data_em.ieee_addr);

    log_i("em ext data ieee address: %s", 
          ieee_addr_str);
      

    if (memcmp(channel_extended_data_em.ieee_addr, 
               z2s_channels_table[channel_number_slot].ieee_addr, 
               sizeof(esp_zb_ieee_addr_t)) == 0) {

      log_i("EM channel extended data successfully LOADED");
      Supla_Z2S_ElectricityMeter->setEnergyInitialCounters(&channel_extended_data_em);

    }
  } else {

    memcpy(channel_extended_data_em.ieee_addr, 
           z2s_channels_table[channel_number_slot].ieee_addr, 
           sizeof(esp_zb_ieee_addr_t));
  
    if (Z2S_saveChannelExtendedData(channel_number_slot, 
                                    CHANNEL_EXTENDED_DATA_TYPE_EM,
                                    (uint8_t*)&channel_extended_data_em))
      log_i("EM channel extended data successfully CREATED");
  }
  
  Supla_Z2S_ElectricityMeter->setIgnoreZigbeeScaling(ignore_zigbee_scaling);

  Supla_Z2S_ElectricityMeter->setCurrentMultiplierModifier(current_multiplier_modifier);
  Supla_Z2S_ElectricityMeter->setCurrentDivisorModifier(current_divisor_modifier);

  Supla_Z2S_ElectricityMeter->setVoltageMultiplier(voltage_multiplier, false);
  Supla_Z2S_ElectricityMeter->setVoltageDivisor(voltage_divisor, false);

  Supla_Z2S_ElectricityMeter->setCurrentMultiplier(current_multiplier, false);
  Supla_Z2S_ElectricityMeter->setCurrentDivisor(current_divisor, false);

  Supla_Z2S_ElectricityMeter->setActivePowerMultiplier(active_power_multiplier, false);
  Supla_Z2S_ElectricityMeter->setActivePowerDivisor(active_power_divisor, false);

  Supla_Z2S_ElectricityMeter->setFreqMultiplier(ac_frequency_multiplier, false);
  Supla_Z2S_ElectricityMeter->setFreqDivisor(ac_frequency_divisor, false);

  Supla_Z2S_ElectricityMeter->setEnergyMultiplier(energy_multiplier, false);
  Supla_Z2S_ElectricityMeter->setEnergyDivisor(energy_divisor, false);
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void addZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, 
                                  zbg_device_params_t *device, 
                                  bool isTuya, 
                                  bool active_query, 
                                  uint8_t free_slot,
                                  int8_t sub_id, 
                                  bool one_phase) {
  
  auto Supla_Z2S_ElectricityMeter = 
    new Supla::Sensor::Z2S_ElectricityMeter(gateway, 
                                            device, 
                                            isTuya, 
                                            active_query, 
                                            one_phase);
  
  if (active_query) 
    z2s_channels_table[free_slot].refresh_secs = 30; //active_query replacement 
  
  channel_extended_data_em_t channel_extended_data_em = {};

  memcpy(channel_extended_data_em.ieee_addr, device->ieee_addr, sizeof(esp_zb_ieee_addr_t));

  Z2S_fillChannelsTableSlot(device, 
                            free_slot, 
                            Supla_Z2S_ElectricityMeter->getChannelNumber(), 
                            SUPLA_CHANNELTYPE_ELECTRICITY_METER, 
                            sub_id, 
                            "Electricity meter",
                            SUPLA_CHANNELFNC_ELECTRICITY_METER,
                            0xFF, 
                            CHANNEL_EXTENDED_DATA_TYPE_EM,
                            (uint8_t*)&channel_extended_data_em);
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void updateZ2SDeviceElectricityMeter(int16_t channel_number_slot) {

  channel_extended_data_em_t channel_extended_data_em = {};

  memcpy(channel_extended_data_em.ieee_addr, 
         z2s_channels_table[channel_number_slot].ieee_addr, 
         sizeof(esp_zb_ieee_addr_t));
  
  if (Z2S_saveChannelExtendedData(channel_number_slot, 
                                  CHANNEL_EXTENDED_DATA_TYPE_EM,
                                  (uint8_t*)&channel_extended_data_em,
                                  false))
    log_i("EM channel extended data successfully UPDATED (counters zeroed)"); 
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceElectricityMeter(int16_t channel_number_slot, 
                                  uint8_t emv_selector, 
                                  int64_t em_value) {

  Z2S_updateZbDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (element != nullptr && 
      element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {

        auto Supla_ElectricityMeter = reinterpret_cast<Supla::Sensor::Z2S_ElectricityMeter *>(element);
       
        Supla_ElectricityMeter->pong();
        
        log_i("selector 0x%x, value %lld", emv_selector, em_value);
        
        switch (emv_selector) {
          

          case Z2S_EM_VOLTAGE_A_SEL: 
            
            Supla_ElectricityMeter->setVoltage2(0, em_value); break;


          case Z2S_EM_CURRENT_A_SEL: 

              Supla_ElectricityMeter->setCurrent2(0, abs(em_value)); break;


          case Z2S_EM_ACTIVE_POWER_A_SEL: 
          
            Supla_ElectricityMeter->setPowerActive2(0, em_value); break;


          case Z2S_EM_ACT_FWD_ENERGY_A_SEL: 
          
            Supla_ElectricityMeter->setFwdActEnergy2(0, em_value); break;


          case Z2S_EM_ACT_RVR_ENERGY_A_SEL: 
              
            Supla_ElectricityMeter->setRvrActEnergy2(0, em_value); break;


          case Z2S_EM_POWER_FACTOR_A_SEL: 
          
            Supla_ElectricityMeter->setPowerFactor(0, em_value * 1000); break;


          case Z2S_EM_REACTIVE_POWER_A_SEL: 
            
            Supla_ElectricityMeter->setPowerReactive(0, em_value * 100000); break;
          


          case Z2S_EM_VOLTAGE_B_SEL: 
            
            Supla_ElectricityMeter->setVoltage2(1, em_value); break;
          
          
          case Z2S_EM_CURRENT_B_SEL: 
            
            Supla_ElectricityMeter->setCurrent2(1, abs(em_value)); break;
          
          
          case Z2S_EM_ACTIVE_POWER_B_SEL: 
          
            Supla_ElectricityMeter->setPowerActive2(1, em_value); break;
          
          
          case Z2S_EM_ACT_FWD_ENERGY_B_SEL: 
          
            Supla_ElectricityMeter->setFwdActEnergy2(1, em_value); break;
          
          
          case Z2S_EM_ACT_RVR_ENERGY_B_SEL: 
          
            Supla_ElectricityMeter->setRvrActEnergy2(1, em_value); break;
          
          
          case Z2S_EM_POWER_FACTOR_B_SEL: 
            
            Supla_ElectricityMeter->setPowerFactor(1, em_value * 1000); break;



          case Z2S_EM_VOLTAGE_C_SEL: 
            
            Supla_ElectricityMeter->setVoltage2(2, em_value); break;


          case Z2S_EM_CURRENT_C_SEL: 
          
            Supla_ElectricityMeter->setCurrent2(2, abs(em_value)); break;


          case Z2S_EM_ACTIVE_POWER_C_SEL: 
            
            Supla_ElectricityMeter->setPowerActive2(2, em_value); break;


          case Z2S_EM_ACT_FWD_ENERGY_C_SEL: 
          
            Supla_ElectricityMeter->setFwdActEnergy2(2, em_value); break;


          case Z2S_EM_ACT_RVR_ENERGY_C_SEL: 
          
            Supla_ElectricityMeter->setRvrActEnergy2(2, em_value); break;


          case Z2S_EM_POWER_FACTOR_C_SEL: 
            
            Supla_ElectricityMeter->setPowerFactor(2, em_value * 1000); break;



          case Z2S_EM_AC_FREQUENCY: 
            
            Supla_ElectricityMeter->setFreq2(em_value); break;



          case Z2S_EM_AC_VOLTAGE_MUL_SEL: 

            Supla_ElectricityMeter->setVoltageMultiplier(em_value); break;
          
          
          case Z2S_EM_AC_VOLTAGE_DIV_SEL: 

            Supla_ElectricityMeter->setVoltageDivisor(em_value); break;
          
          
          case Z2S_EM_AC_CURRENT_MUL_SEL: 

            Supla_ElectricityMeter->setCurrentMultiplier(em_value); break;
          
          
          case Z2S_EM_AC_CURRENT_DIV_SEL: 

            Supla_ElectricityMeter->setCurrentDivisor(em_value); break;
          
          
          case Z2S_EM_AC_ACTIVE_POWER_MUL_SEL:  

            Supla_ElectricityMeter->setActivePowerMultiplier(em_value); break;
          
          
          case Z2S_EM_AC_ACTIVE_POWER_DIV_SEL:  

            Supla_ElectricityMeter->setActivePowerDivisor(em_value); break;
          
          
          case Z2S_EM_AC_FREQUENCY_MUL_SEL: 

            Supla_ElectricityMeter->setFreqMultiplier(em_value); break;
          
          
          case Z2S_EM_AC_FREQUENCY_DIV_SEL: 

            Supla_ElectricityMeter->setFreqDivisor(em_value); break;
          
          
          case Z2S_EM_ACT_FWD_ENERGY_MUL_SEL: 

            Supla_ElectricityMeter->setEnergyMultiplier(em_value); break;
          
        
          case Z2S_EM_ACT_FWD_ENERGY_DIV_SEL: 

            Supla_ElectricityMeter->setEnergyDivisor(em_value); break;
        }
    }
}
