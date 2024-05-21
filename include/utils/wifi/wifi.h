#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "config/wifi_config.h"
#include "utils/mac_address.h"

namespace hpa::utils::wifi {

namespace ap {

const IPAddress kLocalIp(192, 168, 100, 100);
const IPAddress kGatewayIp(0, 0, 0, 0);
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

}  // namespace hpa::utils::wifi
