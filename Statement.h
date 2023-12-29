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
std::string printLevelOrder(const std::string &levelOrder, const std::string &retract);

class Statement{
//    friend class Program;

protected:
    std::string statement;
    int lineNumber;
    int runTime;
public:
    statementType type;
    Statement();
    virtual ~Statement()=0;
    virtual void setRunStatistics(int n)=0;
    virtual void parse(Program &program)=0;
    virtual void exec(Program &program)=0;
    virtual std::string syntaxTree() const;
    virtual std::string syntaxTreeWithRunStatistics() const;
    statementType getType() const;
    std::string getRaw()const;
    int getRunTime() const;
};


class Arithstatement:public Statement{
//    friend class Program;
private:
    ExpressionEvaluator* expressionEvaluator;
public:
    Arithstatement();
    Arithstatement(int lineNumber, std::string statement);
    virtual ~Arithstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    int getValue();
//    Tree syntaxTree() override;
    virtual std::string syntaxTree() const override;
    std::string mergeTrees(const Arithstatement &B) const;
    virtual std::string syntaxTreeWithRunStatistics() const override;
    std::string syntaxTreeWithOffset(int offset) const;
    std::string syntaxTreeWithRunStatistics(int offset) const;
};

class REMstatement:public Statement{
private:
    std::string remark;
public:
    REMstatement(int lineNumber, std::string statement);
    virtual ~REMstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};

class LETstatement:public Statement{
private:
    std::string LHS;
    Arithstatement LHS_sta;
    Arithstatement RHS;
    int runTime;
public:
    LETstatement(int lineNumber, std::string statement);
    virtual ~LETstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};


class IFstatement:public Statement{
private:
    Arithstatement LHS;
    Arithstatement RHS;
    char ifOperator;
    int toLine;
    int trueTime;
    int falseTime;
    std::vector<std::string> splitByNewline(const std::string& str) const;
    std::string mergeTrees(const std::string& A, const std::string& B) const;
    std::string printLevelOrder(const std::string &levelOrder) const;

public:
    IFstatement(int lineNumber, std::string statement);
    virtual ~IFstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};

class PRINTstatement:public Statement{
private:
    Arithstatement print;
    int runTime;
public:
    PRINTstatement(int lineNumber, std::string statement);
    virtual ~PRINTstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};

class INPUTstatement:public Statement{
private:
    std::string input;
    int runTime;
public:
    INPUTstatement(int lineNumber, std::string statement);
    virtual ~INPUTstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};


class GOTOstatement:public Statement{
private:
    int toLine;
    int runTime;
public:
    GOTOstatement(int lineNumber, std::string statement);
    virtual ~GOTOstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};


class ENDstatement:public Statement{
private:
    int runTime;
public:
    ENDstatement(int lineNumber, std::string statement);
    virtual ~ENDstatement();
    virtual void setRunStatistics(int n) override;
    virtual void parse(Program &program) override;
    virtual void exec(Program &program) override;
    virtual std::string syntaxTree() const override;
    virtual std::string syntaxTreeWithRunStatistics() const override;
};



#endif // STATEMENT_H
