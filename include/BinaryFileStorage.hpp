#pragma once

#ifndef __GORAGE__BINARY_FILE_STORAGE__
#define __GORAGE__BINARY_FILE_STORAGE__

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	/**
	 * @brief Storage for binary data. Stores data as files in given folder and with given extension
	 *
	 */
	class BinaryFileStorage : public Storage<Bytes> {

	public:

		/**
		 * @brief Construct a new Binary File Storage object
		 *
		 * @param folder_path Folder to store files in
		 * @param extension Extension to store files with
		 */
		BinaryFileStorage(const std::string& folder_path, const std::string& extension):
			_folder_path(folder_path), _extension(extension) {}

	protected:

		/**
		 * @brief Folder to store files in
		 *
		 */
		const std::string _folder_path;
		/**
		 * @brief Extension to store files with
		 *
		 */
		const std::string _extension;

		/**
		 * @brief Saves given data with given USI
		 *
		 * @param usi Unique Storage Identifier
		 * @param content Arbitrary binary data
		 */
		void save(const std::string& usi, const Bytes& content) {

			if (!std::filesystem::exists(_folder_path)) {
				std::filesystem::create_directories(_folder_path);
			}

			std::string file_path = _FilePath(usi);

			std::ofstream file(file_path, std::ios::trunc | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + file_path);
			}

			file.write(reinterpret_cast<const char*>(&content[0]), content.size());
			file.close();

		}

		/**
		 * @brief Loads data with given USI
		 *
		 * @param usi Unique Storage Identifier
		 * @return std::string Loaded data
		 */
		Bytes load(const std::string& usi) {

			std::string path = _FilePath(usi);

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

		/**
		 * @brief Removes data with given USI
		 *
		 * @param usi Unique Storage Identifier
		 */
		void remove(const std::string& usi) {
			std::filesystem::remove(_folder_path + "/"+ usi + _extension);
		}

	private:

		/**
		 * @brief Composes file path from USI and this storage properties
		 *
		 * @param usi Unique Storage Identifier
		 * @return const std::string File path
		 */
		const std::string _FilePath(const std::string& usi) const {
			return _folder_path + "/" + usi + _extension;
		}

		/**
		 * @brief Method to load USIs for iteration
		 *
		 */
		void loadUsis() {
			this->_usis.clear();
			if (std::filesystem::exists(_folder_path)) {
				for (const auto & p : std::filesystem::directory_iterator(_folder_path)) {
					if (p.path().extension() == _extension) {
						this->_usis.insert(p.path().stem().string());
					}
				}
			}
		}

	};

} // gorage

#endif