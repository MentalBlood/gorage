#include <functional>

#include "../include/FileStorage.hpp"
#include "../include/FlatStorage.hpp"
#include "../include/MemoryStorage.hpp"
#include "../include/Storage.hpp"
#include "doctest.h"

namespace gorage {
namespace tests {

using Value = std::function<std::string(const gorage::Key &)>;
using TestStorage = std::shared_ptr<gorage::Storage<std::string>>;

inline void test_storage_keys(TestStorage storage, const Value &value, const std::vector<gorage::Key> &keys,
                              const std::string &name) {
  SUBCASE(name.c_str()) {
    CHECK_EQ(storage->keys().size(), keys.size());
    for (const auto &k : keys)
      CHECK_EQ(storage->load(k), value(k));
  }
}

inline void test_storage(
    TestStorage storage, const size_t &initial_size = 4,
    const Value &value = [](const gorage::Key &key) { return key.value + "_value"; }) {
  for (size_t i = 0; i < initial_size; i++) {
    const auto name = "remove from position " + std::to_string(i);
    SUBCASE(name.c_str()) {
      storage->clear();

      std::vector<gorage::Key> keys;
      for (size_t j = 0; j < initial_size; j++) {
        const auto key = storage->key();
        storage->save(key, value(key));
        keys.push_back(key);
        test_storage_keys(storage, value, keys,
                          "test keys after adding " + std::to_string(keys.size()) + " key-value pairs");
      }

      while (keys.size()) {
        const size_t key_to_remove = (i < keys.size()) ? i : (keys.size() / 2);
        storage->remove(keys[key_to_remove]);
        keys.erase(keys.begin() + key_to_remove);
        test_storage_keys(storage, value, keys,
                          "test keys after removing all but " + std::to_string(keys.size()) + " key-value pairs");
      }
    }
  }
  storage->clear();
}

TEST_CASE("MemoryStorage") { test_storage(std::make_shared<gorage::MemoryStorage<std::string>>()); }
TEST_CASE("FileStorage") {
  test_storage(std::make_shared<gorage::Storage<gorage::File<std::string>>>("file_storage", "txt"));
}
TEST_CASE("FlatStorage") {
  const auto path = "flat_storage.txt";
  test_storage(std::make_shared<gorage::FlatStorage<std::string>>(path));
  std::filesystem::remove(path);
}
} // namespace tests
} // namespace gorage
