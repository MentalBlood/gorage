#pragma once

#include <filesystem>
#include <set>

#include "Bytes.hpp"
#include "File.hpp"
#include "Json.hpp"
#include "Key.hpp"
#include "Storage.hpp"

namespace gorage {
template <class T> class Storage<File<T>> : public Storage<T> {
public:
  std::filesystem::path root;
  std::string extension;

  explicit Storage(const std::filesystem::path &folder, const std::string &extension)
      : root(folder), extension(extension) {}

  std::filesystem::path path(const Key &key) const { return root / (key.value + "." + extension); }

  File<T> file(const Key &key) const { return File<T>(path(key)); }

  T load(const Key &key) const { return file(key).read(); }
  virtual bool exists(const Key &key) const { return file(key).exists(); }

  Bytes raw(const Key &key) const { return File<Bytes>(path(key)).read(); }

  virtual std::set<Key> keys() const {
    if (!std::filesystem::exists(root))
      return {};

    std::set<Key> result;
    for (const auto &p : std::filesystem::directory_iterator(root))
      if (p.path().extension() == "." + extension)
        result.insert(Key(p.path().stem().string()));

    return result;
  }

  void clear() { std::filesystem::remove_all(root); }

  Storage(const Json::Structure &structure) {
    const auto dict = Json::cast<Json::Dict>(structure.value());
    root = Json::get<Json::String>(dict, "root").s;
    extension = Json::get<Json::String>(dict, "extension").s;
  }
  virtual std::any structure() const { return Json::Dict({{"root", root.string()}, {"extension", extension}}); }

protected:
  void _save(const Key &key, const T &object) { file(key).write(object); }
  void _remove(const Key &key) { file(key).remove(); }
};
} // namespace gorage
