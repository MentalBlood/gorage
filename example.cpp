#include <memory>
#include <cassert>
#include <iostream>

#include "modules/rapidjson/document.h"

#include "include/Json.hpp"
#include "include/ItemStorage.hpp"
#include "include/FileStorage.hpp"
#include "include/BinaryFileStorage.hpp"


class CertificateMetadata : public Json {

public:

	std::string a;
	std::string b;

	CertificateMetadata():
		a(""), b("") {}

	CertificateMetadata(const std::string& a, const std::string& b):
		a(a), b(b) {}

	void updateFromJson(const std::string& json_text) {

		rapidjson::Document json;
		json.Parse(json_text.c_str());

		this->a = json["a"].GetString();
		this->b = json["b"].GetString();

	}

	std::string toJson() const {
		return (
			"{"
				"\"a\":" "\"" + this->a + "\"" ","
				"\"b\":" "\"" + this->b + "\"" ""
			"}"
		);
	}

};


int main(void) {

	ItemStorage<CertificateMetadata> storage(
		std::make_shared<BinaryFileStorage>(BinaryFileStorage(
			"storage/data",
			".bin"
		)),
		std::make_shared<FileStorage<CertificateMetadata>>(FileStorage<CertificateMetadata>(
			"storage/metadata",
			".json"
		))
	);

	std::string usi = "1234";
	std::string data("der");
	std::string metadata_a = "lalala";
	std::string metadata_b = "lblblb";

	storage.save(
		usi,
		Item<CertificateMetadata>(
			data,
			CertificateMetadata(metadata_a, metadata_b)
		)
	);

	assert(storage.load(usi).metadata.toJson() == Json::create<CertificateMetadata>(storage.load(usi).metadata.toJson()).toJson());

	assert(storage.load(usi).data == data);
	assert(storage.load(usi).metadata.a == metadata_a);
	assert(storage.load(usi).metadata.b == metadata_b);

	assert(storage.data_storage.load(usi) == data);
	assert(storage.metadata_storage.load(usi).a == metadata_a);
	assert(storage.metadata_storage.load(usi).b == metadata_b);


	assert(storage.load(usi).data == data);
	assert(storage.load(usi).metadata.a == metadata_a);
	assert(storage.load(usi).metadata.b == metadata_b);

	assert(storage.data_storage.load(usi) == data);
	assert(storage.metadata_storage.load(usi).a == metadata_a);
	assert(storage.metadata_storage.load(usi).b == metadata_b);

	std::cout << "Finished OK" << std::endl;

	return 0;

}