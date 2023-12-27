#pragma once

#include <map>
#include <set>

#include "Key.hpp"
#include "exceptions.hpp"

namespace gorage {
namespace exceptions {
class NoSuchField : public exceptions::Base {
public:
  NoSuchField(const std::string &field_name) : Base(field_name) {}
};
} // namespace exceptions
template <class T> class Index {
public:
  class Extractor {
  public:
    using F = std::function<std::string(const T &)>;
    F f;

    Extractor() {}
    Extractor(const F &f)
        : f([f](const T &object) { return gorage::Json::encode(f(object)); }) {}
    Extractor(const std::string &field_name)
        : f([field_name](const T &object) {
            return Json::encode(
                Json::cast<Json::Dict>(object.structure()).at(field_name));
          }){};
    std::string operator()(const T &object) const { return f(object); }
  };
  Extractor extractor;

  Index() {}
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
