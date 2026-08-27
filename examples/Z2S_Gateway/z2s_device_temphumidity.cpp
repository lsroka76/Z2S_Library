#include <NetworkClient.h>
#include <ESPmDNS.h>

#include "z2s_device_temphumidity.h"

#define REMOTE_ADDRESS_TYPE_LOCAL               0x00
#define REMOTE_ADDRESS_TYPE_IP4                 0x01
#define REMOTE_ADDRESS_TYPE_MDNS                0x02

//#define VALUE_TYPE_TEMPERATURE                  0x01
//#define VALUE_TYPE_HUMIDITY                     0x02

NetworkClient RemoteThermometer;
IPAddress ip_address;

/*****************************************************************************/

void initZ2SDeviceTempHumidity(
  int16_t channel_number_slot, bool thermhygrometer) {
    
    initZ2SDeviceTempHumidity(
      channel_number_slot, z2s_channels_table + channel_number_slot,
      thermhygrometer);
}

void initZ2SDeviceTempHumidity(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel, 
  bool thermhygrometer) {
  
  if (thermhygrometer) {

    Supla::Sensor::Z2S_VirtualThermHygroMeter 
      *Supla_Z2S_VirtualThermHygroMeter = nullptr;

    if (strcmp(Z2S_getZbDeviceModelName(
        _z2s_channel->Zb_device_id), "SNZB-02DR2") == 0) {

      auto Z2S_SNZB02DR2ThermHygroMeter = 
        new Supla::Sensor::Z2S_SNZB02DR2ThermHygroMeter;

      Supla_Z2S_VirtualThermHygroMeter = Z2S_SNZB02DR2ThermHygroMeter;

      Z2S_SNZB02DR2ThermHygroMeter->setResentSecs(300);
        //_z2s_channel->refresh_secs);
    }
    else 
      Supla_Z2S_VirtualThermHygroMeter = 
        new Supla::Sensor::Z2S_VirtualThermHygroMeter;
    
    Supla_Z2S_VirtualThermHygroMeter->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

    if (strlen(
          _z2s_channel->Supla_channel_name) > 0) 
      Supla_Z2S_VirtualThermHygroMeter->setInitialCaption(
        _z2s_channel->Supla_channel_name);
  
    if (_z2s_channel->Supla_channel_func != 0) 
      Supla_Z2S_VirtualThermHygroMeter->setDefaultFunction(
        _z2s_channel->Supla_channel_func);

    if (_z2s_channel->user_data_flags & 
          USER_DATA_FLAG_CORRECTIONS_DISABLED)
      Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(false);
    else
      Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(true);

    if (_z2s_channel->user_data_flags & 
          USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_Z2S_VirtualThermHygroMeter->getChannel()->
        setStateOfflineRemoteWakeupNotSupported();

      Supla_Z2S_VirtualThermHygroMeter->setRWNSFlag(true);
    }

    Supla_Z2S_VirtualThermHygroMeter->setTimeoutSecs(
      _z2s_channel->timeout_secs);

    Supla_Z2S_VirtualThermHygroMeter->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

    Supla_Z2S_VirtualThermHygroMeter->setZ2SChannel(
      channel_index, _z2s_channel);
  } else {

    auto Supla_Z2S_VirtualThermometer = 
      new Supla::Sensor::Z2S_VirtualThermometer();

    Supla_Z2S_VirtualThermometer->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

    if (strlen(
          _z2s_channel->Supla_channel_name) > 0) 
      Supla_Z2S_VirtualThermometer->setInitialCaption(
        _z2s_channel->Supla_channel_name);
  
    if (_z2s_channel->Supla_channel_func != 0) 
      Supla_Z2S_VirtualThermometer->setDefaultFunction(
        _z2s_channel->Supla_channel_func);

    if (_z2s_channel->user_data_flags & 
          USER_DATA_FLAG_CORRECTIONS_DISABLED)
      Supla_Z2S_VirtualThermometer->setApplyCorrections(false);
    else
      Supla_Z2S_VirtualThermometer->setApplyCorrections(true);

    if (_z2s_channel->user_data_flags & 
          USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_Z2S_VirtualThermometer->getChannel()->
        setStateOfflineRemoteWakeupNotSupported();

      Supla_Z2S_VirtualThermometer->setRWNSFlag(true);
    }

    Supla_Z2S_VirtualThermometer->setTimeoutSecs(
      _z2s_channel->timeout_secs);

    Supla_Z2S_VirtualThermometer->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

    Supla_Z2S_VirtualThermometer->setZ2SChannel(channel_index, _z2s_channel);

    log_i(
      "device model: %lu, channel model: %lu", 
      Supla_Z2S_VirtualThermometer->getZbDeviceModelId(),
      Supla_Z2S_VirtualThermometer->getChannelModelId());
  }
  RemoteThermometer.setTimeout(1000);
}

void addZ2SDeviceTempHumidity(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t func, bool thermhygrometer) {

  if (thermhygrometer) {

    auto Supla_Z2S_VirtualThermHygroMeter = 
      new Supla::Sensor::Z2S_VirtualThermHygroMeter();
  
    if (name == nullptr)
      name = (char*)default_temphumi_name;
  
    Z2S_fillChannelsTableSlot(
      device, free_slot, Supla_Z2S_VirtualThermHygroMeter->getChannelNumber(), 
      SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, sub_id, name, func);

    Supla_Z2S_VirtualThermHygroMeter->setZ2SChannel(
      free_slot, z2s_channels_table + free_slot);
    
  } else {

    auto Z2S_VirtualThermometer = new Supla::Sensor::Z2S_VirtualThermometer();
  
    if (name == nullptr)
      name = (char*)default_temp_name;
  
    Z2S_fillChannelsTableSlot(
      device, free_slot, Z2S_VirtualThermometer->getChannelNumber(), 
      SUPLA_CHANNELTYPE_THERMOMETER, sub_id, name, func);

    Z2S_VirtualThermometer->setZ2SChannel(
      free_slot, z2s_channels_table + free_slot);
  }
}

