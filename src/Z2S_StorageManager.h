#pragma once

/*****************************************************************************/

#define Z2S_ZB_DEVICES_MAX_NUMBER                               0x20  

#define Z2S_DEVICES_MAX_NUMBER                                  0x20

//Supla channels
#define Z2S_CHANNELS_MAX_NUMBER                                 0x80      

//Supla channels + non-channel elements (i.e. logic gates)
#define Z2S_ELEMENTS_MAX_NUMBER                                 0x100     

/*****************************************************************************/

#define DEVICE_LOCAL_NAME_MAX_SIZE                              36
#define SUPLA_CHANNEL_NAME_MAX_SIZE                             32

/*****************************************************************************/

const static char Z2S_ELEMENTS_INDEX_TABLE_V2[] PROGMEM = 
  "elements_index_table.z2s";
const static char Z2S_ELEMENTS_PREFIX_V2[] PROGMEM = 
  "element_%03d.z2s";

const static char Z2S_ELEMENTS_INDEX_TABLE_V3[] PROGMEM = 
  "elements_index_table_v3.z2s";
  const static char Z2S_ELEMENTS_INDEX_TABLE_BACKUP_V3[] PROGMEM = 
  "elements_index_table_v3.bak";

const static char Z2S_ELEMENTS_PREFIX_V3[] PROGMEM = 
  "element_%03d_v3.z2s";

const static char Z2S_ELEMENTS_BACKUP_PREFIX_V3[] PROGMEM = 
  "element_%03d_v3.bak";

/*****************************************************************************/

const static char Z2S_DEVICES_INDEX_TABLE_V3[] PROGMEM = 
  "devices_index_table_v3.z2s";
  const static char Z2S_DEVICES_INDEX_TABLE_BACKUP_V3[] PROGMEM = 
  "devices_index_table_v3.bak";

const static char Z2S_DEVICES_PREFIX_V3[] PROGMEM = 
  "device_%03d_v3.z2s";

const static char Z2S_DEVICES_BACKUP_PREFIX_V3[] PROGMEM = 
  "device_%03d_v3.bak";

/*****************************************************************************/

/*****************************************************************************/

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

/*****************************************************************************/

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
  uint8_t             source_channel;
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
      int32_t         hvac_fixed_temperature_correction;
      uint32_t         hvac_reserved; //probably unused
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
      void            *Supla_element;
      uint8_t         logic_operator;
    } local_action_handler_data;
    struct {
      uint32_t         reserved_32;
      uint32_t         button_flags;
      uint32_t         button_last_seen_ms;
      int32_t          button_last_value;
    } virtual_button_data;
    struct {
      char            mDNS_name[12];
      uint32_t        remote_ip_address;
    } remote_channel_data;
    struct {
      int32_t         gpio_pin;
      uint32_t        high_is_on : 1;
    } local_relay_data;
    struct {
      int32_t         gpio_pin;
      uint32_t        high_is_on : 1;
    } local_hvac_data;
    struct {
      int32_t         gpio_pin;
      uint32_t        invert_logic : 1;
      uint32_t        pull_up : 1;
    } local_binary_data;
    struct {
      int8_t         trig_pin;
      int8_t         echo_pin;
      int16_t        min_in;
      int16_t        min_out;
      int16_t        max_in;
      int16_t        max_out;
    } local_hcsr04_data;
    struct {
      uint8_t         gpio_pin;
      uint8_t        sensor_address[8];
    } local_ds18b20_data;
  };
  uint32_t            user_data_flags;
union {
  uint32_t            timeout_secs;
  uint32_t            timeout_ms;
  uint32_t            postponed_turn_off_ms;

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

/*****************************************************************************/

bool checkIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index);
bool setIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index);
bool clearIndexTablePosition(uint8_t *index_table, uint16_t index_position, 
  uint16_t max_index);

bool Z2S_loadIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name, 
  bool use_new_format = false, const char *backup_file_name = nullptr);
bool Z2S_saveIndexTable(
  uint8_t *index_table, size_t table_size, const char *file_name, 
  bool use_new_format = false, const char *backup_file_name = nullptr);

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
  size_t object_size, bool use_new_format = false, 
  const char *backup_file_name = nullptr);

