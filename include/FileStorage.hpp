#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Usi.hpp"
#include "Json.hpp"
#include "File.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	template<class T>
	class Storage<File<T>> : public Storage<T> {

	public:

		const std::filesystem::path folder;
		const std::string           extension;

		explicit Storage(const std::filesystem::path& folder, const std::string& extension):
			folder(folder),
			extension(extension) {}

		std::filesystem::path path(const Usi& usi) const { return
			(folder / usi.value()).replace_extension(extension);
		}

		File<T> file(const Usi& usi) const { return
			File<T>(path(usi));
		}

		void save(const Usi& usi, const T& object) {
			file(usi).write(object);
		}

		T load(const Usi& usi) const { return
			file(usi).read();
		}
		Bytes raw(const Usi& usi) const { return
			File<Bytes>(path(usi)).read();
		}

		virtual bool exists(const Usi& usi) const { return
			file(usi).exists();
		}

		void remove(const Usi& usi) {
			file(usi).remove();
		}

		virtual std::vector<Usi> usis() const {

			std::vector<Usi> result;

			if (std::filesystem::exists(folder)) {
				for (const auto& p : std::filesystem::directory_iterator(folder)) {
					if (p.path().extension() == extension) {
						result.push_back(
							Usi(
								p.path().stem().string()
							)
						);
					}
				}
			}

			return result;

		}

	};

} // gorage