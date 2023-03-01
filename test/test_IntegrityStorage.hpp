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
	gorage::Bytes digest(const gorage::Bytes& content) {
		return gorage::Bytes(content.begin(), content.begin() + int(content.size() / 2));
	}

};


TEST_CASE("`IntegrityStorage`") {

	Sample storage(
		std::make_shared<gorage::MemoryStorage<gorage::Bytes>>(),
		std::make_shared<gorage::MemoryStorage<gorage::Bytes>>()
	);
	REQUIRE(storage.size() == 0);

	std::string usi = "usi";
	gorage::Bytes content = {'d', 'a', 't', 'a'};

	SUBCASE("saving") {
		storage.save(usi, content);
		CHECK_EQ(storage.load(usi), content);
	}

}
