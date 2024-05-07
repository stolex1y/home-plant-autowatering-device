#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>

#include "autopump.h"
#include "config/common_config.h"
#include "config/config_repository.h"
#include "http/handlers/get_connection_status_handler.h"
#include "http/handlers/get_device_info_handler.h"
#include "http/handlers/post_common_config_handler.h"
#include "http/handlers/post_switch_mode_handler.h"
#include "json.h"
#include "logger.h"
#include "mqtt/config/config_repository.h"
#include "mqtt/mqtt_client.h"
#include "mqtt/readings/sensor_readings_repository.h"
#include "sensors/battery_charge_level_sensor.h"
#include "sensors/environment_sensor.h"
#include "sensors/light_level_sensor.h"
#include "sensors/soil_moisture_sensor.h"
#include "utils/eeprom_manager.h"
#include "utils/ntp_client.h"
#include "utils/task_queue.h"
#include "utils/utils.h"
#include "utils/wifi/wifi.h"

#undef DEBUGV
#define DEBUGV(...) LOG_DEBUG(__VA_ARGS__)

using namespace hpa;
using namespace utils::chrono_literals;

constexpr const utils::EepromManager::SizeType kEepromSize = 1024;
constexpr const pins::Pin::PinNumber kSoilMoistureSensorPin = A0;
constexpr const pins::Pin::PinNumber kPumpPin = D7;
constexpr const pins::Pin::PinNumber kSensorsPowerPin = D6;

utils::EepromManager eeprom_manager(kEepromSize);
config::ConfigRepository<config::CommonConfig> common_config_repo(
    eeprom_manager, 0, eeprom_manager.GetSize()
);
config::CommonConfig common_config;
utils::TaskQueue task_queue{};

std::optional<AsyncWebServer> web_server;
std::optional<http::handlers::GetDeviceInfoHandler> get_device_info_handler;
std::optional<http::handlers::PostCommonConfigHandler> post_common_config_handler;
std::optional<http::handlers::GetConnectionStatusHandler> get_connection_status_handler;
std::optional<http::handlers::PostSwitchModeHandler> post_switch_mode_handler;

std::optional<utils::NtpClient> ntp_client;
std::optional<mqtt::MqttClient> mqtt_client;

sensors::BatteryChargeLevelSensor battery_charge_level_sensor{};
sensors::EnvironmentSensor environment_sensor{};
sensors::LightLevelSensor light_level_sensor{};
sensors::SoilMoistureSensor soil_moisture_sensor{kSoilMoistureSensorPin};
pins::Pin sensors_power(kSensorsPowerPin, pins::PinType::kDigital, pins::PinMode::kOutputMode);

std::optional<Autopump> autopump;
std::optional<mqtt::readings::SensorReadingsRepository> sensors_readings_repo;
std::optional<mqtt::config::ConfigRemoteRepository> config_remote_repo;
std::optional<mqtt::state::PumpStateRepository> pump_state_repo;

WiFiEventHandler on_connected_wifi_handler;
WiFiEventHandler on_disconnected_wifi_handler;

void ConnectToWifi(const config::WifiConfig &wifi_config) {
  if (!utils::wifi::sta::TryConnectToWifi(
          wifi_config.wifi_ssid, wifi_config.wifi_pass, wifi_config.reconnect_timeout
      )) {
    LOG_TRACE(
        "couldn't connect to wifi, went to sleep on %llu ms",
        common_config.device_config.sync_period
    );
    delay(100);
    EspClass::deepSleep(common_config.device_config.sync_period * 1000);
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
  LOG_DEBUG("disconnected from wifi");
  if (mqtt_client) {
    mqtt_client->Disconnect();
  }
}

void OnConfigUpdate(const std::optional<config::CommonConfig> &updated_config) {
  LOG_TRACE("update config");
  common_config = updated_config.value_or(config::CommonConfig{});
}

void GoToDeepSleep() {
  sensors_power.SetState(0);
  Serial.end();
  WiFi.disconnect(true);
  yield();
  WiFi.forceSleepBegin();
  yield();
  WiFi.mode(WIFI_OFF);
  sensors_readings_repo.reset();
  autopump.reset();
  pump_state_repo.reset();
  mqtt_client.reset();
  config_remote_repo.reset();
  ntp_client.reset();
  digitalWrite(D2, 0);
  digitalWrite(D1, 0);
  digitalWrite(LED_BUILTIN, 1);
  EspClass::deepSleep(common_config.device_config.sync_period * 1000);
}

void setup() {
  if (LOG_LEVEL < static_cast<int>(logger::LogLevel::kNone)) {
    Serial.begin(115200);
    Serial.println();
  }
  LOG_INFO("start");

  Wire.begin();
  battery_charge_level_sensor.Init();
  light_level_sensor.Init();

  WiFi.setAutoConnect(false);

  common_config = common_config_repo.ReadConfig().value_or(config::CommonConfig{});
  if (!common_config.device_config.device_id.isEmpty()) {
    LOG_INFO("ran in working mode");
    LOG_TRACE("saved config: %s", ToJsonString(common_config).value_or("couldn't parse").c_str());

    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);

    sensors_power.SetState(1);
    battery_charge_level_sensor.Enable();

    ntp_client.emplace();
    mqtt_client.emplace(common_config.device_config.sync_period + 1_min, common_config);

    pump_state_repo.emplace(mqtt_client.value());
    autopump.emplace(
        kPumpPin,
        pump_state_repo.value(),
        common_config.device_config,
        soil_moisture_sensor,
        ntp_client.value()
    );
    sensors_readings_repo.emplace(
        mqtt_client.value(),
        soil_moisture_sensor,
        light_level_sensor,
        environment_sensor,
        battery_charge_level_sensor
    );
    config_remote_repo.emplace(mqtt_client.value(), common_config.mqtt_config, common_config_repo);

    on_connected_wifi_handler = WiFi.onStationModeConnected(OnWifiConnected);
    on_disconnected_wifi_handler = WiFi.onStationModeDisconnected(OnWifiDisconnected);
    ConnectToWifi(common_config.wifi_config);
    LOG_DEBUG("my ip is %s", WiFi.localIP().toString().c_str());

    ntp_client->Begin();
    LOG_DEBUG(
        "current time: %s", utils::FormatEpochSecondsAsDateTime(ntp_client->NowSinceEpoch()).c_str()
    );

    mqtt_client->Connect();
    sensors_readings_repo->SyncSensors();
    autopump->Enable();

    LOG_INFO("go to deep sleep");
    delay(100);
    GoToDeepSleep();
  } else {
    LOG_INFO("ran in configuration mode");
    WiFi.mode(WIFI_AP_STA);

    utils::wifi::ap::EnableAp();

    web_server.emplace(common_config.http_config.server_port);
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
  while (const auto task = task_queue.Pop()) {
    LOG_TRACE("call scheduled task in loop...");
    task.value()();
  }
}
