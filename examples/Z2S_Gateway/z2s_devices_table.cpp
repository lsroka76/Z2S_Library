

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

#include <SuplaDevice.h>

#include <supla/control/virtual_relay.h>
#include <supla/sensor/virtual_thermometer.h>

#include <Z2S_control/Z2S_virtual_relay.h>
#include <Z2S_control/Z2S_virtual_relay_scene_switch.h>


extern ZigbeeGateway zbGateway;

z2s_device_params_t    z2s_devices_table[Z2S_CHANNELMAXCOUNT];

z2s_zb_device_params_t z2s_zb_devices_table[Z2S_ZBDEVICESMAXCOUNT];

static uint32_t Styrbar_timer = 0;
static bool     Styrbar_ignore_button_1 = false;

uint32_t Z2S_getDevicesTableSize() {
  uint32_t _z2s_devices_table_size;
  if (Supla::Storage::ConfigInstance()->getUInt32(Z2S_DEVICES_TABLE_SIZE, &_z2s_devices_table_size))
    return _z2s_devices_table_size;
  else
    return 0;
}

uint8_t Z2S_findFirstFreeDevicesTableSlot(uint8_t start_slot) {

  for (uint8_t devices_counter = start_slot; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
      if (!z2s_devices_table[devices_counter].valid_record)
        return devices_counter;
  return 0xFF;
  
}

void Z2S_printDevicesTableSlots(bool toTelnet) {

  
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) 
    if (z2s_devices_table[devices_counter].valid_record) {
      char log_line[1024];

      sprintf(log_line,"ENTRY\t\t\t%u\n\rIEEE ADDRESS\t\t%X:%X:%X:%X:%X:%X:%X:%X\n\rSHORT ADDRESS\t\t0x%X\n\rENDPOINT\t\t0x%X\n\rCLUSTER\t\t\t0x%X\n\r"
            "MODEL\t\t\t%X\n\r"
            "SUPLA CHANNEL\t\t%u\n\rSUPLA SECONDARY CHANNEL\t%u\n\rSUPLA CHANNEL TYPE\t%ld\n\r"
            "SUPLA CHANNEL NAME\t%s\n\rSUPLA CHANNEL FUNCTION\t%lu\n\r"
            "SUB ID\t\t\t%d\n\rUSER FLAGS\t\t%lu\n\r"
            "USER DATA(1)\t\t%lu\n\rUSER DATA(2)\t\t%lu\n\rUSER DATA(3)\t\t%lu\n\rUSER DATA(4)\t\t%lu\n\r"
            "KEEP ALIVE(S)\t\t%lu\n\rTIMEOUT(S)\t\t%lu\n\rREFRESH(S)\t\t%lu\n\rZB device id\t\t%u\n\r",
        devices_counter,
        z2s_devices_table[devices_counter].ieee_addr[7], z2s_devices_table[devices_counter].ieee_addr[6], 
        z2s_devices_table[devices_counter].ieee_addr[5], z2s_devices_table[devices_counter].ieee_addr[4], 
        z2s_devices_table[devices_counter].ieee_addr[3], z2s_devices_table[devices_counter].ieee_addr[2], 
        z2s_devices_table[devices_counter].ieee_addr[1], z2s_devices_table[devices_counter].ieee_addr[0],
        z2s_devices_table[devices_counter].short_addr,
        z2s_devices_table[devices_counter].endpoint,
        z2s_devices_table[devices_counter].cluster_id,
        z2s_devices_table[devices_counter].model_id,
        z2s_devices_table[devices_counter].Supla_channel,
        z2s_devices_table[devices_counter].Supla_secondary_channel,
        z2s_devices_table[devices_counter].Supla_channel_type,
        z2s_devices_table[devices_counter].Supla_channel_name,
        z2s_devices_table[devices_counter].Supla_channel_func,
        z2s_devices_table[devices_counter].sub_id,
        z2s_devices_table[devices_counter].user_data_flags,
        z2s_devices_table[devices_counter].user_data_1,
        z2s_devices_table[devices_counter].user_data_2,
        z2s_devices_table[devices_counter].user_data_3,
        z2s_devices_table[devices_counter].user_data_4,
        z2s_devices_table[devices_counter].keep_alive_secs,
        z2s_devices_table[devices_counter].timeout_secs,
        z2s_devices_table[devices_counter].refresh_secs,
        z2s_devices_table[devices_counter].ZB_device_id);
 
      log_i_telnet(log_line, toTelnet);
    }
}

int16_t Z2S_findChannelNumberSlot(esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, int32_t channel_type, int8_t sub_id) {

  log_i("Z2S_findChannelNumberSlot 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, channel type 0x%x", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, channel_type);
  
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record)
        if ((memcmp(z2s_devices_table[devices_counter].ieee_addr, ieee_addr,8) == 0) && 
        ((endpoint < 0) || (z2s_devices_table[devices_counter].endpoint == endpoint)) &&
        ((channel_type < 0) || (z2s_devices_table[devices_counter].Supla_channel_type == channel_type)) &&
        ((sub_id < 0) || (z2s_devices_table[devices_counter].sub_id == sub_id))) { 
        //&& (z2s_devices_table[devices_counter].cluster_id == cluster)) {
            return devices_counter;
        }

  }  
  return -1;
}

int16_t Z2S_findChannelNumberNextSlot(int16_t prev_slot, esp_zb_ieee_addr_t ieee_addr, int16_t endpoint, uint16_t cluster, 
                                      int32_t channel_type, int8_t sub_id) {

  log_i("Z2S_findChannelNumberNextSlot 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, channel type 0x%x", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, channel_type);
  
  for (uint8_t devices_counter = prev_slot + 1; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record)
        if ((memcmp(z2s_devices_table[devices_counter].ieee_addr, ieee_addr,8) == 0) && 
        ((endpoint < 0) || (z2s_devices_table[devices_counter].endpoint == endpoint)) &&
        ((channel_type < 0) || (z2s_devices_table[devices_counter].Supla_channel_type == channel_type)) &&
        ((sub_id < 0) || (z2s_devices_table[devices_counter].sub_id == sub_id))) { 
        //&& (z2s_devices_table[devices_counter].cluster_id == cluster)) {
            return devices_counter;
        }
  }  
  return -1;
}

