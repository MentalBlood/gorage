class C : public gorage::Json {

public:

	std::string s;

	virtual void update(const std::any& structure) {
		s = std::any_cast<std::string>(
			std::any_cast<Dict>(structure)["s"]
		);
	}

private:

	virtual std::any _getStructure() const {
		return Dict{
			{"s", s}
		};
	}

};