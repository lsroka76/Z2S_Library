#include <NetworkClient.h>
#include <ESPmDNS.h>

#include "z2s_device_temphumidity.h"

/*****************************************************************************/

void initZ2SDeviceTempHumidity(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel, 
  bool thermhygrometer, Supla::Element *element) {
  
  if (thermhygrometer) {

    bool isSNZB02DR2 = Z2S_isZbDeviceModelName(
      _z2s_channel->Zb_device_id, "SNZB-02DR2");

    Supla::Sensor::Z2S_VirtualThermHygroMeter 
      *Supla_Z2S_VirtualThermHygroMeter = nullptr;

    if (element) {

      Supla_Z2S_VirtualThermHygroMeter = isSNZB02DR2 ?
        static_cast<Supla::Sensor::Z2S_SNZB02DR2ThermHygroMeter *>(element) :
        static_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
    }
    else {

      Supla_Z2S_VirtualThermHygroMeter = isSNZB02DR2 ?
        new Supla::Sensor::Z2S_SNZB02DR2ThermHygroMeter :
        new Supla::Sensor::Z2S_VirtualThermHygroMeter;

      Supla_Z2S_VirtualThermHygroMeter->getChannel()->setChannelNumber(
      _z2s_channel->Supla_channel);

      Supla_Z2S_VirtualThermHygroMeter->setZ2SChannel(
        channel_index, _z2s_channel);      
    }
    
    Supla_Z2S_VirtualThermHygroMeter->setInitialCaption(
      _z2s_channel->Supla_channel_name);
   
    Supla_Z2S_VirtualThermHygroMeter->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

    if (_z2s_channel->user_data_flags & USER_DATA_FLAG_CORRECTIONS_DISABLED)
      Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(false);
    else
      Supla_Z2S_VirtualThermHygroMeter->setApplyCorrections(true);

    if (_z2s_channel->user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_Z2S_VirtualThermHygroMeter->getChannel()->
        setStateOfflineRemoteWakeupNotSupported();

      Supla_Z2S_VirtualThermHygroMeter->setRWNSFlag(true);
    }
  } 
  else {

    Supla::Sensor::Z2S_VirtualThermometer *Supla_Z2S_VirtualThermometer = 
      nullptr;

    if (element) {

      Supla_Z2S_VirtualThermometer = static_cast<
        Supla::Sensor::Z2S_VirtualThermometer *>(element);
    }
    else {

      Supla_Z2S_VirtualThermometer = new Supla::Sensor::Z2S_VirtualThermometer;

      Supla_Z2S_VirtualThermometer->getChannel()->setChannelNumber(
        _z2s_channel->Supla_channel);

      Supla_Z2S_VirtualThermometer->setZ2SChannel(channel_index, _z2s_channel);
    }

    if (_z2s_channel->user_data_flags & USER_DATA_FLAG_CORRECTIONS_DISABLED)
      Supla_Z2S_VirtualThermometer->setApplyCorrections(false);
    else
      Supla_Z2S_VirtualThermometer->setApplyCorrections(true);

    if (_z2s_channel->user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
      
      Supla_Z2S_VirtualThermometer->getChannel()->
        setStateOfflineRemoteWakeupNotSupported();

      Supla_Z2S_VirtualThermometer->setRWNSFlag(true);
    }

    log_i(
      "device model: %lu, channel model: %lu", 
      Supla_Z2S_VirtualThermometer->getZbDeviceModelId(),
      Supla_Z2S_VirtualThermometer->getChannelModelId());
  }
  //RemoteThermometer.setTimeout(1000);
}

/*****************************************************************************/

