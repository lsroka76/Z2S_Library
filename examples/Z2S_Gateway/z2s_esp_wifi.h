#include <supla/network/esp_wifi.h>
#include <esp_wifi.h>

class Z2S_ESPWifi : public Supla::ESPWifi {
 public:
  using Supla::ESPWifi::ESPWifi; 

  void setup() override {
    
    Supla::ESPWifi::setup();

    WiFi.setTxPower(txPower_);
    esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);

    SUPLA_LOG_DEBUG("Z2S WiFi: TX Power set to %.2f dBm (raw: %d)",
                    WiFi.getTxPower() * 0.25f, WiFi.getTxPower());
  }

  void setZ2STxPower(wifi_power_t power) {
    
    txPower_ = power;
    if (WiFi.getMode() != WIFI_MODE_NULL) {
      WiFi.setTxPower(txPower_);
    }
  }

 private:
#if CONFIG_IDF_TARGET_ESP32S3
  wifi_power_t txPower_ = WIFI_POWER_11dBm;
#else
  wifi_power_t txPower_ = WIFI_POWER_15dBm;
#endif
};