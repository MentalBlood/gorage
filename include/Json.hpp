#pragma once

#include <set>
#include <map>
#include <any>
#include <regex>
#include <vector>
#include <optional>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "../modules/rapidjson/document.h"
#include "../modules/cppcodec/base64_rfc4648.hpp"

#include "Bytes.hpp"


namespace gorage {
	class Json {
	public:
		using List = std::vector<std::any>;
		using Dict = std::map<std::string, std::any>;

		class String {
		public:
			std::string s;
			Bytes b;

			String() {}
			explicit String(const std::string& s): s(s) {}
			explicit String(const Bytes& b): b(b) {}

			std::string encoded() const {
				if (s.length()) return s;
				return cppcodec::base64_rfc4648::encode(b);
			}
			Bytes decoded() const {
				if (b.size()) return b;
				return cppcodec::base64_rfc4648::decode(s);
			}
			std::string hex() const {
				std::string result;

				char buffer[3];
				for (const auto& i : b) {
					sprintf(buffer, "%02X", i);
					result += buffer;
				}
				return result;
			}
		};
		class Structure {
		public:
			Structure(const std::any& value): _value(value) {}
			const std::any& value() const { return _value; }
		private:
			std::any _value;
		};
		static std::string type_name(const std::type_info& type) {
			if (type == typeid(String)) return "String";
			if (type == typeid(std::string)) return "std::string";
			if (type == typeid(int)) return "int";
			if (type == typeid(double)) return "double";
			if (type == typeid(Bytes)) return "Bytes";
			if (type == typeid(bool)) return "bool";
			if (type == typeid(List)) return "List";
			if (type == typeid(Dict)) return "Dict";
			return "unknown";
		}
		class CastError : public std::runtime_error {
		public:
			explicit CastError(const std::string& name, const std::any& a): std::runtime_error(("Can not cast " + name + " to " + type_name(a.type())).c_str()) {}
		};
		class KeyError : public std::runtime_error {
		public:
			explicit KeyError(const std::string& key): std::runtime_error(("No key \"" + key + "\" found").c_str()) {}
		};
		template<typename T>
		static T cast(const std::any& a, const std::string& name = "something") {
			try { return std::any_cast<T>(a); }
			catch (const std::exception&) { throw CastError(name, a); }
		}
		template<typename T>
		static T get(const Dict& d, const std::string& key, const std::optional<T>& _default = {}) {
			if (d.count(key)) return cast<T>(d.at(key), key);
			if (_default) return *_default;
			throw KeyError(key);
		}
		template<typename T>
		static T get_object(const Dict& d, const std::string& key, const std::optional<T>& _default = {}) {
			if (d.count(key)) return T(d.at(key));
			if (_default) return *_default;
			throw KeyError(key);
		}
		static std::any decode(const std::string& json_text) {
			rapidjson::Document json;
			json.Parse(json_text.c_str());
			return _decode<rapidjson::Document>(json);
		}
		static std::string encode(const std::any& a) {
			if (a.type() == typeid(const char*)) return encode(std::any_cast<const char*>(a));
			if (a.type() == typeid(String)) return encode(std::any_cast<String>(a));
			if (a.type() == typeid(std::string)) return encode(std::any_cast<std::string>(a));
			if (a.type() == typeid(Bytes)) return encode(std::any_cast<Bytes>(a));
			if (a.type() == typeid(int)) return encode(std::any_cast<int>(a));
			if (a.type() == typeid(float)) return encode(std::any_cast<float>(a));
			if (a.type() == typeid(double)) return encode(std::any_cast<double>(a));
			if (a.type() == typeid(bool)) return encode(std::any_cast<bool>(a));
			if (a.type() == typeid(List)) return encode<std::any>(std::any_cast<List>(a));
			if (a.type() == typeid(std::vector<std::string>)) return encode<std::string>(std::any_cast<std::vector<std::string>>(a));
			if (a.type() == typeid(std::vector<Bytes>)) return encode<Bytes>(std::any_cast<std::vector<Bytes>>(a));
			if (a.type() == typeid(std::vector<int>)) return encode<int>(std::any_cast<std::vector<int>>(a));
			if (a.type() == typeid(std::vector<float>)) return encode<float>(std::any_cast<std::vector<float>>(a));
			if (a.type() == typeid(std::vector<double>)) return encode<double>(std::any_cast<std::vector<double>>(a));
			if (a.type() == typeid(Dict)) return encode(cast<Dict>(a));
			throw std::runtime_error("Can not encode type " + std::string(a.type().name()) + " to JSON");
		}
		static std::string encode(const char* s) { return "\"" + _escaped(s) + "\""; }
		static std::string encode(const String& s) { return "\"" + _escaped(s.s) + "\""; }
		static std::string encode(const std::string& s) { return "\"" + _escaped(s) + "\""; }
		static std::string encode(const Bytes& s) { return "\"" + cppcodec::base64_rfc4648::encode(s) + "\""; }
		static std::string encode(const int& i) { return std::to_string(i); }
		static std::string encode(const float& f) { return std::to_string(f); }
		static std::string encode(const double& d) { return std::to_string(d); }
		static std::string encode(const bool& b) { return b ? "true" : "false"; }

