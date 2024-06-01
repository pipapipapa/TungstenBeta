#include "Variable.h"
#include "Constant.h"

// Список(словарь) всех переменных и их значений
std::unordered_map<std::string, const Expression*> Variable::variables;

Variable::Variable(const std::string& name) {
    name_ = name;
}

double Variable::calculate() const {
    if (variables.find(name_) != variables.end()) {
        return variables[name_]->calculate();
    }
    return 0;
}

const Expression* Variable::plug_variable(const std::string& variable) const {
    if (variable == name_) {
        return this;
    }
    else {
        return new Constant(0);
    }
}

const Expression* Variable::complex_derivative(const std::string& variable) const {
    if (variable == name_) {
        return Constant::ONE;
    } else {
        return Constant::ZERO;
    }
}

const Expression* Variable::simplify() const {
    return this;
}

const Expression* Variable::copy() const {
    return new Variable(name_);
}

std::string Variable::to_string() const {
    return name_;
}

void Variable::accept(ExpressionVisitor* visitor) {
    visitor->visitVariable(this);
}