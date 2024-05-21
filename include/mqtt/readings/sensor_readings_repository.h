#pragma once

#include "config/device_config.h"
#include "mqtt/mqtt_client.h"
#include "sensors/battery_charge_level_sensor.h"
#include "sensors/environment_sensor.h"
#include "sensors/light_level_sensor.h"
#include "sensors/soil_moisture_sensor.h"
#include "utils/utils.h"

namespace hpa::mqtt::readings {

using namespace utils::chrono_literals;

class SensorReadingsRepository {
 public:
  SensorReadingsRepository(
      mqtt::MqttClient &mqtt_client,
      const sensors::SoilMoistureSensor &soil_moisture_sensor,
      const sensors::LightLevelSensor &light_level_sensor,
      const sensors::EnvironmentSensor &environment_sensor,
      const sensors::BatteryChargeLevelSensor &battery_charge_level_sensor
  );

  void SyncSensors();

 private:
  static constexpr const auto kMaxWaitTime = 10_s;

  mqtt::MqttClient &mqtt_client_;
  const sensors::SoilMoistureSensor &soil_moisture_sensor_;
  const sensors::LightLevelSensor &light_level_sensor_;
  const sensors::EnvironmentSensor &environment_sensor_;
  const sensors::BatteryChargeLevelSensor &battery_charge_level_sensor_;

  template <typename T>
  void SendSensorReading(T reading, const String &sensor_name);
};

template <typename T>
void SensorReadingsRepository::SendSensorReading(const T reading, const String &sensor_name) {
  mqtt_client_.Publish(sensor_name, 2, true, String(reading));
}

}  // namespace hpa::mqtt::readings
