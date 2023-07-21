#pragma once

#include <iostream>
#include <unordered_map>

#include "Usi.hpp"
#include "Storage.hpp"
#include "exceptions.hpp"



namespace gorage {

	namespace exceptions {

		class KeyError : public Base {
		public:
			KeyError(const gorage::Usi& usi):
				Base("No object with usi `" + usi.value() + "`") {}
		};

	}

	template<class T>
	class MemoryStorage : public Storage<T> {

	public:

		MemoryStorage() {}

		virtual void save(const Usi& usi, const T& object) {
			_storage.erase(usi.value());
			_storage.emplace(usi.value(), object);
		}

		virtual T load(const Usi& usi) const { return
			_storage.at(usi.value());
		}

		virtual bool exists(const Usi& usi) const { return
			_storage.count(usi.value());
		}

		virtual void remove(const Usi& usi) {
			if (!_storage.count(usi.value())) {
				throw exceptions::KeyError(usi);
			}
			_storage.erase(usi.value());
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