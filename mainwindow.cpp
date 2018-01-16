#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    //初始化各参数列表
    BaudrateList<<"256000"<<"230400"<<"128000"<<"115200"<<"76800"<<"57600"
               <<"43000"<<"38400"<<"19200"<<"14400"<<"9600"<<"4800"<<"2400"<<"1200";
    StopbitsList<<"1"<<"1.5"<<"2";
    DatabitsList<<"8"<<"7"<<"6"<<"5";
    ParityList<<tr("无")<<tr("奇校验")<<tr("偶校验");

    //初始化ComboBox
    COMBox = new QComboBox();
    BaudrateBox = new QComboBox();
    BaudrateBox->addItems(BaudrateList);
    StopbitsBox = new QComboBox();
    StopbitsBox->addItems(StopbitsList);
    DatabitsBox = new QComboBox();
    DatabitsBox->addItems(DatabitsList);
    ParityBox = new QComboBox();
    ParityBox->addItems(ParityList);

    //初始化label
    BaudrateLabel = new QLabel(tr("波特率"));
    StopbitsLabel = new QLabel(tr("停止位"));
    DatabitsLabel = new QLabel(tr("数据位"));
    ParityLabel = new QLabel(tr("校验位"));

    //发送与接受区域
    RecvArea = new QTextEdit();
    SendArea = new QTextEdit();

    //按钮
    OpenButton = new QPushButton(tr("打开串口"));
    SendButton = new QPushButton(tr("发送"));

    //初始化布局
    layout = new QGridLayout(this);
    layout->addWidget(BaudrateLabel, 1, 0);
    layout->addWidget(StopbitsLabel, 2, 0);
    layout->addWidget(DatabitsLabel, 3, 0);
    layout->addWidget(ParityLabel, 4, 0);
    layout->addWidget(COMBox, 0, 0, 1, 2);
    layout->addWidget(BaudrateBox, 1,1);
    layout->addWidget(StopbitsBox, 2, 1);
    layout->addWidget(DatabitsBox, 3, 1);
    layout->addWidget(ParityBox, 4, 1);
    layout->addWidget(OpenButton, 5, 1);
    layout->addWidget(SendButton, 5, 2);
    layout->addWidget(RecvArea, 0, 2, 3, 1);
    layout->addWidget(SendArea, 3, 2, 2, 1);
    setLayout(layout);

    //计时器 初始化
    CheckTimer = new QTimer(this);
    connect(CheckTimer, QTimer::timeout, this, CheckSerials);
    CheckTimer->start(1000);

    resize(800, 600);

    //初始化串口列表
    CheckSerials();

    //开始串口进程
//    SerialThr.start();
}

void MainWindow::CheckSerials()
{
    QList<QSerialPortInfo> SerialList = QSerialPortInfo::availablePorts();
    if(!SerialList.isEmpty())
    {
        QStringList TmpComList;
        for(QSerialPortInfo serial : SerialList)
        {
            TmpComList<<serial.portName() + " " + serial.description();
        }
        if(COMList != TmpComList)
        {
            //只在串口发生变化时刷新ComboBox
            COMList = TmpComList;
            COMBox->setDisabled(false);
            COMBox->clear();
            COMBox->addItems(COMList);
        }
    }
    else
    {
        COMBox->clear();
        COMList.clear();
        COMBox->addItem(tr("(空)"));
        COMBox->setDisabled(true);
    }
}

MainWindow::~MainWindow()
{

}
