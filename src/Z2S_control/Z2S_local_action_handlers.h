/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef LOCAL_ACTION_HANDLERS_H
#define LOCAL_ACTION_HANDLERS_H

#include <Arduino.h>
//#include <HTTPClient.h>
//#include <WiFiClientSecure.h>
#include <Wire.h>

#include <ZigbeeGateway.h>
#include <SuplaDevice.h>
//#include <supla/storage/storage.h>
#include <Z2S_custom_actions_events.h>
#include <supla/action_handler.h>
#include <supla/element.h>
#include <supla/local_action.h>
#include <supla/events.h>
#include <supla/element_with_channel_actions.h>
#include <supla/control/action_trigger.h>
#include <supla/control/virtual_relay.h>
#include <supla/control/hvac_base.h>
#include <supla/sensor/virtual_binary.h>
#include <supla/sensor/virtual_therm_hygro_meter.h>
#include <supla/sensor/HC_SR04_GPM.h>
#include <supla/sensor/DS18B20.h>
#include <supla/sensor/SHT3x.h>
#include <supla/sensor/bh1750.h>
#include <supla/sensor/binary.h>
#include <supla/device/notifications.h>
#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>
#include <supla/network/network.h>
#include <supla/control/internal_pin_output.h>

#include "Z2S_common.h"
#include "Z2S_custom_actions_events.h"

#include "Z2S_sensor/virtual_general_purpose_measurement.h"
#include "Z2S_sensor/virtual_thermometer_with_actions.h"
#include "Z2S_sensor/Z2S_SHT4x.h"

/*****************************************************************************/

  #define GATEWAY_EVENTS_CHANNEL_NUMBER   0xFE
  #define GATEWAY_EVENTS_CHANNEL_INDEX    0xFE

inline constexpr const char * const gateway_events_channel_name = 
  "GATEWAY EVENTS";

/*****************************************************************************/

#define PIN_LOGIC_OPERATOR_NONE         0x00
#define PIN_LOGIC_OPERATOR_AND          0x01
#define PIN_LOGIC_OPERATOR_OR           0x02
#define PIN_LOGIC_OPERATOR_NOT          0x03
#define PIN_LOGIC_OPERATOR_XOR          0x04
#define PIN_LOGIC_OPERATOR_NAND         0x05
#define PIN_LOGIC_OPERATOR_NOR          0x06
#define PIN_LOGIC_OPERATOR_AND3         0x07
#define PIN_LOGIC_OPERATOR_OR3          0x08
#define PIN_LOGIC_OPERATOR_NOP          0x09
#define PIN_LOGIC_OPERATOR_ONE_HOT      0x0A
#define PIN_LOGIC_OPERATOR_AT_MOST_ONE  0x0B
#define PIN_LOGIC_OPERATOR_INHIBIT      0x0C
#define PIN_LOGIC_OPERATOR_MAJORITY     0x0D
#define PIN_LOGIC_OPERATOR_SR_LATCH     0x0E
#define PIN_LOGIC_OPERATOR_JK_LATCH     0x0F
#define PIN_LOGIC_OPERATOR_D_LATCH      0x10
#define PIN_LOGIC_OPERATOR_T_LATCH      0x11
#define PIN_LOGIC_OPERATOR_MAX          0x12

inline constexpr const char * const pin_logic_operator_names[] = {
  "NONE",
  "AND",
  "OR",
  "NOT",
  "XOR",
  "NAND",
  "NOR",
  "AND (3 inputs)",
  "OR (3 inputs)",
  "NOP",
  "ONE-HOT (1-OF-N)",
  "AT MOST ONE (MUTEX)",
  "INHIBIT",
  "MAJORITY (QUORUM)",
  "S-R LATCH",
  "J-K LATCH",
  "D LATCH",
  "T LATCH"
};

/*****************************************************************************/

#define SB_UPDATE_DATA_LOAD_DIR         0x00
#define SB_UPDATE_DATA_SAVE_DIR         0x01

#define SB_DEVICE_TYPE_PRESS_ID         0x00
#define SB_DEVICE_TYPE_ON_OFF_ID        0x01

/*****************************************************************************/

typedef void (*_actionhandler_callback)(int event, int action);

/*****************************************************************************/

inline const char *sb_url_template = 
  "https://api.switch-bot.com/v1.1/devices/%s/commands";

inline const char *sb_host = "api.switch-bot.com";

