#include "../include/MemoryStorage.hpp"
#include "doctest.h"

TEST_CASE("`Index`") {
  gorage::MemoryStorage<std::string> storage;
  REQUIRE(storage.keys().size() == 0);

  const auto s = "string";
  const auto usi = gorage::Key("usi");

  storage.index("first letter", [](const std::string &s) {
    REQUIRE(s.length() >= 1);
    return std::string(1, s.at(0));
  });
  storage.index("second letter", [](const std::string &s) {
    REQUIRE(s.length() >= 2);
    return std::string(1, s.at(1));
  });

  SUBCASE("saving") {
    storage.save(usi, s);
    CHECK_EQ(storage.load(usi), s);
    CHECK_EQ(storage.Storage::keys("first letter", "s"),
             std::set<gorage::Key>({gorage::Key("usi")}));
    CHECK_EQ(storage.Storage::keys("second letter", "t"),
             std::set<gorage::Key>({gorage::Key("usi")}));
    CHECK_EQ(storage.Storage::keys("first letter", "a"),
             std::set<gorage::Key>());
    CHECK_EQ(storage.Storage::keys("second letter", "a"),
             std::set<gorage::Key>());
  }
  SUBCASE("removing") {
    storage.save(usi, s);
    storage.remove(usi);

    CHECK_EQ(storage.keys().size(), 0);

    CHECK_EQ(storage.Storage::keys("first letter", "s"),
             std::set<gorage::Key>());
    CHECK_EQ(storage.Storage::keys("second letter", "t"),
             std::set<gorage::Key>());
    CHECK_EQ(storage.Storage::keys("first letter", "a"),
             std::set<gorage::Key>());
    CHECK_EQ(storage.Storage::keys("second letter", "a"),
             std::set<gorage::Key>());
  }
  SUBCASE("iterating") {
    REQUIRE(storage.keys().size() == 0);

    const auto keys = std::vector<gorage::Key>(
        {gorage::Key("a"), gorage::Key("b"), gorage::Key("c")});
    for (const auto &u : keys)
      storage.save(u, u.value + "_value");

    CHECK_EQ(storage.keys().size(), keys.size());

    for (const auto &u : keys)
      CHECK_EQ(storage.load(u), u.value + "_value");
  }
}
