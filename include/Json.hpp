#pragma once

#include <iostream>



/**
 * @brief Class for representing JSONable objects
 * 
 */
class Json {

public:

	/**
	 * @brief Updates data and metadata from corresponding JSON text
	 * 
	 * @param json_text JSON text to update from
	 */
	virtual void updateFromJson(const std::string& json_text) = 0;
	/**
	 * @brief Converts object to JSON
	 * 
	 * @return std::string JSONed object
	 */
	virtual std::string toJson() const = 0;

	/**
	 * @brief Converts JSON text to object of given type
	 * 
	 * @tparam T Object type
	 * @param json_text JSON text
	 * @return T Object corresponded to JSON text
	 */
	template<typename T>
	static T create(const std::string& json_text) {
		T json;
		json.updateFromJson(json_text);
		return json;
	}

};