int16_t Z2S_findTableSlotByChannelNumber(uint8_t channel_id) {
  
  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) {
      if ((z2s_devices_table[devices_counter].valid_record) && (z2s_devices_table[devices_counter].Supla_channel == channel_id))
        return devices_counter;
  }
  return -1;
}

void Z2S_fillDevicesTableSlot(zbg_device_params_t *device, uint8_t slot, uint8_t channel, int32_t channel_type, int8_t sub_id,
                              char *name, uint32_t func, uint8_t secondary_channel) {

  z2s_devices_table[slot].valid_record = true;
  memcpy(z2s_devices_table[slot].ieee_addr,device->ieee_addr,8);
  z2s_devices_table[slot].short_addr = device->short_addr;
  z2s_devices_table[slot].model_id = device->model_id;
  z2s_devices_table[slot].endpoint = device->endpoint;
  z2s_devices_table[slot].cluster_id = device->cluster_id;
  z2s_devices_table[slot].Supla_channel = channel;
  z2s_devices_table[slot].Supla_secondary_channel = secondary_channel;
  z2s_devices_table[slot].Supla_channel_type = channel_type;
  z2s_devices_table[slot].sub_id = sub_id; 
  if (name) strcpy(z2s_devices_table[slot].Supla_channel_name, name);
  z2s_devices_table[slot].Supla_channel_func = func;
  z2s_devices_table[slot].ZB_device_id = Z2S_updateZBDeviceTableSlot(z2s_devices_table[slot].ieee_addr, z2s_devices_table[slot].Supla_channel);
  Z2S_saveDevicesTable();

  //Z2S_printDevicesTableSlots();
}

bool Z2S_loadDevicesTable() {

  
  uint32_t z2s_devices_table_size =  Z2S_getDevicesTableSize(); 
  log_i("Z2S_getDevicesTableSize %d, sizeof(z2s_devices_table) %d, sizeof(z2s_device_params_t) %d, sizeof(bool)%d",
  z2s_devices_table_size, sizeof(z2s_devices_table), sizeof(z2s_device_params_t), sizeof(bool));

  if (z2s_devices_table_size == 0) {

      log_i(" No devices table found, writing empty one with size %d", sizeof(z2s_devices_table));
      
      memset(z2s_devices_table,0,sizeof(z2s_devices_table));
      
      if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
        log_i ("Devices table write failed!");
        return false;
      }
      else { 
        if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_DEVICES_TABLE_SIZE, sizeof(z2s_devices_table))) {
          Supla::Storage::ConfigInstance()->commit();
          return true;
        }
        else { 
          log_i ("Devices table size write failed!");
          return false;
        }
      }
  } else
  {
    if (z2s_devices_table_size != sizeof(z2s_devices_table)) {

      if (z2s_devices_table_size == 0x2200) { //legacy 0.4.2
        log_i("Previous version of devices table detected with size 0x%x, trying to upgrade", z2s_devices_table_size);
        z2s_legacy_device_params_t *z2s_devices_legacy_table = (z2s_legacy_device_params_t *)malloc(z2s_devices_table_size);
        if (z2s_devices_legacy_table == nullptr) {
          log_e("Error while allocating memory for legacy table copying");
          return false;
        }
        else {
          if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_legacy_table, z2s_devices_table_size)) {
            log_i ("Legacy devices table load failed!");
            return false;
          }
          for (uint8_t table_index = 0; table_index < Z2S_CHANNELMAXCOUNT; table_index++) {

              z2s_devices_table[table_index].valid_record = (z2s_devices_legacy_table + table_index)->valid_record;
              z2s_devices_table[table_index].model_id = (z2s_devices_legacy_table + table_index)->model_id;
              memcpy(z2s_devices_table[table_index].ieee_addr, (z2s_devices_legacy_table + table_index)->ieee_addr,
                     sizeof(esp_zb_ieee_addr_t));
              z2s_devices_table[table_index].endpoint = (z2s_devices_legacy_table + table_index)->endpoint;
              z2s_devices_table[table_index].cluster_id = (z2s_devices_legacy_table + table_index)->cluster_id;
              z2s_devices_table[table_index].short_addr = (z2s_devices_legacy_table + table_index)->short_addr;
              z2s_devices_table[table_index].Supla_channel = (z2s_devices_legacy_table + table_index)->Supla_channel;
              z2s_devices_table[table_index].Supla_secondary_channel = 0xFF;
              z2s_devices_table[table_index].Supla_channel_type = (z2s_devices_legacy_table + table_index)->Supla_channel_type;
              memcpy(z2s_devices_table[table_index].Supla_channel_name, (z2s_devices_legacy_table + table_index)->Supla_channel_name, 
                     sizeof(z2s_devices_table[table_index].Supla_channel_name));
              z2s_devices_table[table_index].Supla_channel_func = (z2s_devices_legacy_table + table_index)->Supla_channel_func;
              z2s_devices_table[table_index].sub_id = (z2s_devices_legacy_table + table_index)->sub_id;
              z2s_devices_table[table_index].user_data_1 = 0;
              z2s_devices_table[table_index].user_data_2 = 0;
              z2s_devices_table[table_index].user_data_3 = 0;
              z2s_devices_table[table_index].user_data_4 = 0;
              z2s_devices_table[table_index].keep_alive_secs = 0;
              z2s_devices_table[table_index].timeout_secs = 0;
              z2s_devices_table[table_index].refresh_secs = 0;
              z2s_devices_table[table_index].data_counter = 0;
              z2s_devices_table[table_index].ZB_device_id = 0xFF;
            }
          log_i("Devices table upgrade completed - saving new table");
          Z2S_saveDevicesTable();
          Z2S_printDevicesTableSlots();
          free(z2s_devices_legacy_table);
          return true;
        }
      } else
      if (z2s_devices_table_size == 0x2600) { //legacy 0.6.xx
        log_i("Previous version of devices table detected with size 0x%x, trying to upgrade", z2s_devices_table_size);
        z2s_legacy_2_device_params_t *z2s_devices_legacy_2_table = (z2s_legacy_2_device_params_t *)malloc(z2s_devices_table_size);
        if (z2s_devices_legacy_2_table == nullptr) {
          log_e("Error while allocating memory for legace table copying");
          return false;
        }
        else {
          if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_legacy_2_table, z2s_devices_table_size)) {
            log_i ("Legacy devices table load failed!");
            return false;
          }
          for (uint8_t table_index = 0; table_index < Z2S_CHANNELMAXCOUNT; table_index++) {

              z2s_devices_table[table_index].valid_record = (z2s_devices_legacy_2_table + table_index)->valid_record;
              z2s_devices_table[table_index].model_id = (z2s_devices_legacy_2_table + table_index)->model_id;
              memcpy(z2s_devices_table[table_index].ieee_addr, (z2s_devices_legacy_2_table + table_index)->ieee_addr,
                     sizeof(esp_zb_ieee_addr_t));
              z2s_devices_table[table_index].endpoint = (z2s_devices_legacy_2_table + table_index)->endpoint;
              z2s_devices_table[table_index].cluster_id = (z2s_devices_legacy_2_table + table_index)->cluster_id;
              z2s_devices_table[table_index].short_addr = (z2s_devices_legacy_2_table + table_index)->short_addr;
              z2s_devices_table[table_index].Supla_channel = (z2s_devices_legacy_2_table + table_index)->Supla_channel;
              z2s_devices_table[table_index].Supla_secondary_channel = 0xFF;
              z2s_devices_table[table_index].Supla_channel_type = (z2s_devices_legacy_2_table + table_index)->Supla_channel_type;
              memcpy(z2s_devices_table[table_index].Supla_channel_name, (z2s_devices_legacy_2_table + table_index)->Supla_channel_name, 
                     sizeof(z2s_devices_table[table_index].Supla_channel_name));
              z2s_devices_table[table_index].Supla_channel_func = (z2s_devices_legacy_2_table + table_index)->Supla_channel_func;
              z2s_devices_table[table_index].sub_id = (z2s_devices_legacy_2_table + table_index)->sub_id;
              z2s_devices_table[table_index].user_data_flags = 0; //(z2s_devices_legacy_2_table + table_index)->user_data_1;
              z2s_devices_table[table_index].user_data_1 = 0; //(z2s_devices_legacy_2_table + table_index)->user_data_2;
              z2s_devices_table[table_index].user_data_2 = 0; 
              z2s_devices_table[table_index].user_data_3 = 0;
              z2s_devices_table[table_index].user_data_4 = 0;
              z2s_devices_table[table_index].keep_alive_secs = 0;
              z2s_devices_table[table_index].timeout_secs = 0;
              if ((z2s_devices_legacy_2_table + table_index)->user_data_1 & USER_DATA_FLAG_SED_TIMEOUT == USER_DATA_FLAG_SED_TIMEOUT)
                z2s_devices_table[table_index].timeout_secs = (z2s_devices_legacy_2_table + table_index)->user_data_2 * 3600;
              z2s_devices_table[table_index].refresh_secs = 0;
              z2s_devices_table[table_index].data_counter = 0;
              z2s_devices_table[table_index].ZB_device_id = 0xFF;
            }
          log_i("Devices table upgrade completed - saving new table");
          Z2S_saveDevicesTable();
          Z2S_printDevicesTableSlots();
          free(z2s_devices_legacy_2_table);
          return true;
        }
      } else {
        log_i("Devices table size mismatch %d <> %d, no upgrade is possible", z2s_devices_table_size, sizeof(z2s_devices_table));
        return false;
      }
    }
    else {
        if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
          log_i ("Devices table load failed!");
          return false;
        } else {
          log_i ("Devices table load success!");
          Z2S_printDevicesTableSlots();
          return true;
        }
    }
  }
}

