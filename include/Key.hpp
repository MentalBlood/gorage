#pragma once

#include "Bytes.hpp"
#include "Json.hpp"
#include "random.hpp"

namespace gorage {
class Key : public Json {
public:
  std::string value;

  Key() : Key(32) {}
  Key(const size_t &length) : Key(random::Name(length).value()) {}

  Key(const std::string &value) : value(value) {}
  Key(const gorage::Bytes &source) : Key(Json::String(source).hex()) {}

  explicit Key(const Json::Structure &structure) : value(cast<String>(structure.value()).s) {}
  std::any structure() const { return value; }
  bool operator<(const Key &other) const { return value < other.value; }
};
} // namespace gorage
