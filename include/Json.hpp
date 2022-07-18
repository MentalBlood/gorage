#pragma once

#include <iostream>



class Json {

public:

	virtual void updateFromJson(const std::string& json_text) = 0;
	virtual std::string toJson() const = 0;

	template<typename T>
	static T create(const std::string& json_text) {
		T json;
		json.updateFromJson(json_text);
		return json;
	}

};