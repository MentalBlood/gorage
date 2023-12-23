#pragma once

#include <map>

#include "Storage.hpp"
#include "Usi.hpp"
#include "exceptions.hpp"

namespace gorage {
template <class T> class MemoryStorage : public Storage<T> {
public:
  MemoryStorage() {}

  virtual T load(const Usi &usi) const {
    if (!exists(usi))
      throw exceptions::KeyError(usi);
    return _storage.at(usi.value);
  }
  virtual bool exists(const Usi &usi) const {
    return _storage.count(usi.value);
  }

  virtual Bytes raw(const Usi &usi) const {
    throw exceptions::NotImplemented("MemoryStorage", "raw");
  }

  virtual std::set<Usi> usis() const {
    std::set<Usi> result;
    for (const auto &pair : _storage)
      result.insert(Usi(pair.first));
    return result;
  }

protected:
  virtual void _remove(const Usi &usi) {
    if (!exists(usi))
      throw exceptions::KeyError(usi);
    _storage.erase(usi.value);
  }
  virtual void _save(const Usi &usi, const T &object) {
    _storage.erase(usi.value);
    _storage.emplace(usi.value, object);
  }

private:
  std::map<std::string, T> _storage;
};
} // namespace gorage
