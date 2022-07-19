#pragma once

#include <iostream>
#include <stdbool.h>
#include <unordered_set>
#include <unordered_map>

#include "Cache.hpp"



template<class T>
class Storage {

public:

	Cache<T> cache;

	Storage() {
		this->loadKeys();
	}

	void save(const std::string& usi, const T& something) {
		this->cache.set(usi, something);
		this->keys.insert(usi);
		return this->_save(usi, something);
	}

	T load(const std::string& usi) {

		try {

			T result = this->cache.get(usi);
			this->keys.insert(usi);
			return result;

		} catch (std::out_of_range) {

			T result = this->_load(usi);
			this->cache.set(usi, result);
			this->keys.insert(usi);
			return result;

		}

	}

	void remove(const std::string& usi) {
		this->_remove(usi);
		this->cache.remove(usi);
		this->keys.erase(usi);
	}

	auto begin() {
		return this->keys.begin();
	}

	auto end() {
		return this->keys.end();
	}

protected:

	std::unordered_set<std::string> keys;

	virtual void _save(const std::string& usi, const T& something) = 0;
	virtual T _load(const std::string& usi) = 0;
	virtual void _remove(const std::string& usi) = 0;

private:

	void loadKeys() {};

};