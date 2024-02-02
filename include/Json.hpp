#pragma once

#include <any>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

#include "Base64.hpp"

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
    explicit String(const std::string &s) : s(s) {}
    explicit String(const Bytes &b) : b(b) {}

    std::string encoded() const {
      if (s.length())
        return s;
      return base64::encode(b);
    }
    Bytes decoded() const {
      if (b.size())
        return b;
      return base64::decode(s);
    }
    std::string hex() const {
      std::string result;

      char buffer[3];
      for (const auto &i : b) {
        sprintf(buffer, "%02X", i);
        result += buffer;
      }
      return result;
    }
  };
  class Structure {
  public:
    Structure(const std::any &value) : _value(value) {}
    const std::any &value() const { return _value; }

  private:
    std::any _value;
  };
  static std::string type_name(const std::type_info &type) {
    if (type == typeid(String))
      return "String";
    if (type == typeid(std::string))
      return "std::string";
    if (type == typeid(long))
      return "long";
    if (type == typeid(double))
      return "double";
    if (type == typeid(Bytes))
      return "Bytes";
    if (type == typeid(bool))
      return "bool";
    if (type == typeid(List))
      return "List";
    if (type == typeid(Dict))
      return "Dict";
    return "unknown";
  }
  class CastError : public std::runtime_error {
  public:
    explicit CastError(const std::string &name, const std::any &a)
        : std::runtime_error(("Can not cast " + name + " to " + type_name(a.type())).c_str()) {}
  };
  class KeyError : public std::runtime_error {
  public:
    explicit KeyError(const std::string &key) : std::runtime_error(("No key \"" + key + "\" found").c_str()) {}
  };
  template <typename T> static T cast(const std::any &a, const std::string &name = "something") {
    try {
      return std::any_cast<T>(a);
    } catch (const std::exception &) {
      throw CastError(name, a);
    }
  }
  template <typename T> static T get(const Dict &d, const std::string &key, const std::optional<T> &_default = {}) {
    if (d.count(key))
      return cast<T>(d.at(key), key);
    if (_default)
      return *_default;
    throw KeyError(key);
  }
  template <typename T>
  static T get_object(const Dict &d, const std::string &key, const std::optional<T> &_default = {}) {
    if (d.count(key))
      return T(d.at(key));
    if (_default)
      return *_default;
    throw KeyError(key);
  }
  static std::any decode(const std::string &json_text) {
    auto stream = std::stringstream(json_text);
    return _decode<std::any>(stream);
  }
  static std::string encode(const std::any &a) {
    if (a.type() == typeid(const char *))
      return encode(std::any_cast<const char *>(a));
    if (a.type() == typeid(String))
      return encode(std::any_cast<String>(a));
    if (a.type() == typeid(std::string))
      return encode(std::any_cast<std::string>(a));
    if (a.type() == typeid(Bytes))
      return encode(std::any_cast<Bytes>(a));
    if (a.type() == typeid(long))
      return encode(std::any_cast<long>(a));
    if (a.type() == typeid(int))
      return encode(long(std::any_cast<int>(a)));
    if (a.type() == typeid(float))
      return encode(std::any_cast<float>(a));
    if (a.type() == typeid(double))
      return encode(std::any_cast<double>(a));
    if (a.type() == typeid(bool))
      return encode(std::any_cast<bool>(a));
    if (a.type() == typeid(List))
      return encode<std::any>(std::any_cast<List>(a));
    if (a.type() == typeid(std::vector<std::string>))
      return encode<std::string>(std::any_cast<std::vector<std::string>>(a));
    if (a.type() == typeid(std::vector<Bytes>))
      return encode<Bytes>(std::any_cast<std::vector<Bytes>>(a));
    if (a.type() == typeid(std::vector<long>))
      return encode<long>(std::any_cast<std::vector<long>>(a));
    if (a.type() == typeid(std::vector<int>))
      return encode<int>(std::any_cast<std::vector<int>>(a));
    if (a.type() == typeid(std::vector<float>))
      return encode<float>(std::any_cast<std::vector<float>>(a));
    if (a.type() == typeid(std::vector<double>))
      return encode<double>(std::any_cast<std::vector<double>>(a));
    if (a.type() == typeid(Dict))
      return encode(cast<Dict>(a));
    throw std::runtime_error("Can not encode type " + std::string(a.type().name()) + " to JSON");
  }
  static std::string encode(const char *s) { return "\"" + _escaped(s) + "\""; }
  static std::string encode(const String &s) { return "\"" + _escaped(s.s) + "\""; }
  static std::string encode(const std::string &s) { return "\"" + _escaped(s) + "\""; }
  static std::string encode(const Bytes &s) { return "\"" + String(s).encoded() + "\""; }
  static std::string encode(const long &i) { return std::to_string(i); }
  static std::string encode(const int &i) { return std::to_string(i); }
  static std::string encode(const double &d) { return std::to_string(d); }
  static std::string encode(const bool &b) { return b ? "true" : "false"; }

  template <typename T> static std::string encode_iterable(const T &v) {
    if (!v.size())
      return "[]";

    std::string result;
    result += "[";

    for (const auto &e : v)
      result += encode(e) + ",";
    result[result.length() - 1] = ']';

    return result;
  }
  template <typename T> static std::string encode(const std::vector<T> &v) {
    return encode_iterable<std::vector<T>>(v);
  }
  template <typename T> static std::string encode(const std::set<T> &v) { return encode_iterable<std::set<T>>(v); }

  static std::string encode(const Dict &m) {
    if (!m.size())
      return "{}";

    std::string result;
    result += "{";

    for (const auto &pair : m)
      result += "\"" + pair.first +
                "\""
                ":" +
                encode(pair.second) + ",";
    result[result.length() - 1] = '}';

    return result;
  }

  template <class T> static T from(const std::string &json_text) { return T(decode(json_text)); }

  std::string encoded() const { return encode(structure()); }

  virtual std::any structure() const = 0;

  bool operator<(const Json &r) const { return encoded() < r.encoded(); }
  bool operator==(const Json &r) const { return encoded() == r.encoded(); }

