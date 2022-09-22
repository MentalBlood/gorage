#pragma once

#ifndef __GORAGE__ITEM_STORAGE__
#define __GORAGE__ITEM_STORAGE__

#include <any>
#include <memory>
#include <vector>
#include <iostream>

#include "../modules/rapidjson/writer.h"
#include "../modules/rapidjson/document.h"
#include "../modules/rapidjson/stringbuffer.h"
#include "../modules/cppcodec/base64_rfc4648.hpp"

#include "Json.hpp"
#include "gorage.hpp"
#include "Storage.hpp"



namespace gorage {

	/**
	 * @brief Abstract class for arbitrary objects to formalize storing
	 * 
	 * @tparam T Metadata object type, should be inherited from Json
	 */
	template<class T>
	class Item : public Json {

	public:

		/**
		 * @brief Arbitrary bytes
		 * 
		 */
		Bytes data;
		/**
		 * @brief Arbitrary structured JSONable data
		 * 
		 */
		T metadata;

		/**
		 * @brief Construct a new Item object with empty data and metadata
		 * 
		 */
		Item():
			data({}), metadata(T()) {}

		/**
		 * @brief Construct a new Item object
		 * 
		 * @param data Arbitrary bytes
		 * @param metadata Arbitrary structured JSONable data
		 */
		Item(Bytes data, T metadata):
			data(data), metadata(metadata) {}

		Item(std::string data_base64, T metadata):
			data(
				cppcodec::base64_rfc4648::decode(
					data_base64
				)
			),
			metadata(metadata) {}

		/**
		 * @brief Updates data and metadata from corresponding structure
		 * 
		 * @param structure structure to update from. JSON form should be like {"data": <base64 string>, "metadata": <dict corresponding to T>}
		 */
		void update(const std::any& structure) {

			data = std::any_cast<Bytes>(
				std::any_cast<Dict>(structure)["data"]
			);
			metadata = Json::create<T>(
				std::any_cast<Dict>(structure)["metadata"]
			);

		}

	protected:

		std::any _getStructure() const {
			return Dict{
				{"data", data},
				{"metadata", metadata}
			};
		}

	};


	/**
	 * @brief Items storage. Uses separate storages for data and metadata
	 * 
	 * @tparam Metadata Items metadata type
	 */
	template<class Metadata>
	class ItemStorage : public Storage<Item<Metadata>> {

	public:

		/**
		 * @brief Storage for items data
		 * 
		 */
		std::shared_ptr<Storage<Bytes>> data_storage;
		/**
		 * @brief Storage for items metadata
		 * 
		 */
		std::shared_ptr<Storage<Metadata>> metadata_storage;

		/**
		 * @brief Construct a new ItemStorage object
		 * 
		 * @param data_storage Storage for items data
		 * @param metadata_storage Storage for items metadata
		 */
		ItemStorage(std::shared_ptr<Storage<Bytes>> data_storage, std::shared_ptr<Storage<Metadata>> metadata_storage):
			data_storage(data_storage),
			metadata_storage(metadata_storage) {}

		/**
		 * @brief Saves item with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @param item Item to save
		 */
		void save(const std::string& usi, const Item<Metadata>& item) {
			data_storage->save(usi, item.data);
			metadata_storage->save(usi, item.metadata);
		}

		/**
		 * @brief Loads item with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 * @return Item<Metadata> Loaded item
		 */
		Item<Metadata> load(const std::string& usi) {
			return Item<Metadata>(
				data_storage->load(usi),
				metadata_storage->load(usi)
			);
		}

		/**
		 * @brief Removes item with given USI
		 * 
		 * @param usi Unique Storage Identifier
		 */
		void remove(const std::string& usi) {
			data_storage->remove(usi);
			metadata_storage->remove(usi);
		}

	private:

		/**
		 * @brief Loads USIs for iteration
		 * 
		 */
		void loadUsis() {
			_usis.clear();
			for (const auto & k : *data_storage) {
				_usis.insert(k);
			}
		}

	};

} // gorage

#endif