		template<typename T>
		static std::string encode_iterable(const T& v) {
			if (!v.size()) return "[]";

			std::string result;
			result += "[";

			for (const auto& e : v) result += encode(e) + ",";
			result[result.length() - 1] = ']';

			return result;
		}
		template<typename T>
		static std::string encode(const std::vector<T>& v) { return encode_iterable<std::vector<T>>(v); }
		template<typename T>
		static std::string encode(const std::set<T>& v) { return encode_iterable<std::set<T>>(v); }

		static std::string encode(const Dict& m) {
			if (!m.size()) return "{}";

			std::string result;
			result += "{";

			for (const auto& pair : m) result += "\"" + pair.first + "\"" ":" + encode(pair.second) + ",";
			result[result.length() - 1] = '}';

			return result;
		}

		template<class T>
		static T from(const std::string& json_text) { return T(decode(json_text)); }

		std::string encoded() const { return encode(structure()); }

		bool contains(const std::string& key, const std::any& structure) const { return contains(this->structure(), key, structure); }

		static bool contains(const std::any& haystack, const std::string& key, const std::any& needle) { return
			std::regex_search(
				encode(haystack),
				std::regex(
					std::regex_replace(
						(
							"\"" + key + "\" *: *" +
							std::regex_replace(
								encode(needle),
								std::regex("\\(|\\)"),
								"\\$&"
							) +
							"(?:\\n| )*(?:,|})"
						),
						std::regex("\\[|\\]|\\{|\\}|\\/|\\+|\\.|\\\""),
						"\\$&"
					)
				)
			);
		}

		virtual std::any structure() const = 0;

		bool operator<(const Json& r) const { return encoded() < r.encoded(); }
		bool operator==(const Json& r) const { return encoded() == r.encoded(); }

	private:
		template<typename T>
		static std::any _decode(const T& v) {
			if (v.IsNumber()) {
				if (v.IsInt()) { return v.GetInt(); }
				if (v.IsDouble()) { return v.GetDouble(); }
			}
			if (v.IsString()) { return String(std::string(v.GetString())); }
			if (v.IsArray()) {
				List result;
				for (const auto& e : v.GetArray()) result.push_back(_decode<rapidjson::Value>(e));
				return result;
			}
			if (v.IsObject()) {
				Dict result;
				for (const auto& k_v : v.GetObject()) result[k_v.name.GetString()] = _decode<rapidjson::Value>(k_v.value);
				return result;
			}
			if (v.IsBool()) return v.GetBool();

			throw std::runtime_error("Can not decode JSON");
		}

		static std::string _escaped(const std::string& s) { return
			std::regex_replace(
				std::regex_replace(s, std::regex("\\\\"), "\\\\"),
				std::regex("\""),
				"\\\""
			);
		}
	};
} // gorage