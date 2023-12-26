#include "../include/ItemStorage.hpp"
#include "../include/Json.hpp"
#include "../include/MemoryStorage.hpp"
#include "common.hpp"
#include "doctest.h"

TEST_CASE("`ItemStorage`") {
  gorage::MemoryStorage<gorage::Item<C>> storage;
  REQUIRE(storage.keys().size() == 0);

  const auto usi = gorage::Key("usi");
  const auto json = std::string("{\"s\": \"string\"}");
  const auto item = gorage::Item(gorage::Bytes{'d', 'a', 't', 'a'},
                                 gorage::Json::from<C>(json));

  SUBCASE("saving") {
    storage.save(usi, item);
    CHECK_EQ(storage.load(usi).data.size(), item.data.size());
    CHECK_EQ(storage.load(usi).metadata.s, item.metadata.s);
  }
}
