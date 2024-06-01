// TungstenBetaGUI.cpp
#include "TungstenBetaGUI.h"

#include <gtk/gtk.h>

#include <sstream>
#include <stack>
#include <map>
#include <algorithm>
#include <cctype>
#include <queue>

GtkWindow *window;
GtkEntry *entry;
GtkLabel *output_label;
GtkButton *calculate_button;
GtkButton *find_max_button;
GtkButton *find_min_button;
GtkButton *taylor_button;
GtkButton *newton_button;
GtkLabel *variable_label;
GtkEntry *variable_entry;
GtkEntry *initial_guess_entry;


const Expression* parsed_expression = nullptr;


#include <iostream>
#include <sstream>
#include <stack>
#include <queue>
#include <map>
#include <string>
#include <cctype>

const Expression* construct_expression_from_rpn(std::queue<std::string>& rpn) {
    std::stack<const Expression*> expressionStack;
    while (!rpn.empty()) {
        std::string token = rpn.front();
        std::cout << token << " ";
        rpn.pop();

        if (std::isdigit(token[0]) || (token[0] == '-' && token.length() > 1 && std::isdigit(token[1]))) {
            expressionStack.push(new Constant(std::stoll(token)));
        } 

        else if (std::isalpha(token[0])) {
            expressionStack.push(new Variable(token));
        } 

        else if (token == "e") {
            expressionStack.push(Constant::e);
        } 
        else if (token == "pi") {
            expressionStack.push(Constant::pi);
        } 

        else if (token == "sin" || token == "cos" || token == "tan" || token == "sqrt" || token == "ln" || token == "lg") {
            if (expressionStack.empty()) {
                return nullptr;
            }
            const Expression* operand = expressionStack.top();
            expressionStack.pop();

            if (token == "sin") {
                expressionStack.push(new ElementaryFunctions::Sin(operand));
            } 
            else if (token == "cos") {
                expressionStack.push(new ElementaryFunctions::Cos(operand));
            } 
            else if (token == "tan") {
                expressionStack.push(new ElementaryFunctions::Tan(operand));
            } 
            else if (token == "sqrt") {
                expressionStack.push(new ElementaryFunctions::Power(operand, new operators::Fraction(Constant::ONE, new Constant(2))));
            } 
            else if (token == "ln") {
                expressionStack.push(new ElementaryFunctions::Log(Constant::e, operand));
            } 
            else if (token == "lg") {
                expressionStack.push(new ElementaryFunctions::Log(new Constant(10), operand));
            }
        }

        else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            if (expressionStack.size() < 2) {
                return nullptr; 
            }
            const Expression* rhs = expressionStack.top();
            expressionStack.pop();
            const Expression* lhs = expressionStack.top();
            expressionStack.pop();

            if (token == "+") {
                expressionStack.push(new operators::Sum({lhs, rhs}));
            } 
            else if (token == "-") {
                expressionStack.push(new operators::Sum({lhs, new operators::Product({new Constant(-1), rhs})}));
            } 
            else if (token == "*") {
                expressionStack.push(new operators::Product({lhs, rhs}));
            } 
            else if (token == "/") {
                expressionStack.push(new operators::Fraction(lhs, rhs));
            } 
            else if (token == "^") {
                if (!hasVariables(lhs)){
                    expressionStack.push(new ElementaryFunctions::Exp(lhs, rhs)); 
                }
                else if (!hasVariables(rhs)){
                    expressionStack.push(new ElementaryFunctions::Power(lhs, rhs)); 
                }
            }
        } 
    }

    return expressionStack.top()->simplify();
}

