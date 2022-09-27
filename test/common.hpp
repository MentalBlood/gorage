#pragma once



class C : public gorage::Json {

public:

	std::string s;

	C():
		s("") {}

	C(const std::string& s):
		s(s) {}

	virtual void update(const std::any& structure) {
		s = std::any_cast<String>(
			std::any_cast<Dict>(structure)["s"]
		).s;
	}

	virtual std::any getStructure() const {
		return Dict{
			{"s", s}
		};
	}

};