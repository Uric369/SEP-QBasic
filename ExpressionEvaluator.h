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
#include "Exception.h"

namespace myQBasic {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args);
}

class Program;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual int calculate() const = 0;
};

struct NumberNode : public ASTNode {
    int value;
    explicit NumberNode(int val);

    int calculate() const override;
};

struct BinaryOpNode : public ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(std::string operation, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs);

    int calculate() const override;
};

struct VariableNode : public ASTNode {
    std::string name;
    Program *program;

    VariableNode(std::string varName, Program *program);

    int calculate() const override;
};

void trimWhitespace(const std::string& expr, size_t& pos);

std::unique_ptr<ASTNode> parseExpression(const std::string& expr, size_t& pos, Program *program);
std::unique_ptr<ASTNode> parseTerm(const std::string& expr, size_t& pos, Program *program);
std::unique_ptr<ASTNode> parseFactor(const std::string& expr, size_t& pos, Program *program);
std::unique_ptr<ASTNode> parsePrimary(const std::string& expr, size_t& pos, Program *program);

class ExpressionEvaluator {
private:
    std::unique_ptr<ASTNode> root;
    Program *program;
    std::vector<std::string> splitByNewline(const std::string& str) const;
public:
    ExpressionEvaluator(const std::string& expression, Program *program);

    int getValue() const;

    std::string syntaxTree() const;

    std::string levelOrder() const;

    std::string syntaxTreeWithOffset(int count) const;

    std::string syntaxTreeWithRunStatistics(int offset) const;

    std::string mergeTrees(ExpressionEvaluator &B) const;
};

#endif // EXPRESSIONEVALUATOR_H
