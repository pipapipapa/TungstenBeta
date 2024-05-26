
#include "TungstenBeta.h"

#include "operators.h"
#include "Constant.h"
#include "ElementaryFunctions.h"

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