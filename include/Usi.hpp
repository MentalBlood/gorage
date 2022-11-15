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
			s(s) {}
		Usi(const Bytes& b):
			s(
				std::regex_replace(
					Json::String(b).encoded(),
					std::regex("[^\\w|\\d]"),
					"_"
				)
			) {}
		Usi(const size_t& length):
			s(
				_random(length)
			) {}

		std::string operator()() const {
			return s;
		}

	private:

		std::string s;

		static std::string _random(const size_t& length) {

			static const std::string symbols = 
				"0123456789" 
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
				"abcdefghijklmnopqrstuvwxyz";

			std::srand(GetTickCount64());

			std::string result;
			result.reserve(length);
			for (size_t i = 0; i < length; i++) {
				result += symbols[rand() % (symbols.length() - 1)];
			}

			return result;

		}

	};

} // gorage

#endif