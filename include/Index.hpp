#pragma once

#include <map>
#include <set>

#include "Json.hpp"
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

    Extractor(const F &f) : f(f) {}
    Extractor(const std::string &field_name) {
      if (std::is_same_v<T, gorage::Json>)
        f = [field_name](const T &object) {
          const auto json = object.encoded();
          static const auto regex = std::regex(std::regex_replace(
              "\"" + field_name + "\" *: (*)" + "(?:\\n| )*(?:,|})",
              std::regex("\\[|\\]|\\{|\\}|\\/|\\+|\\.|\\\""), "\\$&"));
          std::smatch matches;
          if (std::regex_search(json, matches, regex))
            if (matches.size() >= 2)
              return matches[1];
          throw exceptions::NoSuchField(field_name);
        };
      else
        throw exceptions::NotImplemented(
            "Index", "Extractor for fields of non-JSON objects");
    };
    std::string operator()(const T &object) { return f(object); }
  };
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
