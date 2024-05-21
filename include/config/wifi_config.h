#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/utils.h"

namespace hpa::config {

using namespace utils::chrono_literals;

constexpr static const auto kDefaultReconnectTimeout = 15_s;
constexpr static const auto kDefaultApLocalIp = "192.168.100.100";
constexpr static const auto kDefaultApSubnet = "255.255.255.0";

struct WifiConfig {
  String wifi_ssid;
  String wifi_pass;
  uint64_t reconnect_timeout = kDefaultReconnectTimeout;
  String ap_local_ip = kDefaultApLocalIp;
  String ap_subnet = kDefaultApSubnet;
};

bool convertToJson(const WifiConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, WifiConfig &dst);

bool operator==(const WifiConfig &first, const WifiConfig &second);
bool operator!=(const WifiConfig &first, const WifiConfig &second);

}  // namespace hpa::config
