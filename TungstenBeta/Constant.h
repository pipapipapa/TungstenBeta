#ifndef CONSTANT_H
#define CONSTANT_H

#include "Expression.h"

class Constant : public Expression {
public:
    static const Expression* e;
    static const Expression* pi;
    static const Expression* ZERO;
    static const Expression* ONE;

    Constant(long long value);
    Constant();

    double calculate() const override;
    int get_exact_value() const;
    const Expression* complex_derivative(const std::string& variable) const override;
    const Expression* plug_variable(const std::string& variable) const override;
    const Expression* copy() const override;
    const Expression* simplify() const override;
    std::string to_string() const override;

    void accept(ExpressionVisitor* visitor) override;

private:
    long long value_;
};

#endif // CONSTANT_H