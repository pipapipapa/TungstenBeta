#ifndef ELEMENTARY_FUNCTIONS_H
#define ELEMENTARY_FUNCTIONS_H

#include "Expression.h"

namespace ElementaryFunctions {
    class ElementaryFunction : public Expression {
    protected:
        const Expression* input_;

    public:
        ElementaryFunction();
        ~ElementaryFunction();

        virtual const Expression* derivative(const std::string& variable) const = 0;
        virtual const Expression* get_input() const = 0;

        const Expression* complex_derivative(const std::string& variable) const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Power : public ElementaryFunction {
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
        const Expression* plug_variable(const std::string& variable) const override;
        double calculate() const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Exp : public ElementaryFunction {
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
        const Expression* plug_variable(const std::string& variable) const override;
        double calculate() const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Log : public ElementaryFunction {
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
        const Expression* plug_variable(const std::string& variable) const override;
        double calculate() const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Sin : public ElementaryFunction {
    private:
        const Expression* arg_;

    public:
        Sin(const Expression* arg);
        const Expression* get_arg() const { return arg_; };

        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        const Expression* plug_variable(const std::string& variable) const override;
        double calculate() const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Cos : public ElementaryFunction {
    private:
        const Expression* arg_;

    public:
        Cos(const Expression* arg);
        const Expression* get_arg() const { return arg_; };

        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        const Expression* plug_variable(const std::string& variable) const override;
        double calculate() const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Tan : public ElementaryFunction {
    private:
        const Expression* arg_;

    public:
        Tan(const Expression* arg);
        const Expression* get_arg() const { return arg_; };

        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        double calculate() const override;
        const Expression* plug_variable(const std::string& variable) const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };

    class Cot : public ElementaryFunction {
    private:
        const Expression* arg_;

    public:
        Cot(const Expression* arg);
        const Expression* get_arg() const { return arg_; };

        // elementary function
        const Expression* derivative(const std::string& variable) const override;
        const Expression* simplify() const override;
        const Expression* get_input() const override;

        // expression
        const Expression* plug_variable(const std::string& variable) const override;
        double calculate() const override;
        const Expression* copy() const override;
        std::string to_string() const override;

        void accept(ExpressionVisitor* visitor) override;
    };
};

#endif // ELEMENTARY_FUNCTIONS_H