#include "Program.h"
#include "Statement.h"
//#include "mainwindow.h"
#include <QObject>

bool hasContentAfterFirstNumber(const std::string& str) {
    bool numberFound = false; // 标记是否找到数字
    for (char ch : str) {
        if (std::isdigit(ch)) { // 如果字符是数字
            numberFound = true; // 设置标记
        } else if (numberFound && !std::isspace(ch)) {
            // 如果之前找到数字，并且当前字符不是空格，则返回true
            return true;
        }
    }
    // 如果没有找到非空格字符或没有找到数字，则返回false
    return false;
}

//Program::Program(QObject *parent) : QObject(parent) {
//    this->isInputFinished = false;
//    this->currentLine = -1;
////    this->maxLine = -1;
////    this->ifTrue = false;
//    this->hasEND = false;
//}

void Program::reset(){
    this->variables.clear();
    this->statements.clear();
    this->raw.clear();
    this->input.clear();
    this->output.clear();
    this->currentLine = -1;
//    this->maxLine = -1;
//    this->ifTrue = false;
    this->hasEND = false;
//    this->isInputFinished = false;
    this->isRunning = false;
}

void Program::Load(const std::string path){
        std::ifstream file(path);
        std::string line;
        int maxLine = -1; // Initialize with an invalid line number

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << path << std::endl;
            return;
        }

        while (getline(file, line)) {
            // Remove leading spaces
            line.erase(0, line.find_first_not_of(" \t"));

            // Skip empty lines
            if (line.empty()) continue;

            // Read line number at the beginning of the line
            std::istringstream iss(line);
            raw.push_back(line);
            int lineNumber;
            if (!(iss >> lineNumber)) {
//                std::cerr << "Error: Line number not found or invalid format" << std::endl;
                throw ParseException(ParseErrorType::InvalidLineNumberError, "invalid line number after", maxLine);
                return;
            }

            // Check if the current line number is greater than the max line number
//            if (lineNumber <= maxLine) {
//                throw ParseException(ParseErrorType::InvalidLineNumberError, "decreasing line number", lineNumber);
//                return;
//            }
//            maxLine = lineNumber;

            // Remove the line number and the following space character (if exists)
            size_t spacePos = line.find(' ');
            if (spacePos != std::string::npos) {
                saveLine(lineNumber, line.substr(spacePos + 1));
            }
            else {
                throw ParseException(ParseErrorType::InvalidExpressionError, "lack of expression", lineNumber);
            }
        }

        file.close();

        if (!hasEND) {
            throw ParseException(ParseErrorType::MissingEndError, "missing END", -1);
        }
}


void Program::LoadContent(const std::string &content) {
    std::istringstream file(content);
    std::string line;
    int cur_line = -1; // Initialize with an invalid line number

    while (getline(file, line)) {
        // Remove leading spaces
        line.erase(0, line.find_first_not_of(" \t"));

        // Skip empty lines
        if (line.empty()) continue;

        // Read line number at the beginning of the line
        std::istringstream iss(line);
        raw.push_back(line);
        int lineNumber;
        if (!(iss >> lineNumber)) {
            throw ParseException(ParseErrorType::InvalidLineNumberError, "invalid line number after", cur_line);
        }

        cur_line = lineNumber;

        // Remove the line number and the following space character (if exists)
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            saveLine(lineNumber, line.substr(spacePos + 1));
        }
        else {
            throw ParseException(ParseErrorType::InvalidExpressionError, "lack of expression", lineNumber);
        }
    }

    if (!hasEND) {
        throw ParseException(ParseErrorType::MissingEndError, "missing END", -1);
    }
}


std::string Program::display() const{
    std::string result;
    for (auto it = this->statements.begin(); it != statements.end(); ++it) {
        result += it->second->getRaw();
    }
    return result;
}

void Program::saveLine(int lineNumber, std::string cmd){
    std::cout << "saveLine: " << cmd << std::endl;
    std::istringstream iss(cmd);
    std::string firstWord;
    iss >> firstWord;
    if (firstWord.length() == 0) return;
    if (firstWord == "REM") {
        statements[lineNumber] = new REMstatement(lineNumber, cmd);
        return;
    }
    if (firstWord == "LET"){
        statements[lineNumber] = new LETstatement(lineNumber, cmd);
        return;
    }
    if (firstWord == "IF"){
        statements[lineNumber] = new IFstatement(lineNumber, cmd);
        return;
    }
    if (firstWord == "PRINT"){
        statements[lineNumber] = new PRINTstatement(lineNumber, cmd);
        return;
    }
    if (firstWord == "INPUT"){
        statements[lineNumber] = new INPUTstatement(lineNumber, cmd);
        return;
    }
    if (firstWord == "GOTO"){
        statements[lineNumber] = new GOTOstatement(lineNumber, cmd);
        return;
    }
    if (firstWord == "END"){
        statements[lineNumber] = new ENDstatement(lineNumber, cmd);
        this->hasEND = true;
        return;
    }
    throw ParseException(ParseErrorType::InvalidExpressionError, "unknown expression", lineNumber);
}

