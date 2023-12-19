#include "ExpressionEvaluator.h"
// Parse a primary expression, which could be a number or a parenthesized expression
// Modify the parsePrimary function


// Helper function to skip whitespace
void trimWhitespace(const std::string& expr, size_t& pos) {
    while (pos < expr.size() && std::isspace(expr[pos])) {
        ++pos;
    }
}


std::unique_ptr<ASTNode> parsePrimary(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables) {
    trimWhitespace(expr, pos);

    if (expr[pos] == '(') {
        ++pos; // Skip '('
        auto node = parseExpression(expr, pos, variables);
        trimWhitespace(expr, pos);
        if (expr[pos] != ')') {
            throw std::runtime_error("Expected ')'");
        }
        ++pos; // Skip ')'
        return node;
    } else {
        if (std::isalpha(expr[pos])) { // If the character is a letter, it's a variable name.
            size_t startPos = pos;
            while (pos < expr.size() && std::isalnum(expr[pos])) {
                ++pos;
            }
            std::string varName = expr.substr(startPos, pos - startPos);
            return myQBasic::make_unique<VariableNode>(varName, variables);
        } else {
        bool negative = false;
        if (expr[pos] == '-') {
            negative = true;
            ++pos;
        }

        size_t startPos = pos;
        while (pos < expr.size() && (isdigit(expr[pos]) || expr[pos] == '-')) {
            ++pos;
        }
        int num = std::stoi(expr.substr(startPos, pos - startPos));
        return myQBasic::make_unique<NumberNode>(negative ? -num : num);
        }
    }
}

// Parse an exponentiation expression
std::unique_ptr<ASTNode> parseFactor(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables) {
    auto node = parsePrimary(expr, pos, variables);

    trimWhitespace(expr, pos);

    while (pos < expr.size() && expr.substr(pos, 2) == "**") {
        std::string op = expr.substr(pos, 2);
        pos += 2; // Skip '**'
        auto right = parseFactor(expr, pos, variables); // Right associative
        node = myQBasic::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
        trimWhitespace(expr, pos);
    }

    return node;
}

// Parse a multiplication/division/modulo expression
std::unique_ptr<ASTNode> parseTerm(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables) {
    auto node = parseFactor(expr, pos, variables);

    trimWhitespace(expr, pos);

    while (pos < expr.size() && (expr[pos] == '*' || expr[pos] == '/' || expr.substr(pos, 3) == "MOD")) {
        std::string op;
        if (expr.substr(pos, 3) == "MOD") {
            op = "MOD";
            pos += 3; // Skip 'MOD'
        } else {
            op = expr[pos];
            pos++; // Skip '*' or '/'
        }
        auto right = parseFactor(expr, pos, variables);
        node = myQBasic::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
        trimWhitespace(expr, pos);
    }

    return node;
}

// Parse an addition/subtraction expression
std::unique_ptr<ASTNode> parseExpression(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables) {
    auto node = parseTerm(expr, pos, variables);

    trimWhitespace(expr, pos);

    while (pos < expr.size() && (expr[pos] == '+' || expr[pos] == '-')) {
        std::string op(1, expr[pos]);
        pos++; // Skip '+' or '-'
        auto right = parseTerm(expr, pos, variables);
        node = myQBasic::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
        trimWhitespace(expr, pos);
    }

    return node;
}
