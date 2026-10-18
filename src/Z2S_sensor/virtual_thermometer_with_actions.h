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

#ifndef VIRTUAL_THERMOMETER_WITH_ACTIONS_H_
#define VIRTUAL_THERMOMETER_WITH_ACTIONS_H_

#include <supla/sensor/virtual_thermometer.h>

namespace Supla {
namespace Sensor {

class VirtualThermometerWithActions 
  : public Supla::Sensor::VirtualThermometer, public ActionHandler {
  
public:
    
  void handleAction(int event, int action) override {}
    
};
};  // namespace Sensor
};  // namespace Supla

#endif  // VIRTUAL_THERMOMETER_WITH_ACTIONS_H_
