#ifndef HVAC_BASE_EE_H
#define HVAC_BASE_EE_H

#include <supla/control/hvac_base.h>

namespace Supla
{
  namespace Control
  {
    class HvacBaseEE:public HvacBase
    {
      public:
        explicit HvacBaseEE(Supla::Control::OutputInterface *primaryOutput = nullptr,
                    Supla::Control::OutputInterface *secondaryOutput = nullptr);
        int32_t handleNewValueFromServer(TSD_SuplaChannelNewValue *newValue) override;  
        virtual ~HvacBaseEE();
        
        Supla::Control::OutputInterface *getPrimaryOutputEE();
        void setPrimaryOutputEE(Supla::Control::OutputInterface *output);
      
      private:
        Supla::Control::OutputInterface *primaryOutputEE = nullptr;
    };
  }
}

#endif