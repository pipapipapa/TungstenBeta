#include "TungstenBeta.h"


namespace operators{

    // Sum
    Sum::Sum(std::vector<Expression*>&& terms){
        terms_ = terms;
    }

    Sum::~Sum(){
        for (Expression* term : terms_){
            delete term;
        }
    }

    Expression* Sum::copy() const{
        std::vector<Expression*> clonedTerms;
        for (const Expression* term : terms_){
            clonedTerms.push_back(term->copy());
        }
        return new Sum(std::move(clonedTerms));
    }

    double Sum::get_value() const{
        double result = 0;

        for (const Expression* term : terms_){
            result += term->get_value();
        }

        return result;
    }

    Expression* Sum::complex_derivative(const std::string& variable) const{
        std::vector<Expression*> derivedTerms;

        for (const Expression* term : terms_){
            derivedTerms.push_back(term->complex_derivative(variable));
        }

        return new Sum(std::move(derivedTerms));
    }


    // Product
    Product::Product(std::vector<Expression*>&& factors){
        factors_= factors;
    }

    Product::~Product(){
        for (Expression* factor : factors_){
            delete factor;
        }
    }

    Expression* Product::copy() const{
        std::vector<Expression*> clonedFactors;
        for (const Expression* factor : factors_){
            clonedFactors.push_back(factor->copy());
        }
        return new Product(std::move(clonedFactors));
    }

    double Product::get_value() const{
        double result = 1;
        for (const Expression* factor : factors_){
            result *= factor->get_value();
        }
        return result;
    }

    Expression* Product::complex_derivative(const std::string& variable) const{
        std::vector<Expression*> derivedFactors;
        
        for (int i = 0; i < factors_.size(); ++i){

            std::vector<Expression*> otherFactors;
            for (int j = 0; j < factors_.size(); ++j){
                if (i != j){
                    otherFactors.push_back(factors_[j]->copy());
                }
            }

            Expression* productTerm = new Product(std::move(otherFactors));
            derivedFactors.push_back(new Product({productTerm, factors_[i]->complex_derivative(variable)}));
        }

        return new Sum(std::move(derivedFactors));
    }


    // Fraction
    Fraction::Fraction(Expression* dividend, Expression* divisor){
        dividend_ = dividend;
        divisor_ = divisor;
    }

    Fraction::~Fraction(){
        delete dividend_;
        delete divisor_;
    }

    double Fraction::get_value() const{
        return dividend_->get_value() / divisor_->get_value();
    }

    Expression* Fraction::copy() const{
        return new Fraction(dividend_->copy(), divisor_->copy());
    }

    Expression* Fraction::complex_derivative(const std::string& variable) const{
        Expression* numerator = new Sum({
            new Product({dividend_->complex_derivative(variable), divisor_->copy()}),
            new Product({dividend_->copy(), new Product({new Constant(-1), divisor_->complex_derivative(variable)})}),
        });

        Expression* denominator = new Product({divisor_->copy(), divisor_->copy()});
        return new Fraction(numerator, denominator);
    }
};


// Constant
const double Constant::e = 2.718281828459045;
const double Constant::pi = 3.141592653589793;

Constant::Constant(int value){
    value_ = value;
}

double Constant::get_value() const{
    return value_;
}

Expression* Constant::copy() const{
        return new Constant(value_);
}

Expression* Constant::complex_derivative(const std::string& variable) const{
    return new Constant(0);
}


// Variable
Variable::Variable(const std::string& name){
    name_ = name;
}

double Variable::get_value() const{
    auto it = Variable::variables.find(name_);
    return it->second->get_value();
}

Expression* Variable::complex_derivative(const std::string& variable) const{
    if (variable == name_){
        return new Constant(1);
    } 
    else{
        return new Constant(0);
    }
}

Expression* Variable::copy() const{
    return new Variable(name_);
}
//std::string Variable::toString() const override{ return name_; }


namespace ElementaryFunctions{
    Expression* ElementaryFunction::complex_derivative(const std::string& variable) const{
        return new operators::Product({this->complex_derivative(variable), (this->get_input())->complex_derivative(variable)});
    }

    ElementaryFunction::ElementaryFunction() = default;
    ElementaryFunction::~ElementaryFunction() = default;


    // Power
    Power::Power(Expression* base, Expression* power){
        base_ = base;
        power_ = power;
    }

    double Power::get_value() const{
        return std::pow(base_->get_value(), power_->get_value());
    }

    Expression* Power::copy() const{
        return new Power(input_->copy(), power_);
    }

    Expression* Power::derivative(const std::string& variable) const{
        if (power_->get_value() == 1){
            return new Constant(1);
        } 
        else
       {
            return new operators::Product({
                new Constant(power_->get_value()),
                new Power(base_->copy(), new Constant(power_->get_value() - 1))
            });
        }
    }

    Expression* Power::get_input() const{
        return base_;
    }


    // Exponent
    Exp::Exp(Expression* base, Expression* power){
        base_ = base;
        power_ = power;
    }

    double Exp::get_value() const{
        return std::exp(power_->get_value() * std::log(base_->get_value()));
    }

    Expression* Exp::copy() const{
        return new Exp(power_->copy(), base_);
    }

    Expression* Exp::derivative(const std::string& variable) const{
        return new operators::Product({
                new Exp(power_->copy(), base_),
                new Log(power_->copy(), base_)
            });
    }

    Expression* Exp::get_input() const{
        return power_;
    }


    // LOgarithm
    Log::Log(Expression* base, Expression* arg){
        base_ = base;
        arg_ = arg;
    }

    double Log::get_value() const{
        return std::log(arg_->get_value()) / std::log(base_->get_value());
    }

    Expression* Log::copy() const{
        return new Log(arg_->copy(), base_);
    }

    Expression* Log::derivative(const std::string& variable) const{
        return new operators::Fraction(
            new Constant(1),
            new operators::Product({
                arg_->copy(),
                new Log(double_to_fraction(Constant::e), double_to_fraction(base_->get_value()))
            }));
    }

    Expression* Log::get_input() const{
        return arg_;
    }
};


// overloading
Expression* operator+(const Expression& lhs, const Expression& rhs){
    return new operators::Sum({lhs.copy(), rhs.copy()});
}

Expression* operator-(const Expression& lhs, const Expression& rhs){
    return new operators::Sum({lhs.copy(), new operators::Product({new Constant(-1), rhs.copy()})});
}

Expression* operator*(const Expression& lhs, const Expression& rhs){
    return new operators::Product({lhs.copy(), rhs.copy()});
}

Expression* operator/(const Expression& lhs, const Expression& rhs){
    return new operators::Fraction(lhs.copy(), rhs.copy());
}


Expression* double_to_fraction(double value){
    int precision = 100000000;
    int numerator = static_cast<int>(value * precision);
    int denominator = precision;

    int gcd = std::gcd(numerator, denominator);

    numerator /= gcd;
    denominator /= gcd;

    return new operators::Fraction(new Constant(numerator), new Constant(denominator));
}

std::unordered_map<std::string, Expression*> Variable::variables;