#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>
#include <QFile>
#include <QTextStream>

#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_clicked(bool checked);

    void readData();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked(bool checked);

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QSerialPort *serialPort;
    QVector<double> Time;
    QVector<double> Data;

    void Sensor_Plot(QVector<double>, QVector<double>);
    void savePlot(const QString &fileName);
    void saveCSV(const QString &fileName);

};
#endif // MAINWINDOW_H
