#pragma once

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <optional>

#include "Bytes.hpp"
#include "Json.hpp"
#include "Storage.hpp"
#include "Key.hpp"

namespace gorage {
template <class T> class Item : public Json {
public:
  Bytes data;
  T metadata;

  Item() {}
  explicit Item(const Bytes &data, const T &metadata)
      : data(data), metadata(metadata) {}
  explicit Item(const std::any &structure)
      : Item(get<String>(cast<Dict>(structure), "data").decoded(),
             get_object<T>(cast<Dict>(structure), "metadata")) {}
  std::any structure() const {
    return Dict({{"data", data}, {"metadata", metadata.structure()}});
  }
};

template <class T> class Query {
public:
  const std::map<std::string, std::any> keys;

  Query(const std::map<std::string, std::any> &keys) : keys(keys) {}
  Query(const std::string &key, const std::any &structure)
      : keys({{key, structure}}) {}

  bool match(const T &metadata) const {
    for (const auto &q : keys)
      if (!metadata.contains(q.first, q.second))
        return false;
    return true;
  }
  std::optional<std::pair<Key, T>>
  execute(const std::shared_ptr<Storage<T>> storage,
          const std::optional<std::function<bool(const std::pair<Key, T> &,
                                                 const std::pair<Key, T> &)>>
              &first = {}) const {
    std::optional<std::shared_ptr<std::pair<Key, T>>> result;

    for (const auto &key : storage->keys()) {
      const auto item = storage->load(key);
      if (match(item)) {
        const auto current = std::make_shared<std::pair<Key, T>>(key, item);

        if (!first.has_value())
          return *current;

        if (result.has_value()) {
          if (first.value()(*current, *result.value()))
            result = current;
        } else
          result = current;
      }
    }
    if (result.has_value())
      return *result.value();
    else
      return {};
  }
};
} // namespace gorage
