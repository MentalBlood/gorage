#pragma once

#ifndef __GORAGE__BINARY_FILE_STORAGE__
#define __GORAGE__BINARY_FILE_STORAGE__

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "gorage.hpp"
#include "Storage.hpp"



namespace gorage {

	/**
	 * @brief Storage for binary data. Stores data as files in given folder and with given extension
	 * 
	 */
	class BinaryFileStorage : public Storage<gorage::Bytes> {

	public:

		/**
		 * @brief Construct a new Binary File Storage object
		 * 
		 * @param folder_path Folder to store files in
		 * @param extension Extension to store files with
		 */
		BinaryFileStorage(const std::string& folder_path, const std::string& extension):
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
		 * @brief Saves given data with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @param content Arbitrary binary data
		 */
		void save(const std::string& usi, const gorage::Bytes& content) {

			std::string file_path = this->FilePath(usi);

			std::ofstream file(file_path, std::ios::trunc | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Can not save file " + file_path);
			}

			size_t content_size = content.size();

			file.write(reinterpret_cast<const char*>(&content[0]), content.size());

			file.close();

		}

		/**
		 * @brief Loads data with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @return std::string Loaded data
		 */
		gorage::Bytes load(const std::string& usi) {

			std::string file_path = this->FilePath(usi);

			// std::ifstream file(file_path, std::ios::binary);
			std::basic_ifstream<unsigned char, std::char_traits<unsigned char>> file(file_path, std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("Can not load file " + file_path);
			}

			return gorage::Bytes(
				std::istreambuf_iterator<unsigned char>(file),
				std::istreambuf_iterator<unsigned char>()
			);

		}

		/**
		 * @brief Removes data with given USI
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
			for (const auto & p : std::filesystem::directory_iterator(this->folder_path)) {
				if (p.path().extension() == this->extension) {
					this->usis.insert(p.path().stem().string());
				}
			}
		}

	};

} // gorage

#endif