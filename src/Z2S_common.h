#ifndef SRC_Z2S_COMMON_H_
#define SRC_Z2S_COMMON_H_

#include "Z2S_StorageManager.h"

/*****************************************************************************/

#define SUPLA_CHANNELTYPE_LOCALCHANNEL                          0x0000

/*****************************************************************************/

#define USER_DATA_FLAG_SED_TIMEOUT                        (1 << 0)  // 0x00001
#define USER_DATA_FLAG_MSG_DISABLED                       (1 << 1)  // 0x00002

#define USER_DATA_FLAG_CORRECTIONS_DISABLED               (1 << 2)  // 0x00004
#define USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE               (1 << 3)  // 0x00008
#define USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG                (1 << 4)  // 0x00010
#define USER_DATA_FLAG_DISABLE_NOTIFICATIONS              (1 << 5)  // 0x00020
#define USER_DATA_FLAG_SET_SORWNS_ON_START                (1 << 6)  // 0x00040
#define USER_DATA_FLAG_HAS_EXTENDED_DATA                  (1 << 7)  // 0x00080
#define USER_DATA_FLAG_TRV_FIXED_CORRECTION               (1 << 8)  // 0x00100
#define USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL        (1 << 9)  // 0x00200
#define USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK          (1 << 10) // 0x00400
#define USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE          (1 << 11) // 0x00800
#define USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS           (1 << 12) // 0x01000
#define USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0         (1 << 13) // 0x02000
#define USER_DATA_FLAG_SKIP_SUBDEVICE_REGISTRATION        (1 << 14) // 0x04000
#define USER_DATA_FLAG_EXTENDED_DATA_COUNTER              (1 << 15) // 0x08000
#define USER_DATA_FLAG_IGNORE_CHANNEL_BATTERY_LEVEL       (1 << 16) // 0x10000

/*****************************************************************************/

#define ZBD_USER_DATA_FLAG_VERSION_2_0                          (1 << 0)
#define ZBD_USER_DATA_FLAG_BINDING_REQUIRED                     (1 << 1)
#define ZBD_USER_DATA_FLAG_FULL_UPDATE_REQUIRED                 (1 << 2)
#define ZBD_USER_DATA_FLAG_RESERVED_3                           (1 << 3)
#define ZBD_USER_DATA_FLAG_RESERVED_4                           (1 << 4)
#define ZBD_USER_DATA_FLAG_SUBDEVICE_REGISTERED                 (1 << 5)
#define ZBD_USER_DATA_FLAG_DISABLE_BATTERY_MSG                  (1 << 6)
#define ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG       (1 << 7)
#define ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG          (1 << 8)

#define ZBD_USER_DATA_FLAG_TUYA_USE_SEND_DATA                   (1 << 13)
#define ZBD_USER_DATA_FLAG_TUYA_FORCE_TIME_SYNC                 (1 << 14)
#define ZBD_USER_DATA_FLAG_TUYA_MCU_VERSION_REQUEST             (1 << 15)
#define ZBD_USER_DATA_FLAG_TUYA_QUERY_AFTER_REJOIN              (1 << 16)
#define ZBD_USER_DATA_FLAG_IAS_ZONE_STATUS_QUERY_AFTER_REJOIN   (1 << 17)
#define ZBD_USER_DATA_FLAG_ON_OFF_STATE_QUERY_AFTER_REJOIN      (1 << 18)

/*****************************************************************************/

#define LOCAL_CHANNEL_TYPE_ACTION_HANDLER                       0x01
#define LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY                        0x02
#define LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY                       0x03
#define LOCAL_CHANNEL_TYPE_VIRTUAL_GPM                          0x04

#define LOCAL_CHANNEL_TYPE_REMOTE_RELAY                         0x10
#define LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER                   0x11

#define LOCAL_CHANNEL_TYPE_VIRTUAL_BUTTON                       0x20

#define LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC                         0x30

#define LOCAL_CHANNEL_TYPE_SWITCHBOT                            0x40

#define LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER            0x50
#define LOCAL_CHANNEL_TYPE_CALC_THERMOMETER                     0x51

#define LOCAL_CHANNEL_TYPE_GPIO_RELAY                           0x60
#define LOCAL_CHANNEL_TYPE_GPIO_HVAC                            0x61
#define LOCAL_CHANNEL_TYPE_GPIO_DS18B20                         0x65
#define LOCAL_CHANNEL_TYPE_GPIO_SHT3X                           0x66
#define LOCAL_CHANNEL_TYPE_GPIO_BINARY                          0x69
#define LOCAL_CHANNEL_TYPE_GPIO_HCSR04                          0x6A


#define LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS                       0x80

#define REMOTE_ADDRESS_TYPE_LOCAL                               0x00
#define REMOTE_ADDRESS_TYPE_IP4                                 0x01
#define REMOTE_ADDRESS_TYPE_MDNS                                0x02

/*****************************************************************************/

#define REMOTE_ADDRESS_TYPE_LOCAL                               0x00
#define REMOTE_ADDRESS_TYPE_IP4                                 0x01
#define REMOTE_ADDRESS_TYPE_MDNS                                0x02

#define RTH_VALUE_TYPE_NONE                                     0x00
#define RTH_VALUE_TYPE_TEMPERATURE                              0x01
#define RTH_VALUE_TYPE_HUMIDITY                                 0x02
#define RTH_VALUE_TYPE_PRESSURE                                 0x03

/*****************************************************************************/

inline const char *no_extended_data_counter_key = 
  "no edc key";

inline const char *invalid_extended_data_counter_key = 
  "invalid edc key";

/*****************************************************************************/

extern portMUX_TYPE Z2S_globalMutex;


/*****************************************************************************/

size_t mbstrnlen(const char *mb_str, size_t max_bytes);

/*****************************************************************************/

namespace Supla {
namespace Control {
class Z2S_RemoteRelay;
class SwitchBotRelay;
class Z2S_TRVInterface;
class HvacBaseEE;
}
}


/*****************************************************************************/
/*****************************************************************************/

class Z2S_Device;

inline std::vector<Z2S_Device *> Z2S_Devices;

/*****************************************************************************/

class Z2S_Device {

  public:

    Z2S_Device() {

      Z2S_Devices.push_back(this);
    }

