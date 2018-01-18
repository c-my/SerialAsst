#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QSerialPortInfo>
#include <QLabel>
#include <QThread>
#include "serialcontroller.h"

#include <QDebug>


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    void CheckSerials();
    ~MainWidget();
    void SendContent();
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void requestOpen(QString portName);
    void requestClose();
    void setBaudRate(QString baudrate);
    void setStopBits(QString stopbits);
    void setDataBits(QString databits);
    void setParity(QString parity);
    void sendData(QString content);
    void sendStatus(QString status);    //更新statusbar

public slots:
    void serialOpened();    //串口打开成功
    void serialNotOpened(); //串口打开失败
    void serialClosed();    //串口关闭
    void getRecv(QByteArray recv); //串口接受数据
    void OpenSerial();
    void CloseSerial();
    void ClearRecv();

private:
    QComboBox *COMBox, *BaudrateBox, *StopbitsBox, *DatabitsBox, *ParityBox;
    QStringList COMList, BaudrateList, StopbitsList, DatabitsList, ParityList;
    QLabel *BaudrateLabel, *StopbitsLabel, *DatabitsLabel, *ParityLabel;
    QGridLayout *layout;
    QTextEdit *RecvArea, *SendArea;
    QPushButton *OpenButton, *SendButton, *ClearButton;
    QTimer *CheckTimer;
    SerialController *serialController;
    QThread SerialThr;

};

#endif // MAINWIDGET_H
