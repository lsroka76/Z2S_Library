#ifndef SRC_Z2S_COMMON_H_
#define SRC_Z2S_COMMON_H_

/***********************************************************************************/

#define Z2S_ZB_DEVICES_MAX_NUMBER                               0x20  //32

//Supla channels
#define Z2S_CHANNELS_MAX_NUMBER                                 0x80      

//Supla channels + non-channel elements (i.e. logic gates)
#define Z2S_ELEMENTS_MAX_NUMBER                                 0x100     

/***********************************************************************************/

#define SUPLA_CHANNELTYPE_LOCALCHANNEL                          0x0000

/***********************************************************************************/

#define USER_DATA_FLAG_SED_TIMEOUT                              (1 << 0)  // 0x00001
#define USER_DATA_FLAG_MSG_DISABLED                             (1 << 1)  // 0x00002

#define USER_DATA_FLAG_CORRECTIONS_DISABLED                     (1 << 2)  // 0x00004
#define USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE                     (1 << 3)  // 0x00008
#define USER_DATA_FLAG_TRV_IGNORE_NEXT_MSG                      (1 << 4)  // 0x00010
#define USER_DATA_FLAG_DISABLE_NOTIFICATIONS                    (1 << 5)  // 0x00020
#define USER_DATA_FLAG_SET_SORWNS_ON_START                      (1 << 6)  // 0x00040
#define USER_DATA_FLAG_HAS_EXTENDED_DATA                        (1 << 7)  // 0x00080
#define USER_DATA_FLAG_TRV_FIXED_CORRECTION                     (1 << 8)  // 0x00100
#define USER_DATA_FLAG_TRV_AUTO_TO_SCHEDULE_MANUAL              (1 << 9)  // 0x00200
#define USER_DATA_FLAG_TRV_COOPERATIVE_CHILDLOCK                (1 << 10) // 0x00400
#define USER_DATA_FLAG_ENABLE_RESEND_TEMPERATURE                (1 << 11) // 0x00800
#define USER_DATA_FLAG_REMOTE_ADDRESS_TYPE_MDNS                 (1 << 12) // 0x01000
#define USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0               (1 << 13) // 0x02000
#define USER_DATA_FLAG_SKIP_SUBDEVICE_REGISTRATION              (1 << 14) // 0x04000
#define USER_DATA_FLAG_EXTENDED_DATA_COUNTER                    (1 << 15) // 0x08000
#define USER_DATA_FLAG_IGNORE_CHANNEL_BATTERY_LEVEL             (1 << 16) // 0x10000

/***********************************************************************************/

#define ZBD_USER_DATA_FLAG_VERSION_2_0                          (1 << 0)
#define ZBD_USER_DATA_FLAG_BINDING_REQUIRED                     (1 << 1)
#define ZBD_USER_DATA_FLAG_RESERVED_2                           (1 << 2)
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

/***********************************************************************************/

#define DEVICE_LOCAL_NAME_MAX_SIZE                              36
#define SUPLA_CHANNEL_NAME_MAX_SIZE                             32

/***********************************************************************************/

#define LOCAL_CHANNEL_TYPE_ACTION_HANDLER                       0x01
#define LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY                        0x02
#define LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY                       0x03

#define LOCAL_CHANNEL_TYPE_REMOTE_RELAY                         0x10
#define LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER                   0x11

#define LOCAL_CHANNEL_TYPE_VIRTUAL_BUTTON                       0x20

#define LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC                         0x30

#define LOCAL_CHANNEL_TYPE_SWITCHBOT                            0x40

#define LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS                       0x80

#define REMOTE_ADDRESS_TYPE_LOCAL                               0x00
#define REMOTE_ADDRESS_TYPE_IP4                                 0x01
#define REMOTE_ADDRESS_TYPE_MDNS                                0x02

/***********************************************************************************/

const static char Z2S_ELEMENTS_INDEX_TABLE_V2[] PROGMEM = 
  "elements_index_table.z2s";
const static char Z2S_ELEMENTS_PREFIX_V2[] PROGMEM = 
  "element_%03d.z2s";

static const char *no_extended_data_counter_key = 
  "no edc key";

