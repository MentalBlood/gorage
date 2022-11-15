#include <memory>

#include <MemoryStorage.hpp>



TEST_CASE("`MemoryStorage`") {

	gorage::MemoryStorage<std::string> storage;
	REQUIRE(storage.size() == 0);

	std::string s = "string";
	gorage::Usi usi;

	SUBCASE("saving") {

		SUBCASE("with given USI") {

			usi = gorage::Usi("usi");

			storage.save(usi, s);
			CHECK_EQ(storage.load(usi), s);

		}

		SUBCASE("with random generated USI") {

			usi = gorage::Usi(32);

			storage.save(usi, s);
			CHECK_EQ(storage.load(usi), s);

		}

		SUBCASE("with USI generated from Bytes") {

			usi = gorage::Usi(gorage::Bytes{'l', 'a'});

			storage.save(usi, s);
			CHECK_EQ(storage.load(usi), s);

		}

	}

	SUBCASE("removing") {

		usi = gorage::Usi("usi");

		storage.save(usi, s);
		storage.remove(usi);

		CHECK_EQ(storage.size(), 0);

	}

	SUBCASE("iterating") {

		std::vector<std::string> usis_strings = {"a", "b", "c"};
		for (const auto& u : usis_strings) {
			storage.save(gorage::Usi(u), u + "_value");
		}

		CHECK_EQ(storage.size(), usis_strings.size());

		for (const auto& u : usis_strings) {
			CHECK_EQ(storage.load(gorage::Usi(u)), u + "_value");
		}

	}

}