#ifndef TUNGSTENBETA_H
#define TUNGSTENBETA_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <numeric>
#include <unordered_map>

class Expression{
public:
    virtual double get_value() const = 0;
    virtual const Expression* complex_derivative(const std::string& variable) const = 0;
    virtual const Expression* copy() const = 0;
    virtual std::string to_string() const = 0;
    virtual const Expression* simplify() const = 0;
    virtual ~Expression() = default;

    friend const Expression* operator+(const Expression& lhs, const Expression& rhs);
    friend const Expression* operator-(const Expression& lhs, const Expression& rhs);
    friend const Expression* operator*(const Expression& lhs, const Expression& rhs);
    friend const Expression* operator/(const Expression& lhs, const Expression& rhs);
};


namespace operators{
    class Sum : public Expression{
    private:
        std::vector<const Expression*> terms_;

    public:
        Sum(std::vector<const Expression*>&& terms);
        ~Sum();
        std::vector<const Expression*> get_terms() const { return terms_; };

        double get_value() const override;
        const Expression* complex_derivative(const std::string& variable) const override;
        const Expression* copy() const override;
        const Expression* simplify() const override;
        std::string to_string() const override;
    };


    class Product : public Expression{
    private:
        std::vector<const Expression*> factors_;

    public:
        //Product(std::vector<const Expression*>&& factors);
        Product(std::vector<const Expression*> factors);
        ~Product();

        std::vector<const Expression*> get_factors() const { return factors_; };

        double get_value() const override;
        const Expression* simplify() const override;
        const Expression* complex_derivative(const std::string& variable) const override;
        const Expression* copy() const override;
        std::string to_string() const override;
    };


    class Fraction : public Expression{
    private:
        const Expression* dividend_;
        const Expression* divisor_;

    public:
        Fraction(const Expression* dividend, const Expression* divisor);
        ~Fraction();

        double get_value() const override;
        const Expression* get_dividend() const { return dividend_; };
        const Expression* get_divisor() const { return divisor_; };

        const Expression* simplify() const override;
        const Expression* complex_derivative(const std::string& variable) const override;
        const Expression* copy() const override;
        std::string to_string() const override;
    };
}


class Constant : public Expression{
public:
    static const double e;
    static const double pi;
    static const Expression* ZERO;
    static const Expression* ONE;

    Constant(long long value);

    double get_value() const override;
    int get_exact_value() const;
    const Expression* complex_derivative(const std::string& variable) const override;
    const Expression* copy() const override;
    const Expression* simplify() const override;
    std::string to_string() const override;

private:
    long long value_;
};


class Variable : public Expression{
public:
    // Список(словарь) всех переменных и их значений
    static std::unordered_map<std::string, const Expression*> variables;

    Variable(const std::string& name);

    double get_value() const override;
    std::string get_name() const { return name_; };

    const Expression* complex_derivative(const std::string& variable) const override;
    const Expression* simplify() const override;

    const Expression* copy() const override;
    std::string to_string() const override;

    private:
    std::string name_;
};


namespace ElementaryFunctions{
    class ElementaryFunction : public Expression{
    protected:
        const Expression* input_;

    public:
        ElementaryFunction();
        ~ElementaryFunction();

        virtual const Expression* derivative(const std::string& variable) const = 0;
        virtual const Expression* get_input() const = 0;

        const Expression* complex_derivative(const std::string& variable) const override;
    };


    class Power : public ElementaryFunction{
    private:
        const Expression* base_;
        const Expression* power_;

    public:
        Power(const Expression* base, const Expression* power);
        const Expression* get_base() const { return base_; };
        const Expression* get_power() const { return power_; };

        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        double get_value() const override;
        const Expression* copy() const override;
        std::string to_string() const override;
    };


    class Exp : public ElementaryFunction{
    private:
        const Expression* power_;
        const Expression* base_;

    public:
        Exp(const Expression* base, const Expression* power);
        const Expression* get_base() const { return base_; };
        const Expression* get_power() const { return power_; };

        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        double get_value() const override;
        const Expression* copy() const override;
        std::string to_string() const override;
    };


    class Log : public ElementaryFunction{
    private:
        const Expression* base_;
        const Expression* arg_;

    public:
        Log(const Expression* base, const Expression* arg);
        const Expression* get_base() const { return base_; };
        const Expression* get_arg() const { return arg_; };


        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        double get_value() const override;
        const Expression* copy() const override;
        std::string to_string() const override;
    };
}

const Expression* double_to_fraction(double value);

const Expression* Taylor_series(const Expression* f, const std::string& variable, double point);

bool hasVariables(const Expression* expr);

#endif // TUNGSTENBETA_H