#include <any>
#include <memory>
#include <cassert>
#include <iostream>

#include "include/gorage.hpp"


class CertificateMetadata : public gorage::Json {

public:

	std::string s;
	List v;

	CertificateMetadata() {}

	CertificateMetadata(const std::string& s, const List& v):
		s(s), v(v) {}

	void updateFromJson(const std::string& json_text) {}

protected:

	std::any _getStructure() const {
		return Dict{
			{"s", s},
			{"v", v}
		};
	}

};


int main(void) {

	try {

		gorage::ItemStorage<CertificateMetadata> storage(
			std::make_shared<gorage::MemoryStorage<gorage::Bytes>>(),
			std::make_shared<gorage::MemoryStorage<CertificateMetadata>>()
		);

		gorage::Bytes data{'1', '2', '3'};

		CertificateMetadata metadata(
			"lalala",
			gorage::Json::List{
				"la",
				1,
				1.1,
				gorage::Bytes{'b', 'y', 't', 'e', 's'},
				gorage::Json::Dict{
					{"a", 1},
					{"b", 2}
				},
				// std::make_shared<CertificateMetadata>("lololo", gorage::Json::List{1, 2, 3}),
				"\"\\escaping test\\\""
			}
		);
		std::string correct_result = 
		"{"
			"\"s\":" "\"lalala\","
			"\"v\":" "["
				"\"la\","
				"1,"
				"1.100000,"
				"\"Ynl0ZXM=\","
				"{"
					"\"a\":" "1,"
					"\"b\":" "2"
				"},"
				"\"\\\"\\\\escaping test\\\\\\\"\""
			"]"
		"}";
		std::string result = metadata.toJson();
		std::cout << result << std::endl;
		std::cout << correct_result << std::endl;
		assert(result == correct_result);

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