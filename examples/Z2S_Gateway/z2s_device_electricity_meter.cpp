#include "z2s_device_electricity_meter.h"

/*****************************************************************************/

void initZ2SDeviceElectricityMeter(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel,
  Supla::Element *element) {

  Z2S_Device *device = Z2S_Device::getZ2SDeviceByDeviceIndex(
    _z2s_channel->Zb_device_id);

  if (device == nullptr) {
    log_e(
      "Fatal error - no ZigBee device for channel (index %u)", channel_index);
    return;
  }

  uint8_t profile_id = Z2S_EM_PROFILE_DEFAULT;
  uint32_t list_idx  = device->getDevicesListIdx();
  if (list_idx < Z2S_DEVICES_LIST_COUNT) {
    profile_id = Z2S_DEVICES_LIST[list_idx].z2s_device_profile_id;
  }
  if (profile_id >= Z2S_EM_PROFILE_COUNT) {
    profile_id = Z2S_EM_PROFILE_DEFAULT;
  }

  const z2s_em_profile_t &p = Z2S_EM_PROFILES[profile_id];

  bool one_phase             = !(p.flags & Z2S_EM_FLAG_THREE_PHASE);
  bool active_query          =  (p.flags & Z2S_EM_FLAG_ACTIVE_QUERY);
  bool ignore_zigbee_scaling =  (p.flags & Z2S_EM_FLAG_IGNORE_ZB_SCALING);
  bool extended_data_counter =  (p.flags & Z2S_EM_FLAG_EXTENDED_COUNTER);

  int64_t fwd_energy_counter = 0;

  Supla::Sensor::Z2S_ElectricityMeter *Supla_Z2S_ElectricityMeter = nullptr;

  if (element) {
    Supla_Z2S_ElectricityMeter =
      static_cast<Supla::Sensor::Z2S_ElectricityMeter *>(element);
  } else {
    Supla_Z2S_ElectricityMeter = new Supla::Sensor::Z2S_ElectricityMeter(
      active_query, one_phase);

    Supla_Z2S_ElectricityMeter->setZ2SChannel(channel_index, _z2s_channel);
    Supla_Z2S_ElectricityMeter->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);
  }

  if (active_query && (Supla_Z2S_ElectricityMeter->getRefreshMs() == 0))
    Supla_Z2S_ElectricityMeter->setRefreshValue(30);

  if (extended_data_counter) {
    if (!Supla_Z2S_ElectricityMeter->checkChannelUserDataFlags(
          USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
      fwd_energy_counter = Supla_Z2S_ElectricityMeter->getDataCounter();
      Supla_Z2S_ElectricityMeter->initZ2SChannelExtendedDataCounter();
      Supla_Z2S_ElectricityMeter->setChannelExtendedDataCounter(
        fwd_energy_counter);
    } else {
      fwd_energy_counter =
        Supla_Z2S_ElectricityMeter->getChannelExtendedDataCounter();
    }

    Supla_Z2S_ElectricityMeter->clearFwdEnergyBuffer();
    Supla_Z2S_ElectricityMeter->setFwdEnergyTimer(millis());
  }

  channel_extended_data_em_t channel_extended_data_em = {};

  if (Supla_Z2S_ElectricityMeter->loadChannelExtendedData(
        CHANNEL_EXTENDED_DATA_TYPE_EM,
        (uint8_t *)&channel_extended_data_em)) {

    char ieee_addr_str[24] = {};
    ieee_addr_to_str(ieee_addr_str, channel_extended_data_em.ieee_addr);
    log_i("em ext data ieee address: %s", ieee_addr_str);

    if (memcmp(
          channel_extended_data_em.ieee_addr, _z2s_channel->ieee_addr,
          sizeof(esp_zb_ieee_addr_t)) == 0) {
      log_i("EM channel extended data successfully LOADED");
      Supla_Z2S_ElectricityMeter->setEnergyInitialCounters(
        &channel_extended_data_em);
    }
  } else {
    memcpy(
      channel_extended_data_em.ieee_addr, _z2s_channel->ieee_addr,
      sizeof(esp_zb_ieee_addr_t));

    if (Supla_Z2S_ElectricityMeter->saveChannelExtendedData(
          CHANNEL_EXTENDED_DATA_TYPE_EM,
          (uint8_t *)&channel_extended_data_em))
      log_i("EM channel extended data successfully CREATED");
  }

  Supla_Z2S_ElectricityMeter->setIgnoreZigbeeScaling(ignore_zigbee_scaling);

  Supla_Z2S_ElectricityMeter->setCurrentMultiplierModifier(1);
  Supla_Z2S_ElectricityMeter->setCurrentDivisorModifier(p.c_div_mod);

  Supla_Z2S_ElectricityMeter->setVoltageMultiplier(p.v_mul, false);
  Supla_Z2S_ElectricityMeter->setVoltageDivisor(p.v_div, false);

  Supla_Z2S_ElectricityMeter->setCurrentMultiplier(p.c_mul, false);
  Supla_Z2S_ElectricityMeter->setCurrentDivisor(p.c_div, false);

  Supla_Z2S_ElectricityMeter->setActivePowerMultiplier(p.p_mul, false);
  Supla_Z2S_ElectricityMeter->setActivePowerDivisor(p.p_div, false);

  Supla_Z2S_ElectricityMeter->setFreqMultiplier(p.f_mul, false);
  Supla_Z2S_ElectricityMeter->setFreqDivisor(p.f_div, false);

  Supla_Z2S_ElectricityMeter->setPowerFactorMultiplier(p.pf_mul, false);
  Supla_Z2S_ElectricityMeter->setPowerFactorDivisor(p.pf_div, false);

  Supla_Z2S_ElectricityMeter->setEnergyMultiplier(p.e_mul, false);
  Supla_Z2S_ElectricityMeter->setEnergyDivisor(p.e_div, false);

  if (fwd_energy_counter)
    Supla_Z2S_ElectricityMeter->setFwdActEnergy2(0, fwd_energy_counter);
}

