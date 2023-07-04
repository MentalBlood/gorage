#pragma once

#include <any>
#include <memory>
#include <vector>
#include <optional>
#include <iostream>
#include <unordered_map>

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
				get<String>(
					cast<Dict>(structure),
					"data"
				).decoded()
			),
			metadata(
				get_object<T>(
					cast<Dict>(structure),
					"metadata"
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

		class Query {

		public:

			const std::unordered_map<std::string, std::any> keys;

			Query(const std::unordered_map<std::string, std::any>& keys):
				keys(keys) {}

			Query(const std::string& key, const std::any& structure):
				keys({
					{key, structure}
				}) {}

			bool match(const Metadata& metadata) const {
				for (const auto& q : keys) {
					if (!metadata.contains(q.first, q.second)) {
						return false;
					}
				}
				return true;
			}

		};

		explicit ItemStorage(std::shared_ptr<Storage<Bytes>> data_storage, std::shared_ptr<Storage<Metadata>> metadata_storage):
			data_storage(data_storage),
			metadata_storage(metadata_storage) {}

		void save(const Usi& usi, const Item<Metadata>& item) {
			data_storage->save(usi, item.data);
			metadata_storage->save(usi, item.metadata);
		}

		Item<Metadata> load(const Usi& usi) const { return
			Item<Metadata>(
				data_storage->load(usi),
				metadata_storage->load(usi)
			);
		}

		virtual bool exists(const Usi& usi) const { return
			data_storage->exists(usi) && metadata_storage->exists(usi);
		}

		void remove(const Usi& usi) {
			data_storage->remove(usi);
			metadata_storage->remove(usi);
		}

		std::optional<std::pair<Usi, Item<Metadata>>> find(const Query& query) const {
			for (const auto& usi : usis()) {
				const Metadata metadata(metadata_storage->load(usi));
				if (query.match(metadata)) {
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

		virtual std::vector<Usi> usis() const { return
			metadata_storage->usis();
		}

	};

} // gorage