#include "Json.hpp"

namespace gorage {
template <class T> class Set : gorage::Json {
public:
  std::set<T> value;

  Set() {}
  Set(std::set<T> value) : value(value) {}
  Set(const gorage::Json::Structure &structure) {
    for (const auto &[key, value] : Json::cast<Set>(structure.value()))
      value.insert(Json::cast<T>(value));
  }
  virtual std::any structure() const { return value; }
};
} // namespace gorage
