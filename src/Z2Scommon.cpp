/*****************************************************************************/

#include <cstring>
#include <Arduino.h>
#include <Preferences.h>
#include <ZigbeeGateway.h>
#include <SuplaDevice.h>
#include <supla/element.h>

#include "Z2S_common.h"
#include "z2s_little_fs.h"
#include "z2s_devices_database.h"

#include <Z2S_control/Z2S_remote_relay.h>
#include <Z2S_sensor/Z2S_remote_thermometer.h>
#include <Z2S_control/Z2S_local_action_handlers.h>
#include <Z2S_control/hvac_base_ee.h>
#include <Z2S_control/Z2S_trv_interface.h>
#include <Z2S_sensor/Z2S_virtual_therm_hygro_meter.h>
#include <Z2S_sensor/Z2S_virtual_thermometer.h>


/*****************************************************************************/

extern Preferences Z2S_GatewayPreferences;

/*****************************************************************************/

NetworkClient RemoteThermometer;
IPAddress ip_address;

/*****************************************************************************/

size_t mbstrnlen(const char *mb_str, size_t max_bytes) {

	if (mb_str == nullptr)
		return 0;

	size_t mb_counter = 0;
	size_t str_counter = 0;
	while (*(mb_str + str_counter) != '\0') {

		uint8_t next_char = *(mb_str + str_counter);
		if (next_char <= 0x7F)
			str_counter++;
		else 
		if ((next_char >= 0xC0) && (next_char <= 0xDF))
			str_counter += 2;
		else 
		if ((next_char >= 0xE0) && (next_char <= 0xEF))
			str_counter += 3;
		else 
		if ((next_char >= 0xF0) && (next_char <= 0xF7)) 
			str_counter += 4;
	else 
		if ((next_char >= 0xF8) && (next_char <= 0xFB))
			str_counter += 5;
	else 
		if ((next_char >= 0xFC) && (next_char <= 0xFD)) 
			str_counter += 6;
	else 
		if ((next_char >= 0xFE) && (next_char <= 0xFF)) 
			return 0;

	if (str_counter > max_bytes)
		return mb_counter; 
	else
		mb_counter = str_counter;
	}
	return mb_counter;
}

/*****************************************************************************/
/*****************************************************************************/

bool Z2S_Core::fillZ2SChannelStruct(z2s_device_params_t& z2s_channel) {

  memcpy(&z2s_channel, &_z2s_channel, sizeof(z2s_device_params_t));
  return true;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByChannelIndex(int16_t channel_index) {

  if (channel_index < 0)
    return nullptr;

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Supla::Element *Z2S_Core::getSuplaElementByChannelNumberAndType(
    uint8_t channel_number, int32_t Supla_channel_type){

  Supla::Element *element = Supla::Element::getElementByChannelNumber(
    channel_number);

  if (element && 
      (element->getChannel()->getChannelType() == Supla_channel_type))
    return element;
  else
    return nullptr;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByChannelNumber(uint8_t channel_number) {

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByZbDeviceId(uint8_t Zb_device_id) {

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Zb_device_id == Zb_device_id)
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Z2S_Core *Z2S_Core::getZ2SCoreByZbDeviceIdAndChannelType(
  uint8_t Zb_device_id, int32_t Supla_channel_type) {

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if ((z2s_core->_z2s_channel.Zb_device_id == Zb_device_id) &&
        (z2s_core->_z2s_channel.Supla_channel_type == Supla_channel_type))
      return z2s_core;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/


Supla::Element *Z2S_Core::getZ2SElementByChannelIndex(int16_t channel_index) {

  if (channel_index < 0)
    return nullptr;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core->_z2s_element;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Supla::Element *Z2S_Core::getZ2SElementByChannelNumber(
  uint8_t channel_number) {

  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER) 
    return &GatewayEventsInstance;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core->_z2s_element;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Supla::Element *Z2S_Core::getSuplaElementByChannelNumber(
  uint8_t channel_number) {

  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER) 
    return &GatewayEventsInstance;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number) {

      if (z2s_core->_z2s_channel.Supla_channel_type == SUPLA_CHANNELTYPE_HVAC)
        return z2s_core->getHvacPtr(); 
      else  
      return z2s_core->_z2s_element;
    }

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

Supla::Element *Z2S_Core::getZ2SElementByZbDeviceId(uint8_t Zb_device_id) {

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Zb_device_id == Zb_device_id)
      return z2s_core->_z2s_element;

    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

uint8_t Z2S_Core::updateZ2SChannelsByZbDeviceId(
  uint8_t Zb_device_id, uint8_t rssi_percentage, uint8_t battery_level) {

  uint8_t channels_number = 0;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Zb_device_id == Zb_device_id) {     

      Supla::Channel *channel = z2s_core->_z2s_element->getChannel();

      if (channel) {

        channel->setStateOnline();
        channel->setBridgeSignalStrength(rssi_percentage);
        if ((battery_level < 0xFF) &&
          ((z2s_core->_z2s_channel.user_data_flags &
            USER_DATA_FLAG_IGNORE_CHANNEL_BATTERY_LEVEL) == 0)) 

          channel->setBatteryLevel(battery_level);
      }
      else
        {

        }
      channels_number++;
    }
    core_it++;
  }
  return channels_number;
}

/*****************************************************************************/

uint8_t Z2S_Core::countZ2SChannelsByZbDeviceId(uint8_t Zb_device_id) {

  uint8_t channels_number = 0;

  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    if ((*core_it)->_z2s_channel.Zb_device_id == Zb_device_id) {
      
      channels_number++;
    }
    core_it++;
  }
  return channels_number;
}

/*****************************************************************************/

int16_t Z2S_Core::getZ2SChannelIndexByChannelNumber(uint8_t channel_number) {

  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER)
    return GATEWAY_EVENTS_CHANNEL_INDEX;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core->_channel_index;

    core_it++;
  }
  return -1;
}