private:
  static std::string _escaped(const std::string &s) {
    return std::regex_replace(std::regex_replace(s, std::regex("\\\\"), "\\\\"), std::regex("\""), "\\\"");
  }

  template <typename T> static std::any _decode(std::stringstream &input) {
    if constexpr (std::is_same_v<T, std::any>)
      switch (input.peek()) {
      case '"':
        return String(std::any_cast<std::string>(_decode<std::string>(input)));
      case '[':
        return _decode<List>(input);
      case '{':
        return _decode<Dict>(input);
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return _decode<long>(input);
      }
    if constexpr (std::is_same_v<T, std::string>) {
      input.get();
      return _read_till(input, {'"'}, true);
    }
    if constexpr (std::is_same_v<T, long> || std::is_same_v<T, double>) {
      const auto s = _read_till(input, {']', '}', ','});
      input.unget();

      if (std::count(s.begin(), s.end(), '.'))
        return atof(s.c_str());
      else
        return atol(s.c_str());
    }
    if constexpr (std::is_same_v<T, List>) {
      List result;
      input.get();
      _skip(input);
      if (input.peek() == ']')
        return result;
      while (true) {
        _skip(input);
        result.push_back(_decode<std::any>(input));
        _skip(input);
        if (input.get() == ']')
          break;
      }
      return result;
    }
    if constexpr (std::is_same_v<T, Dict>) {
      Dict result;
      input.get();
      _skip(input);
      if (input.peek() == '}')
        return result;
      while (true) {
        _skip(input);
        const auto key = std::any_cast<std::string>(_decode<std::string>(input));
        _skip(input);
        if (input.get() != ':')
          throw std::runtime_error("JSON objects keys and values must be separated by ':'");
        _skip(input);
        result[key] = _decode<std::any>(input);
        _skip(input);
        if (input.get() == '}')
          break;
      }
      return result;
    }
    throw std::runtime_error("Can not decode JSON");
  }
  static void _skip(std::stringstream &input, const std::set<char> &chars = {' ', '\n', '\t'}) {
    while (chars.count(input.get())) {
    }
    input.unget();
  }
  static std::string _read_till(std::stringstream &input, const std::set<char> &ends, const bool &escaping = false) {
    std::string result;
    while (!input.eof()) {
      const char c = input.get();
      if (input.eof())
        break;

      if ((ends.count(c) && (!escaping || !result.length() || (result.at(result.length() - 1) != '\\'))) || !c)
        break;
      else
        result.push_back(c);
    }
    return result;
  }
};
} // namespace gorage
