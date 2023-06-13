#pragma once



class C : public gorage::Json {

public:

	const std::string s;

	C():
		s("") {}

	C(const std::any& structure):
		s(
			std::any_cast<String>(
				std::any_cast<Dict>(structure)["s"]
			).s
		) {}

	virtual std::any structure() const { return
		Dict{
			{"s", s}
		};
	}

};