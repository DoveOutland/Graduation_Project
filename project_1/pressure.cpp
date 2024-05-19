#include "pressure.h"
#include <QTime>
#include <cmath>

Pressure::Pressure(QWidget *parent)
    : QWidget(parent), peakPressure(0.0), waveformType(0) {
    setFixedSize(1400, 1050);

    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    resetButton = new QPushButton("Reset", this);
    saveButton = new QPushButton("Save Configuration", this);
    loadButton = new QPushButton("Load Configuration", this);
    pressureLabel = new QLabel("压力: 0", this);
    peakPressureLabel = new QLabel("最高压力: 0", this);
    frequencySlider = new QSlider(Qt::Horizontal, this);
    frequencySlider->setRange(10, 1000);
    frequencySlider->setValue(100);
    waveformSelector = new QComboBox(this);
    waveformSelector->addItem("正弦波");
    waveformSelector->addItem("锯齿波");
    waveformSelector->addItem("方波");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(startButton);
    layout->addWidget(stopButton);
    layout->addWidget(resetButton);
    layout->addWidget(saveButton);
    layout->addWidget(loadButton);
    layout->addWidget(frequencySlider);
    layout->addWidget(waveformSelector);
    layout->addWidget(pressureLabel);
    layout->addWidget(peakPressureLabel);
    setLayout(layout);

    connect(startButton, &QPushButton::clicked, this, &Pressure::startSimulation);
    connect(stopButton, &QPushButton::clicked, this, &Pressure::stopSimulation);
    connect(resetButton, &QPushButton::clicked, this, &Pressure::resetSimulation);
    connect(saveButton, &QPushButton::clicked, this, &Pressure::saveConfiguration);
    connect(loadButton, &QPushButton::clicked, this, &Pressure::loadConfiguration);
    connect(waveformSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Pressure::changeWaveform);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Pressure::updatePressure);
}

void Pressure::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event); // 显式声明未使用的参数

    QPainter painter(this);
    painter.setPen(Qt::blue);
    int y0 = height() / 2;
    if (pressureData.size() > 1) {
        for (int i = 1; i < pressureData.size(); ++i) {
            painter.drawLine(8 * (i - 1), y0 - static_cast<int>(pressureData[i - 1]),
                             8 * i, y0 - static_cast<int>(pressureData[i]));
        }
    }
}

void Pressure::startSimulation() {
    timer->start(1000 / frequencySlider->value());
}

void Pressure::stopSimulation() {
    timer->stop();
}

void Pressure::resetSimulation() {
    pressureData.clear();
    peakPressure = 0.0;
    peakPressureLabel->setText("Peak Pressure: 0");
    update();
}

void Pressure::updatePressure() {
    double currentPressure = 0;
    switch (waveformType) {
        case 0:
            currentPressure = 100 * std::sin(QTime::currentTime().msec() / 1000.0);
            break;
        case 1:
            currentPressure = 100 * (static_cast<double>(QTime::currentTime().msec() % 1000) / 1000 - 0.5);
            break;
        case 2:
            currentPressure = (QTime::currentTime().msec() % 1000 > 500) ? 100 : -100;
            break;
    }
    pressureData.append(currentPressure);
    if (pressureData.size() > 100) pressureData.pop_front();
    pressureLabel->setText(QString("Pressure: %1").arg(currentPressure));
    if (currentPressure > peakPressure) {
        peakPressure = currentPressure;
        peakPressureLabel->setText(QString("Peak Pressure: %1").arg(peakPressure));
    }
    update();  // Trigger a repaint
}

void Pressure::changeWaveform(int index) {
    waveformType = index;
    resetSimulation();  // Reset simulation to apply new waveform
}

void Pressure::saveConfiguration() {
    QFile file("pressure_config.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << waveformType << "\n";
        out << frequencySlider->value() << "\n";
    }
}

void Pressure::loadConfiguration() {
    QFile file("pressure_config.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int type, frequency;
        in >> type;
        in >> frequency;
        waveformSelector->setCurrentIndex(type);
        frequencySlider->setValue(frequency);
    }
}
