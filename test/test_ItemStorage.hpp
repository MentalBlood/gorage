#include <ItemStorage.hpp>

#include <Json.hpp>
#include <MemoryStorage.hpp>



class C : public gorage::Json {

public:

	std::string s;

	virtual void update(const std::any& structure) {
		s = std::any_cast<std::string>(
			std::any_cast<Dict>(structure)["s"]
		);
	}

private:

	virtual std::any _getStructure() const {
		return Dict{
			{"s", s}
		};
	}

};


TEST_CASE("testing `ItemStorage`") {

	try {

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

	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

}