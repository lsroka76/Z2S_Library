#ifndef Z2S_DEVICE_TEMPHUMIDITY_H_
#define Z2S_DEVICE_TEMPHUMIDITY_H_

#include "z2s_devices_table.h"
#include <Z2S_sensor/Z2S_virtual_therm_hygro_meter.h>
#include <Z2S_sensor/Z2S_virtual_thermometer.h>

/*****************************************************************************/

static const char *default_temphumi_name = "T/H";
static const char *default_temp_name = "TEMP";

/*****************************************************************************/

void initZ2SDeviceTempHumidity(
  int16_t channel_number_slot, bool thermhygrometer = true);

/*****************************************************************************/

void initZ2SDeviceTempHumidity(
  uint16_t channel_index, z2s_device_params_t* _z2s_channel, 
  bool thermhygrometer = true);

/*****************************************************************************/

void addZ2SDeviceTempHumidity(
  zbg_device_params_t *device, uint8_t free_slot, int8_t sub_id = -1, 
  const char *name = nullptr, uint32_t func = 0,bool thermhygrometer = true);

/*****************************************************************************/

void msgZ2SDeviceTempHumidityTemp(
  int16_t channel_number_slot, double temp, bool refresh_only = false); 

/*****************************************************************************/

void msgZ2SDeviceTempHumidityTemp(Supla::Element* element, double temp); 

/*****************************************************************************/

void msgZ2SDeviceTempHumidityHumi(int16_t channel_number_slot, double humi); 

/*****************************************************************************/


void msgZ2SDeviceTempHumidityHumi(Supla::Element* element, double humi); 

/*****************************************************************************/

#endif
