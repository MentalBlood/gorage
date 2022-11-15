#pragma once

#ifndef __GORAGE__STORAGE__
#define __GORAGE__STORAGE__

#include <iostream>
#include <unordered_set>

#include "Key.hpp"



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
		 * @brief Saves object of type `T` with key `key`
		 * 
		 * @param key unique storage identifier
		 * @param object Object to store
		 */
		virtual void save(const Key& key, const T& object) = 0;

		/**
		 * @brief Loads object stored with key key
		 * 
		 * @param key unique storage identifier
		 * @return T Found object
		 */
		virtual T load(const Key& key) = 0;

		/**
		 * @brief Removes object stored with key key
		 * 
		 * @param key unique storage identifier
		 */
		virtual void remove(const Key& key) = 0;

		/**
		 * @brief Iteration begin related method
		 * 
		 * @return auto keys set begin iterator
		 */
		auto begin() {
			loadKeys();
			return _keys.begin();
		}

		/**
		 * @brief Iteration end related method
		 * 
		 * @return auto keys set end iterator
		 */
		auto end() {
			return _keys.end();
		}

		size_t size() {
			loadKeys();
			return _keys.size();
		}

	protected:

		/**
		 * @brief Set to which keys will be loaded for iteration
		 * 
		 */
		std::unordered_set<std::string> _keys;

		/**
		 * @brief Method to load keys in `keys` set for iteration
		 * 
		 */
		virtual void loadKeys() = 0;

	};

} // gorage

#endif