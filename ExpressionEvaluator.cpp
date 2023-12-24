#include "ExpressionEvaluator.h"
#include "Program.h"
// Parse a primary expression, which could be a number or a parenthesized expression
// Modify the parsePrimary function


// Helper function to skip whitespace
void trimWhitespace(const std::string& expr, size_t& pos) {
    while (pos < expr.size() && std::isspace(expr[pos])) {
        ++pos;
    }
}


std::unique_ptr<ASTNode> parsePrimary(const std::string& expr, size_t& pos, Program *program, int lineNumber) {
    trimWhitespace(expr, pos);

    if (expr[pos] == '(') {
        ++pos; // Skip '('
        auto node = parseExpression(expr, pos, program, lineNumber);
        trimWhitespace(expr, pos);
        if (expr[pos] != ')') {
            throw ParseException(ParseErrorType::InvalidExpressionError, "missing corresponding ')'", lineNumber);
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
            return myQBasic::make_unique<VariableNode>(varName, program, lineNumber);
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
std::unique_ptr<ASTNode> parseFactor(const std::string& expr, size_t& pos, Program *program, int lineNumber) {
    auto node = parsePrimary(expr, pos, program, lineNumber);

    trimWhitespace(expr, pos);

    while (pos < expr.size() && expr.substr(pos, 2) == "**") {
        std::string op = expr.substr(pos, 2);
        pos += 2; // Skip '**'
        auto right = parseFactor(expr, pos, program, lineNumber); // Right associative
        node = myQBasic::make_unique<BinaryOpNode>(op, std::move(node), std::move(right), lineNumber);
        trimWhitespace(expr, pos);
    }

    return node;
}

// Parse a multiplication/division/modulo expression
std::unique_ptr<ASTNode> parseTerm(const std::string& expr, size_t& pos, Program *program, int lineNumber) {
    auto node = parseFactor(expr, pos, program, lineNumber);

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
        auto right = parseFactor(expr, pos, program, lineNumber);
        node = myQBasic::make_unique<BinaryOpNode>(op, std::move(node), std::move(right), lineNumber);
        trimWhitespace(expr, pos);
    }

    return node;
}

// Parse an addition/subtraction expression
std::unique_ptr<ASTNode> parseExpression(const std::string& expr, size_t& pos, Program *program, int lineNumber) {
    auto node = parseTerm(expr, pos, program, lineNumber);

    trimWhitespace(expr, pos);

    while (pos < expr.size() && (expr[pos] == '+' || expr[pos] == '-')) {
        std::string op(1, expr[pos]);
        pos++; // Skip '+' or '-'
        auto right = parseTerm(expr, pos, program, lineNumber);
        node = myQBasic::make_unique<BinaryOpNode>(op, std::move(node), std::move(right), lineNumber);
        trimWhitespace(expr, pos);
    }

    return node;
}


namespace myQBasic {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

class Program;

// Base class: AST Node
//struct ASTNode {
//    virtual ~ASTNode() = default;
//    virtual int calculate() const = 0;
//};

// Number Node
NumberNode::NumberNode(int val) : value(val) {}

int NumberNode::calculate() const {
        return this->value;
}


BinaryOpNode::BinaryOpNode(std::string operation, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs, int lineNumber)
            : op(std::move(operation)), left(std::move(lhs)), right(std::move(rhs)), lineNumber(lineNumber) {}

int BinaryOpNode::calculate() const{
        if (op == "+") {
            return left->calculate() + right->calculate();
        } else if (op == "-") {
            return left->calculate() - right->calculate();
        } else if (op == "*") {
            return left->calculate() * right->calculate();
        } else if (op == "/") {
            int divisor = right->calculate();
            if (divisor == 0) throw ParseException(ParseErrorType::DivideByZeroError, "divided by zero", lineNumber);
            return left->calculate() / divisor;
        } else if (op == "MOD") {
            int divisor = right->calculate();
            if (divisor == 0) throw ParseException(ParseErrorType::DivideByZeroError, "mod by zero", lineNumber);
            if (divisor < 0) return left->calculate() % divisor + divisor;
            return left->calculate() % divisor;
        } else if (op == "**") {
            return std::pow(left->calculate(), right->calculate());
        } else {
            throw ParseException(ParseErrorType::InvalidExpressionError, "invalid operator", lineNumber);
        }
}


// Variable Node
VariableNode::VariableNode(std::string varName, Program *program, int lineNumber)
        : name(std::move(varName)), program(program), lineNumber(lineNumber) {}

int VariableNode::calculate() const {
        auto it = program->variables.find(name);
        if (it == program->variables.end()) {
            throw ParseException(ParseErrorType::UndefinedVariableError, "undefined variable: " + name, lineNumber);
        }
        it->second.usageCount++; // Increment usage count
        return it->second.value;
}

ExpressionEvaluator::ExpressionEvaluator(const std::string& expression, Program *program, int lineNumber)
        : program(program), lineNumber(lineNumber) {
        size_t pos = 0;
        root = parseExpression(expression, pos, program, lineNumber);
    }

int ExpressionEvaluator::getValue() const {
        return root->calculate();
    }

std::string ExpressionEvaluator::syntaxTree() const {
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

std::string ExpressionEvaluator::syntaxTreeWithOffset(int count) const {
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

std::string ExpressionEvaluator::levelOrder() const {
            if (!root) {
                std::cout << "The syntax tree is empty!" << std::endl;
                return nullptr;
            }

            std::queue<const ASTNode*> nodesQueue;
            nodesQueue.push(root.get());
            nodesQueue.push(nullptr); // End-of-level marker for root
            std::string syntaxTree = "";

            while (!nodesQueue.empty()) {
                const ASTNode* current = nodesQueue.front();
                nodesQueue.pop();


                if (current == nullptr) {
                    // End of a level
                    std::cout << std::endl;
                    syntaxTree += "\n";
                    if (!nodesQueue.empty()) {
                        nodesQueue.push(nullptr); // Marker for the next level
                    }
                } else {
                    // Output based on the type of node
                    if (const NumberNode* numNode = dynamic_cast<const NumberNode*>(current)) {
                        std::cout << numNode->value << std::endl;
                        syntaxTree += std::to_string(numNode->value) + " ";
                    } else if (const VariableNode* varNode = dynamic_cast<const VariableNode*>(current)) {
                        std::cout << varNode->name << std::endl;
                        syntaxTree += varNode->name + " ";
                    } else if (const BinaryOpNode* binOpNode = dynamic_cast<const BinaryOpNode*>(current)) {
                        std::cout << binOpNode->op << std::endl;
                        syntaxTree += binOpNode->op + " ";

                        nodesQueue.push(binOpNode->left.get());
                        nodesQueue.push(binOpNode->right.get());
                    }
                }
            }
            return syntaxTree;
}


std::string ExpressionEvaluator::syntaxTreeWithRunStatistics(int offset) const {
            if (!root) {
                std::cout << "The syntax tree is empty!" << std::endl;
                return nullptr;
            }

            std::queue<const ASTNode*> nodesQueue;
            nodesQueue.push(root.get());
            nodesQueue.push(nullptr); // End-of-level marker for root
            std::string format;
            for (int i = 0; i < offset; ++i) {
                format += retract; // 将n添加到a上，重复四次
            }
            std::string syntaxTree = "";

            while (!nodesQueue.empty()) {
                const ASTNode* current = nodesQueue.front();
                nodesQueue.pop();


                if (current == nullptr) {
                    // End of a level
                    std::cout << std::endl;
                    format += retract; // Increase offset for the next level

                    if (!nodesQueue.empty()) {
                        nodesQueue.push(nullptr); // Marker for the next level
                    }
                } else {
                    // Output based on the type of node
                    if (const NumberNode* numNode = dynamic_cast<const NumberNode*>(current)) {
                        std::cout << offset << numNode->value << std::endl;
                        syntaxTree += format + std::to_string(numNode->value) + "\n";
                    } else if (const VariableNode* varNode = dynamic_cast<const VariableNode*>(current)) {
                        std::cout << offset << varNode->name << std::endl;
                        auto it = varNode->program->variables.find(varNode->name);
                        syntaxTree += format + varNode->name + " " + std::to_string(it->second.usageCount) + "\n";
                    } else if (const BinaryOpNode* binOpNode = dynamic_cast<const BinaryOpNode*>(current)) {
                        std::cout << offset << binOpNode->op << std::endl;
                        syntaxTree += format + binOpNode->op + "\n";

                        nodesQueue.push(binOpNode->left.get());
                        nodesQueue.push(binOpNode->right.get());
                    }
                }
            }
            return syntaxTree;
}


std::vector<std::string> ExpressionEvaluator::splitByNewline(const std::string& str) const {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line)) {
        result.push_back(line);
    }
    return result;
}

std::string ExpressionEvaluator::mergeTrees(ExpressionEvaluator &B) const {
    // Split the strings by new lines to get a vector of layers
    std::vector<std::string> layersA = splitByNewline(this->levelOrder());
    std::vector<std::string> layersB = splitByNewline(B.levelOrder());
    std::string mergedResult;

    // Get the maximum number of layers to iterate through all layers
    size_t numLayers = std::max(layersA.size(), layersB.size());

    for (size_t i = 0; i < numLayers; ++i) {
        // Merge layers if both trees have the current layer
        if (i < layersA.size()) {
            mergedResult += layersA[i];
        }
        if (i < layersA.size() && i < layersB.size()) {
            mergedResult += " ";
        }
        if (i < layersB.size()) {
            mergedResult += layersB[i];
        }

        // Add a new line if not the last layer
        if (i < numLayers - 1) {
            mergedResult += "\n";
        }
    }

    return mergedResult;
}