/*****************************************************************************/

void addZ2SDeviceElectricityMeter(
  ZigbeeGateway *gateway, zbg_device_params_t *device, bool isTuya, 
  bool active_query, uint8_t free_slot, int8_t sub_id, bool one_phase) {

  auto Supla_Z2S_ElectricityMeter = new Supla::Sensor::Z2S_ElectricityMeter(
    active_query, one_phase);

  Z2S_setChannelData(
    Supla_Z2S_ElectricityMeter->getZ2SCorePtr(), device, free_slot,
    Supla_Z2S_ElectricityMeter->getChannelNumber(), 
    SUPLA_CHANNELTYPE_ELECTRICITY_METER, sub_id, DEFAULT_EM_NAME, 
    SUPLA_CHANNELFNC_ELECTRICITY_METER);

  Supla_Z2S_ElectricityMeter->setInitialCaption(DEFAULT_EM_NAME);
  Supla_Z2S_ElectricityMeter->setDefaultFunction(
    SUPLA_CHANNELFNC_ELECTRICITY_METER);  

  initZ2SDeviceElectricityMeter(
    free_slot, Supla_Z2S_ElectricityMeter->getZ2SChannel(), 
    Supla_Z2S_ElectricityMeter->getZ2SElementPtr());

  Supla_Z2S_ElectricityMeter->getChannel()->setSubDeviceId(
    device->zb_device_id + 1);
  Supla_Z2S_ElectricityMeter->getChannel()->setFlag(
    SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

  Supla_Z2S_ElectricityMeter->onLoadConfig(&SuplaDevice);
  Supla_Z2S_ElectricityMeter->onInit();

  if (!Supla::Storage::IsStateStorageValid()) 
      Supla::Storage::WriteStateStorage();
          
  Supla::Storage::ConfigInstance()->commit();
}

/*****************************************************************************/

void msgZ2SDeviceElectricityMeter(
  Supla::Element* element, uint8_t emv_selector, int64_t em_value) {
  
  auto Supla_ElectricityMeter = static_cast<
    Supla::Sensor::Z2S_ElectricityMeter *>(element);
       
  Supla_ElectricityMeter->pong();

  Supla_ElectricityMeter->setZbDeviceLastSeenMs(millis());
        
  log_i("selector %u, value %lld", emv_selector, em_value);

  switch (emv_selector) {
    

    case Z2S_EM_VOLTAGE_A_SEL: 
      
      Supla_ElectricityMeter->setVoltage2(0, em_value); 
    break;


    case Z2S_EM_CURRENT_A_SEL: 

        Supla_ElectricityMeter->setCurrent2(0, abs(em_value)); 
    break;


    case Z2S_EM_ACTIVE_POWER_A_SEL: 
    
      Supla_ElectricityMeter->setPowerActive2(0, em_value); 
    break;


    case Z2S_EM_ACT_FWD_ENERGY_A_SEL: 
    
      Supla_ElectricityMeter->setFwdActEnergy2(0, em_value); 
    break;


    case Z2S_EM_ACT_RVR_ENERGY_A_SEL: 
        
      Supla_ElectricityMeter->setRvrActEnergy2(0, em_value); 
    break;


    case Z2S_EM_POWER_FACTOR_A_SEL: 
    
      Supla_ElectricityMeter->setPowerFactor2(0, em_value); 
    break;


    case Z2S_EM_REACTIVE_POWER_A_SEL: 
      
      Supla_ElectricityMeter->setPowerReactive2(0, em_value); 
    break;


    case Z2S_EM_APPARENT_POWER_A_SEL: 
      
      Supla_ElectricityMeter->setPowerApparent2(0, em_value); 
    break;

    //this is special case, when meter report only changes in energy value
    case Z2S_EM_ACT_FWD_ENERGY_A_DELTA_SEL: { 

      Supla_ElectricityMeter->addFwdEnergyBuffer(em_value);
      
      if ((Supla_ElectricityMeter->getFwdEnergyBuffer() >= 100) ||
          ((millis() - Supla_ElectricityMeter->getFwdEnergyTimer()) >= 
            1800000)) {

        uint64_t fwd_energy_counter = 
          Supla_ElectricityMeter->getChannelExtendedDataCounter() +
          Supla_ElectricityMeter->getFwdEnergyBuffer();

        Supla_ElectricityMeter->setChannelExtendedDataCounter(
          fwd_energy_counter);
  
        Supla_ElectricityMeter->clearFwdEnergyBuffer();
        Supla_ElectricityMeter->setFwdEnergyTimer(millis());

        Supla_ElectricityMeter->setFwdActEnergy2(0, fwd_energy_counter);
      }
    } break;

    
    case Z2S_EM_VOLTAGE_B_SEL: 
      
      Supla_ElectricityMeter->setVoltage2(1, em_value); 
    break;
    
    
    case Z2S_EM_CURRENT_B_SEL: 
      
      Supla_ElectricityMeter->setCurrent2(1, abs(em_value)); 
    break;
    
    
    case Z2S_EM_ACTIVE_POWER_B_SEL: 
    
      Supla_ElectricityMeter->setPowerActive2(1, em_value); 
    break;
    
    
    case Z2S_EM_ACT_FWD_ENERGY_B_SEL: 
    
      Supla_ElectricityMeter->setFwdActEnergy2(1, em_value); 
    break;
    
    
    case Z2S_EM_ACT_RVR_ENERGY_B_SEL: 
    
      Supla_ElectricityMeter->setRvrActEnergy2(1, em_value); 
    break;
    
    
    case Z2S_EM_POWER_FACTOR_B_SEL: 
      
      Supla_ElectricityMeter->setPowerFactor2(1, em_value); 
    break;


    case Z2S_EM_REACTIVE_POWER_B_SEL: 
      
      Supla_ElectricityMeter->setPowerReactive2(1, em_value); 
    break;


    case Z2S_EM_APPARENT_POWER_B_SEL: 
      
      Supla_ElectricityMeter->setPowerApparent2(1, em_value); 
    break;


    case Z2S_EM_VOLTAGE_C_SEL: 
      
      Supla_ElectricityMeter->setVoltage2(2, em_value); 
    break;


    case Z2S_EM_CURRENT_C_SEL: 
    
      Supla_ElectricityMeter->setCurrent2(2, abs(em_value)); 
    break;


    case Z2S_EM_ACTIVE_POWER_C_SEL: 
      
      Supla_ElectricityMeter->setPowerActive2(2, em_value); 
    break;


    case Z2S_EM_ACT_FWD_ENERGY_C_SEL: 
    
      Supla_ElectricityMeter->setFwdActEnergy2(2, em_value); 
    break;


    case Z2S_EM_ACT_RVR_ENERGY_C_SEL: 
    
      Supla_ElectricityMeter->setRvrActEnergy2(2, em_value); 
    break;


    case Z2S_EM_POWER_FACTOR_C_SEL: 
      
      Supla_ElectricityMeter->setPowerFactor2(2, em_value); 
    break;

    
    case Z2S_EM_REACTIVE_POWER_C_SEL: 
      
      Supla_ElectricityMeter->setPowerReactive2(2, em_value); 
    break;


    case Z2S_EM_APPARENT_POWER_C_SEL: 
      
      Supla_ElectricityMeter->setPowerApparent2(2, em_value); 
    break;


    case Z2S_EM_AC_FREQUENCY: 
      
      Supla_ElectricityMeter->setFreq2(em_value); 
    break;


    case Z2S_EM_FWD_BALANCED_ENERGY_SEL:

      Supla_ElectricityMeter->setFwdBalancedEnergy2(em_value); 
    break;


    case Z2S_EM_RVR_BALANCED_ENERGY_SEL:

      Supla_ElectricityMeter->setRvrBalancedEnergy2(em_value); 
    break;


    case Z2S_EM_AC_VOLTAGE_MUL_SEL: 

      Supla_ElectricityMeter->setVoltageMultiplier(em_value);   
    break;
    
    
    case Z2S_EM_AC_VOLTAGE_DIV_SEL: 

      Supla_ElectricityMeter->setVoltageDivisor(em_value); 
    break;
    
    
    case Z2S_EM_AC_CURRENT_MUL_SEL: 

      Supla_ElectricityMeter->setCurrentMultiplier(em_value); 
    break;
    
    
    case Z2S_EM_AC_CURRENT_DIV_SEL: 

      Supla_ElectricityMeter->setCurrentDivisor(em_value); break;
    
    
    case Z2S_EM_AC_ACTIVE_POWER_MUL_SEL:  

      Supla_ElectricityMeter->setActivePowerMultiplier(em_value); 
    break;
    
    
    case Z2S_EM_AC_ACTIVE_POWER_DIV_SEL:  

      Supla_ElectricityMeter->setActivePowerDivisor(em_value); 
    break;
    
    
    case Z2S_EM_AC_FREQUENCY_MUL_SEL: 

      Supla_ElectricityMeter->setFreqMultiplier(em_value); 
    break;
    
    
    case Z2S_EM_AC_FREQUENCY_DIV_SEL: 

      Supla_ElectricityMeter->setFreqDivisor(em_value); 
    break;
    
    
    case Z2S_EM_ACT_FWD_ENERGY_MUL_SEL: 

      Supla_ElectricityMeter->setEnergyMultiplier(em_value); 
    break;
    
  
    case Z2S_EM_ACT_FWD_ENERGY_DIV_SEL: 

      Supla_ElectricityMeter->setEnergyDivisor(em_value); 
    break;
  }
}


//test case
  /*if (strcmp(Z2S_getZbDeviceManufacturerName(
       _z2s_channel->Zb_device_id),
      "ADEO") == 0) {

    voltage_multiplier = 1;
    voltage_divisor = 1;

    current_multiplier = 1;
    current_divisor    = 1000;

    active_power_multiplier = 1;
    active_power_divisor = 1;

    energy_multiplier = 1;
    energy_divisor  = 100;

    ignore_zigbee_scaling = true;

    if (!Z2S_checkChannelFlags(
        channel_number_slot, USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
        
      fwd_energy_counter = 
        _z2s_channel->data_counter;

      Z2S_initChannelExtendedDataCounter(channel_number_slot);
      Z2S_setChannelExtendedDataCounter(
        channel_number_slot, fwd_energy_counter);
    } else
      fwd_energy_counter = Z2S_getChannelExtendedDataCounter(
        channel_number_slot);

      _z2s_channel->fwd_energy_buffer = 0;
      _z2s_channel->fwd_energy_timer = millis();
  }*/                                        
