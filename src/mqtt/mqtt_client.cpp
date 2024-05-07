#include "mqtt/mqtt_client.h"

#include <algorithm>

#include "logger.h"
#include "utils/utils.h"

namespace hpa::mqtt {

String MqttClient::DisconnectReasonToString(const AsyncMqttClientDisconnectReason reason) {
  switch (reason) {
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
      return "TCP_DISCONNECTED";
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
      return "MQTT_UNACCEPTABLE_PROTOCOL_VERSION";
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
      return "MQTT_IDENTIFIER_REJECTED";
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
      return "MQTT_SERVER_UNAVAILABLE";
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
      return "MQTT_MALFORMED_CREDENTIALS";
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
      return "MQTT_NOT_AUTHORIZED";
    case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
      return "ESP8266_NOT_ENOUGH_SPACE";
    case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
      return "TLS_BAD_FINGERPRINT";
  }
  return "UNKNOWN";
}

MqttClient::MqttClient(const uint64_t keep_alive, const config::CommonConfig &config)
    : base_topic_(config.mqtt_config.base_topic + "/" + config.device_config.device_id + "/") {
  setKeepAlive(std::max<uint64_t>(1, keep_alive / 1000));
  setCredentials(config.mqtt_config.username.c_str(), config.mqtt_config.password.c_str());
  const auto [host, port] = utils::SeparateUrlToHostAndPort(config.mqtt_config.url);
  const auto host_ip = utils::IpAddressFromString(host);
  if (!host_ip) {
    LOG_ERROR("unknown host: %s", host.c_str());
    return;
  }
  LOG_TRACE("set mqtt broker: %s:%d", host_ip.value().toString().c_str(), port);
  setServer(host_ip.value(), port);

  /*
    const auto will_topic = base_topic_ + config.mqtt_config.will_topic;
    LOG_TRACE("will topic: %s", will_topic.c_str());
    setWill(will_topic.c_str(), 2, true, "died", 4);
  */

  onPublish([this](PacketId packet_id) {
    OnPublished(packet_id);
  });
  onSubscribe([this](PacketId packet_id, Qos qos) {
    OnSubscribed(packet_id);
  });
  onConnect([this](bool session_present) {
    OnConnected();
  });
  onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
    OnDisconnected(reason);
  });
  onMessage([this](
                char *topic,
                char *payload,
                AsyncMqttClientMessageProperties properties,
                size_t len,
                size_t index,
                size_t total
            ) {
    OnNewMessage(topic, payload, properties, len, index, total);
  });
}

void MqttClient::Connect() {
  if (connect_) {
    return;
  }

  LOG_DEBUG("connect to mqtt broker");
  connect_ = true;
  reconnecting_timer_.detach();
  connect();
}

void MqttClient::Disconnect() {
  if (!connect_) {
    return;
  }

  LOG_DEBUG("disconnect from mqtt broker");
  connect_ = false;
  reconnecting_timer_.detach();
  disconnect(false);
}

void MqttClient::OnConnected() {
  LOG_DEBUG("connected to broker");
  for (auto &sub : subscriptions_) {
    LOG_TRACE("send subscribe msg to topic: %s", sub.topic.c_str());
    sub.packet_id = subscribe(sub.topic.c_str(), sub.qos);
  }
  while (!future_publications_.empty()) {
    auto &pub = future_publications_.front();
    LOG_TRACE("publish msg to topic: %s", pub.topic.c_str());
    const auto packet_id =
        publish(pub.topic.c_str(), pub.qos, pub.retain, pub.payload.c_str(), pub.payload.length());
    publications_.emplace_back(Publication{
        .topic = std::move(pub.topic),
        .packet_id = packet_id,
        .on_published = std::move(pub.on_published)
    });
    future_publications_.pop();
  }
}

void MqttClient::OnDisconnected(const AsyncMqttClientDisconnectReason reason) {
  LOG_DEBUG("disconnected from broker: %s", DisconnectReasonToString(reason).c_str());
  if (connect_) {
    reconnecting_timer_.once(kDefaultReconnectDelay / 1000.0, [this]() {
      LOG_TRACE("reconnect to broker");
      Connect();
    });
  }
}

void MqttClient::Subscribe(
    const String &topic, const Qos qos, MqttClient::OnMessageCallback on_message
) {
  const auto full_topic = base_topic_ + topic;
  LOG_DEBUG("subscribe to topic: %s", full_topic.c_str());
  auto &subscription = GetSubscriptionOrPut(full_topic);
  subscription.topic = full_topic;
  subscription.on_message = std::move(on_message);
  subscription.qos = qos;
  if (connected()) {
    subscription.packet_id = subscribe(subscription.topic.c_str(), subscription.qos);
  }
}

void MqttClient::Unsubscribe(const String &topic) {
  const auto full_topic = base_topic_ + topic;
  LOG_DEBUG("unsubscribe from topic: %s", full_topic.c_str());
  const auto sub = FindSubscription(full_topic);
  if (sub) {
    sub.value()->packet_id = unsubscribe(full_topic.c_str());
  }
}

