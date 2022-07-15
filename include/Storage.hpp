#pragma once

#include <iostream>
#include <unordered_map>



template<class T>
class Storage {

public:

	void save(const std::string& usi, const T& something) {
		this->cache[usi] = something;
		return this->_save(usi, something);
	}

	T load(const std::string& usi) {
		try {
			return this->cache.at(usi);
		} catch (std::out_of_range) {
			T result = this->_load(usi);
			this->cache[usi] = result;
			return result;
		}
	}

	void clearCache() {
		this->cache.clear();
	}

	auto begin() {
		return this->cache.begin();
	}

	auto end() {
		return this->cache.end();
	}

protected:

	virtual void _save(const std::string& usi, const T& something) = 0;
	virtual T _load(const std::string& usi) = 0;

private:

	std::unordered_map<std::string, T> cache;

};