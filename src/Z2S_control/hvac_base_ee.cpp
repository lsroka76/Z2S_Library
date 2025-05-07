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

#include <supla/events.h>
#include "hvac_base_ee.h"

using Supla::Control::HvacBaseEE;

HvacBaseEE::HvacBaseEE(Supla::Control::OutputInterface *primaryOutput,
                   Supla::Control::OutputInterface *secondaryOutput) : HvacBase(primaryOutput, secondaryOutput) {}

HvacBaseEE::~HvacBaseEE() {}

int32_t HvacBaseEE::handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue)
{
  /*auto hvacValue = reinterpret_cast<THVACValue *>(newValue->value);

  int32_t temperature_setpoint_div = (hvacValue->SetpointTemperatureHeat / 100) * 100;
  int32_t temperature_setpoint_mod = hvacValue->SetpointTemperatureHeat % 100;
  
  if ((temperature_setpoint_mod >= 0) && (temperature_setpoint_mod < 25))
    hvacValue->SetpointTemperatureHeat = temperature_setpoint_div;
  else
  if ((temperature_setpoint_mod >= 25) && (temperature_setpoint_mod < 75))
    hvacValue->SetpointTemperatureHeat = temperature_setpoint_div + 50;
  else
  if (temperature_setpoint_mod >= 75)
    hvacValue->SetpointTemperatureHeat = temperature_setpoint_div + 100;
  */  
  //log_i("HvacBaseEE::handleNewValueFromServer: hvacValue->SetpointTemperatureHeat (adjusted) = %d", hvacValue->SetpointTemperatureHeat);

  int32_t handleNewValueFromServer_result = HvacBase::handleNewValueFromServer(newValue);
  if (handleNewValueFromServer_result)
    runAction(Supla::ON_CHANGE);
  
  return handleNewValueFromServer_result; 
}

Supla::Control::OutputInterface *HvacBaseEE::getPrimaryOutputEE() {

  return primaryOutputEE;
}

void HvacBaseEE::setPrimaryOutputEE(Supla::Control::OutputInterface *output) {

  primaryOutputEE = output;
}