/*****************************************************************************/

uint8_t Z2S_Core::getZ2SChannelNumberByChannelIndex(int16_t channel_index) {

  if (channel_index == GATEWAY_EVENTS_CHANNEL_INDEX)
     return GATEWAY_EVENTS_CHANNEL_NUMBER;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core->_z2s_channel.Supla_channel;

    core_it++;
  }
  return 0xFF;
}

/*****************************************************************************/

const char *Z2S_Core::getZ2SChannelNameByChannelNumber(
  uint8_t channel_number) {
    
  if (channel_number == GATEWAY_EVENTS_CHANNEL_NUMBER)
    return gateway_events_channel_name;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == channel_number)
      return z2s_core->_z2s_channel.Supla_channel_name;

    core_it++;
  }
  return "missing channel name!";
}

/*****************************************************************************/

const char *Z2S_Core::getZ2SChannelNameByChannelIndex(int16_t channel_index) {
    
  if (channel_index == GATEWAY_EVENTS_CHANNEL_INDEX)
    return gateway_events_channel_name;

  auto core_it = Z2S_Cores.begin();
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_channel_index == channel_index)
      return z2s_core->_z2s_channel.Supla_channel_name;

    core_it++;
  }
  return "missing channel name!";
}

/*****************************************************************************/

Supla::Control::SwitchBotRelay *Z2S_Core::getSwitchBotRelayInstance(
  int16_t channel_index, uint8_t channel_number) {

  auto core_it = Z2S_Cores.begin();

  bool core_found = false;
  
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (channel_index < 0) {

      if ((z2s_core->_z2s_channel.Supla_channel == channel_number) &&
          ((z2s_core->_z2s_channel.local_channel_type == 
            LOCAL_CHANNEL_TYPE_SWITCHBOT)))
       return static_cast<
        Supla::Control::SwitchBotRelay *>(z2s_core->_z2s_element);    
    }
    else {

      if ((z2s_core->_channel_index == channel_index) &&
          ((z2s_core->_z2s_channel.local_channel_type == 
            LOCAL_CHANNEL_TYPE_SWITCHBOT)))
        return static_cast<
          Supla::Control::SwitchBotRelay *>(z2s_core->_z2s_element);    
    }
    core_it++;
  }
  return nullptr;
}

/*****************************************************************************/

