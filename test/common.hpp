#pragma once

#include <string>

#include "../include/Json.hpp"

class C : public gorage::Json {
public:
  const std::string s;

  C() {}
  C(const Structure &structure) : s(get<String>(cast<Dict>(structure.value()), "s").s) {}

  virtual std::any structure() const { return Dict({{"s", s}}); }
};
class Complex : public gorage::Json {
public:
  long i;
  double d;
  std::string s;

  Complex() {}
  Complex(const long &i, const double &d, const std::string &s) : i(i), d(d), s(s) {}
  Complex(const Structure &structure) {
    const auto dict = cast<Dict>(structure.value());
    i = get<long>(dict, "i");
    d = get<double>(dict, "d");
    s = get<String>(dict, "s").s;
  }

  virtual std::any structure() const { return Dict({{"i", i}, {"d", d}, {"s", s}}); }
};
