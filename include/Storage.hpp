#pragma once

#include <any>
#include <set>

#include "Bytes.hpp"
#include "Json.hpp"
#include "Key.hpp"
#include "Pointer.hpp"
#include "Set.hpp"
#include "exceptions.hpp"
#include "random.hpp"

namespace gorage {
namespace exceptions {
class NoSuchField : public exceptions::Base {
public:
  NoSuchField(const std::string &field_name) : Base(field_name) {}
};
} // namespace exceptions
template <class T, class P = T> class Storage : public Json {
public:
  class Index {
  public:
    class Extractor {
    public:
      using F = std::function<std::any(const T &)>;
      F f;

      Extractor() {}
      Extractor(const F &f) : f(f) {}
      Extractor(const std::string &field_name)
          : f([field_name](const T &object) { return Json::cast<Json::Dict>(object.structure()).at(field_name); }){};
      std::string operator()(const T &object) const { return Json::encode(f(object)); }
    };
    Extractor extractor;

    Index() {}
    Index(const std::string &field_name, const std::shared_ptr<Storage<Pointer<Storage<Key>>>> &value_to_keys,
          const std::shared_ptr<Storage<Pointer<Storage<std::string>>>> &key_to_values,
          const std::function<Pointer<Storage<Key>>(const std::string &value)> &create_keys_storage,
          const std::function<Pointer<Storage<std::string>>(const Key &key)> &create_values_storage)
        : Index(Extractor(field_name), value_to_keys, key_to_values, create_keys_storage, create_values_storage) {}
    Index(const typename Extractor::F &f, const std::shared_ptr<Storage<Pointer<Storage<Key>>>> &value_to_keys,
          const std::shared_ptr<Storage<Pointer<Storage<std::string>>>> &key_to_values,
          const std::function<Pointer<Storage<Key>>(const std::string &value)> &create_keys_storage,
          const std::function<Pointer<Storage<std::string>>(const Key &key)> &create_values_storage)
        : Index(Extractor(f), value_to_keys, key_to_values, create_keys_storage, create_values_storage) {}
    Index(const Extractor &extractor, const std::shared_ptr<Storage<Pointer<Storage<Key>>>> &value_to_keys,
          const std::shared_ptr<Storage<Pointer<Storage<std::string>>>> &key_to_values,
          const std::function<Pointer<Storage<Key>>(const std::string &value)> &create_keys_storage,
          const std::function<Pointer<Storage<std::string>>(const Key &key)> &create_values_storage)
        : extractor(extractor), _value_to_keys(value_to_keys), _key_to_values(key_to_values),
          _create_keys_storage(create_keys_storage), _create_values_storage(create_values_storage) {}

    void save(const Key &key, const T &object) { _save(key, extractor(object)); }
    void remove(const Key &key) {
      if (!_key_to_values->exists(key))
        return;

      auto values_storage = _key_to_values->load(key).value;
      for (const auto &value_key : values_storage->keys())
        _value_to_keys->load(values_storage->load(value_key)).value->remove(key);

      _key_to_values->remove(key);
    }
    const std::set<Key> load(const std::string &value) const {
      static const std::set<Key> empty;
      if (!_value_to_keys->exists(value))
        return empty;

      std::set<Key> result;
      const auto keys_storage = _value_to_keys->load(value).value;
      for (const auto k : keys_storage->keys())
        result.insert(keys_storage->load(k));
      return result;
    }

  private:
    std::shared_ptr<Storage<Pointer<Storage<Key>>>> _value_to_keys;
    std::shared_ptr<Storage<Pointer<Storage<std::string>>>> _key_to_values;
    std::function<Pointer<Storage<Key>>(const std::string &value)> _create_keys_storage;
    std::function<Pointer<Storage<std::string>>(const Key &key)> _create_values_storage;

    void _save(const Key &key, const std::string &value) {
      remove(key);
      {
        const auto exists = _value_to_keys->exists(value);
        auto current = exists ? _value_to_keys->load(value) : _create_keys_storage(value);
        current.value->save(key, key);
        if (!exists)
          _value_to_keys->save(value, current);
        _value_to_keys->load(value).value->load(key);
      }
      {
        const auto exists = _key_to_values->exists(key);
        auto current = exists ? _key_to_values->load(key) : _create_values_storage(key);
        current.value->save(value, value);
        if (!exists)
          _key_to_values->save(key, current);
        _key_to_values->load(key).value->load(value);
      }
    }
  };

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

  void index(const std::string &name, const Index &index) {
    if (_indexes.count(name))
      return;
    _indexes[name] = index;
    auto &result = _indexes[name];
    for (const auto &key : keys())
      result.save(key, load(key));
  }
  void indexes(const std::map<std::string, Index> &indexes) {
    for (const auto &n_i : indexes) {
      if (_indexes.count(n_i.first))
        continue;
      _indexes[n_i.first] = n_i.second;
    }
    for (const auto &key : keys())
      for (const auto &n_i : indexes)
        _indexes[n_i.first].save(key, load(key));
  }
  const std::set<Key> keys(const std::string &index_name, const std::any &indexed_value) const {
    if (!_indexes.count(index_name))
      throw exceptions::KeyError(index_name);
    return _indexes.at(index_name).load(gorage::Json::encode(indexed_value));
  }
  std::set<Key> keys(const std::map<std::string, std::any> &indexed_values) const {
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
  std::shared_ptr<Storage<T>> _base;
  std::map<std::string, Index> _indexes;
};
} // namespace gorage
