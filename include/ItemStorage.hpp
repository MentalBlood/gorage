#pragma once

#ifndef __GORAGE__ITEM_STORAGE__
#define __GORAGE__ITEM_STORAGE__

#include <any>
#include <memory>
#include <vector>
#include <optional>
#include <iostream>

#include "Key.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
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
				Json::String(data_base64).decoded()
			),
			metadata(metadata) {}

		/**
		 * @brief Updates data and metadata from corresponding structure
		 * 
		 * @param structure structure to update from. JSON form should be like {"data": <base64 string>, "metadata": <dict corresponding to T>}
		 */
		void update(const std::any& structure) {

			data = std::any_cast<String>(
				std::any_cast<Dict>(structure)["data"]
			).decoded();
			metadata = Json::create<T>(
				std::any_cast<Dict>(structure)["metadata"]
			);

		}

		std::any getStructure() const {
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
		 * @brief Saves item with given key
		 * 
		 * @param key unique storage identifier
		 * @param item Item to save
		 */
		void save(const Key& key, const Item<Metadata>& item) {
			data_storage->save(key, item.data);
			metadata_storage->save(key, item.metadata);
		}

		/**
		 * @brief Loads item with given key
		 * 
		 * @param key unique storage identifier
		 * @return Item<Metadata> Loaded item
		 */
		Item<Metadata> load(const Key& key) {
			return Item<Metadata>(
				data_storage->load(key),
				metadata_storage->load(key)
			);
		}

		/**
		 * @brief Removes item with given key
		 * 
		 * @param key unique storage identifier
		 */
		void remove(const Key& key) {
			data_storage->remove(key);
			metadata_storage->remove(key);
		}

		std::optional<Item<Metadata>> find(const std::string& key, const std::any& structure) {
			for (const auto& key : *this) {
				Item<Metadata> item = load(key);
				if (item.contains(key, structure)) {
					return item;
				}
			}
			return {};
		}

		std::optional<Item<Metadata>> metadata_find(const std::string& key, const std::any& structure) {
			for (const auto& key : *metadata_storage) {
				if (metadata_storage->load(key).contains(key, structure)) {
					return load(key);
				}
			}
			return {};
		}

	private:

		/**
		 * @brief Loads keys for iteration
		 * 
		 */
		void loadKeys() {
			_keys.clear();
			for (const auto & k : *data_storage) {
				_keys.insert(k);
			}
		}

	};

} // gorage

#endif