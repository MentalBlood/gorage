#pragma once

#ifndef __GORAGE__BINARY_FILE_STORAGE__
#define __GORAGE__BINARY_FILE_STORAGE__

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Key.hpp"
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
		 * @brief Saves given data with given key
		 * 
		 * @param key unique storage identifier
		 * @param content Arbitrary binary data
		 */
		void save(const Key& key, const Bytes& content) {

			if (!std::filesystem::exists(_folder_path)) {
				std::filesystem::create_directories(_folder_path);
			}

			std::string file_path = _FilePath(key);

			std::ofstream file(file_path, std::ios::trunc | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + file_path);
			}

			file.write(reinterpret_cast<const char*>(&content[0]), content.size());
			file.close();

		}

		/**
		 * @brief Loads data with given key
		 * 
		 * @param key unique storage identifier
		 * @return std::string Loaded data
		 */
		Bytes load(const Key& key) {

			std::string file_path = _FilePath(key);

			std::basic_ifstream<unsigned char, std::char_traits<unsigned char>> file(file_path, std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Can not load file " + file_path);
			}

			return Bytes(
				std::istreambuf_iterator<unsigned char>(file),
				std::istreambuf_iterator<unsigned char>()
			);

		}

		/**
		 * @brief Removes data with given key
		 * 
		 * @param key unique storage identifier
		 */
		void remove(const Key& key) {
			std::filesystem::remove(_FilePath(key));
		}

	private:

		/**
		 * @brief Composes file path from key and this storage properties
		 * 
		 * @param key unique storage identifier
		 * @return const std::string File path
		 */
		const std::string _FilePath(const Key& key) const {
			return _folder_path + "/" + key() + _extension;
		}

		/**
		 * @brief Method to load keys for iteration
		 * 
		 */
		void loadKeys() {
			_keys.clear();
			if (std::filesystem::exists(_folder_path)) {
				for (const auto & p : std::filesystem::directory_iterator(_folder_path)) {
					if (p.path().extension() == _extension) {
						_keys.insert(
							p.path().stem().string()
						);
					}
				}
			}
		}

	};

} // gorage

#endif