#pragma once

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>

#include <unordered_set>

#include "config/common_config.h"

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
      String payload,
      std::optional<MqttClient::OnPublishCallback> on_publish = std::nullopt
  );
  [[nodiscard]] bool IsConnected() const;
  [[nodiscard]] bool WaitFinishingAllPublications(uint64_t timeout) const;
  [[nodiscard]] bool HasPublications() const;

 private:
  using PublicationId = uint64_t;

  struct Subscription {
    String topic;
    OnMessageCallback on_message;
    String message_buf;
  };

  struct FutureSubscription {
    String topic;
    OnMessageCallback on_message;
    Qos qos;
  };

  struct Publication {
    String topic;
    std::optional<OnPublishCallback> on_published;
    Qos qos;
    String payload;
    bool retain;
  };

  static constexpr const uint64_t kDefaultReconnectDelay = 2000;

  static uint64_t next_publication_id_;

  std::unordered_map<PublicationId, Publication> publications_{};
  std::unordered_map<PacketId, PublicationId> publication_packets_{};
  std::unordered_map<String, FutureSubscription, utils::StringHash> future_subscriptions_{};
  std::unordered_map<PacketId, std::reference_wrapper<String>> subscription_packets_{};
  std::unordered_map<String, Subscription, utils::StringHash> subscriptions_{};
  bool connect_ = false;
  Ticker reconnecting_timer_;
  String base_topic_;

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
};

}  // namespace hpa::mqtt