/*****************************************************************************/

Supla::Sensor::Z2S_VirtualThermHygroMeter* getZ2SDeviceTempHumidityPtr(
  uint8_t Supla_channel) {

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

/*****************************************************************************/

void resendTHValue(
  Z2S_Core *z2s_core, uint8_t value_type, int32_t value) {


  uint8_t remote_Supla_channel =z2s_core->getSuplaRemoteChannel();
    
  uint8_t remote_address_type = z2s_core->checkChannelUserDataFlags(
    USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ? REMOTE_ADDRESS_TYPE_MDNS : 
    REMOTE_ADDRESS_TYPE_IP4;

  log_i(
    "Resending T/H value: type = %u, address flag = %s, channel = %u,"
    "value = %i", value_type, (remote_address_type == 
    REMOTE_ADDRESS_TYPE_MDNS) ? "MDNS" : "IP4", remote_Supla_channel, value);    
  
  switch(remote_address_type) {


    case REMOTE_ADDRESS_TYPE_IP4: {

      ip_address = z2s_core->getRemoteIPAddress();
      
      if (ip_address == IPAddress(0, 0, 0, 0)) {

        uint8_t Supla_channel = z2s_core->getZ2SChannelNumber();

        log_i(
          "remote_Supla_channel %u, ip_address %u, Supla_channel %u, "
          "value_type %u, value %u", remote_Supla_channel, ip_address, 
          Supla_channel, value_type, value);

        updateRemoteThermometer(
          remote_Supla_channel, ip_address, Supla_channel, value_type, value);
        return;
      }
    } break;


    case REMOTE_ADDRESS_TYPE_MDNS: {

      ip_address = MDNS.queryHost(z2s_core->getMDNSName());

      z2s_core->setRemoteIPAddress(ip_address);
    } break;
  }

  if (RemoteThermometer.connect(ip_address, 1234, 500)) {

    uint8_t cmd_id = (value_type == RTH_VALUE_TYPE_TEMPERATURE) ? 0x10 : 0x11;

    RemoteThermometer.printf(
      "Z2SCMD%02u%03u%03u%08ld\n", cmd_id, remote_Supla_channel,
      z2s_core->getZ2SChannelNumber(), value);
    
    String response = RemoteThermometer.readStringUntil('\n');
      
    if (response == "OK") 
      log_i("T/H value forwarded");
        
    RemoteThermometer.stop();
  } else
    log_e(
      "T/H value forwarding FAILED - no connection to remote thremometer"); 
}

/*****************************************************************************/

void msgZ2SDeviceTempHumidityTemp(Supla::Element* element, double temp) {

  switch (element->getChannel()->getChannelType()) {
    

    case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {

      auto Supla_Z2S_VirtualThermHygroMeter = 
        static_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);

      Supla_Z2S_VirtualThermHygroMeter->setZbDeviceLastSeenMs(millis());    
      Supla_Z2S_VirtualThermHygroMeter->setTemperature(temp);

      if (Supla_Z2S_VirtualThermHygroMeter->getChannelUserDataFlags() &
			    USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE) {

        //this allows Supla to apply any channel corrections
        temp = Supla_Z2S_VirtualThermHygroMeter->getChannel()->
          getValueDoubleFirst();

        resendTHValue(
          Supla_Z2S_VirtualThermHygroMeter->getZ2SCorePtr(),
          RTH_VALUE_TYPE_TEMPERATURE, temp * 100);
      }
    } break;


    case SUPLA_CHANNELTYPE_THERMOMETER: {

      auto Supla_Z2S_VirtualThermometer = static_cast<
        Supla::Sensor::Z2S_VirtualThermometer *>(element);
    
      Supla_Z2S_VirtualThermometer->setZbDeviceLastSeenMs(millis());
      Supla_Z2S_VirtualThermometer->setTemperature(temp);

      if (Supla_Z2S_VirtualThermometer->getChannelUserDataFlags() &
			    USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE) {

        //this allows Supla to apply any channel corrections
        temp = Supla_Z2S_VirtualThermometer->getChannel()->getValueDouble();

        resendTHValue(
          Supla_Z2S_VirtualThermometer->getZ2SCorePtr(), 
          RTH_VALUE_TYPE_TEMPERATURE, temp * 100);
      }
    } break;
  }
}

/*****************************************************************************/

void msgZ2SDeviceTempHumidityHumi(Supla::Element* element, double humi) {

  auto Supla_Z2S_VirtualThermHygroMeter = static_cast<
    Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
    
  
  if (Supla_Z2S_VirtualThermHygroMeter) {

    Supla_Z2S_VirtualThermHygroMeter->setZbDeviceLastSeenMs(millis());

    switch (Supla_Z2S_VirtualThermHygroMeter->getChannelModelId()) {
      
      
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10: 
      
        humi *= 10; 
      break;
      
      
      default: break;
    }
    
    Supla_Z2S_VirtualThermHygroMeter->setHumi(humi);

    if (Supla_Z2S_VirtualThermHygroMeter->getChannelUserDataFlags() &
			  USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE) {

      //this allows Supla to apply any channel corrections
      humi = Supla_Z2S_VirtualThermHygroMeter->getChannel()->
        getValueDoubleSecond();

      resendTHValue(
        Supla_Z2S_VirtualThermHygroMeter->getZ2SCorePtr(),
        RTH_VALUE_TYPE_HUMIDITY, humi * 100);
    }
  }
}
