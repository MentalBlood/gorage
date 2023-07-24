#pragma once

#include <regex>
#include <iostream>
#include <unordered_set>

#include "Usi.hpp"
#include "Json.hpp"
#include "Bytes.hpp"
#include "exceptions.hpp"
#include "RandomName.hpp"



namespace gorage {

	template<class T>
	class Storage {

	public:

		virtual void save(const Usi& usi, const T& object) = 0;

		virtual T load(const Usi& usi) const = 0;

		virtual bool exists(const Usi& usi) const = 0;

		T load(const Usi& usi, T default_) {
			try {
				return load(usi);
			} catch(const exceptions::Base& e) {
				throw e;
			} catch(const std::exception& e) {
				save(usi, default_);
				return default_;
			}
		}

		virtual void remove(const Usi& usi) = 0;

		virtual std::vector<Usi> usis() const = 0;

	};

} // gorage