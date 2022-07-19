#pragma once

#include <vector>
#include <iostream>

#include "../modules/rapidjson/writer.h"
#include "../modules/cpp-base64/base64.h"
#include "../modules/rapidjson/document.h"
#include "../modules/rapidjson/stringbuffer.h"

#include "Json.hpp"
#include "Storage.hpp"



template<class T>
class Item : public Json {

public:

	std::string data;
	T metadata;

	Item():
		data(std::string{}), metadata(T()) {}

	Item(std::string data, T metadata):
		data(data), metadata(metadata) {}

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

	std::string dataToJson() const {
		return "\"" + base64_encode(this->data) + "\"";
	}

	std::string toJson() const {
		return
		"{"
			"\"data\":" + this->dataToJson() + ","
			"\"metadata\":" +  this->metadata.toJson() + ""
		"}";
	}

};


template<class Metadata>
class ItemStorage : public Storage<Item<Metadata>> {

public:

	Storage<std::string>& data_storage;
	Storage<Metadata>& metadata_storage;

	ItemStorage(Storage<std::string>& data_storage, Storage<Metadata>& metadata_storage):
		data_storage(data_storage), metadata_storage(metadata_storage) {
		this->cache.disable();
	}

	void _save(const std::string& usi, const Item<Metadata>& item) {
		this->data_storage.save(usi, item.data);
		this->metadata_storage.save(usi, item.metadata);
	}

	Item<Metadata> _load(const std::string& usi) {
		return Item<Metadata>(
			this->data_storage.load(usi),
			this->metadata_storage.load(usi)
		);
	}

private:

	void loadKeys() {
		for (const auto & k : this->data_storage) {
			this->keys.insert(k);
		}
	}

};