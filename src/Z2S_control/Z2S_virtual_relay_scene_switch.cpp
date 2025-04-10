#include "Z2S_virtual_relay_scene_switch.h"

#include <supla/log_wrapper.h>
#include <supla/storage/storage.h>

Supla::Control::VirtualRelaySceneSwitch::VirtualRelaySceneSwitch(_supla_int_t functions, uint16_t debounceTimeMs)
    : Supla::Control::VirtualRelay(functions)
{
    _lastChangeTime = millis();
    _debounceTime = debounceTimeMs;
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
}

void Supla::Control::VirtualRelaySceneSwitch::onSaveState() {
  
  Supla::Storage::WriteState((unsigned char *)&state, sizeof(state));
}

void Supla::Control::VirtualRelaySceneSwitch::onLoadState() {
  
  bool value = false;
  
  if (Supla::Storage::ReadState((unsigned char *)&value, sizeof(value)))
    state = value;
}


void Supla::Control::VirtualRelaySceneSwitch::turnOn(_supla_int_t duration)
{
    unsigned long time = millis() - _lastChangeTime;
    if ( time> _debounceTime)
    {
        VirtualRelay::turnOn(duration);
        _lastChangeTime=millis();
         log_i(
      "Relay[%d] turn ON as following time passed (time %d ms)",
      channel.getChannelNumber(),
      time);
    }

}

void Supla::Control::VirtualRelaySceneSwitch::turnOff(_supla_int_t duration)
{
    unsigned long time = millis() - _lastChangeTime;
    if (time > _debounceTime)
    {
        VirtualRelay::turnOff(duration);
        _lastChangeTime=millis();
         log_i(
      "Relay[%d] turn OFF as following time passed (time %d ms)",
      channel.getChannelNumber(),
      time);
    }
}
