#include "Z2S_virtual_relay_scene_switch.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

Supla::Control::VirtualRelaySceneSwitch::VirtualRelaySceneSwitch(_supla_int_t functions, uint32_t debounceTimeMs)
    : Supla::Control::VirtualRelay(functions)
{
    _lastChangeTimeMs = millis();
    _debounceTimeMs   = debounceTimeMs;
}


bool Supla::Control::VirtualRelaySceneSwitch::getValue() {
  return state;
}

void Supla::Control::VirtualRelaySceneSwitch::onInit() {
  uint32_t duration = durationMs;
	
  switch (stateOnInit) {
    case STATE_ON_INIT_ON:
    case STATE_ON_INIT_RESTORED_ON:
      turnOn(duration); break;
    case STATE_ON_INIT_RESTORE:
      channel.setNewValue(getValue()); break;
    default: 
      turnOff(duration); break;
  }
  initDone = true;
}

void Supla::Control::VirtualRelaySceneSwitch::onSaveState() {
  
  Supla::Storage::WriteState((unsigned char *)&state, sizeof(state));
}

void Supla::Control::VirtualRelaySceneSwitch::onLoadState() {
  
  bool value = false;
  
  if (Supla::Storage::ReadState((unsigned char *)&value, sizeof(value)))
    state = value;
}


void Supla::Control::VirtualRelaySceneSwitch::turnOn(_supla_int_t duration) {

  uint32_t timeMs = millis() - _lastChangeTimeMs;

  if ( timeMs > _debounceTimeMs) {

    log_i("durationMs = %lu, storedTurnOnDurationMs = %lu, durationTimestamp = %lu, keepTurnOnDurationMs = %u",
          durationMs, storedTurnOnDurationMs, durationTimestamp, keepTurnOnDurationMs);

    if (keepTurnOnDurationMs || isStaircaseFunction() || isImpulseFunction())
      
      VirtualRelay::turnOn(storedTurnOnDurationMs);

    else

      VirtualRelay::turnOn(duration);

    _lastChangeTimeMs = millis();
    log_i("Relay[%d] turn ON as following time passed (time %d ms)", channel.getChannelNumber(), timeMs);
  }
}

void Supla::Control::VirtualRelaySceneSwitch::turnOff(_supla_int_t duration) {

  uint32_t timeMs = millis() - _lastChangeTimeMs;
  if (timeMs > _debounceTimeMs) {

    VirtualRelay::turnOff(duration);
    _lastChangeTimeMs = millis();
    log_i("Relay[%d] turn OFF as following time passed (time %d ms)", channel.getChannelNumber(), timeMs);
  }
}

void Supla::Control::VirtualRelaySceneSwitch::setDebounceTimeMs(uint32_t debounceTimeMs) {

  _debounceTimeMs = debounceTimeMs;
}