static const char *invalid_extended_data_counter_key = 
  "invalid edc key";

/***********************************************************************************/

typedef struct z2s_zb_device_params_s {

  uint32_t record_id;
  uint32_t device_uid;
  uint32_t devices_list_idx;
  uint32_t desc_id;
  uint32_t device_gui_id;
  uint32_t reserved_0;
  uint32_t reserved_1;
  char device_local_name[DEVICE_LOCAL_NAME_MAX_SIZE];
  esp_zb_ieee_addr_t ieee_addr;
  uint16_t short_addr;
  uint8_t endpoints_count;
  uint8_t power_source;
  int8_t rssi;
  uint8_t battery_percentage;
  uint8_t battery_voltage_min;
  uint8_t battery_voltage_max;
  uint32_t last_seen_ms;
  uint32_t keep_alive_ms;
  uint32_t timeout_ms;
  uint32_t user_data_flags;
  union {
    struct {
      uint32_t user_data_1;
      uint32_t user_data_2; 
      uint64_t user_data_3;
      uint64_t user_data_4;
    };
    struct {
      uint32_t value : 24;
      uint32_t program : 8;
      uint32_t pause_time : 24;
      uint32_t cycles : 8;
    } smart_valve_data;
    struct {
      uint16_t        total_duration;
      uint16_t        irrigation_duration;
      uint16_t        pause_duration;
      uint16_t        irrgation_volume;
      uint16_t        fail_safe_duration;
      uint8_t         channel_id;
      uint8_t         program_id;
      uint8_t         unit_id;
    } smart_dual_valve_data;    
  };
} __attribute__((packed)) z2s_zb_device_params_t;  //fields are padded properly anyway

/***********************************************************************************/

typedef struct z2s_device_params_s {

  bool                valid_record;
  uint8_t             extended_data_type;
  uint8_t             local_channel_type;
  uint8_t             local_channel_func;
  uint32_t            model_id;
  esp_zb_ieee_addr_t  ieee_addr;
  uint8_t             endpoint;
  uint16_t            cluster_id;
  uint16_t            short_addr;
  uint8_t             Supla_channel;
union {
  uint8_t             Supla_secondary_channel;
  uint8_t             Supla_remote_channel;
};
  int32_t             Supla_channel_type;
  char                Supla_channel_name[SUPLA_CHANNEL_NAME_MAX_SIZE];
  uint32_t            Supla_channel_func;
  int8_t              sub_id;
  uint8_t             reserved_4;
  uint16_t            gui_control_id;
  
  union {
    struct {
      uint32_t        user_data_1; 
      uint32_t        user_data_2;
      uint32_t        user_data_3;
      uint32_t        user_data_4; 
  
    };
    struct {
      uint32_t        fwd_energy_buffer;
      uint32_t        fwd_energy_timer;
    };
    struct {
      uint32_t        rain_intensity_treshold;
    };
    struct {
      uint32_t        rgb_color_mode;
    };
    struct {
      int32_t        hvac_fixed_temperature_correction;
    };
    struct {
      int32_t         ignore_next_msg_counter;
    };
    struct {
      uint32_t        value : 24;
      uint32_t        program : 8;
      uint32_t        pause_time : 24;
      uint32_t        cycles : 8;
    } smart_valve_data;
    struct {
      uint16_t        total_duration;
      uint16_t        irrigation_duration;
      uint16_t        pause_duration;
      uint16_t        irrgation_volume;
      uint16_t        fail_safe_duration;
      uint8_t         program_id;
      uint8_t         unit_id;
    } smart_dual_valve_data;
    struct {
      Supla::Element  *Supla_element;
      uint8_t         logic_operator;
    } local_action_handler_data;
    struct {
      Supla::Element  *Supla_element;
      uint32_t         button_flags;
      uint32_t         button_last_seen_ms;
      //uint32_t         button_debounce_ms;
    } virtual_button_data;
    struct {
      char            mDNS_name[12];
      uint32_t        remote_ip_address;
    } remote_channel_data;
  };
  uint32_t            user_data_flags;
union {
  uint32_t            timeout_secs;
  uint32_t            timeout_ms;

};
union {
  uint32_t            keep_alive_secs;
  uint32_t            keep_alive_ms;
  uint32_t            action_trigger_hold_ms;
  uint32_t            postponed_turn_on_ms;
};
union {  
  uint32_t            refresh_secs;
  uint32_t            refresh_ms;
  uint32_t            debounce_ms;
  uint32_t            resent_secs;
  uint32_t            resent_ms;
  uint32_t            connected_thermometer_timeout_ms;
  uint32_t            auto_set_ms;
  uint32_t            auto_clear_ms;
};
union {
  struct {
    uint64_t          data_counter;
  };
  struct {
    int64_t           fwd_energy_counter;
  };
  struct {
    char              extended_data_counter[8];
  };
  struct {
    uint32_t          last_temperature_measurement;//TEMP*100
    };
  struct {
    double            initial_gpm_value;
  };
};
  uint8_t             Zb_device_id;
  uint8_t             reserved_7;
  uint8_t             reserved_8;
  uint8_t             reserved_9;
} z2s_device_params_t;