bool Z2S_saveDevicesTable() {

  if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_DEVICES_TABLE, (char *)z2s_devices_table, sizeof(z2s_devices_table))) {
    log_i("Devices table write failed!");
    return false;
  }
  else { 
    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_DEVICES_TABLE_SIZE, sizeof(z2s_devices_table))) {
      log_i("Devices table new size(%d) write success!", sizeof(z2s_devices_table));
      Supla::Storage::ConfigInstance()->commit();
      return true;
    } else { 
      log_i("Devices table size write failed!");
      return false;
    }
  }
}

void Z2S_clearDevicesTable() {
  log_i("Clear devices table");
  memset(z2s_devices_table,0,sizeof(z2s_devices_table));
  Z2S_saveDevicesTable();
}

/* ZB_DEVICE_FUNCTIONS */

uint8_t Z2S_findFirstFreeZBDevicesTableSlot(uint8_t start_slot) {

  for (uint8_t devices_counter = start_slot; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
      if (z2s_zb_devices_table[devices_counter].record_id == 0)
        return devices_counter;
  return 0xFF;  
}

uint8_t Z2S_findZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr) {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
      if ((z2s_zb_devices_table[devices_counter].record_id > 0) && 
          (memcmp(z2s_zb_devices_table[devices_counter].ieee_addr, ieee_addr, sizeof(esp_zb_ieee_addr_t)) == 0))
        return devices_counter;
  return 0xFF;  
}

void Z2S_initZBDevices(uint32_t init_ms) {

  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
      if (z2s_zb_devices_table[devices_counter].record_id > 0)
        z2s_zb_devices_table[devices_counter].last_seen_ms = init_ms;
}

