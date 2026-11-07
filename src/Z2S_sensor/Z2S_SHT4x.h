// SPDX-FileCopyrightText: AC SOFTWARE SP. Z O.O.
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SRC_Z2S_SENSOR_SHT4X_H_
#define SRC_Z2S_SENSOR_SHT4X_H_

// Dependency: RobTillaart SHT4x library - use library manager to install it
// https://github.com/RobTillaart/SHT4x

#include <SHT4x.h>
#include <supla/log_wrapper.h>
#include <supla/sensor/therm_hygro_meter.h>

namespace Supla {
namespace Sensor {
class SHT4x : public ThermHygroMeter {
 public:
  explicit SHT4x(int8_t address = 0x44) : address(address), sht(address) {
  }

  double getTemp() override {
    readValuesFromDevice();
    return temperature;
  }

  double getHumi() override {
    return humidity;
  }

  void onInit() override {
    sht.begin();
    readValuesFromDevice();
    channel.setNewValue(temperature, humidity);
  }

 private:
  void readValuesFromDevice() {
    if (!sht.read(SHT4x_MEASUREMENT_MEDIUM)) {
      SUPLA_LOG_ERROR("SHT4x [ERROR] Code #%d", sht.getError());
      retryCount++;
      if (retryCount > 3) {
        retryCount = 0;
        temperature = TEMPERATURE_NOT_AVAILABLE;
        humidity = HUMIDITY_NOT_AVAILABLE;
      }
    } else {
      retryCount = 0;
      temperature = sht.getTemperature();
      humidity = sht.getHumidity();
    }
  }

 protected:
  int8_t address;
  double temperature = TEMPERATURE_NOT_AVAILABLE;
  double humidity = HUMIDITY_NOT_AVAILABLE;
  int8_t retryCount = 0;
  ::SHT4x sht;  // I2C
};

};  // namespace Sensor
};  // namespace Supla

#endif  // SRC_Z2S_SENSOR_SHT4X_H_