/***********************************************************************************/

extern portMUX_TYPE Z2S_globalMutex;

extern z2s_zb_device_params_t z2s_zb_devices_table[Z2S_ZB_DEVICES_MAX_NUMBER];

extern uint8_t z2s_elements_index_table[Z2S_ELEMENTS_MAX_NUMBER / 8];

/***********************************************************************************/

size_t mbstrnlen(const char *mb_str, size_t max_bytes);

/******************************************************************************/

bool checkIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index);
bool setIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index);
bool clearIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index);

bool Z2S_loadIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name);
bool Z2S_saveIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name);
uint16_t Z2S_getIndexTableEntriesNumber(
  uint8_t *index_table, uint16_t max_index);
int16_t Z2S_getIndexTablePositionCounter(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index);

int16_t Z2S_findFreeEntryIndex(uint8_t *index_table, uint16_t max_index);
int16_t Z2S_findNextIndexPosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index);
int16_t Z2S_findPrevIndexPosition(
  uint8_t *index_table, uint16_t index_position, uint16_t max_index);

bool Z2S_saveObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data, 
  size_t object_size);

bool Z2S_loadObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data, 
  size_t object_size);

bool Z2S_removeObject(uint16_t object_index, const char *file_name_prefix);

/*****************************************************************************/

bool checkElementsIndexTablePosition(uint16_t index_position);
bool setElementsIndexTablePosition(uint16_t index_position);
bool clearElementsIndexTablePosition(uint16_t index_position);

bool Z2S_loadElementsIndexTable();
bool Z2S_saveElementsIndexTable();
uint16_t Z2S_getElementsNumber();
int16_t Z2S_getElementsCounter(uint16_t element_position);

int16_t Z2S_findFreeChannelIndex();
int16_t Z2S_findNextElementPosition(uint16_t element_position = 0);
int16_t Z2S_findPrevElementPosition(
  uint16_t element_position = Z2S_ELEMENTS_MAX_NUMBER);

bool Z2S_saveElement(uint16_t element_index, z2s_device_params_t &element);

bool Z2S_loadElement(uint16_t element_index, z2s_device_params_t &element);

bool Z2S_removeElement(uint16_t element_index);

bool Z2S_saveChannelsTable();

/*****************************************************************************/

namespace Supla {
namespace Control {
class Z2S_RemoteRelay;
class SwitchBotRelay;
class Z2S_TRVInterface;
class HvacBaseEE;
}
}

class Z2S_Core;

inline std::vector<Z2S_Core*> Z2S_Cores;

/***********************************************************************************/

class Z2S_Core {

public:

  Z2S_Core(Supla::Element* z2s_element) {

    //_z2s_core_ptr = this;
    Z2S_Cores.push_back(this);
    _z2s_element = z2s_element;
    //log_i("Element <%08X>, Core <%08X>", _z2s_element, _z2s_core_ptr);
  };

