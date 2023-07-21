#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Usi.hpp"
#include "File.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	class BinaryFileStorage : public Storage<Bytes> {

	public:

		explicit BinaryFileStorage(const std::filesystem::path& folder, const std::string& extension):
			_folder(folder),
			_extension(extension) {}

	protected:

		const std::filesystem::path _folder;
		const std::string           _extension;

		void save(const Usi& usi, const Bytes& content) {
			File(_path(usi)).write<Bytes>(content);
		}

		Bytes load(const Usi& usi) const { return
			File(_path(usi)).read<Bytes>();
		}

		virtual bool exists(const Usi& usi) const { return
			std::filesystem::exists(_path(usi));
		}

		void remove(const Usi& usi) {
			std::filesystem::remove(_path(usi));
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

	private:
		std::filesystem::path _path(const Usi& usi) const {
			std::filesystem::path result = _folder / usi.value();
			result.replace_extension(_extension);
			return result;
		}

	};

} // gorage