#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <gorage.hpp>



TEST_CASE("testing encoding to JSON") {

	CHECK(
		gorage::Json::encode(
			gorage::Json::Dict{
				{"s", "lalala"},
				{"v", gorage::Json::List{
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
				}}
			}
		)
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