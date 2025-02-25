/* credits @rafalekkalwak@wp.pl */

#ifndef SRC_SUPLA_CONTROL_VIRTUAL_RELAY_SCENE_SWITCH_H_
#define SRC_SUPLA_CONTROL_VIRTUAL_RELAY_SCENE_SWITCH_H_

#include "supla/control/virtual_relay.h"

namespace Supla
{
    namespace Control
    {
        class VirtualRelaySceneSwitch : public VirtualRelay
        {
        public:
            /// @brief Creates Virtual Relay channel with deboucing capabilities
            /// @param functions Supla channel functions.
            /// @param debounceTimeMs Debounce time in milliseconds.
            VirtualRelaySceneSwitch(_supla_int_t functions =
                                        (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER),
                                    u_int16_t debounceTimeMs=100);

            
            bool getValue();

	    void onLoadState() override;
	    void onSaveState() override;

            void onInit() override;

	    void turnOn(_supla_int_t duration = 0) override;
            void turnOff(_supla_int_t duration = 0) override;

        protected:
            unsigned long _lastChangeTime = 0;
            u_int16_t _debounceTime = 0;
        };

    }; // namespace Control
}; // namespace Supla

#endif //SRC_SUPLA_CONTROL_VIRTUAL_RELAY_SCENE_SWITCH_H_