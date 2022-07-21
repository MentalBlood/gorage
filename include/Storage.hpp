#pragma once

#include <iostream>
#include <stdbool.h>
#include <unordered_set>
#include <unordered_map>



template<class T>
class Storage {

public:

	virtual void save(const std::string& usi, const T& something) = 0;
	virtual T load(const std::string& usi) = 0;
	virtual void remove(const std::string& usi) = 0;

	auto begin() {
		this->loadKeys();
		return this->keys.begin();
	}

	auto end() {
		return this->keys.end();
	}

protected:

	std::unordered_set<std::string> keys;

private:

	void loadKeys() {};

};