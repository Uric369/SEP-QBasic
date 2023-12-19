#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H
//#include "Statement.h"
#include <vector>
#include <memory>
#include "Typedef.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "Typedef.h"
#include <map>

class Statement;

class Program {
private:
    std::vector<std::string> raw;
    std::map<std::string, VariableInfo> variables;
    std::map<int, Statement*> statements;
    std::string print;
    int ifTrue;
    int currentLine;
    int maxLine;
    bool hasEND;
    friend class Statement;
    friend class Arithstatement;
    friend class REMstatement;
    friend class LETstatement;
    friend class IFstatement;
    friend class PRINTstatement;
    friend class INPUTstatement;
    friend class GOTOstatement;
    friend class ENDstatement;
public:
    Program();
    void Load(std::string path);
    void saveLine(int lineNumber, std::string cmd);
    void exec();
    void reset();
};

#endif // PROGRAM_H
