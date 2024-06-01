#include "ElementaryFunctions.h"
#include "operators.h"
#include "Constant.h"

namespace ElementaryFunctions {
    const Expression* ElementaryFunction::complex_derivative(const std::string& variable) const {
        return (new operators::Product({this->derivative(variable), (this->get_input())->complex_derivative(variable)}))->simplify();
    }

    ElementaryFunction::ElementaryFunction() = default;
    ElementaryFunction::~ElementaryFunction() = default;

    void ElementaryFunction::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // Power
    Power::Power(const Expression* base, const Expression* power) {
        base_ = base;
        power_ = power;
    }

    double Power::calculate() const {
        return std::pow(base_->calculate(), power_->calculate());
    }

    const Expression* Power::copy() const {
        return (new Power(base_, power_))->simplify();
    }

    const Expression* Power::simplify() const {
        if (base_->simplify() == Constant::ONE) {
            return Constant::ONE;
        }
        if ((base_->simplify() == Constant::ZERO) && (power_->simplify() != Constant::ZERO)) {
            return Constant::ZERO;
        }
        if (power_->simplify() == Constant::ONE) {
            return base_->simplify();
        }
        if (typeid(*base_) == typeid(Power)) {
            return new Power(static_cast<const Power*>(base_)->get_base(), (new operators::Product({power_, static_cast<const Power*>(base_)->get_power()}))->simplify());
        }
        return new Power(base_->simplify(), power_->simplify());
    }

    const Expression* Power::derivative(const std::string& variable) const {
        if (power_->calculate() == 1) {
            return Constant::ONE;
        } else {
            return (new operators::Product({
                new Constant(power_->calculate()),
                new Power(base_, new Constant(power_->calculate() - 1))
            }))->simplify();
        }
    }

    const Expression* Power::plug_variable(const std::string& variable) const {
        return (new Power(base_->plug_variable(variable)->simplify(), power_->plug_variable(variable)))->simplify();
    }

    const Expression* Power::get_input() const {
        return base_;
    }

    std::string Power::to_string() const {
        std::string s;
        if (power_->calculate() == 1) {
            s = base_->to_string();
        } else {
            s = "(" + base_->to_string() + "^" + power_->to_string() + ")";
        }
        return s;
    }

    void Power::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // Exponent
    Exp::Exp(const Expression* base, const Expression* power) {
        base_ = base;
        power_ = power;
    }

    double Exp::calculate() const {
        return std::exp(power_->calculate() * std::log(base_->calculate()));
    }

    const Expression* Exp::copy() const {
        return (new Exp(base_, power_))->simplify();
    }

    const Expression* Exp::simplify() const {
        if (base_->simplify() == Constant::ONE) {
            return Constant::ONE;
        }
        if ((base_->simplify() == Constant::ZERO) && (power_->simplify() != Constant::ZERO)) {
            return Constant::ZERO;
        }
        if (power_->simplify() == Constant::ONE) {
            return base_->simplify();
        }
        if (typeid(*base_) == typeid(Power)) {
            return new Exp(static_cast<const Power*>(base_)->get_base()->simplify(), (new operators::Product({power_, static_cast<const Power*>(base_)->get_power()}))->simplify());
        }
        return new Exp(base_->simplify(), power_->simplify());
    }

    const Expression* Exp::derivative(const std::string& variable) const {
        return (new operators::Product({
            new Exp(base_, power_),
            new Log(Constant::e, base_)
        }))->simplify();
    }

    const Expression* Exp::plug_variable(const std::string& variable) const {
        return (new Exp(base_->plug_variable(variable)->simplify(), power_->plug_variable(variable)))->simplify();
    }

    const Expression* Exp::get_input() const {
        return power_;
    }

    std::string Exp::to_string() const {
        std::string s;
        s = base_->to_string() + "^(" + power_->to_string() + ")";
        return s;
    }

    void Exp::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // LOgarithm
    Log::Log(const Expression* base, const Expression* arg) {
        base_ = base;
        arg_ = arg;
    }

    double Log::calculate() const {
        return std::log(arg_->calculate()) / std::log(base_->calculate());
    }

    const Expression* Log::copy() const {
        return (new Log(base_, arg_))->simplify();
    }

    const Expression* Log::simplify() const {
        if (base_->to_string() == arg_->to_string()) {
            return Constant::ONE;
        }
        return new Log(base_->simplify(), arg_->simplify());
    }

