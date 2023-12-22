#pragma once

#include <set>

#include "Bytes.hpp"
#include "Index.hpp"
#include "Json.hpp"
#include "Usi.hpp"
#include "exceptions.hpp"
#include "random.hpp"

namespace gorage {
template <class T, class P = T> class Storage {
public:
  virtual void save(const Usi &usi, const T &object) = 0;
  Usi save(const T &object) {
    const auto result = usi();
    save(result, object);
    return result;
  }
  virtual T load(const Usi &usi) const = 0;
  virtual P load(const Usi &usi, size_t &part_number) const {
    if (part_number == 0)
      return load(usi);
    else
      throw exceptions::NoSuchPart(usi, part_number);
  }
  virtual T build(const std::vector<P> &parts) {
    if (parts.size() == 1)
      return parts[0];
    else
      throw exceptions::CanNotBuild(parts.size());
  }
  virtual Bytes raw(const Usi &usi) const = 0;
  virtual bool exists(const Usi &usi) const = 0;

  Usi usi() const {
    while (true) {
      const auto result = Usi();
      if (!exists(result))
        return result;
    }
  }
  T load(const Usi &usi, const T &default_) const {
    try {
      return load(usi);
    } catch (const exceptions::Base &e) {
      throw;
    } catch (const std::exception &e) {
      return default_;
    }
  }
  virtual void remove(const Usi &usi) = 0;

  void clear() {
    for (const Usi &usi : usis())
      remove(usi);
  }

  virtual std::set<Usi> usis() const = 0;

  using Extractor = std::function<std::any(const T &)>;
  void index(const Extractor &get_value) {
    if (_indexes.count(get_value))
      return;
    auto result = Index();
    for (const Usi &usi : usis())
      result.save(usi, Json::encode(get_value(load(usi))));
    _indexes[get_value] = result;
  }

private:
  std::map<Extractor, Index> _indexes;
};
} // namespace gorage
