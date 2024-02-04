#include <chrono>

#include "../modules/blake3/blake3.h"
#include "Storage.hpp"

namespace gorage {
template <class T> using P = std::shared_ptr<T>;

template <class T> class Item : public Json {
public:
  using time = std::chrono::time_point<std::chrono::system_clock>;

  T content;
  time created;
  std::string chain_id;

  Bytes content_digest() {
    const auto content_json = encode(content);
    const auto content_json_bytes = Bytes(content_json.begin(), content_json.end());

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, content_json_bytes.data(), content_json_bytes.size());

    Bytes result;
    result.reserve(BLAKE3_OUT_LEN);
    blake3_hasher_finalize(&hasher, &result[0], BLAKE3_OUT_LEN);
    return result;
  }

  Item(const T &input, const T &output) : content(output.content), created(input.created), chain_id(input.chain_id) {}

  template <class O>
  Item(const T &input, const O &output) : content(output.content), created(output.created), chain_id(input.chain_id) {}

  Item(const T &content)
      : content(content), created(std::chrono::system_clock::now()), chain_id(String(content_digest()).encoded()) {}

  Item(const Json::Structure &structure) {
    const auto dict = cast<Dict>(structure.value());
    content = T(dict.at("content"));
    chain_id = get<String>(dict, "chain_id").s;
    created = time(std::chrono::milliseconds(get<long>(dict, "created")));
  }
  virtual std::any structure() const {
    return Dict(
        {{"content", content.structure()},
         {"chain_id", chain_id},
         {"created", std::chrono::time_point_cast<std::chrono::milliseconds>(created).time_since_epoch().count()}});
  }
};

template <class I, class O> class Worker {
public:
  P<Storage<Item<I>>> input_storage;
  P<Storage<Item<O>>> output_storage;
  Query query;

  virtual O process(const Item<I> &input) = 0;

  virtual void operator()() {
    for (const auto k : input_storage->keys(query)) {
      const auto input = input_storage->load(k);
      output_storage->save(Item(input, process(input)));
    }
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
