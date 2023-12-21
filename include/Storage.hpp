#pragma once

#include <vector>

#include "Bytes.hpp"
#include "Json.hpp"
#include "Usi.hpp"
#include "exceptions.hpp"
#include "random.hpp"

namespace gorage {
namespace exceptions {
class NoSuchPart : public Base {
public:
  explicit NoSuchPart(const Usi &usi, const size_t &number)
      : Base("Object with USI " + usi.value + "have no part of number " +
             std::to_string(number)) {}
};
class CanNotBuild : public Base {
public:
  explicit CanNotBuild(const size_t &number)
      : Base("Can not build object from given " + std::to_string(number) +
             " parts"){};
};
} // namespace exceptions
template <class T, class P = T> class Storage {
public:
  virtual void save(const Usi &usi, const T &object) = 0;
  Usi save(const T &object) {
    const auto usi = this->usi();
    save(usi, object);
    return usi;
  }
  virtual T load(const Usi &usi) const = 0;
  virtual P load(const Usi &usi, size_t &part_number) const {
    if (part_number == 0)
      return load(usi);
    else
      throw exceptions::NoSuchPart(usi, part_number);
  }
  virtual T build(const std::vector<P> &parts) {
    if (parts.size() == 1)
      return parts[0];
    else
      throw exceptions::CanNotBuild(parts.size());
  }
  virtual Bytes raw(const Usi &usi) const = 0;
  virtual bool exists(const Usi &usi) const = 0;

  Usi usi() const {
    while (true) {
      const auto result = Usi();
      if (!exists(result))
        return result;
    }
  }
  T load(const Usi &usi, const T &default_) const {
    try {
      return load(usi);
    } catch (const exceptions::Base &e) {
      throw;
    } catch (const std::exception &e) {
      return default_;
    }
  }
  virtual void remove(const Usi &usi) = 0;

  void clear() {
    for (const Usi &usi : usis())
      remove(usi);
  }

  virtual std::vector<Usi> usis() const = 0;
};
} // namespace gorage
