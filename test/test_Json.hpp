#include <Json.hpp>

#include <regex>



TEST_CASE("testing decoding from JSON") {

	SUBCASE("testing strings decoding") {

		std::string s = "\"lalala\"";
		std::any decoded = gorage::Json::decode(s);

		CHECK(
			std::any_cast<std::string>(decoded)
			==
			"lalala"
		);

	}

	SUBCASE("testing numbers decoding") {

		SUBCASE("testing integer decoding") {

			std::string s = "1234";
			std::any decoded = gorage::Json::decode(s);

			CHECK(std::any_cast<int>(decoded) == 1234);

		}

		SUBCASE("testing floating point decoding") {

			std::string s = "1234.1234";
			std::any decoded = gorage::Json::decode(s);

			CHECK(std::any_cast<double>(decoded) == 1234.1234);

		}

	}

	SUBCASE("testing list decoding") {

		std::string s = "["
			"\"lalala\","
			"1234,"
			"1234.123400"
		"]";
		gorage::Json::List decoded = std::any_cast<gorage::Json::List>(gorage::Json::decode(s));

		CHECK(std::any_cast<std::string>(decoded[0]) == "lalala");
		CHECK(std::any_cast<int>		(decoded[1]) == 1234);
		CHECK(std::any_cast<double>		(decoded[2]) == 1234.1234);

	}

	SUBCASE("testing dict decoding") {

		std::string s = "{"
			"\"double\":" "1234.123400,"
			"\"string\":" "\"lalala\","
			"\"int\":" "1234"
		"}";
		gorage::Json::Dict decoded = std::any_cast<gorage::Json::Dict>(gorage::Json::decode(s));

		CHECK(std::any_cast<std::string>(decoded["string"]) == "lalala");
		CHECK(std::any_cast<int>		(decoded["int"]) == 1234);
		CHECK(std::any_cast<double>		(decoded["double"]) == 1234.1234);

	}

}


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
		SUBCASE("testing `double` encoding") {
			const double f = 1234.1234;
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
			{"double", 1234.1234}
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
					"\"double\":" "1234.123400,"
					"\"string\":" "\"lalala\","
					"\"int\":" "1234"
				"}"
			);
		}

	}

}