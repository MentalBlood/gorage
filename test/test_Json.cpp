#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS
#include "doctest.h"

#include <gorage.hpp>

#include <regex>



void testEncodedString(const std::string& encoded) {

	SUBCASE("encoded string must be surrounded by double quotes") {

		REQUIRE(encoded.length() > 0);

		CHECK(encoded[0] == '\"');
		CHECK(encoded[encoded.length()-1] == '\"');

	}

	SUBCASE("encoded string `\\` characters must be escaped") {
		CHECK(
			!std::regex_match(
				encoded,
				std::regex("[^\\\\]\\\\(?:[^\\\\]|$)")
			)
		);
	}

}


void testEncodedNumber(const std::string& encoded) {

	SUBCASE("encoded number must contain only `1234567890.` characters") {
		CHECK(
			!std::regex_match(
				encoded,
				std::regex("[^1234567890.]")
			)
		);
	}

}


TEST_CASE("testing encoding to JSON") {

	SUBCASE("testing strings encoding") {

		SUBCASE("testing `char*` encoding") {

			const char s[7] = "lalala";
			const std::string encoded = gorage::Json::encode(s);

			testEncodedString(encoded);
			SUBCASE("hardcoded full comparison") {
				CHECK(encoded == "\"lalala\"");
			}

		}
		SUBCASE("testing `std::string` encoding") {

			const std::string s("lalala");
			const std::string encoded = gorage::Json::encode(s);

			testEncodedString(encoded);
			SUBCASE("hardcoded full comparison") {
				CHECK(encoded == "\"lalala\"");
			}

		}
		SUBCASE("testing `gorage::Bytes` encoding") {

			const gorage::Bytes s{'l', 'a', 'l', 'a', 'l', 'a'};
			const std::string encoded = gorage::Json::encode(s);

			testEncodedString(encoded);
			SUBCASE("hardcoded full comparison") {
				CHECK(encoded == "\"bGFsYWxh\"");
			}

		}

	}

	SUBCASE("testing numbers encoding") {

		SUBCASE("testing `int` encoding") {
			const int i = 1234;
			testEncodedNumber(gorage::Json::encode(i));
		}
		SUBCASE("testing `float` encoding") {
			const float f = 1234.1234;
			testEncodedNumber(gorage::Json::encode(f));
		}

	}

	SUBCASE("testing lists encoding") {

		const gorage::Json::List l{
			"lalala",
			1234,
			1234.1234
		};
		const std::string encoded = gorage::Json::encode(l);

		SUBCASE("encoded list must be surrounded by square brackets `[]`") {

			REQUIRE(l.size() > 0);

			CHECK(encoded[0] == '[');
			CHECK(encoded[encoded.length() - 1] == ']');

		}

		SUBCASE("hardcoded full comparison") {
			CHECK(
				encoded
				==
				"["
					"\"lalala\","
					"1234,"
					"1234.123400"
				"]"
			);
		}

	}

	SUBCASE("testing dicts encoding") {

		const gorage::Json::Dict l{
			{"string", "lalala"},
			{"int", 1234},
			{"float", 1234.1234}
		};
		const std::string encoded = gorage::Json::encode(l);

		SUBCASE("encoded dict must be surrounded by square brackets `{}`") {

			REQUIRE(l.size() > 0);

			CHECK(encoded[0] == '{');
			CHECK(encoded[encoded.length() - 1] == '}');

		}

		SUBCASE("hardcoded full comparison") {
			CHECK(
				encoded
				==
				"{"
					"\"string\":" "\"lalala\","
					"\"int\":" "1234,"
					"\"float\":" "1234.123400"
				"}"
			);
		}

	}

}