#ifndef METHODS_H
#define METHODS_H

#include "Expression.h"

class NewtonMethod{
public:
    static const Expression* Newton_root(const Expression* func, const std::string variable, double initial_guess, double tolerance = 1e-6, int max_iterations = 100);
};

const Expression* double_to_fraction(double value);

const Expression* Taylor_series(const Expression* f, const std::string& variable, double point);

bool hasVariables(const Expression* expr);


#endif // METHODS_H