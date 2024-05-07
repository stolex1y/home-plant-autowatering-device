#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <utility>

#include "logger.h"
#include "utils/utils.h"

namespace hpa {

String ToJson(const String &value);

template <typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
String ToJson(const T &value) {
  return String(value);
}

template <typename T>
String FormatToJsonField(const String &field_name, const T &value, bool last = false) {
  String json = utils::Format(R"("%s": "%s")", field_name.c_str(), ToJson(value).c_str());
  if (!last) {
    json += ',';
  }
  json += '\n';
  return json;
}

template <typename T>
bool SetFieldFromJson(const JsonVariantConst &json, const String &field_name, T &field) {
  const auto &json_field = json[field_name];
  if (json_field.isNull())
    return false;
  field = json_field.as<T>();
  return true;
}

template <typename T>
bool SetJsonFromField(JsonVariant &json, const String &field_name, const T &field) {
  return json[field_name] = field;
}

template <typename T>
std::optional<String> ToJsonString(const T &value) {
  JsonDocument json;
  if (!json.set(value)) {
    LOG_DEBUG("couldn't convert to json");
    return std::nullopt;
  }
  String json_str;
  serializeJson(json, json_str);
  return json_str;
}

template <typename T>
std::pair<DeserializationError, std::optional<T>> FromJsonString(const String &json) {
  JsonDocument json_doc;
  const auto error = deserializeJson(json_doc, json);
  if (error) {
    LOG_DEBUG("couldn't parse json: %s", error.c_str());
    return std::make_pair(error, std::nullopt);
  }
  return std::make_pair(error, json_doc.as<T>());
}

}  // namespace hpa
