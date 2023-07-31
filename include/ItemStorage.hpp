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
		const T     metadata;

		Item():
			data({}), metadata(T()) {}

		explicit Item(const Bytes& data, const T& metadata):
			data(data),
			metadata(metadata) {}

		explicit Item(const std::any& structure):
			Item(
				get<String>(
					cast<Dict>(structure),
					"data"
				).decoded(),
				get_object<T>(
					cast<Dict>(structure),
					"metadata"
				)
			) {}

		std::any structure() const { return
			Dict{
				{"data",     data},
				{"metadata", metadata}
			};
		}

	};

	template<class Metadata>
	class Storage<Item<Metadata>> {

	public:

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

	};

} // gorage