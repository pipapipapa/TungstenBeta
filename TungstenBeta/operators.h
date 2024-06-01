#ifndef OPERATORS_H
#define OPERATORS_H

#include "Expression.h"

namespace operators {
    class Sum : public Expression {
    private:
        std::vector<const Expression*> terms_;

    public:
        Sum(std::vector<const Expression*>&& terms);
        ~Sum();

        std::vector<const Expression*> get_terms() const { return terms_; };

        double calculate() const override;
        const Expression* complex_derivative(const std::string& variable) const override;
        const Expression* copy() const override;
        const Expression* plug_variable(const std::string& variable) const override;
        const Expression* simplify() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Product : public Expression {
    private:
        std::vector<const Expression*> factors_;

    public:
        Product(std::vector<const Expression*> factors);
        ~Product();

        std::vector<const Expression*> get_factors() const { return factors_; };

        double calculate() const override;
        const Expression* simplify() const override;
        const Expression* plug_variable(const std::string& variable) const override;
        const Expression* complex_derivative(const std::string& variable) const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Fraction : public Expression {
    private:
        const Expression* dividend_;
        const Expression* divisor_;

    public:
        Fraction(const Expression* dividend, const Expression* divisor);
        ~Fraction();

        double calculate() const override;
        const Expression* get_dividend() const { return dividend_; };
        const Expression* get_divisor() const { return divisor_; };

        const Expression* simplify() const override;
        const Expression* plug_variable(const std::string& variable) const override;
        const Expression* complex_derivative(const std::string& variable) const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };
}

#endif // OPERATORS_H