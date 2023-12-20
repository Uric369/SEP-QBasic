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
    std::cout << "Run" << std::endl;
    program.exec();
    std::cout << program.getOutput() << std::endl;
    ui->textBrowser->setPlainText(QString::fromStdString(program.getOutput()));
    ui->treeDisplay->setPlainText(QString::fromStdString(program.getSyntaxTree()));
}

void MainWindow::Clear(){
    std::cout << "Clear" << std::endl;
}

// 添加一个新的槽函数用来处理returnPressed信号
void MainWindow::onLineEditReturnPressed() {
    QString text = ui->cmdLineEdit->text(); // 获取QLineEdit的文本
    std::string stdText = text.toStdString(); // Convert QString to std::string
    std::cout << "Input: " << stdText << std::endl;
}

void MainWindow::on_cmdLineEdit_editingFinished(){

}

void MainWindow::Load()
{
    program.reset();
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



