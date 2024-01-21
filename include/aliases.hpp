#include "MemoryStorage.hpp"
#include "Storage.hpp"

namespace gorage {
template <class T, class S = gorage::MemoryStorage<T>>
void indexes(gorage::Storage<T> &storage,
             const std::map<std::string, typename gorage::Storage<T>::Index::Extractor::V> &functions) {
  if constexpr (std::is_same_v<S, gorage::MemoryStorage<T>>) {
    auto indexes = std::map<std::string, typename gorage::Storage<T>::Index>();
    for (const auto &n_f : functions)
      indexes[n_f.first] = typename gorage::Storage<T>::Index(
          n_f.second, std::make_shared<gorage::MemoryStorage<gorage::Pointer<gorage::Storage<gorage::Key>>>>(),
          std::make_shared<gorage::MemoryStorage<gorage::Pointer<gorage::Storage<std::string>>>>(),
          [](const std::string &value) {
            return gorage::Pointer<gorage::Storage<gorage::Key>>(
                std::make_shared<gorage::MemoryStorage<gorage::Key>>());
          },
          [](const gorage::Key &key) {
            return gorage::Pointer<gorage::Storage<std::string>>(
                std::make_shared<gorage::MemoryStorage<std::string>>());
          });
    storage.indexes(indexes);
  }
}
template <class T, class S = gorage::MemoryStorage<T>> void indexes(gorage::Storage<T> &storage) {
  if (!std::is_base_of<gorage::Json, T>::value)
    throw exceptions::NotImplemented("Automatic field indexes for non-JSON objects is not implemented");

  std::map<std::string, typename gorage::Storage<T>::Index::Extractor::V> fields;
  for (const auto &field : Json::cast<Json::Dict>(T().structure()))
    fields[field.first] = field.first;

  indexes<T, S>(storage, fields);
}
} // namespace gorage
