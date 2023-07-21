#pragma once

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "Usi.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"



namespace gorage {

	namespace exceptions {

		class CanNotReadFile : public Base {
		public:
			explicit CanNotReadFile(const std::filesystem::path& path):
				Base("Can not read file `" + path.string() + "`") {}
		};

		class CanNotWriteFile : public Base {
		public:
			explicit CanNotWriteFile(const std::filesystem::path& path):
				Base("Can not write file `" + path.string() + "`") {}
		};

	}

	class File {

	public:

		const std::filesystem::path path;

		File(const std::filesystem::path& path):
			path(path) {}

		template<typename T>
		void write(const T& object) {

			std::ofstream file(path, std::ios::out | std::ios::trunc);
			if (!file.is_open()) {
				throw exceptions::CanNotWriteFile(path);
			}

			file << object.encoded();
			file.close();

		}

		template<>
		void write<Bytes>(const Bytes& bytes) {

			std::ofstream file(path, std::ios::trunc | std::ios::binary);
			if (!file.is_open()) {
				throw exceptions::CanNotWriteFile(path);
			}

			file.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
			file.close();
		}

		template<typename T>
		T read() {

			std::ifstream file(path, std::ios::in);
			if (!file.is_open()) {
				throw exceptions::CanNotReadFile(path);
			}

			std::stringstream result_stream;
			result_stream << file.rdbuf();

			file.close();

			return Json::from<T>(result_stream.str());

		}

		template<>
		Bytes read<Bytes>() {

			std::ifstream file(path, std::ios::binary | std::ios::ate);
			if (!file.is_open()) {
				throw exceptions::CanNotReadFile(path);
			}
			std::ifstream::pos_type length = file.tellg();
			if (length == 0) {
				return Bytes();
			}

			Bytes result(length);
			file.seekg(0, std::ios::beg);
			file.read(reinterpret_cast<char*>(&result[0]), length);
			return result;

		}

	};

} // gorage