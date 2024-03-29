#ifndef TUNGSTENBETA_H
#define TUNGSTENBETA_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>

class Expression{
public:
    virtual double get_value() const = 0;
    virtual Expression* complex_derivative() const = 0;
    virtual Expression* copy() const = 0;
    //virtual std::string to_string() const = 0;
    virtual ~Expression() = default;

    friend Expression* operator+(const Expression& lhs, const Expression& rhs);
    friend Expression* operator-(const Expression& lhs, const Expression& rhs);
    friend Expression* operator*(const Expression& lhs, const Expression& rhs);
    friend Expression* operator/(const Expression& lhs, const Expression& rhs);
};


namespace operators{
    class Sum : public Expression{
    private:
        std::vector<Expression*> terms_;

    public:
        Sum(std::vector<Expression*>&& terms);
        ~Sum();

        double get_value() const override;
        Expression* complex_derivative() const override;
        Expression* copy() const override;
        //std::string to_string() const override;
    };


    class Product : public Expression{
    private:
        std::vector<Expression*> factors_;

    public:
        Product(std::vector<Expression*>&& factors);
        ~Product();

        double get_value() const override;
        Expression* complex_derivative() const override;
        Expression* copy() const override;
        //std::string to_string() const override;
    };


    class Fraction : public Expression{
    private:
        Expression* dividend_;
        Expression* divisor_;

    public:
        Fraction(Expression* dividend, Expression* divisor);
        ~Fraction();

        double get_value() const override;
        Expression* complex_derivative() const override;
        Expression* copy() const override;
        //std::string to_string() const override;
    };
}


class Constant : public Expression{
public:
    static const double e;
    static const double pi;

    Constant(int value);

    double get_value() const override;
    Expression* complex_derivative() const override;
    Expression* copy() const override;
    //std::string to_string() const override;

private:
    int value_;
};


namespace ElementaryFunctions{
    class ElementaryFunction : public Expression{
    protected:
        Expression* input_;

    public:
        ElementaryFunction() = default;
        ~ElementaryFunction() = default;

        virtual Expression* derivative() const;
        virtual Expression* get_input() const;

        Expression* complex_derivative() const override;
    };


    class Power : public ElementaryFunction{
    private:
        Expression* base_;
        Expression* power_;

    public:
        Power(Expression* base, Expression* power);

        // elementary function
        Expression* derivative() const override;
        Expression* get_input() const override;

        // expression
        double get_value() const override;
        Expression* copy() const override;
        //std::string to_string() const override;
    };


    class Exp : public ElementaryFunction{
    private:
        Expression* power_;
        Expression* base_;

    public:
        Exp(Expression* base, Expression* power);

        // elementary function
        Expression* derivative() const override;
        Expression* get_input() const override;

        // expression
        double get_value() const override;
        Expression* copy() const override;
        //std::string to_string() const override;
    };


    class Log : public ElementaryFunction{
    private:
        Expression* base_;
        Expression* arg_;

    public:
        Log(Expression* base, Expression* arg);

        // elementary function
        Expression* derivative() const override;
        Expression* get_input() const override;

        // expression
        double get_value() const override;
        Expression* copy() const override;
        //std::string to_string() const override;
    };
}

Expression* double_to_fraction(double value);

#endif // TUNGSTENBETA_H