#include <ItemStorage.hpp>

#include <Usi.hpp>
#include <Json.hpp>
#include <common.hpp>
#include <MemoryStorage.hpp>


TEST_CASE("`ItemStorage`") {

	gorage::MemoryStorage<gorage::Item<C>> storage;
	REQUIRE(storage.usis().size() == 0);

	const gorage::Usi  usi("usi");
	const std::string  json = "{\"s\": \"string\"}";
	const gorage::Item item(
		gorage::Bytes{'d', 'a', 't', 'a'},
		gorage::Json::from<C>(json)
	);

	SUBCASE("saving") {

		storage.save(usi, item);

		CHECK_EQ(storage.load(usi).data.size(), item.data.size());
		CHECK_EQ(storage.load(usi).metadata.s, item.metadata.s);

	}

}
