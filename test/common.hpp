#pragma once



class C : public gorage::Json {

public:

	const std::string s;

	C():
		s("") {}

	C(const Structure& structure):
		s(
			cast<String>(
				cast<Dict>(structure.value())["s"]
			).s
		) {}

	virtual std::any structure() const { return
		Dict{
			{"s", s}
		};
	}

};