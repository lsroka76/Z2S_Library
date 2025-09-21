#include "z2s_device_iaszone.h"
#include <supla/device/notifications.h>

void initZ2SDeviceIASzone(int16_t channel_number_slot) {
  
  //uint8_t timeout = z2s_channels_table[channel_number_slot].timeout_secs / 3600;
  
  auto Supla_Z2S_VirtualBinary = new Supla::Sensor::Z2S_VirtualBinary(true);
  
  Supla_Z2S_VirtualBinary->getChannel()->setChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (strlen(z2s_channels_table[channel_number_slot].Supla_channel_name) > 0) 
    Supla_Z2S_VirtualBinary->setInitialCaption(z2s_channels_table[channel_number_slot].Supla_channel_name);
  
  if (z2s_channels_table[channel_number_slot].Supla_channel_func != 0) 
    Supla_Z2S_VirtualBinary->setDefaultFunction(z2s_channels_table[channel_number_slot].Supla_channel_func);

  Supla_Z2S_VirtualBinary->setAutoSetSecs(z2s_channels_table[channel_number_slot].refresh_secs);

  if (z2s_channels_table[channel_number_slot].user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
    Supla_Z2S_VirtualBinary->getChannel()->setStateOfflineRemoteWakeupNotSupported();
    Supla_Z2S_VirtualBinary->setRWNSFlag(true);
  }

  Supla_Z2S_VirtualBinary->setTimeoutSecs(z2s_channels_table[channel_number_slot].timeout_secs);

  Supla::Notification::RegisterNotification(z2s_channels_table[channel_number_slot].Supla_channel, false, true);
}

void addZ2SDeviceIASzone(zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, char *name, uint32_t func) {
  
  auto Supla_Z2S_VirtualBinary = new Supla::Sensor::Z2S_VirtualBinary(true);

  Z2S_fillChannelsTableSlot(device, free_slot, 
                            Supla_Z2S_VirtualBinary->getChannelNumber(), 
                            SUPLA_CHANNELTYPE_BINARYSENSOR, 
                            sub_id, name, func);

  if (name == nullptr)
    name = (char*)default_vb_name;

  Supla_Z2S_VirtualBinary->setInitialCaption(name);
  
  if (func !=0) 
    Supla_Z2S_VirtualBinary->setDefaultFunction(func);  
}

void msgZ2SDeviceIASzone(int16_t channel_number_slot, bool state, bool check_flags) {

  if (channel_number_slot < 0) {
    log_e("msgZ2SDeviceIASzone - invalid channel number slot");
    return;
  }

  if (check_flags && (z2s_channels_table[channel_number_slot].user_data_flags & USER_DATA_FLAG_MSG_DISABLED)) {
    log_e("msgZ2SDeviceIASzone - USER_DATA_FLAG_MSG_DISABLED set, no message is sent");
    return;
  }

  Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);

  if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

        auto Supla_Z2S_VirtualBinary = reinterpret_cast<Supla::Sensor::Z2S_VirtualBinary *>(element);
        
        Supla_Z2S_VirtualBinary->Refresh();

        if (state) Supla_Z2S_VirtualBinary->extClear(); 
        else Supla_Z2S_VirtualBinary->extSet();
              
        if ((sendIASNotifications) && 
            ~(z2s_channels_table[channel_number_slot].user_data_flags & USER_DATA_FLAG_DISABLE_NOTIFICATIONS)) {

          Supla::Notification::SendF(z2s_channels_table[channel_number_slot].Supla_channel, 
                                     z2s_channels_table[channel_number_slot].Supla_channel_name,
                                    "State changed - now is %s", state ? "ON" : "OFF");
        }
    }
}