inline const char *sb_device_path = "POST /v1.1/devices/%s/commands "
  "HTTP/1.1\r\n";

/*****************************************************************************/

#define CALCULATED_THERMOMETER_FUNC_DEW_POINT                           0x0001
#define CALCULATED_THERMOMETER_FUNC_HEAT_INDEX                          0x0002
#define CALCULATED_THERMOMETER_FUNC_READ_HVAC_TEMP_SETPOINT_HEAT        0x0003
#define CALCULATED_THERMOMETER_FUNC_READ_CGPM_TMIN                      0x0004
#define CALCULATED_THERMOMETER_FUNC_READ_CGPM_TMAX                      0x0005
#define CALCULATED_THERMOMETER_FUNC_READ_CGPM_SETPOINT                  0x0006

inline constexpr const char * const calculated_thermomether_functions_names[] = {
  "DEW POINT",
  "HEAT INDEX(FEELS LIKE)",
  "READ HVAC TEMPERATURE SETPOINT HEAT",
  "READ CGPM T_MIN (LINEAR MAP)",
  "READ CGPM T_MAX (LINEAR MAP)",
  "READ CGPM SETPOINT (PI/PID)"
};

/*****************************************************************************/

#define CALCULATED_GPM_FUNC_VPD                                         0x0001
#define CALCULATED_GPM_FUNC_ABSOLUTE_HUMIDITY                           0x0002
#define CALCULATED_GPM_FUNC_SPECIFIC_ENTHALPY                           0x0003
#define CALCULATED_GPM_FUNC_DEW_POINT_SPREAD                            0x0004
#define CALCULATED_GPM_FUNC_WOOD_EMC                                    0x0005
#define CALCULATED_GPM_FUNC_LINEAR_MAP                                  0x0006
#define CALCULATED_GPM_FUNC_PI_CONTROL                                  0x0007
#define CALCULATED_GPM_FUNC_PID_CONTROL                                 0x0008

inline constexpr const char * const calculated_gpm_functions_names[] = {
  "VPD (VAPOR PRESSURE DEFICIT) [kPa]",
  "ABSOLUTE HUMIDITY [g/m3]",
  "SPECIFIC ENTHALPY [kJ/kg]",
  "DEW POINT SPREAD (CONDENSATION MARGIN) [K]",
  "EQUILIBRIUM MOISTURE CONTENT (EMC) [%]",
  "PWM LINEAR MAP (TEMP -> 0-100%) [%]",
  "PWM PI CONTROLLER (TEMP -> 0-100%) [%]",
  "PWM PID CONTROLLER (TEMP -> 0-100%) [%]"
};

/*****************************************************************************/

#define LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_RESTORE                         0x0001
#define LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_ON                              0x0002
#define LOCAL_VIRTUAL_RELAY_FUNC_SWITCH_OFF                             0x0003
#define LOCAL_VIRTUAL_RELAY_FUNC_MOMENTARY                              0x0004
#define LOCAL_VIRTUAL_RELAY_FUNC_ASTABLE                                0x0005

inline constexpr const char * const local_virtual_relay_functions_names[] = {
  "SWITCH (RESTORE STATE)",
  "SWITCH (ON STATE)",
  "SWITCH (OFF STATE)",
  "MOMENTARY (STATELESS PULSE)",
  "ASTABLE (CYCLE)"
};

/*****************************************************************************/

extern bool sendIASNotifications;
extern Supla::Control::VirtualRelay *toggleNotifications;

/*****************************************************************************/

inline bool ensureLocalI2C(int8_t targetSda, int8_t targetScl) {
  static int8_t activeSda = -1;
  static int8_t activeScl = -1;

  if (targetSda < 0 || targetScl < 0 || targetSda == targetScl) {
    return false;
  }

  if (!digitalPinCanOutput(targetSda) || !digitalPinCanOutput(targetScl)) {
    log_e("Invalid I2C pins: SDA=%d, SCL=%d", targetSda, targetScl);
    return false;
  }

  if (targetSda != activeSda || targetScl != activeScl) {
    Wire.end();
    if (Wire.begin(targetSda, targetScl)) {
      activeSda = targetSda;
      activeScl = targetScl;
    } else {
      activeSda = -1;
      activeScl = -1;
      return false;
    }
  }
  return true;
}

/*****************************************************************************/
/*****************************************************************************/

