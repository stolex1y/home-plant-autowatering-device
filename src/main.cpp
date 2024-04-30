#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#include "config/common_config.h"
#include "config/config_repository.h"
#include "config/device_config.h"
#include "eeprom_manager.h"
#include "http/handlers/get_connection_status_handler.h"
#include "http/handlers/get_device_info_handler.h"
#include "http/handlers/post_common_config_handler.h"
#include "http/handlers/post_switch_mode_handler.h"
#include "json.h"
#include "logger.h"
#include "ntp_client.h"
#include "task_queue.h"
#include "utils.h"
#include "wifi.h"

#undef DEBUGV
#define DEBUGV(...) LOG_DEBUG(__VA_ARGS__)

using namespace hpa;
using namespace utils::chrono_literals;

const EepromManager::SizeType kEepromSize = 1024;
const EepromManager::SizeType kCommonConfigStartAddr = 0;
const EepromManager::SizeType kCommonConfigEndAddr = kEepromSize / 2;
const EepromManager::SizeType kDeviceConfigStartAddr = kCommonConfigEndAddr;
const EepromManager::SizeType kDeviceConfigEndAddr = kEepromSize;
const uint64_t kDefaultSyncPeriod = 10_s;
const int16_t kDefaultSoilMoistureMin = INT16_MIN;
const int16_t kDefaultSoilMoistureMax = INT16_MAX;
const auto kDefaultBaseTopic = "autowatering";
const auto kWebServerPort = 8080;

EepromManager eeprom_manager{};
config::ConfigRepository<config::CommonConfig> common_config_repo(
    eeprom_manager, kCommonConfigStartAddr, kCommonConfigEndAddr
);
config::ConfigRepository<config::DeviceConfig> device_config_repo(
    eeprom_manager, kDeviceConfigStartAddr, kDeviceConfigEndAddr
);
std::optional<config::CommonConfig> common_config;
config::DeviceConfig device_config{
    .base_topic = kDefaultBaseTopic,
    .sync_period = kDefaultSyncPeriod,
    .soil_moisture_min = kDefaultSoilMoistureMin,
    .soil_moisture_max = kDefaultSoilMoistureMax
};
tasks::TaskQueue task_queue{};

std::optional<AsyncWebServer> web_server;
std::optional<http::handlers::GetDeviceInfoHandler> get_device_info_handler;
std::optional<http::handlers::PostCommonConfigHandler> post_common_config_handler;
std::optional<http::handlers::GetConnectionStatusHandler> get_connection_status_handler;
std::optional<http::handlers::PostSwitchModeHandler> post_switch_mode_handler;

time::NtpClient ntp_client{};

void setup() {
  if (LOG_LEVEL < static_cast<int>(logger::LogLevel::kNone)) {
    Serial.begin(115200);
    Serial.println();
  }
  LOG_INFO("start");

  WiFi.setAutoConnect(false);

  const auto has_device_config = device_config_repo.HasConfig();
  if (!has_device_config) {
    LOG_TRACE("write default device config: %s", ToJsonString(device_config).c_str());
    device_config_repo.WriteConfig(device_config);
  }
  device_config = device_config_repo.ReadConfig().value();

  const auto has_common_config = common_config_repo.HasConfig();
  if (has_common_config) {
    LOG_INFO("ran in working mode");
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);

    common_config.emplace(common_config_repo.ReadConfig().value());
    LOG_TRACE("saved config: %s", ToJsonString(common_config.value()).c_str());

    if (!wifi::sta::TryConnectToWifi(common_config->wifi_ssid, common_config->wifi_pass, 10_s)) {
      LOG_TRACE("couldn't connect to wifi, went to sleep on %llu ms", device_config.sync_period);
      common_config_repo.ResetConfig();
      delay(500);
      ESP.restart();
      EspClass::deepSleep(device_config.sync_period * 1000);
    }
    LOG_DEBUG("successful connected to wifi");
    ntp_client.Begin();
    LOG_DEBUG(
        "current time %s", utils::FormatEpochSecondsAsDateTime(ntp_client.NowSinceEpoch()).c_str()
    );
    delay(10_s);
    LOG_DEBUG(
        "current time after 10s %s",
        utils::FormatEpochSecondsAsDateTime(ntp_client.NowSinceEpoch()).c_str()
    );
  } else {
    LOG_INFO("ran in configuration mode");
    WiFi.mode(WIFI_AP_STA);

    wifi::ap::EnableAp();

    web_server.emplace(kWebServerPort);
    get_device_info_handler.emplace();
    post_common_config_handler.emplace(common_config_repo);
    get_connection_status_handler.emplace();
    post_switch_mode_handler.emplace(task_queue, common_config_repo);
    web_server->addHandler(&get_device_info_handler.value());
    web_server->addHandler(&post_common_config_handler.value());
    web_server->addHandler(&get_connection_status_handler.value());
    web_server->addHandler(&post_switch_mode_handler.value());
    web_server->begin();
  }
}

void loop() {
  ntp_client.Update();
  while (auto task = task_queue.Pop()) {
    LOG_TRACE("call scheduled task in loop...");
    task.value()();
    task.reset();
  }
}
