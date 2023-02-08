#pragma once

#ifndef __GORAGE__JSON__
#define __GORAGE__JSON__

#include <set>
#include <map>
#include <any>
#include <regex>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "../modules/rapidjson/document.h"
#include "../modules/cppcodec/base64_rfc4648.hpp"

#include "Bytes.hpp"



namespace gorage {

	/**
	 * @brief Class for representing JSONable objects
	 *
	 */
	class Json {

	public:

		// using Bytes = Bytes;

		/**
		 * @brief Generic list
		 *
		 */
		using List = std::vector<std::any>;
		/**
		 * @brief Generic map
		 *
		 */
		using Dict = std::map<std::string, std::any>;
		/**
		 * @brief Class for storing optionally base64-decodable string
		 *
		 */
		class String {

		public:

			const std::string s;
			const Bytes b;

			String() {}

			String(const std::string& s):
				s(s) {}

			String(const Bytes& b):
				b(b) {}

			std::string encoded() {
				if (s.length()) {
					return s;
				}
				return cppcodec::base64_rfc4648::encode(b);
			}

			Bytes decoded() {
				if (b.size()) {
					return b;
				}
				return cppcodec::base64_rfc4648::decode(s);
			}

		};

		static std::string hex(const Bytes& bytes) {

			std::stringstream result_stream;

			result_stream << std::hex << std::setw(2) << std::setfill('0');
			for (const auto& b : bytes) {
				result_stream << ((unsigned char)b & 0xff);
			}

			return result_stream.str();

		}

		template<typename T>
		static T get(const Dict& d, const std::string& key, const T& _default) {
			if (d.count(key)) {
				return std::any_cast<T>(d.at(key));
			}
			return _default;
		}

		template<typename T>
		static T getObject(const Dict& d, const std::string& key, const T& _default) {
			if (d.count(key)) {
				return create<T>(d.at(key));
			}
			return _default;
		}

		/**
		 * @brief JSON text decoding to generic object
		 *
		 * @param json_text
		 * @return std::any May be one of: `std::string`, `int`, `double`, `List`, `Dict`
		 */
		static std::any decode(const std::string& json_text) {

			rapidjson::Document json;
			json.Parse(json_text.c_str());

			return _decode<rapidjson::Document>(json);

		}

		/**
		 * @brief Generic object encoding to JSON text
		 *
		 * @param a May be one of: `std::string`, `char*`, `int`, `double`, `List`, `Dict`
		 * @return std::string
		 */
		static std::string encode(const std::any& a) {

			if (a.type() == typeid(const char*)) {
				return encode(std::any_cast<const char*>(a));
			}
			if (a.type() == typeid(String)) {
				return encode(std::any_cast<String>(a));
			}
			if (a.type() == typeid(std::string)) {
				return encode(std::any_cast<std::string>(a));
			}
			if (a.type() == typeid(Bytes)) {
				return encode(std::any_cast<Bytes>(a));
			}

			if (a.type() == typeid(int)) {
				return encode(std::any_cast<int>(a));
			}
			if (a.type() == typeid(float)) {
				return encode(std::any_cast<float>(a));
			}
			if (a.type() == typeid(double)) {
				return encode(std::any_cast<double>(a));
			}


			if (a.type() == typeid(List)) {
				return encode<std::any>(std::any_cast<List>(a));
			}
			if (a.type() == typeid(std::vector<std::string>)) {
				return encode<std::string>(std::any_cast<std::vector<std::string>>(a));
			}
			if (a.type() == typeid(std::vector<Bytes>)) {
				return encode<Bytes>(std::any_cast<std::vector<Bytes>>(a));
			}
			if (a.type() == typeid(std::vector<int>)) {
				return encode<int>(std::any_cast<std::vector<int>>(a));
			}
			if (a.type() == typeid(std::vector<float>)) {
				return encode<float>(std::any_cast<std::vector<float>>(a));
			}
			if (a.type() == typeid(std::vector<double>)) {
				return encode<double>(std::any_cast<std::vector<double>>(a));
			}

			if (a.type() == typeid(Dict)) {
				return encode(std::any_cast<Dict>(a));
			}

			throw std::runtime_error("Can not encode type " + std::string(a.type().name()) + " to JSON");

		}

		/**
		 * @brief C string encoding to JSON text
		 *
		 * @param s
		 * @return std::string
		 */
		static std::string encode(const char* s) {
			return "\"" + _getEscaped(s) + "\"";
		}
		/**
		 * @brief `String` encoding to JSON text
		 *
		 * @param s
		 * @return std::string
		 */
		static std::string encode(const String& s) {
			return "\"" + _getEscaped(s.s) + "\"";
		}
		/**
		 * @brief C++ string encoding to JSON text
		 *
		 * @param s
		 * @return std::string
		 */
		static std::string encode(const std::string& s) {
			return "\"" + _getEscaped(s) + "\"";
		}
		/**
		 * @brief Bytes encoding to JSON text using base64 rfc4648
		 *
		 * @param s
		 * @return std::string
		 */
		static std::string encode(const Bytes& s) {
			return "\"" + cppcodec::base64_rfc4648::encode(s) + "\"";
		}

