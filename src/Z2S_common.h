#ifndef SRC_Z2S_COMMON_H_
#define SRC_Z2S_COMMON_H_

#define DEVICE_LOCAL_NAME_MAX_SIZE            36
#define SUPLA_CHANNEL_NAME_MAX_SIZE           32

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
    };
    struct {
      uint32_t value : 24;
      uint32_t program : 8;
      uint32_t pause_time : 24;
      uint32_t cycles : 8;
    } smart_valve_data;
  };
  uint64_t user_data_3;
  uint64_t user_data_4;

} __attribute__((packed)) z2s_zb_device_params_t;  //fields are padded properly anyway

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
      uint32_t        last_temperature_measurement;//TEMP*100
    };
};
  uint8_t             Zb_device_id;
  uint8_t             reserved_7;
  uint8_t             reserved_8;
  uint8_t             reserved_9;
} z2s_device_params_t;

class Z2S_Core {

public:

  void setZ2SZbDevice(z2s_zb_device_params_t *z2s_zb_device) {

    _z2s_zb_device = z2s_zb_device;
  };

  z2s_zb_device_params_t *getZ2SZbDevice() {

    return _z2s_zb_device;
  };

  void setZ2SChannel(z2s_device_params_t *z2s_channel) {

    _z2s_channel = z2s_channel;
  }

  z2s_device_params_t *getZ2SChannel() {

    return _z2s_channel;
  };

  void updateShortAddress(uint16_t short_addr)  {

    _device.short_addr = short_addr;
  }

  void test_func() { 
  
  log_i("sent from core!"); 
};

protected:

  zbg_device_params_t _device;
  z2s_zb_device_params_t *_z2s_zb_device = nullptr;
  z2s_device_params_t *_z2s_channel = nullptr;
};

#endif //SRC_Z2S_COMMON_H_
