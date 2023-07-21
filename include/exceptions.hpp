#pragma once

#include <regex>
#include <iostream>
#include <filesystem>
#include <unordered_set>

#include "Usi.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
#include "RandomName.hpp"



namespace gorage {

	namespace exceptions {

		class Base : public std::exception {

		public:

			Base() {}

			explicit Base(const std::string& message):
				_message(message) {}

			const char* what() const noexcept override {
				return &_message[0];
			}

		private:
			std::string _message;

		};

	} // exceptions

} // gorage