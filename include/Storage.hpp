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
  void save(const Usi &usi, const T &object) {
    for (auto &i : _indexes)
      i.second.save(usi, object);
    _save(usi, object);
  }
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
  virtual void remove(const Usi &usi) {
    for (auto &i : _indexes)
      i.second.remove(usi);
    _remove(usi);
  }

  void clear() {
    for (const Usi &usi : usis())
      remove(usi);
  }

  virtual std::set<Usi> usis() const = 0;

  void index(const std::string &name,
             const typename Index<T>::Extractor &extractor) {
    if (_indexes.count(name))
      return;
    auto result = Index(extractor);
    for (const Usi &usi : usis())
      result.save(usi, load(usi));
    _indexes[name] = result;
  }
  const std::set<Usi> &usis(const std::string &index_name,
                            const std::string &indexed_value) const {
    if (!_indexes.count(index_name))
      throw exceptions::KeyError(Usi(index_name));
    return _indexes[index_name].load(indexed_value);
  }
  std::set<Usi>
  usis(const std::map<std::string, std::string> &indexed_values) const {
    std::set<Usi> result;
    for (const auto &pair : indexed_values) {
      const auto &new_usis = load(pair.first, pair.second);
      if (!result.size())
        result = new_usis;
      else
        for (const auto &u : result)
          if (!new_usis.count(u)) {
            if (result.size() == 1)
              return {};
            result.erase(u);
          }
    }
    return result;
  }
  std::vector<T> load(const std::set<Usi> &usis) const {
    std::set<T> result;
    for (const auto &u : usis)
      result.insert(load(u));
    return result;
  }

protected:
  virtual void _save(const Usi &usi, const T &object) = 0;
  virtual void _remove(const Usi &usi) = 0;

private:
  std::map<std::string, Index<T>> _indexes;
};
} // namespace gorage
