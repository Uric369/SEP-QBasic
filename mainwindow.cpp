#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::Load);
    connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::Run);
    connect(ui->btnClearCode, &QPushButton::clicked, this, &MainWindow::Clear);

    // 连接QLineEdit的returnPressed信号到自定义槽函数
    connect(ui->cmdLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onLineEditReturnPressed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Run(){
    try {
    std::cout << "Run" << std::endl;
    program.preRun();
    program.exec();
    std::cout << program.getOutput() << std::endl;
    ui->textBrowser->setPlainText(QString::fromStdString(program.getOutput()));
    ui->treeDisplay->setPlainText(QString::fromStdString(program.getSyntaxTreeWithRunStatistics()));
//    ui->treeDisplay->setPlainText(QString::fromStdString(program.getSyntaxTree()));
    }
    catch(ParseException &e) {
        ui->textBrowser->setPlainText(e.what());
    } catch (const std::invalid_argument& ia) {
        // 如果输入的不是整数，将会捕获到invalid_argument异常
        std::cerr << "Error: Invalid input, not an integer." << std::endl;
        ui->textBrowser->setPlainText(ia.what());
    } catch (const std::out_of_range& oor) {
        // 如果整数超出了int的范围，将会捕获到out_of_range异常
        std::cerr << "Error: Integer out of range." << std::endl;
        ui->textBrowser->setPlainText(oor.what());
}
}

void MainWindow::Clear(){
    std::cout << "Clear" << std::endl;
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    ui->textBrowser->clear();
    program.reset();
}

// 添加一个新的槽函数用来处理returnPressed信号
void MainWindow::onLineEditReturnPressed() {
    try {
    QString text = ui->cmdLineEdit->text(); // 获取QLineEdit的文本
    std::string stdText = text.toStdString(); // Convert QString to std::string
    std::cout << "Input: " << stdText << std::endl;
    size_t first = stdText.find_first_not_of(' ');
    if (std::string::npos == first) {
        return;
    }
    size_t last = stdText.find_last_not_of(' ');
    stdText = stdText.substr(first, (last - first + 1));
    std::istringstream iss(stdText);
    std::string firstWord;
    iss >> firstWord;
    if (firstWord == "RUN"){
        this->Run();
        return;
    }
    if (firstWord == "LOAD"){
        this->Load();
        return;
    }
    if (firstWord == "CLEAR"){
        this->Clear();
        return;
    }
    if (firstWord == "HELP"){
// TODO
        return;
    }
    if (firstWord == "QUIT"){
        this->close();
        return;
    }
//    program.cmd(stdText);
        if (std::isdigit(stdText[0])) {
            // 首个字符是数字，找到第一个空格并提取前面的数字
            program.edit(stdText);
            ui->CodeDisplay->setPlainText(QString::fromStdString(program.display()));
        } else if (std::isalpha(stdText[0])) {
            // 首个字符是字母，找到第一个空格并提取前面的字符串
//            program.preRun();
//            program.exec();
            program.execLine(stdText);
            ui->textBrowser->setPlainText(QString::fromStdString(program.getOutput()));
            ui->treeDisplay->setPlainText(QString::fromStdString(program.getSyntaxTreeWithRunStatistics()));
        } else {
            // 首个字符既不是数字也不是字母
            throw ParseException(ParseErrorType::InvalidExpressionError, "invalid command", -1);
        }
        ui->cmdLineEdit->clear();
    }
    catch(ParseException &e) {
        ui->textBrowser->setPlainText(e.what());
    } catch (const std::invalid_argument& ia) {
        // 如果输入的不是整数，将会捕获到invalid_argument异常
        std::cerr << "Error: Invalid input, not an integer." << std::endl;
        ui->textBrowser->setPlainText(ia.what());
    } catch (const std::out_of_range& oor) {
        // 如果整数超出了int的范围，将会捕获到out_of_range异常
        std::cerr << "Error: Integer out of range." << std::endl;
        ui->textBrowser->setPlainText(oor.what());
    }
    }

void MainWindow::on_cmdLineEdit_editingFinished(){

}

void MainWindow::Load()
{
    try {
    std::cout << "Load" << std::endl;
    // Open file selection dialog and get the selected file path
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Code File"), "", tr("Code Files (*.txt *.h *.cpp);;All Files (*)"));

    // Check if a file was selected (filePath is not empty)
    if (!filePath.isEmpty())
    {
        // Convert the QString filePath to std::string
        std::string path = filePath.toStdString();

        // Create an input file stream
        std::ifstream file(path);

        // Check if the file is successfully opened
        if (file)
        {
            // Read the file content and pass it to program
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            program.reset();
            program.LoadContent(content); // Assuming program.Load() can take file content as a string

            // Set the content in the UI's CodeDisplay
            ui->CodeDisplay->setPlainText(QString::fromStdString(content));
        }
        else{
            QMessageBox::warning(this, tr("Error"), tr("Could not open the file."));
        }
    }
    else{
        QMessageBox::information(this, tr("Information"), tr("No file was selected."));
    }
    }
    catch(ParseException &e) {
        ui->textBrowser->setPlainText(e.what());
    } catch (const std::invalid_argument& ia) {
        // 如果输入的不是整数，将会捕获到invalid_argument异常
        std::cerr << "Error: Invalid input, not an integer." << std::endl;
        ui->textBrowser->setPlainText(ia.what());
    } catch (const std::out_of_range& oor) {
        // 如果整数超出了int的范围，将会捕获到out_of_range异常
        std::cerr << "Error: Integer out of range." << std::endl;
        ui->textBrowser->setPlainText(oor.what());
    }
}