uint8_t Z2S_updateZBDeviceTableSlot(esp_zb_ieee_addr_t  ieee_addr, uint8_t Supla_channel) {

  uint8_t zb_device_slot = Z2S_findZBDeviceTableSlot(ieee_addr);
  uint8_t first_free_channel_slot = 0xFF;
  //bool channel_added = false;

  if (zb_device_slot == 0xFF) {
    log_i("Z2S_updateZBDeviceTableSlot failed - device not found");
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
      log_i("Z2S_updateZBDeviceTableSlot failed - no free Supla channel slots");
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
  
  for (uint8_t devices_counter = 0; devices_counter < Z2S_ZBDEVICESMAXCOUNT; devices_counter++) 
    if (z2s_zb_devices_table[devices_counter].record_id > 0) {
      char log_line[1024];

      sprintf(log_line,"ENTRY\t\t\t%u\n\rRECORD ID\t\t%u\n\r"
            "MANUFACTURER NAME\t%s\n\rMODEL NAME\t\t%s\n\r"
            "IEEE ADDRESS\t\t%X:%X:%X:%X:%X:%X:%X:%X\n\r"
            "SHORT ADDRESS\t\t0x%X\n\rENDPOINTS COUNT\t\t0x%X\n\rPOWER SOURCE\t\t0x%X\n\r"
            "MODEL\t\t\t%lu\n\r"
            "LAST RSSI\t\t%d\n\rLAST SEEN (MS)\t\t%lu\n\rKEEP ALIVE (MS)\t\t%lu\n\r"
            "TIMEOUT (MS)\t\t%lu\n\rUSER FLAGS\t\t%lu\n\rUSER DATA\t\t%lu\n\r"
            "SUPLA CH[0]\t\t%u\n\rSUPLA CH[1]\t\t%u\n\rSUPLA CH[2]\t\t%u\n\rSUPLA CH[3]\t\t%u\n\r"
            "SUPLA CH[4]\t\t%u\n\rSUPLA CH[5]\t\t%u\n\rSUPLA CH[6]\t\t%u\n\rSUPLA CH[7]\t\t%u\n\r"
            "SUPLA CH[8]\t\t%u\n\rSUPLA CH[9]\t\t%u\n\rSUPLA CH[10]\t\t%u\n\rSUPLA CH[11]\t\t%u\n\r"
            "SUPLA CH[12]\t\t%u\n\rSUPLA CH[13]\t\t%u\n\rSUPLA CH[14]\t\t%u\n\rSUPLA CH[15]\t\t%u\n\r",
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
        
      log_i_telnet(log_line, toTelnet);
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
      
      if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
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
          if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_legacy_table, z2s_zb_devices_table_size)) {
            log_i ("Zigbee devices legacy table load failed!");
            return false;
          }
          for (uint8_t table_index = 0; table_index < Z2S_ZBDEVICESMAXCOUNT; table_index++) {

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
              z2s_zb_devices_table[table_index].last_rssi = (z2s_zb_devices_legacy_table + table_index)->last_rssi;
              z2s_zb_devices_table[table_index].last_battery_percentage = (z2s_zb_devices_legacy_table + table_index)->last_battery_percentage;
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
        if (!Supla::Storage::ConfigInstance()->getBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
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

  if (!Supla::Storage::ConfigInstance()->setBlob(Z2S_ZB_DEVICES_TABLE, (char *)z2s_zb_devices_table, sizeof(z2s_zb_devices_table))) {
    log_i ("Zigbee devices table write failed!");
    return false;
  }
  else { 
    if (Supla::Storage::ConfigInstance()->setUInt32(Z2S_ZB_DEVICES_TABLE_SIZE, sizeof(z2s_zb_devices_table))) {
      log_i("Devices table new size(%d) write success!", sizeof(z2s_zb_devices_table));
      Supla::Storage::ConfigInstance()->commit();
      return true;
    }
    else { 
      log_i ("Zigbee devices table size write failed!");
      return false;
    }
  }
}

void Z2S_clearZBDevicesTable() {
  log_i("Clear zigbee devices table");
  memset(z2s_zb_devices_table,0,sizeof(z2s_zb_devices_table));
  Z2S_saveZBDevicesTable();
}

void Z2S_onDataSaveRequest(uint8_t selector) {
  
  switch (selector) {
    case 0: Z2S_saveDevicesTable(); break;
    case 1: Z2S_saveZBDevicesTable(); break;
    case 2: {
      Z2S_saveZBDevicesTable();
      Z2S_saveDevicesTable();
    } break;
  }
}

void Z2S_initSuplaChannels() {

  log_i ("initSuplaChannels starting");
  zbg_device_params_t *device = (zbg_device_params_t *)malloc(sizeof(zbg_device_params_t));

  for (uint8_t devices_counter = 0; devices_counter < Z2S_CHANNELMAXCOUNT; devices_counter++) {
      if (z2s_devices_table[devices_counter].valid_record) {

        device->endpoint = z2s_devices_table[devices_counter].endpoint;
        device->cluster_id = z2s_devices_table[devices_counter].cluster_id;
        memcpy(device->ieee_addr, z2s_devices_table[devices_counter].ieee_addr, 8);
        device->short_addr = z2s_devices_table[devices_counter].short_addr;
        device->model_id = z2s_devices_table[devices_counter].model_id;
        
        Z2S_updateZBDeviceTableSlot(z2s_devices_table[devices_counter].ieee_addr, z2s_devices_table[devices_counter].Supla_channel);

        switch (z2s_devices_table[devices_counter].Supla_channel_type) {

          case SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR:
            initZ2SDeviceTempHumidity(devices_counter); break;

          case SUPLA_CHANNELTYPE_PRESSURESENSOR:
            initZ2SDevicePressure(devices_counter); break;

          case SUPLA_CHANNELTYPE_GENERAL_PURPOSE_MEASUREMENT:
            initZ2SDeviceGeneralPurposeMeasurement(devices_counter); break; 
          
          case SUPLA_CHANNELTYPE_BINARYSENSOR: 
            initZ2SDeviceIASzone(devices_counter); break;
          
          case SUPLA_CHANNELTYPE_RELAY: 
            initZ2SDeviceVirtualRelay(&zbGateway, device, devices_counter); break;
          
          case SUPLA_CHANNELTYPE_ACTIONTRIGGER: 
            initZ2SDeviceActionTrigger(devices_counter); break;

          case SUPLA_CHANNELTYPE_ELECTRICITY_METER: 
            initZ2SDeviceElectricityMeter(&zbGateway, device, devices_counter); break;
          
          case SUPLA_CHANNELTYPE_HVAC: initZ2SDeviceHvac(&zbGateway, device, devices_counter); break;
          
          case SUPLA_CHANNELTYPE_DIMMER: initZ2SDeviceDimmer(&zbGateway, device, devices_counter); break;
          
          case SUPLA_CHANNELTYPE_RGBLEDCONTROLLER: initZ2SDeviceRGB(&zbGateway, device, devices_counter); break;
          
          //case SUPLA_CHANNELTYPE_DIMMERANDRGBLED: initZ2SDeviceRGBW(&zbGateway, device, z2s_devices_table[devices_counter].Supla_channel); break;
          
          default: 
            log_i("Can't create channel for %d channel type", z2s_devices_table[devices_counter].Supla_channel_type); break; 
        }
      }
  }
  free(device);
  auto TestVT = new Supla::Sensor::VirtualThermometer();
        TestVT->getChannel()->setChannelNumber(100);
}

void Z2S_onTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float temperature, signed char rssi) {

  log_i("onTemperatureReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
    msgZ2SDeviceTempHumidityTemp(channel_number_slot, temperature, rssi);
}

void Z2S_onHumidityReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float humidity, signed char rssi) {

  log_i("onHumidityReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
    msgZ2SDeviceTempHumidityHumi(channel_number_slot, humidity, rssi);
}

void Z2S_onPressureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, float pressure, signed char rssi) {

  log_i("onPressureReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_PRESSURESENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
    msgZ2SDevicePressure(channel_number_slot, pressure, rssi);
}

void Z2S_onIlluminanceReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t illuminance, signed char rssi) {

  log_i("onIlluminanceReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, illuminance 0x%x", ieee_addr[7], ieee_addr[6], 
        ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint, illuminance);

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
  
  log_i("onIlluminanceReceive - no channel found for address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
}

void Z2S_onOccupancyReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t occupancy, signed char rssi) {

  log_i("onOccupancyReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, occupancy 0x%x", ieee_addr[7], ieee_addr[6], 
        ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint, occupancy);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceIASzone(channel_number_slot, (occupancy > 0), rssi);
    return;
  }

  log_i("onOccupancyReceive - no channel found for address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);

}

void Z2S_onThermostatTemperaturesReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, 
                                         uint16_t id, int16_t temperature, signed char rssi) {

  log_i("onThermostatTemperaturesReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, attribute id 0x%x, temperature %d", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, id, temperature);

  int16_t channel_number_slot_1 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR, 
                                                            NO_CUSTOM_CMD_SID);
  
  int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HVAC, 
                                                            NO_CUSTOM_CMD_SID);

  if (channel_number_slot_2 < 0) {
    log_i("onThermostatTemperaturesReceive - no thermostat channel found for address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",
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

  log_i("onThermostatModesReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x, attribute id 0x%x, mode %u", 
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], 
        endpoint, id, mode);

  int16_t channel_number_slot_2 = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_HVAC, 
                                                            NO_CUSTOM_CMD_SID);

  if (channel_number_slot_2 < 0) {
    log_i("onThermostatModesReceive - no thermostat channel found for address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",
          ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
    return;
  }

  switch (id) {

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_RUNNING_MODE_ID: {
      uint8_t running_mode = (mode == 0) ? 0 : 1; 
      msgZ2SDeviceHvac(channel_number_slot_2, TRV_RUNNING_STATE_MSG, running_mode, rssi);
    } break;

    case ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID: {
      uint8_t system_mode = (mode == 0) ? 0 : 1;
      msgZ2SDeviceHvac(channel_number_slot_2, TRV_SYSTEM_MODE_MSG, system_mode, rssi);
    } break;
  }
}


