
#include "Z2S_Devices_Table.h"

extern ZigbeeGateway zbGateway;

z2s_device_params_t z2s_devices_table[Z2S_CHANNELMAXCOUNT];

uint32_t Z2S_getDevicesTableSize() {
  uint32_t _z2s_devices_table_size;
  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_DEVICES_TABLE_SIZE, &_z2s_devices_table_size))
    return _z2s_devices_table_size;
  else
    return 0;
}

uint8_t Z2S_findFirstFreeDevicesTableSlot() {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
      if (!z2s_devices_table[devices_counter].valid_record)
        return devices_counter;
  return 0xFF;
  
}

void Z2S_printDevicesTableSlots() {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record)
      log_i("valid %d, ieee addr %s, model_id 0x%x, endpoint 0x%x, cluster 0x%x, channel %d, channel type %d",
        z2s_devices_table[devices_counter].valid_record,
        z2s_devices_table[devices_counter].ieee_addr,
        z2s_devices_table[devices_counter].model_id,
        z2s_devices_table[devices_counter].endpoint,
        z2s_devices_table[devices_counter].cluster_id,
        z2s_devices_table[devices_counter].Supla_channel,
        z2s_devices_table[devices_counter].Supla_channel_type);  
}


int16_t Z2S_findChannelNumberSlot(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int32_t channel_type ) {

  log_i("Z2S_findChannelNumberSlot 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, channel type 0x%x", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, channel_type);
  
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record)
        if ((memcmp(z2s_devices_table[devices_counter].ieee_addr, ieee_addr,8) == 0) && 
        (z2s_devices_table[devices_counter].endpoint == endpoint) &&
        ((channel_type < 0) || (z2s_devices_table[devices_counter].Supla_channel_type == channel_type))) { 
        //&& (z2s_devices_table[devices_counter].cluster_id = cluster)) {
            return devices_counter;
        }

  }  
  return -1;
}

void Z2S_fillDevicesTableSlot(zb_device_params_t *device, uint8_t slot, uint8_t channel, int32_t channel_type) {

  z2s_devices_table[slot].valid_record = true;
  memcpy(z2s_devices_table[slot].ieee_addr,device->ieee_addr,8);
  z2s_devices_table[slot].model_id = device->model_id;
  z2s_devices_table[slot].endpoint = device->endpoint;
  z2s_devices_table[slot].cluster_id = device->cluster_id;
  z2s_devices_table[slot].Supla_channel = channel;
  z2s_devices_table[slot].Supla_channel_type = channel_type; 
  
  Z2S_saveDevicesTable();
  Z2S_printDevicesTableSlots();
}

bool Z2S_loadDevicesTable() {

  log_i("before get devices table");
  uint32_t z2s_devices_table_size =  Z2S_getDevicesTableSize(); //3584

  if (z2s_devices_table_size == 0) {

      log_i(" No devices table found, writing empty one with size %d", sizeof(z2s_devices_table));
      
      memset(z2s_devices_table,0,sizeof(z2s_devices_table));
      
      if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
        log_i ("Devices table write failed!");
        return false;
      }
      else { 
        if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_DEVICES_TABLE_SIZE, sizeof(z2s_devices_table))) {
          Supla::Storage::ConfigInstance()->commit();
          return true;
        }
        else { 
          log_i ("Devices table size write failed!");
          return false;
        }
      }
  } else
  {
    if (z2s_devices_table_size != sizeof(z2s_devices_table)) {
      
      log_i("Devices table size mismatch %d <> %d", z2s_devices_table_size, sizeof(z2s_devices_table));
      return false;
    }
    else {
        if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
          log_i ("Devices table load failed!");
          return false;
        } else {
          log_i ("Devices table load success!");
          return true;
        }
    }
  }
}

bool Z2S_saveDevicesTable() {

  if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
    log_i ("Devices table write failed!");
    return false;
  }
  else { 
    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_DEVICES_TABLE_SIZE, sizeof(z2s_devices_table)))
      return true;
    else { 
      log_i ("Devices table size write failed!");
      return false;
    }
  }
  Supla::Storage::ConfigInstance()->commit();
}