    ~Z2S_Device() {

      auto device_it = std::find(Z2S_Devices.begin(), Z2S_Devices.end(), this);
      
      if (device_it != Z2S_Devices.end())
        Z2S_Devices.erase(device_it);
    }

    int16_t getDeviceIndex() const {

      return _device_index;
    }

    void setDeviceIndex(int16_t index) {
    
      _device_index = index;
    }

    void setZ2SDevice(
      int16_t device_index, z2s_zb_device_params_t *z2s_device) {

      _device_index = device_index;
      memcpy(&_z2s_device, z2s_device, sizeof(z2s_zb_device_params_t));
  }

    bool saveDeviceData() {

      if (_device_index >= 0)
        return devicesStorage.save(_device_index, _z2s_device);
      else
        return false;
    }

    bool updateDeviceUidIdx(
      const char *manufacturer_name = nullptr, 
      const char *model_name = nullptr);

    void syncDeviceDescFlags(uint32_t devices_desc_slot);

    const char* getManufacturerName();
    bool hasManufacturerName(const char *manufacturer_name);
    const char* getModelName();
    bool isModelName(const char *zb_device_name);

    static Z2S_Device *getZ2SDeviceByDeviceIndex(int16_t device_index);
    static Z2S_Device *getZ2SDeviceByIeeeAddress(esp_zb_ieee_addr_t ieee_addr);
    static Z2S_Device *getZ2SDeviceByShortAddress(uint16_t short_addr);
    static bool hasZbDevice(uint32_t desc_id);

    bool checkDeviceUserDataFlags(uint32_t flags_to_check) const {

      return ((_z2s_device.user_data_flags & flags_to_check) == flags_to_check);
    }

    bool setDeviceUserDataFlags(uint32_t flags_to_set, bool save = true) {

      _z2s_device.user_data_flags |= flags_to_set;
        
      if (save)
        return saveDeviceData();
      else
        return true;
    }

