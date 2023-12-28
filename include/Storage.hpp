#pragma once

#include <any>
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
  void save(const Key &key, const T &object) {
    for (auto &i : _indexes) {
      i.second.remove(key);
      i.second.save(key, object);
    }
    _save(key, object);
  }
  Key save(const T &object) {
    const auto result = key();
    save(result, object);
    return result;
  }
  virtual T load(const Key &key) const = 0;
  virtual P load(const Key &key, size_t &part_number) const {
    if (part_number == 0)
      return load(key);
    else
      throw exceptions::NoSuchPart(key, part_number);
  }
  virtual T build(const std::vector<P> &parts) {
    if (parts.size() == 1)
      return parts[0];
    else
      throw exceptions::CanNotBuild(parts.size());
  }
  virtual Bytes raw(const Key &key) const = 0;
  virtual bool exists(const Key &key) const = 0;

  Key key() const {
    while (true) {
      const auto result = Key();
      if (!exists(result))
        return result;
    }
  }
  virtual void remove(const Key &key) {
    for (auto &i : _indexes)
      i.second.remove(key);
    _remove(key);
  }

  void clear() {
    for (const Key &key : keys())
      remove(key);
  }

  virtual std::set<Key> keys() const = 0;

  void index(const std::string &name,
             const typename Index<T>::Extractor &extractor) {
    if (_indexes.count(name))
      return;
    _indexes[name] = Index<T>(extractor);
    auto &result = _indexes[name];
    for (const auto &key : keys())
      result.save(key, load(key));
  }
  void indexes(
      const std::map<std::string, typename Index<T>::Extractor> &extractors) {
    for (const auto &n_e : extractors) {
      if (_indexes.count(n_e.first))
        continue;
      _indexes[n_e.first] = Index<T>(n_e.second);
    }
    for (const auto &key : keys())
      for (const auto &n_e : extractors)
        _indexes[n_e.first].save(key, load(key));
  }
  void index(const std::string &name,
             const typename Index<T>::Extractor::F &f) {
    index(name, typename Index<T>::Extractor(f));
  }
  void
  indexes(const std::map<std::string, typename Index<T>::Extractor::F> &fs) {
    std::map<std::string, typename Index<T>::Extractor> extractors;
    for (const auto &n_f : fs)
      extractors[n_f.first] = n_f.second;
    indexes(extractors);
  }
  void indexes(const std::set<std::string> &names) {
    std::map<std::string, typename Index<T>::Extractor> extractors;
    for (const auto &n : names)
      extractors[n] = typename Index<T>::Extractor(n);
    indexes(extractors);
  }
  void index(const std::string &name) {
    index(name, typename Index<T>::Extractor(name));
  }
  const std::set<Key> &keys(const std::string &index_name,
                            const std::any &indexed_value) const {
    if (!_indexes.count(index_name))
      throw exceptions::KeyError(Key(index_name));
    return _indexes.at(index_name).load(gorage::Json::encode(indexed_value));
  }
  std::set<Key>
  keys(const std::map<std::string, std::any> &indexed_values) const {
    std::set<Key> result;
    for (const auto &pair : indexed_values) {
      const auto &new_keys = load(pair.first, pair.second);
      if (!result.size())
        result = new_keys;
      else
        for (auto first = result.begin(), last = result.end(); first != last;)
          if (!new_keys.count(*first))
            first = result.erase(first);
          else
            ++first;
      if (!result.size())
        break;
    }
    return result;
  }
  std::map<std::string, T> load(const std::set<Key> &keys) const {
    std::map<std::string, T> result;
    for (const auto &u : keys)
      result[u] = load(u);
    return result;
  }

protected:
  virtual void _save(const Key &key, const T &object) = 0;
  virtual void _remove(const Key &key) = 0;

private:
  std::map<std::string, Index<T>> _indexes;
};
} // namespace gorage
