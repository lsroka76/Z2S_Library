#include <SuplaDevice.h>

#include <supla/storage/config.h>
#include <supla/storage/storage.h>

#include <supla/control/virtual_relay.h>
#include <supla/sensor/virtual_thermometer.h>

#include <supla/condition.h>

#include <Z2S_control/Z2S_virtual_relay.h>
#include <Z2S_control/Z2S_virtual_relay_scene_switch.h>
#include <Z2S_control/dimmer_input_interface.h>

#include "z2s_devices_table.h"
#include "z2s_device_iaszone.h"
#include "z2s_device_virtual_relay.h"
#include "z2s_device_electricity_meter.h"
#include "z2s_device_hvac.h"
#include "z2s_device_dimmer.h"
#include "z2s_device_rgb.h"
#include "z2s_device_temphumidity.h"
#include "z2s_device_pressure.h"
#include "z2s_device_tuya_custom_cluster.h"
#include "z2s_device_general_purpose_measurement.h"
#include "z2s_device_action_trigger.h"
#include "z2s_device_virtual_valve.h"
#include "z2s_little_fs.h"

extern ZigbeeGateway zbGateway;

z2s_device_params_t    z2s_channels_table[Z2S_CHANNELS_MAX_NUMBER];

z2s_zb_device_params_t z2s_zb_devices_table[Z2S_ZB_DEVICES_MAX_NUMBER];

uint8_t z2s_actions_index_table[Z2S_ACTIONS_MAX_NUMBER / 8];

static uint32_t Styrbar_timer = 0;
static bool     Styrbar_ignore_button_1 = false;

void (*_on_Tuya_custom_cluster_receive)(uint8_t command_id, uint16_t payload_size, uint8_t * payload_data) = nullptr;

void no_channel_found_error_func(char *ieee_addr_str) {

  log_e("No channel found for address %s", ieee_addr_str);
}

void ieee_addr_to_str(char *ieee_addr_str, esp_zb_ieee_addr_t ieee_addr) {

  sprintf_P(ieee_addr_str, PSTR("%x:%x:%x:%x:%x:%x:%x:%x"), ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
          ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
}

void devices_table_full_error_func() {
  log_e("Channels table full");
}

uint32_t Z2S_getChannelsTableSize() {
  uint32_t _z2s_channels_table_size;
  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_CHANNELS_TABLE_SIZE_ID, &_z2s_channels_table_size))
    return _z2s_channels_table_size;
  else
    return 0;
}

uint8_t Z2S_findFirstFreeChannelsTableSlot(uint8_t start_slot) {

  for (uint8_t channels_counter = start_slot; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) 
      if (!z2s_channels_table[channels_counter].valid_record)
        return channels_counter;
  return 0xFF;
  
}

void Z2S_printChannelsTableSlots(bool toTelnet) {
  
  for (uint8_t channels_counter = 0; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) 
    if (z2s_channels_table[channels_counter].valid_record) {
      char log_line[1024];

      sprintf_P(log_line, PSTR("ENTRY\t\t\t%u\n\rIEEE ADDRESS\t\t%X:%X:%X:%X:%X:%X:%X:%X\n\rSHORT ADDRESS\t\t0x%X\n\rENDPOINT\t\t0x%X\n\rCLUSTER\t\t\t0x%X\n\r"
            "MODEL\t\t\t%X\n\r"
            "SUPLA CHANNEL\t\t%u\n\rSUPLA SECONDARY CHANNEL\t%u\n\rSUPLA CHANNEL TYPE\t%ld\n\r"
            "SUPLA CHANNEL NAME\t%s\n\rSUPLA CHANNEL FUNCTION\t%lu\n\r"
            "SUB ID\t\t\t%d\n\rUSER FLAGS\t\t%lu\n\r"
            "USER DATA(1)\t\t%lu\n\rUSER DATA(2)\t\t%lu\n\rUSER DATA(3)\t\t%lu\n\rUSER DATA(4)\t\t%lu\n\r"
            "KEEP ALIVE(S)\t\t%lu\n\rTIMEOUT(S)\t\t%lu\n\rREFRESH(S)\t\t%lu\n\rZB device id\t\t%u\n\r"),
        channels_counter,
        z2s_channels_table[channels_counter].ieee_addr[7], z2s_channels_table[channels_counter].ieee_addr[6], 
        z2s_channels_table[channels_counter].ieee_addr[5], z2s_channels_table[channels_counter].ieee_addr[4], 
        z2s_channels_table[channels_counter].ieee_addr[3], z2s_channels_table[channels_counter].ieee_addr[2], 
        z2s_channels_table[channels_counter].ieee_addr[1], z2s_channels_table[channels_counter].ieee_addr[0],
        z2s_channels_table[channels_counter].short_addr,
        z2s_channels_table[channels_counter].endpoint,
        z2s_channels_table[channels_counter].cluster_id,
        z2s_channels_table[channels_counter].model_id,
        z2s_channels_table[channels_counter].Supla_channel,
        z2s_channels_table[channels_counter].Supla_secondary_channel,
        z2s_channels_table[channels_counter].Supla_channel_type,
        z2s_channels_table[channels_counter].Supla_channel_name,
        z2s_channels_table[channels_counter].Supla_channel_func,
        z2s_channels_table[channels_counter].sub_id,
        z2s_channels_table[channels_counter].user_data_flags,
        z2s_channels_table[channels_counter].user_data_1,
        z2s_channels_table[channels_counter].user_data_2,
        z2s_channels_table[channels_counter].user_data_3,
        z2s_channels_table[channels_counter].user_data_4,
        z2s_channels_table[channels_counter].keep_alive_secs,
        z2s_channels_table[channels_counter].timeout_secs,
        z2s_channels_table[channels_counter].refresh_secs,
        z2s_channels_table[channels_counter].ZB_device_id);
 
      log_i_telnet2(log_line, toTelnet);
    }
}

int16_t Z2S_findChannelNumberSlot(esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, int32_t channel_type, int8_t sub_id) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, channel type 0x%x", ieee_addr_str, endpoint, channel_type);
  
  for (uint8_t channels_counter = 0; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) {
      if (z2s_channels_table[channels_counter].valid_record)
        if ((memcmp(z2s_channels_table[channels_counter].ieee_addr, ieee_addr,8) == 0) && 
        ((endpoint < 0) || (z2s_channels_table[channels_counter].endpoint == endpoint)) &&
        ((channel_type < 0) || (z2s_channels_table[channels_counter].Supla_channel_type == channel_type)) &&
        ((sub_id < 0) || (z2s_channels_table[channels_counter].sub_id == sub_id))) { 
        //&& (z2s_channels_table[channels_counter].cluster_id == cluster)) {
            return channels_counter;
        }

  }  
  return -1;
}

int16_t Z2S_findChannelNumberNextSlot(int16_t prev_slot, esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, 
                                      int32_t channel_type, int8_t sub_id) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);
  
  log_i("%s, endpoint 0x%x, channel type 0x%x", ieee_addr_str, endpoint, channel_type);
  
  for (uint8_t channels_counter = prev_slot + 1; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) {
      if (z2s_channels_table[channels_counter].valid_record)
        if ((memcmp(z2s_channels_table[channels_counter].ieee_addr, ieee_addr,8) == 0) && 
        ((endpoint < 0) || (z2s_channels_table[channels_counter].endpoint == endpoint)) &&
        ((channel_type < 0) || (z2s_channels_table[channels_counter].Supla_channel_type == channel_type)) &&
        ((sub_id < 0) || (z2s_channels_table[channels_counter].sub_id == sub_id))) { 
        //&& (z2s_channels_table[channels_counter].cluster_id == cluster)) {
            return channels_counter;
        }
  }  
  return -1;
}

int16_t Z2S_findTableSlotByChannelNumber(uint8_t channel_id) {
  
  for (uint8_t channels_counter = 0; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) {

      if ((z2s_channels_table[channels_counter].valid_record) && (z2s_channels_table[channels_counter].Supla_channel == channel_id))
        return channels_counter;
  }
  return -1;
}

void Z2S_fillChannelsTableSlot(zbg_device_params_t *device, uint8_t slot, uint8_t channel, int32_t channel_type, int8_t sub_id,
                              char *name, uint32_t func, uint8_t secondary_channel) {

  
  memset(&z2s_channels_table[slot], 0, sizeof(z2s_device_params_t));
  z2s_channels_table[slot].valid_record = true;
  memcpy(z2s_channels_table[slot].ieee_addr,device->ieee_addr, sizeof(esp_zb_ieee_addr_t));
  z2s_channels_table[slot].short_addr = device->short_addr;
  z2s_channels_table[slot].model_id = device->model_id;
  z2s_channels_table[slot].endpoint = device->endpoint;
  z2s_channels_table[slot].cluster_id = device->cluster_id;
  z2s_channels_table[slot].Supla_channel = channel;
  z2s_channels_table[slot].Supla_secondary_channel = secondary_channel;
  z2s_channels_table[slot].Supla_channel_type = channel_type;
  z2s_channels_table[slot].sub_id = sub_id; 
  if (name) strcpy(z2s_channels_table[slot].Supla_channel_name, name);
  z2s_channels_table[slot].Supla_channel_func = func;
  z2s_channels_table[slot].ZB_device_id = Z2S_updateZBDeviceTableSlot(z2s_channels_table[slot].ieee_addr, z2s_channels_table[slot].Supla_channel);
  Z2S_saveChannelsTable();
}

bool Z2S_setChannelFlags(int16_t channel_number_slot, uint32_t flags_to_set) {

  if ((channel_number_slot >= 0) && (channel_number_slot < Z2S_CHANNELS_MAX_NUMBER) && z2s_channels_table[channel_number_slot].valid_record) {
    z2s_channels_table[channel_number_slot].user_data_flags |= flags_to_set;;
    if (Z2S_saveChannelsTable()) {
    log_i("Device(channel %d) flags set successfully to %x", z2s_channels_table[channel_number_slot].Supla_channel, z2s_channels_table[channel_number_slot].user_data_flags);
      return true;
    }
    return false;
  }
  else 
    return false;
}

bool Z2S_clearChannelFlags(int16_t channel_number_slot, uint32_t flags_to_clear) {

  if ((channel_number_slot >= 0) && (channel_number_slot < Z2S_CHANNELS_MAX_NUMBER) && z2s_channels_table[channel_number_slot].valid_record) {
    z2s_channels_table[channel_number_slot].user_data_flags &= ~flags_to_clear;
    if (Z2S_saveChannelsTable()) {
    log_i("Device(channel %d) flags cleared successfully to %x", z2s_channels_table[channel_number_slot].Supla_channel, z2s_channels_table[channel_number_slot].user_data_flags);
      return true;
    }
    return false;
  }
  else 
    return false;
}


