#include "../include/FileStorage.hpp"
#include "../include/FlatStorage.hpp"
#include "../include/MemoryStorage.hpp"
#include "../include/Storage.hpp"
#include "doctest.h"

inline void test_Storage(std::shared_ptr<gorage::Storage<std::string>> storage) {
  const auto value = "value";

  SUBCASE("saving") { CHECK_EQ(storage->load(storage->save(value)), value); }
  SUBCASE("removing") {
    SUBCASE("one") {
      storage->clear();
      storage->remove(storage->save(value));

      CHECK_EQ(storage->keys().size(), 0);
    }
    SUBCASE("first") {
      storage->clear();
      const auto first = storage->save(value);
      const auto middle = storage->save(value);
      const auto last = storage->save(value);
      storage->remove(first);

      CHECK_EQ(storage->keys().size(), 2);
      CHECK_EQ(storage->load(middle), value);
      CHECK_EQ(storage->load(last), value);
    }
    SUBCASE("last") {
      storage->clear();
      const auto first = storage->save(value);
      const auto middle = storage->save(value);
      const auto last = storage->save(value);
      storage->remove(last);

      CHECK_EQ(storage->keys().size(), 2);
      CHECK_EQ(storage->load(first), value);
      CHECK_EQ(storage->load(middle), value);
    }
    SUBCASE("middle") {
      storage->clear();
      const auto first = storage->save(value);
      const auto middle = storage->save(value);
      const auto last = storage->save(value);
      storage->remove(middle);

      CHECK_EQ(storage->keys().size(), 2);
      CHECK_EQ(storage->load(first), value);
      CHECK_EQ(storage->load(last), value);
    }
  }
  SUBCASE("iterating") {
    storage->clear();
    REQUIRE(storage->keys().size() == 0);

    const auto keys = std::vector<gorage::Key>({gorage::Key("a"), gorage::Key("b"), gorage::Key("c")});
    for (const auto &u : keys)
      storage->save(u, u.value + "_value");

    CHECK_EQ(storage->keys().size(), keys.size());

    for (const auto &u : keys)
      CHECK_EQ(storage->load(u), u.value + "_value");
  }
  storage->clear();
}

TEST_CASE("MemoryStorage") { test_Storage(std::make_shared<gorage::MemoryStorage<std::string>>()); }
TEST_CASE("FileStorage") {
  test_Storage(std::make_shared<gorage::Storage<gorage::File<std::string>>>("file_storage", "txt"));
}
TEST_CASE("FlatStorage") { test_Storage(std::make_shared<gorage::FlatStorage<std::string>>("flat_storage.txt")); }
