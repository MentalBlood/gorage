#pragma once



class C : public gorage::Json {

public:

	const std::string s;

	C():
		s("") {}

	C(const Structure& structure):
		s(
			std::any_cast<String>(
				std::any_cast<Dict>(structure.value())["s"]
			).s
		) {}

	virtual std::any structure() const { return
		Dict{
			{"s", s}
		};
	}

};