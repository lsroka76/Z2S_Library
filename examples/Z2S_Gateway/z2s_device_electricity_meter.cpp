#include "z2s_device_electricity_meter.h"

void initZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {
  
  bool _isTuya, _active_query;

  switch (z2s_devices_table[channel_number_slot].model_id) {
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {
      _isTuya = true; _active_query = false; //true;
    } break;
    case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER: {
      _isTuya = true; _active_query = false;
    } break;
    default: {
      _isTuya = false; _active_query = false;
    }
  }

  auto Supla_Z2S_OnePhaseElectricityMeter = new Supla::Sensor::Z2S_OnePhaseElectricityMeter(gateway, device, 
                                            &z2s_devices_table[channel_number_slot].data_counter, _isTuya, _active_query);

  Supla_Z2S_OnePhaseElectricityMeter->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
  
  //if (z2s_devices_table[channel_number_slot].keep_alive_secs > 0) 0 == disable
    Supla_Z2S_OnePhaseElectricityMeter->setKeepAliveSecs(z2s_devices_table[channel_number_slot].keep_alive_secs);

  //if (z2s_devices_table[channel_number_slot].timeout_secs > 0) 0 == disable
    Supla_Z2S_OnePhaseElectricityMeter->setTimeoutSecs(z2s_devices_table[channel_number_slot].timeout_secs);

  //if (z2s_devices_table[channel_number_slot].refresh_secs > 0) 0 == disable
    Supla_Z2S_OnePhaseElectricityMeter->setRefreshSecs(z2s_devices_table[channel_number_slot].refresh_secs);

  //if (z2s_devices_table[channel_number_slot].data_counter > 0)
    Supla_Z2S_OnePhaseElectricityMeter->setEnergyInitialCounter(z2s_devices_table[channel_number_slot].data_counter);

}

void addZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zbg_device_params_t *device, bool isTuya, bool active_query, uint8_t free_slot) {
  
  auto Supla_Z2S_OnePhaseElectricityMeter = new Supla::Sensor::Z2S_OnePhaseElectricityMeter(gateway, device, 0, isTuya, active_query);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_OnePhaseElectricityMeter->getChannelNumber(), SUPLA_CHANNELTYPE_ELECTRICITY_METER, -1);

}

void msgZ2SDeviceElectricityMeter(int16_t channel_number_slot, uint8_t selector, uint64_t value, signed char rssi) {

  Z2S_updateZBDeviceLastSeenMs(z2s_devices_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
        auto Supla_OnePhaseElectricityMeter = reinterpret_cast<Supla::Sensor::Z2S_OnePhaseElectricityMeter *>(element);
        Supla_OnePhaseElectricityMeter->pong();
        //Supla_OnePhaseElectricityMeter->getChannel()->setBridgeSignalStrength(Supla::rssiToSignalStrength(rssi));
        switch (selector) {
          case Z2S_EM_VOLTAGE_SEL: Supla_OnePhaseElectricityMeter->setVoltage(0, value * 100); break;
          case Z2S_EM_CURRENT_SEL: Supla_OnePhaseElectricityMeter->setCurrent(0, value * 1); break;
          case Z2S_EM_ACTIVE_POWER_SEL: Supla_OnePhaseElectricityMeter->setPowerActive(0, value * 100000); break;
          case Z2S_ACT_FWD_ENERGY_SEL: Supla_OnePhaseElectricityMeter->setFwdActEnergy2(0, value * 1000); break;
        }
    }
}
