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

		explicit Storage(const std::filesystem::path& folder, const std::string& extension):
			_folder(folder),
			_extension(extension) {}

	protected:

		const std::filesystem::path _folder;
		const std::string           _extension;

		void save(const Usi& usi, const T& object) {
			file(usi).write(object);
		}

		T load(const Usi& usi) const { return
			file(usi).read();
		}

		virtual bool exists(const Usi& usi) const { return
			file(usi).exists();
		}

		void remove(const Usi& usi) {
			file(usi).remove();
		}

		virtual std::vector<Usi> usis() const {

			std::vector<Usi> result;

			if (std::filesystem::exists(_folder)) {
				for (const auto& p : std::filesystem::directory_iterator(_folder)) {
					if (p.path().extension() == _extension) {
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

		File<T> file(const Usi& usi) const { return
			File<T>(
				(_folder / usi.value()).replace_extension(_extension)
			);
		}

	};

} // gorage