#pragma once

#include <filesystem>

#include "Bytes.hpp"
#include "File.hpp"
#include "Json.hpp"
#include "Storage.hpp"
#include "Usi.hpp"

namespace gorage {
template <class T> class Storage<File<T>> : public Storage<T> {
public:
  std::filesystem::path root;
  std::string extension;

  explicit Storage(const std::filesystem::path &folder,
                   const std::string &extension)
      : root(folder), extension(extension) {}

  std::filesystem::path path(const Usi &usi) const {
    return root / (usi.value + "." + extension);
  }

  File<T> file(const Usi &usi) const { return File<T>(path(usi)); }

  void save(const Usi &usi, const T &object) { file(usi).write(object); }
  T load(const Usi &usi) const { return file(usi).read(); }
  void remove(const Usi &usi) { file(usi).remove(); }
  virtual bool exists(const Usi &usi) const { return file(usi).exists(); }

  Bytes raw(const Usi &usi) const { return File<Bytes>(path(usi)).read(); }

  virtual std::vector<Usi> usis() const {
    if (!std::filesystem::exists(root))
      return {};

    std::vector<Usi> result;
    for (const auto &p : std::filesystem::directory_iterator(root))
      if (p.path().extension() == extension)
        result.push_back(Usi(p.path().stem().string()));

    return result;
  }
};
} // namespace gorage