bool Z2S_loadChannelsTable() {

  
  uint32_t z2s_channels_table_size =  Z2S_getChannelsTableSize(); 

  log_i("Z2S_getChannelsTableSize %d, sizeof(z2s_channels_table) %d, sizeof(z2s_device_params_t) %d, sizeof(bool)%d",
  z2s_channels_table_size, sizeof(z2s_channels_table), sizeof(z2s_device_params_t), sizeof(bool));

  if (z2s_channels_table_size == 0) {

      log_i(" No no channels table found, writing empty one with size %d", sizeof(z2s_channels_table));
      
      memset(z2s_channels_table,0,sizeof(z2s_channels_table));
      
      //if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_CHANNELS_TABLE_ID, (char *)z2s_channels_table, sizeof(z2s_channels_table))) {
      if (!Z2S_saveFile(Z2S_CHANNELS_TABLE_ID_V2, (uint8_t *)z2s_channels_table, sizeof(z2s_channels_table))) {
        log_i ("Channels table write failed!");
        return false;
      }
      else { 
        if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_CHANNELS_TABLE_SIZE_ID, sizeof(z2s_channels_table))) {
          Supla::Storage::ConfigInstance()->commit();
          return true;
        }
        else { 
          log_i ("Channels table size write failed!");
          return false;
        }
      }
  } else
  {
    if (z2s_channels_table_size != sizeof(z2s_channels_table)) {

      if (z2s_channels_table_size == 0x2200) { //legacy 0.4.2
        log_i("Previous version of channels table detected with size 0x%x, trying to upgrade", z2s_channels_table_size);
        z2s_legacy_device_params_t *z2s_devices_legacy_table = (z2s_legacy_device_params_t *)malloc(z2s_channels_table_size);
        if (z2s_devices_legacy_table == nullptr) {
          log_e("Error while allocating memory for legacy table copying");
          return false;
        }
        else {
          //if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_CHANNELS_TABLE_ID, (char *)z2s_devices_legacy_table, z2s_channels_table_size)) {
          if (!Z2S_loadFile(Z2S_CHANNELS_TABLE_ID_V2, (uint8_t *)z2s_devices_legacy_table, z2s_channels_table_size)) {
            log_i ("Legacy channels table load failed!");
            return false;
          }
          for (uint8_t table_index = 0; table_index < Z2S_CHANNELS_MAX_NUMBER; table_index++) {

              z2s_channels_table[table_index].valid_record = (z2s_devices_legacy_table + table_index)->valid_record;
              z2s_channels_table[table_index].model_id = (z2s_devices_legacy_table + table_index)->model_id;
              memcpy(z2s_channels_table[table_index].ieee_addr, (z2s_devices_legacy_table + table_index)->ieee_addr,
                     sizeof(esp_zb_ieee_addr_t));
              z2s_channels_table[table_index].endpoint = (z2s_devices_legacy_table + table_index)->endpoint;
              z2s_channels_table[table_index].cluster_id = (z2s_devices_legacy_table + table_index)->cluster_id;
              z2s_channels_table[table_index].short_addr = (z2s_devices_legacy_table + table_index)->short_addr;
              z2s_channels_table[table_index].Supla_channel = (z2s_devices_legacy_table + table_index)->Supla_channel;
              z2s_channels_table[table_index].Supla_secondary_channel = 0xFF;
              z2s_channels_table[table_index].Supla_channel_type = (z2s_devices_legacy_table + table_index)->Supla_channel_type;
              memcpy(z2s_channels_table[table_index].Supla_channel_name, (z2s_devices_legacy_table + table_index)->Supla_channel_name, 
                     sizeof(z2s_channels_table[table_index].Supla_channel_name));
              z2s_channels_table[table_index].Supla_channel_func = (z2s_devices_legacy_table + table_index)->Supla_channel_func;
              z2s_channels_table[table_index].sub_id = (z2s_devices_legacy_table + table_index)->sub_id;
              z2s_channels_table[table_index].user_data_1 = 0;
              z2s_channels_table[table_index].user_data_2 = 0;
              z2s_channels_table[table_index].user_data_3 = 0;
              z2s_channels_table[table_index].user_data_4 = 0;
              z2s_channels_table[table_index].keep_alive_secs = 0;
              z2s_channels_table[table_index].timeout_secs = 0;
              z2s_channels_table[table_index].refresh_secs = 0;
              z2s_channels_table[table_index].data_counter = 0;
              z2s_channels_table[table_index].ZB_device_id = 0xFF;
            }
          log_i("Channels table upgrade completed - saving new table");
          Z2S_saveChannelsTable();
          Z2S_printChannelsTableSlots();
          free(z2s_devices_legacy_table);
          return true;
        }
      } else
      if (z2s_channels_table_size == 0x2600) { //legacy 0.6.xx

        log_i("Previous version of channels table detected with size 0x%x, trying to upgrade", z2s_channels_table_size);
        z2s_legacy_2_device_params_t *z2s_devices_legacy_2_table = (z2s_legacy_2_device_params_t *)malloc(z2s_channels_table_size);
        if (z2s_devices_legacy_2_table == nullptr) {
          log_e("Error while allocating memory for legacy table copying");
          return false;
        }
        else {

          //if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_CHANNELS_TABLE_ID, (char *)z2s_devices_legacy_2_table, z2s_channels_table_size)) {
          if (!Z2S_loadFile(Z2S_CHANNELS_TABLE_ID_V2, (uint8_t *)z2s_devices_legacy_2_table, z2s_channels_table_size)) {

            log_i ("Legacy channels table load failed!");
            return false;
          }
          for (uint8_t table_index = 0; table_index < Z2S_CHANNELS_MAX_NUMBER; table_index++) {

              z2s_channels_table[table_index].valid_record = (z2s_devices_legacy_2_table + table_index)->valid_record;
              z2s_channels_table[table_index].model_id = (z2s_devices_legacy_2_table + table_index)->model_id;
              memcpy(z2s_channels_table[table_index].ieee_addr, (z2s_devices_legacy_2_table + table_index)->ieee_addr,
                     sizeof(esp_zb_ieee_addr_t));
              z2s_channels_table[table_index].endpoint = (z2s_devices_legacy_2_table + table_index)->endpoint;
              z2s_channels_table[table_index].cluster_id = (z2s_devices_legacy_2_table + table_index)->cluster_id;
              z2s_channels_table[table_index].short_addr = (z2s_devices_legacy_2_table + table_index)->short_addr;
              z2s_channels_table[table_index].Supla_channel = (z2s_devices_legacy_2_table + table_index)->Supla_channel;
              z2s_channels_table[table_index].Supla_secondary_channel = 0xFF;
              z2s_channels_table[table_index].Supla_channel_type = (z2s_devices_legacy_2_table + table_index)->Supla_channel_type;
              memcpy(z2s_channels_table[table_index].Supla_channel_name, (z2s_devices_legacy_2_table + table_index)->Supla_channel_name, 
                     sizeof(z2s_channels_table[table_index].Supla_channel_name));
              z2s_channels_table[table_index].Supla_channel_func = (z2s_devices_legacy_2_table + table_index)->Supla_channel_func;
              z2s_channels_table[table_index].sub_id = (z2s_devices_legacy_2_table + table_index)->sub_id;
              z2s_channels_table[table_index].user_data_flags = 0; //(z2s_devices_legacy_2_table + table_index)->user_data_1;
              z2s_channels_table[table_index].user_data_1 = 0; //(z2s_devices_legacy_2_table + table_index)->user_data_2;
              z2s_channels_table[table_index].user_data_2 = 0; 
              z2s_channels_table[table_index].user_data_3 = 0;
              z2s_channels_table[table_index].user_data_4 = 0;
              z2s_channels_table[table_index].keep_alive_secs = 0;
              z2s_channels_table[table_index].timeout_secs = 0;
              if ((z2s_devices_legacy_2_table + table_index)->user_data_1 & USER_DATA_FLAG_SED_TIMEOUT == USER_DATA_FLAG_SED_TIMEOUT)
                z2s_channels_table[table_index].timeout_secs = (z2s_devices_legacy_2_table + table_index)->user_data_2 * 3600;
              z2s_channels_table[table_index].refresh_secs = 0;
              z2s_channels_table[table_index].data_counter = 0;
              z2s_channels_table[table_index].ZB_device_id = 0xFF;
            }
          log_i("Channels table upgrade completed - saving new table");
          Z2S_saveChannelsTable();
          Z2S_printChannelsTableSlots();
          free(z2s_devices_legacy_2_table);
          return true;
        }
      } else {
        log_i("Channels table size mismatch %d <> %d, no upgrade is possible", z2s_channels_table_size, sizeof(z2s_channels_table));
        return false;
      }
    }
    else {
        //if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_CHANNELS_TABLE_ID, (char *)z2s_channels_table, sizeof(z2s_channels_table))) {
        if (!Z2S_loadFile(Z2S_CHANNELS_TABLE_ID_V2, (uint8_t *)z2s_channels_table, z2s_channels_table_size)) {
          
          log_i ("Channels table load failed!");
          return false;

        } else {

          log_i ("Channels table load success!");
          Z2S_printChannelsTableSlots();
          return true;
        }
    }
  }
}

bool Z2S_saveChannelsTable() {

  //if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_CHANNELS_TABLE_ID, (char *)z2s_channels_table, sizeof(z2s_channels_table))) {
  if (!Z2S_saveFile(Z2S_CHANNELS_TABLE_ID_V2, (uint8_t *)z2s_channels_table, sizeof(z2s_channels_table))) {

    log_i("Channels table write failed!");
    return false;
  }
  else { 
    
    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_CHANNELS_TABLE_SIZE_ID, sizeof(z2s_channels_table))) {

      log_i("Channels table new size(%d) write success!", sizeof(z2s_channels_table));
      Supla::Storage::ConfigInstance()->commit();
      return true;

    } else { 

      log_i("Channels table size write failed!");
      return false;
    }
  }
}

bool Z2S_clearChannelsTable() {

  log_i("Clear channels table");
  memset(z2s_channels_table,0,sizeof(z2s_channels_table));
  return Z2S_saveChannelsTable();
}

/* ZB_DEVICE_FUNCTIONS */

uint8_t Z2S_findFirstFreeZBDevicesTableSlot(uint8_t start_slot) {

  for (uint8_t devices_counter = start_slot; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) 
      if (z2s_zb_devices_table[devices_counter].record_id == 0)
        return devices_counter;
  return 0xFF;  
}

uint8_t Z2S_findZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr) {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) 
      if ((z2s_zb_devices_table[devices_counter].record_id > 0) && 
          (memcmp(z2s_zb_devices_table[devices_counter].ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t)) == 0))
        return devices_counter;
  return 0xFF;  
}

bool  Z2S_hasZBDevice(uint32_t desc_id) {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) 
      if ((z2s_zb_devices_table[devices_counter].record_id > 0) && 
          (z2s_zb_devices_table[devices_counter].desc_id == desc_id))
        return true;
  return false;
}

bool Z2S_removeZBDeviceWithAllChannels(uint8_t zb_device_slot) {

  if (z2s_zb_devices_table[zb_device_slot].record_id > 0) {

    //bool zb_devices_table_save_required = false;
    bool devices_table_save_required = false;

    for (uint8_t channels_counter = 0; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) {
      if ((z2s_channels_table[channels_counter].valid_record) && 
          (memcmp(z2s_channels_table[channels_counter].ieee_addr, 
          z2s_zb_devices_table[zb_device_slot].ieee_addr, 
          sizeof(esp_zb_ieee_addr_t)) == 0)) {

            z2s_channels_table[channels_counter].valid_record = false;
            devices_table_save_required = true;
      }
    }
    bool tables_save_result = false;

    if (devices_table_save_required)
      tables_save_result = Z2S_saveChannelsTable();
    z2s_zb_devices_table[zb_device_slot].record_id = 0;
    tables_save_result = Z2S_saveZBDevicesTable();
    return tables_save_result;
  } else {
    log_e("Invalid ZB devices table slot!");
    return false;
  }
}

void Z2S_initZBDevices(uint32_t init_ms) {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) 
      if (z2s_zb_devices_table[devices_counter].record_id > 0)
        z2s_zb_devices_table[devices_counter].last_seen_ms = init_ms;
}

uint8_t Z2S_updateZBDeviceTableSlot(esp_zb_ieee_addr_t ieee_addr, uint8_t Supla_channel) {

  uint8_t zb_device_slot = Z2S_findZBDeviceTableSlot(ieee_addr);
  uint8_t first_free_channel_slot = 0xFF;
  //bool channel_added = false;

  if (zb_device_slot == 0xFF) {
    log_i("update failed - device not found");
    return zb_device_slot;
  }
  else {
    for(int i = 0; i < MAX_ZB_DEVICE_SUPLA_CHANNELS; i++) {
      if ((i < first_free_channel_slot) && (z2s_zb_devices_table[zb_device_slot].Supla_channels[i] == 0xFF))
        first_free_channel_slot = i;
      if (z2s_zb_devices_table[zb_device_slot].Supla_channels[i] == Supla_channel) 
        return zb_device_slot;
    }    
    if (first_free_channel_slot == 0xFF) {
      log_i("update failed - no free Supla channel slots");
      return 0xFF;
    }  
    z2s_zb_devices_table[zb_device_slot].Supla_channels[first_free_channel_slot] = Supla_channel;
    Z2S_saveZBDevicesTable();
    return zb_device_slot;
  }
}

bool Z2S_addZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr, uint16_t short_addr, char *manufacturer_name, char *model_name, 
                              uint8_t endpoints_count, uint32_t desc_id, uint8_t power_source) {

  uint8_t zb_device_slot = Z2S_findZBDeviceTableSlot(ieee_addr);

  if (zb_device_slot == 0xFF) {
    log_i("New ZB device - adding to the ZB devices table");
    zb_device_slot = Z2S_findFirstFreeZBDevicesTableSlot();
    if (zb_device_slot == 0xFF) {
      log_e("ZB devices full - can't add new one!");
      return false;
    } else {
      memset(&z2s_zb_devices_table[zb_device_slot], 0, sizeof(z2s_zb_device_params_t));
      z2s_zb_devices_table[zb_device_slot].record_id = 1;
      memcpy(z2s_zb_devices_table[zb_device_slot].ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t));
      memcpy(z2s_zb_devices_table[zb_device_slot].manufacturer_name, manufacturer_name, strlen(manufacturer_name));
      memcpy(z2s_zb_devices_table[zb_device_slot].model_name, model_name, strlen(model_name));
      z2s_zb_devices_table[zb_device_slot].short_addr = short_addr;
      z2s_zb_devices_table[zb_device_slot].endpoints_count = endpoints_count;
      z2s_zb_devices_table[zb_device_slot].desc_id = desc_id;
      z2s_zb_devices_table[zb_device_slot].power_source = power_source;
      memset(z2s_zb_devices_table[zb_device_slot].Supla_channels, 0xFF, sizeof(z2s_zb_devices_table[zb_device_slot].Supla_channels));
      Z2S_saveZBDevicesTable();
      return true;
    }
  } else
    log_i("ZB device already in ZB devices table");
    if (z2s_zb_devices_table[zb_device_slot].short_addr != short_addr) {
      z2s_zb_devices_table[zb_device_slot].short_addr = short_addr;
      if (Z2S_saveZBDevicesTable())
        log_i("ZB device short adress updated!");
    }
    return true;
}

void  Z2S_updateZBDeviceLastSeenMs(esp_zb_ieee_addr_t  ieee_addr, uint32_t last_seen_ms){

  uint8_t zb_device_slot = Z2S_findZBDeviceTableSlot(ieee_addr);

  if (zb_device_slot == 0xFF) {
    log_e("Unknown ZB device - update not possible!");
    return;
  } else
    z2s_zb_devices_table[zb_device_slot].last_seen_ms = last_seen_ms;
}

