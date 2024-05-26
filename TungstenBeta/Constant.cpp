#include "Constant.h"

// Constant
const Expression* Constant::e = new Constant;
const Expression* Constant::pi = new Constant;
const Expression* Constant::ZERO = new Constant(0);
const Expression* Constant::ONE = new Constant(1);

Constant::Constant(long long value){
    value_ = value;
}

Constant::Constant(){
}

double Constant::calculate() const{
    if (this == Constant::e){
        return 2.718281828;
    }
    return value_;
}

int Constant::get_exact_value() const{
    return value_;
}

const Expression* Constant::simplify() const{
    return this;
}

const Expression* Constant::plug_variable(const std::string& variable) const{
    return this;
}

const Expression* Constant::copy() const{
    return new Constant(value_);
}

const Expression* Constant::complex_derivative(const std::string& variable) const{
    return Constant::ZERO;
}

std::string Constant::to_string() const{
    if (this == Constant::e){
        return "e";
    }
    if (value_ >= 0){
        return std::to_string(value_);
    }
    else{
        return "(" + std::to_string(value_) + ")";
    }
}