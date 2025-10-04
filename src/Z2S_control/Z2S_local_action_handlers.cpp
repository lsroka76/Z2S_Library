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

#include "Z2S_local_action_handlers.h"


using Supla::LocalActionHandlerWithTrigger;


LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(uint8_t trigger_value) 
  : _trigger_value(trigger_value) {};

LocalActionHandlerWithTrigger::~LocalActionHandlerWithTrigger() {};

void LocalActionHandlerWithTrigger::handleAction(int event, int action)
{
  //if (actionhandler_callback != nullptr)  actionhandler_callback(event, action);  
}

/*void LocalActionHandlerWithTrigger::LocalActionHandlerWithTrigger(_actionhandler_callback actionhandler_callback)
{
    this->actionhandler_callback = actionhandler_callback;
}*/
//void iterateAlways();