void Z2S_printZBDevicesTableSlots(bool toTelnet) {
  
  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZB_DEVICES_MAX_NUMBER; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {
      char log_line[1024];

      sprintf_P(log_line,PSTR("ENTRY\t\t\t%u\n\rRECORD ID\t\t%u\n\r"
            "MANUFACTURER NAME\t%s\n\rMODEL NAME\t\t%s\n\r"
            "IEEE ADDRESS\t\t%X:%X:%X:%X:%X:%X:%X:%X\n\r"
            "SHORT ADDRESS\t\t0x%X\n\rENDPOINTS COUNT\t\t0x%X\n\rPOWER SOURCE\t\t0x%X\n\r"
            "MODEL\t\t\t%lu\n\r"
            "LAST RSSI\t\t%d\n\rLAST SEEN (MS)\t\t%lu\n\rKEEP ALIVE (MS)\t\t%lu\n\r"
            "TIMEOUT (MS)\t\t%lu\n\rBATTERY (%)\t\t%u\n\rUSER FLAGS\t\t%lu\n\rUSER DATA\t\t%lu\n\r"
            "SUPLA CH[0]\t\t%u\n\rSUPLA CH[1]\t\t%u\n\rSUPLA CH[2]\t\t%u\n\rSUPLA CH[3]\t\t%u\n\r"
            "SUPLA CH[4]\t\t%u\n\rSUPLA CH[5]\t\t%u\n\rSUPLA CH[6]\t\t%u\n\rSUPLA CH[7]\t\t%u\n\r"
            "SUPLA CH[8]\t\t%u\n\rSUPLA CH[9]\t\t%u\n\rSUPLA CH[10]\t\t%u\n\rSUPLA CH[11]\t\t%u\n\r"
            "SUPLA CH[12]\t\t%u\n\rSUPLA CH[13]\t\t%u\n\rSUPLA CH[14]\t\t%u\n\rSUPLA CH[15]\t\t%u\n\r"),
        devices_counter, z2s_zb_devices_table[devices_counter].record_id,
        z2s_zb_devices_table[devices_counter].manufacturer_name, z2s_zb_devices_table[devices_counter].model_name,
        z2s_zb_devices_table[devices_counter].ieee_addr[7], z2s_zb_devices_table[devices_counter].ieee_addr[6], 
        z2s_zb_devices_table[devices_counter].ieee_addr[5], z2s_zb_devices_table[devices_counter].ieee_addr[4], 
        z2s_zb_devices_table[devices_counter].ieee_addr[3], z2s_zb_devices_table[devices_counter].ieee_addr[2], 
        z2s_zb_devices_table[devices_counter].ieee_addr[1], z2s_zb_devices_table[devices_counter].ieee_addr[0],
        z2s_zb_devices_table[devices_counter].short_addr,
        z2s_zb_devices_table[devices_counter].endpoints_count,
        z2s_zb_devices_table[devices_counter].power_source,
        z2s_zb_devices_table[devices_counter].desc_id,
        z2s_zb_devices_table[devices_counter].rssi,
        z2s_zb_devices_table[devices_counter].last_seen_ms,
        z2s_zb_devices_table[devices_counter].keep_alive_ms,
        z2s_zb_devices_table[devices_counter].timeout_ms,
        z2s_zb_devices_table[devices_counter].battery_percentage,
        z2s_zb_devices_table[devices_counter].user_data_1,
        z2s_zb_devices_table[devices_counter].user_data_2,
        z2s_zb_devices_table[devices_counter].Supla_channels[0],
        z2s_zb_devices_table[devices_counter].Supla_channels[1],
        z2s_zb_devices_table[devices_counter].Supla_channels[2],
        z2s_zb_devices_table[devices_counter].Supla_channels[3],
        z2s_zb_devices_table[devices_counter].Supla_channels[4],
        z2s_zb_devices_table[devices_counter].Supla_channels[5],
        z2s_zb_devices_table[devices_counter].Supla_channels[6],
        z2s_zb_devices_table[devices_counter].Supla_channels[7],
        z2s_zb_devices_table[devices_counter].Supla_channels[8],
        z2s_zb_devices_table[devices_counter].Supla_channels[9],
        z2s_zb_devices_table[devices_counter].Supla_channels[10],
        z2s_zb_devices_table[devices_counter].Supla_channels[11],
        z2s_zb_devices_table[devices_counter].Supla_channels[12],
        z2s_zb_devices_table[devices_counter].Supla_channels[13],
        z2s_zb_devices_table[devices_counter].Supla_channels[14],
        z2s_zb_devices_table[devices_counter].Supla_channels[15]);  
        
      log_i_telnet2(log_line, toTelnet);
    }
}

uint32_t Z2S_getZBDevicesTableSize() {
  uint32_t _z2s_zb_devices_table_size;
  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_ZB_DEVICES_TABLE_SIZE, &_z2s_zb_devices_table_size))
    return _z2s_zb_devices_table_size;
  else
    return 0;
}

bool Z2S_loadZBDevicesTable() {

  uint32_t z2s_zb_devices_table_size =  Z2S_getZBDevicesTableSize(); 

  log_i("Z2S_getZBDevicesTableSize %d, sizeof(z2s_zb_devices_table) %d, sizeof(z2s_zb_device_params_t) %d, sizeof(bool)%d",
  z2s_zb_devices_table_size, sizeof(z2s_zb_devices_table), sizeof(z2s_zb_device_params_t), sizeof(bool));

  if (z2s_zb_devices_table_size == 0) {

      log_i(" No zigbee devices table found, writing empty one with size %d", sizeof(z2s_zb_devices_table));
      
      memset(z2s_zb_devices_table, 0, sizeof(z2s_zb_devices_table));
      
      //if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {        
      if (!Z2S_saveFile(Z2S_ZB_DEVICES_TABLE_ID_V2, (uint8_t *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {

        log_i ("Zigbee devices table write failed!");
        return false;
      }
      else { 
        if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_ZB_DEVICES_TABLE_SIZE, sizeof(z2s_zb_devices_table))) {
          Supla::Storage::ConfigInstance()->commit();
          return true;
        }
        else { 
          log_i ("Zigbee devices table size write failed!");
          return false;
        }
      }
  } else {
    if (z2s_zb_devices_table_size != sizeof(z2s_zb_devices_table)) {

      if (z2s_zb_devices_table_size == 0x1100) { //legacy 0.7.10

        log_i("Previous version of Zigbee devices table detected with size 0x%x, trying to upgrade", z2s_zb_devices_table_size);

        z2s_legacy_zb_device_params_t *z2s_zb_devices_legacy_table = (z2s_legacy_zb_device_params_t *)malloc(z2s_zb_devices_table_size);

        if (z2s_zb_devices_legacy_table == nullptr) {

          log_e("Error while allocating memory for Zigbee devices legacy table copying");
          return false;

        } else {
          
          //if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_legacy_table, z2s_zb_devices_table_size)) {
          if (!Z2S_loadFile(Z2S_ZB_DEVICES_TABLE_ID_V2, (uint8_t *)z2s_zb_devices_legacy_table, z2s_zb_devices_table_size)) {  

            log_i ("Zigbee devices legacy table load failed!");
            return false;
          }
          for (uint8_t table_index = 0; table_index < Z2S_ZB_DEVICES_MAX_NUMBER; table_index++) {

              z2s_zb_devices_table[table_index].record_id = (z2s_zb_devices_legacy_table + table_index)->record_id;
              memcpy(z2s_zb_devices_table[table_index].manufacturer_name, (z2s_zb_devices_legacy_table + table_index)->manufacturer_name, 32);
              memcpy(z2s_zb_devices_table[table_index].model_name, (z2s_zb_devices_legacy_table + table_index)->model_name, 32);
              z2s_zb_devices_table[table_index].desc_id = (z2s_zb_devices_legacy_table + table_index)->desc_id;
              memcpy(z2s_zb_devices_table[table_index].ieee_addr, (z2s_zb_devices_legacy_table + table_index)->ieee_addr,
                     sizeof(esp_zb_ieee_addr_t));
              z2s_zb_devices_table[table_index].endpoints_count = (z2s_zb_devices_legacy_table + table_index)->endpoints_count;
              z2s_zb_devices_table[table_index].device_status = (z2s_zb_devices_legacy_table + table_index)->device_status;
              z2s_zb_devices_table[table_index].short_addr = (z2s_zb_devices_legacy_table + table_index)->short_addr;
              z2s_zb_devices_table[table_index].power_source = (z2s_zb_devices_legacy_table + table_index)->power_source;
              z2s_zb_devices_table[table_index].battery_percentage = (z2s_zb_devices_legacy_table + table_index)->battery_percentage;
              z2s_zb_devices_table[table_index].battery_voltage_min = (z2s_zb_devices_legacy_table + table_index)->battery_voltage_min;
              z2s_zb_devices_table[table_index].battery_voltage_max = (z2s_zb_devices_legacy_table + table_index)->battery_voltage_max;
              z2s_zb_devices_table[table_index].last_seen_ms = (z2s_zb_devices_legacy_table + table_index)->last_seen_ms;
              z2s_zb_devices_table[table_index].keep_alive_ms = (z2s_zb_devices_legacy_table + table_index)->keep_alive_ms;
              z2s_zb_devices_table[table_index].timeout_ms = (z2s_zb_devices_legacy_table + table_index)->timeout_ms;
              z2s_zb_devices_table[table_index].user_data_flags = (z2s_zb_devices_legacy_table + table_index)->user_data_flags;
              z2s_zb_devices_table[table_index].user_data_1 = (z2s_zb_devices_legacy_table + table_index)->user_data_1;
              z2s_zb_devices_table[table_index].user_data_2 = (z2s_zb_devices_legacy_table + table_index)->user_data_2;
              z2s_zb_devices_table[table_index].user_data_3 = 0;
              z2s_zb_devices_table[table_index].user_data_4 = 0;
              memcpy(z2s_zb_devices_table[table_index].Supla_channels, (z2s_zb_devices_legacy_table + table_index)->Supla_channels, 16);
            }
          log_i("Zigbee devices table upgrade completed - saving new table");
          Z2S_saveZBDevicesTable();
          Z2S_printZBDevicesTableSlots();
          free(z2s_zb_devices_legacy_table);
          return true;
        }
      } else {
        log_i("Zigbee devices table size mismatch %d <> %d, no upgrade is possible", z2s_zb_devices_table_size, sizeof(z2s_zb_devices_table));
        return false;
      }
    }
    else {
        //if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
        if (!Z2S_loadFile(Z2S_ZB_DEVICES_TABLE_ID_V2, (uint8_t *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
          
          log_i("Zigbee devices table load failed!");
          return false;
        } else {
          log_i("Zigbee devices table load success!");
          Z2S_printZBDevicesTableSlots();
          return true;
        }
    }
  }
}

bool Z2S_saveZBDevicesTable() {

  //if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
  if (!Z2S_saveFile(Z2S_ZB_DEVICES_TABLE_ID_V2, (uint8_t *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
    log_i ("Zigbee devices table write failed!");
    return false;
  }
  else { 
    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_ZB_DEVICES_TABLE_SIZE, sizeof(z2s_zb_devices_table))) {
      log_i("Zigbee devices table new size(%d) write success!", sizeof(z2s_zb_devices_table));
      Supla::Storage::ConfigInstance()->commit();
      return true;
    }
    else { 
      log_i ("Zigbee devices table size write failed!");
      return false;
    }
  }
}

bool Z2S_clearZBDevicesTable() {

  log_i("Clear Zigbee devices table");
  memset(z2s_zb_devices_table,0,sizeof(z2s_zb_devices_table));
  return Z2S_saveZBDevicesTable();
}

bool Z2S_setZBDeviceFlags(int8_t device_number_slot, uint32_t flags_to_set) {

  if ((device_number_slot >= 0) && (device_number_slot < Z2S_ZB_DEVICES_MAX_NUMBER) && z2s_zb_devices_table[device_number_slot].record_id > 0) {
    z2s_zb_devices_table[device_number_slot].user_data_flags |= flags_to_set;;
    if (Z2S_saveZBDevicesTable()) {
    log_i("Device global flags set successfully to %x", z2s_zb_devices_table[device_number_slot].user_data_flags);
      return true;
    }
    return false;
  }
  else 
    return false;
}

bool Z2S_clearZBDeviceFlags(int8_t device_number_slot, uint32_t flags_to_clear) {

  if ((device_number_slot >= 0) && (device_number_slot < Z2S_ZB_DEVICES_MAX_NUMBER) && z2s_zb_devices_table[device_number_slot].record_id > 0) {
    z2s_zb_devices_table[device_number_slot].user_data_flags &= ~flags_to_clear;
    if (Z2S_saveZBDevicesTable()) {
    log_i("Device global flags cleared successfully to %x", z2s_zb_devices_table[device_number_slot].user_data_flags);
      return true;
    }
    return false;
  }
  else 
    return false;
}

void Z2S_onDataSaveRequest(uint8_t selector) {
  
  switch (selector) {
    case 0: Z2S_saveChannelsTable(); break;
    case 1: Z2S_saveZBDevicesTable(); break;
    case 2: {
      Z2S_saveZBDevicesTable();
      Z2S_saveChannelsTable();
    } break;
  }
}

void Z2S_initSuplaChannels() {

  log_i ("initSuplaChannels starting");
  zbg_device_params_t *device = (zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));

  for (uint8_t channels_counter = 0; channels_counter < Z2S_CHANNELS_MAX_NUMBER; channels_counter++) {
      if (z2s_channels_table[channels_counter].valid_record) {

        device->endpoint = z2s_channels_table[channels_counter].endpoint;
        device->cluster_id = z2s_channels_table[channels_counter].cluster_id;
        memcpy(device->ieee_addr, z2s_channels_table[channels_counter].ieee_addr, 8);
        device->short_addr = z2s_channels_table[channels_counter].short_addr;
        device->model_id = z2s_channels_table[channels_counter].model_id;
        
        Z2S_updateZBDeviceTableSlot(z2s_channels_table[channels_counter].ieee_addr, z2s_channels_table[channels_counter].Supla_channel);

        bool channel_created = true;
        
        switch (z2s_channels_table[channels_counter].Supla_channel_type) {

          case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
            initZ2SDeviceTempHumidity(channels_counter); break;

          case SUPLA_CHANNELTYPE_PRESSURESENSOR:
            initZ2SDevicePressure(channels_counter); break;

          case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT:
            initZ2SDeviceGeneralPurposeMeasurement(channels_counter); break; 
          
          case SUPLA_CHANNELTYPE_BINARYSENSOR: 
            initZ2SDeviceIASzone(channels_counter); break;
          
          case SUPLA_CHANNELTYPE_RELAY: 
            initZ2SDeviceVirtualRelay(&zbGateway, device, channels_counter); break;
          
          case SUPLA_CHANNELTYPE_ACTIONTRIGGER: 
            initZ2SDeviceActionTrigger(channels_counter); break;

          case SUPLA_CHANNELTYPE_ELECTRICITY_METER: 
            initZ2SDeviceElectricityMeter(&zbGateway, device, channels_counter); break;
          
          case SUPLA_CHANNELTYPE_HVAC: initZ2SDeviceHvac(&zbGateway, device, channels_counter); break;
          
          case SUPLA_CHANNELTYPE_DIMMER: initZ2SDeviceDimmer(&zbGateway, device, channels_counter); break;
          
          case SUPLA_CHANNELTYPE_RGBLEDCONTROLLER: initZ2SDeviceRGB(&zbGateway, device, channels_counter); break;

          case SUPLA_CHANNELTYPE_VALVE_OPENCLOSE: initZ2SDeviceVirtualValve(&zbGateway, device, channels_counter); break;
          
          //case SUPLA_CHANNELTYPE_DIMMERANDRGBLED: initZ2SDeviceRGBW(&zbGateway, device, z2s_channels_table[channels_counter].Supla_channel); break;
          
          default: {
            log_i("Can't create channel for %d channel type", z2s_channels_table[channels_counter].Supla_channel_type); 
            channel_created = false;
          } break; 
        }
        if (channel_created) {
          uint8_t zb_device_number_slot = Z2S_findZBDeviceTableSlot(z2s_channels_table[channels_counter].ieee_addr);
         // if ((zb_device_number_slot < 0xFF) &&
         //     (z2s_zb_devices_table[zb_device_number_slot].battery_percentage >= 0x80))
          //  updateSuplaBatteryLevel(channels_counter, ZBD_BATTERY_RESTORE_MSG, 
           //                         z2s_zb_devices_table[zb_device_number_slot].battery_percentage, true);
        }
      }
  }
  free(device);
  //auto TestVT = new Supla::Sensor::VirtualThermometer(); //for TRV testing
  //TestVT->getChannel()->setChannelNumber(100); //RIP
  //auto TestDI = new Supla::Control::DimmerInputInterface(&zbGateway, device, 0);
  //TestDI->getChannel()->setChannelNumber(101);
}

bool checkActionsIndexTablePosition(uint16_t index_position) {

  if (index_position >= Z2S_ACTIONS_MAX_NUMBER)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index = index_position % 8;

  if (z2s_actions_index_table[byte_index] & (1 << bit_index))
    return true;
  else
    return false;
}

bool setActionsIndexTablePosition(uint16_t index_position) {

  if (index_position >= Z2S_ACTIONS_MAX_NUMBER)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index = index_position % 8;

  z2s_actions_index_table[byte_index] |= (1 << bit_index);
  return true;
}

bool clearActionsIndexTablePosition(uint16_t index_position) {

  if (index_position >= Z2S_ACTIONS_MAX_NUMBER)
    return false;

  uint8_t byte_index = index_position / 8;
  uint8_t bit_index = index_position % 8;

  z2s_actions_index_table[byte_index] &= ~(1 << bit_index);
  return true;
}

bool Z2S_loadActionsIndexTable() {

  memset(z2s_actions_index_table, 0, sizeof(z2s_actions_index_table));

  //if (Supla::Storage::ConfigInstance()->getBlob(Z2S_CHANNELS_ACTIONS_INDEX_TABLE, (char *)z2s_actions_index_table, sizeof(z2s_actions_index_table))) {
  if (Z2S_loadFile(Z2S_CHANNELS_ACTIONS_INDEX_TABLE_V2, (uint8_t *)z2s_actions_index_table, sizeof(z2s_actions_index_table))) {

    log_i ("Zigbee<=>Supla actions index table load SUCCESS!");
    return true;

  } else
  //if (Supla::Storage::ConfigInstance()->setBlob(Z2S_CHANNELS_ACTIONS_INDEX_TABLE, (char *)z2s_actions_index_table, sizeof(z2s_actions_index_table))) {
  if (Z2S_saveFile(Z2S_CHANNELS_ACTIONS_INDEX_TABLE_V2, (uint8_t *)z2s_actions_index_table, sizeof(z2s_actions_index_table))) {

    log_i ("Zigbee<=>Supla actions index table not found - writing new one: SUCCESS!");
    return true;
  
  } else {
  
    log_i ("Zigbee<=>Supla actions index table not found - writing new one: FAILED!");
    return false;
  }
}

bool Z2S_saveActionsIndexTable() {

  //if (Supla::Storage::ConfigInstance()->setBlob(Z2S_CHANNELS_ACTIONS_INDEX_TABLE, (char *)z2s_actions_index_table, sizeof(z2s_actions_index_table))) {
  if (Z2S_saveFile(Z2S_CHANNELS_ACTIONS_INDEX_TABLE_V2, (uint8_t *)z2s_actions_index_table, sizeof(z2s_actions_index_table))) {

    log_i ("Saving Zigbee<=>Supla actions index table: SUCCESS!");
    return true;

  } else {
    
    log_i ("Saving Zigbee<=>Supla actions index table: FAILED!");
    return false;
  }
}

uint16_t Z2S_getActionsNumber() {

  uint16_t actions_number = 0;

  for (uint16_t index = 0; index < Z2S_ACTIONS_MAX_NUMBER; index++)
    if (checkActionsIndexTablePosition(index))
      actions_number++;
  
  return actions_number;
}

int16_t  Z2S_findFreeActionIndex() {

  for (uint16_t index = 0; index < Z2S_ACTIONS_MAX_NUMBER; index++)
    if (!checkActionsIndexTablePosition(index))
      return index;
  
  return -1; 
}

bool Z2S_saveAction(uint16_t action_index, z2s_channel_action_t &action) {

  if (action_index >= Z2S_ACTIONS_MAX_NUMBER)
    return false;

  char file_name_buffer[50] = {};
  sprintf(file_name_buffer, Z2S_CHANNELS_ACTIONS_PPREFIX_V2, action_index);
  
  //if (Supla::Storage::ConfigInstance()->setBlob(blob_name_buffer, (char *)&action, sizeof(z2s_channel_action_t))) {
  if (Z2S_saveFile(file_name_buffer, (uint8_t*) &action, sizeof(z2s_channel_action_t))) {

    log_i ("Saving Zigbee<=>Supla action in file %s: SUCCESS", file_name_buffer);
    setActionsIndexTablePosition(action_index);
    Z2S_saveActionsIndexTable();
    return true;
  } else {

    log_i ("Saving Zigbee<=>Supla action in file %s: FAILED", file_name_buffer);
    return false;
  }
}

bool Z2S_loadAction(uint16_t action_index, z2s_channel_action_t &action) {

  if (action_index >= Z2S_ACTIONS_MAX_NUMBER)
    return false;

  char file_name_buffer[50] = {};
  sprintf(file_name_buffer, Z2S_CHANNELS_ACTIONS_PPREFIX_V2, action_index);
  
  //if (Supla::Storage::ConfigInstance()->getBlob(blob_name_buffer, (char *)&action, sizeof(z2s_channel_action_t))) {
  if (Z2S_loadFile(file_name_buffer, (uint8_t*) &action, sizeof(z2s_channel_action_t))) {

    log_i ("Loading Zigbee<=>Supla action from file %s: SUCCESS", file_name_buffer);
    return true;
  } else {
    
    log_i ("Loading Zigbee<=>Supla action from file %s: FAILED", file_name_buffer);
    return false;
  }
}

bool Z2S_removeAction(uint16_t action_index) {

  if (action_index >= Z2S_ACTIONS_MAX_NUMBER)
    return false;

  char file_name_buffer[50] = {};
  sprintf(file_name_buffer, Z2S_CHANNELS_ACTIONS_PPREFIX_V2, action_index);
  
  if (Z2S_deleteFile(file_name_buffer)) 
    log_i ("Removing Zigbee<=>Supla action file %s: SUCCESS", file_name_buffer);
  else 
    log_i ("Removing Zigbee<=>Supla action file %s: FAILED", file_name_buffer);

  clearActionsIndexTablePosition(action_index);
  Z2S_saveActionsIndexTable();

  return true;
}

void Z2S_initSuplaActions() {

  if (!Z2S_loadActionsIndexTable()) {
    
    log_e("No actions index table available!");
    return;
  }

  log_i("actions number: %u", Z2S_getActionsNumber());

  z2s_channel_action_t new_action = {};

  for (uint16_t index = 0; index < Z2S_ACTIONS_MAX_NUMBER; index++) {
    
    if (checkActionsIndexTablePosition(index)) {
      Z2S_loadAction(index, new_action);
      //Z2S_add_action(new_action.action_name, new_action.src_Supla_channel, new_action.dst_Supla_action, new_action.dst_Supla_channel, 
        //            new_action.src_Supla_event, new_action.is_condition, new_action.min_value, new_action.max_value);
      log_i("Action name: %s, src_Supla_channel %u, dst_Supla_action %u, dst_Supla_channel %u, src_Supla_event %u, is_condition %u" 
            "min_value %f, max_value %f", new_action.action_name, new_action.src_Supla_channel, new_action.dst_Supla_action, new_action.dst_Supla_channel, 
                    new_action.src_Supla_event, new_action.is_condition, new_action.min_value, new_action.max_value);    
    }
  }
}

void Z2S_onTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float temperature, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, temperature %f", ieee_addr_str, endpoint, temperature);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0) 
    no_channel_found_error_func(ieee_addr_str);
  else
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, temperature, rssi);
}

