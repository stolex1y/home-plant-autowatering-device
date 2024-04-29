#ifndef HOME_PLANT_AUTOWATERING_DEVICE_WIFI_H
#define HOME_PLANT_AUTOWATERING_DEVICE_WIFI_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "mac_address.h"

namespace hpa::wifi {

namespace ap {

const IPAddress kLocalIp(192, 168, 100, 100);
const IPAddress kGatewayIp(192, 168, 100, 1);
const IPAddress kSubnet(255, 255, 255, 0);
const auto kSsid = "HPA Device";

void EnableAp();
void DisableAp();
[[nodiscard]] MacAddress GetMacAddress();

}  // namespace ap

namespace sta {

bool TryConnectToWifi(const String &ssid, const String &pass, uint64_t timeout = 0);
[[nodiscard]] bool IsConnected();

}  // namespace sta

}  // namespace hpa::wifi

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_WIFI_H