void Z2S_Core::updateZ2SCoresShortAddress(
  uint16_t prev_addr, uint16_t new_addr){

  auto core_it = Z2S_Cores.begin();

  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.short_addr == prev_addr) {

      z2s_core->_short_addr = new_addr;
      z2s_core->_z2s_channel.short_addr = new_addr;
      z2s_core->saveChannelData();
    }

    core_it++;
  }
}

/*****************************************************************************/

Supla::Control::Z2S_RemoteRelay *Z2S_Core::getRemoteRelayPtr() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_REMOTE_RELAY)
    return static_cast<
      Supla::Control::Z2S_RemoteRelay *>(_z2s_element);
  else
    return nullptr;
}

/*****************************************************************************/

Supla::Control::HvacBaseEE *Z2S_Core::getHvacPtr() {

  if (_z2s_channel.Supla_channel_type == SUPLA_CHANNELTYPE_HVAC) {

    Supla::Control::Z2S_TRVInterface *Supla_Z2S_TRVInterface = static_cast<
      Supla::Control::Z2S_TRVInterface *>(_z2s_element);
      
    return Supla_Z2S_TRVInterface->getTRVHvac();
  }
  return nullptr;
}

/*****************************************************************************/

bool Z2S_Core::setMDNSName(const char *mDNS_name) {
  
  if (mDNS_name) {

    // cut "mdns://"
    memcpy(_z2s_channel.remote_channel_data.mDNS_name, mDNS_name + 7, 11);
      _z2s_channel.remote_channel_data.mDNS_name[11] = '\0';
      
    return saveChannelData();
  }
  else
    return false;
}

/*****************************************************************************/

bool Z2S_Core::setRemoteIPAddress(uint32_t remote_ip_address, bool save) {
  
  _z2s_channel.remote_channel_data.remote_ip_address = remote_ip_address;

  if (save)          
    return saveChannelData();
  else
    return true;
}

/*****************************************************************************/

bool Z2S_Core::setSuplaRemoteChannel(uint8_t Supla_remote_channel) {

  _z2s_channel.Supla_remote_channel = Supla_remote_channel;
                
  return saveChannelData();
}

/*****************************************************************************/

bool Z2S_Core::setSourceChannel(uint8_t source_channel) {

  _z2s_channel.source_channel = source_channel;
                
  return saveChannelData();
}


/*****************************************************************************/

bool Z2S_Core::isInSeconds() {

  switch (_z2s_channel.Supla_channel_type) {


    case SUPLA_CHANNELTYPE_ACTIONTRIGGER:
      return false;


    default:
      return true;
  }
}

/*****************************************************************************/

bool Z2S_Core::initZ2SChannelExtendedDataCounter() {

  

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len > 0) &&
        (extended_data_counter_str_len < 8))
      return false;
  }
  sprintf(
    _z2s_channel.extended_data_counter, "EDC_%03U", _channel_index);

  return setChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER, true);
}

/*****************************************************************************/

bool Z2S_Core::setChannelExtendedDataCounter(uint64_t extended_data_counter) {

  
  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len == 0) &&
        (extended_data_counter_str_len == 8))
      return false;

    return Z2S_GatewayPreferences.putULong64(
      _z2s_channel.extended_data_counter, extended_data_counter);
  } 
  else {

    _z2s_channel.data_counter = extended_data_counter;
    return saveChannelData();
  }
}

/*****************************************************************************/

const char *Z2S_Core::getChannelExtendedDataCounterKey() {


  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
        _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len == 0) &&
        (extended_data_counter_str_len == 8))
      return invalid_extended_data_counter_key;

    return (const char*)_z2s_channel.extended_data_counter;
  }
  return no_extended_data_counter_key;
}

/*****************************************************************************/

bool Z2S_Core::removeChannelExtendedDataCounter() {

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len > 0) &&
        (extended_data_counter_str_len < 8))
      Z2S_GatewayPreferences.remove(_z2s_channel.extended_data_counter);

    return clearChannelUserDataFlags(
      USER_DATA_FLAG_EXTENDED_DATA_COUNTER, true);
  }
  return false;
}

/*****************************************************************************/

uint64_t Z2S_Core::getChannelExtendedDataCounter() {

  if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
    size_t extended_data_counter_str_len = strnlen(
      _z2s_channel.extended_data_counter, 8);

    if ((extended_data_counter_str_len == 0) &&
        (extended_data_counter_str_len == 8))
      return 0;
        
    return Z2S_GatewayPreferences.getULong64(
      _z2s_channel.extended_data_counter);
  }
  else
    return _z2s_channel.data_counter;
}

