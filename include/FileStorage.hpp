#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Usi.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	template<class T>
	class FileStorage : public Storage<T> {

	public:

		FileStorage(const std::filesystem::path folder, const std::string extension):
			_folder(folder),
			_extension(extension) {}

	protected:

		const std::filesystem::path _folder;
		const std::string           _extension;

		void save(const Usi usi, const T object) {

			if (!std::filesystem::exists(_folder)) {
				std::filesystem::create_directories(_folder);
			}

			const std::string path = _path(usi()).string();

			std::ofstream file(path, std::ios::out | std::ios::trunc);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + path);
			}

			file << object.encoded();
			file.close();

		}

		T load(const Usi usi) const {

			const std::string path = _path(usi()).string();

			std::ifstream file(path, std::ios::in);
			if (!file.is_open()) {
				throw std::runtime_error("Can not load file " + path);
			}

			std::stringstream result_stream;
			result_stream << file.rdbuf();

			file.close();

			return Json::from<T>(result_stream.str());

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