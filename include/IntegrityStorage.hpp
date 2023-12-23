#pragma once

#include <memory>

#include "File.hpp"
#include "Storage.hpp"
#include "Id.hpp"
#include "exceptions.hpp"

namespace gorage {
namespace exceptions {
class IntegrityError : public Base {
public:
  IntegrityError(const gorage::Id &id)
      : Base("Can not load object with id `" + id.value +
             "`: integrity check failed") {}
};
} // namespace exceptions
template <class T, class I> class IntegrityStorage : public Storage<T> {
public:
  explicit IntegrityStorage(std::shared_ptr<Storage<T>> base,
                            std::shared_ptr<Storage<I>> integrity)
      : _base(base), _integrity(integrity) {}

  T load(const Id &id) const {
    const T result = _base->load(id);
    if (digest(result) != _integrity->load(digest_id(id)))
      throw exceptions::IntegrityError(id);
    return result;
  }
  Bytes raw(const Id &id) const {
    const Bytes result = _base->raw(id);
    if (digest_raw(result) != _integrity->load(digest_id(id)))
      throw exceptions::IntegrityError(id);
    return result;
  }
  bool check(const Id &id) const {
    try {
      return digest_raw(_base->raw(id)) == _integrity->load(digest_id(id));
    } catch (const exceptions::CanNotReadFile &e) {
      return false;
    }
  }
  void recalculate(const Id &id) const {
    _integrity->save(digest_id(id), digest(_base->load(id)));
  }

  virtual bool exists(const Id &id) const {
    return _base->exists(id) && _integrity->exists(digest_id(id));
  }

  virtual std::set<Id> ids() const {
    const auto result = _base->ids();
    return std::set<Id>(result.begin(), result.end());
  }

protected:
  virtual I digest(const T &content) const = 0;
  virtual I digest_raw(const Bytes &content) const = 0;
  void _save(const Id &id, const T &content) {
    _integrity->save(digest_id(id), digest(content));
    _base->save(id, content);
  }
  void _remove(const Id &id) {
    _base->remove(id);
    _integrity->remove(digest_id(id));
  }

  virtual Id digest_id(const Id &id) const {
    if constexpr (std::is_same_v<T, I>)
      if (_base == _integrity)
        return Id(id.value + "_digest");
    return id;
  }

private:
  std::shared_ptr<Storage<T>> _base;
  std::shared_ptr<Storage<I>> _integrity;
};
} // namespace gorage
