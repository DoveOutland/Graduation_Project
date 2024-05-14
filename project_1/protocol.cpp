#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::readData()//数据接收
{
    //16进制接收
    QByteArray receivedData = serialPort->readAll();
    QString hexString = QString(receivedData.toHex());
    ui->textBrowser->insertPlainText(hexString);

    //数据解包
    if(!receivedData.isEmpty())
    {
        frame_Unpack(receivedData);
    }

    // 滚动到底部
    ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
}
//帧最长为256B,(格式:[0xA5] [lenth] (data)*N [0x5A]),data最大253B(当前lenth最大为255,数据若超过256B,则需要拓展lenth字节数)
void MainWindow::frame_Unpack(QByteArray frame_data)
{
    for (int i = 0; i < MAX_LEN; i++)
    {
        if(frame_data[i] == char(0xA5))
        {
            if(frame_data[i + frame_data[i+1]] == char(0x5A))//frame_data[i+1]固定为lenth，值=从0计数的帧长度(如0xA5 lenth data2 0x5A,则lenth=3)
            {
                if(i + frame_data[i+1] > MAX_LEN-1)//帧尾访问越界则视为异常数据
                {
                    qDebug()<<"数据异常";
                    return;
                }
                memset(&DataParse, 0, sizeof(Sensor_Data));
                memcpy(&DataParse, frame_data, sizeof(Sensor_Data));
                break;
            }
        }
    }
}
