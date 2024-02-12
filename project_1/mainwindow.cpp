#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //>>>>>>>>>>>>>>>>>协议解析>>>>>>>>>>>>>>>>>
    qRegisterMetaType<Sensor_Data>("Sensor_Data");
    memset(alalysis_value, 0, sizeof(alalysis_value));


    //<<<<<<<<<<<<<<<<<<协议解析<<<<<<<<<<<<<<<<<<

    //>>>>>>>>>>>>>>>>>串口>>>>>>>>>>>>>>>>>
    serialPort = new QSerialPort(this);
    // 扫描本机的串口，并且添加到下拉框里
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBox->addItem(info.portName());
    }

    // 读取数据
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    //<<<<<<<<<<<<<<<<<<串口<<<<<<<<<<<<<<<<<<


    //>>>>>>>>>>>>>>>>>可视化>>>>>>>>>>>>>>>>>
    ui->sensorPlotWidget->setOpenGl(true);
    qDebug()<<"opengl:"<<ui->sensorPlotWidget->openGl();
    //曲线显示区域参数设置
    ui->sensorPlotWidget->setInteractions(QCP::iRangeDrag |QCP::iRangeZoom | QCP::iSelectAxes |
                                          QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->sensorPlotWidget->legend->setSelectableParts(QCPLegend::spItems);
    ui->sensorPlotWidget->addGraph();
    ui->sensorPlotWidget->graph(0)->setPen(QPen(Qt:: red));
    ui->sensorPlotWidget->yAxis->setLabel(QString(tr("value")));
    ui->sensorPlotWidget->xAxis->setLabel(QString(tr("time(m/s)")));
    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime); //时间日期作为X轴
    timeTicker->setDateTimeFormat("mm:ss");
    ui->sensorPlotWidget->xAxis->setTicker(timeTicker);
    connect(ui->sensorPlotWidget->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->sensorPlotWidget->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->sensorPlotWidget->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->sensorPlotWidget->yAxis2, SLOT(setRange(QCPRange)));
    //<<<<<<<<<<<<<<<<<<可视化<<<<<<<<<<<<<<<<<<
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)//边栏切换
{
    ui->tabWidget->setCurrentIndex(currentRow);
}

void MainWindow::readData()//数据接收
{
    //16进制接收
    QByteArray receivedData = serialPort->readAll();
    QString hexString = QString(receivedData.toHex());
    ui->textBrowser->insertPlainText(hexString);

    //数据解包
    if(!receivedData.isEmpty())
    {
        frame_Unpack(receivedData);
    }

    // 滚动到底部
    ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
}

void MainWindow::on_pushButton_clicked(bool checked)//打开串口
{
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

void MainWindow::on_pushButton_2_clicked()//串口发送
{
    //16进制发送
    QString textToSend = ui->textEdit->toPlainText();
    QByteArray byteArray;
    for(int i = 0; i < textToSend.length(); i += 2) {
        QString byteString = textToSend.mid(i, 2);
        bool ok;
        char byte = byteString.toUShort(&ok, 16);
        if(ok) {
            byteArray.append(byte);
        }
    }
    serialPort->write(byteArray);
}

void MainWindow::on_pushButton_3_clicked()//清除发送窗口
{
    // 清空发送的数据
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_4_clicked(bool checked)// 绘图
{
    //开始的时间戳
    static double initialTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    //暂停绘图的时间戳
    static double pausedTime = 0.0;
    static double currentSeconds = 0.0;

    if (checked){
        // 动态绘制
        timer = new QTimer(this);
        if(pausedTime != 0.0) {
            initialTime += (QDateTime::currentDateTime().toSecsSinceEpoch() - pausedTime);
            pausedTime = 0.0;
        }
        connect(timer, &QTimer::timeout, this, [=]() {
            // 获取当前时间（秒）
            double currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();
            currentSeconds = currentTime - initialTime;
            // 更新数据
            this->Time.push_back(currentSeconds);
            this->Data.push_back(frame_to_Plot(DataParse));
            this->basic_Plot(Time, Data);
        });
        timer->start(500);

        ui->pushButton_4->setText("Pause");
    }
    else{
        if(timer) {
            timer->stop();
            delete timer;
            timer = nullptr;
            pausedTime = QDateTime::currentDateTime().toSecsSinceEpoch();
        }
        ui->sensorPlotWidget->replot();
        ui->pushButton_4->setText("Start");
    }
}

void MainWindow::on_pushButton_5_clicked()//文件数据保存
{
    QString defaultPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));;
    // 创建一个文件选择器
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Data and Plot"), defaultPath, tr("PNG Files (*.png);;CSV Files (*.csv)"));
    if (fileName.isEmpty())
        return;

    // 获取文件扩展名
    QFileInfo fileInfo(fileName);
    QString extension = fileInfo.suffix();

    // 如果是CSV文件，保存数据
    if (extension == "csv") {
        saveCSV(fileName);
    }

    // 如果是PNG文件，保存绘图
    if (extension == "png") {
        savePlot(fileName);
    }
}

void MainWindow::basic_Plot(QVector<double> X_Time, QVector<double> Sensor_Value)//底层绘图接口
{
    ui->sensorPlotWidget->graph(0)->setData(X_Time, Sensor_Value);
    ui->sensorPlotWidget->graph(0)->rescaleAxes();
    ui->sensorPlotWidget->replot(QCustomPlot::rpQueuedReplot);
}

void MainWindow::saveCSV(const QString &fileName)//csv文件保存
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    QTextStream out(&file);
    // 写入表头
    out << "X Data, Y Data" << endl;
    // 写入数据
    for (int i = 0; i < Time.size(); ++i) {
        out << QString::number(Time[i]) << "," << QString::number(Data[i]) << endl;
    }
    file.close();
    QMessageBox::information(this, tr("Success"), tr("Data saved successfully!"));
}

void MainWindow::savePlot(const QString &fileName)//图像保存
{
    // 获取图像
    QPixmap pixmap = ui->sensorPlotWidget->toPixmap();

    // 保存图像
    if (!pixmap.save(fileName)) {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot save plot to file %1.").arg(fileName));
        return;
    }

    QMessageBox::information(this, tr("Success"), tr("Plot saved successfully!"));
}

double MainWindow::frame_to_Plot(Sensor_Data frame_info)
{
    double Value = frame_info.HighDataBit<<8|frame_info.LowDataBit;
    return Value;

}

//帧最长为256B,(格式:[0xA5] [lenth] (data)*N [0x5A]),data最大253B(当前lenth最大为255,数据若超过256B,则需要拓展lenth字节数)
void MainWindow::frame_Unpack(QByteArray frame_data)
{
    for (int i = 0; i < MAX_LEN; i++)
    {
        if(frame_data[i] == char(0xA5))
        {
            if(frame_data[i + frame_data[i+1]] == char(0x5A))//frame_data[i+1]固定为lenth，值=从0计数的帧长度(如0xA5 lenth data2 0x5A,则lenth=3)
            {
                if(i + frame_data[i+1] > MAX_LEN-1)//帧尾访问越界则视为异常数据
                {
                    qDebug()<<"数据异常";
                    return;
                }
                memset(&DataParse, 0, sizeof(Sensor_Data));
                memcpy(&DataParse, frame_data, sizeof(Sensor_Data));
                break;
            }
        }
    }
}