void Z2S_initSuplaChannels(){

  log_i ("initSuplaChannels starting");
  for (uint8_t devices_counter = 0; devices_counter < SUPLA_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record) 
        switch (z2s_devices_table[devices_counter].Supla_channel_type) {
          case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {
            zb_device_params_t device;
            device.endpoint = z2s_devices_table[devices_counter].endpoint;
            device.cluster_id = z2s_devices_table[devices_counter].cluster_id;
            memcpy(device.ieee_addr, z2s_devices_table[devices_counter].ieee_addr,8);
            /*log_i("init Channel device ieee 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", device.ieee_addr[7], device.ieee_addr[6], device.ieee_addr[5], 
            device.ieee_addr[4], device.ieee_addr[3], device.ieee_addr[2], device.ieee_addr[1], device.ieee_addr[0]);
            log_i("init Channel device table ieee 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", z2s_devices_table[devices_counter].ieee_addr[7], z2s_devices_table[devices_counter].ieee_addr[6], z2s_devices_table[devices_counter].ieee_addr[5], 
            z2s_devices_table[devices_counter].ieee_addr[4], z2s_devices_table[devices_counter].ieee_addr[3], z2s_devices_table[devices_counter].ieee_addr[2], z2s_devices_table[devices_counter].ieee_addr[1], z2s_devices_table[devices_counter].ieee_addr[0]);
            device.short_addr = esp_zb_address_short_by_ieee(device.ieee_addr);
            log_i("device short_addr 0x%x", device.short_addr);*/
            
            //log_i("auto Supla_VirtualThermHygroMeter = new Supla::Sensor::Z2S_VirtualThermHygroMeter(&zbGateway,&device);");
            //auto Supla_VirtualThermHygroMeter = new Supla::Sensor::Z2S_VirtualThermHygroMeter(&zbGateway,&device);
            auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();
            Supla_VirtualThermHygroMeter->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
            //Supla_VirtualThermHygroMeter->getChannel()->setBatteryPowered(true);
          } break;
          case SUPLA_CHANNELTYPE_BINARYSENSOR: {
            auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
            Supla_VirtualBinary->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
            //Supla_VirtualBinary->getChannel()->setBatteryPowered(true);
          } break;
          case SUPLA_CHANNELTYPE_RELAY: {
            auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(&zbGateway,z2s_devices_table[devices_counter].ieee_addr );
            Supla_Z2S_VirtualRelay->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
          } break;
          case SUPLA_CHANNELTYPE_ELECTRICITY_METER: {
            auto Supla_Z2S_OnePhaseElectricityMeter = new Supla::Sensor::OnePhaseElectricityMeter();
            Supla_Z2S_OnePhaseElectricityMeter->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
          } break;
          case SUPLA_CHANNELTYPE_HVAC: {
            //auto output = new Supla::Control::InternalPinOutput(7);
            //auto t1 = new Supla::Sensor::VirtualThermometer;
            //auto Supla_Z2S_HvacBase = new Supla::Control::HvacBase(output);
            //Supla_Z2S_HvacBase->setMainThermometerChannelNo(t1->getChannel()->getChannelNumber());
            //Supla_Z2S_HvacBase->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);
            zb_device_params_t device;
            device.endpoint = z2s_devices_table[devices_counter].endpoint;
            device.cluster_id = z2s_devices_table[devices_counter].cluster_id;
            memcpy(device.ieee_addr, z2s_devices_table[devices_counter].ieee_addr,8);
            device.short_addr = z2s_devices_table[devices_counter].short_addr; // esp_zb_address_short_by_ieee(device.ieee_addr);
            log_i("auto Tuya_Hvac = new Supla::Control::Z2S_TuyaThermostat() - BEFORE");
            auto Tuya_Hvac = new Supla::Control::Z2S_TuyaThermostat();
            log_i("auto Tuya_Hvac = new Supla::Control::Z2S_TuyaThermostat() - DONE");
            Tuya_Hvac->setZigbeeDevice(&zbGateway, &device);
            log_i("Tuya_Hvac->setZigbeeDevice(&zbGateway, &device) - DONE");
            auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Tuya_Hvac);
            log_i("auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Tuya_Hvac) - DONE");
            Tuya_Hvac->setHvac(Supla_Z2S_HvacBase);
            log_i("Tuya_Hvac->setHvac(Supla_Z2S_HvacBase) - DONE");
            Supla_Z2S_HvacBase->setMainThermometerChannelNo(Tuya_Hvac->getChannel()->getChannelNumber());
            Supla_Z2S_HvacBase->getChannel()->setChannelNumber(z2s_devices_table[devices_counter].Supla_channel);

            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_MODE_OFF);
            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_MODE_HEAT);
            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_WEEKLY_SCHEDULE_ENABLED);
            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_WEEKLY_SCHEDULE_DISABLED);
            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_STANDBY);
            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_HVAC_HEATING);
            Supla_Z2S_HvacBase->addAction(Supla::TURN_ON, Tuya_Hvac,Supla::ON_CHANGE);

            Supla_Z2S_HvacBase->allowWrapAroundTemperatureSetpoints();

            Supla_Z2S_HvacBase->getChannel()->setDefault(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);


          } break;
          default: {
            log_i("Can't create channel for %d channel type", z2s_devices_table[devices_counter].Supla_channel_type);
          } break;
        }
  }  
}

