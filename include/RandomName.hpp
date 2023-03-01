#pragma once

#ifndef GORAGE__RANDOMNAME
#define GORAGE__RANDOMNAME

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <cmath>
#include <thread>
#include <algorithm>
#ifdef _WIN32
	#include <Windows.h>
#endif

namespace gorage {

	class RandomName {

	public:

		RandomName(size_t length) {

			const std::string symbols =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";

			#ifdef _WIN32
				std::srand((unsigned int)GetTickCount64());
			#else
				struct timespec t;
				clock_gettime(CLOCK_MONOTONIC, &t);
				std::srand((unsigned int)t.tv_nsec);
			#endif

			_s.reserve(length);
			for (size_t i = 0; i < length; i++) {
				int r = std::rand();
				_s += symbols[r % (symbols.length() - 1)];
			}

		}

		std::string str() const {
			return _s;
		}

	private:

		std::string _s;

	};

} // gorage

#endif