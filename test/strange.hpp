#include <iostream>



class A {

public:

	void f(const int i) {return;}

};


class B: public A {

public:

	void f(const std::string& s, const int i) {}

};


TEST_CASE("testing some strange things") {

	B().A::f(1);

}