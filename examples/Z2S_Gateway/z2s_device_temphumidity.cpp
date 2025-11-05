#include <NetworkClient.h>
#include <ESPmDNS.h>

#include "z2s_device_temphumidity.h"

#define REMOTE_ADDRESS_TYPE_LOCAL               0x00
#define REMOTE_ADDRESS_TYPE_IP4                 0x01
#define REMOTE_ADDRESS_TYPE_MDNS                0x02


NetworkClient RemoteThermometer;
IPAddress ip_address;
/*---------------------------------------------------------------------------------------------------------------------------*/

void initZ2SDeviceTempHumidity(int16_t channel_number_slot, 
                               bool thermhygrometer) {
  
  if (thermhygrometer) {

    auto Supla_Z2S_VirtualThermHygroMeter = 
      new Supla::Sensor::Z2S_VirtualThermHygroMeter();

    Supla_Z2S_VirtualThermHygroMeter->getChannel()->setChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

    if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
      Supla_Z2S_VirtualThermHygroMeter->setInitialCaption(
        z2s_channels_table[channel_number_slot].Supla_channel_name);
  
    if (z2s_channels_table[channel_number_slot].Supla_channel_func != 0) 
      Supla_Z2S_VirtualThermHygroMeter->setDefaultFunction(
        z2s_channels_table[channel_number_slot].Supla_channel_func);

    if (z2s_channels_table[channel_number_slot].user_data_flags & 
          USER_DATA_FLAG_CORRECTIONS_DISABLED)
      Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(false);
    else
      Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(true);

    if (z2s_channels_table[channel_number_slot].user_data_flags & 
          USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_Z2S_VirtualThermHygroMeter->getChannel()->
        setStateOfflineRemoteWakeupNotSupported();

      Supla_Z2S_VirtualThermHygroMeter->setRWNSFlag(true);
    }

    Supla_Z2S_VirtualThermHygroMeter->setTimeoutSecs(
      z2s_channels_table[channel_number_slot].timeout_secs);

  } else {

    auto Supla_Z2S_VirtualThermometer = 
      new Supla::Sensor::Z2S_VirtualThermometer();

    Supla_Z2S_VirtualThermometer->getChannel()->setChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

    if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
      Supla_Z2S_VirtualThermometer->setInitialCaption(
        z2s_channels_table[channel_number_slot].Supla_channel_name);
  
    if (z2s_channels_table[channel_number_slot].Supla_channel_func != 0) 
      Supla_Z2S_VirtualThermometer->setDefaultFunction(
        z2s_channels_table[channel_number_slot].Supla_channel_func);

    if (z2s_channels_table[channel_number_slot].user_data_flags & 
          USER_DATA_FLAG_CORRECTIONS_DISABLED)
      Supla_Z2S_VirtualThermometer->setApplyCorrections(false);
    else
      Supla_Z2S_VirtualThermometer->setApplyCorrections(true);

    if (z2s_channels_table[channel_number_slot].user_data_flags & 
          USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_Z2S_VirtualThermometer->getChannel()->
        setStateOfflineRemoteWakeupNotSupported();

      Supla_Z2S_VirtualThermometer->setRWNSFlag(true);
    }

    Supla_Z2S_VirtualThermometer->setTimeoutSecs(
      z2s_channels_table[channel_number_slot].timeout_secs);
  }
}

void addZ2SDeviceTempHumidity(zbg_device_params_t *device, 
                              uint8_t free_slot, 
                              int8_t sub_id, 
                              const char *name, 
                              uint32_t func,
                              bool thermhygrometer) {

  if (thermhygrometer) {

    auto Supla_Z2S_VirtualThermHygroMeter = 
      new Supla::Sensor::Z2S_VirtualThermHygroMeter();
  
    if (name == nullptr)
      name = (char*)default_temphumi_name;
  
    Z2S_fillChannelsTableSlot(device, 
                              free_slot, 
                              Supla_Z2S_VirtualThermHygroMeter->getChannelNumber(), 
                              SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, 
                              sub_id, 
                              name, 
                              func);
  } else {

    auto Z2S_VirtualThermometer = 
      new Supla::Sensor::Z2S_VirtualThermometer();
  
    if (name == nullptr)
      name = (char*)default_temp_name;
  
    Z2S_fillChannelsTableSlot(device, 
                              free_slot, 
                              Z2S_VirtualThermometer->getChannelNumber(), 
                              SUPLA_CHANNELTYPE_THERMOMETER, 
                              sub_id, 
                              name, 
                              func);
  }
}

/*---------------------------------------------------------------------------------------------------------------------------*/

