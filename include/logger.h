#pragma once

#include <Arduino.h>

#include <cinttypes>
#include <optional>

#ifndef LOG_LEVEL
#define LOG_LEVEL 5
#endif

#define NOOP \
  do {       \
  } while (0)

namespace hpa::logger {

enum class LogLevel : int {
  kTrace = 0,
  kDebug = 1,
  kInfo = 2,
  kWarn = 3,
  kError = 4,
  kNone = 5,
  kCount = 6,
};

}  // namespace hpa::logger

#define LOG_TRACE(...)                                                          \
  if constexpr (LOG_LEVEL <= static_cast<int>(hpa::logger::LogLevel::kTrace)) { \
    Serial.printf("[TRACE] [" __FILE__ ":%d] [%lu]: ", __LINE__, millis());     \
    Serial.printf(__VA_ARGS__);                                                 \
    Serial.println();                                                           \
  } else                                                                        \
    NOOP

#define LOG_DEBUG(...)                                                          \
  if constexpr (LOG_LEVEL <= static_cast<int>(hpa::logger::LogLevel::kDebug)) { \
    Serial.printf("[DEBUG] [" __FILE__ ":%d] [%lu]: ", __LINE__, millis());     \
    Serial.printf(__VA_ARGS__);                                                 \
    Serial.println();                                                           \
  } else                                                                        \
    NOOP

#define LOG_INFO(...)                                                          \
  if constexpr (LOG_LEVEL <= static_cast<int>(hpa::logger::LogLevel::kInfo)) { \
    Serial.printf("[INFO] [" __FILE__ ":%d] [%lu]: ", __LINE__, millis());     \
    Serial.printf(__VA_ARGS__);                                                \
    Serial.println();                                                          \
  } else                                                                       \
    NOOP

#define LOG_WARN(...)                                                          \
  if constexpr (LOG_LEVEL <= static_cast<int>(hpa::logger::LogLevel::kWarn)) { \
    Serial.printf("[WARN] [" __FILE__ ":%d] [%lu]: ", __LINE__, millis());     \
    Serial.printf(__VA_ARGS__);                                                \
    Serial.println();                                                          \
  } else                                                                       \
    NOOP

#define LOG_ERROR(...)                                                          \
  if constexpr (LOG_LEVEL <= static_cast<int>(hpa::logger::LogLevel::kError)) { \
    Serial.printf("[ERROR] [" __FILE__ ":%d] [%lu]: ", __LINE__, millis());     \
    Serial.printf(__VA_ARGS__);                                                 \
    Serial.println();                                                           \
  } else                                                                        \
    NOOP
