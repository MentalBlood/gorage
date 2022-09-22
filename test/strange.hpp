#include <any>
#include <iostream>



class A {

public:

	template<class T>
	static T f(const std::string& s) {return T();};

	template<class T>
	static T f(const std::any& i) {return T();};

};


TEST_CASE("testing some strange things") {

	A::f<A>("lalala");

}