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

#ifndef VIRTUAL_GENERAL_PURPOSE_MEASUREMENT_H_
#define VIRTUAL_GENERAL_PURPOSE_MEASUREMENT_H_

#include <supla/sensor/general_purpose_channel_base.h>

namespace Supla {
namespace Sensor {

class VirtualGeneralPurposeMeasurement 
  : public Supla::Sensor::GeneralPurposeChannelBase, public ActionHandler {
  
public:
    
  VirtualGeneralPurposeMeasurement() : 
    GeneralPurposeChannelBase(nullptr, false) {

    channel.setType(SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT);
    channel.setDefaultFunction(SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT);
  }

  void virtual setValue(double value) {
    
    _value = value;
  }

  double getValue() {

    return _value;
  }

  void handleAction(int event, int action) override {}

  void iterateAlways() override {
    
    GeneralPurposeChannelBase::iterateAlways();
  }
    
 protected:

  double _value = NAN;
};
};  // namespace Sensor
};  // namespace Supla

#endif  // Z2S_GENERAL_PURPOSE_MEASUREMENT_H_
