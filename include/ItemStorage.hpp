#pragma once

#include <any>
#include <memory>
#include <vector>
#include <optional>
#include <iostream>

#include "../modules/rapidjson/writer.h"
#include "../modules/rapidjson/document.h"
#include "../modules/rapidjson/stringbuffer.h"
#include "../modules/cppcodec/base64_rfc4648.hpp"

#include "Usi.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
#include "Storage.hpp"



namespace gorage {

	template<class T>
	class Item : public Json {

	public:

		const Bytes data;
		const T metadata;

		Item():
			data({}), metadata(T()) {}

		explicit Item(const Bytes& data, const T& metadata):
			data(data),
			metadata(metadata) {}

		explicit Item(const std::string& data_base64, const T& metadata):
			Item(
				gorage::Json::String(data_base64).decoded(),
				metadata
			) {}

		explicit Item(const std::any& structure):
			data(
				std::any_cast<String>(
					std::any_cast<Dict>(structure)["data"]
				).decoded()
			),
			metadata(
				T(
					std::any_cast<Dict>(structure)["metadata"]
				)
			) {}

		std::any structure() const { return
			Dict{
				{"data", data},
				{"metadata", metadata}
			};
		}

	};

	template<class Metadata>
	class ItemStorage : public Storage<Item<Metadata>> {

	public:

		std::shared_ptr<Storage<Bytes>>    data_storage;
		std::shared_ptr<Storage<Metadata>> metadata_storage;

		explicit ItemStorage(std::shared_ptr<Storage<Bytes>> data_storage, std::shared_ptr<Storage<Metadata>> metadata_storage):
			data_storage(data_storage),
			metadata_storage(metadata_storage) {}

		void save(const Usi& usi, const Item<Metadata>& item) {
			data_storage->save(usi, item.data);
			metadata_storage->save(usi, item.metadata);
		}

		Item<Metadata> load(const Usi& usi) const {
			return Item<Metadata>(
				data_storage->load(usi),
				metadata_storage->load(usi)
			);
		}

		void remove(const Usi& usi) {
			data_storage->remove(usi);
			metadata_storage->remove(usi);
		}

		std::optional<std::pair<Usi, Item<Metadata>>> find(const std::string& key, const std::any& structure) const {
			for (const auto& usi : usis()) {
				Item<Metadata> item = load(usi);
				if (item.contains(key, structure)) {
					return {{usi, item}};
				}
			}
			return {};
		}

		std::optional<std::pair<Usi, Item<Metadata>>> metadata_find(const std::string& key, const std::any& structure) const {
			for (const auto& usi : usis()) {
				Metadata metadata(metadata_storage->load(usi));
				if (metadata.contains(key, structure)) {
					return {{
						usi,
						Item<Metadata>(
							data_storage->load(usi),
							metadata
						)
					}};
				}
			}
			return {};
		}

		virtual std::vector<Usi> usis() const {
			return metadata_storage->usis();
		}

	};

} // gorage