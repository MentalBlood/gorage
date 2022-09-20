#include <map>
#include <memory>
#include <vector>
#include <cassert>
#include <iostream>
#include <unordered_map>

#include "modules/rapidjson/document.h"

#include "include/gorage.hpp"


class CertificateMetadata : public gorage::Json {

public:

	std::string s;
	std::vector<std::string> v;

	CertificateMetadata() {}

	CertificateMetadata(const std::string& s, const std::vector<std::string>& v):
		s(s), v(v) {}

	void updateFromJson(const std::string& json_text) {}

	std::string toJson() const {
		std::cout << ".toJson()" << std::endl;
		return _toJson(
			Dict{
				{"s", s},
				{"v", v}
			}
		);
	}

};


int main(void) {

	try {

		gorage::ItemStorage<CertificateMetadata> storage(
			std::make_shared<gorage::MemoryStorage<gorage::Bytes>>(),
			std::make_shared<gorage::MemoryStorage<CertificateMetadata>>()
		);

		gorage::Bytes data{'1', '2', '3'};

		CertificateMetadata metadata("lalala", {"la", "la", "la"});
		std::cout << metadata.toJson();
		assert(metadata.toJson() == "{\"s\":\"lalala\",\"v\":[\"la\",\"la\",\"la\"]}");

		// CertificateMetadata metadata("aaa", "bbb");
		// assert(metadata.toJson() == "{\"a\":\"aaa\",\"b\":\"bbb\"}");
		// gorage::Item<CertificateMetadata> item(data, metadata);

		// storage.save("lalala", item);

		// assert(storage.get("lalala").metadata.a == metadata.a);
		// assert(storage.get("lalala").metadata.b == metadata.b);

		std::cout << "ok" << std::endl;

	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;

}