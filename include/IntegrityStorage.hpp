#pragma once

#include <memory>

#include "File.hpp"
#include "Storage.hpp"
#include "Usi.hpp"
#include "exceptions.hpp"

namespace gorage {
namespace exceptions {
class IntegrityError : public Base {
public:
  IntegrityError(const gorage::Usi &usi)
      : Base("Can not load object with usi `" + usi.value +
             "`: integrity check failed") {}
};
} // namespace exceptions
template <class T, class I> class IntegrityStorage : public Storage<T> {
public:
  explicit IntegrityStorage(std::shared_ptr<Storage<T>> base,
                            std::shared_ptr<Storage<I>> integrity)
      : _base(base), _integrity(integrity) {}

  T load(const Usi &usi) const {
    const T result = _base->load(usi);
    if (digest(result) != _integrity->load(digest_usi(usi)))
      throw exceptions::IntegrityError(usi);
    return result;
  }
  Bytes raw(const Usi &usi) const {
    const Bytes result = _base->raw(usi);
    if (digest_raw(result) != _integrity->load(digest_usi(usi)))
      throw exceptions::IntegrityError(usi);
    return result;
  }
  bool check(const Usi &usi) const {
    try {
      return digest_raw(_base->raw(usi)) == _integrity->load(digest_usi(usi));
    } catch (const exceptions::CanNotReadFile &e) {
      return false;
    }
  }
  void recalculate(const Usi &usi) const {
    _integrity->save(digest_usi(usi), digest(_base->load(usi)));
  }

  virtual bool exists(const Usi &usi) const {
    return _base->exists(usi) && _integrity->exists(digest_usi(usi));
  }

  virtual std::set<Usi> usis() const {
    const auto result = _base->usis();
    return std::set<Usi>(result.begin(), result.end());
  }

protected:
  virtual I digest(const T &content) const = 0;
  virtual I digest_raw(const Bytes &content) const = 0;
  void _save(const Usi &usi, const T &content) {
    _integrity->save(digest_usi(usi), digest(content));
    _base->save(usi, content);
  }
  void _remove(const Usi &usi) {
    _base->remove(usi);
    _integrity->remove(digest_usi(usi));
  }

  virtual Usi digest_usi(const Usi &usi) const {
    if constexpr (std::is_same_v<T, I>)
      if (_base == _integrity)
        return Usi(usi.value + "_digest");
    return usi;
  }

private:
  std::shared_ptr<Storage<T>> _base;
  std::shared_ptr<Storage<I>> _integrity;
};
} // namespace gorage
