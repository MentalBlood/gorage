#include <MemoryStorage.hpp>

TEST_CASE("`MemoryStorage`") {
  gorage::MemoryStorage<std::string> storage;
  REQUIRE(storage.ids().size() == 0);

  const auto s = "string";
  const auto usi = gorage::Id("usi");

  SUBCASE("saving") {
    storage.save(usi, s);
    CHECK_EQ(storage.load(usi), s);
  }
  SUBCASE("removing") {
    storage.save(usi, s);
    storage.remove(usi);

    CHECK_EQ(storage.ids().size(), 0);
  }
  SUBCASE("iterating") {
    REQUIRE(storage.ids().size() == 0);

    const auto ids = std::vector<gorage::Id>(
        {gorage::Id("a"), gorage::Id("b"), gorage::Id("c")});
    for (const auto &u : ids)
      storage.save(u, u.value + "_value");

    CHECK_EQ(storage.ids().size(), ids.size());

    for (const auto &u : ids)
      CHECK_EQ(storage.load(u), u.value + "_value");
  }
}