void MqttClient::Publish(
    const String &topic,
    const Qos qos,
    const bool retain,
    const String &payload,
    std::optional<MqttClient::OnPublishCallback> on_publish
) {
  const auto full_topic = base_topic_ + topic;
  LOG_DEBUG("publish message to topic: %s", full_topic.c_str());
  if (IsConnected()) {
    const auto packet_id =
        publish(full_topic.c_str(), qos, retain, payload.c_str(), payload.length());
    publications_.emplace_back(Publication{
        .topic = full_topic, .packet_id = packet_id, .on_published = std::move(on_publish)
    });
  } else {
    future_publications_.emplace(FuturePublication{
        .topic = full_topic,
        .on_published = std::move(on_publish),
        .qos = qos,
        .payload = payload,
        .retain = retain
    });
  }
}

bool MqttClient::IsConnected() const {
  return connected();
}

bool MqttClient::WaitFinishingAllPublications(uint64_t timeout) const {
  const auto start = millis();
  if (timeout == 0) {
    return !HasPublications();
  }
  while (millis() - start < timeout) {
    if (!HasPublications()) {
      return true;
    }
    yield();
  }
  return !HasPublications();
}

bool MqttClient::HasPublications() const {
  return !publications_.empty() || !future_publications_.empty();
}

void MqttClient::OnSubscribed(const PacketId packet_id) {
  const auto subscription = FindSubscription(packet_id);
  if (!subscription) {
    LOG_WARN("subscribed to unknown topic");
    return;
  }
  LOG_DEBUG("successfully subscribed to topic: %s", subscription.value()->topic.c_str());
}

void MqttClient::OnNewMessage(
    char *topic,
    char *payload,
    AsyncMqttClientMessageProperties properties,
    size_t len,
    size_t index,
    size_t total
) {
  const auto sub_it = FindSubscription(topic);
  if (!sub_it) {
    LOG_WARN("got message without subscriber with topic: %s", topic);
    unsubscribe(topic);
    return;
  }
  auto &sub = *sub_it.value();
  if (!index) {
    LOG_TRACE("start handling new message payload by topic: %s", topic);
    LOG_TRACE("reserve %zu bytes to message", total);
    sub.message_buf.reserve(total);
  }
  sub.message_buf.concat(reinterpret_cast<char *>(payload), len);

  if (index + len == total) {
    LOG_TRACE("finish handling new message payload");
    sub.on_message(sub.message_buf, properties);
    sub.message_buf = String();
  }
}

void MqttClient::OnUnsubscribed(const PacketId packet_id) {
  const auto subscription = FindSubscription(packet_id);
  if (!subscription) {
    LOG_WARN("subscribed to unknown topic");
    return;
  }
  LOG_DEBUG("successfully unsubscribed from topic: %s", subscription.value()->topic.c_str());
  subscriptions_.erase(subscription.value());
}

void MqttClient::OnPublished(const PacketId packet_id) {
  const auto it = FindPublication(packet_id);
  if (!it) {
    LOG_WARN("published unknown message");
    return;
  }
  LOG_DEBUG("successfully publish message to topic: %s", it.value()->topic.c_str());
  const auto &callback = it.value()->on_published;
  if (callback) {
    callback.value()();
  }
  publications_.erase(it.value());
}

MqttClient::Subscription &MqttClient::GetSubscriptionOrPut(const String &topic) {
  auto it = FindSubscription(topic);
  if (!it) {
    subscriptions_.emplace_back();
    return subscriptions_.back();
  }
  return *it.value();
}

std::optional<std::vector<MqttClient::Subscription>::iterator> MqttClient::FindSubscription(
    const String &topic
) {
  auto it =
      std::find_if(subscriptions_.begin(), subscriptions_.end(), [&topic](const Subscription &sub) {
        return sub.topic == topic;
      });
  if (it == subscriptions_.end()) {
    return std::nullopt;
  }
  return it;
}

std::optional<std::vector<MqttClient::Subscription>::iterator> MqttClient::FindSubscription(
    const PacketId packet_id
) {
  auto it = std::find_if(
      subscriptions_.begin(),
      subscriptions_.end(),
      [packet_id](const Subscription &sub) {
        return sub.packet_id == packet_id;
      }
  );
  if (it == subscriptions_.end()) {
    return std::nullopt;
  }
  return it;
}

std::optional<std::vector<MqttClient::Publication>::iterator> MqttClient::FindPublication(
    const PacketId packet_id
) {
  auto it =
      std::find_if(publications_.begin(), publications_.end(), [packet_id](const Publication &pub) {
        return pub.packet_id == packet_id;
      });
  if (it == publications_.end()) {
    return std::nullopt;
  }
  return it;
}

}  // namespace hpa::mqtt
