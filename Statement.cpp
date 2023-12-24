#include "Statement.h"
#include "Program.h"
std::string trimLeadingWhitespace(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\v\f\r");
    return (start == std::string::npos) ? "" : str.substr(start);
}

// 去掉字符串首尾的空格
std::string trimBothEnds(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to split a string by a delimiter and return a vector of strings
std::vector<std::string> split(const std::string &str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delim)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}




Statement::Statement(){}

Statement::~Statement(){}

std::string Statement::syntaxTree() const {
    return nullptr;
}

std::string Statement::syntaxTreeWithRunStatistics() const {
    return nullptr;
}

statementType Statement::getType() const{
    return this->type;
}

std::string Statement::getRaw() const{
    return std::to_string(this->lineNumber) + " " + this->statement + "\n";
}

Arithstatement::Arithstatement(){}

Arithstatement::Arithstatement(int lineNumber, std::string statement): Statement(){
    // Trim leading whitespace
    statement = trimLeadingWhitespace(statement);
    this->type = statementType::Arith;
    this->statement = statement;
    this->lineNumber = lineNumber;
    this->expressionEvaluator = nullptr;
}

Arithstatement::~Arithstatement(){}

void Arithstatement::setRunStatistics(int n){
    this->runTime = n;
}

void Arithstatement::parse(Program &program) {
    this->expressionEvaluator = new ExpressionEvaluator(statement, &program, lineNumber);
}

void Arithstatement::exec(Program &program){}

int Arithstatement::getValue(){
    return this->expressionEvaluator->getValue();
}

std::string Arithstatement::syntaxTree() const{
    return this->expressionEvaluator->syntaxTree();
}

std::string Arithstatement::mergeTrees(const Arithstatement &B) const{
    return this->expressionEvaluator->mergeTrees(*(B.expressionEvaluator));
}

std::string Arithstatement::syntaxTreeWithOffset(int offset) const{
    std::cout << "Arith syntaxTree" << this->expressionEvaluator->syntaxTreeWithOffset(offset);
    return this->expressionEvaluator->syntaxTreeWithOffset(offset);
}

std::string Arithstatement::syntaxTreeWithRunStatistics() const{
    return this->expressionEvaluator->syntaxTreeWithRunStatistics(0);
}

std::string Arithstatement::syntaxTreeWithRunStatistics(int offset) const{
    return this->expressionEvaluator->syntaxTreeWithRunStatistics(offset);
}

REMstatement::REMstatement(int lineNumber, std::string statement) : Statement() {    // Trim leading whitespace
    this->type = statementType::REM;
    this->statement = trimLeadingWhitespace(statement);
    this->lineNumber = lineNumber;
    this->runTime = 0;
}

REMstatement::~REMstatement(){}

void REMstatement::setRunStatistics(int n){
    this->runTime = n;
}

void REMstatement::parse(Program &program) {
    if (statement.size() >= 3 && statement.substr(0, 3) == "REM") {
        remark = trimLeadingWhitespace(statement.substr(3));
    }
    else throw ParseException(ParseErrorType::InvalidExpressionError, "invalid expression", lineNumber);
}

void REMstatement::exec(Program &program) {
    this->runTime ++;
    return;
}

std::string REMstatement::syntaxTree() const{
    if (remark.length() > 0) return std::to_string(lineNumber) + " REM" + "\n" + retract + remark + "\n";
    else return std::to_string(lineNumber) + " REM" + "\n";
}

std::string REMstatement::syntaxTreeWithRunStatistics() const{
    if (remark.length() > 0) return std::to_string(lineNumber) + " REM " + std::to_string(this->runTime) + "\n" + retract + remark + "\n";
    else return std::to_string(lineNumber) + " REM" + "\n";
}

LETstatement::LETstatement(int lineNumber, std::string statement): Statement(){
    this->type = statementType::LET;
    this->lineNumber = lineNumber;
    this->statement = trimLeadingWhitespace(statement);
    this->runTime = 0;
}

LETstatement::~LETstatement(){}

void LETstatement::setRunStatistics(int n){
    this->runTime = n;
}

