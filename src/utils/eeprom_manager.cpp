#include "utils/eeprom_manager.h"

#include "logger.h"

namespace hpa::utils {

static const EepromManager::SizeType kMaxSize = 4096;

EepromManager::WriteTransaction::WriteTransaction(EepromManager &eeprom_manager)
    : eeprom_manager_(eeprom_manager) {
  LOG_DEBUG("start write transaction");
}

EepromManager::WriteTransaction::~WriteTransaction() {
  eeprom_manager_.Commit();
  LOG_DEBUG("close transaction");
}

EepromManager::EepromManager(const SizeType mem_size) : size_(mem_size) {
  if (mem_size > kMaxSize) {
    LOG_ERROR("eeprom size must be less or equal than %d", static_cast<int>(kMaxSize));
    assert(false);
  }
  EEPROM.begin(mem_size);
}

EepromManager::~EepromManager() {
  EEPROM.end();
}

EepromManager::WriteTransaction EepromManager::StartTransaction() {
  return WriteTransaction(*this);
}

EepromManager::SizeType EepromManager::GetSize() const {
  return size_;
}

void EepromManager::ReadDataAndMove(SizeType &address, uint8_t *const data, const SizeType data_len)
    const {
  for (SizeType i = 0; i < data_len; ++i) {
    data[i] = EEPROM.read(address + i);
  }
  address += data_len;
}

void EepromManager::ReadData(SizeType address, uint8_t *const data, const SizeType data_len) const {
  SizeType address_cpy = address;
  return ReadDataAndMove(address_cpy, data, data_len);
}

String EepromManager::ReadDataAndMove(SizeType &address) const {
  String string{};
  const auto str_len = ReadDataAndMove<SizeType>(address);
  if (!string.reserve(str_len)) {
    LOG_ERROR("couldn't read string - have no memory");
  }
  for (SizeType i = 0; i < str_len; ++i) {
    string += ReadDataAndMove<char>(address);
  }
  return string;
}

String EepromManager::ReadData(SizeType address) const {
  SizeType address_cpy = address;
  return ReadDataAndMove(address_cpy);
}

void EepromManager::WriteDataAndMove(
    SizeType &address, const uint8_t *const data, const SizeType data_len
) {
  for (SizeType i = 0; i < data_len; ++i) {
    EEPROM.write(address + i, data[i]);
  }
  address += data_len;
}

void EepromManager::WriteData(
    SizeType address, const uint8_t *const data, const SizeType data_len
) {
  SizeType address_cpy = address;
  WriteDataAndMove(address_cpy, data, data_len);
}

void EepromManager::WriteDataAndMove(SizeType &address, const String &string) {
  const SizeType str_len = string.length();
  WriteDataAndMove(address, str_len);
  WriteDataAndMove(address, string.c_str(), str_len);
}

void EepromManager::WriteData(SizeType address, const String &string) {
  SizeType address_cpy = address;
  WriteDataAndMove(address_cpy, string);
}

void EepromManager::Commit() {
  EEPROM.commit();
}

}  // namespace hpa::utils
