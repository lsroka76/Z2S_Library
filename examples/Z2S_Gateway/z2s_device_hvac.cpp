#include "z2s_device_hvac.h"

void initZ2SDeviceHvac(ZigbeeGateway *gateway, zbg_device_params_t *device, int16_t channel_number_slot) {
  
  uint8_t trv_mode;

  switch (z2s_devices_table[channel_number_slot].model_id) {
    case Z2S_DEVICE_DESC_TUYA_HVAC_6567C: trv_mode = 1; break;

    case Z2S_DEVICE_DESC_TUYA_HVAC_23457: trv_mode = 2; break;

    case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY: trv_mode = 3; break;
  }
  
  auto Supla_Z2S_TRVOutputInterface = new Supla::Control::Z2S_TRVOutputInterface(gateway, device, trv_mode);

  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE(Supla_Z2S_TRVOutputInterface);
            
  Supla_Z2S_HvacBase->getChannel()->setChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_devices_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_HvacBase->setInitialCaption(z2s_devices_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_devices_table[channel_number_slot].Supla_channel_func !=0) 
    Supla_Z2S_HvacBase->setDefaultFunction(z2s_devices_table[channel_number_slot].Supla_channel_func);

  if (z2s_devices_table[channel_number_slot].Supla_secondary_channel < 0xFF) 
    Supla_Z2S_HvacBase->setMainThermometerChannelNo(z2s_devices_table[channel_number_slot].Supla_secondary_channel);

  Supla_Z2S_HvacBase->allowWrapAroundTemperatureSetpoints();

  //Supla_Z2S_HvacBase->getChannel()->setDefault(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);
}

void addZ2SDeviceHvac(ZigbeeGateway * gateway, zbg_device_params_t *device, uint8_t free_slot, uint8_t trv_thermometer_slot) {
  
  auto Supla_Z2S_HvacBase = new Supla::Control::HvacBaseEE();

  Z2S_fillDevicesTableSlot(device, free_slot, Supla_Z2S_HvacBase->getChannel()->getChannelNumber(), SUPLA_CHANNELTYPE_HVAC, -1, 
                           "THERMOSTAT", SUPLA_CHANNELFNC_HVAC_THERMOSTAT, z2s_devices_table[trv_thermometer_slot].Supla_secondary_channel); 


}

void msgZ2SDeviceHvac(uint8_t Supla_channel, uint16_t cluster, uint8_t command_id,
                                    uint16_t payload_size, uint8_t *payload, signed char rssi) {
                                      

}
