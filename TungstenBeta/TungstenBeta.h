#include <iostream>
#include <vector>


class Expression{
public:
    virtual double getValue() const = 0;
    virtual Expression* derivative() const = 0;
    virtual ~Expression() = default;


    friend Expression* operator+(const Expression& lhs, const Expression& rhs);
    friend Expression* operator-(const Expression& lhs, const Expression& rhs);
    friend Expression* operator*(const Expression& lhs, const Expression& rhs);
    friend Expression* operator/(const Expression& lhs, const Expression& rhs);
};


namespace operators{
    class Sum : public Expression{
        private:
            std::vector<Expression*> terms_;
        public:
            Sum(std::vector<Expression*>&& terms) : terms_(std::move(terms)){}
            ~Sum(){
                for (Expression* term : terms_){
                    delete term;
                }
            }

            double getValue() const override{
            double result = 0;
            for (const Expression* term : terms_){
                result += term->getValue();
            }
            return result;
            }

            Expression* derivative() const override{
                std::vector<Expression*> derivedTerms;
                for (const Expression* term : terms_){
                    derivedTerms.push_back(term->derivative());
                }
                return new Sum(std::move(derivedTerms));
            }
        };


    
    class Product : public Expression{
        private:
            std::vector<Expression*> factors_;
        public:
            Product(std::vector<Expression*>&& factors) : factors_(std::move(factors)){}
            ~Product(){
                for (Expression* factor : factors_){
                    delete factor;
                }
            }

            double getValue() const override {
                double result = 1;
                for (const Expression* factor : factors_) {
                    result *= factor->getValue();
                }
                return result;
            }


            Sum derivative(){
                return lhs_.complex_derivative() * rhs_ + rhs_.complex_derivative() * lhs;
            }
    };


    
    class Fraction : public Expression{
        private:
        Expression* dividend_;
        Expression* divisor_;
        public:
            Fraction derivative(){
                return Fraction(Sum(Product(dividend_.complex_derivative(), divisor_) - Product(divisor_.complex_derivative(), dividend_)), ElementaryFunctions::Power3(divisor_, 2));
            }
    };
};


class Constant : public Expression{
    public:
        static const double e = 2.718281828459045;
        static const double pi = 3.141592653589793;
        int value_;

        Constant(int value){
            value_ = value;
        }

        Constant derivative(){
            return Constant(0);
        }
};

namespace ElementaryFunctions{

    class ElementaryFunction : public Expression{
        private:
            Expression* input_;
        public:
            Expression* complex_derivative(){
                return this->derivative() * (this->get_input()).complex_derivative();
            }
    };




    class Power : public ElementaryFunction, public Expression{
        private:
            Expression* base_;
            Expression* power_;

            Expression* derivative(){
                if (power_ == 1){
                    return Constant(1);
                }
                else{
                    return power_ * Power(base_, power_ - 1);
                }
            }
        public:
            Power(Expression*& base, Expression* power){
                base_ = base;
                power_ = power;
            }

            Expression* get_input(){
                return base_;
            }

        
    };

    class Exp : public ElementaryFunction, public Expression{
        private:
            Expression* power_;
            Expression* base_;

            Expression* derivative(){
                return Log(power_) * Exp(base_, power_);
            }
        public:
            Exp(Constant base, Expression*& power){
                base_ = base;
                power_ = power;
            }

            Expression* get_input(){
                return power_;
            }


    };


    class Log : public ElementaryFunction, public Expression{
        private:
            Expression* base_;
            Expression* arg_;

            Expression* derivative(){
                return Constant(1) / operators::Product(arg_, Log(Constant::e, base_));
            }
        public:
            Log(Expression*& base, Expression*& arg){
                base_ = base;
                arg_ = arg;
            }

            Expression* get_input(){
                return arg_;
            }
    };
};








Expression* operator+(Expression* lhs, Expressio* rhs){
    return new operators::Sum(lhs, rhs);
}


Expression* operator-(Expression* lhs, Expression* rhs){
    return new operators::Sum(lhs, operators::Product(-1, rhs));
}


Expression* operator*(Expression* lhs, Expression* rhs){
    return new operators::Product(lhs, rhs);
}


Expression* operator/(Expression* lhs, Expression* rhs){
    return new operators::Fraction(lhs, rhs);
}   