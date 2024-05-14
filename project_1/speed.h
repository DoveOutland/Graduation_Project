#ifndef SPEED_H
#define SPEED_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

class Speed : public QWidget {
    Q_OBJECT

public:
    explicit Speed(QWidget *parent = nullptr);

signals:
    void speedChanged(int speed);  // 确保信号声明正确
    void sendCommand(const QByteArray &command);

private slots:
    void updateSpeedLabel(int value);
    void onStartButtonClicked();
    void onStopButtonClicked();
    void onEmergencyButtonClicked();
    void onSliderValueChanged(int value);

private:
    QSlider *speedSlider;
    QLabel *speedLabel;
    QComboBox *presetSpeeds;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *emergencyButton;
};

#endif // SPEED_H
