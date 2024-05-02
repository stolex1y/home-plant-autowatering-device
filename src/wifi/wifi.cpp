#include "wifi.h"

#include "logger.h"

namespace hpa::wifi {

namespace ap {

void EnableAp() {
  WiFi.softAPConfig(kLocalIp, kGatewayIp, kSubnet);
  LOG_INFO("enabling soft-AP...");
  while (!WiFi.softAP(kSsid)) {
    delay(500);
    LOG_INFO("retry to enable soft_AP...");
  }
  LOG_INFO("soft-AP enabled!");
}

void DisableAp() {
  LOG_INFO("disabling soft-AP...");
  while (!WiFi.softAPdisconnect()) {
    delay(500);
    LOG_INFO("retry to disable soft_AP...");
  }
  LOG_INFO("soft-AP disabled!");
}

MacAddress GetMacAddress() {
  MacAddress mac_address{};
  WiFi.softAPmacAddress(mac_address.groups);
  return mac_address;
}

}  // namespace ap

namespace sta {

bool TryConnectToWifi(const String &ssid, const String &pass, const uint64_t timeout) {
  LOG_DEBUG("try to connect to WiFi with ssid: %s, pass: %s", ssid.c_str(), pass.c_str());
  WiFi.begin(ssid, pass);
  WiFi.printDiag(Serial);
  return WiFi.waitForConnectResult(timeout) == WL_CONNECTED;
}

bool IsConnected() {
  return WiFi.isConnected();
}

}  // namespace sta

}  // namespace hpa::wifi
