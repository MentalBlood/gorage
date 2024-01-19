#include "Json.hpp"

namespace gorage {
template <class T> class Pointer : gorage::Json {
public:
  std::shared_ptr<T> value;

  Pointer() {}
  Pointer(const std::shared_ptr<T> &value) : value(value) {}
  Pointer(const gorage::Json::Structure &structure) : value(std::make_shared<T>(structure)) {}
  virtual std::any structure() const { return value->structure(); }
};
} // namespace gorage
