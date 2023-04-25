#include <IntegrityStorage.hpp>

#include <Json.hpp>
#include <common.hpp>
#include <Storage.hpp>
#include <MemoryStorage.hpp>



class Sample : public gorage::IntegrityStorage<gorage::Bytes, gorage::Bytes> {

public:
	Sample(std::shared_ptr<gorage::Storage<gorage::Bytes>> base, std::shared_ptr<gorage::Storage<gorage::Bytes>> integrity):
		gorage::IntegrityStorage<gorage::Bytes, gorage::Bytes>(base, integrity) {}


protected:
	gorage::Bytes digest(const gorage::Bytes& content) const {
		return gorage::Bytes(content.begin(), content.begin() + int(content.size() / 2));
	}

};


void test(std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> base, std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> integrity) {

	Sample storage(base, integrity);
	REQUIRE(storage.usis().size() == 0);

	gorage::Usi usi("usi");
	gorage::Bytes content = {'d', 'a', 't', 'a'};

	SUBCASE("saving") {
		storage.save(usi, content);
		CHECK_EQ(storage.load(usi), content);
		CHECK_EQ(storage.load(usi), base->load(usi));
	}

	SUBCASE("removing") {

		storage.save(usi, content);
		storage.remove(usi);

		try {
			storage.load(usi);
			CHECK_EQ(1, 0);
		} catch(const std::exception& e) {}

		try {
			base->load(usi);
			CHECK_EQ(1, 0);
		} catch(const std::exception& e) {}

		try {
			integrity->load(usi);
			CHECK_EQ(1, 0);
		} catch(const std::exception& e) {}

	}

}


TEST_CASE("separate storages") {

	std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> base      = std::make_shared<gorage::MemoryStorage<gorage::Bytes>>();
	std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> integrity = std::make_shared<gorage::MemoryStorage<gorage::Bytes>>();

	test(base, integrity);

}

TEST_CASE("same storage") {

	std::shared_ptr<gorage::MemoryStorage<gorage::Bytes>> base      = std::make_shared<gorage::MemoryStorage<gorage::Bytes>>();

	test(base, base);

}