void Z2S_onTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float temperature) {

  log_i("onTemperatureReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {

        auto Supla_VirtualThermHygroMeter = reinterpret_cast<Supla::Sensor::VirtualThermHygroMeter *>(element);
        Supla_VirtualThermHygroMeter->setTemp(temperature);
    }
  }
}

void Z2S_onHumidityReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float humidity) {

  log_i("onHumidityReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {

        auto Supla_VirtualThermHygroMeter = reinterpret_cast<Supla::Sensor::VirtualThermHygroMeter *>(element);
        Supla_VirtualThermHygroMeter->setHumi(humidity);
    }
  }
}

void Z2S_onOnOffReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, bool state) {

  log_i("onOnOffReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RELAY);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {

        auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
        Supla_Z2S_VirtualRelay->Z2S_setOnOff(state); 
    }
  }
}

void Z2S_onRMSVoltageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t voltage) {

  log_i("onRMSVoltageReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {

        auto Supla_OnePhaseElectricityMeter = reinterpret_cast<Supla::Sensor::OnePhaseElectricityMeter *>(element);
        Supla_OnePhaseElectricityMeter->setVoltage(0, voltage * 100);
    }
  }
}

void Z2S_onRMSCurrentReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t current) {

  log_i("onRMSCurrentReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {

        auto Supla_OnePhaseElectricityMeter = reinterpret_cast<Supla::Sensor::OnePhaseElectricityMeter *>(element);
        Supla_OnePhaseElectricityMeter->setCurrent(0, current * 1000);
    }
  }
}

void Z2S_onRMSActivePowerReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t active_power) {

  log_i("onRMSVoltageReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {

        auto Supla_OnePhaseElectricityMeter = reinterpret_cast<Supla::Sensor::OnePhaseElectricityMeter *>(element);
        Supla_OnePhaseElectricityMeter->setPowerActive(0, active_power * 100000);
    }
  }
}

void Z2S_onBatteryPercentageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t battery_remaining) {

  log_i("onBatteryPercentageReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
   ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, -1);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->isBatteryPowered()) {

        element->getChannel()->setBatteryLevel(battery_remaining);
    }
  }
}

