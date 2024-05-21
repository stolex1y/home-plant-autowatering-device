#include "autopump.h"

#include "logger.h"

namespace hpa {

using namespace pins;

Autopump::Autopump(
    pins::Pin::PinNumber pin_number,
    mqtt::state::PumpStateRepository& pump_state_repository,
    const config::DeviceConfig& device_config,
    const sensors::SoilMoistureSensor& soil_moisture_sensor,
    const utils::NtpClient& ntp_client
)
    : pin_(pin_number, PinType::kDigital, PinMode::kOutputMode),
      pump_state_repository_(pump_state_repository),
      device_config_(device_config),
      soil_moisture_sensor_(soil_moisture_sensor),
      ntp_client_(ntp_client) {
  pin_.SetState(0);
}

void Autopump::Enable() {
  LOG_TRACE("enable autopump");
  const auto soil_moisture = soil_moisture_sensor_.GetSoilMoisture();
  if (soil_moisture < device_config_.soil_moisture_min) {
    LOG_TRACE("need pump");
    const auto last_pump = pump_state_repository_.GetLastPumpTime();
    const auto now = ntp_client_.NowSinceEpoch();
    if ((now - last_pump).count() >= static_cast<int64_t>(device_config_.pump_delay)) {
      LOG_TRACE("continue pump after pump delay");
      EnablePump();
      delay(device_config_.pump_time);
      DisablePump();
      pump_state_repository_.UpdateLastPumpTime(now);
    }
  }
}

void Autopump::EnablePump() {
  LOG_TRACE("enable pump");
  pin_.SetState(1);
}

void Autopump::DisablePump() {
  LOG_TRACE("disable pump");
  pin_.SetState(0);
}

}  // namespace hpa
