#include <cassert>
#include <iostream>

#include "include/ItemStorage.hpp"
#include "include/FileStorage.hpp"
#include "include/BinaryFileStorage.hpp"

#include "modules/rapidjson/document.h"


class CertificateMetadata {

public:

	std::string a;
	std::string b;

	CertificateMetadata():
		a(""), b("") {}

	CertificateMetadata(const std::string& a, const std::string& b):
		a(a), b(b) {}

	void fromJson(const std::string& json_text) {

		std::cout << "CertificateMetadata.fromJson" << std::endl;

		rapidjson::Document json;
		json.Parse(json_text.c_str());

		this->a = json["a"].GetString();
		this->b = json["b"].GetString();

	}

	const std::string toJson() const {
		return (
			"{"
				"\"a\":" "\"" + this->a + "\"" ","
				"\"b\":" "\"" + this->b + "\"" ""
			"}"
		);
	}

};


int main(void) {

	BinaryFileStorage data_storage("storage/data", ".bin");
	FileStorage<CertificateMetadata> metadata_storage("storage/metadata", ".json");

	ItemStorage<CertificateMetadata> storage(data_storage, metadata_storage);

	std::string usi = "1234";
	std::vector<uint8_t> data{'d', 'e', 'r'};
	std::string metadata_a = "lalala";
	std::string metadata_b = "lblblb";

	storage.save(
		usi,
		Item<CertificateMetadata>(
			data,
			CertificateMetadata(metadata_a, metadata_b)
		)
	);

	assert(storage.load(usi).data == data);
	assert(storage.load(usi).metadata.a == metadata_a);
	assert(storage.load(usi).metadata.b == metadata_b);

	assert(storage.data_storage.load(usi) == data);
	assert(storage.metadata_storage.load(usi).a == metadata_a);
	assert(storage.metadata_storage.load(usi).b == metadata_b);

	storage.clearCache();

	assert(storage.load(usi).data == data);
	assert(storage.load(usi).metadata.a == metadata_a);
	assert(storage.load(usi).metadata.b == metadata_b);

	assert(storage.data_storage.load(usi) == data);
	assert(storage.metadata_storage.load(usi).a == metadata_a);
	assert(storage.metadata_storage.load(usi).b == metadata_b);

	std::cout << "Finished OK" << std::endl;

	return 0;

}