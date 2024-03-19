#include <iostream>


namespace operators{
    template <class T1, class T2>
    class Sum{
        private:
            T1 lhs_;
            T2 rhs_;
        public:
            Sum(T1& lhs, T2& rhs){
                lhs_ = lhs;
                rhs_ = rhs;
            };

            Sum derivative(){
                return lhs_.complex_derivative() + rhs_.complex_derivative();
            }
        };


    template <class T1, class T2>
    class Product{
        private:
            T1 lhs_;
            T2 rhs_;
        public:
            Product(T1& lhs, T2& rhs){
                lhs_ = lhs;
                rhs_ = rhs;
            };


            Sum derivative(){
                return lhs_.complex_derivative() * rhs_ + rhs_.complex_derivative() * lhs;
            }
    };


    template <class T1, class T2>
    class Quotient{
        private:
        T1 dividend_;
        T2 divisor_;
        public:
            Quotient derivative(){
                return Quotient(Sum(Product(dividend_.complex_derivative(), divisor_) - Product(divisor_.complex_derivative(), dividend_)), ElementaryFunctions::Power3(divisor_, 2));
            }
    };
};


class Constant{
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
    template <class T>
    class ElementaryFunction{
        private:
            T input_;
        public:
            operators::Product<class T1, class T2> complex_derivative(){
                return this->derivative() * (this->get_input()).complex_derivative();
            }
    };



    template <class T>
    class Power : public ElementaryFunction{
        private:
            T base_;
            Constant power_;

            Product<class T1, class T2> derivative(){
                if (power_ == 1){
                    return Constant(1) * Constant(1);
                }
                else{
                    return power_ * Power(base_, power_ - 1);
                }
            }
        public:
            Power(T& base, Constant power){
                base_ = base;
                power_ = power;
            }

            T get_input(){
                return base_;
            }

        
    };

    template <class T>
    class Exp : public ElementaryFunction{
        private:
            T power_;
            Constant base_;

            Product<class T1, class T2> derivative(){
                return Log(power_) * Exp(base_, power_);
            }
        public:
            Exp(Constant base, T& power){
                base_ = base;
                power_ = power;
            }

            T get_input(){
                return power_;
            }


    };


    template <class T1, class T2>
    class Log : public ElementaryFunction{
        private:
            T1 base_;
            T2 arg_;

            Product<class T1, class T2> derivative(){
                return Constant(1) / operators::Product(arg_, Log(Constant::e, base_));
            }
        public:
            Log(T1& base, T2& arg){
                base_ = base;
                arg_ = arg;
            }

            T2 get_input(){
                return arg_;
            }
    };
};







template <class T1, class T2>
operators::Sum<T1, T2> operator+(T1& lhs, T2& rhs){
    return operators::Sum(T1, T2);
}

template <class T1, class T2>
operators::Sum<T1, T2> operator-(T1& lhs, T2& rhs){
    return operators::Sum<T1, operators::Product<int, T2>>(lhs, operators::Product<int, T2>(-1, rhs));
}

template <class T1, class T2>
operators::Product<T1, T2> operator*(T1& lhs, T2& rhs){
    return operators::Product<T1, T2>(lhs, rhs);
}

template <class T1, class T2>
operators::Quotient<T1, T2> operator/(T1& lhs, T2& rhs){
    return operators::Quotient<T1, T2>(lhs, rhs);
}   