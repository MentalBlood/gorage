#pragma once

#include <string>


namespace gorage {
	namespace exceptions {
		class Base : public std::exception {
		public:
			std::string message;

			Base() {}
			explicit Base(const std::string& message): message(message) {}

			const char* what() const noexcept override { return message.c_str(); }
		};
		class NotImplemented : public Base {
		public:
			NotImplemented(const std::string& class_, const std::string& method): Base("Method `" + method + "` not implemented for class `" + class_ + "`") {}
		};
	} // exceptions
} // gorage