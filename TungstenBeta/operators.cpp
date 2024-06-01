#include "Methods.h"
#include "operators.h"
#include "Constant.h"
#include "Variable.h"
#include "ElementaryFunctions.h"

const Expression* double_to_fraction(double value) {
    long long precision = 1000000000000;
    long long numerator = static_cast<long long>(value * precision);
    long long denominator = precision;

    long long gcd = std::gcd(numerator, denominator);

    numerator /= gcd;
    denominator /= gcd;

    return new operators::Fraction(new Constant(numerator), new Constant(denominator));
}

const Expression* WholeFactorial(long long n) {
    std::vector <const Expression*> factors;
    factors.push_back(new Constant(1));
    for (long long i = 2; i <= n; ++i) {
        factors.push_back(new Constant(i));
    }
    return new operators::Product(std::move(factors));
}

const Expression* Taylor_series(const Expression* f, const std::string& variable_name, double point) {
    const Expression* buff = Variable::variables[variable_name];
    Variable::variables[variable_name] = double_to_fraction(point);

    std::vector <const Expression*> terms;
    std::vector <const Expression*> fNDerivative;
    fNDerivative.push_back(f);
    terms.push_back(f);

    long long STEPS = 5;
    for (long long i = 1; i < STEPS; ++i) {
        fNDerivative.push_back((fNDerivative[i - 1]->complex_derivative(variable_name))->simplify());
        const Expression* k = new operators::Fraction(fNDerivative[i]->plug_variable(variable_name), new operators::Product({new Constant(i), fNDerivative[i - 1]->plug_variable(variable_name)}));

        const Expression* term = new operators::Product({
            k,
            new operators::Sum({new Variable(variable_name), new operators::Product({new Constant(-1), double_to_fraction(point)})}),
            terms[i - 1]->plug_variable(variable_name)
        });
        terms.push_back(term->simplify());
    }

    Variable::variables[variable_name] = buff;
    return (new operators::Sum(std::move(terms)))->simplify();
}

const Expression* NewtonMethod::Newton_root(const Expression* func, const std::string variable, double initial_guess = 1.0, double tolerance, int max_iterations) {
    Variable::variables[variable] = double_to_fraction(initial_guess); // Set initial guess
    for (int i = 0; i < max_iterations; ++i) {
        double f_x = func->calculate();
        const Expression* derivative = func->complex_derivative(variable);
        double f_prime_x = derivative->calculate();
        delete derivative;
        if (std::abs(f_prime_x) < 1e-12) {
            return nullptr;
        }
        double new_guess = initial_guess - (f_x / f_prime_x);
        if (std::abs(new_guess - initial_guess) < tolerance) {
            return double_to_fraction(new_guess);
        }
        initial_guess = new_guess;
        Variable::variables[variable] = new Constant(initial_guess);
    }
    return nullptr;
}

bool hasVariables(const Expression* expr) {
    if (typeid(*expr) == typeid(Variable)) {
        return true;
    } else if (typeid(*expr) == typeid(operators::Sum)) {
        const operators::Sum* sumExpr = static_cast<const operators::Sum*>(expr);
        for (const Expression* term : sumExpr->get_terms()) {
            if (hasVariables(term)) {
                return true;
            }
        }
    } else if (typeid(*expr) == typeid(operators::Product)) {
        const operators::Product* prodExpr = static_cast<const operators::Product*>(expr);
        for (const Expression* factor : prodExpr->get_factors()) {
            if (hasVariables(factor)) {
                return true;
            }
        }
    } else if (typeid(*expr) == typeid(operators::Fraction)) {
        const operators::Fraction* fracExpr = static_cast<const operators::Fraction*>(expr);
        return hasVariables(fracExpr->get_dividend()) || hasVariables(fracExpr->get_divisor());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Power)) {
        const ElementaryFunctions::Power* powExpr = static_cast<const ElementaryFunctions::Power*>(expr);
        return hasVariables(powExpr->get_input()) || hasVariables(powExpr->get_power());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Log)) {
        const ElementaryFunctions::Log* logExpr = static_cast<const ElementaryFunctions::Log*>(expr);
        return hasVariables(logExpr->get_input()) || hasVariables(logExpr->get_base());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Exp)) {
        const ElementaryFunctions::Exp* expExpr = static_cast<const ElementaryFunctions::Exp*>(expr);
        return hasVariables(expExpr->get_input()) || hasVariables(expExpr->get_base());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Sin)) {
        const ElementaryFunctions::Sin* sinExpr = static_cast<const ElementaryFunctions::Sin*>(expr);
        return hasVariables(sinExpr->get_input());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Cos)) {
        const ElementaryFunctions::Cos* cosExpr = static_cast<const ElementaryFunctions::Cos*>(expr);
        return hasVariables(cosExpr->get_input());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Tan)) {
        const ElementaryFunctions::Tan* tanExpr = static_cast<const ElementaryFunctions::Tan*>(expr);
        return hasVariables(tanExpr->get_input());
    } else if (typeid(*expr) == typeid(ElementaryFunctions::Cot)) {
        const ElementaryFunctions::Cot* cotExpr = static_cast<const ElementaryFunctions::Cot*>(expr);
        return hasVariables(cotExpr->get_input());
    }
    return false;
}