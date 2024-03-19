#include <iostream>
#include <string>





namespace operators{
    template <class T1, class T2>
    class Sum{
        private:
            T1 lhs_;
            T2 rhs_;
        public:
            Sum(T1 lhs, T2 rhs) : lhs_(lhs), rhs_(rhs) {}
            
            Sum derivative(){
                return Sum(lhs_.complex_derivative(), rhs_.complex_derivative());
            }
        };

    template <class T1, class T2>
    class Product{
        private:
            T1 lhs_;
            T2 rhs_;
        public:
            Product(T1 lhs, T2 rhs) : lhs_(lhs), rhs_(rhs) {}

            Sum<T1, T2> derivative(){
                return Sum(Product(lhs_.complex_derivative(), rhs_), Product(rhs_.complex_derivative(), lhs_));
            }
    };

    template <class T1, class T2>
    class Quotient{
        private:
            T1 dividend_;
            T2 divisor_;
        public:
            Quotient(T1 dividend, T2 divisor) : dividend_(dividend), divisor_(divisor) {}

            Quotient derivative(){
                return Quotient(Sum(Product(dividend_.complex_derivative(), divisor_), Product(divisor_.complex_derivative(), dividend_)), Product(divisor_, divisor_));
            }
    };
};

class Constant{
    public:
        static const double e = 2.718281828459045;
        static const double pi = 3.141592653589793;
        int value_;

        Constant(int value) : value_(value) {}

        Constant derivative(){
            return Constant(0);
        }
};

class Variable{
    private:
        std::string name_;

    public:
        Variable(std::string name) : name_(name) {}

        Constant derivative(){
            return Constant(1);
        }
};

template <class T>
class ElementaryFunction{
    protected:
        T input_;
    public:
        ElementaryFunction(T input) : input_(input) {}

        operators::Product complex_derivative(){
            if (typeid(this->get_input()) == typeid(Variable)){
                return operators::Product<Constant, Constant>(Constant(1), Constant(1));;
            }

            return Product(this->derivative(), this->get_input().complex_derivative());
        }
};


namespace ElementaryFunctions{
    template <class T>
    class Power : public ElementaryFunction<T>{
        private:
            T base_;
            Constant power_;
        public:
            Power(T base, Constant power) : ElementaryFunction<T>(base), base_(base), power_(power) {}

            T get_input(){
                return base_;
            }

            operators::Product<T, T> derivative(){
                return operators::Product<T, T>(power_, Power(base_, Constant(power_.value_ - 1)));
            }
    };

    template <class T>
    class Exp : public ElementaryFunction<T>{
        private:
            T power_;
            Constant base_;
        public:
            Exp(Constant base, T power) : ElementaryFunction<T>(power), base_(base), power_(power) {}

            T get_input(){
                return power_;
            }

            operators::Product<T, T> derivative(){
                return operators::Product<T, T>(Log(base_, power_), Exp(base_, power_));
            }
    };

    template <class T1, class T2>
    class Log : public ElementaryFunction<T2>{
        private:
            T1 base_;
            T2 arg_;
        public:
            Log(T1 base, T2 arg) : ElementaryFunction<T2>(arg), base_(base), arg_(arg) {}

            T2 get_input(){
                return arg_;
            }

            operators::Product<T1, T2> derivative(){
                return operators::Product<T1, T2>(Constant(1), operators::Quotient<T1, T2>(Constant(1), operators::Product<T1, T2>(arg_, Log(Constant::e, base_))));
            }
    };
};

template <class T1, class T2>
operators::Sum<T1, T2> operator+(T1 lhs, T2 rhs){
    return operators::Sum<T1, T2>(lhs, rhs);
}

template <class T1, class T2>
operators::Sum<T1, T2> operator-(T1 lhs, T2 rhs){
    return operators::Sum<T1, T2>(lhs, operators::Product<int, T2>(-1, rhs));
}

template <class T1, class T2>
operators::Product<T1, T2> operator*(T1 lhs, T2 rhs){
    return operators::Product<T1, T2>(lhs, rhs);
}

template <class T1, class T2>
operators::Quotient<T1, T2> operator/(T1 lhs, T2 rhs){
    return operators::Quotient<T1, T2>(lhs, rhs);
}






int main(int argc, char *argv[]) {
    Constant(2) / Constant(1);
}


