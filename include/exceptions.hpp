#pragma once

#include <regex>
#include <iostream>
#include <filesystem>
#include <unordered_set>

#include "Usi.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
#include "random.hpp"



namespace gorage {

	namespace exceptions {

		class Base : public std::exception {

		public:

			Base() {}

			explicit Base(const std::string& message):
				_message(message) {}

			const char* what() const noexcept override {
				return _message.c_str();
			}

		private:
			std::string _message;

		};

		class NotImplemented : public Base {
		public:
			NotImplemented(const std::string& class_, const std::string& method):
				Base("Method `" + method + "` not implemented for class `" + class_ + "`") {}
		};


	} // exceptions

} // gorage