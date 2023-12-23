#pragma once

#include <string>

#include "Id.hpp"

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
  NotImplemented(const std::string &class_, const std::string &method)
      : Base("Method `" + method + "` not implemented for class `" + class_ +
             "`") {}
};
class NoSuchPart : public Base {
public:
  explicit NoSuchPart(const Id &id, const size_t &number)
      : Base("Object with USI " + id.value + "have no part of number " +
             std::to_string(number)) {}
};
class CanNotBuild : public Base {
public:
  explicit CanNotBuild(const size_t &number)
      : Base("Can not build object from given " + std::to_string(number) +
             " parts"){};
};
class KeyError : public Base {
public:
  KeyError(const gorage::Id &id)
      : Base("No object with id `" + id.value + "`") {}
};
} // namespace exceptions
} // namespace gorage
