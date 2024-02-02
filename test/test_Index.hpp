#include "../include/MemoryStorage.hpp"
#include "../include/aliases.hpp"
#include "common.hpp"
#include "doctest.h"

TEST_CASE("`Index`") {
  SUBCASE("custom extractors") {
    gorage::MemoryStorage<std::string> storage;
    REQUIRE(storage.keys().size() == 0);

    const auto s = "string";
    const auto usi = gorage::Key("usi");

    gorage::indexes<std::string>(storage, {{"first letter",
                                            [](const std::string &s) {
                                              REQUIRE(s.length() >= 1);
                                              return s.substr(0, 1);
                                            }},
                                           {"second letter", [](const std::string &s) {
                                              REQUIRE(s.length() >= 2);
                                              return s.substr(1, 1);
                                            }}});

    SUBCASE("saving") {
      storage.save(usi, s);
      CHECK_EQ(storage.load(usi), s);
      CHECK_EQ(storage.Storage::keys("first letter", "s"), std::set<gorage::Key>({gorage::Key("usi")}));
      CHECK_EQ(storage.Storage::keys("second letter", "t"), std::set<gorage::Key>({gorage::Key("usi")}));
      CHECK_EQ(storage.Storage::keys("first letter", "a").size(), 0);
      CHECK_EQ(storage.Storage::keys("second letter", "a").size(), 0);
    }
    SUBCASE("removing") {
      storage.save(usi, s);
      storage.remove(usi);

      CHECK_EQ(storage.keys().size(), 0);

      CHECK_EQ(storage.Storage::keys("first letter", "s").size(), 0);
      CHECK_EQ(storage.Storage::keys("second letter", "t").size(), 0);
      CHECK_EQ(storage.Storage::keys("first letter", "a").size(), 0);
      CHECK_EQ(storage.Storage::keys("second letter", "a").size(), 0);
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
  SUBCASE("JSON field name based extractors") {
    gorage::MemoryStorage<Complex> storage;
    REQUIRE(storage.keys().size() == 0);

    const auto s = "string";
    const auto a = Complex(1, 1.5, "a");
    const auto b = Complex(2, 2.5, "b");

    gorage::indexes<Complex>(storage);

    SUBCASE("saving") {
      const auto a_key = storage.save(a);
      const auto b_key = storage.save(b);

      CHECK_EQ(storage.load(a_key), a);
      CHECK_EQ(storage.load(b_key), b);
      CHECK_EQ(storage.Storage::keys("i", long(1)), std::set({a_key}));
      CHECK_EQ(storage.Storage::keys("d", 1.5), std::set({a_key}));
      CHECK_EQ(storage.Storage::keys("s", "a"), std::set({a_key}));
      CHECK_EQ(storage.Storage::keys("i", long(2)), std::set({b_key}));
      CHECK_EQ(storage.Storage::keys("d", 2.5), std::set({b_key}));
      CHECK_EQ(storage.Storage::keys("s", "b"), std::set({b_key}));
    }
    SUBCASE("removing") {
      const auto a_key = storage.save(a);
      const auto b_key = storage.save(b);
      storage.remove(a_key);
      storage.remove(b_key);

      CHECK_EQ(storage.exists(a_key), false);
      CHECK_EQ(storage.exists(b_key), false);
      CHECK_EQ(storage.Storage::keys("i", long(1)).size(), 0);
      CHECK_EQ(storage.Storage::keys("d", 1.5).size(), 0);
      CHECK_EQ(storage.Storage::keys("s", "a").size(), 0);
      CHECK_EQ(storage.Storage::keys("i", long(2)).size(), 0);
      CHECK_EQ(storage.Storage::keys("d", 2.5).size(), 0);
      CHECK_EQ(storage.Storage::keys("s", "b").size(), 0);
    }
  }
}
