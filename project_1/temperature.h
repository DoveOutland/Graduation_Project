#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTimer>

class Temperature : public QWidget
{
    Q_OBJECT

public:
    explicit Temperature(QWidget *parent = nullptr);

signals:
    void sendCommand(const QByteArray &command);

private slots:
    void updateTemperature(int value);
    void startSimulation();
    void stopSimulation();
    void loadScenario();
    void simulateTemperatureChange();
    void scenarioChanged(int index);
    void sendTemperature(int temperature);

private:
    QSlider *temperatureSlider;
    QLabel *temperatureLabel;
    QPushButton *startButton, *stopButton;
    QComboBox *scenarioSelector;
    QProgressBar *temperatureProgress;
    QTimer *simulationTimer;

    int simulationTime = 0;
    int currentMode = 0;
    double temperature = 0.0;
    double ambientTemp = 20.0;  // Example ambient temperature
    double initialTemp = 5.0;  // Initial temperature
    double maxTemp = 100.0;     // Maximum temperature for high load
    double coldTemp = 99.0;     // Target temperature for cold start
    double tau = 50.0;          // Time constant for normal operation
    double tauPrime = 30.0;     // Time constant for high load
    double tauDoublePrime = 20.0; // Time constant for cold start
};

#endif // TEMPERATURE_H
