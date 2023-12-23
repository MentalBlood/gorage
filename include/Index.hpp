#pragma once

#include <map>
#include <set>

#include "Key.hpp"

namespace gorage {
template <class T> class Index {
public:
  using Extractor = std::function<std::string(const T &)>;
  Extractor extractor;

  Index(const Extractor &extractor) : extractor(extractor) {}

  void save(const Key &key, const T &object) { _save(key, extractor(object)); }
  void remove(const Key &key) {
    if (!_key_to_values.count(key))
      return;
    for (const auto &value : _key_to_values[key])
      _value_to_keys[value].erase(key);
    _key_to_values.erase(key);
  }
  const std::set<Key> &load(const std::string &value) const {
    static const std::set<Key> empty;
    if (!_value_to_keys.count(value))
      return empty;
    return _value_to_keys.at(value);
  }

private:
  std::map<std::string, std::set<Key>> _value_to_keys;
  std::map<Key, std::set<std::string>> _key_to_values;
  void _save(const Key &key, const std::string &value) {
    remove(key);
    if (!_value_to_keys.count(value))
      _value_to_keys[value] = {};
    _value_to_keys[value].insert(key);
    if (!_key_to_values.count(key))
      _key_to_values[key] = {};
    _key_to_values[key].insert(value);
  }
};
} // namespace gorage
