#include "z2s_device_local_action_handler.h"
#include <utility>

/*****************************************************************************/

NetworkClient Z2S_NetworkClient;
IPAddress Z2S_IPAddress;

/*****************************************************************************/

namespace {

struct LocalChannelDescriptor {
  uint8_t type;
  const char* type_name;
  const char* default_caption;
  uint32_t default_func;
  bool disconnect_on_add;
  void (*init_fn)(uint16_t idx, z2s_device_params_t* ch, Supla::Element* el);
  void (*add_fn)(uint8_t slot, const LocalChannelDescriptor& desc,
                 uint32_t func, uint8_t logic_op);
};

/*****************************************************************************/

template <typename T, typename... Args>
T* initOrCastLocalElement(
    uint16_t channel_index, z2s_device_params_t* z2s_channel,
    Supla::Element* element, bool set_channel_num, Args&&... args) {

  if (element) {
    return static_cast<T*>(element);
  }
  T* obj = new T(std::forward<Args>(args)...);
  obj->setZ2SChannel(channel_index, z2s_channel);
  if (set_channel_num && obj->getChannel()) {
    obj->getChannel()->setChannelNumber(z2s_channel->Supla_channel);
  }
  return obj;
}

/*****************************************************************************/

template <typename T, typename PostSetupFn = void (*)(T*)>
void registerNewLocalChannel(
    T* obj, uint8_t slot, const LocalChannelDescriptor& desc,
    uint32_t channel_func, const char* custom_caption = nullptr,
    PostSetupFn post_setup = nullptr) {

  const char* caption = custom_caption ? custom_caption : desc.default_caption;
  bool save_immediately = (post_setup == nullptr);

  Z2S_setLocalChannelData(
      obj->getZ2SCorePtr(), slot, obj->getChannelNumber(),
      NO_CUSTOM_CMD_SID, caption, desc.default_func, 0xFF,
      desc.type, channel_func, 0xFF, save_immediately);

  if (caption) {
    obj->setInitialCaption(caption);
  }
  if (desc.default_func != SUPLA_CHANNELFNC_NONE) {
    obj->setDefaultFunction(desc.default_func);
  }

  if (desc.init_fn) {
    desc.init_fn(slot, obj->getZ2SChannel(), obj->getZ2SElementPtr());
  }

  if (post_setup) {
    post_setup(obj);
    obj->saveChannelData();
  }

  if (auto* ch = obj->getChannel()) {
    ch->setFlag(SUPLA_CHANNEL_FLAG_ALWAYS_ALLOW_CHANNEL_DELETION);
  }

  obj->onLoadConfig(&SuplaDevice);
  obj->onInit();
}

/*****************************************************************************/

template <typename T>
void defaultInitFn(uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
  initOrCastLocalElement<T>(idx, ch, el, true);
}

template <typename T>
void defaultAddFn(uint8_t slot, const LocalChannelDescriptor& desc,
                  uint32_t func, uint8_t) {
  registerNewLocalChannel(new T(), slot, desc, func);
}

/*****************************************************************************/

constexpr LocalChannelDescriptor LOCAL_CHANNEL_REGISTRY[] = {

  {
    LOCAL_CHANNEL_TYPE_ACTION_HANDLER,
    "Local action handler",
    nullptr,
    SUPLA_CHANNELFNC_NONE,
    false,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      initOrCastLocalElement<Supla::LocalActionHandlerWithTrigger>(
          idx, ch, el, false, ch->local_action_handler_data.logic_operator);
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, 
       uint8_t op) {
      uint8_t free_id = Z2S_findFirstFreeLocalActionHandlerId();
      auto* lah = new Supla::LocalActionHandlerWithTrigger(op);
      Z2S_setLocalChannelData(
        lah->getZ2SCorePtr(), slot, free_id, NO_CUSTOM_CMD_SID,
        ACTION_HANDLERS_DEFAULT_NAMES[op], SUPLA_CHANNELFNC_NONE,
        0xFF, d.type, func, op, true);
      d.init_fn(slot, lah->getZ2SChannel(), lah->getZ2SElementPtr());
    }
  },
  {
    LOCAL_CHANNEL_TYPE_VIRTUAL_RELAY,
    "Local virtual relay",
    LOCAL_VIRTUAL_RELAY_NAME,
    SUPLA_CHANNELFNC_POWERSWITCH,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* r = initOrCastLocalElement<Supla::Control::LocalVirtualRelay>(
        idx, ch, el, true, RELAY_FLAGS);
      r->setDefaultStateRestore();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      registerNewLocalChannel(
        new Supla::Control::LocalVirtualRelay(RELAY_FLAGS), slot, d, func);
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_RELAY,
    "Local relay (GPIO)",
    LOCAL_GPIO_RELAY_NAME,
    SUPLA_CHANNELFNC_POWERSWITCH,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      int8_t pin = ch->local_relay_data.gpio_pin;
      bool highIsOn = ch->local_relay_data.high_is_on;

      auto* r = initOrCastLocalElement<Supla::Control::LocalRelay>(
        idx, ch, el, true, pin, highIsOn, RELAY_FLAGS);
      r->setDefaultStateRestore();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* r = new Supla::Control::LocalRelay(-1, true, RELAY_FLAGS);
      registerNewLocalChannel(r, slot, d, func, nullptr, [](auto* r) {
        r->setLocalRelayGpioPin(-1);
        r->setLocalRelayHighIsOn(true);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_BINARY,
    "Local binary (GPIO)",
    LOCAL_GPIO_BINARY_NAME,
    SUPLA_CHANNELFNC_BINARY_SENSOR,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* b = initOrCastLocalElement<Supla::Sensor::LocalBinary>(
        idx, ch, el, true);
      b->updateLocalBinaryParams();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* b = new Supla::Sensor::LocalBinary();
      registerNewLocalChannel(b, slot, d, func, nullptr, [](auto* b) {
        b->setLocalBinaryGpioPin(-1);
        b->setLocalBinaryInvertLogic(false);
        b->setLocalBinaryPullUp(false);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_HVAC,
    "Local hvac (GPIO)",
    LOCAL_GPIO_HVAC_NAME,
    SUPLA_CHANNELFNC_HVAC_THERMOSTAT,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      int8_t pin = ch->local_hvac_data.gpio_pin;
      bool highIsOn = ch->local_hvac_data.high_is_on;

      auto* h = initOrCastLocalElement<Supla::Control::LocalHvac>(
        idx, ch, el, false, pin, highIsOn, ch->Supla_channel);
      h->enableDomesticHotWaterFunctionSupport();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* h = new Supla::Control::LocalHvac(-1, true);
      registerNewLocalChannel(h, slot, d, func, nullptr, [](auto* h) {
        h->setLocalHvacGpioPin(-1);
        h->setLocalHvacHighIsOn(true);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_DS18B20,
    "Local DS18B20 (GPIO)",
    LOCAL_GPIO_DS18B20_NAME,
    SUPLA_CHANNELFNC_THERMOMETER,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      initOrCastLocalElement<Supla::Sensor::LocalDS18B20>(
          idx, ch, el, true, ch->local_ds18b20_data.gpio_pin,
          ch->local_ds18b20_data.sensor_address);
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* ds = new Supla::Sensor::LocalDS18B20(4, nullptr);
      registerNewLocalChannel(ds, slot, d, func, nullptr, [](auto* ds) {
        ds->setLocalDS18B20GpioPin(4);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_HCSR04,
    "Local HC-SR04 (GPIO)",
    LOCAL_GPIO_HCSR04_NAME,
    SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* hc = initOrCastLocalElement<Supla::Sensor::LocalHCSR04>(
          idx, ch, el, true);
      hc->updateLocalHcsr04Params();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* hc = new Supla::Sensor::LocalHCSR04();
      registerNewLocalChannel(hc, slot, d, func, nullptr, [](auto* hc) {
        hc->setLocalHCSR04TrigPin(5);
        hc->setLocalHCSR04EchoPin(6);
        hc->setLocalHCSR04MinIn(0);
        hc->setLocalHCSR04MinOut(500);
        hc->setLocalHCSR04MaxIn(0);
        hc->setLocalHCSR04MaxOut(500);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_SHT3X,
    "Local SHT3x (I2C)",
    LOCAL_GPIO_SHT3X_NAME,
    SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* sht = initOrCastLocalElement<Supla::Sensor::LocalSHT3x>(
          idx, ch, el, true);
      sht->updateLocalSHT3xParams();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* sht = new Supla::Sensor::LocalSHT3x();
      registerNewLocalChannel(sht, slot, d, func, nullptr, [](auto* sht) {
        sht->setLocalSHT3xSdaPin(21);
        sht->setLocalSHT3xSclPin(22);
        sht->setLocalSHT3xAddress(0x44);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_GPIO_ADC,
    "Local ADC (GPIO)",
    LOCAL_GPIO_ADC_NAME,
    SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* adc = initOrCastLocalElement<Supla::Sensor::LocalADC>(
          idx, ch, el, true);
      adc->updateLocalAdcParams();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* adc = new Supla::Sensor::LocalADC();
      registerNewLocalChannel(adc, slot, d, func, nullptr, [](auto* adc) {
        adc->setLocalAdcPin(34);
        adc->setLocalAdcAttenuation(3);
      });
    }
  },
  {
    LOCAL_CHANNEL_TYPE_SWITCHBOT,
    "Switchbot object",
    nullptr,
    SUPLA_CHANNELFNC_POWERSWITCH,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* sb = initOrCastLocalElement<Supla::Control::SwitchBotRelay>(
          idx, ch, el, true, ch->local_channel_func);
      sb->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      sb->setDefaultStateRestore();

      channel_extended_data_sb_t sb_data = {};
      if (Z2S_loadChannelExtendedData(
            idx, CHANNEL_EXTENDED_DATA_TYPE_SB, (uint8_t*)&sb_data)) {
        log_i("SwitchBot device BLE address: %s", sb_data.ble_mac_address);
        log_i(
          "SwitchBot device token: %s (%u)", sb_data.token, 
          sb_data.token_size);
        log_i("SwitchBot device json payload: %s (%u)",
              sb_data.json_payload, sb_data.json_payload_size);
        sb->updateSwitchBotData(sb_data, SB_UPDATE_DATA_LOAD_DIR);
      }
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* sb = new Supla::Control::SwitchBotRelay(func);
      registerNewLocalChannel(
          sb, slot, d, func,
          Supla::Control::SwitchBotRelay::getSwitchBotChannelName(func));
    }
  },
  {
    LOCAL_CHANNEL_TYPE_VIRTUAL_HVAC,
    "Local virtual hvac",
    LOCAL_VIRTUAL_HVAC_NAME,
    SUPLA_CHANNELFNC_HVAC_THERMOSTAT,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      Supla::Control::LocalVirtualHvac* vh = nullptr;
      if (el) {
        vh = static_cast<Supla::Control::LocalVirtualHvac*>(el);
      } else {
        auto* voi = new Supla::Control::VirtualOutputInterface();
        vh = new Supla::Control::LocalVirtualHvac(
          voi, nullptr, ch->Supla_channel);
        vh->setZ2SChannel(idx, ch);
        vh->setInitialCaption(LOCAL_VIRTUAL_HVAC_NAME);
        vh->setDefaultFunction(SUPLA_CHANNELFNC_HVAC_THERMOSTAT);
      }
      vh->enableDomesticHotWaterFunctionSupport();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* voi = new Supla::Control::VirtualOutputInterface();
      auto* vh = new Supla::Control::LocalVirtualHvac(voi);
      registerNewLocalChannel(vh, slot, d, func);
    }
  },
  {
    LOCAL_CHANNEL_TYPE_VIRTUAL_BINARY,
    "Local virtual binary",
    LOCAL_VIRTUAL_BINARY_NAME,
    SUPLA_CHANNELFNC_BINARY_SENSOR,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* vb = initOrCastLocalElement<Supla::Sensor::LocalVirtualBinary>(
          idx, ch, el, true, true);
      vb->setDefaultFunction(SUPLA_CHANNELFNC_BINARY_SENSOR);
      if (Supla::Notification::RegisterNotification(
            vb->getZ2SChannelNumber(), false, true)) {
        vb->registerNotification();
      }
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      registerNewLocalChannel(
          new Supla::Sensor::LocalVirtualBinary(true), slot, d, func);
    }
  },
  {
    LOCAL_CHANNEL_TYPE_REMOTE_THERMOMETER,
    "Local remote thermometer",
    LOCAL_REMOTE_THERMOMETER_NAME,
    SUPLA_CHANNELFNC_THERMOMETER,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* rt = initOrCastLocalElement<Supla::Sensor::Z2S_RemoteThermometer>(
          idx, ch, el, true);
      if (!rt->getConnectedThermometerTimeoutMs()) {
        rt->setConnectedThermometerTimeoutValue(30);
      }
    },
    &defaultAddFn<Supla::Sensor::Z2S_RemoteThermometer>
  },
  {
    LOCAL_CHANNEL_TYPE_REMOTE_RELAY,
    "Local remote relay",
    LOCAL_REMOTE_RELAY_NAME,
    SUPLA_CHANNELFNC_POWERSWITCH,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* rr = initOrCastLocalElement<Supla::Control::Z2S_RemoteRelay>(
          idx, ch, el, true, &Z2S_NetworkClient, 0xFF);
      rr->setDefaultFunction(SUPLA_CHANNELFNC_POWERSWITCH);
      rr->setDefaultStateRestore();
    },
    [](uint8_t slot, const LocalChannelDescriptor& d, uint32_t func, uint8_t) {
      auto* rr = new Supla::Control::Z2S_RemoteRelay(&Z2S_NetworkClient, 0xFF);
      rr->setDefaultStateRestore();
      registerNewLocalChannel(rr, slot, d, func);
    }
  },
  {
    LOCAL_CHANNEL_TYPE_CALC_THERMOMETER,
    "Local calculated thermometer",
    LOCAL_CALCULATED_THERMOMETER_NAME,
    SUPLA_CHANNELFNC_THERMOMETER,
    true,
    &defaultInitFn<Supla::Sensor::CalculatedThermometer>,
    &defaultAddFn<Supla::Sensor::CalculatedThermometer>
  },
  {
    LOCAL_CHANNEL_TYPE_VIRTUAL_THERM_HYGRO_METER,
    "Local virtual thermhygrometer",
    LOCAL_VIRTUAL_THERM_HYGRO_METER_NAME,
    SUPLA_CHANNELFNC_HUMIDITYANDTEMPERATURE,
    true,
    &defaultInitFn<Supla::Sensor::LocalVirtualThermHygroMeter>,
    &defaultAddFn<Supla::Sensor::LocalVirtualThermHygroMeter>
  },
  {
    LOCAL_CHANNEL_TYPE_VIRTUAL_GPM,
    "Local virtual GPM",
    LOCAL_VIRTUAL_GPM_NAME,
    SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT,
    true,
    &defaultInitFn<Supla::Sensor::LocalVirtualGPM>,
    &defaultAddFn<Supla::Sensor::LocalVirtualGPM>
  },
  {
    LOCAL_CHANNEL_TYPE_CALC_GPM,
    "Local calculated GPM",
    LOCAL_CALCULATED_GPM_NAME,
    SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT,
    true,
    [](uint16_t idx, z2s_device_params_t* ch, Supla::Element* el) {
      auto* cgpm = initOrCastLocalElement<Supla::Sensor::CalculatedGPM>(
          idx, ch, el, true);
      cgpm->configureFunctionDefaults();
    },
    &defaultAddFn<Supla::Sensor::CalculatedGPM>
  },
  {
    LOCAL_CHANNEL_TYPE_GATEWAY_EVENTS,
    "Gateway events",
    nullptr,
    SUPLA_CHANNELFNC_NONE,
    false,
    nullptr,
    nullptr
  }
};

const LocalChannelDescriptor* findLocalChannelDescriptor(uint8_t type) {
  for (const auto& desc : LOCAL_CHANNEL_REGISTRY) {
    if (desc.type == type) {
      return &desc;
    }
  }
  return nullptr;
}

}  // namespace

/*****************************************************************************/

const char* getZ2SDeviceLocalActionHandlerTypeName(
    z2s_device_params_t* _z2s_channel) {

  if (const auto* desc = findLocalChannelDescriptor(
      _z2s_channel->local_channel_type)) {
    
    return desc->type_name;
  }
  return "Unknown local object!";
}

/*****************************************************************************/

const char* getZ2SDeviceLocalActionHandlerLogicOperatorName(
    z2s_device_params_t* _z2s_channel) {

  if (_z2s_channel->local_channel_type == LOCAL_CHANNEL_TYPE_ACTION_HANDLER) {
    
    return ACTION_HANDLERS_DEFAULT_NAMES[
        _z2s_channel->local_action_handler_data.logic_operator];
  }
  return "No special functions";
}

/*****************************************************************************/

void initZ2SDeviceLocalActionHandler(
    uint16_t channel_index, z2s_device_params_t* _z2s_channel,
    Supla::Element* element) {

  const auto* desc = findLocalChannelDescriptor(
    _z2s_channel->local_channel_type);
  
  if (desc && desc->init_fn) {
    
    desc->init_fn(channel_index, _z2s_channel, element);
  }
}

/*****************************************************************************/

int16_t addZ2SDeviceLocalActionHandler(
    uint8_t local_channel_type, uint32_t local_channel_func,
    uint8_t logic_operator) {

  uint8_t first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
  if (first_free_slot == 0xFF) {
    return -1;
  }

  const auto* desc = findLocalChannelDescriptor(local_channel_type);
  if (!desc || !desc->add_fn) {
    log_i("unknown local action handler channel type 0%u", local_channel_type);
    return -1;
  }

  desc->add_fn(first_free_slot, *desc, local_channel_func, logic_operator);

  if (!Supla::Storage::IsStateStorageValid()) {
    Supla::Storage::WriteStateStorage();
  }
  Supla::Storage::ConfigInstance()->commit();

  if (desc->disconnect_on_add) {
    Supla::Network::DisconnectProtocols();
  }

  return first_free_slot;
}