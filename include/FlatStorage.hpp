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

  explicit FlatStorage(const std::filesystem::path &path, const Pos &key_size = 24, const Pos &value_size = 24)
      : path(path), key_size(key_size), value_size(value_size) {
    if (!std::filesystem::exists(path))
      std::ofstream(path).close();
  }

  Pos row_size() const { return key_size + value_size; }

  T load(const Key &key) const {
    auto f = _ifstream();
    const auto pos = _key_pos(f, f.tellg(), key);
    if (!pos.has_value())
      throw exceptions::KeyError(key);

    f.seekg(pos.value() + key_size);
    const auto result = _read_till(f, '^');
    f.close();

    if constexpr (std::is_same_v<T, std::string>)
      return result;
    else if constexpr (std::is_base_of<Json, T>::value)
      return Json::from<T>(result);
    else
      return Json::decode(result);
  }
  virtual bool exists(const Key &key) const {
    auto f = _ifstream();
    return _key_pos(f, f.tellg(), key).has_value();
  }
  void clear() { std::filesystem::resize_file(path, 0); }

  virtual std::set<Key> keys() const {
    auto f = _ifstream();
    auto result = std::set<Key>();
    const auto f_size = f.tellg();

    for (Pos start = 0; start < f_size; start += row_size()) {
      f.seekg(start, std::ios::beg);
      std::string current = _read_till(f, '^');
      result.insert(current);
    }
    return result;
  }

  FlatStorage(const Json::Structure &structure) {
    const auto dict = Json::cast<Json::Dict>(structure.value());
    path = Json::get<Json::String>(dict, "path").s;
    key_size = Json::get<int>(dict, "key_size");
    value_size = Json::get<int>(dict, "value_size");
  }
  virtual std::any structure() const {
    return Json::Dict({{"path", path.string()}, {"key_size", key_size}, {"value_size", value_size}});
  }

protected:
  void _save(const Key &key, const T &object) {
    auto f = _fstream();
    const auto pos = _key_pos<std::fstream>(f, f.tellg(), key);
    if (pos.has_value())
      f.seekg(pos.value());
    else
      f.seekg(0, std::ios::end);
    f << _row(key, object);
    f.close();
  }
  void _remove(const Key &key) {
    auto f = _fstream();
    const auto f_size = f.tellg();
    const auto pos = _key_pos(f, f_size, key);
    if (!pos.has_value())
      return;

    if (pos <= size_t(f_size) - row_size()) {
      const auto last_row = _row(f, size_t(f_size) - row_size());
      f.seekg(pos.value());
      f << last_row;
    }
    f.close();
    std::filesystem::resize_file(path, size_t(f_size) - row_size());
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

  std::string _read_till(std::ifstream &f, const char &end) const {
    std::string result;
    while (!f.eof()) {
      char c = f.get();
      if (c == end)
        break;
      result.push_back(c);
    }
    return result;
  }
  std::string _read_till(std::fstream &f, const Pos &amount) const {
    std::string result;
    for (Pos i = 0; (i < amount) && !f.eof(); i++)
      result.push_back(f.get());

    return result;
  }
  template <class S = std::ifstream>
  std::optional<Pos> _key_pos(S &f, const std::ifstream::pos_type &f_size, const Key &key) const {
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
    return _pad(key.value, '^', key_size) + _pad(_value(object), '^', value_size);
  }
  std::string _row(std::fstream &f, const Pos &start) {
    f.seekg(start, std::ios::beg);
    return _read_till(f, row_size());
  }
};
} // namespace gorage
