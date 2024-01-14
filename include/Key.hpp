#pragma once

#include <regex>

#include "Bytes.hpp"
#include "Json.hpp"
#include "random.hpp"

#include "../modules/cppcodec/base32_rfc4648.hpp"

namespace gorage {
class Key : public Json {
public:
  std::string value;

  Key() : Key(32) {}
  Key(const size_t &length) : Key(random::Name(length).value()) {}

  Key(const std::string &value) : value(value) {}
  Key(const gorage::Bytes &source)
      : Key(std::regex_replace(cppcodec::base32_rfc4648::encode(source), std::regex("="), "")) {}

  Bytes decoded() const { return cppcodec::base32_rfc4648::decode(value); }

  explicit Key(const Json::Structure &structure) : value(cast<String>(structure.value()).s) {}
  std::any structure() const { return value; }
  bool operator<(const Key &other) const { return value < other.value; }
};
} // namespace gorage
