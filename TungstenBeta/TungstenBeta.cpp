
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
        return (new Sum(std::move(clonedTerms)))->simplify();
    }

    double Sum::get_value() const{
        double result = 0;

        for (const Expression* term : terms_){
            result += term->get_value();
        }

        return result;
    }

    const Expression* Sum::simplify() const{
        std::vector<const Expression*> openedTerms;
        std::vector<const Expression*> simplifiedTerms;
        std::unordered_map<std::string, const Expression*> coefficients;
        int constantTerm = 0;

        for (const Expression* term : terms_){
            const Expression* simplifiedTerm = term->simplify();

            if (simplifiedTerm == Constant::ZERO){
                continue;
            }
            if (typeid(*simplifiedTerm) == typeid(Constant)){
                constantTerm += static_cast<const Constant*>(simplifiedTerm)->get_exact_value();
            }
            else if (typeid(*simplifiedTerm) == typeid(Sum)){
                for (const Expression* in_term : static_cast<const Sum*>(simplifiedTerm)->get_terms()){
                    openedTerms.push_back(in_term);
                }
            }
            else if (typeid(*simplifiedTerm) != typeid(Constant)){
                openedTerms.push_back(simplifiedTerm);
            }
        }

        for (const Expression* term : openedTerms){
            const Expression* simplifiedTerm = term->simplify();

            if (typeid(*simplifiedTerm) == typeid(Constant)){
                constantTerm += static_cast<const Constant*>(simplifiedTerm)->get_exact_value();
            }
            else{
                simplifiedTerms.push_back(simplifiedTerm);
            }
        }

        std::vector<const Expression*> finalTerms;
        if (constantTerm != 0){
            finalTerms.push_back(new Constant(constantTerm));
        }
        for (const Expression* term : simplifiedTerms){
            finalTerms.push_back(term);
        }
        if (finalTerms.empty()){
            return Constant::ZERO;
        }
        else if (finalTerms.size() == 1){
            return finalTerms[0];
        }
        else{
            return new Sum(std::move(finalTerms));
        }
    }

    const Expression* Sum::complex_derivative(const std::string& variable) const{
        std::vector<const Expression*> derivedTerms;

        for (const Expression* term : terms_){
            derivedTerms.push_back(term->complex_derivative(variable));
        }

        return (new Sum(std::move(derivedTerms)))->simplify();
    }

    std::string Sum::to_string() const{
        std::string s;
        s = "(" + terms_[0]->to_string();
        for (long long i = 1; i < terms_.size(); ++i){
            s += " + " + terms_[i]->to_string();
        }
        s = s + ")";
        return s;
    }


    // Product
    //Product::Product(std::vector<const Expression*>&& factors){
    //    factors_= factors;
    //}

    Product::Product(std::vector<const Expression*> factors){
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
        return (new Product(std::move(clonedFactors)))->simplify();
    }

    double Product::get_value() const{
        double result = 1;
        for (const Expression* factor : factors_){
            result *= factor->get_value();
        }
        return result;
    }

    const Expression* Product::simplify() const{
        std::vector<const Expression*> simplifiedFactors;
        std::vector<const Expression*> openedFactors;
        std::vector<const Expression*> fractions;
        std::vector<const Expression*> constantFactor;
        int constantBuff = 1;

        std::unordered_map<std::string, const Expression*> powers; 

        for (const Expression* factor : factors_){
            const Expression* simplifiedFactor = factor->simplify();
            if (simplifiedFactor == Constant::ZERO){
                return Constant::ZERO;
            }
            if (simplifiedFactor == Constant::ONE){
                continue;
            }
            if (!hasVariables(simplifiedFactor)){
                if (typeid(*simplifiedFactor) == typeid(Constant)){
                    constantBuff += static_cast<const Constant*>(simplifiedFactor)->get_exact_value();
                }
                else{
                    constantFactor.push_back(simplifiedFactor);
                }
            } 
            else if (typeid(*simplifiedFactor) == typeid(Product)){
                for (const Expression* in_factor : static_cast<const Product*>(simplifiedFactor)->get_factors()){
                    openedFactors.push_back(in_factor);
                }
            }
            else if (typeid(*simplifiedFactor) != typeid(Constant)){
                openedFactors.push_back(simplifiedFactor);
            }
        }

        for (const Expression* factor : openedFactors){
            const Expression* simplifiedFactor = factor->simplify();
            if (!hasVariables(simplifiedFactor)){
                if (typeid(*simplifiedFactor) == typeid(Constant)){
                    constantBuff += static_cast<const Constant*>(simplifiedFactor)->get_exact_value();
                }
                else{
                    constantFactor.push_back(simplifiedFactor);
                }
            } 
            else if (typeid(*simplifiedFactor) == typeid(Fraction)){
                fractions.push_back(simplifiedFactor);
            }
            else{
                simplifiedFactors.push_back(simplifiedFactor);
            }
        }

        std::vector<const Expression*> finalFactors;
        if (constantBuff != 1){
            finalFactors.push_back(new Constant(constantBuff));
        }
        if (!constantFactor.empty()){
            finalFactors.push_back(new Product(std::move(constantFactor)));
        }

        for (const Expression* factor : simplifiedFactors){
            finalFactors.push_back(factor->copy());
        }

        if (!fractions.empty()){
            std::vector<const Expression*> divisorTerms;
            for (const Expression* fraction : fractions){
                const Fraction* fraction_ = static_cast<const Fraction*>(fraction);
                finalFactors.push_back(fraction_->get_dividend()->copy());
                divisorTerms.push_back(fraction_->get_divisor()->copy());
            }

            const Expression* productDividend = new Product(std::move(finalFactors));
            const Expression* productDivisor = new Product(std::move(divisorTerms));
            return new Fraction(productDividend, productDivisor);
        }
        else{
            if (finalFactors.empty()){
                return Constant::ONE;
            }
            else if (finalFactors.size() == 1){
                return finalFactors[0];
            }
            else{
                return new Product(std::move(finalFactors)); 
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

        return (new Sum(std::move(derivedFactors)))->simplify();
    }

    std::string Product::to_string() const{
        std::string s = "";
        if (factors_.size() == 0){
            return s;
        }
        s = factors_[0]->to_string();
        for (size_t i = 1; i < factors_.size(); ++i){
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
        return (new Fraction(dividend_->copy(), divisor_->copy()))->simplify();
    }

    const Expression* Fraction::simplify() const{
        const Expression* simplifiedDividend = dividend_->simplify();
        const Expression* simplifiedDivisor = divisor_->simplify();

        // If the divisor is 1, simplify to just the dividend
        if (simplifiedDivisor == Constant::ONE){
            return simplifiedDividend;
        } 

        // Handle cases where both dividend and divisor are constants
        if (typeid(*simplifiedDividend) == typeid(Constant) && typeid(*simplifiedDivisor) == typeid(Constant)){

            int num = static_cast<const Constant*>(simplifiedDividend)->get_exact_value();
            int den = static_cast<const Constant*>(simplifiedDivisor)->get_exact_value();
            if ((num < 0) && (den < 0)){
                num *= -1;
                den *= -1;
            }
            else if (den < 0){
                num *= -1;
                den *= -1;
            }

            if (num % den == 0){
                return new Constant(num / den);
            } else{
                int gcd = std::gcd(abs(num), abs(den));
                //std::cout << "simplifiedDividend: " << simplifiedDividend->to_string() << std::endl; 
                //std::cout << "simplifiedDivisor: " << simplifiedDivisor->to_string() << std::endl;
                return new Fraction(new Constant(num / gcd), new Constant(den / gcd));
            }
        }

        return new Fraction(simplifiedDividend, simplifiedDivisor);
    }

    const Expression* Fraction::complex_derivative(const std::string& variable) const{
        const Expression* numerator = new Sum({
            new Product({dividend_->complex_derivative(variable), divisor_->copy()}),
            new Product({dividend_->copy(), new Product({new Constant(-1), divisor_->complex_derivative(variable)})}),
        });

        const Expression* denominator = new Product({divisor_->copy(), divisor_->copy()});
        return (new Fraction(numerator, denominator))->simplify();
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

const Expression* Constant::simplify() const{
    return this;
}


const Expression* Constant::copy() const{
    return new Constant(value_);
}

const Expression* Constant::complex_derivative(const std::string& variable) const{
    return Constant::ZERO;
}

std::string Constant::to_string() const{
    if (value_ > 0){
        return std::to_string(value_);
    }
    else{
        return "(" + std::to_string(value_) + ")";
    }
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

const Expression* Variable::simplify() const{
    return this;
}

std::string Variable::to_string() const{
    return name_;
}


namespace ElementaryFunctions{
    const Expression* ElementaryFunction::complex_derivative(const std::string& variable) const{
        return (new operators::Product({this->derivative(variable), (this->get_input())->complex_derivative(variable)}))->simplify();
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
        return (new Power(base_->copy(), power_->copy()))->simplify();
    }

    const Expression* Power::simplify() const{
        return new Power(base_->simplify(), power_->simplify());
    }

    const Expression* Power::derivative(const std::string& variable) const{
        if (power_->get_value() == 1){
            return Constant::ONE;
        } 
        else
      {
            return (new operators::Product({
                new Constant(power_->get_value()),
                new Power(base_->copy(), new Constant(power_->get_value() - 1))
            }))->simplify();
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
        return (new Exp(power_->copy(), base_->copy()))->simplify();
    }

    const Expression* Exp::simplify() const{
        return new Exp(power_->simplify(), base_->simplify());
    }

    const Expression* Exp::derivative(const std::string& variable) const{
        return (new operators::Product({
                new Exp(base_->copy(), power_->copy()),
                new Log(base_->copy(), power_->copy())
            }))->simplify();
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
        return (new Log(arg_->copy(), base_->copy()))->simplify();
    }

    const Expression* Log::simplify() const{
        return new Log(arg_->simplify(), base_->simplify());
    }

    const Expression* Log::derivative(const std::string& variable) const{
        return (new operators::Fraction(
            Constant::ONE,
            new operators::Product({
                arg_->copy(),
                new Log(double_to_fraction(Constant::e), double_to_fraction(base_->get_value()))
            })))->simplify();
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
    return (new operators::Sum({lhs.copy(), rhs.copy()}))->simplify();
}

const Expression* operator-(const Expression& lhs, const Expression& rhs){
    return (new operators::Sum({lhs.copy(), new operators::Product({new Constant(-1), rhs.copy()})}))->simplify();
}

const Expression* operator*(const Expression& lhs, const Expression& rhs){
    return (new operators::Product({lhs.copy(), rhs.copy()}))->simplify();
}

const Expression* operator/(const Expression& lhs, const Expression& rhs){
    return (new operators::Fraction(lhs.copy(), rhs.copy()))->simplify();
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
    std::vector <const Expression*> fNDerivative;
    fNDerivative.push_back(f);
    long long STEPS = 4;
    for (long long i = 0; i < STEPS; ++i){
        
        Expression* k = new operators::Fraction(double_to_fraction(fNDerivative[i]->get_value()), WholeFactorial(i));
        const Expression* term = new operators::Product({
            k,
            new ElementaryFunctions::Power(new operators::Sum({new Variable(variable_name), new operators::Product({new Constant(-1), double_to_fraction(point)})}), new Constant(i))
        });
        terms.push_back(term->simplify());
        std::cout << std::endl;
        std::cout << i << ":  " << "der:  " << fNDerivative[i]->simplify()->to_string() << "\n\n    term: " << term->simplify()->to_string() << std::endl;
        std::cout << std::endl;
        fNDerivative.push_back((fNDerivative[i]->complex_derivative(variable_name))->simplify());
    }

    Variable::variables[variable_name] = buff;
    return (new operators::Sum(std::move(terms)))->simplify();
}


bool hasVariables(const Expression* expr){
    if (typeid(*expr) == typeid(Variable)) {
        return true;
    }
    else if (typeid(*expr) == typeid(operators::Sum)) {
        const operators::Sum* sumExpr = static_cast<const operators::Sum*>(expr);
        for (const Expression* term : sumExpr->get_terms()) {
            if (hasVariables(term)) {
                return true;
            }
        }
    }
    else if (typeid(*expr) == typeid(operators::Product)) {
        const operators::Product* prodExpr = static_cast<const operators::Product*>(expr);
        for (const Expression* factor : prodExpr->get_factors()) {
            if (hasVariables(factor)) {
                return true;
            }
        }
    } 
    else if (typeid(*expr) == typeid(operators::Fraction)) {
        const operators::Fraction* fracExpr = static_cast<const operators::Fraction*>(expr);
        return hasVariables(fracExpr->get_dividend()) || hasVariables(fracExpr->get_divisor());
    } 
    else if (typeid(*expr) == typeid(ElementaryFunctions::Power)) {
        const ElementaryFunctions::Power* powExpr = static_cast<const ElementaryFunctions::Power*>(expr);
        return hasVariables(powExpr->get_input()) || hasVariables(powExpr->get_power());
    } 
    else if (typeid(*expr) == typeid(ElementaryFunctions::Log)) {
        const ElementaryFunctions::Log* logExpr = static_cast<const ElementaryFunctions::Log*>(expr);
        return hasVariables(logExpr->get_input()) || hasVariables(logExpr->get_base());
    }
    else if (typeid(*expr) == typeid(ElementaryFunctions::Exp)) {
        const ElementaryFunctions::Exp* expExpr = static_cast<const ElementaryFunctions::Exp*>(expr);
        return hasVariables(expExpr->get_input()) || hasVariables(expExpr->get_base());
    }
    // сюда еще тригу
    return false;
}