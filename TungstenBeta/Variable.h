#ifndef VARIABLE_H
#define VARIABLE_H

#include "Expression.h"

class Variable : public Expression {
public:
    // Список(словарь) всех переменных и их значений
    static std::unordered_map<std::string, const Expression*> variables;

    Variable(const std::string& name);

    double calculate() const override;
    std::string get_name() const { return name_; };

    const Expression* plug_variable(const std::string& variable) const override;
    const Expression* complex_derivative(const std::string& variable) const override;
    const Expression* simplify() const override;

    const Expression* copy() const override;
    std::string to_string() const override;

    void accept(ExpressionVisitor* visitor) override;

private:
    std::string name_;
};

#endif // VARIABLE_H