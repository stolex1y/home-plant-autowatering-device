#include "mqtt/state/pump_state_repository.h"

#include <charconv>

#include "logger.h"

namespace hpa::mqtt::state {

PumpStateRepository::PumpStateRepository(MqttClient &mqtt_client) : mqtt_client_(mqtt_client) {
  mqtt_client.Subscribe(
      kPumpStateTopic,
      2,
      [this](const String &payload, const AsyncMqttClientMessageProperties &properties) {
        HandleMessage(payload);
      }
  );
}

PumpStateRepository::~PumpStateRepository() {
  mqtt_client_.Unsubscribe(kPumpStateTopic);
}

void PumpStateRepository::HandleMessage(const String &payload) {
  LOG_TRACE("receive message");
  uint64_t last_pump_time = 0;
  const auto res = std::from_chars(payload.begin(), payload.end(), last_pump_time);
  if (res.ec != std::errc()) {
    LOG_ERROR(
        "couldn't parse time from received message: %s. Errc - %d",
        payload.c_str(),
        static_cast<int>(res.ec)
    );
    return;
  }
  last_pump_time_ = std::chrono::seconds(last_pump_time);
  LOG_TRACE(
      "update last pump time: %s", utils::FormatEpochSecondsAsDateTime(last_pump_time_).c_str()
  );
}

std::chrono::seconds PumpStateRepository::GetLastPumpTime() const {
  return last_pump_time_;
}

void PumpStateRepository::UpdateLastPumpTime(std::chrono::seconds last_pump_time) {
  const String payload(last_pump_time.count());
  mqtt_client_.Publish(kPumpStateTopic, 2, true, payload);
}

}  // namespace hpa::mqtt::state
