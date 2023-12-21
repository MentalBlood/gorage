#pragma once

#include <map>
#include <set>

#include "Bytes.hpp"
#include "Usi.hpp"

namespace gorage {
class Index {
public:
  void save(const Usi &usi, const std::string &value) {
    remove(usi);
    if (!_value_to_usis.count(value))
      _value_to_usis[value] = {};
    _value_to_usis[value].insert(usi);
    if (!_usi_to_values.count(usi))
      _usi_to_values[usi] = {};
    _usi_to_values[usi].insert(value);
  }
  void remove(const Usi &usi) {
    if (!_usi_to_values.count(usi))
      return;
    for (const auto &value : _usi_to_values[usi])
      _value_to_usis[value].erase(usi);
    _usi_to_values.erase(usi);
  }

private:
  std::map<std::string, std::set<Usi>> _value_to_usis;
  std::map<Usi, std::set<std::string>> _usi_to_values;
};
} // namespace gorage
