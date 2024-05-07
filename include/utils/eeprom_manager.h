#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include "logger.h"

namespace hpa::utils {

class EepromManager {
 public:
  using SizeType = uint16_t;

  class WriteTransaction {
   public:
    explicit WriteTransaction(EepromManager &eeprom_manager);
    ~WriteTransaction();

   private:
    EepromManager &eeprom_manager_;
  };

  static constexpr SizeType kDefaultMemSize = 4096;

  explicit EepromManager(SizeType mem_size = kDefaultMemSize);
  EepromManager(const EepromManager &other) = delete;
  ~EepromManager();

  template <typename T>
  void WriteDataAndMove(SizeType &address, const T &data);
  void WriteDataAndMove(SizeType &address, const String &data);
  template <typename T>
  void WriteData(SizeType address, const T &data);
  void WriteData(SizeType address, const String &string);

  template <typename T>
  void WriteDataAndMove(SizeType &address, const T *data, SizeType data_len);
  void WriteDataAndMove(SizeType &address, const uint8_t *data, SizeType data_len);
  template <typename T>
  void WriteData(SizeType address, const T *data, SizeType data_len);
  void WriteData(SizeType address, const uint8_t *data, SizeType data_len);

  void Commit();

  template <typename T>
  [[nodiscard]] T ReadDataAndMove(SizeType &address) const;
  [[nodiscard]] String ReadDataAndMove(SizeType &address) const;
  template <typename T>
  [[nodiscard]] T ReadData(SizeType address) const;
  [[nodiscard]] String ReadData(SizeType address) const;

  template <typename T>
  void ReadDataAndMove(SizeType &address, T *data, SizeType data_len) const;
  void ReadDataAndMove(SizeType &address, uint8_t *data, SizeType data_len) const;
  template <typename T>
  void ReadData(SizeType address, T *data, SizeType data_len) const;
  void ReadData(SizeType address, uint8_t *data, SizeType data_len) const;

  [[nodiscard]] WriteTransaction StartTransaction();
  [[nodiscard]] SizeType GetSize() const;

 private:
  SizeType size_;
};

template <typename T>
void EepromManager::WriteDataAndMove(SizeType &address, const T &data) {
  EEPROM.put(address, data);
  address += sizeof(data);
}

template <typename T>
void EepromManager::WriteData(SizeType address, const T &data) {
  SizeType address_cpy = address;
  WriteDataAndMove(address_cpy, data);
}

template <typename T>
void EepromManager::WriteDataAndMove(
    SizeType &address, const T *const data, const SizeType data_len
) {
  for (SizeType i = 0; i < data_len; ++i) {
    WriteDataAndMove(address, reinterpret_cast<const uint8_t *>(data + i), sizeof(T));
  }
}

template <typename T>
void EepromManager::WriteData(SizeType address, const T *const data, const SizeType data_len) {
  SizeType address_cpy = address;
  WriteDataAndMove(address_cpy, data, data_len);
}

template <typename T>
T EepromManager::ReadDataAndMove(SizeType &address) const {
  T data{};
  EEPROM.get(address, data);
  address += sizeof(T);
  return data;
}

template <typename T>
T EepromManager::ReadData(SizeType address) const {
  SizeType address_cpy = address;
  return ReadDataAndMove<T>(address_cpy);
}

template <typename T>
void EepromManager::ReadDataAndMove(SizeType &address, T *const data, const SizeType data_len)
    const {
  for (SizeType i = 0; i < data_len; ++i) {
    ReadDataAndMove(address, reinterpret_cast<uint8_t *>(data + i), sizeof(T));
  }
}

template <typename T>
void EepromManager::ReadData(SizeType address, T *const data, const SizeType data_len) const {
  SizeType address_cpy = address;
  return ReadDataAndMove(address_cpy, data, data_len);
}

}  // namespace hpa::utils
