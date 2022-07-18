#pragma once

#include <vector>
#include <iostream>

#include "Storage.hpp"



template<class T>
class Item {

public:

	std::vector<uint8_t> data;
	T metadata;

	Item():
		data(std::vector<uint8_t>{}), metadata(T()) {}

	Item(std::vector<uint8_t> data, T metadata):
		data(data), metadata(metadata) {}

};


template<class Metadata>
class ItemStorage : public Storage<Item<Metadata>> {

public:

	Storage<std::vector<uint8_t>>& data_storage;
	Storage<Metadata>& metadata_storage;

	ItemStorage(Storage<std::vector<uint8_t>>& data_storage, Storage<Metadata>& metadata_storage):
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