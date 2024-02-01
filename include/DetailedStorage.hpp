#pragma once

#include <memory>

#include "Key.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"

namespace gorage {
template <class T, class F, class S> class DetailedStorage : public Storage<T> {
public:
  std::shared_ptr<Storage<F>> first;
  std::shared_ptr<Storage<S>> second;
  std::function<F(const T &)> first_extractor;
  std::function<S(const T &)> second_extrator;
  std::function<T(const F &, const S &)> assembler;

  explicit DetailedStorage(std::shared_ptr<Storage<F>> first, std::shared_ptr<Storage<S>> second,
                           std::function<F(const T &)> first_extractor, std::function<S(const T &)> second_extractor,
                           std::function<T(const F &, const S &)> assembler)
      : first(first), second(second), first_extractor(first_extractor), second_extrator(second_extractor),
        assembler(assembler) {}

  T load(const Key &key) const { return assembler(first->load(key), second->load(key)); }

  virtual bool exists(const Key &key) const { return first->exists(key) && second->exists(key); }

  virtual std::set<Key> keys() const { return first->keys(); }

  DetailedStorage(const Json::Structure &structure) {
    throw exceptions::NotImplemented("DetailedStorage", "from_json");
  }
  virtual std::any structure() const {
    return Json::Dict({{"first", first->structure()}, {"second", second->structure()}});
  }

protected:
  void _save(const Key &key, const T &content) {
    first->save(first_extractor(content));
    second->save(second_extractor(content));
  }
  void _remove(const Key &key) {
    first->remove(key);
    second->remove(key);
  }
};
} // namespace gorage
