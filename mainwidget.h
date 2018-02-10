#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QApplication>
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
#include <QSettings>
#include <QFormLayout>
#include <QGroupBox>
#include "serialcontroller.h"

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
  void sendStatus(QString status);     //更新statusbar
  void sendDateTime(QString datetime); //更新stastatusbar中的时间
  void changeRTS(bool set);
  void changeDTR(bool set);

public slots:
  void serialOpened();           //串口打开成功
  void serialNotOpened();        //串口打开失败
  void serialClosed();           //串口关闭
  void getRecv(QByteArray recv); //串口接受数据
  void OpenSerial();
  void CloseSerial();
  void ClearRecv();
  void ClearSend();
  void detNewLine(int state); //处理发送新行
  void ControlSendTimer(int state);
  void changeSendTimer();
  void detHex(int state);     //处理16进制发送
  void detRecvHex(int state); //处理16进制接收
  void RTSControl(int state);
  void DTRControl(int state);

private:
  QComboBox *COMBox, *BaudrateBox, *StopbitsBox, *DatabitsBox, *ParityBox;
  QStringList BaudrateList, StopbitsList, DatabitsList, ParityList;
  QStringList COMList, PortNameList, DescList;
  QLabel *COMLabel, *BaudrateLabel, *StopbitsLabel, *DatabitsLabel, *ParityLabel;
  QGridLayout *centralLayout;
  QVBoxLayout *cvlayout, *rightLayout;
  QHBoxLayout *bottomLayout;
  QFormLayout *leftLlayout;
  QGroupBox *paramGroup;
  QTextEdit *RecvArea, *SendArea;
  QPushButton *OpenButton, *SendButton, *ClearRecvButton, *ClearSendButton;
  QCheckBox *NewLineBox, *TimerBox, *HexSend, *HexRecv, *RTSBox, *DTRBox;
  QSpinBox *TimerSpin;
  QTimer *CheckTimer, *SendTimer;
  SerialController *serialController;
  QThread SerialThr;
  QSettings settings;

  bool isSendNewLine = false;
  bool isSendHex = false;
  bool isRecvHex = false;
  bool isOpened = false;

  QString HexStringToString(QString hexstr); //解码16进制字符串
  void ACtionAttachToSerial(bool set);
};

#endif // MAINWIDGET_H
