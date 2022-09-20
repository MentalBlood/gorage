#pragma once

#ifndef __GORAGE__JSON__
#define __GORAGE__JSON__

#include <any>
#include <map>
#include <vector>
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

		template<typename T>
		static std::string _toJson(const T& t) {
			std::cout << "unknown" << std::endl;
			return "";
		}

		template<>
		static std::string _toJson<std::any>(const std::any& a) {

			std::cout << "std::any" << std::endl;

			try {
				return _toJson(std::any_cast<std::string>(a));
			} catch (const std::bad_any_cast& e) {};
			try {
				return _toJson(std::any_cast<int>(a));
			} catch (const std::bad_any_cast& e) {};
			try {
				return _toJson(std::any_cast<float>(a));
			} catch (const std::bad_any_cast& e) {};
			try {
				return _toJson(std::any_cast<double>(a));
			} catch (const std::bad_any_cast& e) {};

			throw std::runtime_error("Can not encode type " + std::string(a.type().name()) + " to JSON");

		}

		template<>
		static std::string _toJson<std::string>(const std::string& s) {
			std::cout << "gorage::Bytes" << std::endl;
			return "\"" + s + "\"";
		}
		template<>
		static std::string _toJson<int>(const int& i) {
			std::cout << "gorage::Bytes" << std::endl;
			return "\"" + std::to_string(i) + "\"";
		}
		template<>
		static std::string _toJson<float>(const float& f) {
			std::cout << "gorage::Bytes" << std::endl;
			return "\"" + std::to_string(f) + "\"";
		}
		template<>
		static std::string _toJson<double>(const double& d) {
			std::cout << "gorage::Bytes" << std::endl;
			return "\"" + std::to_string(d) + "\"";
		}
		template<>
		static std::string _toJson<gorage::Bytes>(const gorage::Bytes& b) {
			std::cout << "gorage::Bytes" << std::endl;
			return "\"" + cppcodec::base64_rfc4648::encode(b) + "\"";
		}

		template<typename T>
		static std::string _toJson(const std::vector<T>& v) {

			std::cout << "std::vector" << std::endl;

			std::string result;

			result += "[";
			for (const auto& e : v) {
				result += _toJson<T>(e) + ",";
			}
			result[result.length() - 1] = ']';

			return result;

		}

		template<typename T>
		static std::string _toJson(const std::unordered_map<std::string, T>& m) {

			std::cout << "std::unordered_map" << std::endl;

			std::string result;

			result += "{";
			for (const auto& pair : m) {
				result += _toJson<std::string>(pair.first) + ":" + _toJson<T>(pair.second) + ",";
			}
			result[result.length() - 1] = '}';

			return result;

		}

	};

} // gorage

#endif