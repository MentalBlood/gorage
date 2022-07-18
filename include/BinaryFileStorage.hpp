#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Storage.hpp"



class BinaryFileStorage : public Storage<std::vector<uint8_t>> {

public:

	const std::string folder_path;
	const std::string extension;

	BinaryFileStorage(const std::string& folder_path, const std::string& extension):
		folder_path(folder_path), extension(extension) {}

protected:

	void _save(const std::string& id, const std::vector<uint8_t>& content) {

		std::string file_path = this->FilePath(id);

		std::ofstream file(file_path, std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Can not save file " + file_path);
		}

		file.write(reinterpret_cast<const char*>(&content[0]), content.size());

		file.close();

	}

	std::vector<uint8_t> _load(const std::string& id) {

		std::string file_path = this->FilePath(id);

		std::ifstream file(file_path, std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Can not load file " + file_path);
		}

		file.seekg(0, std::ios::end);
		size_t length = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<uint8_t> result(length);

		file.read(reinterpret_cast<char*>(&result[0]), length);

		file.close();

		return result;

	}

private:

	const std::string FilePath(const std::string& id) const {
		return this->folder_path + "/" + id + this->extension;
	}

	void loadKeys() {
		for (const auto & p : std::filesystem::directory_iterator(this->folder_path)) {
			this->keys.insert(p.path().string());
		}
	}

};