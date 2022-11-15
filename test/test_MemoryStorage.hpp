#include <MemoryStorage.hpp>



void checkUsi(gorage::MemoryStorage<std::string>& storage, const gorage::Usi& usi) {
	storage.save(usi, "s");
	CHECK_EQ(storage.load(usi), "s");
}


TEST_CASE("`MemoryStorage`") {

	gorage::MemoryStorage<std::string> storage;
	REQUIRE(storage.size() == 0);

	SUBCASE("saving") {

		SUBCASE("with given USI") {
			checkUsi(storage, gorage::Usi("usi"));
		}
		SUBCASE("with random generated USI") {
			checkUsi(storage, gorage::Usi(32));
		}
		SUBCASE("with USI generated from Bytes") {
			checkUsi(storage, gorage::Usi(gorage::Bytes{'l', 'a'}));
		}

	}

	SUBCASE("removing") {

		gorage::Usi usi("usi");

		CHECK_EQ(storage.size(), 0);

		storage.save(usi, "s");
		CHECK_EQ(storage.size(), 1);

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