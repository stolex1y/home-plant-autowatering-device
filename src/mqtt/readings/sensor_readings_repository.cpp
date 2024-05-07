#include "mqtt/readings/sensor_readings_repository.h"

#include "logger.h"

namespace hpa::mqtt::readings {

SensorReadingsRepository::SensorReadingsRepository(
    mqtt::MqttClient &mqtt_client,
    const sensors::SoilMoistureSensor &soil_moisture_sensor,
    const sensors::LightLevelSensor &light_level_sensor,
    const sensors::EnvironmentSensor &environment_sensor,
    const sensors::BatteryChargeLevelSensor &battery_charge_level_sensor
)
    : mqtt_client_(mqtt_client),
      soil_moisture_sensor_(soil_moisture_sensor),
      light_level_sensor_(light_level_sensor),
      environment_sensor_(environment_sensor),
      battery_charge_level_sensor_(battery_charge_level_sensor) {
}

void SensorReadingsRepository::SyncSensors() {
  LOG_DEBUG("start syncing sensors");
  SendSensorReading(soil_moisture_sensor_.GetSoilMoisture(), "soil-moisture");
  SendSensorReading(
      static_cast<int64_t>(light_level_sensor_.GetLightLevelLx().value_or(-1)), "light-level"
  );
  SendSensorReading(environment_sensor_.GetTemperature().value_or(-1), "air-temp");
  SendSensorReading(environment_sensor_.GetHumidity().value_or(-1), "air-humidity");
  SendSensorReading(battery_charge_level_sensor_.GetChargeLevel().value_or(-1), "battery-charge");
  const auto success = mqtt_client_.WaitFinishingAllPublications(kMaxWaitTime);
  if (!success) {
    LOG_ERROR("didn't published all sensors!");
    return;
  }
  LOG_DEBUG("successfully synced sensor readings");
}

}  // namespace hpa::mqtt::readings
