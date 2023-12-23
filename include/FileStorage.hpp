#pragma once

#include <filesystem>
#include <set>

#include "Bytes.hpp"
#include "File.hpp"
#include "Json.hpp"
#include "Storage.hpp"
#include "Key.hpp"

namespace gorage {
template <class T> class Storage<File<T>> : public Storage<T> {
public:
  std::filesystem::path root;
  std::string extension;

  explicit Storage(const std::filesystem::path &folder,
                   const std::string &extension)
      : root(folder), extension(extension) {}

  std::filesystem::path path(const Key &id) const {
    return root / (id.value + "." + extension);
  }

  File<T> file(const Key &id) const { return File<T>(path(id)); }

  T load(const Key &id) const { return file(id).read(); }
  virtual bool exists(const Key &id) const { return file(id).exists(); }

  Bytes raw(const Key &id) const { return File<Bytes>(path(id)).read(); }

  virtual std::set<Key> keys() const {
    if (!std::filesystem::exists(root))
      return {};

    std::set<Key> result;
    for (const auto &p : std::filesystem::directory_iterator(root))
      if (p.path().extension() == extension)
        result.insert(Key(p.path().stem().string()));

    return result;
  }

protected:
  void _save(const Key &id, const T &object) { file(id).write(object); }
  void _remove(const Key &id) { file(id).remove(); }
};
} // namespace gorage
