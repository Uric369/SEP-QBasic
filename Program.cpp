#include "Program.h"
#include "Statement.h"

Program::Program(){
    this->currentLine = -1;
    this->maxLine = -1;
    this->ifTrue = false;
    this->hasEND = false;
}

void Program::reset(){
    this->variables.clear();
    this->statements.clear();
    this->raw.clear();
    this->input.clear();
    this->output.clear();
    this->currentLine = -1;
    this->maxLine = -1;
    this->ifTrue = false;
    this->hasEND = false;
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


void Program::LoadContent(const std::string &content) {
    std::istringstream file(content);
    std::string line;
    int maxLine = -1; // Initialize with an invalid line number

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
            throw ParseException(ParseErrorType::InvalidLineNumberError, "invalid line number after", maxLine);
        }

        // Check if the current line number is greater than the max line number
        if (lineNumber <= maxLine) {
            throw ParseException(ParseErrorType::InvalidLineNumberError, "decreasing line number", lineNumber);
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

    if (!hasEND) {
        throw ParseException(ParseErrorType::MissingEndError, "missing END", -1);
    }
}


std::string Program::display() const{
    std::string result;
    for (const auto& line : raw) {
        std::cout << line << '\n';
        result += line + '\n';
    }
    return result;
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
    bool iteratorUpdated = false;

    for (auto it = this->statements.begin(); it != statements.end(); /** We'll increment it manually */) {
        currentLine = it->first; // Get the line number of the current statement.
        int saveLine = currentLine; // Save the current line number to check for changes later.
        Statement* stmt = it->second; // Get the statement object.

        if (stmt) { // Ensure the pointer to the statement is not null.
            if (stmt->type == statementType::END) break; // Stop if the statement type is END.

            stmt->parse(*this); // Parse the statement.
            stmt->exec(*this); // Execute the statement.

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

std::string Program::getSyntaxTree() const{
    std::string syntaxTree;
    for (auto it = this->statements.begin(); it != statements.end(); ++it) {
        Statement* stmt = it->second;
        syntaxTree += stmt->syntaxTree();
    }
    return syntaxTree;
}

void Program::setInput(std::string input){
    this->input = input;
}

