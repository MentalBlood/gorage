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

	class Seed {

	public:

		Seed() {}

		unsigned int operator()() {
			_last_tick_count = _tick_count();
			while (true) {
				const unsigned int tick_count = _tick_count();
				if (tick_count != _last_tick_count) {
					_last_tick_count = tick_count;
					return tick_count;
				}
			}
		}

	private:

		unsigned int _last_tick_count;

		static unsigned int _tick_count() {
			#ifdef _WIN32
				return (unsigned int)GetTickCount64();
			#else
				struct timespec t;
				clock_gettime(CLOCK_MONOTONIC, &t);
				return (unsigned int)t.tv_nsec;
			#endif
		}

	};

	class RandomName {

	public:

		RandomName(const size_t length):
			_alphabet(
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
			) {

			srand(Seed()());

			_s.reserve(length);
			for (size_t i = 0; i < length; i++) {
				_s += _alphabet[std::rand() % (_alphabet.length() - 1)];
			}

		}

		std::string str() const {
			return _s;
		}

	private:

		std::string _s;

		const std::string _alphabet;

	};

} // gorage

#endif