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

#include <Z2S_custom_actions_events.h>
#include "hvac_base_ee.h"
#include "Z2S_trv_interface.h"

using Supla::Control::HvacBaseEE;

HvacBaseEE::HvacBaseEE(
  Supla::Control::OutputInterface *primaryOutput,
  Supla::Control::OutputInterface *secondaryOutput) 
    : HvacBase(primaryOutput, secondaryOutput), 
      primaryOutputEE(primaryOutput) {}

HvacBaseEE::~HvacBaseEE() {}

int32_t HvacBaseEE::handleNewValueFromServer(
  TSD_SuplaChannelNewValue *newValue)
{
  
  int32_t handleNewValueFromServer_result = 
    HvacBase::handleNewValueFromServer(newValue);
  
  return handleNewValueFromServer_result; 
}

Supla::Control::OutputInterface *HvacBaseEE::getPrimaryOutputEE() {

  return primaryOutputEE;
}

void HvacBaseEE::setPrimaryOutputEE(Supla::Control::OutputInterface *output) {

  primaryOutputEE = output;
}

void HvacBaseEE::handleAction(int event, int action) {
  (void)(event);
  switch (action) {
  

    case Z2S_SUPLA_ACTION_TURN_ON_MAX_TEMPERATURE:

      applyNewRuntimeSettings(
        SUPLA_HVAC_MODE_HEAT, getDefaultTemperatureRoomMax(), 0, 0);
    break;


    case Z2S_SUPLA_ACTION_TURN_ON_MIN_TEMPERATURE:

      applyNewRuntimeSettings(
        SUPLA_HVAC_MODE_HEAT, getDefaultTemperatureRoomMin(), 0, 0);
    break;


    case Z2S_SUPLA_ACTION_TURN_OFF_BACKLIGHT: {

      auto Supla_Z2S_TRVInterface = static_cast<
        Supla::Control::Z2S_TRVInterface *>(primaryOutputEE);
      
      Supla_Z2S_TRVInterface->sendTRVBacklight(0);
    } break;

    case Z2S_SUPLA_ACTION_TURN_ON_BACKLIGHT: {

      auto Supla_Z2S_TRVInterface = static_cast<
        Supla::Control::Z2S_TRVInterface *>(primaryOutputEE);
      
      Supla_Z2S_TRVInterface->sendTRVBacklight(3);
    } break;


    case Z2S_SUPLA_ACTION_TURN_ON_CHILD_LOCK: {

      setLocalUILock((LocalUILock)1);
      
      auto Supla_Z2S_TRVInterface = static_cast<
        Supla::Control::Z2S_TRVInterface *>(primaryOutputEE);
      
      Supla_Z2S_TRVInterface->sendTRVChildLock(1);
    } break;


    case Z2S_SUPLA_ACTION_TURN_OFF_CHILD_LOCK: {

      setLocalUILock((LocalUILock)0);
      
      auto Supla_Z2S_TRVInterface = static_cast<
        Supla::Control::Z2S_TRVInterface *>(primaryOutputEE);
      
      Supla_Z2S_TRVInterface->sendTRVChildLock(0);
    } break;


    default:

      HvacBase::handleAction(event, action);
    break;
  }
}