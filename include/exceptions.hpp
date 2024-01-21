#pragma once

#include <string>

#include "Key.hpp"

namespace gorage {
namespace exceptions {
class Base : public std::exception {
public:
  std::string message;

  Base() {}
  explicit Base(const std::string &message) : message(message) {}

  const char *what() const noexcept override { return message.c_str(); }
};
class NotImplemented : public Base {
public:
  NotImplemented(const std::string &message) : Base(message) {}
  NotImplemented(const std::string &class_, const std::string &method)
      : NotImplemented("Method `" + method + "` not implemented for class `" + class_ + "`") {}
};
class NoSuchPart : public Base {
public:
  explicit NoSuchPart(const Key &key, const size_t &number)
      : Base("Object with USI " + key.value + "have no part of number " + std::to_string(number)) {}
};
class CanNotBuild : public Base {
public:
  explicit CanNotBuild(const size_t &number)
      : Base("Can not build object from given " + std::to_string(number) + " parts"){};
};
class KeyError : public Base {
public:
  KeyError(const gorage::Key &key) : Base("No object with key `" + key.value + "`") {}
};
} // namespace exceptions
} // namespace gorage