void LETstatement::parse(Program &program){
    std::string save;
    if (statement.size() >= 4 && statement.substr(0, 4) == "LET ") {
         save = trimBothEnds(statement.substr(4));
    }
    else {
        throw ParseException(ParseErrorType::InvalidExpressionError, "invalid expression", lineNumber);
    }

    // 根据等号分割字符串
    size_t pos = save.find('=');
    if (pos != std::string::npos) {
        // 分割出左边的字符串并去除首尾空格
        this->LHS = trimBothEnds(save.substr(0, pos));
        if (this->LHS.length()<= 0) throw ParseException(ParseErrorType::MissingOperandError, "Missing operand on the left side of =", lineNumber);
        this->LHS_sta = Arithstatement(this->lineNumber, this->LHS);
        this->LHS_sta.parse(program);

        auto it = program.variables.find(this->LHS);
        if (it == program.variables.end()) {
            VariableInfo varInfo = {0, 0};

            // 插入新的变量到 map 中
            program.variables[this->LHS] = varInfo;
        }

        // 分割出右边的字符串并去除首尾空格
        std::string right = trimBothEnds(save.substr(pos + 1));
        if (right.length()<= 0) throw ParseException(ParseErrorType::MissingOperandError, "Missing operand on the right side of =", lineNumber);

        this->RHS = Arithstatement(this->lineNumber, right);
        this->RHS.parse(program);
    } else {
        throw ParseException(ParseErrorType::MissingOperandError, "LET command missing '=' ", lineNumber);
    }
}

void LETstatement::exec(Program &program){
    auto it = program.variables.find(LHS);
    if (it != program.variables.end()){
        it->second.value = this->RHS.getValue();
        std::cout << "LET: " << this->RHS.getValue() << std::endl;
//        it->second.usageCount++;
        this->runTime ++;
    }
    else {
        VariableInfo varInfo = {this->RHS.getValue(), 0};

        // 插入新的变量到 map 中
        program.variables[this->LHS] = varInfo;
    }
}


std::string LETstatement::syntaxTree() const{
    std::string tree = std::to_string(lineNumber) + " " + "LET =\n" + retract;
    tree += this->LHS + "\n";
    tree += this->RHS.syntaxTreeWithOffset(1);
    return tree;
}

std::string LETstatement::syntaxTreeWithRunStatistics() const{
    std::string tree = std::to_string(lineNumber) + " " + "LET = " + std::to_string(this->runTime) + "\n" + retract;
    tree += this->LHS_sta.syntaxTreeWithRunStatistics();
    tree += this->RHS.syntaxTreeWithOffset(1);
    return tree;
}

IFstatement::IFstatement(int lineNumber, std::string statement): Statement(){
    statement = trimLeadingWhitespace(statement);
    this->type = statementType::IF;
    this->statement = statement;
    this->lineNumber = lineNumber;
    this->trueTime = 0;
    this->falseTime = 0;
}

IFstatement::~IFstatement(){}

void IFstatement::setRunStatistics(int n){
    this->trueTime = n;
    this->falseTime = n;
}

std::vector<std::string> IFstatement::splitByNewline(const std::string& str) const {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line)) {
        result.push_back(line);
    }
    return result;
}

