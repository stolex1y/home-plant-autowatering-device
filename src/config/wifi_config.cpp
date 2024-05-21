#include "config/wifi_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const WifiConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "wifi_ssid", src.wifi_ssid) &&
         SetJsonFromField(dst, "wifi_pass", src.wifi_pass) &&
         SetJsonFromField(dst, "ap_local_ip", src.ap_local_ip) &&
         SetJsonFromField(dst, "ap_subnet", src.ap_subnet) &&
         SetJsonFromField(dst, "reconnect_timeout", src.reconnect_timeout);
}

bool convertFromJson(const JsonVariantConst &src, WifiConfig &dst) {
  return SetFieldFromJson(src, "wifi_ssid", dst.wifi_ssid) &&
         SetFieldFromJson(src, "wifi_pass", dst.wifi_pass) &&
         SetFieldFromJson(src, "ap_local_ip", dst.ap_local_ip) &&
         SetFieldFromJson(src, "ap_subnet", dst.ap_subnet) &&
         SetFieldFromJson(src, "reconnect_timeout", dst.reconnect_timeout);
}

bool operator==(const WifiConfig &first, const WifiConfig &second) {
  return first.wifi_ssid == second.wifi_ssid && first.wifi_pass == second.wifi_pass &&
         first.reconnect_timeout == second.reconnect_timeout &&
         first.ap_local_ip == second.ap_local_ip && first.ap_subnet == second.ap_subnet;
}

bool operator!=(const WifiConfig &first, const WifiConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
