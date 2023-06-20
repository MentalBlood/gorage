#include <MemoryStorage.hpp>



TEST_CASE("`MemoryStorage`") {

	gorage::MemoryStorage<std::string> storage;
	REQUIRE(storage.usis().size() == 0);

	const std::string s = "string";
	const gorage::Usi usi("usi");

	SUBCASE("saving") {
		storage.save(usi, s);
		CHECK_EQ(storage.load(usi), s);
	}

	SUBCASE("removing") {

		storage.save(usi, s);
		storage.remove(usi);

		CHECK_EQ(storage.usis().size(), 0);

	}

	SUBCASE("iterating") {

		REQUIRE(storage.usis().size() == 0);

		const std::vector<gorage::Usi> usis = {
			gorage::Usi("a"),
			gorage::Usi("b"),
			gorage::Usi("c")
		};
		for (const auto& u : usis) {
			storage.save(u, u.value() + "_value");
		}

		CHECK_EQ(storage.usis().size(), usis.size());

		for (const auto& u : usis) {
			CHECK_EQ(storage.load(u), u.value() + "_value");
		}

	}

}
