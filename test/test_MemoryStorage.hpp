#include <MemoryStorage.hpp>



void checkKey(gorage::MemoryStorage<std::string>& storage, const gorage::Key& key) {
	storage.save(key, "s");
	CHECK_EQ(storage.load(key), "s");
}


TEST_CASE("`MemoryStorage`") {

	gorage::MemoryStorage<std::string> storage;
	REQUIRE(storage.size() == 0);

	SUBCASE("saving") {

		SUBCASE("with given key") {
			checkKey(storage, gorage::Key("key"));
		}
		SUBCASE("with random generated key") {
			checkKey(storage, gorage::Key(32));
		}
		SUBCASE("with key generated from Bytes") {
			checkKey(storage, gorage::Key(gorage::Bytes{'l', 'a'}));
		}

	}

	SUBCASE("removing") {

		gorage::Key key("key");

		CHECK_EQ(storage.size(), 0);

		storage.save(key, "s");
		CHECK_EQ(storage.size(), 1);

		storage.remove(key);
		CHECK_EQ(storage.size(), 0);

	}

	SUBCASE("iterating") {

		std::vector<std::string> keys_strings = {"a", "b", "c"};
		for (const auto& u : keys_strings) {
			storage.save(gorage::Key(u), u + "_value");
		}

		CHECK_EQ(storage.size(), keys_strings.size());

		for (const auto& u : keys_strings) {
			CHECK_EQ(storage.load(gorage::Key(u)), u + "_value");
		}

	}

}