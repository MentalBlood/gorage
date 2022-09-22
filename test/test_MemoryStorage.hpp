#include <MemoryStorage.hpp>



TEST_CASE("testing `MemoryStorage`") {

	gorage::MemoryStorage<std::string> storage;
	REQUIRE(storage.size() == 0);

	std::string s = "string";
	std::string usi = "usi";

	SUBCASE("testing saving") {

		SUBCASE("testing saving with given USI") {
			storage.save(usi, s);
			CHECK(storage.load(usi) == s);
		}

		// SUBCASE("testing saving with random generated USI") {

		// 	std::string s = "string";

		// 	std::string usi = storage.save(s);
		// 	CHECK(storage.load(usi) == s);

		// }

	}

	SUBCASE("testing removing") {

		storage.save(usi, s);
		storage.remove(usi);

		CHECK(storage.size() == 0);

	}

	SUBCASE("testing iterating") {

		std::vector<std::string> usis = {"a", "b", "c"};
		for (const auto& u : usis) {
			storage.save(u, u + "_value");
		}

		CHECK(storage.size() == usis.size());

		for (const auto& u : usis) {
			CHECK(storage.load(u) == u + "_value");
		}

	}

}