void Z2S_onOnOffReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, bool state, signed char rssi) {

  log_i("onOnOffReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

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
    msgZ2SDeviceRGB(z2s_devices_table[channel_number_slot].model_id, z2s_devices_table[channel_number_slot].Supla_channel, 0xFF, 0xFF, state, rssi);
    return;
  }
  
  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMERANDRGBLED, NO_CUSTOM_CMD_SID);
  if (channel_number_slot >= 0) {
    //msgZ2SDeviceRGBW(z2s_devices_table[channel_number_slot].model_id, z2s_devices_table[channel_number_slot].Supla_channel, 0xFF, 0xFF, 0xFFFF, state, rssi);
    return;
  }
  
  log_i("onOnOffReceive - no channel found for address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
}

void Z2S_onRMSVoltageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t voltage, signed char rssi) {

  log_i("onRMSVoltageReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
    msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_VOLTAGE_A_SEL, voltage, rssi);
}

void Z2S_onRMSCurrentReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t current, signed char rssi) {

  log_i("onRMSCurrentReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3],
        ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
    msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_CURRENT_A_SEL, current, rssi);
}

void Z2S_onRMSActivePowerReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t active_power, signed char rssi) {

  log_i("onRMSActivePowerReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else {
    msgZ2SDeviceElectricityMeter(z2s_devices_table[channel_number_slot].Supla_channel, Z2S_EM_ACTIVE_POWER_A_SEL, active_power, rssi);
  }
}

void Z2S_onCurrentSummationReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint64_t active_fwd_energy, signed char rssi) {

  log_i("onCurrentSummationReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], 
        ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_ELECTRICITY_METER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
    msgZ2SDeviceElectricityMeter(channel_number_slot, Z2S_EM_ACT_FWD_ENERGY_A_SEL, active_fwd_energy, rssi);
}

void Z2S_onCurrentLevelReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t level, signed char rssi) {

  log_i("onCurrentLevelReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMER, DIMMER_FUNC_BRIGHTNESS_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceDimmer(channel_number_slot, level, true, rssi);
    return;
  }
  log_i("No channel found for address %s", ieee_addr);
}

void Z2S_onColorHueReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t hue, signed char rssi) {

  log_i("onColorHueReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceRGB(z2s_devices_table[channel_number_slot].model_id, z2s_devices_table[channel_number_slot].Supla_channel, hue, 0xFF, true, rssi);
    return;
  }
  log_i("No channel found for address %s", ieee_addr);
}

void Z2S_onColorSaturationReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t saturation, signed char rssi) {

  log_i("onColorHueReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_RGBLEDCONTROLLER, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceRGB(z2s_devices_table[channel_number_slot].model_id, z2s_devices_table[channel_number_slot].Supla_channel, 0xFF, 
                    saturation,true, rssi);
    return;
  }
  log_i("No channel found for address %s", ieee_addr);
}

