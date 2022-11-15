#pragma once

#ifndef __GORAGE__KEY__
#define __GORAGE__KEY__

#include <regex>
#include <iostream>
#include "windows.h"

#include "Json.hpp"
#include "Bytes.hpp"



namespace gorage {

	class Key {

	public:

		Key():
			Key(32) {}

		Key(const std::string& s):
			_s(
				std::regex_replace(
					s,
					std::regex("[^\\w|\\d]"),
					"_"
				)
			) {}

		Key(const Bytes& b):
			Key(
				Json::String(b).encoded()
			) {}

		Key(const size_t& l):
			Key(
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

	const std::string Key::_characters = 
		"0123456789" 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
		"abcdefghijklmnopqrstuvwxyz";

} // gorage

#endif