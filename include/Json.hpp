#pragma once

#ifndef __GORAGE__JSON__
#define __GORAGE__JSON__

#include <any>
#include <map>
#include <vector>
#include <variant>
#include <iostream>
#include <unordered_map>

#include "gorage.hpp"



namespace gorage {

	/**
	 * @brief Class for representing JSONable objects
	 * 
	 */
	class Json {

	public:

		using List = std::vector<std::any>;
		using Dict = std::unordered_map<std::string, std::any>;

		/**
		 * @brief Updates data and metadata from corresponding JSON text
		 * 
		 * @param json_text JSON text to update from
		 */
		virtual void updateFromJson(const std::string& json_text) = 0;
		/**
		 * @brief Converts object to JSON
		 * 
		 * @return std::string JSONed object
		 */
		virtual std::string toJson() const = 0;

		/**
		 * @brief Converts JSON text to object of given type
		 * 
		 * @tparam T Object type
		 * @param json_text JSON text
		 * @return T Object corresponded to JSON text
		 */
		template<typename T>
		static T create(const std::string& json_text) {
			T json;
			json.updateFromJson(json_text);
			return json;
		}

	protected:

		static std::string _toJson(const std::any& a) {

			std::cout << "std::any" << std::endl;

			try {
				return _toJson(std::any_cast<const char*>(a));
			} catch (std::bad_any_cast& e) {}
			try {
				return _toJson(std::any_cast<std::string>(a));
			} catch (std::bad_any_cast& e) {}
			try {
				return _toJson(std::any_cast<gorage::Bytes>(a));
			} catch (std::bad_any_cast& e) {}

			try {
				return _toJson(std::any_cast<int>(a));
			} catch (std::bad_any_cast& e) {}
			try {
				return _toJson(std::any_cast<float>(a));
			} catch (std::bad_any_cast& e) {}
			try {
				return _toJson(std::any_cast<double>(a));
			} catch (std::bad_any_cast& e) {}


			try {
				return _toJson(std::any_cast<List>(a));
			} catch (const std::bad_any_cast& e) {};
			try {
				return _toJson(std::any_cast<Dict>(a));
			} catch (const std::bad_any_cast& e) {};

			throw std::runtime_error("Can not encode type " + std::string(a.type().name()) + " to JSON");

		}

		static std::string _toJson(const char* s) {
			std::cout << "char*" << std::endl;
			return "\"" + std::string(s) + "\"";
		}
		static std::string _toJson(const std::string& s) {
			std::cout << "std::string" << std::endl;
			return "\"" + s + "\"";
		}
		static std::string _toJson(const gorage::Bytes& s) {
			std::cout << "gorage::Bytes" << std::endl;
			return "\"" + cppcodec::base64_rfc4648::encode(s) + "\"";
		}

		static std::string _toJson(const int& i) {
			std::cout << "int" << std::endl;
			return std::to_string(i);
		}
		static std::string _toJson(const float& f) {
			std::cout << "float" << std::endl;
			return std::to_string(f);
		}
		static std::string _toJson(const double& d) {
			std::cout << "double" << std::endl;
			return std::to_string(d);
		}

		static std::string _toJson(const List& v) {

			std::cout << "Json::List" << std::endl;

			std::string result;

			result += "[";
			for (const auto& e : v) {
				result += _toJson(e) + ",";
			}
			result[result.length() - 1] = ']';

			return result;

		}

		static std::string _toJson(const Dict& m) {

			std::cout << "Json::Dict" << std::endl;

			std::string result;

			result += "{";
			for (const auto& pair : m) {
				result += "\"" + pair.first + "\"" ":" + _toJson(pair.second) + ",";
			}
			result[result.length() - 1] = '}';

			return result;

		}

	};

} // gorage

#endif