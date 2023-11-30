#include <random>

#include "../include/random.hpp"



namespace gorage {
	namespace random {
		const std::string Name::alphabet =	"0123456789"
											"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
											"abcdefghijklmnopqrstuvwxyz";
		unsigned int seed() {
			#ifdef _WIN32
				return (unsigned int)GetTickCount64();
			#else
				struct timespec t;
				clock_gettime(CLOCK_MONOTONIC, &t);
				return (unsigned int)t.tv_sec;
			#endif
		}
		void shuffle() { generator.seed(seed()); }

		gorage::Bytes bytes(const size_t& amount) {
			gorage::Bytes result;
			for (size_t i = 0; i < amount; i++) result.push_back(generator() % 256);
			return result;
		}
	}
} // gorage