Supla::Sensor::Z2S_VirtualThermHygroMeter* getZ2SDeviceTempHumidityPtr(uint8_t Supla_channel) {

  auto element = 
    Supla::Element::getElementByChannelNumber(Supla_channel);

  if (element && 
      (element->getChannel()->getChannelType() == 
        SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR))  

    return 
      reinterpret_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);

  else return 
    nullptr;  
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceTempHumidityTemp(int16_t channel_number_slot, 
                                  double temp) {

  if (channel_number_slot < 0) {
    
    log_e("msgZ2SDeviceTempHumidityTemp - invalid channel number slot");
    return;
  }

  Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto element = 
    Supla::Element::getElementByChannelNumber(
      z2s_channels_table[channel_number_slot].Supla_channel);

  if (element == nullptr)
    return;
  
  switch (element->getChannel()->getChannelType()) {
    

    case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {

      auto Supla_Z2S_VirtualThermHygroMeter = 
        reinterpret_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
    
      Supla_Z2S_VirtualThermHygroMeter->setTemperature(temp);
      //Supla_Z2S_VirtualThermHygroMeter->Refresh();
    } break;


    case SUPLA_CHANNELTYPE_THERMOMETER: {

      auto Supla_Z2S_VirtualThermometer = 
        reinterpret_cast<Supla::Sensor::Z2S_VirtualThermometer *>(element);
    
      //Supla_Z2S_VirtualThermometer->setValue(temp);
      //Supla_Z2S_VirtualThermometer->Refresh();
      Supla_Z2S_VirtualThermometer->setTemperature(temp);
    } break;
  }

  if (z2s_channels_table[channel_number_slot].user_data_flags &
			USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE) {

    uint8_t remote_Supla_channel =
      z2s_channels_table[channel_number_slot].Supla_remote_channel;
    
    uint8_t remote_address_type = 
      Z2S_checkChannelFlags(channel_number_slot, 
                            USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ?
      REMOTE_ADDRESS_TYPE_MDNS : REMOTE_ADDRESS_TYPE_IP4;

    log_i("Resending temperature, address flag = %s, channel = %u",
          (remote_address_type == REMOTE_ADDRESS_TYPE_MDNS) ?
          "MDNS" : "IP4", remote_Supla_channel);

    switch(remote_address_type) {


      case REMOTE_ADDRESS_TYPE_IP4: {

        ip_address = 
          z2s_channels_table[channel_number_slot].\
            remote_channel_data.remote_ip_address;
      }
      break;


      case REMOTE_ADDRESS_TYPE_MDNS: {

        ip_address = MDNS.queryHost(
          z2s_channels_table[channel_number_slot].\
          remote_channel_data.mDNS_name);

        z2s_channels_table[channel_number_slot].\
            remote_channel_data.remote_ip_address = ip_address;
      } break;
    }

    if (z2s_channels_table[channel_number_slot].
        remote_channel_data.remote_ip_address == 0) {

        updateRemoteThermometer(
          remote_Supla_channel,
          ip_address,
          z2s_channels_table[channel_number_slot].Supla_channel,
          (int32_t)(temp*100));

        return;
      }

    if (RemoteThermometer.connect(ip_address, 1234)) {

      RemoteThermometer.printf("Z2SCMD%02u%03u%03u%08ld\n", 
                            0x10, 
                            remote_Supla_channel,
                            z2s_channels_table[channel_number_slot].Supla_channel,
                            (int32_t)(temp*100));
    
    
      String response = RemoteThermometer.readStringUntil('\n');
      
      if (response == "OK") 
        log_i("Temperature forwarded");
        
      RemoteThermometer.stop();
    } else
      log_e("Temperature forwarding FAILED - no connection to remote thremometer"); 
  }
}

/*---------------------------------------------------------------------------------------------------------------------------*/

void msgZ2SDeviceTempHumidityHumi(int16_t channel_number_slot, 
                                  double humi) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceTempHumidityHumi - invalid channel number slot");
    return;
  }

  Z2S_updateZbDeviceLastSeenMs(
    z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto Supla_Z2S_VirtualThermHygroMeter = 
    getZ2SDeviceTempHumidityPtr(
      z2s_channels_table[channel_number_slot].Supla_channel);
  
  if (Supla_Z2S_VirtualThermHygroMeter) {
    
    switch (z2s_channels_table[channel_number_slot].model_id) {
      
      
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10: 
      
        humi *= 10; 
      break;
      
      
      default: break;
    }
    
    Supla_Z2S_VirtualThermHygroMeter->setHumi(humi);
    Supla_Z2S_VirtualThermHygroMeter->Refresh();
  }
}
