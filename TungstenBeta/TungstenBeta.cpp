
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
        clonedTerms.push_back(term);
    }
    return (new Sum(std::move(clonedTerms)))->simplify();
}

double Sum::calculate() const{
    double result = 0;

    for (const Expression* term : terms_){
        result += term->calculate();
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

        if ((typeid(*simplifiedTerm) == typeid(Constant)) && (simplifiedTerm != Constant::e)){
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
    if (terms_.size() == 1){
        return terms_[0]->to_string();
    }
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
        clonedFactors.push_back(factor);
    }
    return (new Product(std::move(clonedFactors)))->simplify();
}

double Product::calculate() const{
    double result = 1;
    for (const Expression* factor : factors_){
        result *= factor->calculate();
    }
    return result;
}

const Expression* Product::simplify() const {
    std::vector<const Expression*> simplifiedFactors;
    std::vector<const Expression*> openedFactors;
    std::vector<const Expression*> fractions;
    std::vector<const Expression*> divisorTerms;
    std::vector<const Expression*> constantFactor;
    int constantBuff = 1;

    std::unordered_map<std::string, std::pair<const Expression*, const Expression*>> powers; 

    for (const Expression* factor : factors_){
        const Expression* simplifiedFactor = factor->simplify();
            
        if (simplifiedFactor == Constant::ZERO){
            return Constant::ZERO;
        }
        if (simplifiedFactor == Constant::ONE){
            continue;
        }
        if ((typeid(*simplifiedFactor) == typeid(Constant)) && (simplifiedFactor != Constant::e)){
            //std::cout << simplifiedFactor->to_string() << "  :  ";
            //std::cout << static_cast<const Constant*>(simplifiedFactor)->get_exact_value() << "\n";

            constantBuff *= static_cast<const Constant*>(simplifiedFactor)->get_exact_value();
        }
        else if (typeid(*simplifiedFactor) == typeid(Product)){
        //std::cout << simplifiedFactor->to_string() << " | ";

            for (const Expression* in_factor : static_cast<const Product*>(simplifiedFactor)->get_factors()){
                openedFactors.push_back(in_factor);
            }
        }
        else if (typeid(*simplifiedFactor) != typeid(Constant)){
            openedFactors.push_back(simplifiedFactor);
        }
    }
    //std::cout << "\n";
    for (const Expression* factor : openedFactors){
        const Expression* simplifiedFactor = factor->simplify();
        //std::cout << simplifiedFactor->to_string() << "\n";
        if (typeid(*simplifiedFactor) == typeid(Constant)){
            constantBuff *= static_cast<const Constant*>(simplifiedFactor)->get_exact_value();
        }
        else if (typeid(*simplifiedFactor) == typeid(Fraction)){
            fractions.push_back(simplifiedFactor);
        }
        else{
            simplifiedFactors.push_back(simplifiedFactor);
        }
    }

    std::vector<const Expression*> finalFactors;

    for (const Expression* factor : simplifiedFactors){
        finalFactors.push_back(factor);
    }

    if (!constantFactor.empty()){
        
        finalFactors.push_back(new Product(std::move(constantFactor)));
    }

    if (!fractions.empty()){
        for (const Expression* fraction : fractions){
            const Fraction* fraction_ = static_cast<const Fraction*>(fraction);
            finalFactors.push_back(fraction_->get_dividend());
            divisorTerms.push_back(fraction_->get_divisor());
        }
    }

    for (const Expression* factor : finalFactors){
        if (typeid(*factor) == typeid(ElementaryFunctions::Power)) {
            const ElementaryFunctions::Power* powExpr = static_cast<const ElementaryFunctions::Power*>(factor);
            const Expression* base = powExpr->get_base();
            const Expression* power = powExpr->get_power();
            std::string key = base->to_string();

            if (powers.find(key) == powers.end()) { 
                powers[key] = std::make_pair(base, Constant::ZERO);
            }

            powers[key].second = (new operators::Sum({powers[key].second, power}))->simplify();
        }
        else{
            std::string key = factor->to_string();
            if (powers.find(key) == powers.end()){
                powers[key] = std::make_pair(factor, Constant::ZERO);
            }
            powers[key].second = (new operators::Sum({powers[key].second, Constant::ONE}))->simplify();
        }
    }

    finalFactors.clear();

    if (constantBuff != 1){
        finalFactors.push_back(new Constant(constantBuff));
    }

    for (auto it = powers.begin(); it != powers.end(); ++it) {
        const Expression* base = it->second.first;
        const Expression* power = it->second.second->simplify();
        //std::cout << it->first << " : " << base->to_string() << " : " << power->to_string() << "\n";
        finalFactors.push_back(new ElementaryFunctions::Power(base, power));
    }

    if (!fractions.empty()){
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
    //this->simplify();
    for (long long i = 0; i < factors_.size(); ++i){

        std::vector<const Expression*> otherFactors;
        for (long long j = 0; j < factors_.size(); ++j){
            if (i != j){
                otherFactors.push_back(factors_[j]);
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
    if (factors_.size() == 1){
        return factors_[0]->to_string();
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

double Fraction::calculate() const{
    return dividend_->calculate() / divisor_->calculate();
}

const Expression* Fraction::copy() const{
    return (new Fraction(dividend_, divisor_))->simplify();
}

const Expression* Fraction::simplify() const{
    const Expression* simplifiedDividend = dividend_->simplify();
    const Expression* simplifiedDivisor = divisor_->simplify();

    if (simplifiedDivisor == Constant::ONE){
        return simplifiedDividend;
    }
    if (simplifiedDivisor == Constant::ZERO){
        return Constant::ZERO;
    }

    if ((typeid(*simplifiedDividend) == typeid(Constant)) && (typeid(*simplifiedDivisor) == typeid(Constant)) && (simplifiedDividend != Constant::e) && (simplifiedDivisor != Constant::e)){

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
        new Product({dividend_->complex_derivative(variable), divisor_}),
        new Product({dividend_, new Product({new Constant(-1), divisor_->complex_derivative(variable)})}),
    });

    const Expression* denominator = new Product({divisor_, divisor_});
    return (new Fraction(numerator, denominator))->simplify();
}

std::string Fraction::to_string() const{
    if (this == Constant::e){
        return "e";
    }
    std::string s;
    s = "(" + dividend_->to_string() + ")" + " / " + "(" + divisor_->to_string() + ")";
    return s;
}
};


// Constant
const Expression* Constant::e = new Constant;
const Expression* Constant::pi = double_to_fraction(3.141592653589793);
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

double Power::calculate() const{
    return std::pow(base_->calculate(), power_->calculate());
}

const Expression* Power::copy() const{
    return (new Power(base_, power_))->simplify();
}

const Expression* Power::simplify() const{
    return new Power(base_->simplify(), power_->simplify());
}

const Expression* Power::derivative(const std::string& variable) const{
    if (power_->calculate() == 1){
        return Constant::ONE;
    } 
    else
    {
        return (new operators::Product({
            new Constant(power_->calculate()),
            new Power(base_, new Constant(power_->calculate() - 1))
        }))->simplify();
    }
}

const Expression* Power::get_input() const{
    return base_;
}

std::string Power::to_string() const{
    std::string s;
    if (power_->calculate() == 1){
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

double Exp::calculate() const{
    return std::exp(power_->calculate() * std::log(base_->calculate()));
}

const Expression* Exp::copy() const{
    return (new Exp(base_, power_))->simplify();
}

const Expression* Exp::simplify() const{
    return new Exp(base_->simplify(), power_->simplify());
}

const Expression* Exp::derivative(const std::string& variable) const{
    return (new operators::Product({
            new Exp(base_, power_),
            new Log(Constant::e, base_)
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

double Log::calculate() const{
    return std::log(arg_->calculate()) / std::log(base_->calculate());
}

const Expression* Log::copy() const{
    return (new Log(arg_, base_))->simplify();
}

const Expression* Log::simplify() const{
    return new Log(base_->simplify(), arg_->simplify());
}

const Expression* Log::derivative(const std::string& variable) const{
    return (new operators::Fraction(
        Constant::ONE,
        new operators::Product({
            arg_,
            new Log(Constant::e, base_)
        })))->simplify();
}

const Expression* Log::get_input() const{
    return arg_;
}

std::string Log::to_string() const{
    std::string s;
    if (base_ == Constant::e){
        s = "ln(" + arg_->to_string() + ")";
    }
    else{
        s = "log[" + base_->to_string() + ", " + arg_->to_string() + "]";
    }
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
    terms.push_back(f);

    long long STEPS = 5;
    for (long long i = 1; i < STEPS; ++i){
        fNDerivative.push_back((fNDerivative[i - 1]->complex_derivative(variable_name))->simplify());

        const Expression* k = new operators::Fraction(double_to_fraction(fNDerivative[i]->calculate()), new operators::Product({new Constant(i), double_to_fraction(fNDerivative[i - 1]->calculate())}));
        
        const Expression* term = new operators::Product({
            k,
            new operators::Sum({new Variable(variable_name), new operators::Product({new Constant(-1), double_to_fraction(point)})}),
            terms[i - 1]
        });
        terms.push_back(term->simplify());
        std::cout << std::endl;
        std::cout << i << ":  " << "der:  " << fNDerivative[i]->simplify()->to_string() << "\n\n    term: " << term->simplify()->to_string() << std::endl;
        std::cout << std::endl;
        
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