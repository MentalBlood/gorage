#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Storage.hpp"



/**
 * @brief Storage for binary data. Stores data as files in given folder and with given extension
 * 
 */
class BinaryFileStorage : public Storage<std::string> {

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
	void save(const std::string& usi, const std::string& content) {

		std::string file_path = this->FilePath(usi);

		std::ofstream file(file_path, std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Can not save file " + file_path);
		}

		file.write(content.c_str(), content.length());

		file.close();

	}

	/**
	 * @brief Loads data with given USI
	 * 
	 * @param usi Unique Storage Identifier
	 * @return std::string Loaded data
	 */
	std::string load(const std::string& usi) {

		std::string file_path = this->FilePath(usi);

		std::ifstream file(file_path, std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Can not load file " + file_path);
		}

		std::ostringstream result_stream;
		result_stream << file.rdbuf();

		file.close();

		return result_stream.str();

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