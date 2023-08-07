#pragma once

#include <regex>
#include <optional>
#include <iostream>

#include "Json.hpp"
#include "Bytes.hpp"
#include "RandomName.hpp"

#include "../modules/cppcodec/base32_rfc4648.hpp"



namespace gorage {

	class Usi {

	public:

		Usi():
			Usi(32) {}

		explicit Usi(const size_t& length):
			Usi(
				gorage::RandomName(length).str()
			) {}

		explicit Usi(const std::string& source, const std::optional<std::string>& comment = {}):
			_value(source) {
			if (
				std::regex_search(
					_value,
					std::regex("[^\\w|\\d|\\=]+")
				)
			) {
				throw std::runtime_error("Invalid usi \"" + _value + "\"" + (comment.has_value() ? (" " + *comment) : ""));
			}
		}

		explicit Usi(const gorage::Bytes& source):
			Usi(
				std::regex_replace(
					cppcodec::base32_rfc4648::encode(source),
					std::regex("="),
					""
				)
			) {}

		const std::string& value() const { return
			_value;
		}

		Bytes decoded() const { return
			cppcodec::base32_rfc4648::decode(_value);
		}

	private:
		std::string _value;

	};

} // gorage