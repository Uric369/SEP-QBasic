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
    std::cout << "Load" << std::endl;
    // 打开文件选择对话框并获取选定的文件路径
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Code File"), "", tr("Code Files (*.txt *.h *.cpp);;All Files (*)"));

    program.Load(filePath.toStdString());
}