namespace Supla {

class LocalActionHandler : public Element, public LocalAction, 
  public ActionHandler {

  public:
    LocalActionHandler();
    virtual ~LocalActionHandler();

};

/*****************************************************************************/

class LocalActionHandlerWithTrigger : public LocalActionHandler, 
  public Z2S_Core {

  public:
    LocalActionHandlerWithTrigger(
      uint8_t pin_logic_operator = PIN_LOGIC_OPERATOR_AND,
      uint8_t logic_pins_number = 2);
    virtual ~LocalActionHandlerWithTrigger();

    void onInit() override;
    void handleAction(int event, int action) override;
    void iterateAlways() override;

    uint8_t validateLogicPinsNumber(uint8_t logic_operator, uint8_t pins);
    void updateLogicMask();
    void resetLogicRuntimeState(bool clear_pin_values);

    void setLogicOperator(uint8_t logic_operator, bool save = true);
    void setLogicPinsNumber(uint8_t logic_pins_number, bool save = true);

    const char* getOperatorName();
    
  protected:
    uint32_t _pending_postponed_turn_on_ms = 0;
    uint32_t _pending_postponed_turn_off_ms = 0;
  
};

/*****************************************************************************/
class GatewayEvents: public LocalActionHandler {

  public:

    GatewayEvents() {};
    virtual ~GatewayEvents() {};

    void onInit();
    void handleAction(int event, int action) override;
    void iterateAlways();
    void setActionHandlerCallback(_actionhandler_callback actionhandler_callback);

    void sendPushoverMessage(uint32_t action);

    void disableActions() {

      _disable_actions = true;
    }

  protected:

    bool      _disable_actions = false;
    bool      _cyclic_event_enabled = false;
    uint32_t  _cyclic_event_ms = 0;
    uint32_t  _cyclic_event_counter = 0;
    int32_t   _action = -1;
    
    _actionhandler_callback actionhandler_callback = nullptr;
};

/*****************************************************************************/
/*****************************************************************************/

namespace Control {

class LocalActionTrigger: public ActionTrigger, public Z2S_Core {

  public:

    LocalActionTrigger();
    LocalActionTrigger(
      int16_t channel_index, z2s_device_params_t *z2s_channel);
    virtual ~LocalActionTrigger();

    void setHoldMs(uint32_t hold_ms);
    uint32_t getHoldMs();

    void setAction(int32_t action) {
      
        _action = action; 
    }

    void handleAction(int event, int action) override;
    void iterateAlways();

  protected:

    uint32_t _hold_ms = 0;
    uint32_t _last_hold_ms = 0;
    int32_t _action = -1;

};

/*****************************************************************************/

class LocalVirtualRelay: public VirtualRelay, public Z2S_Core {

  public:

