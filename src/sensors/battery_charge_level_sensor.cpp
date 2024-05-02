#include "battery_charge_level_sensor.h"

#include <Arduino.h>

#include "logger.h"

namespace hpa::sensors {

float BatteryChargeLevelSensor::GetChargeLevel() const {
  LOG_DEBUG("get charge level");
  if (sensor_.getOverflow()) {
    LOG_ERROR("current sensor is overflow!");
  }
  const auto shunt_voltage_mv = sensor_.getShuntVoltage_mV();
  const auto bus_voltage_v = sensor_.getBusVoltage_V();
  const auto load_voltage_v = bus_voltage_v + (shunt_voltage_mv / 1000);
  const auto charge_level =
      constrain((load_voltage_v - kMinVoltage) / (kMaxVoltage - kMinVoltage) * 100, 0, 100);
  LOG_TRACE("current charge level: %d%%", static_cast<int>(charge_level));
  return charge_level;
}

void BatteryChargeLevelSensor::Enable() {
  sensor_.powerUp();
  sensor_.setMeasureMode(CONTINUOUS);
}

void BatteryChargeLevelSensor::Disable() {
  sensor_.powerDown();
}

void BatteryChargeLevelSensor::Init() {
  if (!sensor_.init()) {
    LOG_ERROR("sensor isn't connected");
  }
  sensor_.setBusRange(BRNG_16);
  sensor_.setPGain(PG_160);
  sensor_.setADCMode(SAMPLE_MODE_128);
}

}  // namespace hpa::sensors
