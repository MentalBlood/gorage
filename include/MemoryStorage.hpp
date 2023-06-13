#pragma once

#include <iostream>
#include <unordered_map>

#include "Usi.hpp"
#include "Storage.hpp"


namespace gorage {

	template<class T>
	class MemoryStorage : public Storage<T> {

	public:

		MemoryStorage() {}

		virtual void save(const Usi& usi, const T& object) {
			_storage.erase(usi());
			_storage.emplace(usi(), object);
		}

		virtual T load(const Usi& usi) const { return
			_storage.at(usi());
		}

		virtual void remove(const Usi& usi) {
			if (!_storage.count(usi())) {
				throw OperationalError("No object with usi '" + usi() + "' to remove");
			}
			_storage.erase(usi());
		}

		virtual std::vector<Usi> usis() const {
			std::vector<Usi> result;
			for (const auto& pair : _storage) {
				result.push_back(Usi(pair.first));
			}
			return result;
		}

	private:
		std::unordered_map<std::string, T> _storage;

	};

} // gorage