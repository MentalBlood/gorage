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

	template<class T>
	class File {

	public:

		const std::filesystem::path path;

		File(const std::filesystem::path& path):
			path(path) {}

		bool exists() const { return
			std::filesystem::exists(path);
		}

		void remove() {
			std::filesystem::remove(path);
		}

		void write(const T& object) {

			if (!std::filesystem::exists(path.parent_path())) {
				std::filesystem::create_directories(path.parent_path());
			}

			if constexpr (std::is_same_v<T, Bytes>) {

				std::ofstream file(path, std::ios::trunc | std::ios::binary);
				if (!file.is_open()) {
					throw exceptions::CanNotWriteFile(path);
				}

				file.write(reinterpret_cast<const char*>(&object[0]), object.size());
				file.close();

			} else {

				std::ofstream file(path, std::ios::out | std::ios::trunc);
				if (!file.is_open()) {
					throw exceptions::CanNotWriteFile(path);
				}

				file << object.encoded();
				file.close();

			}

		}

		T read() const {

			if constexpr (std::is_same_v<T, Bytes>) {

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

			} else {

				std::ifstream file(path, std::ios::in);
				if (!file.is_open()) {
					throw exceptions::CanNotReadFile(path);
				}

				std::stringstream result_stream;
				result_stream << file.rdbuf();

				file.close();

				const std::string result(result_stream.str());

				if constexpr (std::is_same_v<T, std::string>) {
					return result;
				} else {
					return Json::from<T>(result);
				}

			}

		}

	};

} // gorage