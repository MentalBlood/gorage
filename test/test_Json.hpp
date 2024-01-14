#include <regex>

#include "../include/Json.hpp"
#include "doctest.h"

TEST_CASE("decoding from JSON") {
  SUBCASE("strings") {
    const auto s = "\"lalala\"";
    const auto decoded = gorage::Json::decode(s);
    CHECK_EQ(std::any_cast<gorage::Json::String>(decoded).s, "lalala");
  }
  SUBCASE("numbers") {
    SUBCASE("integer") {
      const auto s = "1234";
      const auto decoded = gorage::Json::decode(s);
      CHECK_EQ(std::any_cast<int>(decoded), 1234);
    }
    SUBCASE("floating") {
      const auto s = "1234.1234";
      const auto decoded = gorage::Json::decode(s);
      CHECK_EQ(std::any_cast<double>(decoded), 1234.1234);
    }
  }
  SUBCASE("list") {
    const auto s = "[\"lalala\",1234,1234.123400]";
    const auto decoded = std::any_cast<gorage::Json::List>(gorage::Json::decode(s));

    CHECK_EQ(std::any_cast<gorage::Json::String>(decoded[0]).s, "lalala");
    CHECK_EQ(std::any_cast<int>(decoded[1]), 1234);
    CHECK_EQ(std::any_cast<double>(decoded[2]), 1234.1234);
  }
  SUBCASE("dict") {
    const auto s = "{\"double\":1234.123400,\"string\":\"lalala\",\"int\":1234}";
    const auto decoded = std::any_cast<gorage::Json::Dict>(gorage::Json::decode(s));

    CHECK_EQ(std::any_cast<gorage::Json::String>(decoded.at("string")).s, "lalala");
    CHECK_EQ(std::any_cast<int>(decoded.at("int")), 1234);
    CHECK_EQ(std::any_cast<double>(decoded.at("double")), 1234.1234);
  }
}
inline void testEncodedString(const std::string &encoded) {
  SUBCASE("encoded string must be surrounded by double quotes") {
    REQUIRE(encoded.length() > 0);

    CHECK_EQ(encoded[0], '\"');
    CHECK_EQ(encoded[encoded.length() - 1], '\"');
  }
  SUBCASE("encoded string `\\` characters must be escaped") {
    CHECK_FALSE(std::regex_match(encoded, std::regex("[^\\\\]\\\\(?:[^\\\\]|$)")));
  }
}
inline void testEncodedNumber(const std::string &encoded) {
  SUBCASE("encoded number must contain only `1234567890.` characters") {
    CHECK_FALSE(std::regex_match(encoded, std::regex("[^1234567890.]")));
  }
}
TEST_CASE("encoding to JSON") {
  SUBCASE("strings") {
    SUBCASE("`char*`") {
      const char s[7] = "lalala";
      const auto encoded = gorage::Json::encode(s);

      testEncodedString(encoded);
      SUBCASE("hardcoded full comparison") { CHECK_EQ(encoded, "\"lalala\""); }
    }
    SUBCASE("`std::string`") {
      const std::string s("lalala");
      const std::string encoded = gorage::Json::encode(s);

      testEncodedString(encoded);
      SUBCASE("hardcoded full comparison") { CHECK_EQ(encoded, "\"lalala\""); }
    }
    SUBCASE("`gorage::Bytes`") {
      const gorage::Bytes s{'l', 'a', 'l', 'a', 'l', 'a'};
      const std::string encoded = gorage::Json::encode(s);

      testEncodedString(encoded);
      SUBCASE("hardcoded full comparison") { CHECK_EQ(encoded, "\"bGFsYWxh\""); }
    }
  }
  SUBCASE("numbers") {
    SUBCASE("`int`") {
      const int i = 1234;
      testEncodedNumber(gorage::Json::encode(i));
    }
    SUBCASE("`double`") {
      const double f = 1234.1234;
      testEncodedNumber(gorage::Json::encode(f));
    }
  }
  SUBCASE("lists") {
    SUBCASE("empty list") { CHECK_EQ(gorage::Json::encode(gorage::Json::List{}), "[]"); }

    const gorage::Json::List l({"lalala", 1234, 1234.1234});
    const auto encoded = gorage::Json::encode(l);

    SUBCASE("encoded list must be surrounded by square brackets `[]`") {
      REQUIRE(l.size() > 0);
      CHECK_EQ(encoded[0], '[');
      CHECK_EQ(encoded[encoded.length() - 1], ']');
    }
    SUBCASE("hardcoded full comparison") { CHECK_EQ(encoded, "[\"lalala\",1234,1234.123400]"); }
  }
  SUBCASE("vectors of strings") {
    SUBCASE("empty vector") { CHECK_EQ(gorage::Json::encode(std::vector<std::string>{}), "[]"); }

    const std::vector<std::string> l({"lalala", "1234", "1234.1234"});
    const auto encoded = gorage::Json::encode(l);

    SUBCASE("encoded vector must be surrounded by square brackets `[]`") {
      REQUIRE(l.size() > 0);
      CHECK_EQ(encoded[0], '[');
      CHECK_EQ(encoded[encoded.length() - 1], ']');
    }
    SUBCASE("hardcoded full comparison") { CHECK_EQ(encoded, "[\"lalala\",\"1234\",\"1234.1234\"]"); }
  }
  SUBCASE("dictionaries") {
    SUBCASE("empty dict") { CHECK_EQ(gorage::Json::encode(gorage::Json::Dict{}), "{}"); }

    const gorage::Json::Dict l({{"string", "lalala"}, {"int", 1234}, {"double", 1234.1234}});
    const auto encoded = gorage::Json::encode(l);

    SUBCASE("encoded dict must be surrounded by square brackets `{}`") {
      REQUIRE(l.size() > 0);
      CHECK_EQ(encoded[0], '{');
      CHECK_EQ(encoded[encoded.length() - 1], '}');
    }
    SUBCASE("hardcoded full comparison") {
      CHECK_EQ(encoded, "{\"double\":"
                        "1234.123400,\"int\":"
                        "1234,\"string\":"
                        "\"lalala\"}");
    }
  }
}
TEST_CASE("JSON stability") {
  SUBCASE("strings") {
    SUBCASE("->decoding->encoding->") {
      const auto s = "lalala";
      CHECK_EQ(std::any_cast<gorage::Json::String>(gorage::Json::decode(gorage::Json::encode(s))).s, s);
    }
    SUBCASE("->encoding->decoding->") {
      const auto json = "\"lalala\"";
      CHECK_EQ(gorage::Json::encode(gorage::Json::decode(json)), json);
    }
  }
  SUBCASE("integers") {
    SUBCASE("->decoding->encoding->") {
      const auto i = 1234;
      CHECK_EQ(std::any_cast<int>(gorage::Json::decode(gorage::Json::encode(i))), i);
    }
    SUBCASE("->encoding->decoding->") {
      const auto json = "1234";
      CHECK_EQ(gorage::Json::encode(gorage::Json::decode(json)), json);
    }
  }
  SUBCASE("floating numbers") {
    SUBCASE("->decoding->encoding->") {
      const auto i = 1234.1234;
      CHECK_EQ(std::any_cast<double>(gorage::Json::decode(gorage::Json::encode(i))), i);
    }
    SUBCASE("->encoding->decoding->") {
      const auto json = "1234.123400";
      CHECK_EQ(gorage::Json::encode(gorage::Json::decode(json)), json);
    }
  }
  SUBCASE("lists") {
    SUBCASE("->decoding->encoding->") {
      gorage::Json::List l({1, 2, 3});
      const auto result = std::any_cast<gorage::Json::List>(gorage::Json::decode(gorage::Json::encode(l)));
      REQUIRE(l.size() == result.size());

      for (size_t i = 0; i < l.size(); i++)
        CHECK_EQ(std::any_cast<int>(l[i]), std::any_cast<int>(result[i]));
    }
    SUBCASE("->encoding->decoding->") {
      const auto json = "[1,2,3]";
      CHECK_EQ(gorage::Json::encode(gorage::Json::decode(json)), json);
    }
  }
  SUBCASE("dictionaries") {
    SUBCASE("->decoding->encoding->") {
      gorage::Json::Dict d({{"a", 1}, {"b", 2}, {"c", 3}});
      const auto result = std::any_cast<gorage::Json::Dict>(gorage::Json::decode(gorage::Json::encode(d)));
      REQUIRE(d.size() == result.size());

      for (const auto &k_v : d)
        CHECK_EQ(std::any_cast<int>(d.at(k_v.first)), std::any_cast<int>(result.at(k_v.first)));
      for (const auto &k_v : result)
        CHECK_EQ(std::any_cast<int>(d.at(k_v.first)), std::any_cast<int>(result.at(k_v.first)));
    }
    SUBCASE("->encoding->decoding->") {
      const auto json = "{\"a\":1,\"b\":2,\"c\":3}";
      CHECK_EQ(gorage::Json::encode(gorage::Json::decode(json)), json);
    }
  }
}
