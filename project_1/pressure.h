#ifndef PRESSURE_H
#define PRESSURE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QFile>
#include <QTextStream>

class Pressure : public QWidget {
    Q_OBJECT

public:
    explicit Pressure(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void startSimulation();
    void stopSimulation();
    void updatePressure();
    void resetSimulation();
    void changeWaveform(int index);
    void saveConfiguration();
    void loadConfiguration();

private:
    QTimer *timer;
    QVector<double> pressureData;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *resetButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QLabel *pressureLabel;
    QLabel *peakPressureLabel;
    QSlider *frequencySlider;
    QComboBox *waveformSelector;
    double peakPressure;
    int waveformType;
};

#endif // PRESSURE_H
