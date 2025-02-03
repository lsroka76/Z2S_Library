#include "z2s_device_electricity_meter.h"

void initZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zb_device_params_t *device, bool active_query, uint8_t Supla_channel) {
  
  auto Supla_Z2S_OnePhaseElectricityMeter = new Supla::Sensor::Z2S_OnePhaseElectricityMeter(gateway, device, active_query);
  Supla_Z2S_OnePhaseElectricityMeter->getChannel()->setChannelNumber(Supla_channel);
}

void addZ2SDeviceElectricityMeter(ZigbeeGateway *gateway, zb_device_params_t *device, bool active_query, uint8_t free_slot, uint8_t next_free_slot) {
  
  auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(gateway,device->ieee_addr);
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY,-1);
  auto Supla_Z2S_OnePhaseElectricityMeter = new Supla::Sensor::Z2S_OnePhaseElectricityMeter(gateway, device);
  Z2S_fillDevicesTableSlot(device, next_free_slot, Supla_Z2S_OnePhaseElectricityMeter->getChannelNumber(), SUPLA_CHANNELTYPE_ELECTRICITY_METER, -1);

}

void msgZ2SDeviceElectricityMeter(uint8_t Supla_channel, uint8_t selector, uint64_t value) {

  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {
        auto Supla_OnePhaseElectricityMeter = reinterpret_cast<Supla::Sensor::Z2S_OnePhaseElectricityMeter *>(element);
        switch (selector) {
          case Z2S_EM_VOLTAGE_SEL: Supla_OnePhaseElectricityMeter->setVoltage(0, value * 100); break;
          case Z2S_EM_CURRENT_SEL: Supla_OnePhaseElectricityMeter->setCurrent(0, value * 1); break;
          case Z2S_EM_ACTIVE_POWER_SEL: Supla_OnePhaseElectricityMeter->setPowerActive(0, value * 100000); break;
          case Z2S_ACT_FWD_ENERGY_SEL: Supla_OnePhaseElectricityMeter->setFwdActEnergy(0, value * 1000); break;
        }
        
    }
}