void Z2S_onHumidityReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float humidity, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, humidity %f", ieee_addr_str, endpoint, humidity);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    no_channel_found_error_func(ieee_addr_str);
  else
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, humidity, rssi);
}

void Z2S_onPressureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float pressure, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, pressure %f", ieee_addr_str, endpoint, pressure);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_PRESSURESENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    no_channel_found_error_func(ieee_addr_str);
  else
    msgZ2SDevicePressure(channel_number_slot, pressure, rssi);
}

void Z2S_onIlluminanceReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t illuminance, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, illuminance %u", ieee_addr_str, endpoint, illuminance);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                          TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID);
  if (channel_number_slot >= 0) {
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE, illuminance/10, rssi); 
    return;
  }
  
  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                          NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {                         
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_ILLUMINANCE, illuminance, rssi); 
    return;
  }
  
  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onFlowReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t flow, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, flow %u", ieee_addr_str, endpoint, flow);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                          SONOFF_SMART_VALVE_FLOW_SID);
  if (channel_number_slot >= 0) {                         
    msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, flow, rssi); 
    return;
  }
  
  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onOccupancyReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t occupancy, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, occupancy 0x%x", ieee_addr_str, endpoint, occupancy);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, SONOFF_PIR_SENSOR_OCCUPANCY_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceIASzone(channel_number_slot, (occupancy > 0), rssi);
    return;
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceIASzone(channel_number_slot, (occupancy > 0), rssi);
    return;
  }

  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onThermostatTemperaturesReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
                                         uint16_t id, int16_t temperature, signed char rssi) {

  log_i("%x:%x:%x:%x:%x:%x:%x:%x, endpoint 0x%x, attribute id 0x%x, temperature %d", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, id, temperature);

  int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, 
                                                            NO_CUSTOM_CMD_SID);
  
  int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HVAC, 
                                                            NO_CUSTOM_CMD_SID);

  if (channel_number_slot_2 < 0) {
    log_i("no thermostat channel found for address %x:%x:%x:%x:%x:%x:%x:%x",
          ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
    return;
  }

  switch (id) {

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID: {
      if (channel_number_slot_1 >= 0)
        msgZ2SDeviceTempHumidityTemp(channel_number_slot_1, (float)temperature / 100, rssi);
      else log_e("Missing thermometer channel for thermostat device!");
      msgZ2SDeviceHvac(channel_number_slot_2, TRV_LOCAL_TEMPERATURE_MSG, temperature, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID: {
      msgZ2SDeviceHvac(channel_number_slot_2, TRV_HEATING_SETPOINT_MSG, temperature, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID: {
      msgZ2SDeviceHvac(channel_number_slot_2, TRV_TEMPERATURE_CALIBRATION_MSG, temperature * 10, rssi);
    } break;
  }
}

void Z2S_onThermostatModesReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
                                  uint16_t id, uint8_t mode, signed char rssi) {

  log_i("%x:%x:%x:%x:%x:%x:%x:%x, endpoint 0x%x, attribute id 0x%x, mode %u", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, id, mode);

  int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HVAC, 
                                                            NO_CUSTOM_CMD_SID);

  if (channel_number_slot_2 < 0) {
    log_i("no thermostat channel found for address %x:%x:%x:%x:%x:%x:%x:%x",
          ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
    return;
  }
  
  switch (id) {

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID: {
      uint8_t running_mode = mode & 1;
      msgZ2SDeviceHvac(channel_number_slot_2, TRV_RUNNING_STATE_MSG, running_mode, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID: {
      switch (mode) {
        case 0: msgZ2SDeviceHvac(channel_number_slot_2, TRV_SYSTEM_MODE_MSG, 0, rssi); break;
        case 4: msgZ2SDeviceHvac(channel_number_slot_2, TRV_SYSTEM_MODE_MSG, 1, rssi); break;
        case 1: msgZ2SDeviceHvac(channel_number_slot_2, TRV_SCHEDULE_MODE_MSG, 1, rssi); break;
      }
      //uint8_t system_mode = (mode == 0) ? 0 : 1;
      //msgZ2SDeviceHvac(channel_number_slot_2, TRV_SYSTEM_MODE_MSG, system_mode, rssi);
    } break;
  }
}

void Z2S_onWindowCoveringReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t id,
                                 uint16_t value, signed char rssi) {

  log_i("%x:%x:%x:%x:%x:%x:%x:%x, endpoint 0x%x, attribute id 0x%x, value %u", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, id, value);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RELAY, 
                                                            NO_CUSTOM_CMD_SID);

  if (channel_number_slot < 0) {
    log_i("no roller shutter channel found for address %x:%x:%x:%x:%x:%x:%x:%x",
          ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
    return;
  }
  
  switch (id) {

    case ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID:
      msgZ2SDeviceRollerShutter(channel_number_slot, RS_CURRENT_POSITION_LIFT_PERCENTAGE_MSG, value, rssi); break;
    case 0xF000:
      msgZ2SDeviceRollerShutter(channel_number_slot, RS_MOVING_DIRECTION_MSG, value, rssi); break;
  }
}

void Z2S_onSonoffCustomClusterReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, const esp_zb_zcl_attribute_t *attribute, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  
  log_i("%s, endpoint 0x%x, attribute id 0x%x, size %u", ieee_addr_str, endpoint,attribute->id, attribute->data.size);

  switch (attribute->id) {

    case SONOFF_CUSTOM_CLUSTER_CHILD_LOCK_ID: {

      int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HVAC, 
                                                              NO_CUSTOM_CMD_SID);

      if (channel_number_slot < 0) {
        no_channel_found_error_func(ieee_addr_str);
        return;
      }
      
      msgZ2SDeviceHvac(channel_number_slot, TRV_CHILD_LOCK_MSG, *(uint8_t*)attribute->data.value, rssi);
    } break;

    case SONOFF_CUSTOM_CLUSTER_TAMPER_ID: {

      int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                              SONOFF_CUSTOM_CLUSTER_TAMPER_SID);
      
      if (channel_number_slot < 0) {
        no_channel_found_error_func(ieee_addr_str);
        return;
      }
      msgZ2SDeviceIASzone(channel_number_slot, *(uint8_t*)attribute->data.value, rssi);      
    } break;

    case SONOFF_CUSTOM_CLUSTER_ILLUMINATION_ID: {

      int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, 
                                                              SONOFF_PIR_SENSOR_ILLUMINANCE_SID);
      
      if (channel_number_slot < 0) {
        no_channel_found_error_func(ieee_addr_str);
        return;
      }
      msgZ2SDeviceIASzone(channel_number_slot, *(uint8_t*)attribute->data.value, rssi);      
    } break;

    case SONOFF_CUSTOM_CLUSTER_IRRIGATION_CYCLE_MODE_ID: {
        
      int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                              SONOFF_SMART_VALVE_MODE_SID);
      
      if (channel_number_slot < 0) {
        no_channel_found_error_func(ieee_addr_str);
        return;
      }
      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, *(uint8_t*)attribute->data.value, rssi); 
    } break;

    case SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID:
    case SONOFF_CUSTOM_CLUSTER_VOLUME_IRRIGATION_CYCLE_ID:  {

      if ((attribute->data.type != ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING) || (attribute->data.size != 11)) {
        log_e("Smart valve irrigation cycle data invalid or empty: ieee address %s, type 0x%x, size 0x%x",
              ieee_addr_str, attribute->data.type, attribute->data.size);
        return;
      }
           
      int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                              SONOFF_SMART_VALVE_CYCLE_NUMBER_SID);
      int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                              SONOFF_SMART_VALVE_CYCLES_COUNT_SID);
      int16_t channel_number_slot_3 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                              SONOFF_SMART_VALVE_TIME_SID);
      int16_t channel_number_slot_4 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                              SONOFF_SMART_VALVE_PAUSE_SID);
      int16_t channel_number_slot_5 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT, 
                                                              SONOFF_SMART_VALVE_VOLUME_SID);
      
      
      if ((channel_number_slot_1 < 0) && (channel_number_slot_2 < 0) && (channel_number_slot_3 < 0) && (channel_number_slot_4 < 0)&& (channel_number_slot_5 < 0)) {
        log_e("No smart valve irrigation cycle channels found for address %s", ieee_addr_str);
        return;
      }
      sonoff_smart_valve_cycle_data_t sonoff_smart_valve_cycle_data;
      memcpy(&sonoff_smart_valve_cycle_data, attribute->data.value, sizeof(sonoff_smart_valve_cycle_data_t));
      
      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_1, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, sonoff_smart_valve_cycle_data.cycle_number, rssi); 
      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_2, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, sonoff_smart_valve_cycle_data.cycles_count, rssi); 
      if (attribute->id == SONOFF_CUSTOM_CLUSTER_TIME_IRRIGATION_CYCLE_ID)
        msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_3, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, __builtin_bswap32(sonoff_smart_valve_cycle_data.cycle_data), rssi);
      if (attribute->id == SONOFF_CUSTOM_CLUSTER_VOLUME_IRRIGATION_CYCLE_ID)
        msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_5, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, __builtin_bswap32(sonoff_smart_valve_cycle_data.cycle_data), rssi);
      
      msgZ2SDeviceGeneralPurposeMeasurement(channel_number_slot_4, ZS2_DEVICE_GENERAL_PURPOSE_MEASUREMENT_FNC_NONE, __builtin_bswap32(sonoff_smart_valve_cycle_data.cycle_pause), rssi);
    } break;
  }
}