    LocalVirtualRelay(
      _supla_int_t functions =
       (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

    virtual ~LocalVirtualRelay();

    void configureRelayFunction();
    const char *getFunctionName();

    void turnOn(_supla_int_t duration = 0) override;
    void turnOff(_supla_int_t duration = 0) override;
    void handleAction(int event, int action) override; 
    void iterateAlways() override;

  protected:

    bool     _astable_running = false;
    bool     _astable_phase_on = false;
    uint32_t _astable_last_switch_ms = 0;
    uint32_t _astable_cycles_done = 0;
};

/*****************************************************************************/

class LocalRelay: public Relay, public Z2S_Core {

  public:

    LocalRelay(
      int pin, bool highIsOn = true, _supla_int_t functions =
       (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

    virtual ~LocalRelay();

    void setPin(int pin);
    void setHighIsOn(bool highIsOn);

    int getPin();
    bool getHighIsOn();

    void handleAction(int event, int action) override; 
    void iterateAlways() override;
  protected:
};

/*****************************************************************************/

class CustomInternalPinOutput : public InternalPinOutput {

  public:

    explicit CustomInternalPinOutput()  : InternalPinOutput(-1, true) {
    }

    Supla::Io::IoPin *getIoPin() {

      return &outPin;
    }
};

class LocalHvac: public HvacBase, public Z2S_Core {

  public:

    LocalHvac(int pin, bool highIsOn = true, int channelNumber = -1);

    virtual ~LocalHvac();

    void setPin(int pin);
    void setHighIsOn(bool highIsOn);

    int getPin();
    bool getHighIsOn();

    void handleAction(int event, int action) override; 
    void iterateAlways() override;
  protected:
    CustomInternalPinOutput _local_hvac_out_pin;
};

/*****************************************************************************/

class LocalPWM : public ChannelElement, public ActionHandler, public Z2S_Core {

  public:
    LocalPWM();
    virtual ~LocalPWM();

    void onInit() override;
    void onLoadState() override;
    void onSaveState() override;
    void iterateAlways() override;

    int32_t handleNewValueFromServer(
      TSD_SuplaChannelNewValue *newValue) override;
    void handleAction(int event, int action) override;

    void setupPWM();
    void applyHardwarePWM();

    bool isOn() const {
      return (_brightness > 0);
    }

    void turnOn();
    void turnOff();
    void toggle();
    void setBrightness(uint8_t brightness);
    void increaseBrightness(int8_t step);

    void setPWM(uint8_t duty) {
      
      setBrightness(duty);
    }

    uint8_t getPWM() const {
      
      return _brightness;
    }

  protected:
    int8_t   _attached_pin = -1;
    uint32_t _attached_freq = 0;
    uint8_t  _attached_res = 0;

    uint8_t  _brightness = 0;
    uint8_t  _last_brightness = 100;
    uint32_t _last_msg_received_ms = 0;
};

/*****************************************************************************/

class SwitchBotRelay: public Relay, public Z2S_Core {

  public:

    SwitchBotRelay(uint8_t device_type_id = SB_DEVICE_TYPE_PRESS_ID);

    void onInit() override;

    bool updateSwitchBotData(
      channel_extended_data_sb_t &channel_extended_data_sb, uint8_t direction);

    void turnOn(_supla_int_t duration = 0) override;
    void turnOff(_supla_int_t duration = 0) override;

  static const char *getSwitchBotChannelName(uint8_t device_type_id);
  
  protected:
    String _sb_device_id;
    String _sb_token;
    String _json_payload;
    String _json_payload_2;
    uint8_t _device_type_id = SB_DEVICE_TYPE_PRESS_ID;
    //HTTPClient https;
    bool _state;
    //bool _connected = false;
};

/*****************************************************************************/

class LocalVirtualHvac: public HvacBase, public Z2S_Core {

  public:
    explicit LocalVirtualHvac(
      Supla::Control::OutputInterface *primaryOutput = nullptr,
      Supla::Control::OutputInterface *secondaryOutput = nullptr,
      int channelNumber = -1)
      : HvacBase(primaryOutput, secondaryOutput, channelNumber), 
      Z2S_Core(this) {
}
};
}; //namespace Control

/*****************************************************************************/
/*****************************************************************************/

namespace Sensor {

class LocalVirtualBinary: public VirtualBinary, public Z2S_Core {

  public:

    LocalVirtualBinary(bool keepStateInStorage);
    virtual ~LocalVirtualBinary();

    void setAutoClearSecs(uint32_t auto_clear_secs) {

      _auto_clear_ms = auto_clear_secs * 1000;
    }

    void handleAction(int event, int action) override;
    void iterateAlways() override;

    void registerNotification();
    void enableNotifications();
    void disableNotifications();
  
  private:

    bool _notification_registered = false;
    bool _notifications_enabled = false;

    uint32_t _auto_clear_ms = 0;
    uint32_t _last_clear_ms = 0;

    void sendNotification();

};

/*****************************************************************************/

class LocalVirtualThermHygroMeter: public VirtualThermHygroMeter, 
  public Z2S_Core {

  public:

    LocalVirtualThermHygroMeter();

    void iterateAlways() override;

  private:

    uint8_t _last_source_channel = 0xFF;
    uint32_t  _last_refresh_ms = 0;
    Supla::Sensor::VirtualThermHygroMeter *_source_virtual_therm_hygro_meter =
      nullptr;
};

/*****************************************************************************/

class CalculatedThermometer : public VirtualThermometerWithActions, 
  public Z2S_Core {

  public:

    CalculatedThermometer();

    void onInit() override;
    void iterateAlways() override;
    void handleAction(int event, int action) override;

    bool readSourceParameterValue();
    static void updateLinkedThermometers(uint8_t src_channel_number);

  protected:

    void validateSourceChannel();
    bool isActionDrivenFunc();

  private:

    uint8_t _last_source_channel = 0xFF;
    uint8_t _last_channel_func = 0xFF;
    bool _initial_read_done = false;
    uint32_t _last_refresh_ms = 0;
    Supla::Element *_source_Supla_element = nullptr;
};
/*****************************************************************************/

class LocalVirtualGPM 
  : public VirtualGeneralPurposeMeasurement, public Z2S_Core {

  public:

    LocalVirtualGPM() 
      : VirtualGeneralPurposeMeasurement(), Z2S_Core(this) {
    } 


    void handleAction(int event, int action) override;
    void iterateAlways() override;

  protected:
    
};

/*****************************************************************************/

class CalculatedGPM : public VirtualGeneralPurposeMeasurement, 
  public Z2S_Core {

  public:

    CalculatedGPM();

    void configureFunctionDefaults();
    void resetControllerState();
    void setControllerEnabled(bool enabled);
    bool isControllerEnabled() const { return _controller_enabled; }
    bool isControlAlgorithmFunc();
    const char *getFunctionName();

    void iterateAlways() override;
    void handleAction(int event, int action) override;

  protected:

    void validateSourceChannel();
    void validateDestinationChannel();
    double computeLinearMap(double temp);
    double computePID(double temp, float dt, bool use_derivative);
    void commitPwmOutput(double out_pct);

  private:

    uint8_t _last_source_channel = 0xFF;
    uint8_t _last_destination_channel = 0xFF;
    uint8_t _last_channel_func = 0xFF;
    uint32_t _last_refresh_ms = 0;
    uint32_t _last_pid_calc_ms = 0;
    
    Supla::Element *_source_Supla_element = nullptr;
    Supla::Element *_destination_Supla_element = nullptr;

    float _integral = 0.0f;
    float _prev_temp = NAN;
    float _d_filtered = 0.0f;
    uint8_t _last_pwm_duty = 0xFF;
    bool _controller_enabled = true;
};

/*****************************************************************************/

class LocalHCSR04: public HC_SR04_GPM, public Z2S_Core {

  public:

    LocalHCSR04() : HC_SR04_GPM(5, 6, 0, 500, 0, 500), Z2S_Core(this) {

    }

    virtual ~LocalHCSR04() {

    }

    void setTrigPin(int8_t trig_pin) {

      _trigPin = trig_pin;
      onInit();
    }

    void setEchoPin(int8_t echo_pin) {

      _echoPin = echo_pin;
      onInit();
    }

    void setMinIn(int16_t min_in) {

      _minIn = min_in;
    }

    void setMinOut(int16_t min_out) {

      _minOut = min_out;
    }

    void setMaxIn(int16_t max_in) {

      _maxIn = max_in;
    }

    void setMaxOut(int16_t max_out) {

      _maxOut = max_out;
    }

    int8_t getTrigPin() {

      return _trigPin;
    }

    int8_t getEchoPin() {

      return _echoPin;
    }

    int16_t getMinIn() {

      return _minIn;
    }

    int16_t getMinOut() {

      return _minOut;
    }

    int16_t getMaxIn() {

      return _maxIn;
    }

    int16_t getMaxOut() {

      return _maxOut;
    }
};

/*****************************************************************************/

class LocalDS18B20: public DS18B20, public Z2S_Core {

  public:

    LocalDS18B20(uint8_t gpio_pin, uint8_t *sensor_address);
    virtual ~LocalDS18B20() {}
};

/*****************************************************************************/

class LocalBinary: public Binary, public Z2S_Core {

  public:

    LocalBinary(); 
    virtual ~LocalBinary() {}

    void setPin(int pin);
    void setInvertLogic(bool invertLogic);
    void setPullUp(bool pullUp);

    int getPin();
    bool getInvertLogic();
    bool getPullUp();
};

/*****************************************************************************/

class LocalSHT3x : public SHT3x, public Z2S_Core {
 
 public:
  
  LocalSHT3x() : SHT3x(0x44), Z2S_Core(this) {}

  void onInit() override {

    if (ensureLocalI2C(getLocalSHT3xSdaPin(), getLocalSHT3xSclPin())) {
      Supla::Sensor::SHT3x::onInit();
    }
  }

  void setAddress(int8_t addr) {
    
    address = addr;
    if (ensureLocalI2C(getLocalSHT3xSdaPin(), getLocalSHT3xSclPin())) {
      sht.begin(address);
    }
  }

  double getTemp() override {
  
    if (!ensureLocalI2C(getLocalSHT3xSdaPin(), getLocalSHT3xSclPin())) {
      return TEMPERATURE_NOT_AVAILABLE;
    }
    return Supla::Sensor::SHT3x::getTemp();
  }
};

/*****************************************************************************/

class LocalSHT4x : public SHT4x, public Z2S_Core {

 public:

  LocalSHT4x() : SHT4x(0x44), Z2S_Core(this) {}

  void onInit() override {

    if (ensureLocalI2C(getLocalSHT4xSdaPin(), getLocalSHT4xSclPin())) {
      uint8_t addr = getLocalSHT4xAddress();
      if (addr < 0x44 || addr > 0x46) {
        addr = 0x44;
      }
      address = addr;
      sht = ::SHT4x(address, &Wire);
      Supla::Sensor::SHT4x::onInit();
    }
  }

  void setAddress(int8_t addr) {

    if (addr < 0x44 || addr > 0x46) {
      addr = 0x44;
    }
    address = addr;
    if (ensureLocalI2C(getLocalSHT4xSdaPin(), getLocalSHT4xSclPin())) {
      sht = ::SHT4x(address, &Wire);
      sht.begin();
    }
  }

  double getTemp() override {

    if (!ensureLocalI2C(getLocalSHT4xSdaPin(), getLocalSHT4xSclPin())) {
      humidity = HUMIDITY_NOT_AVAILABLE;
      return TEMPERATURE_NOT_AVAILABLE;
    }
    return Supla::Sensor::SHT4x::getTemp();
  }
};

/*****************************************************************************/

class LocalBH1750 : public Bh1750, public Z2S_Core {

 public:

  LocalBH1750() : Bh1750(0x23), Z2S_Core(this) {}

  bool initBH1750() {

    if (!ensureLocalI2C(getLocalBH1750SdaPin(), getLocalBH1750SclPin())) {
      return false;
    }
    uint8_t addr = getLocalBH1750Address();
    if (addr != 0x23 && addr != 0x5C) {
      addr = 0x23;
    }
    if (sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, addr, &Wire)) {
      SUPLA_LOG_DEBUG("BH1750[%d]: initialized (addr 0x%02X)",
                      getChannelNumber(), addr);
      return true;
    }
    SUPLA_LOG_WARNING("BH1750[%d]: not found at 0x%02X",
                      getChannelNumber(), addr);
    return false;
  }

  void onInit() override {

    if (initBH1750()) {
      channel.setNewValue(getValue());
    }
  }

  void setAddress(int8_t addr) {

    (void)addr;
    if (initBH1750()) {
      channel.setNewValue(getValue());
    }
  }

  double getValue() override {

    if (!ensureLocalI2C(getLocalBH1750SdaPin(), getLocalBH1750SclPin())) {
      return NAN;
    }
    double value = sensor.readLightLevel();
    if (isnan(value) || value < 0) {
      invalidCounter++;
      if (invalidCounter < 4) {
        return lastValue;
      } else {
        invalidCounter = 0;
        initBH1750();
      }
      lastValue = NAN;
      return lastValue;
    }
    invalidCounter = 0;
    lastValue = value;
    return value;
  }
};

/*****************************************************************************/

class LocalADC : public VirtualGeneralPurposeMeasurement, public Z2S_Core {
 public:
  LocalADC() : VirtualGeneralPurposeMeasurement(), Z2S_Core(this) {}

  void iterateAlways() override {
    VirtualGeneralPurposeMeasurement::iterateAlways();
    
    if (millis() - _last_read_time > 1000) {
      _last_read_time = millis();
      
      if (getLocalAdcPin() != 0xFF) {

        analogSetPinAttenuation(
          getLocalAdcPin(), (adc_attenuation_t)getLocalAdcAttenuation());
        setValue(analogReadMilliVolts(getLocalAdcPin()));
      }
    }
  }

 private:
  uint32_t _last_read_time = 0;
};

/*****************************************************************************/

class HttpBinary: public VirtualBinary, public Z2S_Core {

  public:

    HttpBinary(bool keepStateInStorage = true);
    virtual ~HttpBinary();

    void handleAction(int event, int action) override;
    void iterateAlways() override;

    bool sendHttpTrigger();

  private:

    uint32_t _last_http_send_ms = 0;
};

/*****************************************************************************/

}; //namespace Sensor
};  // namespace Supla

/*****************************************************************************/

extern Supla::GatewayEvents GatewayEventsInstance;
#endif
