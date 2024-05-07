#include "sensors/battery_charge_level_sensor.h"

#include <Arduino.h>

#include "logger.h"

namespace hpa::sensors {

std::optional<float> BatteryChargeLevelSensor::GetChargeLevel() const {
  LOG_DEBUG("get charge level");

  if (sensor_.getOverflow()) {
    LOG_ERROR("current sensor is overflow!");
    return std::nullopt;
  }

  const auto start = millis();
  if (!sensor_.startSingleMeasurement(kMeasuringTimeout * 1000)) {
    LOG_ERROR("couldn't get reading");
    return std::nullopt;
  }
  const auto shunt_voltage_mv = sensor_.getShuntVoltage_mV();
  const auto bus_voltage_v = sensor_.getBusVoltage_V();
  const auto load_voltage_v = bus_voltage_v + (shunt_voltage_mv / 1000);
  const auto charge_level =
      constrain((load_voltage_v - kMinVoltage) / (kMaxVoltage - kMinVoltage) * 100, 0, 100);
  const auto stop = millis();

  LOG_TRACE("shunt voltage: %.02f mV", shunt_voltage_mv);
  LOG_TRACE("bus voltage: %.02f V", bus_voltage_v);
  LOG_TRACE("load voltage: %.02f V", load_voltage_v);
  LOG_TRACE("current current: %.02f mA", sensor_.getCurrent_mA());
  LOG_TRACE("measuring time: %lu ms", (stop - start));
  LOG_TRACE("current charge level: %.02f%% (%.02f v)", charge_level, load_voltage_v);

  return charge_level;
}

void BatteryChargeLevelSensor::Init() {
  LOG_DEBUG("init sensor");
  if (!sensor_.init()) {
    LOG_ERROR("sensor isn't connected");
  }
}

void BatteryChargeLevelSensor::Enable() {
  sensor_.powerUp();
  sensor_.setMeasureMode(TRIGGERED);
  sensor_.setBusRange(BRNG_16);
  sensor_.setPGain(PG_160);
  sensor_.setADCMode(SAMPLE_MODE_128);
}

void BatteryChargeLevelSensor::Disable() {
  sensor_.powerDown();
}

}  // namespace hpa::sensors