void Z2S_onOnOffReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, bool state, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, state 0x%x", ieee_addr_str, endpoint, state);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RELAY, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceVirtualRelay(channel_number_slot, state, rssi); //default On/Off channel
    return;
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {
    msgZ2SDeviceIASzone(channel_number_slot, state, rssi); //AQARA magnet
    return;
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMER, NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {
    msgZ2SDeviceDimmer(channel_number_slot, -1, state, rssi);
    return;
  }
  
  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {
    msgZ2SDeviceRGB(z2s_channels_table[channel_number_slot].model_id, z2s_channels_table[channel_number_slot].Supla_channel, 0xFF, 0xFF, state, rssi);
    return;
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_VALVE_OPENCLOSE, NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {
    msgZ2SDeviceVirtualValve(z2s_channels_table[channel_number_slot].Supla_channel, state, rssi);
    return;
  } 
  
  //channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMERANDRGBLED, NO_CUSTOM_CMD_SID);
  //if (channel_number_slot >= 0) {
    //msgZ2SDeviceRGBW(z2s_channels_table[channel_number_slot].model_id, z2s_channels_table[channel_number_slot].Supla_channel, 0xFF, 0xFF, 0xFFFF, state, rssi);
    //return;
 // }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ACTIONTRIGGER, NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {
    if (state)
      msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
    return;
  }

  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onElectricalMeasurementReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
                                        const esp_zb_zcl_attribute_t *attribute, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, attribute id 0x%x, size %u", ieee_addr_str, endpoint, attribute->id, attribute->data.size);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER, 
                                                              NO_CUSTOM_CMD_SID);

  if (channel_number_slot < 0) {
    
    log_e("no electricity meter channel found for address %s", ieee_addr_str);
    return;
  }

  if (attribute->data.value == nullptr) {
      
    log_e("missing data value for address %s", ieee_addr_str);
    return;
  }

  switch (attribute->id) {

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_VOLTAGE_A_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_CURRENT_A_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_ACTIVE_POWER_A_SEL, *(int16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_AC_VOLTAGE_MUL_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_AC_VOLTAGE_DIV_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_AC_CURRENT_MUL_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_AC_CURRENT_DIV_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_AC_ACTIVE_POWER_MUL_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID: {

      msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_AC_ACTIVE_POWER_DIV_SEL, *(uint16_t *)attribute->data.value, rssi);
    } break;
  }
}

void Z2S_onMultistateInputReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
                                        const esp_zb_zcl_attribute_t *attribute, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, attribute id 0x%x, size %u", ieee_addr_str, endpoint, attribute->id, attribute->data.size);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ACTIONTRIGGER, 
                                                              NO_CUSTOM_CMD_SID);

  if (channel_number_slot < 0) {
    
    log_e("no action trigger channel found for address %s", ieee_addr_str);
    //return;
  }

  if (attribute->data.value == nullptr) {
      
    log_e("missing data value for address %s", ieee_addr_str);
    return;
  }

  switch (attribute->id) {

    case 0x000E: {

      log_i("ESP_ZB_ZCL_ATTR_MULTI_VALUE_STATE_TEXT_ID");
    } break;

    case ESP_ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID: {

      log_i("present value = %d", *(uint16_t *)attribute->data.value);
    } break;  
  }
}

void Z2S_onCurrentSummationReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint64_t active_fwd_energy, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, active_fwd_energy 0xllu", ieee_addr_str, endpoint, cluster, active_fwd_energy);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    //log_i(_no_channel_found_str, ieee_addr);
    no_channel_found_error_func(ieee_addr_str);
  else
    msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_ACT_FWD_ENERGY_A_SEL, active_fwd_energy, rssi);
}

void Z2S_onCurrentLevelReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t level, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, level 0x%x", ieee_addr_str, endpoint, cluster, level);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMER, DIMMER_FUNC_BRIGHTNESS_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceDimmer(channel_number_slot, level, true, rssi);
    return;
  }
  //log_i(_no_channel_found_str, ieee_addr);
  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onColorHueReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t hue, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, hue 0x%x", ieee_addr_str, endpoint, cluster, hue);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceRGB(z2s_channels_table[channel_number_slot].model_id, z2s_channels_table[channel_number_slot].Supla_channel, hue, 0xFF, true, rssi);
    return;
  }
  //log_i(_no_channel_found_str, ieee_addr);
  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onColorSaturationReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t saturation, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, saturation 0x%x", ieee_addr_str, endpoint, cluster, saturation);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceRGB(z2s_channels_table[channel_number_slot].model_id, z2s_channels_table[channel_number_slot].Supla_channel, 0xFF, 
                    saturation,true, rssi);
    return;
  }
  //log_i(_no_channel_found_str, ieee_addr);
  no_channel_found_error_func(ieee_addr_str);
}

void Z2S_onColorTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t color_temperature, signed char rssi) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, color temparature 0x%x", ieee_addr_str, endpoint, cluster, color_temperature);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMER, DIMMER_FUNC_COLOR_TEMPERATURE_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceDimmer(z2s_channels_table[channel_number_slot].Supla_channel, color_temperature, true, rssi);
    return;
  }
  //log_i(_no_channel_found_str, ieee_addr);
  no_channel_found_error_func(ieee_addr_str);
}


void Z2S_onBatteryReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t id, uint8_t battery_remaining) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, battery 0x%x", ieee_addr_str, endpoint, cluster, battery_remaining);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, -1, cluster, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0) {
    //log_i(_no_channel_found_str, ieee_addr);
    no_channel_found_error_func(ieee_addr_str);
    return;
  }
  
  switch (id) {
    case ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID:
      updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_PERCENTAGE_MSG, battery_remaining, 0); break;
    case ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID:
      updateSuplaBatteryLevel(channel_number_slot, ZBD_BATTERY_VOLTAGE_MSG, battery_remaining, 0); break;
  }
}

void Z2S_onIASzoneStatusChangeNotification(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int iaszone_status, signed char rssi) {
  
  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, zone status 0x%x", ieee_addr_str, endpoint, cluster, iaszone_status);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, TUYA_VIBRATION_SENSOR_CONTACT_SID);
  
  if (channel_number_slot >= 0) {
    log_i("IASZONE - TUYA_VIBRATION_SENSOR_CONTACT_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 1), rssi);
    return;
  }
  
  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, IAS_ZONE_ALARM_1_SID);

  if (channel_number_slot >= 0) {
    log_i("IASZONE - IAS_ZONE_ALARM_1_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    if (iaszone_status < 0x0400)
      msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 1), rssi);
    else
      msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 0x0400), rssi);
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, IAS_ZONE_ALARM_2_SID);

  if (channel_number_slot >= 0) {
    log_i("IASZONE - IAS_ZONE_ALARM_2_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 2), rssi);
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, IAS_ZONE_TAMPER_SID);

  if (channel_number_slot >= 0) {
    log_i("IASZONE - IAS_ZONE_TAMPER_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 4), rssi);
  }
  
  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, IAS_ZONE_LOW_BATTERY_SID);

  if (channel_number_slot >= 0) {
    log_i("IASZONE - IAS_ZONE_LOW_BATTERY_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 8), rssi);
    return;
  }
  
  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);

  if (channel_number_slot >= 0) {
    log_i("IASZONE - NO_CUSTOM_CMD_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 1), rssi);
    return;
  }

  no_channel_found_error_func(ieee_addr_str);
}

bool compareBuffer(uint8_t *buffer, uint8_t buffer_size, char *lookup_str) {
  
  char byte_str[3];
  byte_str[2] = '\0';

  for (int i = 0; i < buffer_size; i++) {
      memcpy(byte_str, lookup_str + (i * 2), 2);
      if (strtoul(byte_str, nullptr, 16) != *((uint8_t*)(buffer + i)))
        return false;
  }
  return true;
}

bool processIkeaSymfoniskCommands(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster_id, uint8_t command_id, 
                                  uint8_t buffer_size, uint8_t *buffer, signed char  rssi) {

  log_i("IKEA SYMFONISK/SOMRIG command: cluster(0x%x), command id(0x%x), ", cluster_id, command_id);
  
  uint8_t sub_id = 0x7F;

  bool is_IKEA_FC80_EP_2 = false;
  bool is_IKEA_FC80_EP_3 = false;
  bool is_IKEA_FC7F_C_1 = false;
  bool isSomrig = false;

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster_id, SUPLA_CHANNELTYPE_ACTIONTRIGGER, NO_CUSTOM_CMD_SID);

  if ((z2s_channels_table[channel_number_slot].model_id == Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1) ||
      (z2s_channels_table[channel_number_slot].model_id == Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2))
    isSomrig = true;

  if (isSomrig) {
    if ((cluster_id == 0xFC80) && (endpoint == 1))
      is_IKEA_FC80_EP_2 = true;
  else
  if ((cluster_id == 0xFC80) && (endpoint == 2))
      is_IKEA_FC80_EP_3 = true;  
  } else {
    if ((cluster_id == 0xFC80) && (endpoint == 2))
      is_IKEA_FC80_EP_2 = true;
    else
    if ((cluster_id == 0xFC80) && (endpoint == 3))
      is_IKEA_FC80_EP_3 = true;
    else
    if ((cluster_id == 0xFC7F) && (command_id == 1))
      is_IKEA_FC7F_C_1 = true;
  }

  if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (command_id == 0x02))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_PLAY_SID;
  else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x05) && compareBuffer(buffer, buffer_size, "00FF"))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID;
  else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x05)&& compareBuffer(buffer, buffer_size, "01FF"))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID;
  else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x02) && compareBuffer(buffer, buffer_size, "000100000000"))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_NEXT_TRACK_SID;
  else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x02) && compareBuffer(buffer, buffer_size, "010100000000")) 
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_PREV_TRACK_SID;
  else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x01) && 
           (compareBuffer(buffer, buffer_size, "00FF0000") || compareBuffer(buffer, buffer_size, "00C30000"))) 
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_UP_SID;
  else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x01) && 
           (compareBuffer(buffer, buffer_size, "01FF0000") || compareBuffer(buffer, buffer_size, "01C30000")))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_VOLUME_DOWN_SID;
  else if (is_IKEA_FC80_EP_2 && (command_id == 0x01))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID;
  else if (is_IKEA_FC80_EP_2 && (command_id == 0x02))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_HELD_SID;
  else if (is_IKEA_FC80_EP_2 && (command_id == 0x03))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID;
  else if (is_IKEA_FC80_EP_2 && (command_id == 0x04))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_LONG_RELEASED_SID;
  else if (is_IKEA_FC80_EP_2 && (command_id == 0x06))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID;
  else if (is_IKEA_FC80_EP_3 && (command_id == 0x01))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID;
  else if (is_IKEA_FC80_EP_3 && (command_id == 0x02))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_HELD_SID;
  else if (is_IKEA_FC80_EP_3 && (command_id == 0x03))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID;
  else if (is_IKEA_FC80_EP_3 && (command_id == 0x04))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_LONG_RELEASED_SID;
  else if (is_IKEA_FC80_EP_3 && (command_id == 0x06))
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID;
  
  //Symfonsik gen 2 legacy

  if (is_IKEA_FC7F_C_1 && compareBuffer(buffer, buffer_size, "0101")) {
    endpoint = 2;
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_PRESSED_SID;
  } else
  if (is_IKEA_FC7F_C_1 && compareBuffer(buffer, buffer_size, "0102")) {
    endpoint = 2;
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_SHORT_RELEASED_SID;
  } else
  if (is_IKEA_FC7F_C_1 && compareBuffer(buffer, buffer_size, "0201")) {
    endpoint = 3;
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_PRESSED_SID;
  } else
  if (is_IKEA_FC7F_C_1 && compareBuffer(buffer, buffer_size, "0202")) {
    endpoint = 3;
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_SHORT_RELEASED_SID;
  } else
  if (is_IKEA_FC7F_C_1 && compareBuffer(buffer, buffer_size, "0103")) {
    endpoint = 2;
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOT_DOUBLE_PRESSED_SID;
  }
  if (is_IKEA_FC7F_C_1 && compareBuffer(buffer, buffer_size, "0203")) {
    endpoint = 3;
    sub_id = IKEA_CUSTOM_CMD_SYMFONISK_DOTS_DOUBLE_PRESSED_SID;
  }

  if (sub_id == 0x7F) return false;

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster_id, SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id);

  if (channel_number_slot < 0)
    log_i("No IKEA SYMFONISK/SOMRIG channel found for address %s", ieee_addr);
  else 
    msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
  return true;
}