void Z2S_onColorTemperatureReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint16_t color_temperature, signed char rssi) {

  log_i("onColorTemperatureReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], 
        ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_DIMMER, DIMMER_FUNC_COLOR_TEMPERATURE_SID);
  
  if (channel_number_slot >= 0) {
    msgZ2SDeviceDimmer(z2s_devices_table[channel_number_slot].Supla_channel, color_temperature, true, rssi);
    return;
  }
  log_i("No channel found for address %s", ieee_addr);
}


void Z2S_onBatteryPercentageReceive(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, uint8_t battery_remaining) {

  log_i("onBatteryPercentageReceive 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, endpoint 0x%x", ieee_addr[7], ieee_addr[6], ieee_addr[5], 
        ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0], endpoint);

  Z2S_updateZBDeviceLastSeenMs(ieee_addr, millis());

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", ieee_addr);
  else
  while (channel_number_slot >= 0)
  {
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[channel_number_slot].Supla_channel);
    if (element) {
      element->getChannel()->setBatteryLevel(battery_remaining);
      //element->getChannel()->setStateOnline()
    }
    channel_number_slot = Z2S_findChannelNumberNextSlot(channel_number_slot, ieee_addr, endpoint, cluster, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  }
}

void Z2S_onIASzoneStatusChangeNotification(esp_zb_ieee_addr_t ieee_addr, uint16_t endpoint, uint16_t cluster, int iaszone_status, signed char rssi) {
  
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, IAS_ZONE_ALARM_1_SID);

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
    return;
  }

  channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, cluster, SUPLA_CHANNELTYPE_BINARYSENSOR, NO_CUSTOM_CMD_SID);

  if (channel_number_slot >= 0) {
    log_i("IASZONE - NO_CUSTOM_CMD_SID channel:%x, status: %x", channel_number_slot, iaszone_status);
    msgZ2SDeviceIASzone(channel_number_slot, (iaszone_status & 1), rssi);
    return;
  }

  log_i("onIASzoneStatusChangeNotification - no channel found for address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x",
        ieee_addr[7], ieee_addr[6], ieee_addr[5], ieee_addr[4], ieee_addr[3], ieee_addr[2], ieee_addr[1], ieee_addr[0]);
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

  if ((z2s_devices_table[channel_number_slot].model_id == Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_1) ||
      (z2s_devices_table[channel_number_slot].model_id == Z2S_DEVICE_DESC_IKEA_SOMRIG_BUTTON_2))
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
  log_i("Z2S_onCustomCmdReceive cluster 0x%x, command id 0x%x, rssi: %d", cluster_id, command_id, rssi);
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            ALL_SUPLA_CHANNEL_TYPES/*SUPLA_CHANNELTYPE_ACTIONTRIGGER*/, NO_CUSTOM_CMD_SID);
  if (channel_number_slot < 0) {
    log_i("No channel found for address %s", ieee_addr);
    return false;
  }
  
  log_i("z2s_devices_table[channel_number_slot].Supla_channel 0x%x", z2s_devices_table[channel_number_slot].Supla_channel);

  switch (z2s_devices_table[channel_number_slot].model_id) {
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
        log_i("No IKEA device channel found for address %s", ieee_addr);
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
      log_i("TUYA command: cluster(0x%x), command id(0x%x)" ,  cluster_id, command_id);   
      //process Tuya command
      if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) &&
        ((command_id == TUYA_ON_OFF_CUSTOM_CMD_BUTTON_PRESS_ID) || (command_id == TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID))) {

        int8_t sub_id = (command_id == TUYA_ON_OFF_CUSTOM_CMD_BUTTON_ROTATE_ID) ? TUYA_CUSTOM_CMD_BUTTON_ROTATE_RIGHT_SID + (*buffer) : (*buffer);
        channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id);
        if (channel_number_slot < 0)
          log_i("No Tuya device channel found for address %s", ieee_addr);
        else 
          msgZ2SDeviceActionTrigger(channel_number_slot, rssi);
      return true;
    } 
  } break;
  case Z2S_DEVICE_DESC_SONOFF_SMART_BUTTON_3F: {
    if ((cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) && ((command_id >= 0) && (command_id <=2))) {

        int8_t sub_id;
        switch (command_id) {
          case 0: sub_id = TUYA_CUSTOM_CMD_BUTTON_PRESSED_SID; break;
          case 1: sub_id = TUYA_CUSTOM_CMD_BUTTON_DOUBLE_PRESSED_SID; break;
          case 2: sub_id = TUYA_CUSTOM_CMD_BUTTON_HELD_SID; break;
        }  
        channel_number_slot = Z2S_findChannelNumberSlot(ieee_addr, endpoint, ESP_ZB_ZCL_CLUSTER_ID_ON_OFF, 
                                                            SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id);
        if (channel_number_slot < 0)
          log_i("No device channel found for address %s", ieee_addr);
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
    case 0xEF00: processTuyaCustomCluster(ieee_addr, endpoint, command_id, payload_size, payload, rssi); break;
    case 0xFC7F:
    case 0xFC80: {
      log_i("IKEA custom cluster(0x%x) on endpoint(0x%x), command(0x%x)", cluster, endpoint, command_id);
      processIkeaSymfoniskCommands(ieee_addr, endpoint, cluster, command_id, payload_size, payload, rssi);
     } break;
    default: log_i("Unknown custom cluster(0x%x) command(0x%x)", cluster, command_id); break;
  }
}

void Z2S_onBTCBoundDevice(zbg_device_params_t *device) {
  log_i("BTC bound device(0x%x) on endpoint(0x%x), cluster id(0x%x)", device->short_addr, device->endpoint, device->cluster_id );
  
  int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  
  if (channel_number_slot < 0)
    log_i("No channel found for address %s", device->ieee_addr);
  else
  while (channel_number_slot >= 0)
  {
    device->model_id = z2s_devices_table[channel_number_slot].model_id;
    device->user_data = z2s_devices_table[channel_number_slot].Supla_channel;

    channel_number_slot = Z2S_findChannelNumberNextSlot(channel_number_slot, device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, NO_CUSTOM_CMD_SID);
  } 
  Z2S_addZBDeviceTableSlot(device->ieee_addr, device->short_addr, "unknown","unknown", 0, device->model_id,0);

}


