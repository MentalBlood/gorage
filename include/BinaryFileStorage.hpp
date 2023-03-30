#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Usi.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	class BinaryFileStorage : public Storage<Bytes> {

	public:

		BinaryFileStorage(const std::filesystem::path folder, const std::string extension):
			_folder(folder),
			_extension(extension) {}

	protected:

		const std::filesystem::path _folder;
		const std::string           _extension;

		void save(const Usi usi, const Bytes content) {

			if (!std::filesystem::exists(_folder)) {
				std::filesystem::create_directories(_folder);
			}

			const std::string path = _path(usi()).string();

			std::ofstream file(path, std::ios::trunc | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + path);
			}

			file.write(reinterpret_cast<const char*>(&content[0]), content.size());
			file.close();

		}

		Bytes load(const Usi usi) const {

			const std::string path = _path(usi()).string();

			std::ifstream file(path, std::ios::binary | std::ios::ate);
			std::ifstream::pos_type length = file.tellg();
			if (length == 0) {
				return Bytes();
			}

			Bytes result(length);
			file.seekg(0, std::ios::beg);
			file.read(reinterpret_cast<char*>(&result[0]), length);
			return result;

		}

		void remove(const Usi usi) {
			std::filesystem::remove(_path(usi));
		}

		virtual std::vector<Usi> usis() const {

			std::vector<Usi> result;

			if (std::filesystem::exists(_folder)) {
				for (const auto & p : std::filesystem::directory_iterator(_folder)) {
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
		std::filesystem::path _path(const Usi usi) const {
			std::filesystem::path result = _folder / usi();
			result.replace_extension(_extension);
			return result;
		}

	};

} // gorage