#include "Storage.hpp"

namespace gorage {
template <class T> using P = std::shared_ptr<T>;

template <class I, class O> class Worker {
public:
  P<Storage<I>> input_storage;
  P<Storage<O>> output_storage;
  Query query;

  virtual O process(const I &input) = 0;

  virtual void operator()() {
    for (const auto k : input_storage->keys(query))
      output_storage->save(process(input_storage->load(k)));
  }
};

template <class I, class M, class O> class Chain : public Worker<I, O> {
public:
  P<Worker<I, M>> first;
  P<Worker<M, O>> second;

  Chain(const P<Worker<I, M>> &first, const P<Worker<M, O>> &second)
      : Worker<I, O>::input_storage(first->input_storage), Worker<I, O>::output_storage(second->output_storage),
        Worker<I, O>::query(first->query), first(first), second(second) {}

  virtual O process(const I &input) {
    const auto m = first->process(input);
    first->output_storage->save(m);
    return second->process(m);
  }

  template <class N> Chain<I, O, N> operator<<(const P<Worker<O, N>> &next) { return Chain<I, O, N>(*this, next); }
};
} // namespace gorage
