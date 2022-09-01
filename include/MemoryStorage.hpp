/**
 * Этот файл является частью проекта cryptotm
 *
 * Автор: Нечепоренко Степан <necheporenko_s_iu@goznak.ru>
 * Copyright 2022 АО Гознак
 */
#pragma once

#include <iostream>
#include <unordered_map>

#include "Storage.hpp"



template<class T>
class MemoryStorage : public Storage<T> {

public:

	MemoryStorage() {}

protected:

	void save(const std::string& usi, const T& object) {
		this->storage[usi] = object;
	}

	T load(const std::string& usi) {
		return this->storage.at(usi);
	}

	void remove(const std::string& usi) {
		this->storage.at(usi);
		this->storage.erase(usi);
	}

private:
	std::unordered_map<std::string, T> storage;

};