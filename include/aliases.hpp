#include "File.hpp"
#include "MemoryStorage.hpp"
#include "Storage.hpp"
#include <filesystem>

namespace gorage {
using Keys = Pointer<Storage<Key>>;
using Values = Pointer<Storage<std::string>>;
template <class T> using Fields = std::map<std::string, typename gorage::Storage<T>::Index::Extractor::V>;

template <class T> void indexes(gorage::Storage<T> &storage, const Fields<T> &fields) {
  auto indexes = std::map<std::string, typename gorage::Storage<T>::Index>();
  for (const auto &n_f : fields)
    indexes[n_f.first] = typename gorage::Storage<T>::Index(
        n_f.second, std::make_shared<gorage::MemoryStorage<Keys>>(), std::make_shared<gorage::MemoryStorage<Values>>(),
        [](const std::string &value) { return Keys(std::make_shared<gorage::MemoryStorage<gorage::Key>>()); },
        [](const gorage::Key &key) { return Values(std::make_shared<gorage::MemoryStorage<std::string>>()); });
  storage.indexes(indexes);
}

template <class T> void indexes(gorage::Storage<T> &storage, Fields<T> &fields, const std::filesystem::path &path) {
  auto indexes = std::map<std::string, typename gorage::Storage<T>::Index>();
  for (const auto &n_f : fields)
    indexes[n_f.first] = typename gorage::Storage<T>::Index(
        n_f.second, std::make_shared<gorage::MemoryStorage<Keys>>(), std::make_shared<gorage::MemoryStorage<Values>>(),
        [path](const std::string &value) {
          return gorage::Pointer<gorage::Storage<gorage::File<gorage::Key>>>(
              std::make_shared<gorage::Storage<gorage::File<gorage::Key>>>(path / "value_to_keys" / value, "txt"));
        },
        [path](const gorage::Key &key) {
          return gorage::Pointer<gorage::Storage<gorage::File<std::string>>>(
              std::make_shared<gorage::Storage<gorage::File<std::string>>>(path / "key_to_values" / key.value, "txt"));
        });
  storage.indexes(indexes);
}

template <class T> void indexes(gorage::Storage<T> &storage) {
  if (!std::is_base_of<gorage::Json, T>::value)
    throw exceptions::NotImplemented("Automatic field indexes for non-JSON objects is not implemented");

  auto fields = Fields<T>();
  for (const auto &field : Json::cast<Json::Dict>(T().structure()))
    fields[field.first] = field.first;

  indexes<T>(storage, fields);
}
} // namespace gorage
