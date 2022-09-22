#include <MemoryStorage.hpp>



TEST_CASE("`MemoryStorage`") {

	gorage::MemoryStorage<std::string> storage;
	REQUIRE(storage.size() == 0);

	std::string s = "string";
	std::string usi = "usi";

	SUBCASE("saving") {

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

	SUBCASE("removing") {

		storage.save(usi, s);
		storage.remove(usi);

		CHECK(storage.size() == 0);

	}

	SUBCASE("iterating") {

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