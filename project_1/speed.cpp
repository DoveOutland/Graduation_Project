#include "speed.h"

Speed::Speed(QWidget *parent) : QWidget(parent) {
    // 创建滑动条
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(0, 3000);
    speedSlider->setGeometry(10, 10, 380, 30);

    // 创建显示标签
    speedLabel = new QLabel("0 RPM", this);
    speedLabel->setGeometry(10, 50, 380, 30);
    speedLabel->setAlignment(Qt::AlignCenter);

    // 创建预设速度下拉菜单
    presetSpeeds = new QComboBox(this);
    presetSpeeds->setGeometry(10, 90, 380, 30);
    presetSpeeds->addItem("1000 RPM");
    presetSpeeds->addItem("2000 RPM");
    presetSpeeds->addItem("3000 RPM");

    // 创建按钮
    startButton = new QPushButton("Start", this);
    startButton->setGeometry(10, 130, 85, 30);

    stopButton = new QPushButton("Stop", this);
    stopButton->setGeometry(105, 130, 85, 30);

    emergencyButton = new QPushButton("Emergency Stop", this);
    emergencyButton->setGeometry(200, 130, 190, 30);

    // 连接信号
    connect(speedSlider, &QSlider::valueChanged, this, &Speed::updateSpeedLabel);
    connect(speedSlider, &QSlider::valueChanged, this, &Speed::onSliderValueChanged);
    connect(startButton, &QPushButton::clicked, [this]() { emit speedChanged(1000); });
    connect(stopButton, &QPushButton::clicked, [this]() { emit speedChanged(0); });
    connect(emergencyButton, &QPushButton::clicked, [this]() { emit speedChanged(0); });
    connect(startButton, &QPushButton::clicked, this, &Speed::onStartButtonClicked);
    connect(stopButton, &QPushButton::clicked, this, &Speed::onStopButtonClicked);
    connect(emergencyButton, &QPushButton::clicked, this, &Speed::onEmergencyButtonClicked);
}

void Speed::updateSpeedLabel(int value) {
    speedLabel->setText(QString::number(value) + " RPM");
}

void Speed::onStartButtonClicked() {
    // 命令部分的定义
    QByteArray commandPart = "START";

    // 初始化数据帧，首先添加帧头
    QByteArray frame = QByteArray::fromHex("A5");  // 帧头 0xA5

    // 在数据帧中预留长度位置
    frame += QByteArray(1, '\x00');  // 预留一个字节用于存放长度

    // 将命令部分添加到数据帧中
    frame += commandPart;

    // 添加帧尾到数据帧中
    frame += QByteArray::fromHex("5A");  // 帧尾 0x5A

    // 计算整帧的长度，包括所有部分
    int totalLength = frame.size() + 1;  // 长度字节本身也需要计算在内

    // 将长度值设置到数据帧的预留长度位置
    frame[1] = static_cast<char>(totalLength);  // 在第二个位置设置长度

    // 发送构造好的数据帧
    emit sendCommand(frame);

    //监听串口输出到日志，若有回显则发送成功
    qDebug()<<"speed command sent:"<<frame;
}

void Speed::onStopButtonClicked() {
    // 类似地，为STOP命令构建帧
    QByteArray commandPart = "STOP";
    QByteArray frame = QByteArray::fromHex("A5");
    frame += QByteArray(1, '\x00');
    frame += commandPart;
    frame += QByteArray::fromHex("5A");

    // 计算并设置长度
    int totalLength = frame.size() + 1;
    frame[1] = static_cast<char>(totalLength);

    emit sendCommand(frame);

    qDebug()<<"speed command sent:"<<frame;
}

void Speed::onEmergencyButtonClicked() {
    // 为紧急停止命令构建帧
    QByteArray commandPart = "EMERGENCY_STOP";
    QByteArray frame = QByteArray::fromHex("A5");
    frame += QByteArray(1, '\x00');
    frame += commandPart;
    frame += QByteArray::fromHex("5A");

    // 计算并设置长度
    int totalLength = frame.size() + 1;
    frame[1] = static_cast<char>(totalLength);

    emit sendCommand(frame);

    qDebug()<<"speed command sent:"<<frame;
}

void Speed::onSliderValueChanged(int value) {
    QByteArray commandPart = QByteArray::number(value);
    QByteArray frame = QByteArray::fromHex("A5");
    frame += QByteArray(1, '\x00');
    frame += commandPart;
    frame += QByteArray::fromHex("5A");

    // 计算并设置长度
    int totalLength = frame.size() + 1;
    frame[1] = static_cast<char>(totalLength);
    emit sendCommand(frame);

    qDebug()<<"speed command sent:"<<frame;
}
