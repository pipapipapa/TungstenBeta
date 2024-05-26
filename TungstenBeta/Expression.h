#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream> 
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <numeric>
#include <unordered_map>

class Expression{
public:
    virtual double calculate() const = 0;
    virtual const Expression* complex_derivative(const std::string& variable) const = 0;
    virtual const Expression* copy() const = 0;
    virtual const Expression* plug_variable(const std::string& variable) const = 0;
    virtual std::string to_string() const = 0;
    virtual const Expression* simplify() const = 0;
    virtual ~Expression() = default;

    friend const Expression* operator+(const Expression& lhs, const Expression& rhs);
    friend const Expression* operator-(const Expression& lhs, const Expression& rhs);
    friend const Expression* operator*(const Expression& lhs, const Expression& rhs);
    friend const Expression* operator/(const Expression& lhs, const Expression& rhs);
};

#endif // EXPRESSION_H