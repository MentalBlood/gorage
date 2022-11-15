#pragma once

#ifndef __GORAGE__STORAGE__
#define __GORAGE__STORAGE__

#include <iostream>
#include <unordered_set>

#include "Usi.hpp"



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
		 * @brief Saves object of type `T` with USI `usi`
		 * 
		 * @param usi Unique Storage Identifier
		 * @param object Object to store
		 */
		virtual void save(const Usi& usi, const T& object) = 0;

		/**
		 * @brief Loads object stored with USI usi
		 * 
		 * @param usi Unique Storage Identifier
		 * @return T Found object
		 */
		virtual T load(const Usi& usi) = 0;

		/**
		 * @brief Removes object stored with USI usi
		 * 
		 * @param usi Unique Storage Identifier
		 */
		virtual void remove(const Usi& usi) = 0;

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
		 * @brief Method to load USIs in `usis` set for iteration
		 * 
		 */
		virtual void loadUsis() = 0;

	};

} // gorage

#endif