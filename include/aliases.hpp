#include "File.hpp"
#include "MemoryStorage.hpp"
#include "Storage.hpp"
#include <filesystem>

namespace gorage {
template <class T>
void indexes(gorage::Storage<T> &storage,
             const std::map<std::string, typename gorage::Storage<T>::Index::Extractor::V> &functions) {
  auto indexes = std::map<std::string, typename gorage::Storage<T>::Index>();
  for (const auto &n_f : functions)
    indexes[n_f.first] = typename gorage::Storage<T>::Index(
        n_f.second, std::make_shared<gorage::MemoryStorage<gorage::Pointer<gorage::Storage<gorage::Key>>>>(),
        std::make_shared<gorage::MemoryStorage<gorage::Pointer<gorage::Storage<std::string>>>>(),
        [](const std::string &value) {
          return gorage::Pointer<gorage::Storage<gorage::Key>>(std::make_shared<gorage::MemoryStorage<gorage::Key>>());
        },
        [](const gorage::Key &key) {
          return gorage::Pointer<gorage::Storage<std::string>>(std::make_shared<gorage::MemoryStorage<std::string>>());
        });
  storage.indexes(indexes);
}
template <class T>
void indexes(gorage::Storage<T> &storage,
             const std::map<std::string, typename gorage::Storage<T>::Index::Extractor::V> &functions,
             const std::filesystem::path &path) {
  auto indexes = std::map<std::string, typename gorage::Storage<T>::Index>();
  for (const auto &n_f : functions)
    indexes[n_f.first] = typename gorage::Storage<T>::Index(
        n_f.second, std::make_shared<gorage::MemoryStorage<gorage::Pointer<gorage::Storage<gorage::Key>>>>(),
        std::make_shared<gorage::MemoryStorage<gorage::Pointer<gorage::Storage<std::string>>>>(),
        [path](const std::string &value) {
          return gorage::Pointer<gorage::Storage<gorage::File<gorage::Key>>>(
              std::make_shared<gorage::Storage<gorage::File<gorage::Key>>>(path / "value_to_keys" / value));
        },
        [path](const gorage::Key &key) {
          return gorage::Pointer<gorage::Storage<gorage::File<std::string>>>(
              std::make_shared<gorage::Storage<gorage::File<std::string>>>(path / "key_to_values" / key.value));
        });
  storage.indexes(indexes);
}
template <class T> void indexes(gorage::Storage<T> &storage) {
  if (!std::is_base_of<gorage::Json, T>::value)
    throw exceptions::NotImplemented("Automatic field indexes for non-JSON objects is not implemented");

  std::map<std::string, typename gorage::Storage<T>::Index::Extractor::V> fields;
  for (const auto &field : Json::cast<Json::Dict>(T().structure()))
    fields[field.first] = field.first;

  indexes<T>(storage, fields);
}
} // namespace gorage
