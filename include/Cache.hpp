#pragma once

#include <iostream>
#include <stdbool.h>
#include <unordered_map>



template<class T>
class Cache {

public:

	Cache():
		enabled(true) {}

	void enable() {
		this->enabled = true;
	}
	void disable() {
		this->enabled = false;
	}

	void set(const std::string& key, const T& value) {
		if (this->enabled) {
			this->map[key] = value;
		}
	}
	T get(const std::string& key) {
		return this->map.at(key);
	}
	void remove(const std::string& key) {
		this->map.erase(key);
	}

	void clear() {
		this->map.clear();
	}

private:

	bool enabled;
	std::unordered_map<std::string, T> map;

};