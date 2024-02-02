#include "Storage.hpp"

namespace gorage {
template <class T> using P = std::shared_ptr<T>;

template <class T> class Item : public Json {
public:
  T content;
  std::string chain_id;

  Item(const T &content, const std::string &chain_id) : content(content), chain_id(chain_id) {}
  Item(const T &content) : Item(content, std::to_string(random::seed()) + random::Name(16).value) {}

  Item(const Json::Structure &structure) {
    const auto dict = cast<Dict>(structure.value());
    content = get<T>(dict, "content");
    chain_id = get<T>(dict, "chain_id");
  }
  virtual std::any structure() const { return Dict({{"content", content.structure()}, {"chain_id", chain_id}}); }
};

template <class I, class O> class Worker {
public:
  P<Storage<Item<I>>> input_storage;
  P<Storage<Item<O>>> output_storage;
  Query query;

  virtual O process(const Item<I> &input) = 0;

  virtual void operator()() {
    for (const auto k : input_storage->keys(query))
      output_storage->save(process(input_storage->load(k)));
  }
};

template <class I, class M, class O> class Chain : public Worker<I, O> {
public:
  using Base = Worker<Item<I>, Item<O>>;

  P<Worker<Item<I>, Item<M>>> first;
  P<Worker<Item<M>, Item<O>>> second;

  Chain(const P<Worker<Item<I>, Item<M>>> &first, const P<Worker<Item<M>, Item<O>>> &second)
      : Base::input_storage(first->input_storage), Base::output_storage(second->output_storage),
        Base::query(first->query), first(first), second(second) {}

  virtual O process(const Item<I> &input) {
    const auto m = first->process(input);
    first->output_storage->save(m);
    return second->process(m);
  }

  template <class N> Chain<I, O, N> operator<<(const P<Worker<O, N>> &next) {
    return Chain<I, O, N>(P<Base>(this), next);
  }
};
} // namespace gorage
