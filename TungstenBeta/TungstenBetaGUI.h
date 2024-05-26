#ifndef TUNGSTENBETA_GUI_H
#define TUNGSTENBETA_GUI_H


#include "TungstenBeta.h"

#include <gtk/gtk.h>

#include <sstream>
#include <stack>
#include <map>
#include <algorithm>
#include <cctype>
#include <queue>

int run(int argc, char *argv[]);
const Expression* construct_expression_from_rpn(std::queue<std::string>& rpn);
const Expression* parse_expression(const std::string& input);

#endif //TUNGSTENBETA_GUI