    const Expression* Log::derivative(const std::string& variable) const {
        return (new operators::Fraction(
            Constant::ONE,
            new operators::Product({
                arg_,
                new Log(Constant::e, base_)
            })))->simplify();
    }

    const Expression* Log::plug_variable(const std::string& variable) const {
        return (new Log(base_->plug_variable(variable), arg_->plug_variable(variable)))->simplify();
    }

    const Expression* Log::get_input() const {
        return arg_;
    }

    std::string Log::to_string() const {
        std::string s;
        if (base_ == Constant::e) {
            s = "ln(" + arg_->to_string() + ")";
        } else {
            s = "log[" + base_->to_string() + ", " + arg_->to_string() + "]";
        }
        return s;
    }

    void Log::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // Sin
    Sin::Sin(const Expression* arg) {
        arg_ = arg;
    }

    double Sin::calculate() const {
        return std::sin(arg_->calculate());
    }

    const Expression* Sin::copy() const {
        return (new Sin(arg_))->simplify();
    }

    const Expression* Sin::simplify() const {
        return new Sin(arg_->simplify());
    }

    const Expression* Sin::derivative(const std::string& variable) const {
        return (new Cos(arg_))->simplify();
    }

    const Expression* Sin::plug_variable(const std::string& variable) const {
        return (new Sin(arg_->plug_variable(variable)))->simplify();
    }

    const Expression* Sin::get_input() const {
        return arg_;
    }

    std::string Sin::to_string() const {
        return "Sin(" + arg_->to_string() + ")";
    }

    void Sin::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // Cos
    Cos::Cos(const Expression* arg) {
        arg_ = arg;
    }

    double Cos::calculate() const {
        return cos(arg_->calculate());
    }

    const Expression* Cos::copy() const {
        return (new Cos(arg_))->simplify();
    }

    const Expression* Cos::simplify() const {
        return new Cos(arg_->simplify());
    }

    const Expression* Cos::derivative(const std::string& variable) const {
        return (new operators::Product({new Constant(-1), new Sin(arg_)}))->simplify();
    }

    const Expression* Cos::plug_variable(const std::string& variable) const {
        return (new Cos(arg_->plug_variable(variable)))->simplify();
    }

    const Expression* Cos::get_input() const {
        return arg_;
    }

    std::string Cos::to_string() const {
        return "Cos(" + arg_->to_string() + ")";
    }

    void Cos::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // Tan
    Tan::Tan(const Expression* arg) {
        arg_ = arg;
    }

    double Tan::calculate() const {
        return tan(arg_->calculate());
    }

    const Expression* Tan::copy() const {
        return (new Tan(arg_))->simplify();
    }

    const Expression* Tan::simplify() const {
        return new Tan(arg_->simplify());
    }

    const Expression* Tan::derivative(const std::string& variable) const {
        return (new operators::Fraction(
            Constant::ONE,
            new Power(new Cos(arg_), new Constant(2))))->simplify();
    }

    const Expression* Tan::plug_variable(const std::string& variable) const {
        return (new Tan(arg_->plug_variable(variable)))->simplify();
    }

    const Expression* Tan::get_input() const {
        return arg_;
    }

    std::string Tan::to_string() const {
        return "tan(" + arg_->to_string() + ")";
    }

    void Tan::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }

    // Cot
    Cot::Cot(const Expression* arg) {
        arg_ = arg;
    }

    double Cot::calculate() const {
        return 1 / tan(arg_->calculate());
    }

    const Expression* Cot::copy() const {
        return (new Cot(arg_))->simplify();
    }

    const Expression* Cot::simplify() const {
        return new Cot(arg_->simplify());
    }

    const Expression* Cot::derivative(const std::string& variable) const {
        return (new operators::Product({
            new Constant(-1),
            new operators::Fraction(
                Constant::ONE,
                new Power(new Sin(arg_), new Constant(2)))
        }))->simplify();
    }

    const Expression* Cot::plug_variable(const std::string& variable) const {
        return (new Cot(arg_->plug_variable(variable)))->simplify();
    }

    const Expression* Cot::get_input() const {
        return arg_;
    }

    std::string Cot::to_string() const {
        return "cot(" + arg_->to_string() + ")";
    }

    void Cot::accept(ExpressionVisitor* visitor) {
        visitor->visitElementaryFunction(this);
    }
};