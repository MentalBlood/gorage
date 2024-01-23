#pragma once

#include <filesystem>
#include <fstream>
#include <set>

#include "Bytes.hpp"
#include "File.hpp"
#include "Json.hpp"
#include "Key.hpp"
#include "Storage.hpp"

namespace gorage {
template <class T> class FlatStorage : public Storage<T> {
public:
  using Pos = size_t;

  std::filesystem::path path;
  Pos key_size;
  Pos value_size;

  explicit FlatStorage(const std::filesystem::path &path, const Pos &key_size, const Pos &value_size)
      : path(path), key_size(key_size), value_size(value_size) {}

  Pos row_size() const { return key_size + value_size; }

  T load(const Key &key) const {
    auto f = _ifstream();
    f.seekg(_key_pos(f, f.tellg(), key).value() + key_size);

    const auto result = _read_till(f, '\n');
    f.close();
    if (!result.has_value())
      throw exceptions::KeyError(key);

    if constexpr (std::is_same_v<T, std::string>)
      return result.value();
    else if constexpr (std::is_base_of<Json, T>::value)
      return Json::from<T>(result.value());
    else
      return Json::decode(result.value());
  }
  virtual bool exists(const Key &key) const { return _key_pos(key).has_value(); }

  virtual std::set<Key> keys() const {
    auto f = _ifstream();
    auto result = std::set<Key>();
    const auto f_size = f.tellg();

    for (Pos start = 0; start < f_size; start += row_size()) {
      f.seekg(start);
      std::string current;
      current.reserve(key_size);
      f.read(&current[0], key_size);
      result.insert(current);
    }
    return result;
  }

  FlatStorage(const Json::Structure &structure) {
    const auto dict = Json::cast<Json::Dict>(structure.value());
    path = Json::get<Json::String>(dict, "path").s;
    key_size = std::atoi(Json::get<Json::String>(dict, "key_size").s.c_str());
    value_size = std::atoi(Json::get<Json::String>(dict, "value_size").s.c_str());
  }
  virtual std::any structure() const {
    return Json::Dict({{"path", path.string()}, {"key_size", key_size}, {"value_size", value_size}});
  }

protected:
  void _save(const Key &key, const T &object) {
    auto f = _fstream();
    const auto pos = _key_pos(f, f.tellg(), key);
    if (pos.has_value())
      f.seekg(pos);
    f << _row(key, object);
    f.close();
  }
  void _remove(const Key &key) {
    auto f = _fstream();
    const auto f_size = f.tellg();
    const auto pos = _key_pos(f, f_size, key);
    if (!pos.has_value())
      return;

    if (pos <= f_size - row_size()) {
      const auto last_row = _row(f, f_size - row_size());
      f.seekg(pos);
      f << last_row;
    }
    std::filesystem::resize_file(path, f_size - row_size());
    f.close();
  }

private:
  std::ifstream _ifstream() const {
    auto f = std::ifstream(path, std::ios::binary | std::ios::in | std::ios::ate);
    if (!f.is_open())
      throw gorage::exceptions::CanNotReadFile(path);
    return f;
  }
  std::fstream _fstream() const {
    auto f = std::fstream(path, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
    if (!f.is_open())
      throw gorage::exceptions::CanNotReadFile(path);
    return f;
  }

  std::string _read_till(std::ifstream &f, const char &end) {
    std::string result;
    for (char c = f.get(); c != end;)
      result.push_back(c);
    return result;
  }
  std::optional<Pos> _key_pos(std::ifstream &f, const std::ifstream::pos_type &f_size, const Key &key) const {
    f.seekg(0, std::ios::beg);
    for (Pos start = 0; start < f_size; start += row_size()) {
      f.seekg(start);
      bool found = true;
      for (const auto &key_c : key.value)
        if (key_c != f.get()) {
          found = false;
          break;
        }
      if (found)
        return start;
    }
    return {};
  }
  std::optional<Pos> _key_pos(const Key &key) const {
    auto f = _ifstream();
    return _key_pos(f, f.tellg(), key);
  }

  std::string _pad(const std::string &s, const char &c, const Pos &target_size) {
    if (s.length() > target_size)
      throw exceptions::Base("too big key or value size");

    auto result = s;
    result.reserve(target_size);
    for (Pos i = 0; i < target_size - s.length(); i++)
      result.push_back(c);
    return result;
  }
  std::string _value(const T &object) {
    if constexpr (std::is_same_v<T, std::string>)
      return object;
    else if constexpr (std::is_base_of<T, Json>::value)
      return object.encoded();
    else
      return Json::encode(object);
  }
  std::string _row(const Key &key, const T &object) {
    return _pad(key.value, '\n', key_size) + _pad(_value(object), '\n', value_size);
  }
  std::string _row(std::fstream &f, const Pos &start) {
    std::string result;
    result.reserve(row_size());
    f.read(&result[0], row_size());
    return result;
  }
};
} // namespace gorage
