#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
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
    RecvArea->setReadOnly(true);
    QPalette pal;
    pal.setColor(QPalette::Base, Qt::black);
    pal.setColor(QPalette::Text, Qt::green);
    RecvArea->setPalette(pal);
    SendArea = new QTextEdit();
    RecvArea->setFont(QFont(tr("Microsoft YaHei UI"), 10));
    SendArea->setFont(QFont(tr("Microsoft YaHei UI Light"), 12));
    SendArea->installEventFilter(this);

    //按钮
    OpenButton = new QPushButton(tr("打开串口"));
    SendButton = new QPushButton(tr("发送"));
    SendButton->setDisabled(true);
    SendButton->setToolTip(tr("Ctrl+Enter"));
    ClearButton = new QPushButton(tr("清除接收"));

    connect(OpenButton, QPushButton::clicked, this, OpenSerial);
    connect(SendButton, QPushButton::clicked, this, SendContent);
    connect(ClearButton, QPushButton::clicked, this, ClearRecv);

    //复选框
    NewLineBox = new QCheckBox(tr("发送新行"));
    TimerBox = new QCheckBox(tr("定时发送"));

    connect(NewLineBox, QCheckBox::stateChanged, this, detNewLine);
    connect(TimerBox, QCheckBox::stateChanged, this, ControlSendTimer);

    //spinbox
    TimerSpin = new QSpinBox();
    TimerSpin->setSuffix(tr(" ms"));
    TimerSpin->setMaximum(1000000);
    TimerSpin->setValue(1000);

    connect(TimerSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, changeSendTimer);

    //初始化布局
    vlayout = new QVBoxLayout();
    vlayout->addWidget(RecvArea, 7);
    vlayout->addWidget(SendArea, 2);
    hlayout = new QHBoxLayout();
    hlayout->addWidget(NewLineBox, Qt::AlignRight);
    hlayout->addWidget(TimerBox, Qt::AlignRight);
    hlayout->addWidget(TimerSpin);
    hlayout->addWidget(SendButton);
    layout = new QGridLayout(this);
    layout->addWidget(COMBox, 0, 0, 1, 2);
    layout->addWidget(BaudrateLabel, 1, 0);
    layout->addWidget(StopbitsLabel, 2, 0);
    layout->addWidget(DatabitsLabel, 3, 0);
    layout->addWidget(ParityLabel, 4, 0);
    layout->addWidget(BaudrateBox, 1, 1);
    layout->addWidget(StopbitsBox, 2, 1);
    layout->addWidget(DatabitsBox, 3, 1);
    layout->addWidget(ParityBox, 4, 1);
    layout->addWidget(OpenButton, 5, 1);
    layout->addLayout(hlayout, 5, 2);
    //    layout->addWidget(SendButton, 5, 2, Qt::AlignRight);
    layout->addLayout(vlayout, 0, 2, 5, 1);
    layout->addWidget(ClearButton, 0, 3, Qt::AlignLeft);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 4);
    layout->setColumnStretch(3, 1);

    setLayout(layout);

    //计时器 初始化
    CheckTimer = new QTimer(this);
    CheckTimer->start(1000);
    SendTimer = new QTimer(this);
    connect(CheckTimer, QTimer::timeout, this, CheckSerials);
    connect(SendTimer, QTimer::timeout, SendButton, QPushButton::click);

    //初始化串口列表
    CheckSerials();

    //开始串口进程
    serialController = new SerialController;
    serialController->moveToThread(&SerialThr);
    SerialThr.start();

    connect(this, requestOpen, serialController, SerialController::openSerial);
    connect(this, requestClose, serialController, SerialController::closeSerial);
    connect(serialController, SerialController::openSuccess, this, serialOpened);
    connect(serialController, SerialController::openFailed, this, serialNotOpened);
    connect(serialController, SerialController::closeSuccess, this, serialClosed);
    connect(this, setBaudRate, serialController, SerialController::getBaudrate);
    connect(this, setStopBits, serialController, SerialController::getStopbits);
    connect(this, setDataBits, serialController, SerialController::getDatabits);
    connect(this, setParity, serialController, SerialController::getParity);
    connect(BaudrateBox, QComboBox::currentTextChanged, serialController, SerialController::getBaudrate);
    connect(StopbitsBox, QComboBox::currentTextChanged, serialController, SerialController::getStopbits);
    connect(DatabitsBox, QComboBox::currentTextChanged, serialController, SerialController::getDatabits);
    connect(ParityBox, QComboBox::currentTextChanged, serialController, SerialController::getParity);
    connect(this, sendData, serialController, SerialController::writeData);
    connect(serialController, SerialController::recvData, this, getRecv);
}

void MainWidget::CheckSerials()
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
            OpenButton->setDisabled(false);
        }
    }
    else
    {
        COMBox->clear();
        COMList.clear();
        COMBox->addItem(tr("(空)"));
        COMBox->setDisabled(true);
        OpenButton->setDisabled(true);
        emit CloseSerial();
    }
}

MainWidget::~MainWidget()
{
    SerialThr.terminate();
}

void MainWidget::serialOpened()
{
    OpenButton->setText(tr("关闭串口"));
    emit sendStatus(QString(/*tr("串口已打开") + */COMBox->currentText()));
    COMBox->setDisabled(true);
    SendButton->setDisabled(false);
    disconnect(OpenButton, QPushButton::clicked, this, OpenSerial);
    connect(OpenButton, QPushButton::clicked, this, CloseSerial);
}

void MainWidget::serialNotOpened()
{
    emit sendStatus(tr("串口打开失败"));
}

void MainWidget::serialClosed()
{
    OpenButton->setText(tr("打开串口"));
    COMBox->setDisabled(false);
    SendButton->setDisabled(true);
    disconnect(OpenButton, QPushButton::clicked, this, CloseSerial);
    connect(OpenButton, QPushButton::clicked, this, OpenSerial);
    emit sendStatus(tr("串口已关闭"));
}

void MainWidget::getRecv(QByteArray recv)
{
    RecvArea->moveCursor(QTextCursor::End);
    RecvArea->textCursor().insertText(recv);
    RecvArea->selectionChanged();
}

void MainWidget::OpenSerial()
{
    QString portName = COMBox->currentText().split(' ')[0];
    qDebug()<<portName;
    emit requestOpen(portName);
    emit setBaudRate(BaudrateBox->currentText());
    emit setStopBits(StopbitsBox->currentText());
    emit setDataBits(DatabitsBox->currentText());
    emit setParity(ParityBox->currentText());
}

void MainWidget::CloseSerial()
{
    emit requestClose();
}

void MainWidget::ClearRecv()
{
    RecvArea->clear();
}

void MainWidget::detNewLine(int state)
{
    if(state==2)
        isSendNewLine = true;
    else if(state == 0)
        isSendNewLine = false;
}

void MainWidget::ControlSendTimer(int state)
{
    if(state == 0)
        SendTimer->stop();
    else if(state == 2)
        SendTimer->start(TimerSpin->value());
}

void MainWidget::changeSendTimer()
{
    ControlSendTimer(TimerBox->checkState());
}

void MainWidget::SendContent()
{
    QString content = SendArea->toPlainText();
    if(isSendNewLine)
        content += "\r\n";
    emit sendData(content);
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==SendArea&&event->type()==QEvent::KeyPress)
    {
        QKeyEvent *e = (QKeyEvent*)event;
        if(e->modifiers()==Qt::ControlModifier)
        {
            switch (e->key())
            {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                SendButton->click();
                break;
            default:
                break;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
