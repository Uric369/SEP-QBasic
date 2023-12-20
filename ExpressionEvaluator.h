#pragma once
#ifndef EXPRESSIONEVALUATOR_H
#define EXPRESSIONEVALUATOR_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <cstdlib>
#include <queue>
#include <map>
#include "Typedef.h"
#include "Exception.h"

namespace myQBasic {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}


// Base class: AST Node
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual int calculate() const = 0;
};

// Number Node
struct NumberNode : public ASTNode {
    int value;

    explicit NumberNode(int val) : value(val) {}

    int calculate() const override {
        return value;
    }
};

// Binary Operation Node
struct BinaryOpNode : public ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(std::string operation, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
            : op(std::move(operation)), left(std::move(lhs)), right(std::move(rhs)) {}

    int calculate() const override {
        if (op == "+") {
            return left->calculate() + right->calculate();
        } else if (op == "-") {
            return left->calculate() - right->calculate();
        } else if (op == "*") {
            return left->calculate() * right->calculate();
        } else if (op == "/") {
            int divisor = right->calculate();
            if (divisor == 0) throw ParseException(ParseErrorType::DivideByZeroError, "divided by zero", -1);
            return left->calculate() / divisor;
        } else if (op == "MOD") {
            int divisor = right->calculate();
            if (divisor == 0) throw ParseException(ParseErrorType::DivideByZeroError, "mod by zero", -1);
            return left->calculate() % divisor;
        } else if (op == "**") {
            return std::pow(left->calculate(), right->calculate());
        } else {
            throw ParseException(ParseErrorType::InvalidExpressionError, "invalid operator", -1);
        }
    }
};

// Variable Node
struct VariableNode : public ASTNode {
    std::string name;
    std::map<std::string, VariableInfo>& variables;

    VariableNode(std::string varName, std::map<std::string, VariableInfo>& vars)
        : name(std::move(varName)), variables(vars) {}

    int calculate() const override {
        auto it = variables.find(name);
        if (it == variables.end()) {
            throw ParseException(ParseErrorType::UndefinedVariableError, "undefined variable: " + name, -1);
        }
        it->second.usageCount++; // Increment usage count
        return it->second.value;
    }
};

void trimWhitespace(const std::string& expr, size_t& pos);

// Function prototypes
std::unique_ptr<ASTNode> parseExpression(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables);
std::unique_ptr<ASTNode> parseTerm(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables);
std::unique_ptr<ASTNode> parseFactor(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables);
std::unique_ptr<ASTNode> parsePrimary(const std::string& expr, size_t& pos, std::map<std::string, VariableInfo>& variables);


// ExpressionEvaluator class
class ExpressionEvaluator {
private:
    std::unique_ptr<ASTNode> root;
    std::map<std::string, VariableInfo>& variables;

public:
    ExpressionEvaluator(const std::string& expression, std::map<std::string, VariableInfo>& vars)
        : variables(vars) {
        size_t pos = 0;
        root = parseExpression(expression, pos, variables);
    }

    int getValue() const {
        return root->calculate();
    }

    std::string syntaxTree() const {
            if (!root) {
                std::cout << "The syntax tree is empty!" << std::endl;
                return nullptr;
            }

            std::queue<const ASTNode*> nodesQueue;
            nodesQueue.push(root.get());
            nodesQueue.push(nullptr); // End-of-level marker for root
            std::string offset = "";
            std::string syntaxTree = "";

            while (!nodesQueue.empty()) {
                const ASTNode* current = nodesQueue.front();
                nodesQueue.pop();


                if (current == nullptr) {
                    // End of a level
                    std::cout << std::endl;
                    offset += retract; // Increase offset for the next level

                    if (!nodesQueue.empty()) {
                        nodesQueue.push(nullptr); // Marker for the next level
                    }
                } else {
                    // Output based on the type of node
                    if (const NumberNode* numNode = dynamic_cast<const NumberNode*>(current)) {
                        std::cout << offset << numNode->value << std::endl;
                        syntaxTree += offset + std::to_string(numNode->value) + "\n";
                    } else if (const BinaryOpNode* binOpNode = dynamic_cast<const BinaryOpNode*>(current)) {
                        std::cout << offset << binOpNode->op << std::endl;
                        syntaxTree += offset + binOpNode->op + "\n";

                        nodesQueue.push(binOpNode->left.get());
                        nodesQueue.push(binOpNode->right.get());
                    }
                }
            }
            return syntaxTree;
    }

    std::string syntaxTreeWithOffset(int count) const {
            if (!root) {
                std::cout << "The syntax tree is empty!" << std::endl;
                return nullptr;
            }

            std::queue<const ASTNode*> nodesQueue;
            nodesQueue.push(root.get());
            nodesQueue.push(nullptr); // End-of-level marker for root
            std::string offset;
            for (int i = 0; i < count; ++i) {
                offset += retract; // 将n添加到a上，重复四次
            }
            std::string syntaxTree = "";

            while (!nodesQueue.empty()) {
                const ASTNode* current = nodesQueue.front();
                nodesQueue.pop();


                if (current == nullptr) {
                    // End of a level
                    std::cout << std::endl;
                    offset += retract; // Increase offset for the next level

                    if (!nodesQueue.empty()) {
                        nodesQueue.push(nullptr); // Marker for the next level
                    }
                } else {
                    // Output based on the type of node
                    if (const NumberNode* numNode = dynamic_cast<const NumberNode*>(current)) {
                        std::cout << offset << numNode->value << std::endl;
                        syntaxTree += offset + std::to_string(numNode->value) + "\n";
                    } else if (const VariableNode* varNode = dynamic_cast<const VariableNode*>(current)) {
                        std::cout << offset << varNode->name << std::endl;
                        syntaxTree += offset + varNode->name + "\n";
                    } else if (const BinaryOpNode* binOpNode = dynamic_cast<const BinaryOpNode*>(current)) {
                        std::cout << offset << binOpNode->op << std::endl;
                        syntaxTree += offset + binOpNode->op + "\n";

                        nodesQueue.push(binOpNode->left.get());
                        nodesQueue.push(binOpNode->right.get());
                    }
                }
            }
            return syntaxTree;
    }


};



//int main() {
//    std::string expression = "(3 + 2) ** (3 MOD 2)";
//    try {
//        ExpressionEvaluator evaluator(expression);
//        int result = evaluator.getValue();
//        std::cout << expression << " = " << result << std::endl;

//        std::cout << "Syntax Tree (Level-order traversal):" << std::endl;
//        evaluator.syntaxTree();
//    } catch (const std::runtime_error& e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//    return 0;
//}


#endif // EXPRESSIONEVALUATOR_H
