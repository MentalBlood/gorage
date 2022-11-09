/**
 * Этот файл является частью проекта cryptotm
 *
 * Автор: Нечепоренко Степан <necheporenko_s_iu@goznak.ru>
 * Copyright 2022 АО Гознак
 */
#pragma once

#include <iostream>
#include <unordered_map>

#include "gorage.hpp"
#include "Storage.hpp"


namespace gorage {

	/**
	 * @brief Storage for arbitrary objects, uses std::unordered_map
	 * 
	 * @tparam T Objects type
	 */
	template<class T>
	class MemoryStorage : public Storage<T> {

	public:

		/**
		 * @brief Construct a new Memory Storage object
		 * 
		 */
		MemoryStorage() {}

		/**
		 * @brief Saves given object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @param object Arbitrary object
		 */
		virtual void save(const std::string& usi, const T& object) {
			_storage[usi] = object;
		}

		/**
		 * @brief Loads object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @return T Object type
		 */
		T load(const std::string& usi) {
			return _storage.at(usi);
		}

		/**
		 * @brief Removes object with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 */
		void remove(const std::string& usi) {
			_storage.erase(usi);
		}

	protected:

		/**
		 * @brief Loads USIs for iteration
		 * 
		 */
		void loadUsis() {
			_usis.clear();
			for (const auto & k : _storage) {
				_usis.insert(k.first);
			}
		}

	private:

		/**
		 * @brief Map to store objects in
		 * 
		 */
		std::unordered_map<std::string, T> _storage;

	};

} // gorage