#include "../include/MemoryStorage.hpp"
#include "doctest.h"

TEST_CASE("`MemoryStorage`") {
  gorage::MemoryStorage<std::string> storage;
  REQUIRE(storage.keys().size() == 0);

  const auto s = "string";
  const auto usi = gorage::Key("usi");

  SUBCASE("saving") {
    storage.save(usi, s);
    CHECK_EQ(storage.load(usi), s);
  }
  SUBCASE("removing") {
    storage.save(usi, s);
    storage.remove(usi);

    CHECK_EQ(storage.keys().size(), 0);
  }
  SUBCASE("iterating") {
    REQUIRE(storage.keys().size() == 0);

    const auto keys = std::vector<gorage::Key>({gorage::Key("a"), gorage::Key("b"), gorage::Key("c")});
    for (const auto &u : keys)
      storage.save(u, u.value + "_value");

    CHECK_EQ(storage.keys().size(), keys.size());

    for (const auto &u : keys)
      CHECK_EQ(storage.load(u), u.value + "_value");
  }
}
