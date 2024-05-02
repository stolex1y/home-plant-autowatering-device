#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>

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
#include "mqtt/mqtt_client.h"
#include "ntp_client.h"
#include "pins/pin.h"
#include "sensors/battery_charge_level_sensor.h"
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
const auto kDefaultReconnectTimeout = 15_s;
const auto kWebServerPort = 8080;

EepromManager eeprom_manager{};
config::ConfigRepository<config::CommonConfig> common_config_repo(
    eeprom_manager, kCommonConfigStartAddr, kCommonConfigEndAddr
);
config::ConfigRepository<config::DeviceConfig> device_config_repo(
    eeprom_manager, kDeviceConfigStartAddr, kDeviceConfigEndAddr
);
std::optional<config::CommonConfig> common_config;
config::DeviceConfig device_config{};
tasks::TaskQueue task_queue{};

std::optional<AsyncWebServer> web_server;
std::optional<http::handlers::GetDeviceInfoHandler> get_device_info_handler;
std::optional<http::handlers::PostCommonConfigHandler> post_common_config_handler;
std::optional<http::handlers::GetConnectionStatusHandler> get_connection_status_handler;
std::optional<http::handlers::PostSwitchModeHandler> post_switch_mode_handler;

std::optional<time::NtpClient> ntp_client;
std::optional<mqtt::MqttClient> mqtt_client;

sensors::BatteryChargeLevelSensor battery_charge_level_sensor;

void ConnectToWifi(const config::WifiConfig &wifi_config) {
  if (!wifi::sta::TryConnectToWifi(
          wifi_config.wifi_ssid, wifi_config.wifi_pass, kDefaultReconnectTimeout
      )) {
    LOG_TRACE("couldn't connect to wifi, went to sleep on %llu ms", device_config.sync_period);
    common_config_repo.ResetConfig();
    delay(500);
    EspClass::restart();
    //    TODO: replace restart with deepSleep
    //    EspClass::deepSleep(device_config.sync_period * 1000);
  }
}

void OnWifiConnected(const WiFiEventStationModeConnected &event) {
  LOG_DEBUG("successful connected to wifi");
  task_queue.Push([]() {
    if (mqtt_client) {
      mqtt_client->Connect();
    }
    if (ntp_client) {
      ntp_client->Begin();
    }
  });
}

void OnWifiDisconnected(const WiFiEventStationModeDisconnected &event) {
  Serial.println("disconnected from wifi");
  if (mqtt_client) {
    mqtt_client->Disconnect();
  }
}

WiFiEventHandler on_connected_wifi_handler;
WiFiEventHandler on_disconnected_wifi_handler;

void setup() {
  if (LOG_LEVEL < static_cast<int>(logger::LogLevel::kNone)) {
    Serial.begin(115200);
    Serial.println();
  }
  LOG_INFO("start");

  Wire.begin();
  battery_charge_level_sensor.Init();

  WiFi.setAutoConnect(false);

  const auto has_device_config = device_config_repo.HasConfig();
  if (has_device_config) {
    const auto read_device_config = device_config_repo.ReadConfig();
    if (!read_device_config) {
      LOG_ERROR("couldn't read saved device config");
    } else {
      device_config = read_device_config.value();
    }
  } else {
    LOG_TRACE(
        "write default device config: %s",
        ToJsonString(device_config).value_or("couldn't parse").c_str()
    );
    if (!device_config_repo.WriteConfig(device_config)) {
      LOG_ERROR("couldn't write default device config");
    }
  }

  common_config = common_config_repo.ReadConfig();
  if (common_config) {
    LOG_INFO("ran in working mode");
    LOG_TRACE(
        "saved config: %s", ToJsonString(common_config.value()).value_or("couldn't parse").c_str()
    );

    battery_charge_level_sensor.Enable();

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);

    ntp_client.emplace();
    mqtt_client.emplace(device_config.sync_period + 1_min, common_config.value());
    mqtt_client->Subscribe(
        "config",
        2,
        [](const String &payload, const AsyncMqttClientMessageProperties &properties) {
          LOG_INFO("received new message from topic 'config': %s", payload.c_str());
        }
    );

    on_connected_wifi_handler = WiFi.onStationModeConnected(OnWifiConnected);
    on_disconnected_wifi_handler = WiFi.onStationModeDisconnected(OnWifiDisconnected);
    ConnectToWifi(common_config->wifi_config);
    LOG_DEBUG("my ip is %s", WiFi.localIP().toString().c_str());
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
  if (ntp_client) {
    ntp_client->Update();
  }
  if (mqtt_client) {
    const auto connected = mqtt_client->IsConnected();
    LOG_TRACE("mqtt is connected: %d", (int)connected);
    if (connected) {
      mqtt_client->Publish("test", 2, true, "hello");
      mqtt_client->Publish(
          "charge-level", 2, true, String(battery_charge_level_sensor.GetChargeLevel())
      );
      delay(15000);
    }
  }
  while (auto task = task_queue.Pop()) {
    LOG_TRACE("call scheduled task in loop...");
    task.value()();
    task.reset();
  }
}
