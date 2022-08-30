#pragma once

#include <ctime>
#include <vector>
#include <iostream>
#include <algorithm>



std::string RandomString(size_t length) {

	static std::string symbols = 
		"0123456789" 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
		"abcdefghijklmnopqrstuvwxyz";

	std::srand(time(NULL));

	std::string result;
	result.reserve(length);
	for (size_t i = 0; i < length; i++) {
		result += symbols[rand() % (symbols.length() - 1)];
	}

	return result;

}