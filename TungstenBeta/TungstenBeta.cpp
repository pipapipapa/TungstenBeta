#include "TungstenBeta.h"


namespace operators{

    // Sum
    Sum::Sum(std::vector<const Expression*>&& terms){
        terms_ = terms;
    }

    Sum::~Sum(){
        if (terms_.size()){
            for (const Expression* term : terms_){
                delete term;
            }
        }    
    }

    const Expression* Sum::copy() const{
        std::vector<const Expression*> clonedTerms;
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

    const Expression* Sum::simplify() const{
        std::vector<const Expression*> simplified_terms;
        std::unordered_map<const Expression*, std::vector<const Expression*>> coefficients;
        int constant_buffer = 0;
        for (const Expression* term : terms_){
            auto new_term = term->simplify();
            if (new_term == Constant::ZERO){
                continue;
            }

            if (typeid(new_term) == typeid(Sum)){
                for (const Expression* in_term : term->terms_){
                    simplified_terms.push_back(in_term->copy());
                }
            }
            else{
                simplified_terms.push_back(new_term->copy());
            }
        }

        for (const Expression* term : simplified_terms){
            if (typeid(term) == typeid(Constant)){
                constant_buffer += term->get_exact_value();
            }
            else if (typeid(term) == typeid(Fraction)){

            }
            else if (typeid(term) == typeid(Product)){
                const Product* term = dynamic_cast<const Product*> (term);
                auto product = term->factors_;
                for (const Expression* product_term : product){
                    std::vector<const Expression*> newFactors(product);
                    newFactors.erase(std::remove(newFactors.begin(), newFactors.end(), product_term), newFactors.end());
                    const Expression* newProduct = new Product(std::move(newFactors));
                    coefficients[product_term].push_back(newProduct);
                }
            }
        }
        return new Sum(std::move(simplified_terms));
    }

    const Expression* Sum::complex_derivative(const std::string& variable) const{
        std::vector<const Expression*> derivedTerms;

        for (const Expression* term : terms_){
            derivedTerms.push_back(term->complex_derivative(variable));
        }

        return new Sum(std::move(derivedTerms));
    }

    std::string Sum::to_string() const{
        std::string s;
        s = "(" + terms_[0]->to_string();
        for (long long i = 1; i < terms_.size(); ++i) {
            s += " + " + terms_[i]->to_string();
        }
        s = s + ")";
        return s;
    }


    // Product
    Product::Product(std::vector<const Expression*>&& factors){
        factors_= factors;
    }

    Product::~Product(){
        for (const Expression* factor : factors_){
            delete factor;
        }
    }

    const Expression* Product::copy() const{
        std::vector<const Expression*> clonedFactors;
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

    const Expression* Product::simplify() const{
        std::vector<const Expression*> simplified_terms;
        std::vector<const Expression*> fractions;
        std::unordered_map<const Expression*, std::vector<const Expression*>> powers;
        int constant_buffer = 1;

        for (const Expression* term : factors_){
            auto new_term = term->simplify();
            if (new_term == Constant::ZERO){
                return Constant::ZERO;
            }

            if (typeid(new_term) == typeid(Product)){
                const Product* term = dynamic_cast<const Product*> (term);
                for (const Expression* in_term : term->factors_){       
                    simplified_terms.push_back(in_term->copy());
                }
            }
            else{
                simplified_terms.push_back(new_term->copy());
            }
        }
        for (const Expression* term : simplified_terms){
            if (typeid(term) == typeid(Constant)){
                const Constant* term = dynamic_cast<const Constant*> (term);
                constant_buffer *= term->get_exact_value();
            }
            else if (typeid(term) == typeid(Fraction)){
                fractions.push_back(term);
            }
        }
        if (fractions.size()){
            std::vector <const Expression*> dividend;
            std::vector <const Expression*> divisor;

            for (const Expression* fraction : fractions){
                const Fraction* fraction_ = dynamic_cast<const Fraction*> (fraction);
                dividend.push_back(fraction_->get_dividend());
                divisor.push_back(fraction_->get_divisor());
            }
            for (const Expression* term : simplified_terms){
                dividend.push_back(term);
            }
        }
    }

    const Expression* Product::complex_derivative(const std::string& variable) const{
        std::vector<const Expression*> derivedFactors;
        
        for (long long i = 0; i < factors_.size(); ++i){

            std::vector<const Expression*> otherFactors;
            for (long long j = 0; j < factors_.size(); ++j){
                if (i != j){
                    otherFactors.push_back(factors_[j]->copy());
                }
            }

            otherFactors.push_back(factors_[i]->complex_derivative(variable));
            const Expression* productTerm = new Product(std::move(otherFactors));
            derivedFactors.push_back(productTerm);
        }

        return new Sum(std::move(derivedFactors));
    }

    std::string Product::to_string() const{
        std::string s = "";
        if (factors_.size() == 0){
            return s;
        }
        s = factors_[0]->to_string();
        for (size_t i = 1; i < factors_.size(); ++i) {
            s += " * " + factors_[i]->to_string();
        }
        return s;
    }


    // Fraction
    Fraction::Fraction(const Expression* dividend, const Expression* divisor){
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

    const Expression* Fraction::copy() const{
        return new Fraction(dividend_->copy(), divisor_->copy());
    }

    const Expression* Fraction::complex_derivative(const std::string& variable) const{
        const Expression* numerator = new Sum({
            new Product({dividend_->complex_derivative(variable), divisor_->copy()}),
            new Product({dividend_->copy(), new Product({new Constant(-1), divisor_->complex_derivative(variable)})}),
        });

        const Expression* denominator = new Product({divisor_->copy(), divisor_->copy()});
        return new Fraction(numerator, denominator);
    }

    std::string Fraction::to_string() const{
            std::string s;
            s = "(" + dividend_->to_string() + ")" + " / " + "(" + divisor_->to_string() + ")";
            return s;
        }
};


// Constant
const double Constant::e = 2.718281828459045;
const double Constant::pi = 3.141592653589793;
const Expression* Constant::ZERO = new Constant(0);
const Expression* Constant::ONE = new Constant(1);

Constant::Constant(long long value){
    value_ = value;
}

double Constant::get_value() const{
    return value_;
}

int Constant::get_exact_value() const{
    return value_;
}

const Expression* Constant::copy() const{
        return new Constant(value_);
}

const Expression* Constant::complex_derivative(const std::string& variable) const{
    return Constant::ZERO;
}

std::string Constant::to_string() const{
    return std::to_string(value_);
    }


// Variable
std::unordered_map<std::string, const Expression*> Variable::variables;

Variable::Variable(const std::string& name){
    name_ = name;
}

double Variable::get_value() const{
    auto it = Variable::variables.find(name_);
    if (it != variables.end()){
        return it->second->get_value();
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

const Expression* Variable::copy() const{
    return new Variable(name_);
}

std::string Variable::to_string() const{
    return name_;
}


namespace ElementaryFunctions{
    const Expression* ElementaryFunction::complex_derivative(const std::string& variable) const{
        return new operators::Product({this->derivative(variable), (this->get_input())->complex_derivative(variable)});
    }

    ElementaryFunction::ElementaryFunction() = default;
    ElementaryFunction::~ElementaryFunction() = default;


    // Power
    Power::Power(const Expression* base, const Expression* power){
        base_ = base;
        power_ = power;
    }

    double Power::get_value() const{
        return std::pow(base_->get_value(), power_->get_value());
    }

    const Expression* Power::copy() const{
        return new Power(base_->copy(), power_);
    }

    const Expression* Power::derivative(const std::string& variable) const{
        if (power_->get_value() == 1){
            return Constant::ONE;
        } 
        else
       {
            return new operators::Product({
                new Constant(power_->get_value()),
                new Power(base_->copy(), new Constant(power_->get_value() - 1))
            });
        }
    }

    const Expression* Power::get_input() const{
        return base_;
    }

    std::string Power::to_string() const{
        std::string s;
        if (power_->get_value() == 1){
            s = base_->to_string();
        }
        else{
            s = "(" + base_->to_string() + "^" + power_->to_string() + ")";
        }
        return s;
    }


    // Exponent
    Exp::Exp(const Expression* base, const Expression* power){
        base_ = base;
        power_ = power;
    }

    double Exp::get_value() const{
        return std::exp(power_->get_value() * std::log(base_->get_value()));
    }

    const Expression* Exp::copy() const{
        return new Exp(power_->copy(), base_);
    }

    const Expression* Exp::derivative(const std::string& variable) const{
        return new operators::Product({
                new Exp(power_->copy(), base_),
                new Log(base_, power_->copy())
            });
    }

    const Expression* Exp::get_input() const{
        return power_;
    }

    std::string Exp::to_string() const{
        std::string s;
        s = base_->to_string() + "^(" + power_->to_string() + ")";
        return s;
    }


    // LOgarithm
    Log::Log(const Expression* base, const Expression* arg){
        base_ = base;
        arg_ = arg;
    }

    double Log::get_value() const{
        return std::log(arg_->get_value()) / std::log(base_->get_value());
    }

    const Expression* Log::copy() const{
        return new Log(arg_->copy(), base_);
    }

    const Expression* Log::derivative(const std::string& variable) const{
        return new operators::Fraction(
            Constant::ONE,
            new operators::Product({
                arg_->copy(),
                new Log(double_to_fraction(Constant::e), double_to_fraction(base_->get_value()))
            }));
    }

    const Expression* Log::get_input() const{
        return arg_;
    }

    std::string Log::to_string() const{
        std::string s;
        s = "log[" + base_->to_string() + ", " + arg_->to_string() + "]";
        return s;
    }
};


// overloading
const Expression* operator+(const Expression& lhs, const Expression& rhs){
    return new operators::Sum({lhs.copy(), rhs.copy()});
}

const Expression* operator-(const Expression& lhs, const Expression& rhs){
    return new operators::Sum({lhs.copy(), new operators::Product({new Constant(-1), rhs.copy()})});
}

const Expression* operator*(const Expression& lhs, const Expression& rhs){
    return new operators::Product({lhs.copy(), rhs.copy()});
}

const Expression* operator/(const Expression& lhs, const Expression& rhs){
    return new operators::Fraction(lhs.copy(), rhs.copy());
}


const Expression* double_to_fraction(double value){
    long long precision = 1000000000000;
    long long numerator = static_cast<long long>(value * precision);
    long long denominator = precision;

    long long gcd = std::gcd(numerator, denominator);

    numerator /= gcd;
    denominator /= gcd;

    return new operators::Fraction(new Constant(numerator), new Constant(denominator));
}

const Expression* WholeFactorial(long long n){
    std::vector <const Expression*> factors;
    factors.push_back(new Constant(1));
    for (long long i = 2; i <= n; ++i){
        factors.push_back(new Constant(i));
    }
    return new operators::Product(std::move(factors));
}

const Expression* Taylor_series(const Expression* f, const std::string& variable_name, double point){
    const Expression* buff = Variable::variables[variable_name];
    Variable::variables[variable_name] = double_to_fraction(point);

    std::vector <const Expression*> terms;
    std::vector <const Expression*> fNDErivative;
    fNDErivative.push_back(f);
    long long STEPS = 6;
    for (long long i = 0; i < STEPS; ++i){
        
        Expression* k = new operators::Fraction(double_to_fraction(fNDErivative[i]->get_value()), WholeFactorial(i));
        const Expression* term = new operators::Product({
            k,
            new ElementaryFunctions::Power(new operators::Sum({new Variable(variable_name), new operators::Product({new Constant(-1), double_to_fraction(point)})}), new Constant(i))
        });
        terms.push_back(term);
        //std::cout << std::endl;
        //std::cout << i << ":  " << "der:  " << fNDErivative[i]->to_string() << "\n\n    term: " << term->to_string() << std::endl;
        //std::cout << std::endl;
        fNDErivative.push_back(fNDErivative[i]->complex_derivative(variable_name));
    }

    Variable::variables[variable_name] = buff;
    return new operators::Sum(std::move(terms));
}