std::string IFstatement::mergeTrees(const std::string& A, const std::string& B) const {
    // Split the strings by new lines to get a vector of layers
    std::vector<std::string> layersA = splitByNewline(A);
    std::vector<std::string> layersB = splitByNewline(B);
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

// Function to process the level order string
std::string IFstatement::printLevelOrder(const std::string &levelOrder) const {
    std::string syntaxTree = "";
    std::string current_retract = "";
//    std::string levelOrder = this->LHS.mergeTrees(this->RHS);

    std::istringstream iss(levelOrder);
    bool firstLine = true; // To identify the first line

    // Process each line
    for (std::string line; std::getline(iss, line);) {
        std::vector<std::string> elements = split(line, ' ');

        // Process each element in the line
        for (size_t i = 0; i < elements.size(); ++i) {
            if (firstLine && i == 1) { // Before inserting the second element of the first line
                syntaxTree += retract + this->ifOperator + "\n"; // Insert the symbol "<" with spaces before the second element
            }

            syntaxTree += current_retract + retract + elements[i];

            if (i < elements.size() - 1) { // Only add new lines if not the last element
                syntaxTree += "\n";
            }
        }

        // Add toLine at the end of the first line
        if (firstLine) {
            syntaxTree += "\n" + retract + std::to_string(100);
            firstLine = false; // No longer the first line
        }

        // Increase the retract for the next line
        current_retract += retract;

        if (!firstLine) { // If not the first line, add a newline for the next line
            syntaxTree += "\n";
        }
    }
    return syntaxTree;
}


void IFstatement::parse(Program &program){
    std::string save;
    if (statement.size() >= 3 && statement.substr(0, 3) == "IF ") {
         save = trimBothEnds(statement.substr(3));
    }
    else {
        throw ParseException(ParseErrorType::InvalidExpressionError, "invalid expression", lineNumber);
    }

    // 根据等号分割字符串
    size_t pos = save.find(" THEN");
    if (pos != std::string::npos) {
        // 分割出左边的字符串并去除首尾空格
        std::string expression= trimBothEnds(save.substr(0, pos));
        if (expression.length()<= 0) throw ParseException(ParseErrorType::MissingOperandError, "Missing expression after IF", lineNumber);

        std::size_t operatorPos = expression.find_first_of("<>=");
        if (operatorPos == std::string::npos) {
            throw ParseException(ParseErrorType::SyntaxError, "No valid operator found in the IF commmand.", lineNumber);
        }

        std::size_t nextOperatorPos = expression.find_first_of("<>=", operatorPos + 1);
        if (nextOperatorPos != std::string::npos) {
            throw ParseException(ParseErrorType::SyntaxError, "Multiple operators found in the string.", lineNumber);
        }

//        switch (expression[operatorPos]) {
//            case '=':
//                this->ifOperator = '=';
//                break;
//            case '>':
//                this->ifOperator = Greater;
//                break;
//            case '<':
//                this->ifOperator = Less;
//                break;
//            default:
//                // This should not happen since we checked for the operator before
//                throw std::logic_error("Invalid operator found.");
//        }
        this->ifOperator = (char) expression[operatorPos];

        this->LHS = Arithstatement(this->lineNumber, expression.substr(0, operatorPos));
        this->LHS.parse(program);
        this->RHS = Arithstatement(this->lineNumber, expression.substr(operatorPos + 1));
        this->RHS.parse(program);

        // 分割出右边的字符串并去除首尾空格
        std::string target = trimBothEnds(save.substr(pos + 5));
        if (target.length()<= 0) throw ParseException(ParseErrorType::MissingOperandError, "Missing line number after THEN", lineNumber);
        this->toLine = std::stoi(target);
        // 判断line是否存在
        if (program.statements.find(toLine) == program.statements.end()){
            throw ParseException(ParseErrorType::UndefinedLineError, "undefined line number", lineNumber);
        }
    } else {
        throw ParseException(ParseErrorType::MissingOperandError, "IF command missing THEN ", lineNumber);
    }
}

void IFstatement::exec(Program &program){
    std::cout << this->LHS.getValue() << "  " << this->RHS.getValue();
    std::cout << this->ifOperator;
    switch (this->ifOperator) {
        case '=':
        if (this->LHS.getValue() == this->RHS.getValue()) {
            std::cout << "IF: true" << std::endl;
            this->trueTime++;
            program.currentLine = toLine;
        }
        else this->falseTime++;
        break;
        case '>':
        if (this->LHS.getValue() > this->RHS.getValue()) {
            std::cout << "IF: true" << std::endl;
            this->trueTime++;
            program.currentLine = toLine;
        }
        else this->falseTime++;
        break;
        case '<':
        if (this->LHS.getValue() < this->RHS.getValue()) {
            std::cout << "IF: true" << std::endl;
            this->trueTime++;
            program.currentLine = toLine;
        }
        else this->falseTime++;
        break;
        default:
        std::cout << "IF: false" << std::endl;
        this->falseTime++;
    }
}

std::string IFstatement::syntaxTree() const{
    std::string syntaxTree = "IF THEN\n";
    std::string levelOrder = this->LHS.mergeTrees(this->RHS);
    syntaxTree += printLevelOrder(levelOrder);
    return syntaxTree;
}

std::string IFstatement::syntaxTreeWithRunStatistics() const{
    std::string syntaxTree = "IF THEN " + std::to_string(this->trueTime) + " " + std::to_string(this->falseTime) + "\n";
    std::string levelOrder = this->LHS.mergeTrees(this->RHS);
    syntaxTree += printLevelOrder(levelOrder);
    return syntaxTree;
}


PRINTstatement::PRINTstatement(int lineNumber, std::string statement): Statement(){
    statement = trimLeadingWhitespace(statement);
    this->type = statementType::PRINT;
    this->statement = statement;
    this->lineNumber = lineNumber;
    this->runTime = 0;
}

PRINTstatement::~PRINTstatement(){}

void PRINTstatement::setRunStatistics(int n){
    this->runTime = n;
}

void PRINTstatement::parse(Program &program){
    if (statement.size() >= 6 && statement.substr(0, 6) == "PRINT ") {
        this->print = Arithstatement(this->lineNumber, trimBothEnds(statement.substr(6)));
        this->print.parse(program);
    }
    else {
        throw ParseException(ParseErrorType::InvalidExpressionError, "invalid expression", lineNumber);
    }
}

void PRINTstatement::exec(Program &program){
    this->runTime++;
    program.output += std::to_string(this->print.getValue()) + '\n';
}

std::string PRINTstatement::syntaxTree() const{
//     Todo:应该需要输出
    return std::to_string(lineNumber) + " " + "PRINT\n" + this->print.syntaxTreeWithOffset(1);
//    return nullptr;
}

std::string PRINTstatement::syntaxTreeWithRunStatistics() const{
    return std::to_string(lineNumber) + " PRINT " + std::to_string(runTime) + "\n" + this->print.syntaxTreeWithOffset(1);
}

INPUTstatement::INPUTstatement(int lineNumber, std::string statement): Statement(){
    statement = trimLeadingWhitespace(statement);
    this->type = statementType::INPUT;
    this->statement = statement;
    this->lineNumber = lineNumber;
    this->runTime = 0;
}

INPUTstatement::~INPUTstatement(){}

void INPUTstatement::setRunStatistics(int n){
    this->runTime = n;
}

void INPUTstatement::parse(Program &program){
    if (statement.size() >= 6 && statement.substr(0, 6) == "INPUT ") {
         this->input = trimBothEnds(statement.substr(6));
    }
    else {
        throw ParseException(ParseErrorType::InvalidExpressionError, "invalid expression", lineNumber);
    }

    auto it = program.variables.find(this->input);
    if (it == program.variables.end()) {
        // 如果没找到，插入
        VariableInfo varInfo{0 ,0};
        program.variables[this->input] = varInfo;
    }
}

void INPUTstatement::exec(Program &program){
    int value = std::stoi(trimBothEnds(program.input));
    // Todo :invalid输入 非int 抛typeerror
    auto it = program.variables.find(this->input);
    if (it != program.variables.end()){
        it->second.value = value;
        it->second.usageCount++;  // Todo：要加吗？
        this->runTime ++;
    }
    else {
        throw ParseException(ParseErrorType::UndefinedVariableError, "undefined variable: " + this->input, lineNumber);
    }
}

std::string INPUTstatement::syntaxTree() const{
    return std::to_string(lineNumber) + " " + "INPUT\n" + retract + this->input + "\n";
}

std::string INPUTstatement::syntaxTreeWithRunStatistics() const{
    return std::to_string(lineNumber) + " INPUT " + std::to_string(this->runTime) + "\n" + retract + this->input + "\n";
}

GOTOstatement::GOTOstatement(int lineNumber, std::string statement): Statement(){
    statement = trimLeadingWhitespace(statement);
    this->type = statementType::GOTO;
    this->statement = statement;
    this->lineNumber = lineNumber;
    this->runTime = 0;
}

GOTOstatement::~GOTOstatement(){}

void GOTOstatement::setRunStatistics(int n){
    this->runTime = n;
}

void GOTOstatement::parse(Program &program){
    if (statement.size() >= 5 && statement.substr(0, 5) == "GOTO ") {
        this->toLine = std::stoi(trimLeadingWhitespace(statement.substr(5)));
        // 判断line是否存在
        if (program.statements.find(toLine) == program.statements.end()){
            throw ParseException(ParseErrorType::UndefinedLineError, "GOTO line number does't exist", lineNumber);
        }
    }
    else {
        throw ParseException(ParseErrorType::InvalidExpressionError, "invalid expression", lineNumber);
    }
}

void GOTOstatement::exec(Program &program){
    program.currentLine = this->toLine;
    this->runTime++;
    return;
}

std::string GOTOstatement::syntaxTree() const {
    return std::to_string(lineNumber) + " GOTO\n" + retract + std::to_string(this->toLine) + "\n";
}

std::string GOTOstatement::syntaxTreeWithRunStatistics() const {
    return std::to_string(lineNumber) + " GOTO " + std::to_string(this->runTime) +"\n" + retract + std::to_string(this->toLine) + "\n";
}

ENDstatement::ENDstatement(int lineNumber, std::string statement): Statement(){
    this->type = statementType::END;
    this->statement = statement;
    this->lineNumber = lineNumber;
    this->runTime = 0;
}

ENDstatement::~ENDstatement(){}

void ENDstatement::setRunStatistics(int n){
    this->runTime = n;
}

void ENDstatement::parse(Program &program){

}

void ENDstatement::exec(Program &program){
    this->runTime++;
}

std::string ENDstatement::syntaxTree() const {
    return std::to_string(this->lineNumber) + " END";
}

std::string ENDstatement::syntaxTreeWithRunStatistics() const {
    return std::to_string(this->lineNumber) + " END " + std::to_string(this->runTime) + "\n";
}




