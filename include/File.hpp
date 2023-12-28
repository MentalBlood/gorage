#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Bytes.hpp"
#include "Json.hpp"
#include "Key.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"

namespace gorage {
namespace exceptions {
class CanNotReadFile : public Base {
public:
  explicit CanNotReadFile(const std::filesystem::path &path)
      : Base("Can not read file `" + path.string() + "`") {}
};
class CanNotWriteFile : public Base {
public:
  explicit CanNotWriteFile(const std::filesystem::path &path)
      : Base("Can not write file `" + path.string() + "`") {}
};
} // namespace exceptions
template <class T> class File {
public:
  std::filesystem::path path;

  File(const std::filesystem::path &path) : path(path) {}

  bool exists() const { return std::filesystem::exists(path); }
  void remove() const { std::filesystem::remove(path); }
  void write(const T &object) const {

    const auto parent = path.parent_path();
    if (parent.string().length() && !std::filesystem::exists(parent))
      std::filesystem::create_directories(parent);

    if constexpr (std::is_same_v<T, Bytes>) {
      std::ofstream file(path, std::ios::trunc | std::ios::binary);
      if (!file.is_open())
        throw exceptions::CanNotWriteFile(path);

      file.write(reinterpret_cast<const char *>(&object[0]), object.size());
      file.close();
    } else {
      std::ofstream file(path, std::ios::out | std::ios::trunc);
      if (!file.is_open())
        throw exceptions::CanNotWriteFile(path);

      if constexpr (std::is_same_v<T, std::string>)
        file << object;
      else if constexpr (std::is_base_of<T, Json>::value)
        file << object.encoded();
      else
        file << Json::encode(object);
      file.close();
    }
  }
  T read() const {
    if constexpr (std::is_same_v<T, Bytes>) {

      std::ifstream file(path, std::ios::binary | std::ios::ate);
      if (!file.is_open())
        throw exceptions::CanNotReadFile(path);

      std::ifstream::pos_type length = file.tellg();
      if (length == 0) {
        file.close();
        return Bytes();
      }

      Bytes result(length);
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char *>(&result[0]), length);
      file.close();
      return result;
    } else {
      std::ifstream file(path, std::ios::in);
      if (!file.is_open())
        throw exceptions::CanNotReadFile(path);

      std::stringstream result_stream;
      result_stream << file.rdbuf();
      file.close();

      const auto result = result_stream.str();
      if constexpr (std::is_same_v<T, std::string>)
        return result;
      else if constexpr (std::is_base_of<T, Json>::value)
        return Json::from<T>(result);
      else
        return Json::decode(result);
    }
  }
};
} // namespace gorage
