#pragma once

#ifndef GORAGE__RANDOMNAME
#define GORAGE__RANDOMNAME

#include <random>
#include <string>

#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include <Windows.h>
#endif

#include "Bytes.hpp"

namespace gorage {
namespace random {
static std::mt19937 generator;

unsigned int seed();
void shuffle();

class Name {
public:
  static const std::string alphabet;

  explicit Name(const size_t &length) {
    _value.reserve(length);
    for (size_t i = 0; i < length; i++)
      _value += alphabet[generator() % (alphabet.length() - 1)];
  }
  std::string value() const { return _value; }

private:
  std::string _value;
};
gorage::Bytes bytes(const size_t &amount);
} // namespace random
} // namespace gorage
#endif
