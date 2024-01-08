#pragma once

#include <memory>

#include "File.hpp"
#include "Key.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"

namespace gorage {
namespace exceptions {
class IntegrityError : public Base {
public:
  IntegrityError(const gorage::Key &key)
      : Base("Can not load object with key `" + key.value +
             "`: integrity check failed") {}
};
} // namespace exceptions
template <class T, class I> class IntegrityStorage : public Storage<T> {
public:
  explicit IntegrityStorage(std::shared_ptr<Storage<T>> base,
                            std::shared_ptr<Storage<I>> integrity)
      : _base(base), _integrity(integrity) {}

  T load(const Key &key) const {
    const T result = _base->load(key);
    if (digest(result) != _integrity->load(digest_key(key)))
      throw exceptions::IntegrityError(key);
    return result;
  }
  Bytes raw(const Key &key) const {
    const Bytes result = _base->raw(key);
    if (digest_raw(result) != _integrity->load(digest_key(key)))
      throw exceptions::IntegrityError(key);
    return result;
  }
  bool check(const Key &key) const {
    try {
      return digest_raw(_base->raw(key)) == _integrity->load(digest_key(key));
    } catch (const exceptions::CanNotReadFile &e) {
      return false;
    }
  }
  void recalculate(const Key &key) const {
    _integrity->save(digest_key(key), digest(_base->load(key)));
  }

  virtual bool exists(const Key &key) const {
    return _base->exists(key) && _integrity->exists(digest_key(key));
  }

  virtual std::set<Key> keys() const {
    const auto result = _base->keys();
    return std::set<Key>(result.begin(), result.end());
  }

  IntegrityStorage(const Json::Structure &structure) {
    throw exceptions::NotImplemented("IntegrityStorage", "from_json");
  }
  virtual std::any structure() const {
    return Json::Dict(
        {{"base", _base->structure()}, {"integrity", _integrity->structure()}});
  }

protected:
  virtual I digest(const T &content) const = 0;
  virtual I digest_raw(const Bytes &content) const = 0;
  void _save(const Key &key, const T &content) {
    _integrity->save(digest_key(key), digest(content));
    _base->save(key, content);
  }
  void _remove(const Key &key) {
    _base->remove(key);
    _integrity->remove(digest_key(key));
  }

  virtual Key digest_key(const Key &key) const {
    if constexpr (std::is_same_v<T, I>)
      if (_base == _integrity)
        return Key(key.value + "_digest");
    return key;
  }

private:
  std::shared_ptr<Storage<T>> _base;
  std::shared_ptr<Storage<I>> _integrity;
};
} // namespace gorage
