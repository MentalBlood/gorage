#pragma once

#ifndef __GORAGE__STORAGE__
#define __GORAGE__STORAGE__

#include <iostream>
#include <stdbool.h>
#include <unordered_set>
#include <unordered_map>



namespace gorage {

	/**
	 * @brief Abstract class base for all storages
	 * 
	 * @tparam T Type of stored objects
	 */
	template<class T>
	class Storage {

	public:

		/**
		 * @brief Saves object of type T with USI usi
		 * 
		 * @param usi Unique Storage Identifier
		 * @param object Object to store
		 */
		virtual void save(const std::string& usi, const T& object) = 0;

		/**
		 * @brief Saves object of type T with random generated USI
		 * 
		 * @param object Object to store
		 * @return std::string USI with wich object was saved
		 */
		std::string save(const T& object) {

			std::string usi = this->Usi(32); // 62 ^ 32 variants
			this->save(usi, object);

			return usi;

		}

		/**
		 * @brief Loads object stored with USI usi
		 * 
		 * @param usi Unique Storage Identifier
		 * @return T Found object
		 */
		virtual T load(const std::string& usi) = 0;

		/**
		 * @brief Removes object stored with USI usi
		 * 
		 * @param usi Unique Storage Identifier
		 */
		virtual void remove(const std::string& usi) = 0;

		/**
		 * @brief Iteration begin related method
		 * 
		 * @return auto USIs set begin iterator
		 */
		auto begin() {
			loadUsis();
			return _usis.begin();
		}

		/**
		 * @brief Iteration end related method
		 * 
		 * @return auto USIs set end iterator
		 */
		auto end() {
			return _usis.end();
		}

		size_t size() {
			loadUsis();
			return _usis.size();
		}

	protected:

		/**
		 * @brief Set to which USIs will be loaded for iteration
		 * 
		 */
		std::unordered_set<std::string> _usis;

		/**
		 * @brief Method to load USIs in usis set for iteration
		 * 
		 */
		virtual void loadUsis() = 0;

	private:

		std::string Usi(size_t length) {

			static std::string symbols = 
				"0123456789" 
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
				"abcdefghijklmnopqrstuvwxyz";

			std::srand(time(NULL));

			std::string result;
			result.reserve(length);
			for (size_t i = 0; i < length; i++) {
				result += symbols[rand() % (symbols.length() - 1)];
			}

			return result;

		}

	};

} // gorage

#endif