void Program::execLine(std::string cmd){
    std::istringstream iss(cmd);
    std::string firstWord;
    iss >> firstWord;
    if (firstWord == "LET"){
        this->output.clear();
        LETstatement execStmt = LETstatement(-1, cmd);
        execStmt.parse(*this);
        execStmt.exec(*this);
        return;
    }
    if (firstWord == "PRINT"){
        this->output.clear();
        PRINTstatement execStmt = PRINTstatement(-1, cmd);
        execStmt.parse(*this);
        execStmt.exec(*this);
        return;
    }
    if (firstWord == "INPUT"){
        this->output.clear();
        INPUTstatement execStmt = INPUTstatement(-1, cmd);
        execStmt.parse(*this);

        emit this->requestInput();
        waitUntilInputIsFinished(); // 等待用户输入
        std::cout << "program input: " << input << std::endl;

        execStmt.exec(*this);
        return;
    }
    else {
        throw ParseException(ParseErrorType::InvalidCommandError, "invalid command", -1);
    }
}


void Program::exec(){
    bool iteratorUpdated = false;

    for (auto it = this->statements.begin(); it != statements.end(); /** We'll increment it manually */) {
        currentLine = it->first; // Get the line number of the current statement.
        int saveLine = currentLine; // Save the current line number to check for changes later.
        Statement* stmt = it->second; // Get the statement object.

        if (stmt) { // Ensure the pointer to the statement is not null.
            stmt->parse(*this); // Parse the statement.
            if (stmt->type == statementType::INPUT) {
//                std::lock_guard<std::mutex> lock(this->inputMutex); // Locks the mutex
                emit this->requestInput();
//                while (!isInputFinished){}
                 waitUntilInputIsFinished(); // 等待用户输入
                std::cout << "program input: " << input << std::endl;
            }
            stmt->exec(*this); // Execute the statement.
            std::cout << "After line " << currentLine << ":\n";
            // 遍历并输出变量名和使用次数
            for (const auto& pair : variables) {
                std::cout << "Variable Name: " << pair.first
                          << ", Usage Count: " << pair.second.usageCount << std::endl;
            }
            if (stmt->type == statementType::END) break; // Stop if the statement type is END.
            // If the statement is an IF or GOTO, check if the line number has changed.
            if (stmt->type == statementType::IF || stmt->type == statementType::GOTO) {
                // If the current line number has changed, update the iterator to the new line.
                if (currentLine != saveLine) {
                    it = statements.find(currentLine);
                    iteratorUpdated = true; // Mark that the iterator has been updated.
                    std::cout << "skip to line " << currentLine << std::endl;
                }
            }
        }

        // Only increment the iterator if it has not been updated.
        if (!iteratorUpdated) {
            ++it;
        } else {
            // Reset the flag since we have already moved to the correct line.
            iteratorUpdated = false;
        }
    }
}

std::string Program::getOutput() const{
    return this->output;
}

std::string Program::getSyntaxTree() {
    std::string syntaxTree;
    for (auto it = this->statements.begin(); it != statements.end(); ++it) {
        Statement* stmt = it->second;
        stmt->parse(*this);
        syntaxTree += stmt->syntaxTree();
    }
    return syntaxTree;
}

std::string Program::getSyntaxTreeWithRunStatistics(){
    std::string syntaxTree;
    for (auto it = this->statements.begin(); it != statements.end(); ++it) {
        Statement* stmt = it->second;
        std::cout << "line " << it->first << std::endl;
        std::cout << "RunTime" << stmt->getRunTime() << std::endl;
//        if (stmt->getRunTime() == 0) {
//            std::cout << "Parse" << std::endl;
            stmt->parse(*this);
//        }
        syntaxTree += stmt->syntaxTreeWithRunStatistics();
    }
    return syntaxTree;
}

void Program::setInput(std::string input){
    this->input = input;
}

void Program::preRun(){
    variables.clear();
    this->input.clear();
    this->output.clear();
    this->currentLine = -1;
    for (auto it = this->statements.begin(); it != statements.end(); ++it) {
        it->second->setRunStatistics(0);
    }
}


void Program::updateStatement(int lineNumber, std::string statement){
    auto it = this->statements.find(lineNumber);
    if (it != this->statements.end()){
        delete it->second;
        statements.erase(it);
    }
    saveLine(lineNumber, trimBothEnds(statement));
}

void Program::deleteStatement(int lineNumber){
    auto it = this->statements.find(lineNumber);
    if (it != this->statements.end()){
        delete it->second;
        statements.erase(it);
    }
    else {
        throw ParseException(ParseErrorType::UndefinedLineError, "delete invalid line", lineNumber);
    }
}

void Program::edit(std::string cmd){
    size_t space_pos = cmd.find(' ');
    int lineNumber = std::stoi(cmd.substr(0, space_pos));
    if (hasContentAfterFirstNumber(cmd)){
        std::string stmt = cmd.substr(space_pos + 1);
        updateStatement(lineNumber, stmt);
    }
    else {
        deleteStatement(lineNumber);
    }
}

