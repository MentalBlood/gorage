#pragma once

#ifndef __GORAGE__FILE_STORAGE__
#define __GORAGE__FILE_STORAGE__

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Json.hpp"
#include "gorage.hpp"
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
		 * @brief Saves given object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @param object Object to save
		 */
		void save(const std::string& usi, const T& object) {

			if (!std::filesystem::exists(folder_path)) {
				std::filesystem::create_directories(folder_path);
			}

			std::string file_path = _FilePath(usi);

			std::ofstream file(file_path, std::ios::out | std::ios::trunc);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + file_path);
			}

			file << object.encoded();
			file.close();

		}

		/**
		 * @brief Loads data with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @return T Loaded object
		 */
		T load(const std::string& usi) {

			std::string file_path = _FilePath(usi);

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
		 * @brief Removes object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 */
		void remove(const std::string& usi) {
			std::filesystem::remove(_folder_path + usi + _extension);
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
			_usis.clear();
			for (const auto & p : std::filesystem::directory_iterator(_folder_path)) {
				if (p.path().extension() == _extension) {
					_usis.insert(p.path().stem().string());
				}
			}
		}

	};

} // gorage

#endif