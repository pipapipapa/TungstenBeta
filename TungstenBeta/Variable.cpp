#include "Variable.h"
#include "Constant.h"


// Variable
std::unordered_map<std::string, const Expression*> Variable::variables;

Variable::Variable(const std::string& name){
    name_ = name;
}

double Variable::calculate() const{
    auto it = Variable::variables.find(name_);
    if (it != variables.end()){
        return it->second->calculate();
    }
    else{
        return 0;
    }
}

const Expression* Variable::complex_derivative(const std::string& variable) const{
    if (variable == name_){
        return Constant::ONE;
    } 
    else{
        return Constant::ZERO;
    }
}

const Expression* Variable::plug_variable(const std::string& variable) const{
    if (variable == name_){
        return Variable::variables[name_];
    } 
    else{
        return this;
    }
}

const Expression* Variable::copy() const{
    return new Variable(name_);
}

const Expression* Variable::simplify() const{
    return this;
}

std::string Variable::to_string() const{
    return name_;
}