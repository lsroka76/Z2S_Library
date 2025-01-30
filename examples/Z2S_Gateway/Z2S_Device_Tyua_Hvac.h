#ifndef Z2S_DEVICE_TYUA_HVAC_H_
#define Z2S_DEVICE_TYUA_HVAC_H_

#include "Z2S_Devices_Table.h"
//#include <SuplaDevice.h>
//#include <supla/control/hvac_base.h>

#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_Tuya_thermostat.h>


void initZ2SDeviceTyuaHvac(ZigbeeGateway *gateway, zb_device_params_t *device, uint8_t Supla_channel) {
  
  auto Tuya_Hvac = new Supla::Control::Z2S_TuyaThermostat();
  Tuya_Hvac->setZigbeeDevice(gateway, device, TUYA_TRV_CS_001);

  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Tuya_Hvac);
  Tuya_Hvac->setHvac(Supla_Z2S_HvacBase);
            
  Supla_Z2S_HvacBase->setMainThermometerChannelNo(Tuya_Hvac->getChannel()->getChannelNumber());
  Supla_Z2S_HvacBase->getChannel()->setChannelNumber(Supla_channel);

  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_MODE_OFF);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_MODE_HEAT);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_WEEKLY_SCHEDULE_ENABLED);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_WEEKLY_SCHEDULE_DISABLED);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_STANDBY);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_HEATING);
  Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_CHANGE);

  Supla_Z2S_HvacBase->allowWrapAroundTemperatureSetpoints();

  Supla_Z2S_HvacBase->getChannel()->setDefault(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);

}

void addZ2SDeviceTyuaHvac(ZigbeeGateway * gateway, zb_device_params_t *device, uint8_t free_slot) {
  auto Tuya_Hvac = new Supla::Control::Z2S_TuyaThermostat();
  Tuya_Hvac->setZigbeeDevice(gateway, device, TUYA_TRV_CS_001);
        
  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Tuya_Hvac);
  Tuya_Hvac->setHvac(Supla_Z2S_HvacBase);

  Supla_Z2S_HvacBase->setMainThermometerChannelNo(Tuya_Hvac->getChannel()->getChannelNumber());
  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_HvacBase->getChannel()->getChannelNumber(), SUPLA_CHANNELTYPE_HVAC, -1); 
}

void msgZ2SDeviceTyuaHvac(uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                    uint16_t payload_size, uint8_t *payload) {
                                      
  auto element = Supla::Element::getElementByChannelNumber(Supla_channel);
  
  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HVAC) {
    if ((cluster == TUYA_PRIVATE_CLUSTER_EF00)&&(command_id == 0x02))
      {
        auto Supla_HvacBase = reinterpret_cast<Supla::Control::HvacBaseEE *>(element);

        switch (*(payload+2)) {
          case 0x67: {
            float setpoint = (*(payload+8))*256 + (*(payload+9));
            log_i("Tuya thermostat setpoint value %f", setpoint/10);
            Supla_HvacBase->setTemperatureSetpointHeat(((*(payload+8))*256 + (*(payload+9)))*10);
          } break;
          case 0x66: {
            float temperature = (*(payload+8))*256 + (*(payload+9));
            log_i("Tuya thermostat actual temperature value %f", temperature/10);
            auto Supla_VirtualThermometer = reinterpret_cast<Supla::Sensor::VirtualThermometer *>
                                            (Supla::Element::getElementByChannelNumber(Supla_HvacBase->getMainThermometerChannelNo()));
            Supla_VirtualThermometer->setValue(temperature/10);
          } break;
          case 0x65: {
            uint8_t thermostat_on_off = (*(payload+6));
            log_i("Tuya thermostat is %d", thermostat_on_off);
            if (thermostat_on_off == 1) Supla_HvacBase->handleAction(0,Supla::TURN_ON);
            else Supla_HvacBase->handleAction(0,Supla::TURN_OFF);
          } break;
          case 0x6C: {
            uint8_t thermostat_auto_manual = (*(payload+6));
            log_i("Tuya thermostat AUTO/MANUAL %d", thermostat_auto_manual);
          } break;
          case 0x28: {
            uint8_t thermostat_childlock = (*(payload+6));
            log_i("Tuya thermostat childlock is %d", thermostat_childlock);
          } break;
          default: log_i("Tuya thermostat command: 0x%x", (*(payload+2))); break;
          }
        } else
          if (cluster == TUYA_PRIVATE_CLUSTER_EF00)
            log_i("Tuya private cluster command 0x%x, dp 0x%x", command_id, (*(payload+2)));
    }
}

#endif