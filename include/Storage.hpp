#pragma once

#ifndef __GORAGE__STORAGE__
#define __GORAGE__STORAGE__

#include <regex>
#include <iostream>
#include <unordered_set>

#include "Json.hpp"
#include "RandomName.hpp"



namespace gorage {

	using Bytes = std::vector<unsigned char>;

	/**
	 * @brief Abstract class base for all storages
	 *
	 * @tparam T Type of stored objects
	 */
	template<class T>
	class Storage {

	public:

		class OperationalError : public std::exception {
		public:
			OperationalError(const std::string message):
				std::exception(message.c_str()) {}
		};

		/**
		 * @brief Saves object of type `T` with USI `usi`
		 *
		 * @param usi Unique Storage Identifier
		 * @param object Object to store
		 */
		virtual void save(const std::string& usi, const T& object) = 0;

		/**
		 * @brief Saves object of type `T` with USI corresponding to `usi_source`
		 *
		 * @param usi_source Some bytes to generate USI from
		 * @param object Object to store
		 * @return std::string USI with which object was saved
		 */
		std::string save(const Bytes& usi_source, const T& object) {

			std::string usi = Usi(usi_source);
			save(usi, object);

			return usi;

		}

		/**
		 * @brief Saves object of type T with random generated USI
		 *
		 * @param object Object to store
		 * @return std::string USI with which object was saved
		 */
		std::string save(const T& object) {

			std::string usi = Usi(32); // 62 ^ 32 variants
			save(usi, object);

			return usi;

		}

		/**
		 * @brief Loads object stored with USI usi
		 *
		 * @param usi Unique Storage Identifier
		 * @return T Found object
		 */
		virtual T load(const std::string& usi) const = 0;

		T load(const Bytes& usi_source) const {
			return load(
				Usi(usi_source)
			);
		}

		T load(const std::string& usi, T default_) {
			try {
				return load(usi);
			} catch(const OperationalError& e) {
				throw e;
			} catch(const std::exception& e) {
				save(usi, default_);
				return default_;
			}
		}

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
			return this->_usis.begin();
		}

		/**
		 * @brief Iteration end related method
		 *
		 * @return auto USIs set end iterator
		 */
		auto end() {
			return this->_usis.end();
		}

		size_t size() {
			loadUsis();
			return this->_usis.size();
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

	private:

		static std::string Usi(size_t length){
			return gorage::RandomName(length).str();
		}

		static std::string Usi(const Bytes& source){
			return std::regex_replace(
				Json::String(source).encoded(),
				std::regex("[^\\w|\\d]"),
				"_"
			);
		}

	};

} // gorage

#endif