void Z2S_onBoundDevice(zbg_device_params_t *device, bool last_cluster) {
}

uint8_t Z2S_addZ2SDevice(zbg_device_params_t *device, int8_t sub_id, char *name, uint32_t func, char *unit) {

  int16_t channel_number_slot = Z2S_findChannelNumberSlot(device->ieee_addr, device->endpoint, device->cluster_id, ALL_SUPLA_CHANNEL_TYPES, sub_id);
  
  if (channel_number_slot < 0) {
    log_i("No channel found for address  for 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, adding new one!",
        device->ieee_addr[7], device->ieee_addr[6], device->ieee_addr[5], device->ieee_addr[4], 
        device->ieee_addr[3], device->ieee_addr[2], device->ieee_addr[1], device->ieee_addr[0]);
    
    uint8_t first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
    
    if (first_free_slot == 0xFF) {
        log_i("Devices table full");
        return ADD_Z2S_DEVICE_STATUS_DT_FULL;
    }
    log_i("model id %d, first free slot %d", device->model_id, first_free_slot);
    
    switch (device->model_id) {
      case 0x0000: break;
      
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR:
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_1:
      case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR:
      case Z2S_DEVICE_DESC_TUYA_SOIL_TEMPHUMIDITY_SENSOR_1:
      case Z2S_DEVICE_DESC_TUYA_TEMPHUMIDITY_SENSOR: 
      case Z2S_DEVICE_DESC_TEMPHUMIDITY_SENSOR_HUMIX10:
        addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_TEMPHUMIPRESSURE_SENSOR: {
        addZ2SDeviceTempHumidity(device, first_free_slot, sub_id, name, func);
        
        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDevicePressure(device, first_free_slot);
      } break;

      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR: 
      case Z2S_DEVICE_DESC_LUMI_MAGNET_SENSOR: 
      case Z2S_DEVICE_DESC_IKEA_IAS_ZONE_SENSOR_1:
      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_T_B:
        addZ2SDeviceIASzone(device, first_free_slot, sub_id, name, func); break;

      case Z2S_DEVICE_DESC_IAS_ZONE_SENSOR_1_2_T: {
        
        addZ2SDeviceIASzone(device, first_free_slot, IAS_ZONE_ALARM_1_SID, "ALARM 1");

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceIASzone(device, first_free_slot, IAS_ZONE_ALARM_2_SID, "ALARM 2");

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceIASzone(device, first_free_slot, IAS_ZONE_TAMPER_SID, "TAMPER");
      } break;

      case Z2S_DEVICE_DESC_RELAY:
      case Z2S_DEVICE_DESC_RELAY_1: addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, "POWER SWITCH", 
                                                              SUPLA_CHANNELFNC_POWERSWITCH); break;

      case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_1:
      case Z2S_DEVICE_DESC_TUYA_GANG_SWITCH_2: {
        char gang_name[30];
        sprintf(gang_name, "GANG #%d", device->endpoint);
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, gang_name, SUPLA_CHANNELFNC_LIGHTSWITCH); 
      } break;

      case Z2S_DEVICE_DESC_ON_OFF:
      case Z2S_DEVICE_DESC_ON_OFF_1: {
        auto Supla_Z2S_VirtualRelay = new Supla::Control::VirtualRelay();
        Z2S_fillDevicesTableSlot(device, first_free_slot, Supla_Z2S_VirtualRelay->getChannelNumber(), SUPLA_CHANNELTYPE_ACTIONTRIGGER, sub_id); 
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
      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER:
      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_1:
      case Z2S_DEVICE_DESC_RELAY_ELECTRICITY_METER_2:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_1:
      case Z2S_DEVICE_DESC_TUYA_RELAY_ELECTRICITY_METER_2: {
        addZ2SDeviceVirtualRelay(&zbGateway,device, first_free_slot);
        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
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
          addZ2SDeviceElectricityMeter(&zbGateway, device, false, false, first_free_slot, false); 
        else
          addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, name, func, unit);
      } break;

      case Z2S_DEVICE_DESC_TUYA_HVAC:
      case Z2S_DEVICE_DESC_TUYA_HVAC_6567C:
      case Z2S_DEVICE_DESC_TUYA_HVAC_23457:
      case Z2S_DEVICE_DESC_TUYA_HVAC_LEGACY:
      case Z2S_DEVICE_DESC_TS0601_TRV_SASWELL:
      case Z2S_DEVICE_DESC_TS0601_TRV_ME167:
      case Z2S_DEVICE_DESC_TS0601_TRV_BECA:
      case Z2S_DEVICE_DESC_TS0601_TRV_MOES:
      case Z2S_DEVICE_DESC_SONOFF_TRVZB: {
      
        addZ2SDeviceTempHumidity(device, first_free_slot);
        uint8_t trv_thermometer_slot = first_free_slot;

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceHvac(&zbGateway, device, first_free_slot, trv_thermometer_slot); 
      } break;
      
      case Z2S_DEVICE_DESC_TUYA_DIMMER_BULB: {

        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, "DIMMER BULB", SUPLA_CHANNELFNC_DIMMER); 
      } break; 

      case Z2S_DEVICE_DESC_TUYA_RGB_BULB: {

        addZ2SDeviceRGB(&zbGateway,device, first_free_slot, "RGB BULB", SUPLA_CHANNELFNC_RGBLIGHTING);
      } break;

      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_A:
      case Z2S_DEVICE_DESC_TUYA_RGBW_BULB_MODEL_B:
      case Z2S_DEVICE_DESC_IKEA_RGBW_BULB:
      case Z2S_DEVICE_DESC_RGBW_BULB_XY: {
        
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, "RGBW SWITCH", SUPLA_CHANNELFNC_LIGHTSWITCH);

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_BRIGHTNESS_SID, "BRIGHTNESS", SUPLA_CHANNELFNC_DIMMER);

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, DIMMER_FUNC_COLOR_TEMPERATURE_SID, "COLOR TEMPERATURE", SUPLA_CHANNELFNC_DIMMER);

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceRGB(&zbGateway,device, first_free_slot, "RGB", SUPLA_CHANNELFNC_RGBLIGHTING); 
      } break; 

      case Z2S_DEVICE_DESC_TUYA_RGB_LED_CONTROLLER_XY: {
      
        addZ2SDeviceVirtualRelay( &zbGateway,device, first_free_slot, "RGB SWITCH", SUPLA_CHANNELFNC_LIGHTSWITCH);

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceRGB(&zbGateway,device, first_free_slot, "RGB", SUPLA_CHANNELFNC_RGBLIGHTING); 
      } break; 

      case Z2S_DEVICE_DESC_TUYA_DIMMER_DOUBLE_SWITCH:
        addZ2SDeviceDimmer(&zbGateway,device, first_free_slot, sub_id, "DIMMER SWITCH", SUPLA_CHANNELFNC_DIMMER); break;

      case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR: {

        addZ2SDeviceIASzone(device, first_free_slot, -1, "SMOKE DETECT", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
          return ADD_Z2S_DEVICE_STATUS_DT_FWA;
        }
        addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, -1, "SMOKE CONC.", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "ppm");
      } break;

      case Z2S_DEVICE_DESC_TUYA_SMOKE_DETECTOR_1: {

        addZ2SDeviceIASzone(device, first_free_slot, -1, "SMOKE DETECT", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);
      } break;

      case Z2S_DEVICE_DESC_TUYA_ILLUMINANCE_SENSOR:
        addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, -1, "LIGHT ILLU.", SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;

      case Z2S_DEVICE_DESC_ILLUZONE_SENSOR: {
        
        addZ2SDeviceIASzone(device, first_free_slot, -1, "LS ZONE", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR);

        first_free_slot = Z2S_findFirstFreeDevicesTableSlot();
        if (first_free_slot == 0xFF) {
          log_i("ERROR! Devices table full!");
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

      case Z2S_DEVICE_DESC_TUYA_PRESENCE_SENSOR_5: {
        
        switch (sub_id) {
          case TUYA_PRESENCE_SENSOR_PRESENCE_SID:
            addZ2SDeviceIASzone(device, first_free_slot, sub_id, "PRESENCE", SUPLA_CHANNELFNC_ALARMARMAMENTSENSOR); break;
      
          case TUYA_PRESENCE_SENSOR_ILLUMINANCE_SID: 
            addZ2SDeviceGeneralPurposeMeasurement(device, first_free_slot, sub_id, "ILLUMINANCE",
                                                  SUPLA_CHANNELFNC_GENERAL_PURPOSE_MEASUREMENT, "lx"); break;
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

      case Z2S_DEVICE_DESC_ADEO_CONTACT_VIBRATION_SENSOR: 
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

      default : {
        
        log_i("Device (0x%x), endpoint (0x%x), model (0x%x) unknown", device->short_addr, device->endpoint, device->model_id);
        return ADD_Z2S_DEVICE_STATUS_DUN;
      } break;
    }
    return ADD_Z2S_DEVICE_STATUS_OK;
  } else {
    log_i("Device (0x%x), endpoint (0x%x) already in z2s_devices_table (index 0x%x)", device->short_addr, device->endpoint, channel_number_slot);
    if (z2s_devices_table[channel_number_slot].short_addr != device->short_addr) {
      z2s_devices_table[channel_number_slot].short_addr = device->short_addr;
      Z2S_saveDevicesTable();
      log_i("Device short address updated");
    }
    return ADD_Z2S_DEVICE_STATUS_DAP; 
  }
}

void updateTimeout(uint8_t device_id, uint8_t timeout, uint8_t selector, uint32_t timings_secs) {
  
  if (timeout >0) {
    //z2s_devices_table[device_id].user_data_flags |= USER_DATA_FLAG_SED_TIMEOUT;
    //z2s_devices_table[device_id].user_data_1 = timeout;
    z2s_devices_table[device_id].timeout_secs = timeout * 3600;
  }
  else {
    if (selector & 1)
      z2s_devices_table[device_id].keep_alive_secs = timings_secs;
    if (selector & 2)
      z2s_devices_table[device_id].timeout_secs = timings_secs;
    if (selector & 4)
      z2s_devices_table[device_id].refresh_secs = timings_secs;
  }

  
  if (Z2S_saveDevicesTable()) {
    log_i("Device(channel %d) timeout updated. Table saved successfully.", z2s_devices_table[device_id].Supla_channel);
      
    auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[device_id].Supla_channel);
      
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELTYPE_BINARYSENSOR) {

      auto Supla_Z2S_VirtualBinary = reinterpret_cast<Supla::Sensor::Z2S_VirtualBinary *>(element);
      Supla_Z2S_VirtualBinary->setTimeout(timeout);
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
    if (element != nullptr && element->getChannel()->getChannelType() == SUPLA_CHANNELFNC_HVAC_THERMOSTAT) {

      auto Supla_Z2S_HvacBaseEE = reinterpret_cast<Supla::Control::HvacBaseEE*>(element);
      auto Supla_Z2S_TRVInterface = reinterpret_cast<Supla::Control::Z2S_TRVInterface*>(Supla_Z2S_HvacBaseEE->getPrimaryOutputEE());
      //if (selector & 1)
      //  Supla_Z2S_RGBInterface->setKeepAliveSecs(timings_secs);
      if (Supla_Z2S_TRVInterface && (selector & 2))
        Supla_Z2S_TRVInterface->setTimeoutSecs(timings_secs);
    }
  }
}
void updateRGBMode(uint8_t device_id, uint8_t rgb_mode) {

  if (z2s_devices_table[device_id].Supla_channel_type == SUPLA_CHANNELTYPE_RGBLEDCONTROLLER) {

    z2s_devices_table[device_id].user_data_1 = rgb_mode;

    if (Z2S_saveDevicesTable()) {
      log_i("Device(channel %d) RGB mode updated. Table saved successfully.", z2s_devices_table[device_id].Supla_channel);
      
      auto element = Supla::Element::getElementByChannelNumber(z2s_devices_table[device_id].Supla_channel);

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