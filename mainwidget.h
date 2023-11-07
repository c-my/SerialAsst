#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QApplication>
#include <QByteArray>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSettings>
#include <QSpinBox>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
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
  void requestOpen(const QString &portName);
  void requestClose();
  void setBaudRate(const QString &baudrate);
  void setStopBits(const QString &stopbits);
  void setDataBits(const QString &databits);
  void setParity(const QString &parity);
  void setFlowControl(const QString &flowControl);
  void sendData(const QByteArray &content);
  void sendStatus(const QString &status);     //更新statusbar
  void sendDateTime(const QString &datetime); //更新stastatusbar中的时间
  void changeRTS(bool set);
  void changeDTR(bool set);

  public slots:
  void serialOpened();           //串口打开成功
  void serialNotOpened();        //串口打开失败
  void serialClosed();           //串口关闭
  void getRecv(const QByteArray &recv); //串口接受数据
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
  QComboBox *COMBox, *BaudrateBox, *StopbitsBox, *DatabitsBox, *ParityBox, *FlowControlBox;
  QStringList BaudrateList, StopbitsList, DatabitsList, ParityList, FlowControlList;
  QStringList COMList, PortNameList, DescList;
  QLabel *COMLabel, *BaudrateLabel, *StopbitsLabel, *DatabitsLabel, *ParityLabel, *FlowControlLabel;
  QGridLayout *centralLayout;
  QVBoxLayout *cvlayout, *rightLayout;
  QHBoxLayout *bottomLayout, *flowControlLayout;
  QFormLayout *leftLlayout;
  QGroupBox *paramGroup;
  QPlainTextEdit *RecvArea, *SendArea;
  QPushButton *OpenButton, *SendButton, *ClearRecvButton, *ClearSendButton;
  QCheckBox *NewLineBox, *TimerBox, *HexSend, *HexRecv, *RTSBox, *DTRBox;
  QSpinBox *TimerSpin;
  QTimer *CheckTimer, *SendTimer;
  SerialController *serialController;
  QSettings settings;

  static const char HEX_SEPARATOR{':'};

  bool isSendNewLine = false;
  bool isSendHex = false;
  bool isRecvHex = false;
  bool isOpened = false;

  QByteArray HexStringToString(const QString &hexstr); //解码16进制字符串
  char convertAsciiToHex(char ascii);
  void ActionAttachToSerial(bool set);
};

#endif // MAINWIDGET_H
