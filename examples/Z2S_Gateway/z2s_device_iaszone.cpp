#include "z2s_device_iaszone.h"
#include <supla/device/notifications.h>



/*****************************************************************************/

void initZ2SDeviceIASzone(int16_t channel_number_slot) {

  initZ2SDeviceIASzone(
    channel_number_slot, z2s_channels_table + channel_number_slot);
}

/*****************************************************************************/

void initZ2SDeviceIASzone(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel) {
  
  auto Supla_Z2S_VirtualBinary = new Supla::Sensor::Z2S_VirtualBinary(true);
  
  Supla_Z2S_VirtualBinary->getChannel()->setChannelNumber(
    _z2s_channel->Supla_channel);

  Supla_Z2S_VirtualBinary->setZ2SZbDevice(Z2S_getZbDevicePtr(
    _z2s_channel->Zb_device_id));

  Supla_Z2S_VirtualBinary->setZ2SChannel(channel_index, _z2s_channel);

  if (strlen(_z2s_channel->Supla_channel_name) > 0) 
    Supla_Z2S_VirtualBinary->setInitialCaption(
      _z2s_channel->Supla_channel_name);
  
  if (_z2s_channel->Supla_channel_func != 0) 
    Supla_Z2S_VirtualBinary->setDefaultFunction(
      _z2s_channel->Supla_channel_func);

  Supla_Z2S_VirtualBinary->setAutoSetSecs(_z2s_channel->refresh_secs);

  if (_z2s_channel->user_data_flags & USER_DATA_FLAG_SET_SORWNS_ON_START) {
    
    Supla_Z2S_VirtualBinary->getChannel()->\
      setStateOfflineRemoteWakeupNotSupported();
    Supla_Z2S_VirtualBinary->setRWNSFlag(true);
  }

  Supla_Z2S_VirtualBinary->setTimeoutSecs(_z2s_channel->timeout_secs);

  Supla::Notification::RegisterNotification(
    _z2s_channel->Supla_channel, false, true);
}

/*****************************************************************************/

void addZ2SDeviceIASzone(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id, 
  const char *name, uint32_t func) {
  
  auto Supla_Z2S_VirtualBinary = new Supla::Sensor::Z2S_VirtualBinary(true);

  if (name == nullptr)
    name = (char*)default_vb_name;

  Supla_Z2S_VirtualBinary->setInitialCaption(name);
  
  if (func == 0)
    func = SUPLA_CHANNELFNC_BINARY_SENSOR;

  Supla_Z2S_VirtualBinary->setDefaultFunction(func);  

  Z2S_fillChannelsTableSlot(
    device, free_slot, Supla_Z2S_VirtualBinary->getChannelNumber(), 
    SUPLA_CHANNELTYPE_BINARYSENSOR, sub_id, name, func);
}

/*****************************************************************************/

void msgZ2SDeviceIASzone(
  Supla::Element* element, bool state, bool check_flags) {


  auto Supla_Z2S_VirtualBinary = static_cast<
    Supla::Sensor::Z2S_VirtualBinary *>(element);
        
  if (check_flags && (Supla_Z2S_VirtualBinary->getChannelUserDataFlags() & 
      USER_DATA_FLAG_MSG_DISABLED)) {

    log_i("Warning: USER_DATA_FLAG_MSG_DISABLED set, no message is sent");
    return;
  }

  Supla_Z2S_VirtualBinary->setZbDeviceLastSeenMs(millis());

  bool state_changed = (state == !Supla_Z2S_VirtualBinary->getValue()) ?
   false : true;
        
  if (state) 
    Supla_Z2S_VirtualBinary->extClear(); 
  else 
    Supla_Z2S_VirtualBinary->extSet();
              
  if (state_changed && sendIASNotifications && 
      (~(Supla_Z2S_VirtualBinary->getChannelUserDataFlags() & 
      USER_DATA_FLAG_DISABLE_NOTIFICATIONS))) {

    Supla::Notification::SendF(
      Supla_Z2S_VirtualBinary->getChannelNumber(), 
      Supla_Z2S_VirtualBinary->getZ2SChannelName(),
      "State changed - now is %s", state ? "ON" : "OFF");
  }
}

/*****************************************************************************/