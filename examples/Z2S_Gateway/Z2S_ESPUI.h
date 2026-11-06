#ifndef Z2S_ESPUI_H_
#define Z2S_ESPUI_H_

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Maximum number of UI controls + static options across all tabs
#ifndef Z2S_MAX_CONTROLS
#define Z2S_MAX_CONTROLS 420
#endif

// Callback event types matching ESPUI
#define B_DOWN     -1
#define B_UP        1
#define S_ACTIVE    2
#define S_INACTIVE -2
#define SL_VALUE    3
#define N_VALUE     4
#define T_VALUE     5
#define S_VALUE     6

// Control flags matching custom ESPUI extensions
#define CONTROL_FLAG_TEXTAREA 0x01
#define CONTROL_FLAG_PASSWORD 0x02

// Dirty flags for coalesced WebSocket batching
#define Z2S_DIRTY_VAL 0x01
#define Z2S_DIRTY_EN  0x02
#define Z2S_DIRTY_LBL 0x04
#define Z2S_DIRTY_PS  0x08
#define Z2S_DIRTY_ES  0x10

namespace Control {
  using ControlId_t = uint16_t;
  static constexpr uint16_t noParent = 0xFFFF;

  enum class Type : uint8_t {
    Tab = 0,
    Separator,
    Label,
    Button,
    Switcher,
    Number,
    Text,
    Select,
    Option
  };

  enum class Color : uint8_t {
    Turquoise = 0,
    Emerald,
    Peterriver,
    Wetasphalt,
    Sunflower,
    Carrot,
    Alizarin,
    Dark,
    None
  };
}

class BasicControl {
public:
  using Callback_t = void (*)(BasicControl *sender, int type, void *param);

  uint16_t id = 0xFFFF;
  uint16_t parent_id = Control::noParent;
  Control::Type type = Control::Type::Label;
  Control::Color color = Control::Color::Dark;

  uint8_t control_flags = 0;
  uint8_t dirty_flags = 0;
  uint8_t rows = 1;
  uint8_t cols = 32;
  bool enabled : 1;
  bool wide_panel : 1;
  bool label_is_heap : 1;

  const char *label = "";
  const char *panel_style = "";
  const char *element_style = "";

  Callback_t callback = nullptr;
  void *user_data = nullptr;

  int32_t int_val = 0;
  char *str_buf = nullptr;
  uint16_t str_cap = 0;
  const char *static_str_val = nullptr;

  BasicControl() : enabled(true), wide_panel(false), label_is_heap(false) {}

  uint16_t GetId() const { return id; }

  void setUserData(void *data) { user_data = data; }
  void *getUserData() const { return user_data; }

  void reserve(uint16_t cap) {
    if (cap == 0) return;
    if (cap <= str_cap && str_buf != nullptr) return;

    char *new_buf = static_cast<char *>(realloc(str_buf, cap + 1));
    if (new_buf) {
      if (str_buf == nullptr) {
        if (static_str_val) {
          strncpy(new_buf, static_str_val, cap);
          new_buf[cap] = '\0';
        } else {
          new_buf[0] = '\0';
        }
      }
      str_buf = new_buf;
      str_cap = cap;
    }
  }

  void setStringValue(const char *val) {
    if (!val) val = "";
    size_t len = strlen(val);

    if (str_buf == nullptr || len > str_cap) {
      uint16_t new_cap = (len < 32) ? 32 : (uint16_t)len;
      reserve(new_cap);
    }
    if (str_buf) {
      strncpy(str_buf, val, str_cap);
      str_buf[str_cap] = '\0';
    } else {
      static_str_val = val;
    }
    int_val = strtol(val, nullptr, 10);
  }

  void setIntValue(int32_t val) {
    int_val = val;
    if (str_buf && str_cap > 0) {
      snprintf(str_buf, str_cap + 1, "%ld", (long)val);
    } else {
      static_str_val = nullptr;
    }
  }

  void setLabel(const char *new_label, bool copy_to_heap = false) {
    if (!new_label) new_label = "";
    if (copy_to_heap) {
      size_t len = strlen(new_label);
      char *copy = static_cast<char *>(malloc(len + 1));
      if (copy) {
        memcpy(copy, new_label, len + 1);
        if (label_is_heap && label) free(const_cast<char *>(label));
        label = copy;
        label_is_heap = true;
      }
    } else {
      if (label_is_heap && label) free(const_cast<char *>(label));
      label = new_label;
      label_is_heap = false;
    }
  }

