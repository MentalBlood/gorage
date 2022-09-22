#include <ItemStorage.hpp>

#include <Json.hpp>
#include <common.hpp>
#include <MemoryStorage.hpp>


TEST_CASE("testing `ItemStorage`") {

	gorage::ItemStorage<C> storage(
		std::make_shared<gorage::MemoryStorage<gorage::Bytes>>(),
		std::make_shared<gorage::MemoryStorage<C>>()
	);
	REQUIRE(storage.size() == 0);

	std::string usi = "usi";
	std::string json = "{\"s\": \"string\"}";
	gorage::Item item(
		gorage::Bytes{'d', 'a', 't', 'a'},
		gorage::Json::createFromJson<C>(json)
	);

	SUBCASE("testing saving") {

		storage.save(usi, item);

		CHECK(storage.load(usi).data.size() == item.data.size());
		CHECK(storage.load(usi).metadata.s == item.metadata.s);

	}

}