void Z2S_onIASzoneStatusChangeNotification(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int iaszone_status) {
  
int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

        auto Supla_VirtualBinary = reinterpret_cast<Supla::Sensor::VirtualBinary *>(element);
        if (iaszone_status & 1) Supla_VirtualBinary->clear();
        else Supla_VirtualBinary->set();
        if (iaszone_status & 2) log_i("Alarm2 on channel 0x%x", channel_number_slot);
        if (iaszone_status & 4) log_i("Tamper alarm on channel 0x%x", channel_number_slot);

    }
  }
}

void Z2S_onCmdCustomClusterReceive( esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t command_id,
                                    uint16_t payload_size, uint8_t *payload) {
  
int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HVAC);
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
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
}

void Z2S_onBTCBoundDevice(zb_device_params_t *device) {

  
  log_i("BTC bound device 0x%x on endpoint 0x%x cluster id 0x%x", device->short_addr, device->endpoint, device->cluster_id );
}


void Z2S_onBoundDevice(zb_device_params_t *device, bool last_cluster) {
  
}

void Z2S_addZ2SDevice(zb_device_params_t *device) {
  
  
  Z2S_printDevicesTableSlots();

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id, -1);
  
  if (channel_number_slot < 0) {
    log_i("No channel found for address %s, adding new one", device->ieee_addr);
    
    uint8_t first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
    
    if (first_free_slot == 0xFF) {
        log_i("Devices table full");
        return;
    }
    log_i("model id %d, first free slot %d", device->model_id, first_free_slot);
    
    switch (device->model_id) {
      case 0x0000: break;
      
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1: {
        //auto Supla_VirtualThermHygroMeter = new Supla::Sensor::Z2S_VirtualThermHygroMeter(&zbGateway,device);
        auto Supla_VirtualThermHygroMeter = new Supla::Sensor::VirtualThermHygroMeter();
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_VirtualThermHygroMeter->getChannelNumber(), SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);
      } break;
      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: {
        auto Supla_VirtualBinary = new Supla::Sensor::VirtualBinary();
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_VirtualBinary->getChannelNumber(), SUPLA_CHANNELTYPE_BINARYSENSOR); 
      } break;
      case Z2S_DEVICE_DESC_RELAY:
      case Z2S_DEVICE_DESC_RELAY_1: {
        auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(&zbGateway,device->ieee_addr);
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY); 
      } break;
      case Z2S_DEVICE_DESC_ON_OFF:
      case Z2S_DEVICE_DESC_ON_OFF_1: {
        auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(&zbGateway,device->ieee_addr);
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY); 
      } break;
      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER: {
        auto Supla_Z2S_VirtualRelay = new Supla::Control::Z2S_VirtualRelay(&zbGateway,device->ieee_addr);
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_RELAY); 
        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("Devices table full");
          return;
        }
        auto Supla_Z2S_OnePhaseElectricityMeter = new Supla::Sensor::OnePhaseElectricityMeter();
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_OnePhaseElectricityMeter->getChannelNumber(), 
                                SUPLA_CHANNELTYPE_ELECTRICITY_METER);

      } break;
      case Z2S_DEVICE_TUYA_HVAC: {
        auto Tuya_Hvac = new Supla::Control::Z2S_TuyaThermostat();
        Tuya_Hvac->setZigbeeDevice(&zbGateway, device);
        auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Tuya_Hvac);
        Tuya_Hvac->setHvac(Supla_Z2S_HvacBase);
        Supla_Z2S_HvacBase->setMainThermometerChannelNo(Tuya_Hvac->getChannel()->getChannelNumber());
        //Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_HvacBase->getChannelNumber(), SUPLA_CHANNELTYPE_HVAC); 
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_HvacBase->getChannel()->getChannelNumber(), SUPLA_CHANNELTYPE_HVAC);
      } break;
      default : {
        log_i("Device (0x%x), endpoint (0x%x), model (0x%x) unknown", device->short_addr, device->endpoint, device->model_id);
      } break;
    }
  } else
    log_i("Device (0x%x), endpoint (0x%x) already in z2s_devices_table (index 0x%x)", device->short_addr, device->endpoint, channel_number_slot);   
}
