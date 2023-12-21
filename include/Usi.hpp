#pragma once

#include <regex>

#include "Bytes.hpp"
#include "Json.hpp"
#include "random.hpp"

#include "../modules/cppcodec/base32_rfc4648.hpp"

namespace gorage {
class Usi : public Json {
public:
  std::string value;

  Usi() : Usi(32) {}
  Usi(const size_t &length) : Usi(random::Name(length).value()) {}

  explicit Usi(const std::string &value) : value(value) {}
  explicit Usi(const gorage::Bytes &source)
      : Usi(std::regex_replace(cppcodec::base32_rfc4648::encode(source),
                               std::regex("="), "")) {}

  Bytes decoded() const { return cppcodec::base32_rfc4648::decode(value); }

  explicit Usi(const Json::Structure &structure)
      : value(cast<String>(structure.value()).s) {}
  std::any structure() const { return value; }
};
} // namespace gorage
