#pragma once
#ifndef TYPEDEF_H
#define TYPEDEF_H
#include <iostream>

struct VariableInfo {
    int value;      // 变量的值
    int usageCount; // 变量的使用次数
};

enum statementType {
    REM,
    LET,
    IF,
    PRINT,
    INPUT,
    GOTO,
    END,
    Arith,
};

enum Operator {
    Equal,      // =
    Greater,    // >
    Less        // <
};


#endif // TYPEDEF_H