/*****************************************************************************/

void Z2S_Core::updateRemoteThermometer(
  uint8_t Supla_channel, uint32_t connected_thermometer_ip_address,
  uint32_t connected_thermometer_channel, uint8_t value_type,
  int32_t connected_thermometer_value) {


  log_i(
    "Supla_channel %u, connected_thermometer_channel %u, value type %u, "
    "connected_thermometer_temperature %lu", Supla_channel, 
    connected_thermometer_channel, value_type, connected_thermometer_value);

  bool second_source = false;  

  if (Supla_channel > 0x7F) {

    Supla_channel -= 0x80;
    connected_thermometer_channel += 0x80;
    second_source = true;
  }
  
  auto core_it = Z2S_Cores.begin();
    
  while (core_it != Z2S_Cores.end()) {

    auto z2s_core = *core_it;

    if (z2s_core->_z2s_channel.Supla_channel == Supla_channel) {

      if (z2s_core->isRemoteThermometer() &&
          (value_type == RTH_VALUE_TYPE_TEMPERATURE)) {

        
        auto Z2S_RemoteThermometer = static_cast<
          Supla::Sensor::Z2S_RemoteThermometer *>
          (z2s_core->getZ2SElementPtr());
        
        Z2S_RemoteThermometer->setConnectedThermometerTemperature(
          connected_thermometer_ip_address, connected_thermometer_channel,
          connected_thermometer_value);   

        return;         
      }

      if (z2s_core->isVirtualThermHygroMeter()) {

        
        auto Z2S_LocalVirtualThermHygroMeter = static_cast<
          Supla::Sensor::LocalVirtualThermHygroMeter *>
          (z2s_core->getZ2SElementPtr());
        
        switch (value_type) {


          case RTH_VALUE_TYPE_TEMPERATURE: {
  
            Z2S_LocalVirtualThermHygroMeter->setTemp(
              connected_thermometer_value / 100.0);
          } break;


          case RTH_VALUE_TYPE_HUMIDITY: {
  
            Z2S_LocalVirtualThermHygroMeter->setHumi(
              connected_thermometer_value / 100.0);
          } break;
        }    
        return;    
      }

      if (z2s_core->getZbDeviceModelId() == 
            Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL_EXT ||
          z2s_core->getZbDeviceModelId() == 
            Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL_EXT2) {

        auto Z2S_SNZB02DR2ThermHygroMeter = static_cast<
          Supla::Sensor::Z2S_SNZB02DR2ThermHygroMeter *>
          (z2s_core->getZ2SElementPtr());

        switch (value_type) {


          case RTH_VALUE_TYPE_TEMPERATURE: {

            if (second_source)
              Z2S_SNZB02DR2ThermHygroMeter->setSonoffExternalTemperature2(
              connected_thermometer_value);
            else
              Z2S_SNZB02DR2ThermHygroMeter->setSonoffExternalTemperature(
                connected_thermometer_value);
          } break;


          case RTH_VALUE_TYPE_HUMIDITY: {
            
            if (second_source)
              Z2S_SNZB02DR2ThermHygroMeter->setSonoffExternalHumidity2(
                connected_thermometer_value);
            else
              Z2S_SNZB02DR2ThermHygroMeter->setSonoffExternalHumidity(
                connected_thermometer_value);
          } break;
        }
      }
    }
    core_it++;
  }
}

/*****************************************************************************/

