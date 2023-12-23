#pragma once

#include <set>

#include "Bytes.hpp"
#include "Index.hpp"
#include "Json.hpp"
#include "Key.hpp"
#include "exceptions.hpp"
#include "random.hpp"

namespace gorage {
template <class T, class P = T> class Storage {
public:
  void save(const Key &id, const T &object) {
    for (auto &i : _indexes)
      i.second.save(id, object);
    _save(id, object);
  }
  Key save(const T &object) {
    const auto result = id();
    save(result, object);
    return result;
  }
  virtual T load(const Key &id) const = 0;
  virtual P load(const Key &id, size_t &part_number) const {
    if (part_number == 0)
      return load(id);
    else
      throw exceptions::NoSuchPart(id, part_number);
  }
  virtual T build(const std::vector<P> &parts) {
    if (parts.size() == 1)
      return parts[0];
    else
      throw exceptions::CanNotBuild(parts.size());
  }
  virtual Bytes raw(const Key &id) const = 0;
  virtual bool exists(const Key &id) const = 0;

  Key id() const {
    while (true) {
      const auto result = Key();
      if (!exists(result))
        return result;
    }
  }
  T load(const Key &id, const T &default_) const {
    try {
      return load(id);
    } catch (const exceptions::Base &e) {
      throw;
    } catch (const std::exception &e) {
      return default_;
    }
  }
  virtual void remove(const Key &id) {
    for (auto &i : _indexes)
      i.second.remove(id);
    _remove(id);
  }

  void clear() {
    for (const Key &id : keys())
      remove(id);
  }

  virtual std::set<Key> keys() const = 0;

  void index(const std::string &name,
             const typename Index<T>::Extractor &extractor) {
    if (_indexes.count(name))
      return;
    _indexes[name] = Index(extractor);
    auto &result = _indexes[name];
    for (const Key &id : keys())
      result.save(id, load(id));
  }
  const std::set<Key> &keys(const std::string &index_name,
                            const std::string &indexed_value) const {
    if (!_indexes.count(index_name))
      throw exceptions::KeyError(Key(index_name));
    return _indexes[index_name].load(indexed_value);
  }
  std::set<Key>
  keys(const std::map<std::string, std::string> &indexed_values) const {
    std::set<Key> result;
    for (const auto &pair : indexed_values) {
      const auto &new_keys = load(pair.first, pair.second);
      if (!result.size())
        result = new_keys;
      else
        for (auto first = result.begin(), last = result.end(); first != last;) {
          if (!new_keys.count(*first))
            first = result.erase(first);
          else
            ++first;
        }
      if (!result.size())
        break;
    }
    return result;
  }
  std::vector<T> load(const std::set<Key> &keys) const {
    std::vector<T> result;
    for (const auto &u : keys)
      result.insert(load(u));
    return result;
  }

protected:
  virtual void _save(const Key &id, const T &object) = 0;
  virtual void _remove(const Key &id) = 0;

private:
  std::map<std::string, Index<T>> _indexes;
};
} // namespace gorage
