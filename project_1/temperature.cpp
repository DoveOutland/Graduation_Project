#include "temperature.h"
#include <cmath>

Temperature::Temperature(QWidget *parent) : QWidget(parent) {
    temperatureSlider = new QSlider(Qt::Horizontal, this);
    temperatureSlider->setRange(0, 100);

    temperatureLabel = new QLabel("Temperature: 0°C", this);

    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);

    scenarioSelector = new QComboBox(this);
    scenarioSelector->addItems({"Normal Operation", "High Load", "Cold Start"});

    temperatureProgress = new QProgressBar(this);
    temperatureProgress->setRange(0, 100);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(temperatureSlider);
    layout->addWidget(temperatureLabel);
    layout->addWidget(temperatureProgress);
    layout->addWidget(startButton);
    layout->addWidget(stopButton);
    layout->addWidget(scenarioSelector);

    simulationTimer = new QTimer(this);
    simulationTimer->setInterval(100);
    connect(simulationTimer, &QTimer::timeout, this, &Temperature::simulateTemperatureChange);

    connect(scenarioSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(scenarioChanged(int)));
    connect(temperatureSlider, &QSlider::valueChanged, this, &Temperature::updateTemperature);
    connect(startButton, &QPushButton::clicked, this, &Temperature::startSimulation);
    connect(stopButton, &QPushButton::clicked, this, &Temperature::stopSimulation);
}

void Temperature::updateTemperature(int value) {
    temperatureLabel->setText(QString("Temperature: %1°C").arg(value));
    temperatureProgress->setValue(value);
}

void Temperature::startSimulation() {
    // Implement simulation start logic
    simulationTimer->start();
}

void Temperature::stopSimulation() {
    // Implement simulation stop logic
    temperature = 0;
    temperatureSlider->setValue(temperature);
    simulationTimer->stop();
}

void Temperature::loadScenario() {
    // Implement scenario loading logic
}
void Temperature::simulateTemperatureChange() {
    simulationTime += 1;  // Increase time step

    switch(currentMode) {
        case 0: // Normal Operation
            temperature = ambientTemp + (initialTemp - ambientTemp) * exp(-simulationTime / tau);
            break;
        case 1: // High Load
            temperature = maxTemp - (maxTemp - ambientTemp) * exp(-simulationTime / tauPrime);
            break;
        case 2: // Cold Start
            temperature = ambientTemp + (coldTemp - ambientTemp) * (1 - exp(-pow(simulationTime, 2) / tauDoublePrime));
            break;
    }

    temperatureSlider->setValue(static_cast<int>(temperature));
    updateTemperature(static_cast<int>(temperature));

    // 发送更新的温度值
    sendTemperature(static_cast<int>(temperature));
}
void Temperature::scenarioChanged(int index) {
    currentMode = index;  // 直接使用下拉列表的索引来设置当前模式
    // 可以在这里重置模拟时间或进行其他相关设置
    simulationTime = 0;  // 重置模拟时间
    temperature = 0;
    temperatureSlider->setValue(ambientTemp);  // 重置滑块位置到环境温度
    sendTemperature(temperature);
}
void Temperature::sendTemperature(int temperature) {
    // 将温度值转换为字节数组
    QByteArray commandPart = QByteArray::number(temperature);

    // 创建数据帧，以0xA5作为帧头
    QByteArray frame = QByteArray::fromHex("A5");
    frame += QByteArray(1, '\x00');  // 预留长度位
    frame += commandPart;
    frame += QByteArray::fromHex("5A");  // 帧尾

    // 计算整帧的长度，并设置在帧的第二位
    int totalLength = frame.size() + 1;  // 加1是因为长度字节本身也需要计算
    frame[1] = static_cast<char>(totalLength);

    // 发送命令
    emit sendCommand(frame);
    qDebug()<<"temperature command sent:"<<frame;
}
/*
### 主要功能
1. **温度范围设定**：通过滑动条调整模拟的温度范围。
2. **实时温度显示**：实时显示当前模拟温度。
3. **预设场景选择**：通过下拉菜单选择不同的温度模拟场景。
4. **模拟控制**：通过开始和停止按钮控制温度模拟的开始和结束。
5. **温度变化可视化**：通过进度条显示温度的实时变化。

### 操作方式
1. **调整温度**：
   - 使用滑动条来设置想要模拟的具体温度值。
   - 滑动条通常设置有最小值和最大值，例如从0°C到100°C。

2. **查看当前温度**：
   - 观察标签上显示的当前温度值，该标签会实时更新以反映滑动条的位置。

3. **选择模拟场景**：
   - 从下拉菜单中选择预设的温度场景，如“正常操作”、“高负载”或“冷启动”。
   - 每个场景都预设了特定的温度变化曲线和持续时间。

*/
