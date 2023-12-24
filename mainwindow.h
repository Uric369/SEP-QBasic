#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include "Program.h"
#include "Exception.h"
#include <QMessageBox>
#include "Typedef.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void requestInput();
    void onInputReceived();
private slots:
    void on_cmdLineEdit_editingFinished();
private:
    Program program;
    void Load();
    void Run();
    void Clear();
    void Help();
    void onLineEditReturnPressed();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