void Z2S_Core::resendTemperatureHumidityValue(
  uint8_t value_type, int32_t value) {


  if (!Supla::Network::IsReady())
    return;
  
  uint8_t remote_Supla_channel = getSuplaRemoteChannel();
    
  uint8_t remote_address_type = checkChannelUserDataFlags(
    USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS) ? REMOTE_ADDRESS_TYPE_MDNS : 
    REMOTE_ADDRESS_TYPE_IP4;

  log_i(
    "Resending T/H value: type = %u, address flag = %s, channel = %u,"
    "value = %i", value_type, (remote_address_type == 
    REMOTE_ADDRESS_TYPE_MDNS) ? "MDNS" : "IP4", remote_Supla_channel, value);    
  
  switch(remote_address_type) {


    case REMOTE_ADDRESS_TYPE_IP4: {

      ip_address = getRemoteIPAddress();
      
      if (ip_address == IPAddress(0, 0, 0, 0)) {

        uint8_t Supla_channel = getZ2SChannelNumber();

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

      ip_address = MDNS.queryHost(getMDNSName());

      setRemoteIPAddress(ip_address, false);
    } break;
  }

  if (RemoteThermometer.connect(ip_address, 1234, 500)) {

    uint8_t cmd_id = (value_type == RTH_VALUE_TYPE_TEMPERATURE) ? 0x10 : 0x11;

    RemoteThermometer.printf(
      "Z2SCMD%02u%03u%03u%08ld\n", cmd_id, remote_Supla_channel,
      getZ2SChannelNumber(), value);
    
    char response[8];

    size_t len = RemoteThermometer.readBytesUntil(
      '\n', response, sizeof(response) - 1);
    response[len] = '\0';

    if (strcmp(response, "OK") == 0) {
    
      log_i("T/H value forwarded");
    }      
  
    RemoteThermometer.stop();
  } 
  else
    log_e(
      "T/H value forwarding FAILED - no connection to remote thremometer"); 
}

/*****************************************************************************/

bool Z2S_Core::updateLocalRelayParams() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_RELAY) {

    Supla::Control::LocalRelay *Supla_LocalRelay = static_cast<
      Supla::Control::LocalRelay *>(_z2s_element);

    Supla_LocalRelay->setPin(_z2s_channel.local_relay_data.gpio_pin);
    Supla_LocalRelay->setHighIsOn(_z2s_channel.local_relay_data.high_is_on);

    return true;
  }
  else
    return false;
}

/*****************************************************************************/

bool Z2S_Core::updateLocalBinaryParams() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_BINARY) {

    Supla::Sensor::LocalBinary *Supla_LocalBinary = static_cast<
      Supla::Sensor::LocalBinary *>(_z2s_element);

    Supla_LocalBinary->setPin(_z2s_channel.local_binary_data.gpio_pin);
    Supla_LocalBinary->setInvertLogic(
      _z2s_channel.local_binary_data.invert_logic);
    Supla_LocalBinary->setPullUp(_z2s_channel.local_binary_data.pull_up);

    return true;
  }
  else
    return false;
}

/*****************************************************************************/

bool Z2S_Core::updateLocalHvacParams() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_HVAC) {

    Supla::Control::LocalHvac *Supla_LocalHvac = static_cast<
      Supla::Control::LocalHvac *>(_z2s_element);

    Supla_LocalHvac->setPin(_z2s_channel.local_hvac_data.gpio_pin);
    Supla_LocalHvac->setHighIsOn(_z2s_channel.local_hvac_data.high_is_on);

    return true;
  }
  else
    return false;
}

/*****************************************************************************/

bool Z2S_Core::updateLocalHcsr04Params() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_HCSR04) {

    Supla::Sensor::LocalHCSR04 *Supla_LocalHcsr04 = static_cast<
      Supla::Sensor::LocalHCSR04 *>(_z2s_element);

    Supla_LocalHcsr04->setTrigPin(_z2s_channel.local_hcsr04_data.trig_pin);
    Supla_LocalHcsr04->setEchoPin(_z2s_channel.local_hcsr04_data.echo_pin);
    Supla_LocalHcsr04->setMinIn(_z2s_channel.local_hcsr04_data.min_in);
    Supla_LocalHcsr04->setMinOut(_z2s_channel.local_hcsr04_data.min_out);
    Supla_LocalHcsr04->setMaxIn(_z2s_channel.local_hcsr04_data.max_in);
    Supla_LocalHcsr04->setMaxOut(_z2s_channel.local_hcsr04_data.max_out);

    return true;
  }
  else
    return false;
}

/*****************************************************************************/

