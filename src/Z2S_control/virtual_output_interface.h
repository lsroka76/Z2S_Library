/*
 * Copyright (C) AC SOFTWARE SP. Z O.O
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef SRC_SUPLA_CONTROL_VIRTUAL_OUTPUT_INTERFACE_H_
#define SRC_SUPLA_CONTROL_VIRTUAL_OUTPUT_INTERFACE_H_

#include <supla/control/output_interface.h>
#include <supla/local_action.h>
#include <supla/actions.h>

namespace Supla {

namespace Control {

class VirtualOutputInterface: public OutputInterface, public LocalAction {
  
  public:
    VirtualOutputInterface() {}
    virtual ~VirtualOutputInterface() {}
    int getOutputValue() const override {

      return _value;
    }
    void setOutputValue(int value) override {

      _value = value;
      
      if (_value)
        runAction(Supla::TURN_ON);
      else
        runAction(Supla::TURN_OFF);
    }
    bool isOnOffOnly() const override {
      return true;}

    virtual bool isControlledInternally() const { 
      
      return true; }
 
  private:
    int _value = 0; 
};

}  // namespace Control
}  // namespace Supla

#endif  // SRC_SUPLA_CONTROL_VIRTUAL_OUTPUT_INTERFACE_H_
