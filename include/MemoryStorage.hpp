#pragma once

#include <map>

#include "Key.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"

namespace gorage {
template <class T> class MemoryStorage : public Storage<T> {
public:
  MemoryStorage() {}

  virtual T load(const Key &key) const {
    if (!exists(key))
      throw exceptions::KeyError(key);
    return _storage.at(key.value);
  }
  virtual bool exists(const Key &key) const { return _storage.count(key.value); }

  virtual Bytes raw(const Key &key) const { throw exceptions::NotImplemented("MemoryStorage", "raw"); }

  virtual std::set<Key> keys() const {
    std::set<Key> result;
    for (const auto &pair : _storage)
      result.insert(Key(pair.first));
    return result;
  }

  MemoryStorage(const Json::Structure &structure) {
    for (const auto &[key, value] : Json::cast<Json::Dict>(structure.value()))
      _storage.emplace(key, Json::cast<T>(value));
  }
  virtual std::any structure() const { return _storage; }

protected:
  virtual void _remove(const Key &key) {
    if (!exists(key))
      throw exceptions::KeyError(key);
    _storage.erase(key.value);
  }
  virtual void _save(const Key &key, const T &object) {
    _storage.erase(key.value);
    _storage.emplace(key.value, object);
  }

private:
  std::map<std::string, T> _storage;
};
} // namespace gorage