bool Z2S_Core::updateLocalDs18b20Params() {
    
  if (_z2s_channel.local_channel_type == LOCAL_CHANNEL_TYPE_GPIO_DS18B20) {

    Supla::Sensor::LocalDS18B20 *Supla_LocalDs18b20 = static_cast<
      Supla::Sensor::LocalDS18B20 *>(_z2s_element);

    Supla_LocalDs18b20->initDS18B20(_z2s_channel.local_ds18b20_data.gpio_pin, 
      _z2s_channel.local_ds18b20_data.sensor_address);
    return true;
  }
  else
    return false;
}

/*****************************************************************************/

/*                          Z2S_Device                                       */

/*****************************************************************************/

Z2S_Device *Z2S_Device::getZ2SDeviceByDeviceIndex(int16_t device_index) {

  if (device_index < 0)
    return nullptr;

  for (Z2S_Device *device : Z2S_Devices) {

    if (device->getDeviceIndex() == device_index)
      return device;
  }
  return nullptr;
}

/*****************************************************************************/

Z2S_Device *Z2S_Device::getZ2SDeviceByShortAddress(uint16_t short_addr) {

  for (Z2S_Device *device : Z2S_Devices) {

    if (device->getDeviceShortAddress() == short_addr)
      return device;
  }
  return nullptr;
}

/*****************************************************************************/

Z2S_Device *Z2S_Device::getZ2SDeviceByIeeeAddress(
  esp_zb_ieee_addr_t ieee_addr) {

  for (Z2S_Device* device : Z2S_Devices) {
    
    if (std::memcmp(device->getDeviceIeeeAddress(), ieee_addr, 
          sizeof(esp_zb_ieee_addr_t)) == 0) {
            
      return device;
    }
  }
  return nullptr;
}

/*****************************************************************************/

bool Z2S_Device::hasZbDevice(uint32_t desc_id) {

  for (Z2S_Device *device : Z2S_Devices) {

    if ((device->getRecordId() > 0) && 
        (device->getDeviceDescId() == desc_id)) {
      
      return true;
    }
  }
  return false;
}

/*****************************************************************************/

void Z2S_Device::syncDeviceDescFlags(uint32_t devices_desc_slot) {

  uint32_t desc_flags = 
    Z2S_DEVICES_DESC[devices_desc_slot].z2s_device_config_flags;
    
  uint32_t user_flags = getUserDataFlags();

  user_flags &= ~(
    ZBD_USER_DATA_FLAG_TUYA_QUERY_AFTER_REJOIN | 
    ZBD_USER_DATA_FLAG_TUYA_MCU_VERSION_REQUEST | 
    ZBD_USER_DATA_FLAG_TUYA_FORCE_TIME_SYNC | 
    ZBD_USER_DATA_FLAG_TUYA_USE_SEND_DATA
  );

  if (desc_flags & Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_REJOIN_QUERY)
    user_flags |= ZBD_USER_DATA_FLAG_TUYA_QUERY_AFTER_REJOIN;

  if (desc_flags & Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_MCU_VERSION)
    user_flags |= ZBD_USER_DATA_FLAG_TUYA_MCU_VERSION_REQUEST;

  if (desc_flags & Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_FORCE_TIME_SYNC)
    user_flags |= ZBD_USER_DATA_FLAG_TUYA_FORCE_TIME_SYNC;

  if (desc_flags & Z2S_DEVICE_DESC_CONFIG_FLAG_TUYA_USE_SEND_DATA)
    user_flags |= ZBD_USER_DATA_FLAG_TUYA_USE_SEND_DATA;

  setUserDataFlags(user_flags, false);
}

/*****************************************************************************/