  ~Z2S_Core() {

    auto core_it = Z2S_Cores.begin();
    while (core_it != Z2S_Cores.end()) {

      if (*core_it == this)
        core_it = Z2S_Cores.erase(core_it);
      else core_it++;
    }
    listCores();
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

  void setZ2SZbDevice(z2s_zb_device_params_t *z2s_zb_device) {

    _z2s_zb_device = z2s_zb_device;
  };

  z2s_zb_device_params_t *getZ2SZbDevice() {

    return _z2s_zb_device;
  };

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
    if (Zb_device_id < Z2S_ZB_DEVICES_MAX_NUMBER)
      _z2s_zb_device = z2s_zb_devices_table + Zb_device_id;
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

  uint8_t *getChannelIEEEAddress() {


    return _z2s_channel.ieee_addr;
  }

  bool setChannelIEEEAddress(uint8_t *ieee_addr) {

        memcpy(_z2s_channel.ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t)); 
    return true;
  }

  void test_func() { 
  
    log_i("sent from core!"); 
  };
  
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

    return (_z2s_channel.user_data_flags & flags_to_check);
    
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

  double getChannelInitialGPMValue() {

    return _z2s_channel.initial_gpm_value;
    
  }

  void setChannelInitialGPMValue(double initial_gpm_value) {

    _z2s_channel.initial_gpm_value = initial_gpm_value;
  }

  bool isActionTriggerV2() {
    
    return checkChannelUserDataFlags(
      USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0);
  }

  bool isRemoteRelay() {

        return (_z2s_channel.local_channel_type == 
      LOCAL_CHANNEL_TYPE_REMOTE_RELAY);
  }

  bool isHvacChannel() {

    return (_z2s_channel.Supla_channel_type == SUPLA_CHANNELTYPE_HVAC);
  }

  Supla::Control::HvacBaseEE *getHvacPtr(); 

  Supla::Control::Z2S_RemoteRelay *getRemoteRelayPtr();

  uint8_t getLogicOperator() {

    return _z2s_channel.local_action_handler_data.logic_operator;
  }

  void setLogicOperator(uint8_t logic_operator, bool save = true) {

    _z2s_channel.local_action_handler_data.logic_operator = logic_operator;
    if (save)
      saveChannelData();
  }

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

  uint32_t getRemoteIPAddress() {
  
    return _z2s_channel.remote_channel_data.remote_ip_address;
  }

  bool setRemoteIPAddress(uint32_t remote_ip_address);

  uint8_t getSuplaRemoteChannel() {
  
    return _z2s_channel.Supla_remote_channel;
  }

  bool setSuplaRemoteChannel(uint8_t Supla_remote_channel);

  const char* getMDNSName() {
  
    return _z2s_channel.remote_channel_data.mDNS_name;
  }

  bool setMDNSName(const char *mDNS_name);

  uint32_t getIgnoreNextMsgCounter() {

    return _z2s_channel.ignore_next_msg_counter;
  }

  bool setIgnoreNextMsgCounter(uint32_t counter) {

    _z2s_channel.ignore_next_msg_counter = counter;
    return true;
  }

  bool decIgnoreNextMsgCounter() {

    _z2s_channel.ignore_next_msg_counter--;
    return true;
  }

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


  uint16_t getZ2SChannelGUIControlId() {
    
    return _z2s_channel.gui_control_id;
  }

  bool setZ2SChannelGUIControlId(uint16_t gui_control_id) {
    
    _z2s_channel.gui_control_id = gui_control_id;
    return true;
  }

  bool fillZ2SChannelStruct(z2s_device_params_t& z2s_channel);

  static Z2S_Core *getZ2SCoreByChannelIndex(int16_t channel_index);
  static Z2S_Core *getZ2SCoreByChannelNumber(uint8_t channel_number);
  static Z2S_Core *getZ2SCoreByZbDeviceId(uint8_t Zb_device_id);

  static Supla::Element *getZ2SElementByChannelNumber(uint8_t channel_number);
  static Supla::Element *getZ2SElementByChannelIndex(int16_t channel_index);
  static Supla::Element *getZ2SElementByZbDeviceId(uint8_t Zb_device_id);

  static int16_t getZ2SChannelIndexByChannelNumber(uint8_t channel_number);
  static uint8_t getZ2SChannelNumberByChannelIndex(int16_t channel_index);

