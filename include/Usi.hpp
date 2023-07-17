#pragma once

#include <regex>
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

		explicit Usi(const std::string& source):
			_value(source) {
			if (
				std::regex_search(
					_value,
					std::regex("[^\\w|\\d|\=]+")
				)
			) {
				throw std::runtime_error("Invalid usi \"" + _value + "\"");
			}
		}

		explicit Usi(const gorage::Bytes& source):
			Usi(
				cppcodec::base32_rfc4648::encode(source)
			) {}

		std::string value() const { return
			_value;
		}

	private:
		std::string _value;

	};

} // gorage