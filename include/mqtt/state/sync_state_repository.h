#pragma once

#include "mqtt/mqtt_client.h"

namespace hpa::mqtt::state {

class SyncStateRepository {
 public:
  explicit SyncStateRepository(MqttClient &mqtt_client);
  ~SyncStateRepository();

  [[nodiscard]] std::chrono::seconds GetLastSyncTime() const;
  void UpdateLastSyncTime(std::chrono::seconds last_sync_time);

 private:
  static constexpr const auto kSyncStateTopic = "sync-state";

  std::chrono::seconds last_sync_time_{0};
  MqttClient &mqtt_client_;

  void HandleMessage(const String &payload);
};

}  // namespace hpa::mqtt::state
