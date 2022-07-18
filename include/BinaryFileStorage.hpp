#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Storage.hpp"



class BinaryFileStorage : public Storage<std::string> {

public:

	const std::string folder_path;
	const std::string extension;

	BinaryFileStorage(const std::string& folder_path, const std::string& extension):
		folder_path(folder_path), extension(extension) {}

protected:

	void _save(const std::string& id, const std::string& content) {

		std::string file_path = this->FilePath(id);

		std::ofstream file(file_path, std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Can not save file " + file_path);
		}

		file.write(content.c_str(), content.length());

		file.close();

	}

	std::string _load(const std::string& id) {

		std::string file_path = this->FilePath(id);

		std::ifstream file(file_path, std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Can not load file " + file_path);
		}

		std::ostringstream result_stream;
		result_stream << file.rdbuf();

		file.close();

		return result_stream.str();

	}

private:

	const std::string FilePath(const std::string& id) const {
		return this->folder_path + "/" + id + this->extension;
	}

	void loadKeys() {
		for (const auto & p : std::filesystem::directory_iterator(this->folder_path)) {
			if (p.path().extension() == this->extension) {
				this->keys.insert(p.path().stem().string());
			}
		}
	}

};