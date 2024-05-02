#include "../../include/config/wifi_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const WifiConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "wifi_ssid", src.wifi_ssid) &&
         SetJsonFromField(dst, "wifi_pass", src.wifi_pass);
}
bool convertFromJson(const JsonVariantConst &src, WifiConfig &dst) {
  return SetFieldFromJson(src, "wifi_ssid", dst.wifi_ssid) &&
         SetFieldFromJson(src, "wifi_pass", dst.wifi_pass);
}
bool operator==(const WifiConfig &first, const WifiConfig &second) {
  return first.wifi_ssid == second.wifi_ssid && first.wifi_pass == second.wifi_pass;
}
bool operator!=(const WifiConfig &first, const WifiConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