bool Z2S_onCustomCmdReceive( esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster_id, uint8_t command_id, uint8_t buffer_size, uint8_t *buffer, signed char  rssi){
  
  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, ieee_addr);

  log_i("%s, endpoint 0x%x, cluster 0x%x, cmd id 0x%x", ieee_addr_str, endpoint, cluster_id, command_id);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            ALL_SUPLA_CHANNEL_TYPES/*SUPLA_CHANNELTYPE_ACTIONTRIGGER*/, NO_CUSTOM_CMD_SID);
  if (channel_number_slot < 0) {
    //log_i(_no_channel_found_str, ieee_addr);
    no_channel_found_error_func(ieee_addr_str);
    return false;
  }
  
  log_i("z2s_channels_table[channel_number_slot].Supla_channel 0x%x", z2s_channels_table[channel_number_slot].Supla_channel);

  switch (z2s_channels_table[channel_number_slot].model_id) {
    
    case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON:
    case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F:
    case Z2S_DEVICE_DESC_IKEA_VALLHORN_1: {
      
      log_i("IKEA command: cluster(0x%x), command id(0x%x), ", cluster_id, command_id);
      int8_t sub_id = 0x7F;

      if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (command_id == 0x42))
        sub_id = -1;
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (command_id == 0x01)) {
        if ((Styrbar_ignore_button_1) && (millis() - Styrbar_timer < 1500)) return true;
        else {
          sub_id = IKEA_CUSTOM_CMD_BUTTON_1_PRESSED_SID;
          Styrbar_ignore_button_1 = false;
        }
      }
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (command_id == 0x00))
        sub_id = IKEA_CUSTOM_CMD_BUTTON_2_PRESSED_SID;
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x05))
        sub_id = IKEA_CUSTOM_CMD_BUTTON_1_HELD_SID;
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL) && (command_id == 0x01) && compareBuffer(buffer, buffer_size, "01530000"))
        sub_id = IKEA_CUSTOM_CMD_BUTTON_2_HELD_SID;
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_SCENES) && (command_id == 0x09) && compareBuffer(buffer, buffer_size, "0000")) {
        Styrbar_ignore_button_1 = true;
        Styrbar_timer = millis();
        return true; //
      }
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_SCENES) && (command_id == 0x08) && compareBuffer(buffer, 1/*buffer_size*/, "01"))
        sub_id = IKEA_CUSTOM_CMD_BUTTON_3_HELD_SID;
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_SCENES) && (command_id == 0x08) && compareBuffer(buffer, 1/* buffer_size*/, "00"))
        sub_id = IKEA_CUSTOM_CMD_BUTTON_4_HELD_SID;
      else if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_SCENES) && (command_id == 0x07)) {
        if (compareBuffer(buffer, buffer_size, "01010D00"))
          sub_id = IKEA_CUSTOM_CMD_BUTTON_3_PRESSED_SID;
        else if (compareBuffer(buffer, buffer_size, "00010D00"))
          sub_id = IKEA_CUSTOM_CMD_BUTTON_4_PRESSED_SID;
      }  
      if (sub_id == 0x7F) return false;

      channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id);
      if (channel_number_slot < 0)
        log_i("No IKEA device channel found for address %s", ieee_addr_str);
      else 
        msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
      return true;
    } break;

    case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1:
    case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1:  
      
      return processIkeaSymfoniskCommands(ieee_addr, endpoint, cluster_id, command_id, buffer_size, buffer, rssi); break;

    case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON:  //this will never happen
      
      return processIkeaSymfoniskCommands(ieee_addr, endpoint, cluster_id, command_id, buffer_size, buffer, rssi); break;

    case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1: {
      
      if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (command_id == 0x00))
        msgZ2SDeviceIASzone(channel_number_slot, false, rssi);
      if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && (command_id == 0x01))
        msgZ2SDeviceIASzone(channel_number_slot, true, rssi);
      return true;
    } break;
    
    case Z2S_DEVICE_DESC_TUYA_SWITCH_4X3:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
    case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F: {
      
      log_i("TUYA command: cluster(0x%x), command id(0x%x)",  cluster_id, command_id);   
      //process Tuya command
      if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) &&
        ((command_id == TUYA_ON_OFF_CUSTOM_CMD_BUTTON_PRESS_ID) || (command_id == TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID))) {

        int8_t sub_id = (command_id == TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID) ? TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID + (*buffer) : (*buffer);
        channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id);
        if (channel_number_slot < 0)
          log_i("No Tuya device channel found for address %s", ieee_addr_str);
        else 
          msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
      return true;
    } 
  } break;

  /*case Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F: {
    
    if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && ((command_id >= 0) && (command_id < 2))) {
      
      msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
      return true;
    }
  } break;*/

  case Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F: {
    
    if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && ((command_id >= 0) && (command_id <=2))) {

        int8_t sub_id;
        switch (command_id) {
          case 0: sub_id = TUYA_CUSTOM_CMD_BUTTON_HELD_SID; break;
          case 1: sub_id = TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID; break;
          case 2: sub_id = TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID; break;
        }  
        channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id);
        if (channel_number_slot < 0)
          no_channel_found_error_func(ieee_addr_str);
        else 
          msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
      return true;
    } 
  } break;
      return false; 
  }
  return false;
}

void Z2S_onCmdCustomClusterReceive( esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t command_id,
                                     uint16_t payload_size, uint8_t *payload, signed char rssi) {

  switch (cluster) {
    case TUYA_PRIVATE_CLUSTER_EF00: {

      processTuyaCustomCluster(ieee_addr, endpoint, command_id, payload_size, payload, rssi); 
      if (_on_Tuya_custom_cluster_receive) 
        _on_Tuya_custom_cluster_receive(command_id, payload_size, payload);
    } break;

    case ZOSUNG_IR_TRANSMIT_CUSTOM_CLUSTER:
      
      processZosungCustomCluster(ieee_addr, endpoint, command_id, payload_size, payload, rssi); break;
    
    case IKEA_PRIVATE_CLUSTER:
    case IKEA_PRIVATE_CLUSTER_2: {
      
      log_i("IKEA custom cluster(0x%x) on endpoint(0x%x), command(0x%x)", cluster, endpoint, command_id);
      processIkeaSymfoniskCommands(ieee_addr, endpoint, cluster, command_id, payload_size, payload, rssi);
     } break;
    
    default: log_i("Unknown custom cluster(0x%x) command(0x%x)", cluster, command_id); break;
  }
}

void Z2S_onBTCBoundDevice(zbg_device_params_t *device) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, device->ieee_addr);

  log_i("BTC bound device(0x%x) on endpoint(0x%x), cluster id(0x%x)", device->short_addr, device->endpoint, device->cluster_id);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    //log_i(_no_channel_found_str, device->ieee_addr);
    no_channel_found_error_func(ieee_addr_str);
  else
  while (channel_number_slot >= 0)
  {
    device->model_id = z2s_channels_table[channel_number_slot].model_id;
    device->user_data = z2s_channels_table[channel_number_slot].Supla_channel;

    channel_number_slot = Z2S_findChannelNumberNextSlot(channel_number_slot, device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  } 
  //Z2S_addZBDeviceTableSlot(device->ieee_addr, device->short_addr, "unknown","unknown", 0, device->model_id,0);

}


void Z2S_onBoundDevice(zbg_device_params_t *device, bool last_cluster) {
}