const Expression* parse_expression(const std::string& input) {
    std::cout << input << "\n";
    std::istringstream iss(input);
    std::stack<std::string> operatorStack;
    std::queue<std::string> outputQueue;
    std::map<std::string, int> operatorPrecedence{
        {"^", 1},
        {"*", 2}, {"/", 2},
        {"+", 3}, {"-", 3},
        {"=", 4}
    };

    std::string token;
    while (iss >> token) {
        if (token == "(") {
            operatorStack.push(token);
        } 
        else if (token == ")") {
        while (!operatorStack.empty() && operatorStack.top() != "(") {
            if (operatorPrecedence.find(operatorStack.top()) != operatorPrecedence.end() &&
                operatorPrecedence[operatorStack.top()] <= operatorPrecedence[token]) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            } else {
                return nullptr;
            }
        }
        if (operatorStack.empty() || operatorStack.top() != "(") {
            return nullptr;
        }
        operatorStack.pop(); 
    } 
        else if (operatorPrecedence.find(token) != operatorPrecedence.end()) { 
            while (!operatorStack.empty() && operatorPrecedence[operatorStack.top()] < operatorPrecedence[token]) {
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(token);
        } 
        else {
            outputQueue.push(token);
        }
    }


    while (!operatorStack.empty()) {
        if (operatorStack.top() == "(" || operatorStack.top() == ")") {
            return nullptr;
        }
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return construct_expression_from_rpn(outputQueue);
}


void update_output_label(const Expression* expr) {
    if (expr) {
        std::string output = "Result: " + expr->to_string() + " = " + std::to_string(expr->calculate());
        gtk_label_set_text(output_label, output.c_str());
    } else {
        gtk_label_set_text(output_label, "Invalid expression");
    }
}


void on_calculate_button_clicked(GtkButton *button, gpointer user_data) {
    const char *input_text = gtk_editable_get_text(GTK_EDITABLE (entry));
    std::string input(input_text);

    const char *variable_text = gtk_editable_get_text(GTK_EDITABLE (variable_entry));
    std::string variable(variable_text);

    Variable::variables[variable] = new Constant(0);

    if (parsed_expression != nullptr) {
        delete parsed_expression;
    }
    parsed_expression = parse_expression(input);
    update_output_label(parsed_expression);
}


void on_find_max_button_clicked(GtkButton *button, gpointer user_data) {
    if (parsed_expression != nullptr) {
        const char *variable_text = gtk_editable_get_text(GTK_EDITABLE (variable_entry)); 
        std::string variable(variable_text);
        
        const Expression* derivative = parsed_expression->complex_derivative(variable);
        if (derivative != nullptr && hasVariables(derivative)) {
            const Expression* root = NewtonMethod::Newton_root(derivative, variable, 0); 
            if (root) {
                Variable::variables[variable] = root;
                double max_value = parsed_expression->calculate();
                std::string output = "Maximum value: " + std::to_string(max_value);
                gtk_label_set_text(output_label, output.c_str());
                delete root;
            } else {
                gtk_label_set_text(output_label, "Failed to find a maximum.");
            }
            delete derivative;
        } else {
            gtk_label_set_text(output_label, "Expression is not a function of the variable or derivative failed.");
        }
    } else {
        gtk_label_set_text(output_label, "No expression parsed");
    }
}


void on_find_min_button_clicked(GtkButton *button, gpointer user_data) {
    if (parsed_expression != nullptr) {
        const char *variable_text = gtk_editable_get_text(GTK_EDITABLE (variable_entry)); 
        std::string variable(variable_text);
        
        const Expression* derivative = parsed_expression->complex_derivative(variable);
        if (derivative != nullptr && hasVariables(derivative)) {
            const Expression* root = NewtonMethod::Newton_root(derivative, variable, 0); 
            if (root) {
                Variable::variables[variable] = root;
                double min_value = parsed_expression->calculate();
                std::string output = "Minimum value: " + std::to_string(min_value);
                gtk_label_set_text(output_label, output.c_str());
                delete root;
            } else {
                gtk_label_set_text(output_label, "Failed to find a minimum.");
            }
            delete derivative;
        } else {
            gtk_label_set_text(output_label, "Expression is not a function of the variable or derivative failed.");
        }
    } else {
        gtk_label_set_text(output_label, "No expression parsed");
    }
}


void on_taylor_button_clicked(GtkButton *button, gpointer user_data) {
    if (parsed_expression != nullptr) {
        const char *variable_text = gtk_editable_get_text(GTK_EDITABLE (variable_entry)); 
        std::string variable(variable_text);
        
        // TODO: Implement logic to generate the Taylor series representation.
        const Expression* taylor = Taylor_series(parsed_expression, variable, 0);
        std::string output = "Taylor series: " + taylor->to_string();
        gtk_label_set_text(output_label, output.c_str());
        delete taylor;
    } else {
        gtk_label_set_text(output_label, "No expression parsed");
    }
}


void on_newton_button_clicked(GtkButton *button, gpointer user_data) {
    const char *input_text = gtk_editable_get_text(GTK_EDITABLE (entry));
    std::string input(input_text);

    const char *variable_text = gtk_editable_get_text(GTK_EDITABLE (variable_entry));
    std::string variable(variable_text);

    const char *initial_guess_text = gtk_editable_get_text(GTK_EDITABLE (initial_guess_entry));
    double initial_guess = std::stod(initial_guess_text);

    std::cout << variable << "\n";

    Variable::variables[variable] = new Constant(0); 

    if (parsed_expression != nullptr) {
        delete parsed_expression;
    }
    parsed_expression = parse_expression(input);

    if (parsed_expression) {
        const Expression* root = NewtonMethod::Newton_root(parsed_expression, variable, initial_guess);
        if (root != nullptr) {
            std::string output = "Root found: " + root->to_string();
            gtk_label_set_text(output_label, output.c_str());
            delete root;
        }
        else {
            gtk_label_set_text(output_label, "Newton's method failed to converge.");
        }
    }
    else {
        gtk_label_set_text(output_label, "Invalid expression");
    }
}

void activate(GtkApplication* app, gpointer user_data) {
    // Create the main window 
    window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(window, "Tungsten Beta Calculator");
    gtk_window_set_default_size(window, 400, 300);
    gtk_window_set_resizable(window, TRUE);

    // Create a vertical box container
    GtkBox *vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));

    // Create the input entry
    entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(entry, "Enter your expression");
    gtk_box_append(vbox, GTK_WIDGET(entry));

    // Create the output label
    output_label = GTK_LABEL(gtk_label_new("Output:"));
    gtk_box_append(vbox, GTK_WIDGET(output_label));

    // Create the calculate button
    calculate_button = GTK_BUTTON(gtk_button_new_with_label("Calculate"));
    g_signal_connect(calculate_button, "clicked", G_CALLBACK(on_calculate_button_clicked), NULL);
    gtk_box_append(vbox, GTK_WIDGET(calculate_button));

    // Create the find max button
    find_max_button = GTK_BUTTON(gtk_button_new_with_label("Find Max"));
    g_signal_connect(find_max_button, "clicked", G_CALLBACK(on_find_max_button_clicked), NULL);
    gtk_box_append(vbox, GTK_WIDGET(find_max_button));

    // Create the find min button
    find_min_button = GTK_BUTTON(gtk_button_new_with_label("Find Min"));
    g_signal_connect(find_min_button, "clicked", G_CALLBACK(on_find_min_button_clicked), NULL);
    gtk_box_append(vbox, GTK_WIDGET(find_min_button));

    // Create the Taylor series button
    taylor_button = GTK_BUTTON(gtk_button_new_with_label("Taylor Series"));
    g_signal_connect(taylor_button, "clicked", G_CALLBACK(on_taylor_button_clicked), NULL);
    gtk_box_append(vbox, GTK_WIDGET(taylor_button));

    // Create the Newton's method button
    newton_button = GTK_BUTTON(gtk_button_new_with_label("Find Root"));
    g_signal_connect(newton_button, "clicked", G_CALLBACK(on_newton_button_clicked), NULL);
    gtk_box_append(vbox, GTK_WIDGET(newton_button));

    // Create the variable label and entry
    variable_label = GTK_LABEL(gtk_label_new("Variable:"));
    gtk_box_append(vbox, GTK_WIDGET(variable_label));
    variable_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(variable_entry, "x");
    gtk_box_append(vbox, GTK_WIDGET(variable_entry));

    // Create the initial guess entry
    initial_guess_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(initial_guess_entry, "Initial Guess");
    gtk_box_append(vbox, GTK_WIDGET(initial_guess_entry));

    // Set the box container as the child of the window
    gtk_window_set_child(window, GTK_WIDGET(vbox));

    // Show the window
    gtk_window_present(window);

    gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
}

int run(int argc, char *argv[]) {
    // Create the application
    GtkApplication *app;
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS); 
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
