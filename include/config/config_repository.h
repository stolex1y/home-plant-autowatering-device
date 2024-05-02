#ifndef HOME_PLANT_AUTOWATERING_DEVICE_BASE_CONFIG_REPOSITORY_H
#define HOME_PLANT_AUTOWATERING_DEVICE_BASE_CONFIG_REPOSITORY_H

#include "eeprom_manager.h"
#include "json.h"

namespace hpa::config {

template <typename Config>
class ConfigRepository {
 public:
  ConfigRepository(
      EepromManager &eeprom_manager,
      EepromManager::SizeType begin_addr,
      EepromManager::SizeType end_addr
  );

  [[nodiscard]] bool HasConfig() const;
  [[nodiscard]] std::optional<Config> ReadConfig() const;
  bool WriteConfig(const Config &config);
  void ResetConfig();

 private:
  static constexpr uint8 kInitFlag_ = 42;

  EepromManager &eeprom_manager_;
  EepromManager::SizeType begin_addr_;
  EepromManager::SizeType end_addr_;

  void SetInitFlag();
  void ResetInitFlag();
  [[nodiscard]] bool IsInitFlagSet() const;
};

template <typename Config>
ConfigRepository<Config>::ConfigRepository(
    EepromManager &eeprom_manager,
    EepromManager::SizeType begin_addr,
    EepromManager::SizeType end_addr
)
    : eeprom_manager_(eeprom_manager), begin_addr_(begin_addr), end_addr_(end_addr) {
}

template <typename Config>
bool ConfigRepository<Config>::HasConfig() const {
  return IsInitFlagSet();
}

template <typename Config>
std::optional<Config> ConfigRepository<Config>::ReadConfig() const {
  if (!HasConfig()) {
    return std::nullopt;
  }
  EepromManager::SizeType addr = begin_addr_ + sizeof(kInitFlag_);
  const String config_str = eeprom_manager_.ReadData(addr);
  LOG_TRACE("read config: %s", config_str.c_str());
  auto config = FromJsonString<Config>(config_str).second;
  if (!config) {
    LOG_ERROR("couldn't parse config from json");
    return std::nullopt;
  }
  return config;
}

template <typename Config>
bool ConfigRepository<Config>::WriteConfig(const Config &config) {
  EepromManager::SizeType addr = begin_addr_ + sizeof(kInitFlag_);
  auto config_str = ToJsonString(config);
  if (!config_str) {
    LOG_ERROR("couldn't convert to json");
    return false;
  }
  if (sizeof(config) + addr > end_addr_) {
    LOG_ERROR("config size more than memory size");
    return false;
  }
  eeprom_manager_.WriteData(addr, config_str.value());
  SetInitFlag();
  eeprom_manager_.Commit();
  const auto written_config = ReadConfig();
  if (config != written_config) {
    ResetInitFlag();
    eeprom_manager_.Commit();
    LOG_ERROR("written config isn't equals to source");
    return false;
  }
  LOG_TRACE("wrote config: %s", config_str->c_str());
  return true;
}

template <typename Config>
void ConfigRepository<Config>::ResetConfig() {
  const auto transaction = eeprom_manager_.StartTransaction();
  ResetInitFlag();
}

// Must be called in transaction!
template <typename Config>
void ConfigRepository<Config>::SetInitFlag() {
  LOG_TRACE("set init flag");
  eeprom_manager_.WriteData(begin_addr_, kInitFlag_);
}

// Must be called in transaction!
template <typename Config>
void ConfigRepository<Config>::ResetInitFlag() {
  LOG_TRACE("reset init flag");
  eeprom_manager_.WriteData(begin_addr_, ~kInitFlag_);
}

template <typename Config>
bool ConfigRepository<Config>::IsInitFlagSet() const {
  return eeprom_manager_.ReadData<uint8_t>(begin_addr_) == kInitFlag_;
}

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_BASE_CONFIG_REPOSITORY_H
