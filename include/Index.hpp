#pragma once

#include <map>
#include <set>

#include "Usi.hpp"

namespace gorage {
template <class T> class Index {
public:
  using Extractor = std::function<std::string(const T &)>;
  Extractor extractor;

  Index(const Extractor &extractor) : extractor(extractor) {}

  void save(const Usi &usi, const T &object) { _save(usi, extractor(object)); }
  void remove(const Usi &usi) {
    if (!_usi_to_values.count(usi))
      return;
    for (const auto &value : _usi_to_values[usi])
      _value_to_usis[value].erase(usi);
    _usi_to_values.erase(usi);
  }
  const std::set<Usi> &load(const std::string &value) const {
    static const std::set<Usi> empty;
    if (!_value_to_usis.count(value))
      return empty;
    return _value_to_usis.at(value);
  }

private:
  std::map<std::string, std::set<Usi>> _value_to_usis;
  std::map<Usi, std::set<std::string>> _usi_to_values;
  void _save(const Usi &usi, const std::string &value) {
    remove(usi);
    if (!_value_to_usis.count(value))
      _value_to_usis[value] = {};
    _value_to_usis[value].insert(usi);
    if (!_usi_to_values.count(usi))
      _usi_to_values[usi] = {};
    _usi_to_values[usi].insert(value);
  }
};
} // namespace gorage
