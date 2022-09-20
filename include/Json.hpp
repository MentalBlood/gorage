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

		using List = std::vector<std::any>;
		using Dict = std::unordered_map<std::string, std::any>;
		using String = std::variant<std::string, gorage::Bytes>;
		using Number = std::variant<int, float, double>;

		static std::string _toJson(const std::any& a) {

			std::cout << "std::any" << std::endl;

			try {
				return _toJson(std::any_cast<String>(a));
			} catch (const std::bad_any_cast& e) {};
			try {
				return _toJson(std::any_cast<Number>(a));
			} catch (const std::bad_any_cast& e) {};

			throw std::runtime_error("Can not encode type " + std::string(a.type().name()) + " to JSON");

		}

		static std::string _toJson(const String& s) {

			std::cout << "Json::String" << std::endl;

			std::string std_string;

			try {
				std_string = std::get<std::string>(s);
			} catch (std::bad_variant_access& e) {}
			try {
				std_string = cppcodec::base64_rfc4648::encode(std::get<gorage::Bytes>(s));
			} catch (std::bad_variant_access& e) {}

			if (std_string.length()) {
				return "\"" + std_string + "\"";
			}

			throw std::runtime_error("Can not encode string variant type of index " + std::to_string(s.index()) + " to JSON");

		}
		static std::string _toJson(const Number& i) {

			std::cout << "Json::Number" << std::endl;

			std::string number_string;

			try {
				number_string = std::to_string(std::get<int>(i));
			} catch (std::bad_variant_access& e) {}
			try {
				number_string = std::to_string(std::get<float>(i));
			} catch (std::bad_variant_access& e) {}
			try {
				number_string = std::to_string(std::get<double>(i));
			} catch (std::bad_variant_access& e) {}

			if (number_string.length()) {
				return number_string;
			}

			throw std::runtime_error("Can not encode number variant type of index " + std::to_string(i.index()) + " to JSON");

		}

		static std::string _toJson(const List& v) {

			std::cout << "std::vector" << std::endl;

			std::string result;

			result += "[";
			for (const auto& e : v) {
				result += _toJson(e) + ",";
			}
			result[result.length() - 1] = ']';

			return result;

		}

		static std::string _toJson(const Dict& m) {

			std::cout << "std::unordered_map" << std::endl;

			std::string result;

			result += "{";
			for (const auto& pair : m) {
				result += _toJson(pair.first) + ":" + _toJson(pair.second) + ",";
			}
			result[result.length() - 1] = '}';

			return result;

		}

	};

} // gorage

#endif