#ifndef HOME_PLANT_AUTOWATERING_DEVICE_WIFI_CONFIG_H
#define HOME_PLANT_AUTOWATERING_DEVICE_WIFI_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::config {

struct WifiConfig {
  String wifi_ssid;
  String wifi_pass;
};

bool convertToJson(const WifiConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, WifiConfig &dst);

bool operator==(const WifiConfig &first, const WifiConfig &second);
bool operator!=(const WifiConfig &first, const WifiConfig &second);

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_WIFI_CONFIG_H