  static const char* getZ2SChannelNameByChannelNumber(uint8_t channel_number);
  static const char* getZ2SChannelNameByChannelIndex(int16_t channel_index);

  static uint8_t updateZ2SChannelsByZbDeviceId(
    uint8_t Zb_device_id, uint8_t rssi_percentage, uint8_t battery_level);

  static uint8_t countZ2SChannelsByZbDeviceId(uint8_t Zb_device_id); 

  static Supla::Control::SwitchBotRelay *getSwitchBotRelayInstance(
    int16_t channel_index, uint8_t channel_number = 0xFF);

  static void updateZ2SCoresShortAddress(
    uint16_t prev_addr, uint16_t new_addr);
    

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

  uint64_t getDataCounter() {

    return _z2s_channel.data_counter;
  }

  bool initZ2SChannelExtendedDataCounter();

  bool setChannelExtendedDataCounter(uint64_t extended_data_counter);

  const char *getChannelExtendedDataCounterKey();

  bool removeChannelExtendedDataCounter();

  uint64_t getChannelExtendedDataCounter();

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

  uint8_t getExtendedDataType() {

    return _z2s_channel.extended_data_type;
  }

  bool setExtendedDataType(uint8_t extended_data_type) {

    _z2s_channel.extended_data_type = extended_data_type;
    return true;
  }


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

  uint32_t getZbDeviceModelId() {

    if (_z2s_zb_device)
      return  _z2s_zb_device->desc_id;
    else
      return 0;
  };

  uint32_t getChannelModelId() {

    return _z2s_channel.model_id;
  };

  bool setChannelModelId(uint32_t model_id) {

    _z2s_channel.model_id = model_id;
    return true;
  };

  uint32_t getZbDeviceLastSeenMs() {

    if (_z2s_zb_device)
      return _z2s_zb_device->last_seen_ms;
    else
      return 0;
  };

  void setZbDeviceLastSeenMs(uint32_t last_seen_ms) {

    _z2s_zb_device->last_seen_ms = last_seen_ms;

  };

  int8_t getZbDeviceLastRSSI() {

    if (_z2s_zb_device)
      return _z2s_zb_device->rssi;
    else
      return 0;
  };

  uint8_t getZbDeviceLastRSSIPercentage() {

    if (_z2s_zb_device)
     
      return map(_z2s_zb_device->rssi, -100, -30, 0, 100);
    else
      return 0xFF;
  };

  uint8_t getZbDeviceLastBatteryLevelPercentage() {

    if (_z2s_zb_device)
     
      return checkChannelUserDataFlags(
        USER_DATA_FLAG_IGNORE_CHANNEL_BATTERY_LEVEL) ? 0xFF :
        _z2s_zb_device->battery_percentage;
    else
      return 0xFF;
  };

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

  /*void setKeepAliveMs(uint32_t keep_alive_ms) {

    _z2s_channel.keep_alive_ms = keep_alive_ms;
  }*/

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

  uint32_t getRGBColorMode() {
    
    return _z2s_channel.rgb_color_mode;
  }

  void setRGBColorMode(uint32_t rgb_color_mode) {
    
    _z2s_channel.rgb_color_mode = rgb_color_mode;
    saveChannelData();

  }

  int32_t getHvacFixedTemperatureCorrection() {
    
    return _z2s_channel.hvac_fixed_temperature_correction;
  }

  bool setHvacFixedTemperatureCorrection(
    int32_t hvac_fixed_temperature_correction) {
    
    _z2s_channel.hvac_fixed_temperature_correction = 
      hvac_fixed_temperature_correction;
    return saveChannelData();
  }

  bool initChannelData() {

    memset(&_z2s_channel, 0, sizeof(z2s_device_params_t));
    _z2s_channel.valid_record = true;
    return true;
  }

  bool saveChannelData() {

    return Z2S_saveElement(_channel_index, _z2s_channel);
    //return Z2S_saveChannelsTable();
  }

protected:

  z2s_zb_device_params_t *_z2s_zb_device = nullptr;
  z2s_device_params_t _z2s_channel;
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
