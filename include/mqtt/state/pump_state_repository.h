#pragma once

#include "mqtt/mqtt_client.h"

namespace hpa::mqtt::state {

class PumpStateRepository {
 public:
  explicit PumpStateRepository(MqttClient &mqtt_client);
  ~PumpStateRepository();

  [[nodiscard]] std::chrono::seconds GetLastPumpTime() const;
  void UpdateLastPumpTime(std::chrono::seconds last_pump_time);

 private:
  static constexpr const auto kPumpStateTopic = "pump";

  std::chrono::seconds last_pump_time_{0};
  MqttClient &mqtt_client_;

  void HandleMessage(const String &payload);
};

}  // namespace hpa::mqtt::state
