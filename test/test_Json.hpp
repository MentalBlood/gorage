#include <Json.hpp>

#include <regex>



TEST_CASE("decoding from JSON") {

	SUBCASE("strings") {

		std::string s = "\"lalala\"";
		std::any decoded = gorage::Json::decode(s);

		CHECK(
			std::any_cast<std::string>(decoded)
			==
			"lalala"
		);

	}

	SUBCASE("numbers") {

		SUBCASE("integer") {

			std::string s = "1234";
			std::any decoded = gorage::Json::decode(s);

			CHECK(std::any_cast<int>(decoded) == 1234);

		}

		SUBCASE("floating") {

			std::string s = "1234.1234";
			std::any decoded = gorage::Json::decode(s);

			CHECK(std::any_cast<double>(decoded) == 1234.1234);

		}

	}

	SUBCASE("list") {

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

	SUBCASE("dict") {

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


TEST_CASE("encoding to JSON") {

	SUBCASE("strings") {

		SUBCASE("`char*`") {

			const char s[7] = "lalala";
			const std::string encoded = gorage::Json::encode(s);

			testEncodedString(encoded);
			SUBCASE("hardcoded full comparison") {
				CHECK(encoded == "\"lalala\"");
			}

		}
		SUBCASE("`std::string`") {

			const std::string s("lalala");
			const std::string encoded = gorage::Json::encode(s);

			testEncodedString(encoded);
			SUBCASE("hardcoded full comparison") {
				CHECK(encoded == "\"lalala\"");
			}

		}
		SUBCASE("`gorage::Bytes`") {

			const gorage::Bytes s{'l', 'a', 'l', 'a', 'l', 'a'};
			const std::string encoded = gorage::Json::encode(s);

			testEncodedString(encoded);
			SUBCASE("hardcoded full comparison") {
				CHECK(encoded == "\"bGFsYWxh\"");
			}

		}

	}

	SUBCASE("numbers") {

		SUBCASE("testing `int`") {
			const int i = 1234;
			testEncodedNumber(gorage::Json::encode(i));
		}
		SUBCASE("testing `double`") {
			const double f = 1234.1234;
			testEncodedNumber(gorage::Json::encode(f));
		}

	}

	SUBCASE("testing lists") {

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

	SUBCASE("dictionaries") {

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


TEST_CASE("JSON stability") {

	SUBCASE("strings") {

		SUBCASE("->decoding->encoding->") {
			std::string s = "lalala";
			CHECK(
				std::any_cast<std::string>(
					gorage::Json::decode(gorage::Json::encode(s))
				)
				==
				s
			);
		}

		SUBCASE("->encoding->decoding->") {
			std::string json = "\"lalala\"";
			CHECK(
				gorage::Json::encode(gorage::Json::decode(json))
				==
				json
			);
		}

	}

	SUBCASE("integers") {

		SUBCASE("->decoding->encoding->") {
			int i = 1234;
			CHECK(
				std::any_cast<int>(
					gorage::Json::decode(gorage::Json::encode(i))
				)
				==
				i
			);
		}

		SUBCASE("->encoding->decoding->") {
			std::string json = "1234";
			CHECK(gorage::Json::encode(gorage::Json::decode(json)) == json);
		}

	}

	SUBCASE("floating numbers") {

		SUBCASE("->decoding->encoding->") {
			double i = 1234.1234;
			CHECK(
				std::any_cast<double>(
					gorage::Json::decode(gorage::Json::encode(i))
				)
				==
				i
			);
		}

		SUBCASE("->encoding->decoding->") {
			std::string json = "1234.123400";
			CHECK(gorage::Json::encode(gorage::Json::decode(json)) == json);
		}

	}

	SUBCASE("lists") {

		SUBCASE("->decoding->encoding->") {

			gorage::Json::List l{1, 2, 3};
			gorage::Json::List result = std::any_cast<gorage::Json::List>(
				gorage::Json::decode(gorage::Json::encode(l))
			);
			REQUIRE(l.size() == result.size());

			for (size_t i = 0; i < l.size(); i++) {
				CHECK(std::any_cast<int>(l[i]) == std::any_cast<int>(result[i]));
			}

		}

		SUBCASE("->encoding->decoding->") {
			std::string json = "[1,2,3]";
			CHECK(gorage::Json::encode(gorage::Json::decode(json)) == json);
		}

	}

	SUBCASE("dictionaries") {

		SUBCASE("->decoding->encoding->") {

			gorage::Json::Dict d{
				{"a", 1},
				{"b", 2},
				{"c", 3}
			};
			gorage::Json::Dict result = std::any_cast<gorage::Json::Dict>(
				gorage::Json::decode(gorage::Json::encode(d))
			);
			REQUIRE(d.size() == result.size());

			for (const auto& k_v : d) {
				CHECK(std::any_cast<int>(d[k_v.first]) == std::any_cast<int>(result[k_v.first]));
			}
			for (const auto& k_v : result) {
				CHECK(std::any_cast<int>(d[k_v.first]) == std::any_cast<int>(result[k_v.first]));
			}

		}

		SUBCASE("->encoding->decoding->") {
			std::string json = "{\"a\":1,\"b\":2,\"c\":3}";
			CHECK(gorage::Json::encode(gorage::Json::decode(json)) == json);
		}

	}

}