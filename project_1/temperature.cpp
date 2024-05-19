#include "temperature.h"
#include <QTime>
#include <cmath>

Temperature::Temperature(QWidget *parent)
    : QWidget(parent),
      ambientTemp(20.0),
      temperature(ambientTemp),  // 此时 ambientTemp 已经初始化
      simulationTime(0),
      currentMode(0),
      initialTemp(5.0),
      maxTemp(100.0),
      coldTemp(99.0),
      tau(50.0),
      tauPrime(30.0),
      tauDoublePrime(20.0) {
    setFixedSize(1400, 1050);

    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    temperatureSlider = new QSlider(Qt::Horizontal, this);
    temperatureSlider->setRange(0, 100);
    temperatureLabel = new QLabel("Temperature: 0°C", this);
    scenarioSelector = new QComboBox(this);
    scenarioSelector->addItems({"一般启动", "高负载启动", "冷启动"});
    temperatureProgress = new QProgressBar(this);
    temperatureProgress->setRange(0, 100);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(temperatureSlider);
    layout->addWidget(temperatureLabel);
    layout->addWidget(temperatureProgress);
    layout->addWidget(startButton);
    layout->addWidget(stopButton);
    layout->addWidget(scenarioSelector);
    setLayout(layout);

    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &Temperature::simulateTemperatureChange);
    connect(temperatureSlider, &QSlider::valueChanged, this, &Temperature::updateTemperature);
    connect(startButton, &QPushButton::clicked, this, &Temperature::startSimulation);
    connect(stopButton, &QPushButton::clicked, this, &Temperature::stopSimulation);
    connect(scenarioSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Temperature::scenarioChanged);

    temperatureData.reserve(100);  // Prepare space for 100 data points
}

void Temperature::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event); // 显式声明未使用的参数

    QPainter painter(this);
    painter.setPen(Qt::red);
    int y0 = height() / 2;
    if (!temperatureData.isEmpty()) {
        int x = 0;
        for (int i = 1; i < temperatureData.size(); ++i) {
            painter.drawLine(x, y0 - static_cast<int>(temperatureData[i - 1] * 2),
                             x + 8, y0 - static_cast<int>(temperatureData[i] * 2));
            x += 8;
        }
    }
}

void Temperature::updateTemperature(int value) {
    temperatureLabel->setText(QString("Temperature: %1°C").arg(value));
    temperatureProgress->setValue(value);
}

void Temperature::startSimulation() {
    simulationTimer->start(100);  // Start the timer with an interval of 100ms
}

void Temperature::stopSimulation() {
    simulationTimer->stop();
}

void Temperature::loadScenario() {
    // Placeholder for scenario loading logic
}

void Temperature::simulateTemperatureChange() {
    // Increment simulation time
    simulationTime++;

    // Calculate temperature based on the current mode
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

    // Update the temperature data and GUI elements
    temperatureData.append(temperature);
    if (temperatureData.size() > 100) {
        temperatureData.pop_front();
    }
    updateTemperature(static_cast<int>(temperature));
    update();  // Redraw the widget
}

void Temperature::scenarioChanged(int index) {
    currentMode = index;
    simulationTime = 0;
    temperatureData.clear();
    update();  // Reset and redraw
}

void Temperature::sendTemperature(int temperature) {
    Q_UNUSED(temperature);
    // Implement sending temperature data via some communication protocol
}
