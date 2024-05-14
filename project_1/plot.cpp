#include "mainwindow.h"
#include "ui_mainwindow.h"

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