  int32_t getValueInt() const {
    if (type == Control::Type::Number || type == Control::Type::Switcher ||
        type == Control::Type::Select || type == Control::Type::Option) {
      return int_val;
    }
    const char *s = getValueCstr();
    return (s && s[0] != '\0') ? strtol(s, nullptr, 10) : int_val;
  }

  const char *getValueCstr() const {
    if (str_buf) return str_buf;
    if (static_str_val) return static_str_val;
    return "";
  }

  String getValue() const {
    if (str_buf) return String(str_buf);
    if (static_str_val) return String(static_str_val);
    return String(int_val);
  }
};

class Z2S_ESPUIClass {
public:
  bool sliderContinuous = false;
  bool captivePortal = false;

  using WsConnectCallback_t = void (*)();
  using WsCustomCmdCallback_t = void (*)(uint16_t);

  void begin(const char *title, const char *username = nullptr,
             const char *password = nullptr, uint16_t port = 80);

  AsyncWebServer *WebServer() { return _server; }
  AsyncWebSocket *WebSocket() { return _ws; }

  void setOnWsConnectCallback(WsConnectCallback_t cb) { _on_ws_connect = cb; }
  void setOnWsCustomCmdCallback(WsCustomCmdCallback_t cb) { _on_ws_custom_cmd = cb; }

  uint16_t addControl(Control::Type type, const char *label, const char *value = "",
                      Control::Color color = Control::Color::Dark,
                      uint16_t parent = Control::noParent,
                      BasicControl::Callback_t callback = nullptr,
                      void *user_data = nullptr);

  uint16_t addControl(Control::Type type, const char *label, const String &value,
                      Control::Color color = Control::Color::Dark,
                      uint16_t parent = Control::noParent,
                      BasicControl::Callback_t callback = nullptr,
                      void *user_data = nullptr);

  uint16_t addControl(Control::Type type, const char *label, long int value,
                      Control::Color color = Control::Color::Dark,
                      uint16_t parent = Control::noParent,
                      BasicControl::Callback_t callback = nullptr,
                      void *user_data = nullptr);

  BasicControl *getControl(uint16_t id);

  void updateControlValue(uint16_t id, const char *value);
  void updateControlValue(uint16_t id, const String &value);
  void updateControlValue(uint16_t id, long int value);

  void updateLabel(uint16_t id, const char *value)   { updateControlValue(id, value); }
  void updateLabel(uint16_t id, const String &value) { updateControlValue(id, value); }

  void updateText(uint16_t id, const char *value)    { updateControlValue(id, value); }
  void updateText(uint16_t id, const String &value)  { updateControlValue(id, value); }

  void updateNumber(uint16_t id, long int value)     { updateControlValue(id, value); }
  void updateSelect(uint16_t id, long int value)     { updateControlValue(id, value); }
  void updateSwitcher(uint16_t id, bool value)       { updateControlValue(id, (long)value); }

  void updateControlLabel(uint16_t id, const char *label);

  void setEnabled(uint16_t id, bool enable);
  void setPanelStyle(uint16_t id, const char *style);
  void setElementStyle(uint16_t id, const char *style);
  void setPanelWide(uint16_t id, bool wide);
  void setInputType(uint16_t id, const char *input_type);

  // Flush coalesced control updates in batched WebSocket frames
  void flush(bool allow_wait = true);
  void loop();
  bool wsTextAll(const char *payload, size_t len, bool allow_wait = true);

  void _handleWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len);
  size_t _fillUiLayoutChunk(uint8_t *buffer, size_t maxLen, size_t &ctrl_idx);

private:
  const char *_ui_title = "Z2S Control Panel";
  AsyncWebServer *_server = nullptr;
  AsyncWebSocket *_ws = nullptr;

  BasicControl _controls[Z2S_MAX_CONTROLS];
  BasicControl _dummy_ctrl;
  uint16_t _control_count = 0;
  volatile bool _has_dirty_controls = false;
  volatile bool _in_ws_event = false;
  uint32_t _last_loop_ms = 0;

  WsConnectCallback_t _on_ws_connect = nullptr;
  WsCustomCmdCallback_t _on_ws_custom_cmd = nullptr;

  void _markDirty(BasicControl *ctrl, uint8_t flag);
};

extern Z2S_ESPUIClass ESPUI;

#endif // Z2S_ESPUI_H_