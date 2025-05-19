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

#ifndef SRC_SUPLA_CONTROL_DIMMER_INPUT_INTERFACE_H_
#define SRC_SUPLA_CONTROL_DIMMER_INPUT_INTERFACE_H_

#include <supla/action_handler.h>
#include <supla/actions.h>
#include <supla/channel_element.h>
#include "ZigbeeGateway.h"


namespace Supla {
namespace Control {
class DimmerInputInterface : public ChannelElement, public ActionHandler {

public:

  DimmerInputInterface(ZigbeeGateway *gateway, zbg_device_params_t *device, uint8_t dimmer_mode);

  int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;

  void fillSuplaChannelNewValue(TSD_SuplaChannelNewValue *value) override;

  //void onInit() override;
  void iterateAlways() override;

  virtual void handleAction(int event, int action) override {};

  virtual void sendValueToDevice(uint8_t brightness); //= 0;

  virtual void setValueOnServer(uint16_t value);

  //virtual void setStateOnServer(bool state);

  virtual void ping();

  void setDimmerMode(uint8_t dimmer_mode);
  uint8_t getDimmerMode();


protected:

  ZigbeeGateway *_gateway = nullptr;
  zbg_device_params_t 	_device;
  uint8_t _dimmer_mode; //= Z2S_SEND_TO_LEVEL_DIMMER;

  uint8_t _last_brightness = 0;
  uint8_t _brightness = 0;

  uint32_t _lastMsgReceivedMs = 0;

}; //DimmerInputInterface

} //Control
}//Supla

#endif
