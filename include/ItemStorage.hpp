#pragma once

#ifndef __GORAGE__ITEM_STORAGE__
#define __GORAGE__ITEM_STORAGE__

#include <memory>
#include <vector>
#include <iostream>

#include "../modules/rapidjson/writer.h"
#include "../modules/cpp-base64/base64.h"
#include "../modules/rapidjson/document.h"
#include "../modules/rapidjson/stringbuffer.h"

#include "Json.hpp"
#include "gorage.hpp"
#include "Storage.hpp"



/**
 * @brief Abstract class for arbitrary objects to formalize storing
 * 
 * @tparam T Metadata object type, should be inherited from Json
 */
template<class T>
class gorage::Item : public Json {

public:

	/**
	 * @brief Arbitrary bytes
	 * 
	 */
	std::string data;
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
		data(std::string{}), metadata(T()) {}

	/**
	 * @brief Construct a new Item object
	 * 
	 * @param data Arbitrary bytes
	 * @param metadata Arbitrary structured JSONable data
	 */
	Item(std::string data, T metadata):
		data(data), metadata(metadata) {}

	/**
	 * @brief Updates data and metadata from corresponding JSON text
	 * 
	 * @param json_text JSON text to update from. Should be like {"data": <base64 string>, "metadata": <dict corresponding to T>}
	 */
	void updateFromJson(const std::string& json_text) {

		std::cout << "json " << json_text << std::endl;

		rapidjson::Document json;
		json.Parse(json_text.c_str());

		std::string data_base64 = json["data"].GetString();

		this->data = std::string(base64_decode(data_base64));

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		json["metadata"].Accept(writer);
		this->metadata = Json::create<T>(std::string(buffer.GetString()));

	}

	/**
	 * @brief Converts data to JSON using base64
	 * 
	 * @return std::string JSONed data
	 */
	std::string dataToJson() const {
		return "\"" + base64_encode(this->data) + "\"";
	}

	/**
	 * @brief Converts the whole item to JSON
	 * 
	 * @return std::string JSONed item
	 */
	std::string toJson() const {
		return
		"{"
			"\"data\":" + this->dataToJson() + ","
			"\"metadata\":" +  this->metadata.toJson() + ""
		"}";
	}

};


/**
 * @brief Items storage. Uses separate storages for data and metadata
 * 
 * @tparam Metadata Items metadata type
 */
template<class Metadata>
class gorage::ItemStorage : public Storage<Item<Metadata>> {

public:

	/**
	 * @brief Storage for items data
	 * 
	 */
	std::shared_ptr<Storage<std::string>> data_storage;
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
	ItemStorage(std::shared_ptr<Storage<std::string>> data_storage, std::shared_ptr<Storage<Metadata>> metadata_storage):
		data_storage(data_storage),
		metadata_storage(metadata_storage) {}

	/**
	 * @brief Saves item with given USI
	 * 
	 * @param usi Unique Storage Identifier
	 * @param item Item to save
	 */
	void save(const std::string& usi, const Item<Metadata>& item) {
		this->data_storage->save(usi, item.data);
		this->metadata_storage->save(usi, item.metadata);
	}

	/**
	 * @brief Loads item with given USI
	 * 
	 * @param usi Unique Storage Identifier
	 * @return Item<Metadata> Loaded item
	 */
	Item<Metadata> load(const std::string& usi) {
		return Item<Metadata>(
			this->data_storage->load(usi),
			this->metadata_storage->load(usi)
		);
	}

	/**
	 * @brief Removes item with given USI
	 * 
	 * @param usi Unique Storage Identifier
	 */
	void remove(const std::string& usi) {
		this->data_storage->remove(usi);
		this->metadata_storage->remove(usi);
	}

private:

	/**
	 * @brief Loads USIs for iteration
	 * 
	 */
	void loadUsis() {
		for (const auto & k : this->data_storage) {
			this->usis.insert(k);
		}
	}

};

#endif