#pragma once

#include <regex>
#include <iostream>

#include "Json.hpp"
#include "Bytes.hpp"
#include "RandomName.hpp"



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
			_value(source) {}

		explicit Usi(const gorage::Bytes& source):
			_value(
				std::regex_replace(
					Json::String(source).encoded(),
					std::regex("[^\\w|\\d]"),
					"_"
				)
			) {}

		std::string operator()() const {
			return _value;
		}

	private:
		const std::string _value;

	};

} // gorage