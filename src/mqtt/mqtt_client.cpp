#include "mqtt/mqtt_client.h"

#include "logger.h"

namespace hpa::mqtt {

uint64_t MqttClient::next_publication_id_ = 0;

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
  setCleanSession(false);
  setClientId(config.device_config.device_id.c_str());
  const auto [host, port] = utils::SeparateUrlToHostAndPort(config.mqtt_config.url);
  const auto host_ip = utils::IpAddressFromString(host);
  if (!host_ip) {
    LOG_ERROR("unknown host: %s", host.c_str());
    return;
  }
  LOG_TRACE("set mqtt broker: %s:%d", host_ip.value().toString().c_str(), port);
  setServer(host_ip.value(), port);

  const auto will_topic = base_topic_ + config.mqtt_config.will_topic;
  LOG_TRACE("will topic: %s", will_topic.c_str());
  setWill("will", 2, true, "died", 4);

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
  subscription_packets_.clear();
  for (auto &future_sub : future_subscriptions_) {
    LOG_TRACE("send subscribe msg to topic: %s", future_sub.second.topic.c_str());
    const auto packet_id = subscribe(future_sub.second.topic.c_str(), future_sub.second.qos);
    subscription_packets_.emplace(packet_id, future_sub.second.topic);
  }

  publication_packets_.clear();
  for (const auto &[pub_id, pub] : publications_) {
    LOG_TRACE("publish msg to topic: %s", pub.topic.c_str());
    const auto packet_id =
        publish(pub.topic.c_str(), pub.qos, pub.retain, pub.payload.c_str(), pub.payload.length());
    publication_packets_.emplace(packet_id, pub_id);
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
  auto full_topic = base_topic_ + topic;
  LOG_DEBUG("subscribe to topic: %s", full_topic.c_str());

  if (subscriptions_.find(full_topic) != subscriptions_.end() ||
      future_subscriptions_.find(full_topic) != future_subscriptions_.end()) {
    LOG_INFO("already subscribed to topic, unsubscribe first");
    return;
  }

  future_subscriptions_.emplace(
      full_topic,
      FutureSubscription{.topic = full_topic, .on_message = std::move(on_message), .qos = qos}
  );

  auto &future_sub = future_subscriptions_.find(full_topic)->second;
  if (IsConnected()) {
    const auto packet_id = subscribe(future_sub.topic.c_str(), future_sub.qos);
    subscription_packets_.emplace(packet_id, future_sub.topic);
    LOG_TRACE("send subscribe msg");
  }
}

void MqttClient::Unsubscribe(const String &topic) {
  const auto full_topic = base_topic_ + topic;
  LOG_DEBUG("unsubscribe from topic: %s", full_topic.c_str());

  if (const auto &sub = subscriptions_.find(full_topic); sub != subscriptions_.end()) {
    unsubscribe(full_topic.c_str());
    subscriptions_.erase(sub);
  } else if (const auto &future_sub = future_subscriptions_.find(full_topic);
             future_sub != future_subscriptions_.end()) {
    unsubscribe(full_topic.c_str());
    future_subscriptions_.erase(future_sub);
  } else {
    LOG_INFO("already unsubscribed to topic, subscribe first");
    return;
  }
}

void MqttClient::Publish(
    const String &topic,
    const Qos qos,
    const bool retain,
    String payload,
    std::optional<MqttClient::OnPublishCallback> on_publish
) {
  const auto full_topic = base_topic_ + topic;
  LOG_DEBUG("publish message to topic: %s", full_topic.c_str());
  const auto pub_id = next_publication_id_++;
  publications_.emplace(
      pub_id,
      Publication{
          .topic = full_topic,
          .on_published = std::move(on_publish),
          .qos = qos,
          .payload = std::move(payload),
          .retain = retain
      }
  );
  if (IsConnected()) {
    const auto &pub = publications_.find(pub_id)->second;
    const auto packet_id =
        publish(pub.topic.c_str(), pub.qos, pub.retain, pub.payload.c_str(), pub.payload.length());
    publication_packets_.emplace(packet_id, pub_id);
    LOG_DEBUG("send publish message");
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
  return !publications_.empty();
}

void MqttClient::OnSubscribed(const PacketId packet_id) {
  LOG_TRACE("on subscribed message with id: %d", static_cast<int>(packet_id));
  const auto &subscription_packet = subscription_packets_.find(packet_id);
  if (subscription_packet == subscription_packets_.end()) {
    LOG_WARN("subscribed to unknown topic");
    return;
  }
  const auto &future_sub_it = future_subscriptions_.find(subscription_packet->second);
  if (future_sub_it == future_subscriptions_.end()) {
    LOG_ERROR("couldn't find future subscription");
  } else {
    auto &future_sub = future_sub_it->second;
    subscriptions_.emplace(
        future_sub.topic,
        Subscription{.topic = future_sub.topic, .on_message = std::move(future_sub.on_message)}
    );
    LOG_DEBUG("successfully subscribed to topic: %s", future_sub.topic.c_str());
    future_subscriptions_.erase(future_sub_it);
  }
  subscription_packets_.erase(subscription_packet);
}

void MqttClient::OnNewMessage(
    char *topic,
    char *payload,
    AsyncMqttClientMessageProperties properties,
    size_t len,
    size_t index,
    size_t total
) {
  const auto &sub_it = subscriptions_.find(topic);
  if (sub_it == subscriptions_.end()) {
    LOG_WARN("got message without subscriber with topic: %s, unsubscribe..", topic);
    unsubscribe(topic);
    return;
  }
  auto &sub = sub_it->second;
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
  LOG_TRACE(
      "successfully unsubscribed from topic by message with id: %d", static_cast<int>(packet_id)
  );
}

void MqttClient::OnPublished(const PacketId packet_id) {
  const auto &pub_packet = publication_packets_.find(packet_id);
  if (pub_packet == publication_packets_.end()) {
    LOG_WARN("published unknown message");
    return;
  }
  const auto &pub_it = publications_.find(pub_packet->second);
  if (pub_it == publications_.end()) {
    LOG_ERROR("couldn't find publication by id");
  } else {
    const auto &pub = pub_it->second;
    const auto &callback = pub.on_published;
    if (callback) {
      callback.value()();
    }
    LOG_DEBUG("successfully publish message to topic: %s", pub.topic.c_str());
    publications_.erase(pub_it);
  }
  publication_packets_.erase(packet_id);
}

}  // namespace hpa::mqtt
