#include "Z2S_virtual_relay_scene_switch.h"

#include <supla/log_wrapper.h>

Supla::Control::VirtualRelaySceneSwitch::VirtualRelaySceneSwitch(_supla_int_t functions, u_int16_t debounceTimeMs)
    : Supla::Control::VirtualRelay(functions)
{
    _lastChangeTime = millis();
    _debounceTime = debounceTimeMs;
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