uint8_t Z2S_addZ2SDevice(zbg_device_params_t *device, int8_t sub_id, char *name, uint32_t func, char *unit) {

  char ieee_addr_str[24] = {};

  ieee_addr_to_str(ieee_addr_str, device->ieee_addr);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, sub_id);
  
  if (channel_number_slot < 0) {
    log_i("No channel found for address  for %s, adding new one!",ieee_addr_str);
    
    uint8_t first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
    
    if (first_free_slot == 0xFF) {
        devices_table_full_error_func();
        return ADD_Z2S_DEVICE_STATUS_DT_FULL;
    }
    log_i("model id %d, first free slot %d", device->model_id, first_free_slot);
    
    switch (device->model_id) {
      
      case 0x0000: break;
      
      case Z2S_DEVICE_DESC_TUYA_REPEATER: break;

      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_POLL:
      case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR:
      case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
      case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR: 
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10:
        addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR: {
        addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func);
        
        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDevicePressure(device, first_free_slot);
      } break;

      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: 
      case Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR: 
      case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1:
      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B:
      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_B:
      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T:
      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_SONOFF_T_B:
        addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_RELAY:
      case Z2S_DEVICE_DESC_RELAY_1: 
      case Z2S_DEVICE_DESC_LUMI_SWITCH:
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, NO_CUSTOM_CMD_SID, "POWER SWITCH", 
                                                              SUPLA_CHANNELFNC_POWERSWITCH); break;

      case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1:
      case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2: {
        char gang_name[30];
        sprintf(gang_name, "GANG #%d", device->endpoint);
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, NO_CUSTOM_CMD_SID, gang_name, SUPLA_CHANNELFNC_LIGHTSWITCH); 
      } break;

      case Z2S_DEVICE_DESC_ON_OFF:
      case Z2S_DEVICE_DESC_ON_OFF_1: {
        auto Supla_Z2S_VirtualRelay = new Supla::Control::VirtualRelay();
        Z2S_fillChannelsTableSlot(device, first_free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id); 
      } break;

      case Z2S_DEVICE_DESC_TUYA_SWITCH_4X3: {
        char button_name_function[30];
        char button_function[][15] = {"PRESSED", "DOUBLE PRESSED","HELD"};
        sprintf(button_name_function, "BUTTON #%d %s", device->endpoint, button_function[sub_id]); 
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function, SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3: {
        char button_name_function[][30] =  {"BUTTON #1 PRESSED", "BUTTON #1 DOUBLE PRESSED","BUTTON #1 HELD",
                                            "BUTTON #2 PRESSED", "BUTTON #2 DOUBLE PRESSED","BUTTON #2 HELD"};
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function[sub_id], SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_2F:
      case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_3F:
      case Z2S_DEVICE_DESC_TUYA_SMART_BUTTON_5F:
      case Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F: {
        char button_name_function[30];
        char button_function_press[][15] = {"PRESSED", "DOUBLE PRESSED","HELD"};
        char button_function_rotate[][15] = {"ROTATED RIGHT", "ROTATED LEFT"};
        if (sub_id < TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID)
          sprintf(button_name_function, "BUTTON %s", button_function_press[sub_id]);
        else
          sprintf(button_name_function, "BUTTON %s", button_function_rotate[sub_id - TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID]);

        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function, SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON:
      case Z2S_DEVICE_DESC_IKEA_SMART_BUTTON_2F: {
        char button_name_function[30];
        sprintf(button_name_function, IKEA_STYRBAR_BUTTONS[sub_id]);
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function, SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_1:
      case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_1:
      case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_2:
      case Z2S_DEVICE_DESC_IKEA_SYMFONISK_GEN_2_3: {
        char button_name_function[30];
        sprintf(button_name_function, IKEA_SYMFONISK_BUTTONS[sub_id]);
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function, SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1:
      case Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2: {
        char button_name_function[30];
        sprintf(button_name_function, IKEA_SYMFONISK_BUTTONS[sub_id]);
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function, SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_1:
      case Z2S_DEVICE_DESC_PHILIPS_HUE_DIMMER_SWITCH_2: {
        char button_name_function[30];
        sprintf(button_name_function, PHILIPS_HUE_DIMMER_SWITCH_BUTTONS[sub_id]);
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, button_name_function, SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_LUMI_SMART_BUTTON_1F: {
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, "SINGLE", SUPLA_CHANNELFNC_POWERSWITCH);
      } break;

      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_A: {
        
        addZ2SDeviceVirtualRelay(&zbGateway,device, first_free_slot);
        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        if (device->model_id == Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2)
              addZ2SDeviceElectricityMeter(&zbGateway, device, true, true, first_free_slot);
            else if (device->model_id == Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER)
              addZ2SDeviceElectricityMeter(&zbGateway, device, true, false, first_free_slot);
            else
              addZ2SDeviceElectricityMeter(&zbGateway, device, false, false, first_free_slot);
      } break;

      case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER: {
        if (sub_id == TUYA_3PHASES_ELECTRICITY_METER_SID)
          addZ2SDeviceElectricityMeter(&zbGateway, device, false, false, first_free_slot, sub_id, false); 
        else
          addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit);
      } break;

      case Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER: {
        addZ2SDeviceElectricityMeter(&zbGateway, device, false, false, first_free_slot, NO_CUSTOM_CMD_SID, true);
      } break;

      case Z2S_DEVICE_DESC_TUYA_HVAC:
      case Z2S_DEVICE_DESC_TUYA_HVAC_6567C:
      case Z2S_DEVICE_DESC_TUYA_HVAC_23457:
      case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY:
      case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL:
      case Z2S_DEVICE_DESC_TS0601_TRV_ME167:
      case Z2S_DEVICE_DESC_TS0601_TRV_BECA:
      case Z2S_DEVICE_DESC_TS0601_TRV_MOES:
      case Z2S_DEVICE_DESC_TS0601_TRV_TRV601:
      case Z2S_DEVICE_DESC_TS0601_TRV_TRV603:
      case Z2S_DEVICE_DESC_TS0601_TRV_GTZ10:
      case Z2S_DEVICE_DESC_SONOFF_TRVZB: {
      
        addZ2SDeviceTempHumidity(device, first_free_slot);
        uint8_t trv_thermometer_slot = first_free_slot;
        Z2S_setChannelFlags(trv_thermometer_slot, USER_DATA_FLAG_CORRECTIONS_DISABLED);
        
        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceHvac(&zbGateway, device, first_free_slot, trv_thermometer_slot); 
      } break;
      
      case Z2S_DEVICE_DESC_TUYA_LED_DIMMER_F0_E0: {

        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, NO_CUSTOM_CMD_SID, "DIMMER SWITCH", SUPLA_CHANNELFNC_LIGHTSWITCH);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_BRIGHTNESS_SID, "BRIGHTNESS", SUPLA_CHANNELFNC_DIMMER);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_COLOR_TEMPERATURE_SID, "COLOR TEMPERATURE", SUPLA_CHANNELFNC_DIMMER);

      } break; 

      case Z2S_DEVICE_DESC_TUYA_RGB_BULB: {

        addZ2SDeviceRGB(&zbGateway,device, first_free_slot, "RGB BULB", SUPLA_CHANNELFNC_RGBLIGHTING);
      } break;

      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
      case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
      case Z2S_DEVICE_DESC_RGBW_BULB_XY: {
        
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, NO_CUSTOM_CMD_SID, "RGBW SWITCH", SUPLA_CHANNELFNC_LIGHTSWITCH);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_BRIGHTNESS_SID, "BRIGHTNESS", SUPLA_CHANNELFNC_DIMMER);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_COLOR_TEMPERATURE_SID, "COLOR TEMPERATURE", SUPLA_CHANNELFNC_DIMMER);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceRGB(&zbGateway,device, first_free_slot, "RGB", SUPLA_CHANNELFNC_RGBLIGHTING); 
      } break; 

      case Z2S_DEVICE_DESC_IKEA_WW_BULB: {
        
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, NO_CUSTOM_CMD_SID, "BULB SWITCH", SUPLA_CHANNELFNC_LIGHTSWITCH);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_BRIGHTNESS_SID, "BRIGHTNESS", SUPLA_CHANNELFNC_DIMMER);
      } break;

      case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY: {
      
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, NO_CUSTOM_CMD_SID, "RGB SWITCH", SUPLA_CHANNELFNC_LIGHTSWITCH);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceRGB(&zbGateway,device, first_free_slot, "RGB", SUPLA_CHANNELFNC_RGBLIGHTING); 
      } break; 

      case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH:
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, sub_id, "DIMMER SWITCH", SUPLA_CHANNELFNC_DIMMER); break;

      case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR: {

        addZ2SDeviceIASzone(device, first_free_slot, -1, "SMOKE DETECT", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
        //sub_id = -1 has to kept for compatibility reason   
        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, -1, "SMOKE CONC.", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "ppm");
      } break;

      case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1:
      case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2: {

        addZ2SDeviceIASzone(device, first_free_slot, -1, "SMOKE DETECT", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
      } break;

      case Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR:
        addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, -1, "LIGHT ILLU.", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;

      case Z2S_DEVICE_DESC_ILLUZONE_SENSOR: {
        
        addZ2SDeviceIASzone(device, first_free_slot, -1, "LS ZONE", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);

        first_free_slot = Z2S_findFirstFreeChannelsTableSlot();
        if (first_free_slot == 0xFF) {
          devices_table_full_error_func();
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, -1, "LIGHT ILLU.", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx");
      } break;

      case Z2S_DEVICE_DESC_IKEA_VALLHORN_1: 
        addZ2SDeviceActionTrigger(device, first_free_slot, -1, "OCCUPANCY EXEC", SUPLA_CHANNELFNC_STAIRCASETIMER); break;

      case Z2S_DEVICE_DESC_IKEA_VALLHORN_2:  
        addZ2SDeviceIASzone(device, first_free_slot, -1, "OCCUPANCY_DETECTED", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR); break;

      case Z2S_DEVICE_DESC_IKEA_VALLHORN_3: 
        addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "ILLUMINANCE",
                                              SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;

      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR: {
        
        switch (sub_id) {
          case TUYA_PRESENCE_SENSOR_PRESENCE_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, "PRESENCE", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR); break;

          case TUYA_PRESENCE_SENSOR_MOTION_STATE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id,
                                                  "MOTION STATE", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "[0..5]");
            break;
      
          case TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "ILLUMINANCE",
                                                  SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;
        }
      } break;

      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1: {
        
        switch (sub_id) {
          
          case TUYA_PRESENCE_SENSOR_PRESENCE_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

          case TUYA_PRESENCE_SENSOR_TEMPHUMIDITY_SID: 
            addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func); break;
      
          case TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;
        }
      } break;

      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5: {
        
        switch (sub_id) {
          
          case TUYA_PRESENCE_SENSOR_PRESENCE_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, "PRESENCE", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR); break;
      
          case TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "ILLUMINANCE",
                                                  SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;
        }
      } break;

      case Z2S_DEVICE_DESC_SONOFF_SMART_VALVE: {
        
        switch (sub_id) {
          
          case SONOFF_SMART_VALVE_ON_OFF_SID:
           addZ2SDeviceVirtualRelay(&zbGateway, device, first_free_slot, sub_id, name, func); break;
      
          case SONOFF_SMART_VALVE_FLOW_SID: 
          case SONOFF_SMART_VALVE_MODE_SID:
          case SONOFF_SMART_VALVE_CYCLE_NUMBER_SID:
          case SONOFF_SMART_VALVE_CYCLES_COUNT_SID:
          case SONOFF_SMART_VALVE_TIME_SID:
          case SONOFF_SMART_VALVE_PAUSE_SID:
          case SONOFF_SMART_VALVE_VOLUME_SID:
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;
        }
      } break; 
      
      case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR: {
        
        switch (sub_id) {
          
          case TUYA_RAIN_SENSOR_RAIN_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, "RAIN", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR); break;

          case TUYA_RAIN_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id,
                                                  "ILLUMINANCE", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "mV");
            break;

          case TUYA_RAIN_SENSOR_ILLUMINANCE_AVG_20_MIN_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "ILLUMINANCE AVG 20",
                                                  SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "mV"); break;

          case TUYA_RAIN_SENSOR_ILLUMINANCE_MAX_TODAY_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "ILLUMINANCE MAX TODAY",
                                                  SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "mV"); break;

          case TUYA_RAIN_SENSOR_RAIN_INTENSITY_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "RAIN INTENSITY",
                                                  SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "mV"); break;
        }
      } break;

      case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2: {
        
        switch (sub_id) {
          
          case TUYA_RAIN_SENSOR_RAIN_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, "RAIN", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR); break;

          case TUYA_RAIN_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id,
                                                  "ILLUMINANCE", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;
        }
      } break;

      case Z2S_DEVICE_DESC_ADEO_SMART_PIRTH_SENSOR: {
        
        switch (sub_id) {
          
          case IAS_ZONE_ALARM_1_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

          case IAS_ZONE_TAMPER_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

          case ADEO_SMART_PIRTH_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;

          case ADEO_SMART_PIRTH_SENSOR_TEMPHUMI_SID: 
            addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func); break;
        }
      } break;

      case Z2S_DEVICE_DESC_LUMI_MOTION_SENSOR: {
        
        switch (sub_id) {
          
          case LUMI_MOTION_SENSOR_OCCUPANCY_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

          case LUMI_MOTION_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;
        }
      } break;

      case Z2S_DEVICE_DESC_SONOFF_PIR_SENSOR: {
        
        addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;
      } break;

      case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: 
        addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR: 
        addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR: {

        switch (sub_id) {
          
          case TUYA_CO_DETECTOR_CO_SID:
          case TUYA_CO_DETECTOR_SILENCE_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;
          case TUYA_CO_DETECTOR_CO_CONC_SID:
          case TUYA_CO_DETECTOR_SELF_TEST_SID:
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;
        }
      } break;

      case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR: {

        switch (sub_id) {
          
          case TUYA_GAS_DETECTOR_GAS_SID:
          case TUYA_GAS_DETECTOR_PREHEAT_SID:
          //case TUYA_GAS_DETECTOR_SILENCE_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;
          case TUYA_GAS_DETECTOR_GAS_CONC_SID:
          case TUYA_GAS_DETECTOR_SELF_TEST_RESULT_SID:
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;
        }
      } break;

      case Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR: {

        switch (sub_id) {
          
          case TUYA_AIR_QUALITY_SENSOR_TEMPHUMIDITY_SID:
            addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func); break;
          case TUYA_AIR_QUALITY_SENSOR_CO2_SID:
          case TUYA_AIR_QUALITY_SENSOR_VOC_SID:
          case TUYA_AIR_QUALITY_SENSOR_FA_SID:
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit); break;
        }
      } break;

      case Z2S_DEVICE_DESC_IR_REMOTE_CONTROL:
        addZ2SDeviceVirtualRelay(&zbGateway, device, first_free_slot, NO_CUSTOM_CMD_SID, "IR REMOTE", 0); break;

      case Z2S_DEVICE_DESC_TUYA_WINDOW_COVERING_SINGLE:
        addZ2SDeviceVirtualRelay(&zbGateway, device, first_free_slot, NO_CUSTOM_CMD_SID, 
                                "ROLLER SHUTTER", SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER); break;

      case Z2S_DEVICE_DESC_LORATAP_WINDOW_COVERING_SINGLE:
        if (device->endpoint == 1)
          addZ2SDeviceVirtualRelay(&zbGateway, device, first_free_slot, NO_CUSTOM_CMD_SID, 
                                  "ROLLER SHUTTER", SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER); break;

      case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR:
        addZ2SDeviceVirtualRelay(&zbGateway, device, first_free_slot, NO_CUSTOM_CMD_SID, 
                                "ROLLER SHUTTER", SUPLA_CHANNELFNC_CONTROLLINGTHEROLLERSHUTTER); break;
        
      case Z2S_DEVICE_DESC_TUYA_SIREN_ALARM:
        addZ2SDeviceVirtualRelay(&zbGateway, device,first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_MOES_ALARM:
        if (sub_id == MOES_ALARM_DISPLAY_SID)
          addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func);
        else      
          addZ2SDeviceVirtualRelay(&zbGateway, device,first_free_slot, sub_id, name, func); break;
        
      case Z2S_DEVICE_DESC_ADEO_IAS_ACE_SMART_BUTTON_4F:
      case Z2S_DEVICE_DESC_ADEO_SMART_BUTTON_3F: 
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, name, SUPLA_CHANNELFNC_POWERSWITCH); break;

      case Z2S_DEVICE_DESC_ON_OFF_VALVE_DC: 
      case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY:
        addZ2SDeviceVirtualValve(&zbGateway, device, first_free_slot, sub_id, "VALVE", SUPLA_CHANNELFNC_VALVE_OPENCLOSE); break;
      
      case Z2S_DEVICE_DESC_LUMI_CUBE_T1_PRO: 
        addZ2SDeviceActionTrigger(device, first_free_slot, sub_id, "CUBE T1 PRO", SUPLA_CHANNELFNC_POWERSWITCH); break;

      default : {
        
        log_i("Device (0x%x), endpoint (0x%x), model (0x%x) unknown", device->short_addr, device->endpoint, device->model_id);
        return ADD_Z2S_DEVICE_STATUS_DUN;
      } break;
    }
    return ADD_Z2S_DEVICE_STATUS_OK;
  } else {
    log_i("Device (0x%x), endpoint (0x%x) already in z2s_channels_table (index 0x%x)", device->short_addr, device->endpoint, channel_number_slot);
    if (z2s_channels_table[channel_number_slot].short_addr != device->short_addr) {
      z2s_channels_table[channel_number_slot].short_addr = device->short_addr;
      Z2S_saveChannelsTable();
      log_i("Device short address updated");
    }
    return ADD_Z2S_DEVICE_STATUS_DAP; 
  }
}

