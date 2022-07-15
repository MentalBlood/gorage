#pragma once

#include <sstream>
#include <fstream>
#include <iostream>

#include "Storage.hpp"



template<class T>
class FileStorage : public Storage<T> {

public:

	const std::string folder_path;
	const std::string extension;

	FileStorage(const std::string& folder_path, const std::string& extension):
		folder_path(folder_path), extension(extension) {}

protected:

	void _save(const std::string& id, const T& entity) {

		const std::string content = entity.toJson();

		std::string file_path = this->FilePath(id);

		std::ofstream file(file_path, std::ios::out | std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Can not save file " + file_path);
		}

		file << content;
		file.close();

	}

	T _load(const std::string& id) {

		std::string file_path = this->FilePath(id);

		std::ifstream file(file_path, std::ios::in);
		if (!file.is_open()) {
			throw std::runtime_error("Can not load file " + file_path);
		}

		std::stringstream result_stream;
		result_stream << file.rdbuf();

		file.close();

		T result;
		result.fromJson(result_stream.str());

		return result;

	}

private:

	const std::string FilePath(const std::string& id) const {
		return this->folder_path + "/" + id + this->extension;
	}

};