    bool clearDeviceUserDataFlags(uint32_t flags_to_clear, bool save = true) {

      _z2s_device.user_data_flags &= ~flags_to_clear;
      
      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getRecordId() const {

      return _z2s_device.record_id;
    }

    bool setRecordId(uint32_t record_id, bool save = true) {

      _z2s_device.record_id = record_id;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getDeviceUid() const {

      return _z2s_device.device_uid;
    }

    bool setDeviceUid(uint32_t device_uid, bool save = true) {

      _z2s_device.device_uid = device_uid;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getDevicesListIdx() const {

      return _z2s_device.devices_list_idx;
    }

    bool setDevicesListIdx(uint32_t idx, bool save = true) {

      _z2s_device.devices_list_idx = idx;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getDeviceDescId() const {

      return _z2s_device.desc_id;
    }

    bool setDeviceDescId(uint32_t desc_id, bool save = true) {

      _z2s_device.desc_id = desc_id;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getDeviceGuiId() const {

      return _z2s_device.device_gui_id;
    }

    bool setDeviceGuiId(uint32_t gui_id, bool save = true) {

      _z2s_device.device_gui_id = gui_id;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    char *getDeviceLocalName() {

      return _z2s_device.device_local_name;
    }

    bool setDeviceLocalName(const char *name, bool save = true) {

      size_t device_local_name_size_w = mbstrnlen(
        name, DEVICE_LOCAL_NAME_MAX_SIZE - 1);

      strncpy(_z2s_device.device_local_name, name, device_local_name_size_w);

      _z2s_device.device_local_name[device_local_name_size_w] = '\0';

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t *getDeviceIeeeAddress() {

      return _z2s_device.ieee_addr;
    }

    bool setDeviceIeeeAddress(esp_zb_ieee_addr_t addr, bool save = true) {

      memcpy(_z2s_device.ieee_addr, addr, sizeof(esp_zb_ieee_addr_t));

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint16_t getDeviceShortAddress() const {

      return _z2s_device.short_addr;
    }

    bool setDeviceShortAddress(uint16_t short_addr, bool save = true) {

      _z2s_device.short_addr = short_addr;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getEndpointsCount() const {

      return _z2s_device.endpoints_count;
    }

    bool setEndpointsCount(uint8_t count, bool save = true) {

      _z2s_device.endpoints_count = count;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getPowerSource() const {

      return _z2s_device.power_source;
    }

    bool setPowerSource(uint8_t power_source, bool save = true) {

      _z2s_device.power_source = power_source;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    int8_t getRssi() const {

      return _z2s_device.rssi;
    }

    bool setRssi(int8_t rssi, bool save = true) {

      _z2s_device.rssi = rssi;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getBatteryPercentage() const {

      return _z2s_device.battery_percentage;
    }

    bool setBatteryPercentage(uint8_t percentage, bool save = true) {

      _z2s_device.battery_percentage = percentage;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getBatteryVoltageMin() const {

      return _z2s_device.battery_voltage_min;
    }

    bool setBatteryVoltageMin(uint8_t min_v, bool save = true) {

      _z2s_device.battery_voltage_min = min_v;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getBatteryVoltageMax() const {

      return _z2s_device.battery_voltage_max;
    }

    bool setBatteryVoltageMax(uint8_t max_v, bool save = true) {

      _z2s_device.battery_voltage_max = max_v;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getLastSeenMs() const {

      return _z2s_device.last_seen_ms;
    }

    bool setLastSeenMs(uint32_t ms, bool save = true) {

      _z2s_device.last_seen_ms = ms;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getKeepAliveMs() const {

      return _z2s_device.keep_alive_ms;
    }

    bool setKeepAliveMs(uint32_t ms, bool save = true) {

      _z2s_device.keep_alive_ms = ms;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getTimeoutMs() const {

      return _z2s_device.timeout_ms;
    }

    bool setTimeoutMs(uint32_t ms, bool save = true) {

      _z2s_device.timeout_ms = ms;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getUserDataFlags() const {

      return _z2s_device.user_data_flags;
    }

    bool setUserDataFlags(uint32_t flags, bool save = true) {

      _z2s_device.user_data_flags = flags;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getUserData1() const {

      return _z2s_device.user_data_1;
    }

    bool setUserData1(uint32_t data, bool save = true) {

      _z2s_device.user_data_1 = data;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getUserData2() const {

      return _z2s_device.user_data_2;
    }

    bool setUserData2(uint32_t data, bool save = true) {

      _z2s_device.user_data_2 = data;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint64_t getUserData3() const {

      return _z2s_device.user_data_3;
    }

    bool setUserData3(uint64_t data, bool save = true) {

      _z2s_device.user_data_3 = data;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint64_t getUserData4() const {

      return _z2s_device.user_data_4;
    }

    bool setUserData4(uint64_t data, bool save = true) {

      _z2s_device.user_data_4 = data;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getSmartValveDataValue() const {

      return _z2s_device.smart_valve_data.value;
    }

    bool setSmartValveDataValue(uint32_t value, bool save = true) {

      _z2s_device.smart_valve_data.value = value;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getSmartValveDataProgram() const {

      return _z2s_device.smart_valve_data.program;
    }

    bool setSmartValveDataProgram(uint32_t program, bool save = true) {

      _z2s_device.smart_valve_data.program = program;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getSmartValveDataPauseTime() const {

      return _z2s_device.smart_valve_data.pause_time;
    }

    bool setSmartValveDataPauseTime(uint32_t time, bool save = true) {

      _z2s_device.smart_valve_data.pause_time = time;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint32_t getSmartValveDataCycles() const {

      return _z2s_device.smart_valve_data.cycles;
    }

    bool setSmartValveDataCycles(uint32_t cycles, bool save = true) {

      _z2s_device.smart_valve_data.cycles = cycles;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint16_t getSmartDualValveDataTotalDuration() const {

      return _z2s_device.smart_dual_valve_data.total_duration;
    }

    bool setSmartDualValveDataTotalDuration(
      uint16_t duration, bool save = true) {

      _z2s_device.smart_dual_valve_data.total_duration = duration;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint16_t getSmartDualValveDataIrrigationDuration() const {

      return _z2s_device.smart_dual_valve_data.irrigation_duration;
    }

    bool setSmartDualValveDataIrrigationDuration(
      uint16_t duration, bool save = true) {

      _z2s_device.smart_dual_valve_data.irrigation_duration = duration;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint16_t getSmartDualValveDataPauseDuration() const {

      return _z2s_device.smart_dual_valve_data.pause_duration;
    }

    bool setSmartDualValveDataPauseDuration(
      uint16_t duration, bool save = true) {

      _z2s_device.smart_dual_valve_data.pause_duration = duration;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint16_t getSmartDualValveDataIrrigationVolume() const {

      return _z2s_device.smart_dual_valve_data.irrgation_volume;
    }

    bool setSmartDualValveDataIrrigationVolume(
      uint16_t volume, bool save = true) {

      _z2s_device.smart_dual_valve_data.irrgation_volume = volume;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint16_t getSmartDualValveDataFailSafeDuration() const {

      return _z2s_device.smart_dual_valve_data.fail_safe_duration;
    }

    bool setSmartDualValveDataFailSafeDuration(
      uint16_t duration, bool save = true) {

      _z2s_device.smart_dual_valve_data.fail_safe_duration = duration;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getSmartDualValveDataChannelId() const {

      return _z2s_device.smart_dual_valve_data.channel_id;
    }

    bool setSmartDualValveDataChannelId(uint8_t channel_id, bool save = true) {

      _z2s_device.smart_dual_valve_data.channel_id = channel_id;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getSmartDualValveDataProgramId() const {

      return _z2s_device.smart_dual_valve_data.program_id;
    }

    bool setSmartDualValveDataProgramId(uint8_t program_id, bool save = true) {

      _z2s_device.smart_dual_valve_data.program_id = program_id;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

    uint8_t getSmartDualValveDataUnitId() const {

      return _z2s_device.smart_dual_valve_data.unit_id;
    }

    bool setSmartDualValveDataUnitId(uint8_t unit_id, bool save = true) {

      _z2s_device.smart_dual_valve_data.unit_id = unit_id;

      if (save)
        return saveDeviceData();
      else
        return true;
    }

     uint32_t getSmartRemoteDataLastNonOnActionMs() const {

      return _z2s_device.smart_remote_data.last_non_on_action_ms;
    }

    void setSmartRemoteDataLastNonOnActionMs(uint32_t last_non_on_action_ms) {

      _z2s_device.smart_remote_data.last_non_on_action_ms = 
        last_non_on_action_ms;
    }

    bool isAdvancedDevice();
    bool hasTuyaCustomCluster();
    bool isIrRemote();

  protected:

  z2s_zb_device_params_t _z2s_device {};
  int16_t _device_index = -1;

};


/*****************************************************************************/
/*****************************************************************************/

class Z2S_Core;

inline std::vector<Z2S_Core*> Z2S_Cores;

/*****************************************************************************/

class Z2S_Core {

public:

  Z2S_Core(Supla::Element* z2s_element) {

    Z2S_Cores.push_back(this);
    _z2s_element = z2s_element;
  };

  ~Z2S_Core() {

    auto core_it = std::find(Z2S_Cores.begin(), Z2S_Cores.end(), this);
      
      if (core_it != Z2S_Cores.end())
        Z2S_Cores.erase(core_it);
  };

  void listCores() {

    auto core_it = Z2S_Cores.begin();
    while (core_it != Z2S_Cores.end()) {

      log_i(
        "Core ptr 0x%08X, Element ptr 0x%08X, name %s, \n\r"
        "short address 0x%04X, channel #%u, sub id %d\n\r", 
        (*core_it), (*core_it)->_z2s_element,  
        (*core_it)->_z2s_channel.Supla_channel_name, 
        (*core_it)->getChannelShortAddress(), 
        (*core_it)->_z2s_element->getChannelNumber(), 
        (*core_it)->getChannelSubId());
      core_it++;
    }
  }

  Z2S_Core* getZ2SCorePtr() {
    
    return this; 
  }

  Supla::Element* getZ2SElementPtr() {

    return _z2s_element;
  }

  void setZ2SChannel(
    int16_t channel_index, z2s_device_params_t *z2s_channel) {

    _channel_index = channel_index;
    memcpy(&_z2s_channel, z2s_channel, sizeof(z2s_device_params_t));

    setZbDeviceId(_z2s_channel.Zb_device_id);
    
    setZ2SChannelUID(
      _z2s_channel.short_addr, _z2s_channel.endpoint, 
      _z2s_channel.sub_id);
  }

  z2s_device_params_t *getZ2SChannel() {

    return &_z2s_channel;
  };

  uint8_t getZbDeviceId() {
    
    return _z2s_channel.Zb_device_id;
  }

  bool setZbDeviceId(uint8_t Zb_device_id) { 
    
    _z2s_channel.Zb_device_id = Zb_device_id;
    if (Zb_device_id < Z2S_DEVICES_MAX_NUMBER)
      _z2s_device = Z2S_Device::getZ2SDeviceByDeviceIndex(Zb_device_id);
    return true;
  }

  void setZ2SChannelUID(
    uint16_t short_addr, uint8_t endpoint, int8_t sub_id) {
  
    _short_addr = short_addr;
    _endpoint = endpoint;
    _sub_id = sub_id;
  
    log_i("_z2s_channel_uid 0x%08X", _z2s_channel_uid);
  }

  uint32_t getZ2SChannelUID() {

    return _z2s_channel_uid;
  }

  bool setChannelShortAddress(uint16_t short_addr)  {

    _short_addr = short_addr;
    
    _z2s_channel.short_addr = short_addr;
    return true;
  };
  
  bool updateShortAddress(uint16_t short_addr)  {

    return setChannelShortAddress(short_addr);
  };

  uint8_t *getChannelIeeeAddress() {


    return _z2s_channel.ieee_addr;
  }

  bool setChannelIeeeAddress(uint8_t *ieee_addr) {

        memcpy(_z2s_channel.ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t)); 
    return true;
  }

/*****************************************************************************/

  void test_func() { 
  
    log_i("sent from core!"); 
  };
  
/*****************************************************************************/

  uint32_t getChannelUserData1() {

    return _z2s_channel.user_data_1;
    
  }

  uint32_t getChannelUserData2() {

    return _z2s_channel.user_data_2;
    
  }

  uint32_t getChannelUserData3() {

    return _z2s_channel.user_data_3;
    
  }

  uint32_t getChannelUserData4() {

    return _z2s_channel.user_data_4;
    
  }

  void setChannelUserData1(uint32_t user_data_1) {

    _z2s_channel.user_data_1 = user_data_1;
    saveChannelData();
  }

  void setChannelUserData2(uint32_t user_data_2, bool save = true) {

    _z2s_channel.user_data_2 = user_data_2;
    if (save)
      saveChannelData();
  }

  void setChannelUserData3(uint32_t user_data_3) {

    _z2s_channel.user_data_3 = user_data_3;
    saveChannelData();
  }

  void setChannelUserData4(uint32_t user_data_4) {

    _z2s_channel.user_data_4 = user_data_4;
    saveChannelData();
  }

  uint32_t getChannelUserDataFlags() {

    return _z2s_channel.user_data_flags;
    
  }

  bool checkChannelUserDataFlags(uint32_t flags_to_check) {

    return ((_z2s_channel.user_data_flags & flags_to_check) == flags_to_check);
    
  }

  bool setChannelUserDataFlags(uint32_t flags_to_set, bool save = true) {

    _z2s_channel.user_data_flags |= flags_to_set;
      
    if (save)
      return saveChannelData();
    else
      return true;
  }

  bool clearChannelUserDataFlags(uint32_t flags_to_clear, bool save = true) {

    _z2s_channel.user_data_flags &= ~flags_to_clear;
    
    if (save)
      return saveChannelData();
    else
      return true;
  }

/*****************************************************************************/

  double getChannelInitialGPMValue() {

    return _z2s_channel.initial_gpm_value;
    
  }

  void setChannelInitialGPMValue(double initial_gpm_value) {

    _z2s_channel.initial_gpm_value = initial_gpm_value;
  }

  /*****************************************************************************/

  bool isActionTriggerV2() {
    
    return checkChannelUserDataFlags(
      USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0);
  }

/*****************************************************************************/

  bool isRemoteRelay() {

    return (_z2s_channel.local_channel_type == 
      LOCAL_CHANNEL_TYPE_REMOTE_RELAY);
  }

/*****************************************************************************/

  bool isRemoteThermometer() {

    return (_z2s_channel.local_channel_type == 
      LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER);
  }

/*****************************************************************************/

  bool isSwitchBot() {

    return (_z2s_channel.local_channel_type == 
      LOCAL_CHANNEL_TYPE_SWITCHBOT);
  }

  /*****************************************************************************/

  bool isIrRemote() {
      
      return (_z2s_device != nullptr) && _z2s_device->isIrRemote();
  }

  /*****************************************************************************/

  bool isVirtualThermHygroMeter() {

    return (_z2s_channel.local_channel_type == 
      LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER);
  }

  bool isHvacChannel() {

    return (_z2s_channel.Supla_channel_type == SUPLA_CHANNELTYPE_HVAC);
  }

/*****************************************************************************/

  Supla::Control::HvacBaseEE *getHvacPtr(); 

  /*****************************************************************************/

  Supla::Control::Z2S_RemoteRelay *getRemoteRelayPtr();

  /*****************************************************************************/

  uint8_t getLogicOperator() {

    return _z2s_channel.local_action_handler_data.logic_operator;
  }

  void setLogicOperator(uint8_t logic_operator, bool save = true) {

    _z2s_channel.local_action_handler_data.logic_operator = logic_operator;
    if (save)
      saveChannelData();
  }

/*****************************************************************************/

  uint32_t getButtonLastSeenMs() {

    return _z2s_channel.virtual_button_data.button_last_seen_ms;
  }

  void setButtonLastSeenMs(uint32_t button_last_seen_ms) {

    _z2s_channel.virtual_button_data.button_last_seen_ms = 
        button_last_seen_ms;
  }

  uint32_t getButtonFlags() {

    return _z2s_channel.virtual_button_data.button_flags;
  }

  void setButtonFlags(uint32_t button_flags) {

    _z2s_channel.virtual_button_data.button_flags = button_flags;
    saveChannelData();
  }

  int32_t getButtonLastValue() {

    return _z2s_channel.virtual_button_data.button_last_value;
  }

  void setButtonLastValue(int32_t button_last_value) {

    _z2s_channel.virtual_button_data.button_last_value = button_last_value;
  }

  uint32_t getDebounceMs() {
  
    return _z2s_channel.debounce_ms;
  }

  void setDebounceMs(uint32_t debounce_ms) {
  
    _z2s_channel.debounce_ms = debounce_ms;
    saveChannelData();
  }


  uint32_t getActionTriggerHoldMs() {
  
    return _z2s_channel.action_trigger_hold_ms;
  }

  void setActionTriggerHoldMs(uint32_t action_trigger_hold_ms) {
  
    _z2s_channel.action_trigger_hold_ms = action_trigger_hold_ms;
    saveChannelData();
  }

/*****************************************************************************/

  int32_t getLocalRelayGpioPin() {

    return _z2s_channel.local_relay_data.gpio_pin;
  }

  void setLocalRelayGpioPin(int32_t gpio_pin, bool save = false) {

    _z2s_channel.local_relay_data.gpio_pin = gpio_pin;
    if (save)
      saveChannelData();
  }

  bool getLocalRelayHighIsOn() {

    return _z2s_channel.local_relay_data.high_is_on;
  }

  void setLocalRelayHighIsOn(bool high_is_on, bool save = false) {

  _z2s_channel.local_relay_data.high_is_on = high_is_on;
  if (save)
    saveChannelData();
  }

/*****************************************************************************/

  int32_t getLocalBinaryGpioPin() {

    return _z2s_channel.local_binary_data.gpio_pin;
  }

  void setLocalBinaryGpioPin(int32_t gpio_pin, bool save = false) {

    _z2s_channel.local_binary_data.gpio_pin = gpio_pin;
    if (save)
      saveChannelData();
  }

  bool getLocalBinaryInvertLogic() {

    return _z2s_channel.local_binary_data.invert_logic;
  }

  void setLocalBinaryInvertLogic(bool invert_logic, bool save = false) {

  _z2s_channel.local_binary_data.invert_logic = invert_logic;
  if (save)
    saveChannelData();
  }

  bool getLocalBinaryPullUp() {

    return _z2s_channel.local_binary_data.pull_up;
  }

  void setLocalBinaryPullUp(bool pull_up, bool save = false) {

  _z2s_channel.local_binary_data.pull_up = pull_up;
  if (save)
    saveChannelData();
  }

/*****************************************************************************/

  int32_t getLocalHvacGpioPin() {

    return _z2s_channel.local_hvac_data.gpio_pin;
  }

  void setLocalHvacGpioPin(int32_t gpio_pin, bool save = false) {

    _z2s_channel.local_hvac_data.gpio_pin = gpio_pin;
    if (save)
      saveChannelData();
  }

  bool getLocalHvacHighIsOn() {

    return _z2s_channel.local_hvac_data.high_is_on;
  }

  void setLocalHvacHighIsOn(bool high_is_on, bool save = false) {

  _z2s_channel.local_hvac_data.high_is_on = high_is_on;
  if (save)
    saveChannelData();
  }

/*****************************************************************************/

  int8_t getLocalHCSR04TrigPin() {

    return _z2s_channel.local_hcsr04_data.trig_pin;
  }

  void setLocalHCSR04TrigPin(int8_t trig_pin, bool save = false) {

    _z2s_channel.local_hcsr04_data.trig_pin = trig_pin;
    if (save)
      saveChannelData();
  }

  int8_t getLocalHCSR04EchoPin() {

    return _z2s_channel.local_hcsr04_data.echo_pin;
  }

  void setLocalHCSR04EchoPin(int8_t echo_pin, bool save = false) {

    _z2s_channel.local_hcsr04_data.echo_pin = echo_pin;
    if (save)
      saveChannelData();
  }

  int16_t getLocalHCSR04MinIn() {

    return _z2s_channel.local_hcsr04_data.min_in;
  }

  void setLocalHCSR04MinIn(int16_t min_in, bool save = false) {

    _z2s_channel.local_hcsr04_data.min_in = min_in;
    if (save)
      saveChannelData();
  }

  int16_t getLocalHCSR04MinOut() {

    return _z2s_channel.local_hcsr04_data.min_out;
  }

  void setLocalHCSR04MinOut(int16_t min_out, bool save = false) {

    _z2s_channel.local_hcsr04_data.min_out = min_out;
    if (save)
      saveChannelData();
  }

  int16_t getLocalHCSR04MaxIn() {

    return _z2s_channel.local_hcsr04_data.max_in;
  }

  void setLocalHCSR04MaxIn(int16_t max_in, bool save = false) {

    _z2s_channel.local_hcsr04_data.max_in = max_in;
    if (save)
      saveChannelData();
  }

  int16_t getLocalHCSR04MaxOut() {

    return _z2s_channel.local_hcsr04_data.max_out;
  }

  void setLocalHCSR04MaxOut(int16_t max_out, bool save = false) {

    _z2s_channel.local_hcsr04_data.max_out = max_out;
    if (save)
      saveChannelData();
  }

/*****************************************************************************/

  uint8_t getLocalDS18B20GpioPin() {

    return _z2s_channel.local_ds18b20_data.gpio_pin;
  }

  void setLocalDS18B20GpioPin(uint8_t gpio_pin, bool save = false) {

    _z2s_channel.local_ds18b20_data.gpio_pin = gpio_pin;
    if (save)
      saveChannelData();
  }


  uint8_t *getLocalDS18B20Address() {

    if (_z2s_channel.local_ds18b20_data.sensor_address[0] == 0x00)
      return nullptr;
    else
      return _z2s_channel.local_ds18b20_data.sensor_address;
  }

  void setLocalDS18B20Address(uint8_t *sensor_address, bool save = false) {

    memcpy(_z2s_channel.local_ds18b20_data.sensor_address, sensor_address, 8);
    if (save)
      saveChannelData();
  }

/*****************************************************************************/

  uint32_t getFwdEnergyBuffer() {
  
    return _z2s_channel.fwd_energy_buffer;
  }

  void addFwdEnergyBuffer(uint32_t add_value) {
  
    _z2s_channel.fwd_energy_buffer += add_value;
  }

  void clearFwdEnergyBuffer() {
  
    _z2s_channel.fwd_energy_buffer = 0;
  }

  uint32_t getFwdEnergyTimer() {
  
    return _z2s_channel.fwd_energy_timer;
  }

  void setFwdEnergyTimer(uint32_t set_value) {
  
    _z2s_channel.fwd_energy_timer = set_value;
  }

/*****************************************************************************/

  uint32_t getRemoteIPAddress() {
  
    return _z2s_channel.remote_channel_data.remote_ip_address;
  }

  bool setRemoteIPAddress(uint32_t remote_ip_address, bool save = true);

  uint8_t getSuplaRemoteChannel() {
  
    return _z2s_channel.Supla_remote_channel;
  }

  uint8_t getSourceChannel() {
  
    return _z2s_channel.source_channel;
  }

  bool setSuplaRemoteChannel(uint8_t Supla_remote_channel);

  bool setSourceChannel(uint8_t source_channel);

  const char* getMDNSName() {
  
    return _z2s_channel.remote_channel_data.mDNS_name;
  }

  bool setMDNSName(const char *mDNS_name);

  void resendTemperatureHumidityValue( uint8_t value_type, int32_t value);

  uint32_t getIgnoreNextMsgCounter() {

    return _z2s_channel.ignore_next_msg_counter;
  }

/*****************************************************************************/

  bool setIgnoreNextMsgCounter(uint32_t counter) {

    _z2s_channel.ignore_next_msg_counter = counter;
    return true;
  }

  bool decIgnoreNextMsgCounter() {

    _z2s_channel.ignore_next_msg_counter--;
    return true;
  }

/*****************************************************************************/

  uint8_t getSmartValveProgram() {

    return _z2s_channel.smart_valve_data.program;
  }

  bool setSmartValveProgram(uint8_t program) {

    _z2s_channel.smart_valve_data.program = program;
    return saveChannelData();
  }

  uint8_t getSmartValveCycles() {

    return _z2s_channel.smart_valve_data.cycles;
  }

  bool setSmartValveCycles(uint8_t cycles) {

    _z2s_channel.smart_valve_data.cycles = cycles;
    return saveChannelData();
  }

  uint32_t getSmartValveValue() {

    return _z2s_channel.smart_valve_data.value;
  }

  bool setSmartValveValue(uint32_t value) {


    _z2s_channel.smart_valve_data.value = value;
    return saveChannelData();
  }

  uint32_t getSmartValvePauseTime() {

    return _z2s_channel.smart_valve_data.pause_time;
  }

  bool setSmartValvePauseTime(uint32_t pause_time) {

    _z2s_channel.smart_valve_data.pause_time = pause_time;
    return saveChannelData();
  }

  uint8_t getSmartDualValveProgramId() {

  return _z2s_channel.smart_dual_valve_data.program_id;
  }

  bool setSmartDualValveProgramId(uint8_t program_id) {

    _z2s_channel.smart_dual_valve_data.program_id = program_id;
    return saveChannelData();
  }

  uint8_t getSmartDualValveUnitId() {

  return _z2s_channel.smart_dual_valve_data.unit_id;
  }

  bool setSmartDualValveUnitId(uint8_t unit_id) {

    _z2s_channel.smart_dual_valve_data.unit_id = unit_id;
    return saveChannelData();
  }

  uint16_t getSmartDualValveTotalDurationTime() {

    return _z2s_channel.smart_dual_valve_data.total_duration;
  }

  bool setSmartDualValveTotalDurationTime(uint16_t total_duration) {

    _z2s_channel.smart_dual_valve_data.total_duration = total_duration;
    return saveChannelData();
  }

  uint32_t getSmartDualValveIrrigationDurationTime() {

    return _z2s_channel.smart_dual_valve_data.irrigation_duration;
  }

  bool setSmartDualValveIrrigationDurationTime(
    uint16_t irrigation_duration) {

    _z2s_channel.smart_dual_valve_data.irrigation_duration = 
      irrigation_duration;
    return saveChannelData();
  }

  uint32_t getSmartDualValveIrrigationPauseTime() {

    return _z2s_channel.smart_dual_valve_data.pause_duration;
  }

  bool setSmartDualValveIrrigationPauseTime(uint16_t pause_duration) {

    _z2s_channel.smart_dual_valve_data.pause_duration = pause_duration;
    return saveChannelData();
  }

  uint32_t getSmartDualValveFailSafeTime() {

    return _z2s_channel.smart_dual_valve_data.fail_safe_duration;
  }

  bool setSmartDualValveFailSafeTime(uint16_t fail_safe_duration) {

    _z2s_channel.smart_dual_valve_data.fail_safe_duration = 
      fail_safe_duration;
    return saveChannelData();
  }

  uint32_t getSmartDualValveIrrigationVolume() {

    return _z2s_channel.smart_dual_valve_data.irrgation_volume;
  }

  bool setSmartDualValveIrrigationVolume(uint16_t irrgation_volume) {

    _z2s_channel.smart_dual_valve_data.irrgation_volume = irrgation_volume;
    return saveChannelData();
  }

/*****************************************************************************/

  uint8_t getZ2SChannelNumber() {
    
    return _z2s_channel.Supla_channel;
  }

  bool setZ2SChannelNumber(uint8_t Supla_channel) {
    
    _z2s_channel.Supla_channel = Supla_channel;
    return true;
  }

  uint8_t getZ2SSecondaryChannelNumber() {
    
    return _z2s_channel.Supla_secondary_channel;
  }

  bool setZ2SSecondaryChannelNumber(uint8_t Supla_secondary_channel) {
    
    _z2s_channel.Supla_secondary_channel = Supla_secondary_channel;
    return true;
  }

/*****************************************************************************/

  uint16_t getZ2SChannelGUIControlId() {
    
    return _z2s_channel.gui_control_id;
  }

  bool setZ2SChannelGUIControlId(uint16_t gui_control_id) {
    
    _z2s_channel.gui_control_id = gui_control_id;
    return true;
  }

/*****************************************************************************/

  bool fillZ2SChannelStruct(z2s_device_params_t& z2s_channel);

/*****************************************************************************/

  static Z2S_Core *getZ2SCoreByChannelIndex(int16_t channel_index);
  static Z2S_Core *getZ2SCoreByChannelNumber(uint8_t channel_number);
  static Z2S_Core *getZ2SCoreByZbDeviceId(uint8_t Zb_device_id);
  static Z2S_Core *getZ2SCoreByZbDeviceIdAndChannelType(
    uint8_t Zb_device_id, int32_t Supla_channel_type);
  
  static Supla::Element *getZ2SElementByChannelNumber(uint8_t channel_number);
  //this one checks for TRV/Hvac edge case
  static Supla::Element *getSuplaElementByChannelNumber(uint8_t channel_number);
  static Supla::Element *getSuplaElementByChannelNumberAndType(
    uint8_t channel_number, int32_t Supla_channel_type);

  static Supla::Element *getZ2SElementByChannelIndex(int16_t channel_index);
  static Supla::Element *getZ2SElementByZbDeviceId(uint8_t Zb_device_id);

  static int16_t getZ2SChannelIndexByChannelNumber(uint8_t channel_number);
  static uint8_t getZ2SChannelNumberByChannelIndex(int16_t channel_index);

  static const char* getZ2SChannelNameByChannelNumber(uint8_t channel_number);
  static const char* getZ2SChannelNameByChannelIndex(int16_t channel_index);

  static uint8_t updateZ2SChannelsByZbDeviceId(
    uint8_t Zb_device_id, uint8_t rssi_percentage, uint8_t battery_level);

  static uint8_t countZ2SChannelsByZbDeviceId(uint8_t Zb_device_id); 

/*****************************************************************************/

  static Supla::Control::SwitchBotRelay *getSwitchBotRelayInstance(
    int16_t channel_index, uint8_t channel_number = 0xFF);

  static void updateZ2SCoresShortAddress(
    uint16_t prev_addr, uint16_t new_addr);

  static void updateRemoteThermometer(
    uint8_t Supla_channel, uint32_t connected_thermometer_ip_address,
    uint32_t connected_thermometer_channel, uint8_t value_type,
    int32_t connected_thermometer_value);

/*****************************************************************************/

  const char* getZ2SChannelName() {

    return _z2s_channel.Supla_channel_name;
  }

  bool setZ2SChannelName(const char *Supla_channel_name, bool save = true) {

    if (Supla_channel_name) {

      size_t Supla_channel_name_size_w = mbstrnlen(
        Supla_channel_name, SUPLA_CHANNEL_NAME_MAX_SIZE - 1);

      strncpy(
				_z2s_channel.Supla_channel_name, Supla_channel_name, 
        Supla_channel_name_size_w);

        _z2s_channel.Supla_channel_name[Supla_channel_name_size_w] = '\0';
      
      if (save)
        return saveChannelData();

      return true;
    }
    
  }

/*****************************************************************************/

  uint64_t getDataCounter() {

    return _z2s_channel.data_counter;
  }

  bool initZ2SChannelExtendedDataCounter();

  bool setChannelExtendedDataCounter(uint64_t extended_data_counter);

  const char *getChannelExtendedDataCounterKey();

  bool removeChannelExtendedDataCounter();

  uint64_t getChannelExtendedDataCounter();

/*****************************************************************************/

  uint32_t getZ2SChannelFunction() {

    return _z2s_channel.Supla_channel_func;
    
  }

  bool setZ2SChannelFunction(uint32_t Supla_channel_func) {

    _z2s_channel.Supla_channel_func = Supla_channel_func;
    return true;
  }

  int32_t getZ2SChannelType() {

    return _z2s_channel.Supla_channel_type;
    
  }

  bool setZ2SChannelType(int32_t Supla_channel_type) {

    _z2s_channel.Supla_channel_type = Supla_channel_type;
    return true;
  }

  uint8_t getZ2SLocalChannelType() {

    return _z2s_channel.local_channel_type;
  }

  bool setZ2SLocalChannelType(uint8_t local_channel_type) {

    _z2s_channel.local_channel_type = local_channel_type; 
    return true;
  }

  uint8_t getZ2SLocalChannelFunc() {

    return _z2s_channel.local_channel_func;
  }

  bool setZ2SLocalChannelFunc(uint8_t local_channel_func, bool save = true) {
    
    _z2s_channel.local_channel_func = local_channel_func; 
    if (save)
      return saveChannelData();
    else return true;
  }

/*****************************************************************************/

  uint8_t getExtendedDataType() {

    return _z2s_channel.extended_data_type;
  }

  bool setExtendedDataType(uint8_t extended_data_type) {

    _z2s_channel.extended_data_type = extended_data_type;
    return true;
  }

/*****************************************************************************/

  uint16_t getChannelClusterId() {

    return _z2s_channel.cluster_id;
    
  }

  bool setChannelClusterId(uint16_t cluster_id) {

    _z2s_channel.cluster_id = cluster_id;
    return true;
  }

  uint16_t getChannelShortAddress() {

    //return _short_addr;
    return _z2s_channel.short_addr;
  }

  uint8_t getChannelEndpoint() {

    //return _endpoint;
    return _z2s_channel.endpoint;
  }

  bool setChannelEndpoint(uint8_t endpoint) {

    _endpoint = endpoint;

    _z2s_channel.endpoint = endpoint;
    return true;
  }

  int8_t getChannelSubId() {

    //return _sub_id;

    return _z2s_channel.sub_id;
  }

  bool setChannelSubId(int8_t sub_id) {

    _sub_id = sub_id;

    _z2s_channel.sub_id = sub_id;
    return true;
  }

  int16_t getZ2SChannelIndex() {

    return _channel_index;
  }

  void setChannelIndex(int16_t channel_index) {

    _channel_index = channel_index;
  }

  uint32_t getChannelModelId() {

    return _z2s_channel.model_id;
  };

  bool setChannelModelId(uint32_t model_id) {

    _z2s_channel.model_id = model_id;
    return true;
  };

  uint32_t getZbDeviceModelId() {

    if (_z2s_device)
      return _z2s_device->getDeviceDescId();
    else
      return 0;
  }

  uint32_t getZbDeviceLastSeenMs() {

    if (_z2s_device)
      return _z2s_device->getLastSeenMs();
    else
      return 0;
  }

  void setZbDeviceLastSeenMs(uint32_t last_seen_ms) {

    if (_z2s_device)
      _z2s_device->setLastSeenMs(last_seen_ms, false);
  }

  int8_t getZbDeviceLastRSSI() {

    if (_z2s_device)
      return _z2s_device->getRssi();
    else
      return 0;
  }

  uint8_t getZbDeviceLastRSSIPercentage() {

    if (_z2s_device)
      return map(_z2s_device->getRssi(), -100, -30, 0, 100);
    else
      return 0xFF;
  }

  uint8_t getZbDeviceLastBatteryLevelPercentage() {

    if (_z2s_device)
      return checkChannelUserDataFlags(
        USER_DATA_FLAG_IGNORE_CHANNEL_BATTERY_LEVEL) ? 0xFF :
        _z2s_device->getBatteryPercentage();
    else
      return 0xFF;
  }
  
  const char *getZbDeviceManufacturerName() {

    if (_z2s_device)
      return _z2s_device->getManufacturerName();
    else
      return "NO DEVICE!";
  }

  const char *getZbDeviceModelName() {

    if (_z2s_device)
      return _z2s_device->getModelName();
    else
      return "NO DEVICE!";
  }

  const char *getZbDeviceLocalName() {

    if (_z2s_device)
      return _z2s_device->getDeviceLocalName();
    else
      return "NO DEVICE!";
  }

  uint32_t getLastNonOnActionMs() {

    if (_z2s_device)
      return _z2s_device->getSmartRemoteDataLastNonOnActionMs();
    else
      return 0;
  }

  void setLastNonOnActionMs(uint32_t last_non_on_action_ms) {

    if (_z2s_device)
       _z2s_device->setSmartRemoteDataLastNonOnActionMs(last_non_on_action_ms);
  }

/*****************************************************************************/

  bool isInSeconds();

  uint32_t getKeepAliveMs() {

    return _z2s_channel.keep_alive_ms;
  }

  uint32_t getKeepAliveValue() {

    if (isInSeconds())
      return _z2s_channel.keep_alive_ms / 1000;
    else
      return _z2s_channel.keep_alive_ms;
  }

  void setKeepAliveValue(uint32_t keep_alive_value) {

    if (isInSeconds())
      _z2s_channel.keep_alive_ms = keep_alive_value * 1000;
    else
      _z2s_channel.keep_alive_ms = keep_alive_value;

    saveChannelData();
  }

  uint32_t getTimeoutMs() {

    return _z2s_channel.timeout_ms;
  }

  uint32_t getTimeoutValue() {

    if (isInSeconds())
      return _z2s_channel.timeout_ms / 1000;
    else
      return _z2s_channel.timeout_ms;
  }

  void setTimeoutValue(uint32_t timeout_value) {

    if (isInSeconds())
      _z2s_channel.timeout_ms = timeout_value * 1000;
    else
      _z2s_channel.timeout_ms = timeout_value;

    saveChannelData();
  }

  uint32_t getRefreshMs() {

    return _z2s_channel.refresh_ms;
  }

  uint32_t getRefreshValue() {

    if (isInSeconds())
      return _z2s_channel.refresh_ms / 1000;
    else
      return _z2s_channel.refresh_ms;
  }

  void setRefreshValue(uint32_t refresh_value) {

    if (isInSeconds())
      _z2s_channel.refresh_ms = refresh_value * 1000;
    else
      _z2s_channel.refresh_ms = refresh_value;

    saveChannelData();
  }

  uint32_t getResentMs() {

    return _z2s_channel.resent_ms;
  }

  void setResentMs(uint32_t resent_value) {

    if (isInSeconds())
      _z2s_channel.resent_ms = resent_value * 1000;
    else
      _z2s_channel.resent_ms = resent_value;

    saveChannelData();
  }

  uint32_t getPostponedTurnOnMs() {

    return _z2s_channel.postponed_turn_on_ms;
  }

  uint32_t getPostponedTurnOffMs() {

    return _z2s_channel.postponed_turn_off_ms;
  }

  uint32_t getDebounceTimeMs() {

    return _z2s_channel.debounce_ms;
  }

  uint32_t getConnectedThermometerTimeoutMs() {

    return _z2s_channel.connected_thermometer_timeout_ms;
  }

  void setConnectedThermometerTimeoutValue(
    uint32_t connected_thermometer_timeout_value) {

    setRefreshValue(connected_thermometer_timeout_value);
  }

  uint32_t getAutoSetMs() {

    return _z2s_channel.auto_set_ms;
  }

  uint32_t getAutoClearMs() {

    return _z2s_channel.auto_clear_ms;
  }

/*****************************************************************************/

  uint32_t getRGBColorMode() {
    
    return _z2s_channel.rgb_color_mode;
  }

  void setRGBColorMode(uint32_t rgb_color_mode) {
    
    _z2s_channel.rgb_color_mode = rgb_color_mode;
    saveChannelData();

  }

/*****************************************************************************/

  int32_t getHvacFixedTemperatureCorrection() {
    
    return _z2s_channel.hvac_fixed_temperature_correction;
  }

  bool setHvacFixedTemperatureCorrection(
    int32_t hvac_fixed_temperature_correction) {
    
    _z2s_channel.hvac_fixed_temperature_correction = 
      hvac_fixed_temperature_correction;
    return saveChannelData();
  }

/*****************************************************************************/

  bool updateLocalRelayParams();
  bool updateLocalBinaryParams();
  bool updateLocalHvacParams();
  bool updateLocalDs18b20Params();
  bool updateLocalHcsr04Params();

/*****************************************************************************/

  bool initChannelData() {

    memset(&_z2s_channel, 0, sizeof(z2s_device_params_t));
    _z2s_channel.valid_record = true;
    return true;
  }

  bool saveChannelData() {

    if (_channel_index >= 0)
      return elementsStorage.save(_channel_index, _z2s_channel);
    else
      return false;

  }

protected:

  Z2S_Device *_z2s_device = nullptr;
  z2s_device_params_t _z2s_channel {};
  union {
    struct {
      uint16_t _short_addr;
      uint8_t  _endpoint;
      int8_t  _sub_id;
    };
    uint32_t _z2s_channel_uid;
  };
  Supla::Element* _z2s_element = nullptr;
  int16_t _channel_index = -1;
};

#endif //SRC_Z2S_COMMON_H_
