#pragma once

#ifndef __GORAGE__FILE_STORAGE__
#define __GORAGE__FILE_STORAGE__

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Key.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	/**
	 * @brief Storage for arbitrary structured, JSONable data. Stores data as files in given folder and with given extension
	 * 
	 */
	template<class T>
	class FileStorage : public Storage<T> {

	public:

		/**
		 * @brief Construct a new File Storage object
		 * 
		 * @param folder_path Folder to store files in
		 * @param extension Extension to store files with
		 */
		FileStorage(const std::string& folder_path, const std::string& extension):
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
		 * @brief Saves given object with given key
		 * 
		 * @param key unique storage identifier
		 * @param object Object to save
		 */
		void save(const Key& key, const T& object) {

			if (!std::filesystem::exists(_folder_path)) {
				std::filesystem::create_directories(_folder_path);
			}

			std::string file_path = _FilePath(key);

			std::ofstream file(file_path, std::ios::out | std::ios::trunc);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + file_path);
			}

			file << object.encoded();
			file.close();

		}

		/**
		 * @brief Loads data with given key
		 * 
		 * @param key unique storage identifier
		 * @return T Loaded object
		 */
		T load(const Key& key) {

			std::string file_path = _FilePath(key);

			std::ifstream file(file_path, std::ios::in);
			if (!file.is_open()) {
				throw std::runtime_error("Can not load file " + file_path);
			}

			std::stringstream result_stream;
			result_stream << file.rdbuf();

			file.close();

			return Json::createFromJson<T>(result_stream.str());

		}

		/**
		 * @brief Removes object with given key
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