bool Z2S_Device::updateDeviceUidIdx(
  const char *manufacturer_name, const char *model_name) {
  
  bool idx_scan = false;

  if ((manufacturer_name == nullptr) || (model_name == nullptr))
    idx_scan = true;

  uint16_t devices_list_table_size = 
    sizeof(Z2S_DEVICES_LIST) / sizeof(Z2S_DEVICES_LIST[0]);

  uint16_t devices_desc_table_size = 
    sizeof(Z2S_DEVICES_DESC) / sizeof(Z2S_DEVICES_DESC[0]);

  for (uint32_t devices_desc_counter = 0; 
    devices_desc_counter < devices_desc_table_size; devices_desc_counter++) {

    uint32_t device_desc_id = 
      Z2S_DEVICES_DESC[devices_desc_counter].z2s_device_desc_id;
    
    if (getDeviceDescId() == device_desc_id) {
                   
      log_i("Syncing flags for model 0x%04X", device_desc_id);

      syncDeviceDescFlags(devices_desc_counter);
      break;
    }
  }

  for (int devices_list_counter = 0; 
       devices_list_counter < devices_list_table_size; 
       devices_list_counter++) {

    if (idx_scan) {
      if (Z2S_DEVICES_LIST[devices_list_counter].z2s_device_uid == 
          getDeviceUid()) {

        setDevicesListIdx(devices_list_counter, false);

        log_i(
          "device uid = %lu, devices list index = %lu", getDeviceUid(),
          getDevicesListIdx());

        return true;
      }
    } 
    else {
      
      if ((strcmp(model_name, 
            Z2S_DEVICES_LIST[devices_list_counter].model_name) == 0) &&
          (strcmp(manufacturer_name, 
            Z2S_DEVICES_LIST[devices_list_counter].manufacturer_name) == 0)) {

        setDeviceUid(
          Z2S_DEVICES_LIST[devices_list_counter].z2s_device_uid, false);

        setDevicesListIdx(devices_list_counter, false);

        log_i(
          "Manufacturer name: %s, model name: %s, device uid = %lu, "
          "devices list index = %lu", manufacturer_name ? 
          manufacturer_name : "none", model_name ? model_name : "none", 
          getDeviceUid(), getDevicesListIdx());

        return true;
      }
    }
  }
  return false;
}

/*****************************************************************************/

const char* Z2S_Device::getManufacturerName() {

  switch (getRecordId()) {


    case 1: 
    case 2:
      return "Critical error - report to @vajera!";


    case 3: 

      return Z2S_DEVICES_LIST[getDevicesListIdx()].manufacturer_name;
    
  }
  return "Invalid device id!";
}

/*****************************************************************************/

bool Z2S_Device::hasManufacturerName(const char *manufacturer_name) {

  switch (getRecordId()) {


    case 1: 
    case 2:
      return false;


    case 3: 

      return (strcmp(Z2S_DEVICES_LIST[getDevicesListIdx()].manufacturer_name, 
        manufacturer_name) == 0);
    
  }
  return "Invalid device id!";
}

/*****************************************************************************/

const char* Z2S_Device::getModelName() {

  switch (getRecordId()) {


    case 1: 
    case 2:
      return "Critical error - report to @vajera!";


    case 3: 

      return Z2S_DEVICES_LIST[getDevicesListIdx()].model_name;
    
  }
  return "Invalid device id!";
}

/*****************************************************************************/

bool Z2S_Device::isModelName(const char *zb_device_name) {

  switch (getRecordId()) {


    case 1: 
    case 2:
      return false;


    case 3: 

      return (strcmp(
        zb_device_name, 
        Z2S_DEVICES_LIST[getDevicesListIdx()].model_name) == 0);
  }
  return false;
}

/*****************************************************************************/

bool Z2S_Device::isAdvancedDevice() {
      
  switch (getDeviceDescId()) {


    case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE:
    case Z2S_DEVICE_DESC_SONOFF_SMART_DUAL_VALVE:
    case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR:
    case Z2S_DEVICE_DESC_MOES_ALARM:

      return true;


    default:
      return false;
  }
}

/*****************************************************************************/

