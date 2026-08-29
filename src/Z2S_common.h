#ifndef SRC_Z2S_COMMON_H_
#define SRC_Z2S_COMMON_H_

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

extern portMUX_TYPE Z2S_globalMutex;

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
  uint32_t            timeout_secs;
union {
  uint32_t            keep_alive_secs;
  uint32_t            action_trigger_hold_ms;
};
union {  
  uint32_t            refresh_secs;
  uint32_t            debounce_ms;
  uint32_t            resent_secs;
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

bool Z2S_saveChannelsTable();

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
        *core_it, (*core_it)->_z2s_element, (*core_it)->_z2s_channel ? 
        (*core_it)->_z2s_channel->Supla_channel_name : "missing", 
        (*core_it)->_short_addr, 
        (*core_it)->_z2s_element->getChannelNumber(), 
        (*core_it)->_z2s_channel->sub_id);
      core_it++;
    }
  }

  Z2S_Core* getZ2SCorePtr() {
    
    return this; //_z2s_core_ptr;
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
    _z2s_channel = z2s_channel;
    if (_z2s_channel)
      setZ2SChannelUID(
        _z2s_channel->short_addr, _z2s_channel->endpoint, 
        _z2s_channel->sub_id);
  }

  z2s_device_params_t *getZ2SChannel() {

    return _z2s_channel;
  };

  uint8_t getZbDeviceId() {
    
    if (_z2s_channel)
      return _z2s_channel->Zb_device_id;
    else
      return 0xFF;
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

  void updateShortAddress(uint16_t short_addr)  {

    _short_addr = short_addr;
  };

  uint8_t *getIEEEAddress() {

    if (_z2s_channel)
      return _z2s_channel->ieee_addr;
    else 
      return nullptr;
  }

  void test_func() { 
  
    log_i("sent from core!"); 
  };
  
  uint32_t getChannelUserData1() {

    if (_z2s_channel)
      return _z2s_channel->user_data_1;
    else 
      return 0;
  }

  uint32_t getChannelUserData2() {

    if (_z2s_channel)
      return _z2s_channel->user_data_2;
    else 
      return 0;
  }

  uint32_t getChannelUserData3() {

    if (_z2s_channel)
      return _z2s_channel->user_data_3;
    else 
      return 0;
  }

  uint32_t getChannelUserData4() {

    if (_z2s_channel)
      return _z2s_channel->user_data_4;
    else 
      return 0;
  }

  void setChannelUserData1(uint32_t user_data_1) {

    if (_z2s_channel)
      _z2s_channel->user_data_1 = user_data_1;
  }

  void setChannelUserData2(uint32_t user_data_2) {

    if (_z2s_channel)
      _z2s_channel->user_data_2 = user_data_2;
  }

  void setChannelUserData3(uint32_t user_data_3) {

    if (_z2s_channel)
      _z2s_channel->user_data_3 = user_data_3;
  }

  void setChannelUserData4(uint32_t user_data_4) {

    if (_z2s_channel)
      _z2s_channel->user_data_4 = user_data_4;
  }

  uint32_t getChannelUserDataFlags() {

    if (_z2s_channel)
      return _z2s_channel->user_data_flags;
    else 
      return 0;
  }

  bool checkChannelUserDataFlags(uint32_t flags_to_check) {

    if (_z2s_channel)
      return (_z2s_channel->user_data_flags & flags_to_check);
    else 
      return false;
  }

  bool setChannelUserDataFlags(uint32_t flags_to_set, bool save = true) {

    if (_z2s_channel) {

      _z2s_channel->user_data_flags |= flags_to_set;
      
      if (save)
        return Z2S_saveChannelsTable();
      else
        return true;
    }
    else
      return false;
  }

  bool clearChannelUserDataFlags(uint32_t flags_to_clear, bool save = true) {

    if (_z2s_channel) {
      
      _z2s_channel->user_data_flags &= ~flags_to_clear;
    
      if (save)
        return Z2S_saveChannelsTable();
      else
        return true;
    }
    else
      return false;
  }


  double getChannelInitialGPMValue() {

    if (_z2s_channel)
      return _z2s_channel->initial_gpm_value;
    else 
      return 0;
  }

  void setChannelInitialGPMValue(double initial_gpm_value) {

    if (_z2s_channel)
      _z2s_channel->initial_gpm_value = initial_gpm_value;
  }

  bool isActionTriggerV2() {
    
    return checkChannelUserDataFlags(
      USER_DATA_FLAG_ACTION_TRIGGER_VERSION_2_0);
  }

  uint32_t getButtonLastSeenMs() {

    if (_z2s_channel)
      return _z2s_channel->virtual_button_data.button_last_seen_ms;
    else
      return 0;
  }

  void setButtonLastSeenMs(uint32_t button_last_seen_ms) {

    if (_z2s_channel)
      _z2s_channel->virtual_button_data.button_last_seen_ms = 
        button_last_seen_ms;
  }

  uint32_t getDebounceMs() {
  
    if (_z2s_channel)
      return _z2s_channel->debounce_ms;
    else
      return 0;
  }

  uint32_t getActionTriggerHoldMs() {
  
    if (_z2s_channel)
      return _z2s_channel->action_trigger_hold_ms;
    else
      return 0;
  }

  uint32_t getFwdEnergyBuffer() {
  
    if (_z2s_channel)
      return _z2s_channel->fwd_energy_buffer;
    else
      return 0;
  }

  void addFwdEnergyBuffer(uint32_t add_value) {
  
    if (_z2s_channel)
      _z2s_channel->fwd_energy_buffer += add_value;
  }

  void clearFwdEnergyBuffer() {
  
    if (_z2s_channel)
      _z2s_channel->fwd_energy_buffer = 0;
  }

  uint32_t getFwdEnergyTimer() {
  
    if (_z2s_channel)
      return _z2s_channel->fwd_energy_timer;
    else
      return 0;
  }

  void setFwdEnergyTimer(uint32_t set_value) {
  
    if (_z2s_channel)
      _z2s_channel->fwd_energy_timer = set_value;
  }

  uint32_t getRemoteIPAddress() {
  
    if (_z2s_channel)
      return _z2s_channel->remote_channel_data.remote_ip_address;
    else
      return 0;
  }

  bool setRemoteIPAddress(uint32_t remote_ip_address);

  uint8_t getSuplaRemoteChannel() {
  
    if (_z2s_channel)
      return _z2s_channel->Supla_remote_channel;
    else
      return 0;
  }

  bool setSuplaRemoteChannel(uint8_t Supla_remote_channel) {
  
    if (_z2s_channel) {

      _z2s_channel->Supla_remote_channel = Supla_remote_channel;
      return true;
    }
    else
      return false;
  }

  const char* getMDNSName() {
  
    if (_z2s_channel)
      return _z2s_channel->remote_channel_data.mDNS_name;
    else
      return 0;
  }

  bool setMDNSName(const char *mDNS_name);

  uint32_t getIgnoreNextMsgCounter() {

    if (_z2s_channel)
      return _z2s_channel->ignore_next_msg_counter;
    else
      return 0;
  }

  bool setIgnoreNextMsgCounter(uint32_t counter) {

    if (_z2s_channel) {

      _z2s_channel->ignore_next_msg_counter = counter;
      return true;
    }
    else
      return false;
  }


  bool decIgnoreNextMsgCounter() {

    if (_z2s_channel) {

      _z2s_channel->ignore_next_msg_counter--;
      return true;
    }
    else
      return false;
  }

  uint8_t getSmartValveProgram() {

  if (_z2s_channel)
      return _z2s_channel->smart_valve_data.program;
    else
      return 0xFF;
  }

  bool setSmartValveProgram(uint8_t program) {

    if (_z2s_channel) {

      _z2s_channel->smart_valve_data.program = program;
      return true;
    }
    else
      return false;
  }

  uint8_t getSmartValveCycles() {

  if (_z2s_channel)
      return _z2s_channel->smart_valve_data.cycles;
    else
      return 0xFF;
  }

  bool setSmartValveCycles(uint8_t cycles) {

    if (_z2s_channel) {

      _z2s_channel->smart_valve_data.cycles = cycles;
      return true;
    }
    else
      return false;
  }

  uint32_t getSmartValveValue() {

  if (_z2s_channel)
      return _z2s_channel->smart_valve_data.value;
    else
      return 0;
  }

  bool setSmartValveValue(uint32_t value) {

    if (_z2s_channel) {

      _z2s_channel->smart_valve_data.value = value;
      return true;
    }
    else
      return false;
  }

  uint32_t getSmartValvePauseTime() {

  if (_z2s_channel)
      return _z2s_channel->smart_valve_data.pause_time;
    else
      return 0;
  }

  bool setSmartValvePauseTime(uint32_t pause_time) {

    if (_z2s_channel) {

      _z2s_channel->smart_valve_data.pause_time = pause_time;
      return true;
    }
    else
      return false;
  }

  uint8_t getZ2SChannelNumber() {
    
    if (_z2s_channel)
      return _z2s_channel->Supla_channel;
    else 
      return 0xFF;
  }

  uint16_t getZ2SChannelGUIControlId() {
    
    if (_z2s_channel)
      return _z2s_channel->gui_control_id;
    else 
      return 0xFFFF;
  }

  bool setZ2SChannelGUIControlId(uint16_t gui_control_id) {
    
    if (_z2s_channel) {

      _z2s_channel->gui_control_id = gui_control_id;
      return true;
    }
    else 
      return false;
  }

  bool fillZ2SChannelStruct(z2s_device_params_t& z2s_channel);

  static Z2S_Core *getZ2SCoreByChannelIndex(int16_t channel_index);
  static Z2S_Core *getZ2SCoreBySuplaChannelNumber(
    uint8_t Supla_channel_number);
  static Z2S_Core *getZ2SCoreByZbDeviceId(uint8_t Zb_device_id);

  const char* getZ2SChannelName() {

    if (_z2s_channel)
      return _z2s_channel->Supla_channel_name;
    else 
      return nullptr;
  }

  bool setZ2SChannelName(const char *Supla_channel_name) {

    if (_z2s_channel && Supla_channel_name) {

      size_t Supla_channel_name_size_w = mbstrnlen(
        Supla_channel_name, SUPLA_CHANNEL_NAME_MAX_SIZE - 1);

      strncpy(
				_z2s_channel->Supla_channel_name, Supla_channel_name, 
        Supla_channel_name_size_w);

        _z2s_channel->Supla_channel_name[Supla_channel_name_size_w] = '\0';
      return true;
    }
    else 
      return false;
  }

  const char *getZ2SChannelExtendedDataCounter() {

    if (_z2s_channel)
      return _z2s_channel->extended_data_counter;
    else 
      return nullptr;
  }

  bool initZ2SChannelExtendedDataCounter() {

    if (_z2s_channel) {

      if (checkChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER)) {
    
        size_t extended_data_counter_str_len = strnlen(
          _z2s_channel->extended_data_counter, 8);

        if ((extended_data_counter_str_len > 0) &&
            (extended_data_counter_str_len < 8))
        return false;
      }
      sprintf(
        _z2s_channel->extended_data_counter, "EDC_%03U", _channel_index);

      return setChannelUserDataFlags(USER_DATA_FLAG_EXTENDED_DATA_COUNTER);
  }
  else 
    return false;
}

  uint32_t getZ2SChannelType() {

    if (_z2s_channel)
      return _z2s_channel->Supla_channel_type;
    else 
      return 0;
  }

  uint8_t getZ2SLocalChannelType() {

    if (_z2s_channel)
      return _z2s_channel->local_channel_type;
    else 
      return 0xFF;
  }

  uint8_t getExtendedDataType() {

    if (_z2s_channel)
      return _z2s_channel->extended_data_type;
    else 
      return 0x00;
  }

  bool setExtendedDataType(uint8_t extended_data_type) {

    if (_z2s_channel) {

      _z2s_channel->extended_data_type = extended_data_type;
      return true;
    }
    else 
      return false;
  }


  uint32_t getChannelClusterId() {

    if (_z2s_channel)
      return _z2s_channel->cluster_id;
    else 
      return 0;
  }

  uint16_t getChannelShortAddress() {

    return _short_addr;
  }

  uint8_t getChannelEndpoint() {

    return _endpoint;
  }

  int8_t getChannelSubId() {

    return _sub_id;
  }

  int16_t getZ2SChannelIndex() {

    return _channel_index;
  }

  uint32_t getZbDeviceModelId() {

    uint32_t model_id = _z2s_zb_device ? _z2s_zb_device->desc_id : 0;

    return model_id;
  };

  uint32_t getChannelModelId() {

    uint32_t model_id = _z2s_channel ? _z2s_channel->model_id : 0;

    return model_id;
  };

  uint32_t getZbDeviceLastSeenMs() {

    //portENTER_CRITICAL(Z2S_globalMutex);

    uint32_t last_seen_ms = _z2s_zb_device ? _z2s_zb_device->last_seen_ms : 0;

    //portEXIT_CRITICAL(Z2S_globalMutex);

    return last_seen_ms;
  };

  void setZbDeviceLastSeenMs(uint32_t last_seen_ms) {

    _z2s_zb_device->last_seen_ms = last_seen_ms;

  };

  int8_t getZbDeviceLastRSSI() {

    //portENTER_CRITICAL(Z2S_globalMutex);

    int8_t last_rssi = _z2s_zb_device ? _z2s_zb_device->rssi : 0;

    //portEXIT_CRITICAL(Z2S_globalMutex);

    return last_rssi;
  };

protected:

  //zbg_device_params_t _device;
  z2s_zb_device_params_t *_z2s_zb_device = nullptr;
  z2s_device_params_t *_z2s_channel = nullptr;
  union {
    struct {
      uint16_t _short_addr;
      uint8_t  _endpoint;
      int8_t  _sub_id;
    };
    uint32_t _z2s_channel_uid;
  };
  //Z2S_Core* _z2s_core_ptr = nullptr;
  Supla::Element* _z2s_element = nullptr;
  int16_t _channel_index = -1;
};


#endif //SRC_Z2S_COMMON_H_
