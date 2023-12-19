#pragma once
#ifndef STATEMENT_H
#define STATEMENT_H
#include "Exception.h"
#include "Typedef.h"
#include "Program.h"
#include "ExpressionEvaluator.h"
#include <map>
#include <stdexcept>
#include <iostream>
#include <string>

//class Program;
std::string trimLeadingWhitespace(const std::string& str);
std::string trimBothEnds(const std::string& str);

class Statement{
//    friend class Program;

protected:
    statementType type;
    std::string statement;
    int lineNumber;
public:
    Statement();
    virtual ~Statement()=0;
    virtual void parse(Program &program)=0;
    virtual void exec(Program &program)=0;
    virtual std::string syntaxTree() const;
};


class Arithstatement:public Statement{
//    friend class Program;

private:
    ExpressionEvaluator* expressionEvaluator;
public:
    Arithstatement();
    Arithstatement(int lineNumber, std::string statement);
    virtual ~Arithstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    int getValue();
//    Tree syntaxTree() override;
    virtual std::string syntaxTree() const override;
    std::string syntaxTreeWithOffset(int offset) const;
};

class REMstatement:public Statement{
private:
    std::string remark;
    int runTime;
public:
    REMstatement(int lineNumber, std::string statement);
    virtual ~REMstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};

class LETstatement:public Statement{
private:
    std::string LHS;
    Arithstatement RHS;
    int runTime;
public:
    LETstatement(int lineNumber, std::string statement);
    virtual ~LETstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};


class IFstatement:public Statement{
private:
    Arithstatement LHS;
    Arithstatement RHS;
    Operator ifOperator;
    int toLine;
    int trueTime;
    int falseTime;
public:
    IFstatement(int lineNumber, std::string statement);
    virtual ~IFstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};

class PRINTstatement:public Statement{
private:
    Arithstatement print;
    int runTime;
public:
    PRINTstatement(int lineNumber, std::string statement);
    virtual ~PRINTstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};

class INPUTstatement:public Statement{
private:
    std::string input;
    int runTime;
public:
    INPUTstatement(int lineNumber, std::string statement);
    virtual ~INPUTstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};


class GOTOstatement:public Statement{
private:
    int toLine;
    int runTime;
public:
    GOTOstatement(int lineNumber, std::string statement);
    virtual ~GOTOstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};


class ENDstatement:public Statement{
private:
    int runTime;
public:
    ENDstatement(int lineNumber, std::string statement);
    virtual ~ENDstatement();
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
};



#endif // STATEMENT_H
