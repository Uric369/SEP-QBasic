#include "Program.h"
#include "Statement.h"

Program::Program(){
    this->currentLine = -1;
    this->maxLine = -1;
}

void Program::reset(){
    this->variables.clear();
    this->statements.clear();
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
            if (lineNumber <= maxLine) {
                throw ParseException(ParseErrorType::InvalidLineNumberError, "decreasing line number", lineNumber);
                return;
            }
            maxLine = lineNumber;

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

void Program::saveLine(int lineNumber, std::string cmd){
    std::istringstream iss(cmd);
    std::string firstWord;
    iss >> firstWord;
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

void Program::exec(){
    for (auto it = this->statements.begin(); it != statements.end(); ++it) {
        Statement* stmt = it->second;
        if (stmt) { // 确保指针不为空
            stmt->exec(*this);
        }
    }
}