void addZ2SDeviceTempHumidity(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t func, bool thermhygrometer) {

  if (thermhygrometer) {

    bool isSNZB02DR2 = Z2S_isZbDeviceModelName(
      device->zb_device_id, "SNZB-02DR2");

    SuplaDevice.saveStateToStorage();
    Supla::Storage::ConfigInstance()->commit();


    auto Supla_Z2S_VirtualThermHygroMeter = isSNZB02DR2 ?
        new Supla::Sensor::Z2S_SNZB02DR2ThermHygroMeter :
        new Supla::Sensor::Z2S_VirtualThermHygroMeter;
  
    if (name == nullptr)
      name = (char*)default_temphumi_name;

    if (func == 0)
      func = SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE;
  
    Z2S_setChannelData(
      Supla_Z2S_VirtualThermHygroMeter->getZ2SCorePtr(), device, free_slot,
      Supla_Z2S_VirtualThermHygroMeter->getChannelNumber(), 
      SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, sub_id, name, func);

    Supla_Z2S_VirtualThermHygroMeter->setInitialCaption(name);
    Supla_Z2S_VirtualThermHygroMeter->setDefaultFunction(func);

    initZ2SDeviceTempHumidity(
      free_slot, Supla_Z2S_VirtualThermHygroMeter->getZ2SChannel(),
      thermhygrometer, Supla_Z2S_VirtualThermHygroMeter->getZ2SElementPtr());

    Supla_Z2S_VirtualThermHygroMeter->getChannel()->setSubDeviceId(
    device->zb_device_id + 1);
    Supla_Z2S_VirtualThermHygroMeter->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(
      Supla_Z2S_VirtualThermHygroMeter->getZ2SCorePtr());

    Supla_Z2S_VirtualThermHygroMeter->onLoadConfig(&SuplaDevice);
    Supla_Z2S_VirtualThermHygroMeter->onInit();
      
  } else {

    SuplaDevice.saveStateToStorage();
    Supla::Storage::ConfigInstance()->commit();

    auto Z2S_VirtualThermometer = new Supla::Sensor::Z2S_VirtualThermometer();
  
    if (name == nullptr)
      name = (char*)default_temp_name;

    if (func == 0)
      func = SUPLA_CHANNELFNC_THERMOMETER;
  
    Z2S_setChannelData(
      Z2S_VirtualThermometer->getZ2SCorePtr(), device, free_slot,
      Z2S_VirtualThermometer->getChannelNumber(), 
      SUPLA_CHANNELTYPE_THERMOMETER, sub_id, name, func);

    Z2S_VirtualThermometer->setInitialCaption(name);
    Z2S_VirtualThermometer->setDefaultFunction(func);

    initZ2SDeviceTempHumidity(
      free_slot, Z2S_VirtualThermometer->getZ2SChannel(), thermhygrometer,
      Z2S_VirtualThermometer->getZ2SElementPtr());

    Z2S_VirtualThermometer->getChannel()->setSubDeviceId(
      device->zb_device_id + 1);
    Z2S_VirtualThermometer->getChannel()->setFlag(
      SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);

    addChannelsSelectorChannel(Z2S_VirtualThermometer->getZ2SCorePtr());

    Z2S_VirtualThermometer->onLoadConfig(&SuplaDevice);
    Z2S_VirtualThermometer->onInit();
  }
}

/*****************************************************************************/

void msgZ2SDeviceTempHumidityTemp(Supla::Element* element, double temp) {

  switch (element->getChannel()->getChannelType()) {
    

    case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR: {

      auto Supla_Z2S_VirtualThermHygroMeter = 
        static_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);

      Supla_Z2S_VirtualThermHygroMeter->setZbDeviceLastSeenMs(millis());    
      Supla_Z2S_VirtualThermHygroMeter->setTemperature(temp);
    } break;


    case SUPLA_CHANNELTYPE_THERMOMETER: {

      auto Supla_Z2S_VirtualThermometer = static_cast<
        Supla::Sensor::Z2S_VirtualThermometer *>(element);
    
      Supla_Z2S_VirtualThermometer->setZbDeviceLastSeenMs(millis());
      Supla_Z2S_VirtualThermometer->setTemperature(temp);
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
    
    Supla_Z2S_VirtualThermHygroMeter->setHumidity(humi);
  }
}
