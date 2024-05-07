#include "mqtt/state/sync_state_repository.h"

#include <charconv>

#include "logger.h"

namespace hpa::mqtt::state {

SyncStateRepository::SyncStateRepository(MqttClient &mqtt_client) : mqtt_client_(mqtt_client) {
  mqtt_client.Subscribe(
      kSyncStateTopic,
      2,
      [this](const String &payload, const AsyncMqttClientMessageProperties &properties) {
        HandleMessage(payload);
      }
  );
}

SyncStateRepository::~SyncStateRepository() {
  mqtt_client_.Unsubscribe(kSyncStateTopic);
}

void SyncStateRepository::HandleMessage(const String &payload) {
  LOG_TRACE("receive message");
  uint64_t sync_time = 0;
  const auto res = std::from_chars(payload.begin(), payload.end(), sync_time);
  if (res.ec != std::errc()) {
    LOG_ERROR(
        "couldn't parse time from received message: %s. Errc - %d",
        payload.c_str(),
        static_cast<int>(res.ec)
    );
    return;
  }
  last_sync_time_ = std::chrono::seconds(sync_time);
  LOG_TRACE(
      "update last sync time: %s", utils::FormatEpochSecondsAsDateTime(last_sync_time_).c_str()
  );
}

std::chrono::seconds SyncStateRepository::GetLastSyncTime() const {
  return last_sync_time_;
}

void SyncStateRepository::UpdateLastSyncTime(std::chrono::seconds last_sync_time) {
  const String payload(last_sync_time.count());
  mqtt_client_.Publish(kSyncStateTopic, 2, true, payload);
}

}  // namespace hpa::mqtt::state
