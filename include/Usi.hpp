#pragma once

#ifndef __GORAGE__USI__
#define __GORAGE__USI__

#include <regex>
#include <iostream>
#include "windows.h"

#include "Json.hpp"
#include "Bytes.hpp"



namespace gorage {

	class Usi {

	public:

		Usi():
			Usi(32) {}

		Usi(const std::string& s):
			_s(
				std::regex_replace(
					s,
					std::regex("[^\\w|\\d]"),
					"_"
				)
			) {}

		Usi(const Bytes& b):
			Usi(
				Json::String(b).encoded()
			) {}

		Usi(const size_t& l):
			Usi(
				_random(l)
			) {}

		std::string operator()() const {
			return _s;
		}

	private:

		std::string _s;

		static const std::string _characters;

		std::string _random(const size_t& length) {

			std::srand(GetTickCount64());

			std::string result;
			for (size_t i = 0; i < length; i++) {
				result.push_back(_characters[rand() % _characters.length()]);
			}

			return result;

		}

	};

	const std::string Usi::_characters = 
		"0123456789" 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
		"abcdefghijklmnopqrstuvwxyz";

} // gorage

#endif