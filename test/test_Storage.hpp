#include "../include/FileStorage.hpp"
#include "../include/FlatStorage.hpp"
#include "../include/MemoryStorage.hpp"
#include "../include/Storage.hpp"
#include "doctest.h"

inline void test_Storage_remove(std::shared_ptr<gorage::Storage<std::string>> storage, const size_t &initial_size,
                                const std::string &value) {
  SUBCASE("removing") {
    for (size_t i = 0; i < initial_size; i++) {
      const auto name = "remove from position " + std::to_string(i);
      SUBCASE(name.c_str()) {
        storage->clear();

        std::vector<gorage::Key> keys;
        for (size_t j = 0; j < initial_size; j++)
          keys.push_back(storage->save(value));

        storage->remove(keys[i]);
        keys.erase(keys.begin() + i);

        CHECK_EQ(storage->keys().size(), initial_size - 1);
        for (const auto &k : keys)
          CHECK_EQ(storage->load(k), value);
      }
    }
  }
}

inline void test_Storage(std::shared_ptr<gorage::Storage<std::string>> storage) {
  const auto value = "value";

  SUBCASE("saving") { CHECK_EQ(storage->load(storage->save(value)), value); }
  test_Storage_remove(storage, 4, value);
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
TEST_CASE("FlatStorage") {
  const auto path = "flat_storage.txt";
  test_Storage(std::make_shared<gorage::FlatStorage<std::string>>(path));
  std::filesystem::remove(path);
}