bool Z2S_loadObject(
  uint16_t object_index, const char *file_name_prefix, uint8_t *object_data, 
  size_t object_size, bool use_new_format = false, 
  const char *backup_file_name_prefix = nullptr);

bool Z2S_removeObject(uint16_t object_index, const char *file_name_prefix, 
const char *backup_file_name_prefix = nullptr);

/*****************************************************************************/

template <typename T, uint16_t MAX_ITEMS>
class Z2S_StorageManager {
private:
  
  uint8_t index_table[MAX_ITEMS / 8] = {}; 
  const char* index_file;
  const char* index_backup;
  const char* obj_prefix;
  const char* obj_backup_prefix;

public:
  
  Z2S_StorageManager(const char* idx_file, const char* idx_bak, 
                     const char* o_pref, const char* o_bak_pref)
    : index_file(idx_file), index_backup(idx_bak), 
      obj_prefix(o_pref), obj_backup_prefix(o_bak_pref) {}

  bool checkPosition(uint16_t index) {
    
    return checkIndexTablePosition(index_table, index, MAX_ITEMS);
  }

  bool setPosition(uint16_t index) {
    
    return setIndexTablePosition(index_table, index, MAX_ITEMS);
  }

  bool clearPosition(uint16_t index) {
    
    return clearIndexTablePosition(index_table, index, MAX_ITEMS);
  }

  bool loadIndexTable(bool use_new_format = true) {
    
    return Z2S_loadIndexTable(index_table, sizeof(index_table), index_file, 
    use_new_format, index_backup);
  }

  bool saveIndexTable(bool use_new_format = true) {
    
    return Z2S_saveIndexTable(index_table, sizeof(index_table), index_file, 
    use_new_format, index_backup);
  }

  uint16_t getCount() {
    
    return Z2S_getIndexTableEntriesNumber(index_table, MAX_ITEMS);
  }

  int16_t getCounter(uint16_t index) {
    
    return Z2S_getIndexTablePositionCounter(index_table, index, MAX_ITEMS);
  }

  int16_t findFreeIndex(uint16_t limit = MAX_ITEMS) {
    
    return Z2S_findFreeEntryIndex(index_table, limit);
  }

  int16_t findNext(uint16_t index) {
    
    return Z2S_findNextIndexPosition(index_table, index, MAX_ITEMS);
  }

  int16_t findPrev(uint16_t index) {
    
    return Z2S_findPrevIndexPosition(index_table, index, MAX_ITEMS);
  }

  bool save(uint16_t index, T& item, bool use_new_format = true) {
    
    if (index >= MAX_ITEMS) return false;
    
    if (Z2S_saveObject(index, obj_prefix, (uint8_t*)&item, sizeof(T), 
          use_new_format, obj_backup_prefix)) {
      setPosition(index);
      return saveIndexTable(use_new_format);
    }
    return false;
  }

  bool load(uint16_t index, T& item, bool use_new_format = true) {
    
    if (index >= MAX_ITEMS) return false;

    return Z2S_loadObject(index, obj_prefix, (uint8_t*)&item, sizeof(T), 
      use_new_format, obj_backup_prefix);
  }

  bool remove(uint16_t index) {
    if (index >= MAX_ITEMS) return false;
    if (Z2S_removeObject(index, obj_prefix, obj_backup_prefix)) {
      clearPosition(index);
      return saveIndexTable();
    }
    return false;
  }
};

/*****************************************************************************/

inline Z2S_StorageManager<z2s_device_params_t, Z2S_ELEMENTS_MAX_NUMBER> 
  Elements(
    Z2S_ELEMENTS_INDEX_TABLE_V3, 
    Z2S_ELEMENTS_INDEX_TABLE_BACKUP_V3,
    Z2S_ELEMENTS_PREFIX_V3, 
    Z2S_ELEMENTS_BACKUP_PREFIX_V3
  );

inline Z2S_StorageManager<z2s_zb_device_params_t, Z2S_DEVICES_MAX_NUMBER> 
  Devices(
    Z2S_DEVICES_INDEX_TABLE_V3, 
    Z2S_DEVICES_INDEX_TABLE_BACKUP_V3,
    Z2S_DEVICES_PREFIX_V3, 
    Z2S_DEVICES_BACKUP_PREFIX_V3
  );