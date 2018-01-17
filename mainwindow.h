#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QSerialPortInfo>
#include <QLabel>
#include <QThread>
#include "serialcontroller.h"

#include <QDebug>


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void CheckSerials();
    ~MainWindow();

signals:
    void requestOpen(QString portName);
    void requestClose();
    void setBaudRate(QString baudrate);
    void setStopBits(QString stopbits);
    void setDataBits(QString databits);
    void setParity(QString parity);
    void sendData(QString content);

public slots:
    void serialOpened();
    void serialNotOpened();
    void serialClosed();

private:
    QComboBox *COMBox, *BaudrateBox, *StopbitsBox, *DatabitsBox, *ParityBox;
    QStringList COMList, BaudrateList, StopbitsList, DatabitsList, ParityList;
    QLabel *BaudrateLabel, *StopbitsLabel, *DatabitsLabel, *ParityLabel;
    QGridLayout *layout;
    QPlainTextEdit *RecvArea, *SendArea;
    QPushButton *OpenButton, *SendButton;
    QTimer *CheckTimer;
    SerialController *serialController;
    QThread SerialThr;

    void OpenSerial();
    void CloseSerial();
    void SendContent();
};

#endif // MAINWINDOW_H
