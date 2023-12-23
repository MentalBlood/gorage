
#include <IntegrityStorage.hpp>
#include <MemoryStorage.hpp>
#include <Storage.hpp>
#include <common.hpp>

class Sample : public gorage::IntegrityStorage<gorage::Bytes, gorage::Bytes> {
public:
  Sample(std::shared_ptr<gorage::Storage<gorage::Bytes>> base,
         std::shared_ptr<gorage::Storage<gorage::Bytes>> integrity)
      : gorage::IntegrityStorage<gorage::Bytes, gorage::Bytes>(base,
                                                               integrity) {}

protected:
  gorage::Bytes digest(const gorage::Bytes &content) const {
    return gorage::Bytes(content.begin(),
                         content.begin() + int(content.size() / 2));
  }
  gorage::Bytes digest_raw(const gorage::Bytes &content) const {
    return digest(content);
  }
};
void test(std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> base,
          std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> integrity) {
  Sample storage(base, integrity);
  REQUIRE(storage.ids().size() == 0);

  gorage::Id usi("usi");
  gorage::Bytes content = {'d', 'a', 't', 'a'};

  SUBCASE("saving") {
    storage.save(usi, content);
    CHECK_EQ(storage.load(usi), content);
    CHECK_EQ(storage.load(usi), base->load(usi));
  }
  SUBCASE("removing") {
    storage.save(usi, content);
    storage.remove(usi);

    CHECK_EQ(storage.exists(usi), false);
    CHECK_EQ(base->exists(usi), false);
    CHECK_EQ(integrity->exists(usi), false);

    try {
      storage.load(usi);
      CHECK_EQ(1, 0);
    } catch (const gorage::exceptions::KeyError &) {
    }
    try {
      base->load(usi);
      CHECK_EQ(1, 0);
    } catch (const gorage::exceptions::KeyError &) {
    }
    try {
      integrity->load(usi);
      CHECK_EQ(1, 0);
    } catch (const gorage::exceptions::KeyError &) {
    }
  }
}
TEST_CASE("separate storages") {
  test(std::make_shared<gorage::MemoryStorage<gorage::Bytes>>(),
       std::make_shared<gorage::MemoryStorage<gorage::Bytes>>());
}

TEST_CASE("same storage") {
  auto base = std::make_shared<gorage::MemoryStorage<gorage::Bytes>>();
  test(base, base);
}
