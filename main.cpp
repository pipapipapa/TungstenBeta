#include "TungstenBeta/TungstenBeta.h"


int main(int argc, char *argv[]){
    auto x = new Variable("x");
    auto y = new Variable("y");

    Variable::variables["x"] = double_to_fraction(1.5);
    Variable::variables["y"] = double_to_fraction(3.25);

    auto e = Constant::e;
    std::cout << e->to_string() << "\n";

    auto sum = *x + *y;
    std::cout << "Sum: " << sum->to_string() << " = " << sum->calculate() << std::endl;
    std::cout << std::endl;
    std::cout << "Sum derivative of x: " << sum->complex_derivative("x")->to_string() << std::endl;
    std::cout << "Sum derivative of y: " << sum->complex_derivative("y")->to_string() << std::endl;
    std::cout << std::endl;

    auto product = *x * *y;
    std::cout << "Product: " << product->to_string() << " = " << product->calculate() << std::endl;
    std::cout << std::endl;
    std::cout << "Product derivative of x: " << product->complex_derivative("x")->to_string() << std::endl;
    std::cout << "Product derivative of y: " << product->complex_derivative("y")->to_string() << std::endl;
    std::cout << std::endl;
    auto expr = new operators::Product({
        new Constant(5),
        new ElementaryFunctions::Power(new Variable("x"), new Constant(2))
    });

    auto exprp = new ElementaryFunctions::Power(new ElementaryFunctions::Power(new Variable("x"), new Constant(2)), new Constant(4));

    std::cout << "Expression: " << exprp->simplify()->to_string() << std::endl;


    auto expr1 = new operators::Fraction(
        new operators::Sum({
            new ElementaryFunctions::Log(double_to_fraction(2.71), expr),
            new ElementaryFunctions::Exp(new Constant(2), x)}),
        expr);

    std::cout << "Expression: " << expr->to_string() << std::endl;

    auto derivative = expr->complex_derivative("x");
    std::cout << "Derivative: " << derivative->to_string() << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Expression: " << expr1->to_string() << std::endl;
    std::cout << "Expression value: " << expr1->calculate() << std::endl;

    auto derivative1 = expr1->complex_derivative("x");
    std::cout << std::endl;
    std::cout << "Derivative: " << derivative1->to_string() << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    auto z = new Variable("z");
    Variable::variables["z"] = expr1;
    std::cout << "z value: " << z->calculate() << std::endl;
    std::cout << std::endl;
    

    auto ex = new operators::Product({
        new Constant(5),
        new ElementaryFunctions::Exp(Constant::e, new Variable("y")) // Constant(2) -> e
    });
    auto T = Taylor_series(ex, "y", y->calculate()); // 47.5683

    std::cout << T->to_string();
    std::cout << std::endl;
    std::cout << T->calculate();
    std::cout << std::endl;


    delete x;
    delete y;
    delete sum;
    delete product;
    delete expr;
    delete expr1;
    delete derivative;
    delete derivative1;


    return 0;
}
