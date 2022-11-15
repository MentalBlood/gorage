/**
 * Этот файл является частью проекта cryptotm
 *
 * Автор: Нечепоренко Степан <necheporenko_s_iu@goznak.ru>
 * Copyright 2022 АО Гознак
 */
#pragma once

#include <iostream>
#include <unordered_map>

#include "Key.hpp"
#include "Bytes.hpp"
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
		 * @brief Saves given object with given key
		 * 
		 * @param key unique storage identifier
		 * @param object Arbitrary object
		 */
		virtual void save(const Key& key, const T& object) {
			_storage[key()] = object;
		}

		/**
		 * @brief Loads object with given key
		 * 
		 * @param key unique storage identifier
		 * @return T Object type
		 */
		T load(const Key& key) {
			return _storage.at(key());
		}

		/**
		 * @brief Removes object with given key
		 * 
		 * @param key unique storage identifier
		 */
		void remove(const Key& key) {
			_storage.erase(key());
		}

	protected:

		/**
		 * @brief Loads keys for iteration
		 * 
		 */
		void loadKeys() {
			_keys.clear();
			for (const auto & k : _storage) {
				_keys.insert(k.first);
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