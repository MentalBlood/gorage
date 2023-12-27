#pragma once

#include <any>

#include "Bytes.hpp"
#include "Json.hpp"
#include "Key.hpp"
#include "Storage.hpp"

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
} // namespace gorage
