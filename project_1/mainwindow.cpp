#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setLayout(ui->horizontalLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    //siwtch tab callback function
    ui->tabWidget->setCurrentIndex(currentRow);
}
