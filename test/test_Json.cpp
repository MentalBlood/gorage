#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <gorage.hpp>



TEST_CASE("testing encoding to JSON") {

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

	CHECK(
		CertificateMetadata(
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
		).toJson()
		==
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
		"}"
	);

}