#ifndef HOME_PLANT_AUTOWATERING_DEVICE_MQTT_CLIENT_H
#define HOME_PLANT_AUTOWATERING_DEVICE_MQTT_CLIENT_H

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>

#include "config/common_config.h"
#include "utils.h"

namespace hpa::mqtt {

class MqttClient : private AsyncMqttClient {
 public:
  using PacketId = uint16_t;
  using Qos = uint8_t;
  using OnMessageCallback = std::function<
      void(const String &payload, const AsyncMqttClientMessageProperties &properties)>;
  using OnPublishCallback = std::function<void()>;

  MqttClient(uint64_t keep_alive, const config::CommonConfig &config);

  void Connect();
  void Disconnect();
  void Subscribe(const String &topic, Qos qos, OnMessageCallback on_message);
  void Unsubscribe(const String &topic);
  void Publish(
      const String &topic,
      Qos qos,
      bool retain,
      const String &payload,
      std::optional<MqttClient::OnPublishCallback> on_publish = std::nullopt
  );
  [[nodiscard]] bool IsConnected() const;

 private:
  struct Subscription {
    String topic;
    OnMessageCallback on_message;
    Qos qos;
    PacketId packet_id;
    String message_buf;
  };

  struct Publication {
    String topic;
    PacketId packet_id;
    std::optional<OnPublishCallback> on_published;
  };

  static constexpr const uint64_t kDefaultReconnectDelay = 2000;

  std::vector<Subscription> subscriptions_{};
  std::vector<Publication> publications_{};
  bool connect_ = false;
  Ticker reconnecting_timer_;

  static String DisconnectReasonToString(AsyncMqttClientDisconnectReason reason);

  void OnConnected();
  void OnDisconnected(AsyncMqttClientDisconnectReason reason);
  void OnSubscribed(PacketId packet_id);
  void OnUnsubscribed(PacketId packet_id);
  void OnPublished(PacketId packet_id);
  void OnNewMessage(
      char *topic,
      char *payload,
      AsyncMqttClientMessageProperties properties,
      size_t len,
      size_t index,
      size_t total
  );
  Subscription &GetSubscriptionOrPut(const String &topic);
  std::optional<std::vector<Subscription>::iterator> FindSubscription(PacketId packet_id);
  std::optional<std::vector<Subscription>::iterator> FindSubscription(const String &topic);
  std::optional<std::vector<Publication>::iterator> FindPublication(PacketId packet_id);
};

}  // namespace hpa::mqtt

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_MQTT_CLIENT_H