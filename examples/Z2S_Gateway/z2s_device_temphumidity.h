#ifndef Z2S_DEVICE_TEMPHUMIDITY_H_
#define Z2S_DEVICE_TEMPHUMIDITY_H_

#include "z2s_devices_table.h"
#include <Z2S_sensor/Z2S_virtual_therm_hygro_meter.h>

void initZ2SDeviceTempHumidity(int16_t channel_number_slot);

void addZ2SDeviceTempHumidity(zbg_device_params_t *device, uint8_t free_slot);

void msgZ2SDeviceTempHumidityTemp(int16_t channel_number_slot, double temp, signed char rssi); 

void msgZ2SDeviceTempHumidityHumi(int16_t channel_number_slot, double humi, signed char rssi); 

void msgZ2SDeviceTempHumidityBatteryLevel(int16_t channel_number_slot, uint8_t battery_level, signed char rssi); 

#endif