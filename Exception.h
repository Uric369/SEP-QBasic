#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <iostream>
#include <string>
#include <sstream>

// 定义异常类型的枚举
enum class ParseErrorType {
    SyntaxError, //语法错误，当输入的代码不符合语法规则时抛出。
    TokenError, //词法错误，例如无法识别的符号或关键字。
    TypeError, //类型错误，当变量的类型不符合操作要求时抛出。
    UndefinedVariableError, //使用未定义的变量时抛出。
    UndefinedLineError, //GOTO 语句跳转到未定义的标签时抛出。
    LabelRedefinitionError, //重复定义同一个标签时抛出。
    InvalidExpressionError, //表达式无效或者解析表达式失败时抛出。
    InvalidLineNumberError,
    DivideByZeroError, //尝试除以零时抛出。
    EndWithoutIfError, //END 语句出现在没有对应 IF 语句的上下文中时抛出。
    MissingOperandError, //表达式中缺少操作数时抛出。
    MissingOperatorError, //表达式中缺少操作符时抛出。
    MissingEndError, //程序没有终止
};

// ParseException 类用于报告 QBasic 解析过程中的所有异常
class ParseException : public std::exception {
private:
    std::string message; // 错误消息
    ParseErrorType errorType; // 异常类型
    int line; // 发生错误的行号

public:
    // 构造函数，使用错误类型、错误消息和行号初始化异常
    ParseException(ParseErrorType type, const std::string& msg, int lineNumber)
        : errorType(type), message(msg), line(lineNumber) {
        std::ostringstream ss;
        ss << "Parse Error (" << static_cast<int>(type) << ") on line " << lineNumber << ": " << msg;
        message = ss.str();
    }

    // 返回错误消息
    virtual const char* what() const noexcept override {
        static std::string whatMsg;
        whatMsg = "Line " + std::to_string(line) + " : " + message;
        return whatMsg.c_str();
    }

    // 获取异常类型
    ParseErrorType getErrorType() const noexcept {
        return errorType;
    }

    // 获取发生错误的行号
    int getLine() const noexcept {
        return line;
    }
};

#endif // EXCEPTION_H


