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
			folder_path(folder_path), extension(extension) {}

	protected:

		/**
		 * @brief Folder to store files in
		 * 
		 */
		const std::string folder_path;
		/**
		 * @brief Extension to store files with
		 * 
		 */
		const std::string extension;

		/**
		 * @brief Saves given object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @param object Object to save
		 */
		void save(const std::string& usi, const T& object) {

			const std::string content = object.toJson();

			std::string file_path = this->FilePath(usi);

			std::ofstream file(file_path, std::ios::out | std::ios::trunc);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + file_path);
			}

			file << content;
			file.close();

		}

		/**
		 * @brief Loads data with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @return T Loaded object
		 */
		T load(const std::string& usi) {

			std::string file_path = this->FilePath(usi);

			std::ifstream file(file_path, std::ios::in);
			if (!file.is_open()) {
				throw std::runtime_error("Can not load file " + file_path);
			}

			std::stringstream result_stream;
			result_stream << file.rdbuf();

			file.close();

			return Json::create<T>(result_stream.str());

		}

		/**
		 * @brief Removes object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 */
		void remove(const std::string& usi) {
			std::filesystem::remove(this->folder_path + usi + this->extension);
		}

	private:

		/**
		 * @brief Composes file path from USI and this storage properties
		 * 
		 * @param usi Unique Storage Identifier
		 * @return const std::string File path
		 */
		const std::string FilePath(const std::string& usi) const {
			return this->folder_path + "/" + usi + this->extension;
		}

		/**
		 * @brief Method to load USIs for iteration
		 * 
		 */
		void loadUsis() {
			this->usis.clear();
			for (const auto & p : std::filesystem::directory_iterator(this->folder_path)) {
				if (p.path().extension() == this->extension) {
					this->usis.insert(p.path().stem().string());
				}
			}
		}

	};

} // gorage

#endif