#include "operators.h"

#include "Constant.h"
#include "ElementaryFunctions.h"

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

const Expression* Sum::plug_variable(const std::string& variable) const{
    std::vector<const Expression*> updatedTerms;

    for (const Expression* term : terms_){
        updatedTerms.push_back(term->plug_variable(variable));
    }

    return (new Sum(std::move(updatedTerms)))->simplify();
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

const Expression* Product::simplify() const{
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
        if (typeid(*factor) == typeid(ElementaryFunctions::Power)){
            const ElementaryFunctions::Power* powExpr = static_cast<const ElementaryFunctions::Power*>(factor);
            const Expression* base = powExpr->get_base();
            const Expression* power = powExpr->get_power();
            std::string key = base->to_string();

            if (powers.find(key) == powers.end()){ 
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

    for (auto it = powers.begin(); it != powers.end(); ++it){
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

const Expression* Product::plug_variable(const std::string& variable) const{
    std::vector<const Expression*> updatedTerms;

    for (const Expression* factor : factors_){
        updatedTerms.push_back(factor->plug_variable(variable));
    }

    return (new Product(std::move(updatedTerms)))->simplify();
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
    if (simplifiedDividend == Constant::ZERO){
        return Constant::ZERO;
    }

    if ((typeid(*simplifiedDividend) == typeid(Constant)) && (simplifiedDividend != Constant::e) && (typeid(*simplifiedDivisor) == typeid(Constant)) && (simplifiedDivisor != Constant::e)){

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

const Expression* Fraction::plug_variable(const std::string& variable) const{
    return (new Fraction(dividend_->plug_variable(variable), divisor_->plug_variable(variable)))->simplify();
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