bool Z2S_Device::hasTuyaCustomCluster() {
  
  switch (getDeviceDescId()) {
  
    
    case Z2S_DEVICE_DESC_TUYA_QUERY:
    case Z2S_DEVICE_DESC_TUYA_ILLUZONE_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_PIR_ILLUMINANCE_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_TS020C_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_SZLM04U_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_228WZH:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_HS2SA1:
    case Z2S_DEVICE_DESC_MOES_SMOKE_DETECTOR_ZSSHMSSD01:
    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR: 
    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
    case Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F:
    case Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_3F_2:
    case Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_4F:
    case Z2S_DEVICE_DESC_TUYA_SOIL_SENSOR_5F:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_1:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_2:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_3:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_4:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_EF00_SENSOR_5:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_1:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_RELAY:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_NEO:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205Z:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG205ZA:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM100S2:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZYM10024GV3:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG204ZE:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG204ZK:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_ZG104PLV:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_MWPS3Z:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_WZM100:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_SZLR08T:
    case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_CO2_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_CO_GAS_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2:
    case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_1X3:
    case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3:
    case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_TUYA_1PHASE_V1_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_TUYA_DIN_RCBO_EM_TEMP:
    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH:
    case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL:
    case Z2S_DEVICE_DESC_TS0601_TRV_ME167:
    case Z2S_DEVICE_DESC_TS0601_TRV_ME168:
    case Z2S_DEVICE_DESC_TS0601_TRV_BECA:
    case Z2S_DEVICE_DESC_TS0601_TRV_MOES:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV601:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV603:
    case Z2S_DEVICE_DESC_TS0601_TRV_GTZ10:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV602Z:
    case Z2S_DEVICE_DESC_TS0601_TRV_TV02:
    case Z2S_DEVICE_DESC_TS0601_TRV_SITERWELL:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV16:
    case Z2S_DEVICE_DESC_TS0601_ZWT_ZWT198:
    case Z2S_DEVICE_DESC_TS0601_HY08WE:
    case Z2S_DEVICE_DESC_TS0601_ZWT_ZWT100:
    case Z2S_DEVICE_DESC_TS0601_MOES_BHT002:
    case Z2S_DEVICE_DESC_TS0601_MOES_ZHTSR:
    case Z2S_DEVICE_DESC_TS0601_BOTR9V:
    case Z2S_DEVICE_DESC_TS0601_EONE_BATB:
    case Z2S_DEVICE_DESC_TS0601_EONE_230W:
    case Z2S_DEVICE_DESC_TS0601_EONE_E40:
    case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY:
    case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR_2:
    case Z2S_DEVICE_DESC_MOES_ALARM:
    case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR:
    case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR_L3:
    case Z2S_DEVICE_DESC_MOES_COVER:
    case Z2S_DEVICE_DESC_CURRYSMARTER_COVER:
    case Z2S_DEVICE_DESC_ZEMISMART_SHADES_DRIVE_MOTOR:
    case Z2S_DEVICE_DESC_TUYA_MB60L_SMART_BLINDS_MOTOR:
    case Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR:
    case Z2S_DEVICE_DESC_GIEX_SMART_VALVE:
    case Z2S_DEVICE_DESC_TUYA_DUAL_WATER_VALVE:
    case Z2S_DEVICE_DESC_TUYA_WATER_LEVEL_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_LCD_3_RELAYS:
    case Z2S_DEVICE_DESC_TUYA_FINGERBOT_PLUS:
    case Z2S_DEVICE_DESC_TUYA_FLOOR_HEATING_BOX_6_ZONES:
    case Z2S_DEVICE_DESC_TUYA_8_RELAYS_DP_CONTROLLER:
    case Z2S_DEVICE_DESC_TUYA_6_RELAYS_DP_CONTROLLER: 
    case Z2S_DEVICE_DESC_TUYA_4_RELAYS_DP_CONTROLLER:
    case Z2S_DEVICE_DESC_TUYA_5_RELAYS_CONTROLLER:
    case Z2S_DEVICE_DESC_TUYA_TH_SENSOR_TEMP_PROBE:
    case Z2S_DEVICE_DESC_TUYA_TH_SENSOR_NTC_PROBE:
    case Z2S_DEVICE_DESC_TUYA_RGBWCT_LED_EF00:
    case Z2S_DEVICE_DESC_TUYA_DP_RELAY:
    case Z2S_DEVICE_DESC_TUYA_SMART_POOL_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_WATER_QUALITY_MONITOR:
	  case Z2S_DEVICE_DESC_TS0601_TRV_GTVZB:
    case Z2S_DEVICE_DESC_DP_IR_REMOTE_CONTROL:
      return true;
    default:
      return false;
  }  
}

/*****************************************************************************/

bool Z2S_Device::isIrRemote() {

  switch (getDeviceDescId()) {

    case Z2S_DEVICE_DESC_IR_REMOTE_CONTROL:
    case Z2S_DEVICE_DESC_DP_IR_REMOTE_CONTROL:
    case Z2S_DEVICE_DESC_MOES_IR_REMOTE_CONTROL:

      return true;


    default:

      return false;
  }
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/