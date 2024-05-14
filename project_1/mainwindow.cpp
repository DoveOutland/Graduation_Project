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

    // 创建 Speed 控件添加布局到 tab_3
    Speed *speedWidget = new Speed(ui->tab_3);
    if (ui->tab_3->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(ui->tab_3);
        ui->tab_3->setLayout(layout);
    }
    ui->tab_3->layout()->addWidget(speedWidget);

    // 创建并添加 Temperature 控件到 tab_4
    Temperature *temperatureWidget = new Temperature(ui->tab_4);
    if (ui->tab_4->layout() == nullptr) {
        QVBoxLayout *layout = new QVBoxLayout(ui->tab_4);
        ui->tab_4->setLayout(layout);
    }
    ui->tab_4->layout()->addWidget(temperatureWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)//边栏切换
{
    ui->tabWidget->setCurrentIndex(currentRow);
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















