#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QSerialPortInfo>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QThread>
#include <QDateTime>
#include <QByteArray>
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
    void sendDateTime(QString datetime);

public slots:
    void serialOpened();    //串口打开成功
    void serialNotOpened(); //串口打开失败
    void serialClosed();    //串口关闭
    void getRecv(QByteArray recv); //串口接受数据
    void OpenSerial();
    void CloseSerial();
    void ClearRecv();
    void detNewLine(int state);
    void ControlSendTimer(int state);
    void changeSendTimer();
    void detHex(int state);

private:
    QComboBox *COMBox, *BaudrateBox, *StopbitsBox, *DatabitsBox, *ParityBox;
    QStringList COMList, BaudrateList, StopbitsList, DatabitsList, ParityList;
    QLabel *BaudrateLabel, *StopbitsLabel, *DatabitsLabel, *ParityLabel;
    QGridLayout *layout;
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    QTextEdit *RecvArea, *SendArea;
    QPushButton *OpenButton, *SendButton, *ClearButton;
    QCheckBox *NewLineBox, *TimerBox, *HexSend;
    QSpinBox *TimerSpin;
    QTimer *CheckTimer, *SendTimer;
    SerialController *serialController;
    QThread SerialThr;

    bool isSendNewLine = false;
    bool isSendHex = false;

    QString HexStringToString(QString hexstr);

};

#endif // MAINWIDGET_H
