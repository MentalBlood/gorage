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

  void save(const Key &id, const T &object) { _save(id, extractor(object)); }
  void remove(const Key &id) {
    if (!_id_to_values.count(id))
      return;
    for (const auto &value : _id_to_values[id])
      _value_to_keys[value].erase(id);
    _id_to_values.erase(id);
  }
  const std::set<Key> &load(const std::string &value) const {
    static const std::set<Key> empty;
    if (!_value_to_keys.count(value))
      return empty;
    return _value_to_keys.at(value);
  }

private:
  std::map<std::string, std::set<Key>> _value_to_keys;
  std::map<Key, std::set<std::string>> _id_to_values;
  void _save(const Key &id, const std::string &value) {
    remove(id);
    if (!_value_to_keys.count(value))
      _value_to_keys[value] = {};
    _value_to_keys[value].insert(id);
    if (!_id_to_values.count(id))
      _id_to_values[id] = {};
    _id_to_values[id].insert(value);
  }
};
} // namespace gorage
