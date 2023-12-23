#pragma once

#include <map>

#include "Storage.hpp"
#include "Key.hpp"
#include "exceptions.hpp"

namespace gorage {
template <class T> class MemoryStorage : public Storage<T> {
public:
  MemoryStorage() {}

  virtual T load(const Key &id) const {
    if (!exists(id))
      throw exceptions::KeyError(id);
    return _storage.at(id.value);
  }
  virtual bool exists(const Key &id) const {
    return _storage.count(id.value);
  }

  virtual Bytes raw(const Key &id) const {
    throw exceptions::NotImplemented("MemoryStorage", "raw");
  }

  virtual std::set<Key> keys() const {
    std::set<Key> result;
    for (const auto &pair : _storage)
      result.insert(Key(pair.first));
    return result;
  }

protected:
  virtual void _remove(const Key &id) {
    if (!exists(id))
      throw exceptions::KeyError(id);
    _storage.erase(id.value);
  }
  virtual void _save(const Key &id, const T &object) {
    _storage.erase(id.value);
    _storage.emplace(id.value, object);
  }

private:
  std::map<std::string, T> _storage;
};
} // namespace gorage