		/**
		 * @brief integer encoding to JSON text
		 *
		 * @param i
		 * @return std::string
		 */
		static std::string encode(const int& i) {
			return std::to_string(i);
		}
		/**
		 * @brief Floating number encoding to JSON text
		 *
		 * @param f
		 * @return std::string
		 */
		static std::string encode(const float& f) {
			return std::to_string(f);
		}
		/**
		 * @brief Floating number encoding to JSON text
		 *
		 * @param d
		 * @return std::string
		 */
		static std::string encode(const double& d) {
			return std::to_string(d);
		}

		/**
		 * @brief Generic iterable (vector, set etc.) encoding to JSON text
		 *
		 * @param v
		 * @return std::string
		 */
		template<typename T>
		static std::string encodeIterable(const T& v) {

			if (!v.size()) {
				return "[]";
			}

			std::string result;

			result += "[";
			for (const auto& e : v) {
				result += encode(e) + ",";
			}
			result[result.length() - 1] = ']';

			return result;

		}

		/**
		 * @brief Generic list encoding to JSON text
		 *
		 * @param v
		 * @return std::string
		 */
		template<typename T>
		static std::string encode(const std::vector<T>& v) {
			return encodeIterable<std::vector<T>>(v);
		}

		/**
		 * @brief Generic set encoding to JSON text
		 *
		 * @param v
		 * @return std::string
		 */
		template<typename T>
		static std::string encode(const std::set<T>& v) {
			return encodeIterable<std::set<T>>(v);
		}

		/**
		 * @brief Generic map encoding to JSON text
		 *
		 * @param m
		 * @return std::string
		 */
		static std::string encode(const Dict& m) {

			if (!m.size()) {
				return "{}";
			}

			std::string result;

			result += "{";
			for (const auto& pair : m) {
				result += "\"" + pair.first + "\"" ":" + encode(pair.second) + ",";
			}
			result[result.length() - 1] = '}';

			return result;

		}

		/**
		 * @brief Converts JSON text to object of given type
		 *
		 * @tparam T Object type
		 * @param json_text JSON text
		 * @return T Object corresponded to JSON text
		 */
		template<class T>
		static T createFromJson(const std::string& json_text) {
			T json;
			json.update(decode(json_text));
			return json;
		}

		/**
		 * @brief Converts structure to object of given type
		 *
		 * @tparam T Object type
		 * @param structure structure
		 * @return T Object corresponded to structure
		 */
		template<class T>
		static T create(const std::any& structure) {
			T json;
			json.update(structure);
			return json;
		}

		/**
		 * @brief Updates data and metadata from corresponding JSON text
		 *
		 * @param json_text JSON text to update from
		 */
		virtual void update(const std::any& json_text) = 0;
		/**
		 * @brief Converts object to JSON
		 *
		 * @return std::string JSONed object
		 */
		std::string encoded() const {
			return encode(getStructure());
		}

		/**
		 * @brief Check if corresponding structure contains given structure at given key
		 *
		 * @param key Direct key on which given structure contained
		 * @param structure Structure to search
		 * @return true `structure` found on direct key `key`
		 * @return false `structure` not found on direct key `key`
		 */
		bool contains(const std::string& key, const std::any& structure) const {
			return contains(getStructure(), key, structure);
		}

		/**
		 * @brief Check if given structure contains other given structure at given key
		 *
		 * @param haystack Structure to search in
		 * @param key Direct key at which given structure contained
		 * @param needle Structure to search for
		 * @return true `needle` found in `haystack` at direct key `key`
		 * @return false `needle` not found in `haystack` at direct key `key`
		 */
		static bool contains(const std::any& haystack, const std::string& key, const std::any& needle) {
			return std::regex_search(
				encode(haystack),
				std::regex(
					std::regex_replace(
						"\"" + key + "\" *: *" + encode(needle) + "(?:\\n| )*(?:,|})",
						std::regex("\\[|\\]|\\{|\\}|\\/|\\+"),
						"\\$&"
					)
				)
			);
		}

		virtual std::any getStructure() const = 0;

	private:

		template<typename T>
		static std::any _decode(const T& v) {

			if (v.IsNumber()) {
				if (v.IsInt()) {
					return v.GetInt();
				}
				if (v.IsDouble()) {
					return v.GetDouble();
				}
			}
			if (v.IsString()) {
				return String(std::string(v.GetString()));
			}
			if (v.IsArray()) {
				List result;
				for (const auto& e : v.GetArray()) {
					result.push_back(_decode<rapidjson::Value>(e));
				}
				return result;
			}
			if (v.IsObject()) {
				Dict result;
				for (const auto& k_v : v.GetObject()) {
					result[k_v.name.GetString()] = _decode<rapidjson::Value>(k_v.value);
				}
				return result;
			}

			throw std::runtime_error("Can not decode value of type indexed " + v.GetType());

		}

		static std::string _getEscaped(const std::string& s) {
			return std::regex_replace(
				std::regex_replace(s, std::regex("\\\\"), "\\\\"),
				std::regex("\""),
				"\\\""
			);
		}

	};

} // gorage

#endif