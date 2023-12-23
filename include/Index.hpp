#pragma once

#include <map>
#include <set>

#include "Id.hpp"

namespace gorage {
template <class T> class Index {
public:
  using Extractor = std::function<std::string(const T &)>;
  Extractor extractor;

  Index(const Extractor &extractor) : extractor(extractor) {}

  void save(const Id &id, const T &object) { _save(id, extractor(object)); }
  void remove(const Id &id) {
    if (!_id_to_values.count(id))
      return;
    for (const auto &value : _id_to_values[id])
      _value_to_ids[value].erase(id);
    _id_to_values.erase(id);
  }
  const std::set<Id> &load(const std::string &value) const {
    static const std::set<Id> empty;
    if (!_value_to_ids.count(value))
      return empty;
    return _value_to_ids.at(value);
  }

private:
  std::map<std::string, std::set<Id>> _value_to_ids;
  std::map<Id, std::set<std::string>> _id_to_values;
  void _save(const Id &id, const std::string &value) {
    remove(id);
    if (!_value_to_ids.count(value))
      _value_to_ids[value] = {};
    _value_to_ids[value].insert(id);
    if (!_id_to_values.count(id))
      _id_to_values[id] = {};
    _id_to_values[id].insert(value);
  }
};
} // namespace gorage
