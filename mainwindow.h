#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QSerialPortInfo>
#include <QLabel>
#include <QThread>


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void CheckSerials();
    ~MainWindow();

private:
    QComboBox *COMBox, *BaudrateBox, *StopbitsBox, *DatabitsBox, *ParityBox;
    QStringList COMList, BaudrateList, StopbitsList, DatabitsList, ParityList;
    QLabel *BaudrateLabel, *StopbitsLabel, *DatabitsLabel, *ParityLabel;
    QGridLayout *layout;
    QTextEdit *RecvArea, *SendArea;
    QPushButton *OpenButton, *SendButton;
    QTimer *CheckTimer;
    QThread SerialThr;
};

#endif // MAINWINDOW_H
