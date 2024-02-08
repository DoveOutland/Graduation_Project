#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serialPort = new QSerialPort(this);
    // 扫描本机的串口，并且添加到下拉框里
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBox->addItem(info.portName());
    }

    // 读取数据
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
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

void MainWindow::readData()
{
    // 接收数据
    ui->textBrowser->insertPlainText(serialPort->readAll());
}

void MainWindow::on_pushButton_clicked(bool checked)
{//打开串口

    if (checked) {
        // 设置要打开的串口的名字
        serialPort->setPortName(ui->comboBox->currentText());

        // 设置波特率
        serialPort->setBaudRate(ui->comboBox_2->currentText().toInt());

        // 设置停止位
        serialPort->setStopBits(QSerialPort::StopBits(ui->comboBox_3->currentText().toInt()));

        // 设置数据位
        serialPort->setDataBits(QSerialPort::DataBits(ui->comboBox_4->currentText().toInt()));

        // 设置校验位
        switch (ui->comboBox_5->currentIndex()) {
        case 0:
            serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serialPort->setParity(QSerialPort::EvenParity);
            break;
        case 2:
            serialPort->setParity(QSerialPort::OddParity);
            break;
        case 3:
            serialPort->setParity(QSerialPort::SpaceParity);
            break;
        case 4:
            serialPort->setParity(QSerialPort::MarkParity);
            break;
        default:
            break;
        }

        // 设置流控为无
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (!serialPort->open(QIODevice::ReadWrite)) {
            QMessageBox::about(this, "错误", "串口打开失败");
            return;
        }

        //联通后 设置不可用
        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->setEnabled(false);

        ui->pushButton->setText("关闭串口");
    }
    else{
        // 关闭串口
        serialPort->close();
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->comboBox_3->setEnabled(true);
        ui->comboBox_4->setEnabled(true);
        ui->comboBox_5->setEnabled(true);
        ui->pushButton->setText("打开串口");
    }
}



void MainWindow::on_pushButton_2_clicked()
{
    // 发送数据
    serialPort->write(ui->textEdit->toPlainText().toUtf8());
}

void MainWindow::on_pushButton_3_clicked()
{
    // 清空发送的数据
    ui->textEdit->clear();
}
