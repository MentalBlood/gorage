#pragma once

#include <regex>

#include "Bytes.hpp"
#include "Json.hpp"
#include "random.hpp"

#include "../modules/cppcodec/base32_rfc4648.hpp"

namespace gorage {
class Id : public Json {
public:
  std::string value;

  Id() : Id(32) {}
  Id(const size_t &length) : Id(random::Name(length).value()) {}

  explicit Id(const std::string &value) : value(value) {}
  explicit Id(const gorage::Bytes &source)
      : Id(std::regex_replace(cppcodec::base32_rfc4648::encode(source),
                               std::regex("="), "")) {}

  Bytes decoded() const { return cppcodec::base32_rfc4648::decode(value); }

  explicit Id(const Json::Structure &structure)
      : value(cast<String>(structure.value()).s) {}
  std::any structure() const { return value; }
};
} // namespace gorage