void updateTimeout(uint8_t device_id, uint8_t timeout, uint8_t selector, uint32_t timings_secs) {
  
  if (timeout > 0) {
    //z2s_channels_table[device_id].user_data_flags |= USER_DATA_FLAG_SED_TIMEOUT;
    //z2s_channels_table[device_id].user_data_1 = timeout;
    z2s_channels_table[device_id].timeout_secs = timeout * 3600;
  }
  else {
    if (selector & 1)
      z2s_channels_table[device_id].keep_alive_secs = timings_secs;
    if (selector & 2)
      z2s_channels_table[device_id].timeout_secs = timings_secs;
    if (selector & 4)
      z2s_channels_table[device_id].refresh_secs = timings_secs;
  }

  
  if (Z2S_saveChannelsTable()) {
    log_i("Device(channel %d) timeout updated. Table saved successfully.", z2s_channels_table[device_id].Supla_channel);
      
    auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[device_id].Supla_channel);
      
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

      auto Supla_Z2S_VirtualBinary = reinterpret_cast<Supla::Sensor::Z2S_VirtualBinary *>(element);
      if (timeout > 0)
        Supla_Z2S_VirtualBinary->setTimeout(timeout);
      if (selector & 4)
        Supla_Z2S_VirtualBinary->setAutoSetSecs(timings_secs);
    } else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR) {

      auto Supla_Z2S_VirtualThermHygroMeter = reinterpret_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
      Supla_Z2S_VirtualThermHygroMeter->setTimeout(timeout);
    } else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RELAY) {

      auto Supla_Z2S_VirtualRelay = reinterpret_cast<Supla::Control::Z2S_VirtualRelay *>(element);
      if (selector & 1)
        Supla_Z2S_VirtualRelay->setKeepAliveSecs(timings_secs);
      if (selector & 2)
        Supla_Z2S_VirtualRelay->setTimeoutSecs(timings_secs);
    }  else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ELECTRICITY_METER) {

      auto Supla_Z2S_ElectricityMeter = reinterpret_cast<Supla::Sensor::Z2S_ElectricityMeter *>(element);
      if (selector & 1)
        Supla_Z2S_ElectricityMeter->setKeepAliveSecs(timings_secs);
      if (selector & 2)
        Supla_Z2S_ElectricityMeter->setTimeoutSecs(timings_secs);
      if (selector & 4)
        Supla_Z2S_ElectricityMeter->setRefreshSecs(timings_secs);
    } else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_DIMMER) {

      auto Supla_Z2S_DimmerInterface = reinterpret_cast<Supla::Control::Z2S_DimmerInterface *>(element);
      if (selector & 1)
        Supla_Z2S_DimmerInterface->setKeepAliveSecs(timings_secs);
      if (selector & 2)
        Supla_Z2S_DimmerInterface->setTimeoutSecs(timings_secs);
    }  else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER) {

      auto Supla_Z2S_RGBInterface = reinterpret_cast<Supla::Control::Z2S_RGBInterface *>(element);
      if (selector & 1)
        Supla_Z2S_RGBInterface->setKeepAliveSecs(timings_secs);
      if (selector & 2)
        Supla_Z2S_RGBInterface->setTimeoutSecs(timings_secs);
    } else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_HVAC) {

      auto Supla_Z2S_HvacBaseEE = reinterpret_cast<Supla::Control::HvacBaseEE*>(element);
      auto Supla_Z2S_TRVInterface = reinterpret_cast<Supla::Control::Z2S_TRVInterface*>(Supla_Z2S_HvacBaseEE->getPrimaryOutputEE());
      //if (selector & 1)
      //  Supla_Z2S_RGBInterface->setKeepAliveSecs(timings_secs);
      if (Supla_Z2S_TRVInterface && (selector & 2))
        Supla_Z2S_TRVInterface->setTimeoutSecs(timings_secs);
    }
    else
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_ACTIONTRIGGER) {

      auto Supla_Z2S_ActionTrigger = reinterpret_cast<Supla::Control::VirtualRelaySceneSwitch *>(element);
      //if (selector & 1)
      //  Supla_Z2S_RGBInterface->setKeepAliveSecs(timings_secs);
      if (Supla_Z2S_ActionTrigger && (selector & 4))
        Supla_Z2S_ActionTrigger->setDebounceTimeMs(timings_secs);
    }
  }
}
void updateRGBMode(uint8_t device_id, uint8_t rgb_mode) {

  if (z2s_channels_table[device_id].Supla_channel_type == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER) {

    z2s_channels_table[device_id].user_data_1 = rgb_mode;

    if (Z2S_saveChannelsTable()) {
      log_i("Device(channel %d) RGB mode updated. Table saved successfully.", z2s_channels_table[device_id].Supla_channel);
      
      auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[device_id].Supla_channel);

      if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER) {

        auto Supla_Z2S_RGBInterface = reinterpret_cast<Supla::Control::Z2S_RGBInterface *>(element);
        Supla_Z2S_RGBInterface->setRGBMode(rgb_mode);
      }
    }
  }
  else
    log_i("RGB mode update only allowed for SUPLA_CHANNELTYPE_RGBLEDCONTROLLER");
}

void updateDeviceTemperature(uint8_t device_id, int32_t temperature) {
  
      auto element = Supla::Element::getElementByChannelNumber(device_id);

      if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {

        auto VT1 = reinterpret_cast<Supla::Sensor::VirtualThermometer *>(element);
        VT1->setValue(double(temperature)/100);
      }
  else
    log_i("set temperature only allowed for virtual thermometer");
}

bool Z2S_add_action(char *action_name, uint8_t src_channel_id, uint16_t Supla_action, uint8_t dst_channel_id, uint16_t Supla_event, bool condition, 
                    double threshold_1, double threshold_2) {

  auto src_element = Supla::Element::getElementByChannelNumber(src_channel_id);
  if (src_element == nullptr) {
    log_e("Invalid source Supla channel %d", src_channel_id);
    return false;
  }
  auto Supla_Z2S_ActionHandler = reinterpret_cast<Supla::ElementWithChannelActions *>(src_element);

  auto dst_element = Supla::Element::getElementByChannelNumber(dst_channel_id);
  if (dst_element == nullptr) {
    log_e("Invalid destination Supla channel %d", dst_channel_id);
    return false;
  }
  log_i("Action name %s, src channel %u, dst channel %u, event %u, action %u", action_name, src_channel_id, dst_channel_id, Supla_event, Supla_action);
  
  Supla::Condition *Supla_condition = nullptr;

  if (condition) {

    switch (Supla_event) {
      case Supla::ON_LESS:
        Supla_condition = OnLess(threshold_1); break;
      case Supla::ON_LESS_EQ:
        Supla_condition = OnLessEq(threshold_1); break;
      case Supla::ON_GREATER:
        Supla_condition = OnGreater(threshold_1); break;
      case Supla::ON_GREATER_EQ:
        Supla_condition = OnGreaterEq(threshold_1); break;
      case Supla::ON_BETWEEN:
        Supla_condition = OnBetween(threshold_1, threshold_2); break;
      case Supla::ON_BETWEEN_EQ:
        Supla_condition = OnBetweenEq(threshold_1, threshold_2); break;
      case Supla::ON_EQUAL:
        Supla_condition = OnEqual(threshold_1); break;
    }
    if (Supla_condition == nullptr)
      return false;
  }
  
  switch (dst_element->getChannel()->getChannelType()) {
    case SUPLA_CHANNELTYPE_RELAY: {
      auto Supla_Z2S_ActionClient = reinterpret_cast<Supla::Control::Relay *>(dst_element);
      if (condition)
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_condition);
      else
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_event);
    } break; 

    case SUPLA_CHANNELTYPE_BINARYSENSOR: {
      auto Supla_Z2S_ActionClient = reinterpret_cast<Supla::Sensor::VirtualBinary *>(dst_element);
      if (condition)
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_condition);
      else
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_event);
    } break;

    case SUPLA_CHANNELTYPE_HVAC: {
      auto Supla_Z2S_ActionClient = reinterpret_cast<Supla::Control::HvacBase*>(dst_element);
      if (condition)
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_condition);
      else
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_event);
    } break;

    case SUPLA_CHANNELTYPE_DIMMER: {
      auto Supla_Z2S_ActionClient = reinterpret_cast<Supla::Control::Z2S_DimmerInterface*>(dst_element);
      if (condition)
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_condition);
      else
        Supla_Z2S_ActionHandler->addAction(Supla_action, Supla_Z2S_ActionClient, Supla_event);
    } break;

    default: return false; 
  } 
  return true;
}

void updateSuplaBatteryLevel(int16_t channel_number_slot, uint8_t msg_id, uint32_t msg_value, signed char rssi, bool restore) {

  uint8_t battery_level = 0xFF;  

  uint8_t zb_device_number_slot = Z2S_findZBDeviceTableSlot(z2s_channels_table[channel_number_slot].ieee_addr);

  uint8_t battery_voltage_max  = 33;
  uint8_t battery_voltage_min  = 28;

  if (zb_device_number_slot < 0xFF) {
    
    if (z2s_zb_devices_table[zb_device_number_slot].battery_voltage_max > 0)
       battery_voltage_max = z2s_zb_devices_table[zb_device_number_slot].battery_voltage_max; 
    
    if (z2s_zb_devices_table[zb_device_number_slot].battery_voltage_min > 0)
       battery_voltage_min = z2s_zb_devices_table[zb_device_number_slot].battery_voltage_min;

  }
  
  uint16_t battery_voltage_step = 1000 / (battery_voltage_max - battery_voltage_min);

  switch (msg_id) {
    case ZBD_BATTERY_PERCENTAGE_MSG:
      battery_level = msg_value / 2; break;
    case ZBD_BATTERY_VOLTAGE_MSG:
      battery_level = (1000 - (battery_voltage_step * (battery_voltage_max - msg_value))) /10; break;
    case ZBD_BATTERY_LEVEL_MSG:
      battery_level = msg_value; break;
    case ZBD_BATTERY_STATE_MSG:
      battery_level = msg_value; break;
    case ZBD_LOW_BATTERY_MSG:
      battery_level = msg_value; break;
    case ZBD_BATTERY_RESTORE_MSG:
      battery_level = (msg_value < 0xFF) ? (msg_value - 0x80) : 0xFF; break;
  }
  if (!restore)
    Z2S_updateZBDeviceLastSeenMs(z2s_channels_table[channel_number_slot].ieee_addr, millis());

  if (zb_device_number_slot < 0xFF) {
    if (z2s_zb_devices_table[zb_device_number_slot].user_data_flags & 
        ZBD_USER_DATA_FLAG_DISABLE_BATTERY_MSG)
      battery_level = 0xFF;
    if ((msg_id == ZBD_BATTERY_VOLTAGE_MSG) &&
        (z2s_zb_devices_table[zb_device_number_slot].user_data_flags & 
        ZBD_USER_DATA_FLAG_DISABLE_BATTERY_VOLTAGE_MSG))
      battery_level = 0xFF;
    if ((msg_id == ZBD_BATTERY_PERCENTAGE_MSG) &&
        (z2s_zb_devices_table[zb_device_number_slot].user_data_flags & 
        ZBD_USER_DATA_FLAG_DISABLE_BATTERY_PERCENTAGE_MSG))
      battery_level = 0xFF;

    if (battery_level < 0x65) //0x00...0x64 0-100%
      z2s_zb_devices_table[zb_device_number_slot].battery_percentage = 0x80 + battery_level;
    else 
      z2s_zb_devices_table[zb_device_number_slot].battery_percentage = 0xFF;
  
    //Z2S_saveZBDevicesTable(); //switched off - restore not working
  } 
    
  /*if ((z2s_channels_table[channel_number_slot].model_id == Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10) &&
      (id == ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID)) {
    log_i("Temporary fix for inaccurate voltage scaling"); //TODO add battery_voltage_max, battery_voltage_min for different models
    return;
  }*/

  while (channel_number_slot >= 0)
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_channels_table[channel_number_slot].Supla_channel);
 
    if (element) {
      
      if (battery_level < 0xFF)
        element->getChannel()->setBatteryLevel(battery_level);

        if (restore)
          break; //only actual channel since rest will be scanned anyway in initSuplaChannels()
      
        switch (element->getChannel()->getChannelType()) {
          case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:{
            auto Supla_Z2S_VirtualThermHygroMeter = reinterpret_cast<Supla::Sensor::Z2S_VirtualThermHygroMeter *>(element);
            Supla_Z2S_VirtualThermHygroMeter->Refresh();

          } break;
          case SUPLA_CHANNELTYPE_PRESSURESENSOR: break; //TODO add timeout 
          case SUPLA_CHANNELTYPE_BINARYSENSOR:{
            auto Supla_Z2S_VirtualBinary = reinterpret_cast<Supla::Sensor::Z2S_VirtualBinary *>(element);
            Supla_Z2S_VirtualBinary->Refresh();
          } break;
        }    
    }
    channel_number_slot = Z2S_findChannelNumberNextSlot(channel_number_slot, z2s_channels_table[channel_number_slot].ieee_addr, -1, -1, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  }
}

bool hasTuyaCustomCluster(uint32_t model_id) {
  
  switch (model_id) {

    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1:
    case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_2:
    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR: 
    case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
    case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5:
    case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_4IN1:
    case Z2S_DEVICE_DESC_TUYA_CO_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_GAS_DETECTOR:
    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR:
    case Z2S_DEVICE_DESC_TUYA_RAIN_SENSOR_2:
    case Z2S_DEVICE_DESC_TUYA_EF00_SWITCH_2X3:
    case Z2S_DEVICE_DESC_TUYA_3PHASES_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_TUYA_1PHASE_ELECTRICITY_METER:
    case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH:
    case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL:
    case Z2S_DEVICE_DESC_TS0601_TRV_ME167:
    case Z2S_DEVICE_DESC_TS0601_TRV_BECA:
    case Z2S_DEVICE_DESC_TS0601_TRV_MOES:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV601:
    case Z2S_DEVICE_DESC_TS0601_TRV_TRV603:
    case Z2S_DEVICE_DESC_TS0601_TRV_GTZ10:
    case Z2S_DEVICE_DESC_TUYA_ON_OFF_VALVE_BATTERY:
    case Z2S_DEVICE_DESC_TUYA_VIBRATION_SENSOR:
    case Z2S_DEVICE_DESC_MOES_ALARM:
    case Z2S_DEVICE_DESC_MOES_SHADES_DRIVE_MOTOR:
    case Z2S_DEVICE_DESC_TUYA_AIR_QUALITY_SENSOR:
      return true;
    default:
      return false;
  }  
}

void log_i_telnet2(char *log_line, bool toTelnet) {
#ifdef USE_TELNET_CONSOLE
  log_i_telnet(log_line, toTelnet);
#endif //USE_TELNET_CONSOLE
  log_i("%s", log_line);
}

void onTuyaCustomClusterReceive(void (*callback)(uint8_t command_id, uint16_t payload_size, uint8_t * payload_data)) {

  _on_Tuya_custom_cluster_receive = callback;
}