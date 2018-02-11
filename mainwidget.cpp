#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent), settings("TurnMeOn", "SerialAsst")
{
    //初始化各参数列表
    BaudrateList << "256000"
                 << "230400"
                 << "128000"
                 << "115200"
                 << "76800"
                 << "57600"
                 << "43000"
                 << "38400"
                 << "19200"
                 << "14400"
                 << "9600"
                 << "4800"
                 << "2400"
                 << "1200";
    StopbitsList << "1"
                 << "1.5"
                 << "2";
    DatabitsList << "8"
                 << "7"
                 << "6"
                 << "5";
    ParityList << tr("无") << tr("奇校验") << tr("偶校验");

    //初始化ComboBox
    COMBox = new QComboBox();
    BaudrateBox = new QComboBox();
    BaudrateBox->addItems(BaudrateList);
    BaudrateBox->setCurrentIndex(settings.value("Baud rate", 0).toInt());
    StopbitsBox = new QComboBox();
    StopbitsBox->addItems(StopbitsList);
    StopbitsBox->setCurrentIndex(settings.value("Stop bits", 0).toInt());
    DatabitsBox = new QComboBox();
    DatabitsBox->addItems(DatabitsList);
    DatabitsBox->setCurrentIndex(settings.value("Data bits", 0).toInt());
    ParityBox = new QComboBox();
    ParityBox->addItems(ParityList);
    ParityBox->setCurrentIndex(settings.value("Parity", 0).toInt());

    //初始化label
    COMLabel = new QLabel(tr("串口号"));
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
    ClearRecvButton = new QPushButton(tr("清除接收"));
    ClearSendButton = new QPushButton(tr("清除发送"));

    connect(OpenButton, QPushButton::clicked, this, OpenSerial);
    connect(SendButton, QPushButton::clicked, this, SendContent);
    connect(ClearRecvButton, QPushButton::clicked, this, ClearRecv);
    connect(ClearSendButton, QPushButton::clicked, this, ClearSend);

    //复选框
    NewLineBox = new QCheckBox(tr("发送新行"));
    TimerBox = new QCheckBox(tr("定时发送"));
    HexSend = new QCheckBox(tr("发送16进制"));
    HexRecv = new QCheckBox(tr("接收16进制"));
    HexSend->setToolTip(tr("以空格作为间隔符， \n非法字符及其后面的字符将被忽略"));
    RTSBox = new QCheckBox(tr("RTS"));
    DTRBox = new QCheckBox(tr("DTR"));

    //绑定复选框信号
    connect(NewLineBox, QCheckBox::stateChanged, this, detNewLine);
    connect(TimerBox, QCheckBox::stateChanged, this, ControlSendTimer);
    connect(HexSend, QCheckBox::stateChanged, this, detHex);
    connect(HexRecv, QCheckBox::stateChanged, this, detRecvHex);
    connect(RTSBox, QCheckBox::stateChanged, this, RTSControl);
    connect(DTRBox, QCheckBox::stateChanged, this, DTRControl);

    //定时发送spinbox
    TimerSpin = new QSpinBox();
    TimerSpin->setSuffix(tr(" ms"));
    TimerSpin->setMaximum(1000000);
    TimerSpin->setValue(1000);
    //valueChanged有多个重载
    connect(TimerSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, changeSendTimer);

    //初始化布局
    leftLlayout = new QFormLayout();
    leftLlayout->addRow(BaudrateLabel, BaudrateBox);
    leftLlayout->addRow(StopbitsLabel, StopbitsBox);
    leftLlayout->addRow(DatabitsLabel, DatabitsBox);
    leftLlayout->addRow(ParityLabel, ParityBox);
    leftLlayout->addRow(COMLabel, COMBox);
    leftLlayout->addRow(OpenButton);
    leftLlayout->setAlignment(OpenButton, Qt::AlignVCenter);
    leftLlayout->setMargin(30);
    leftLlayout->setHorizontalSpacing(20);
    leftLlayout->setVerticalSpacing(60);

    paramGroup = new QGroupBox();
    paramGroup->setLayout(leftLlayout);
    paramGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    cvlayout = new QVBoxLayout();
    cvlayout->addWidget(RecvArea, 7);
    cvlayout->addWidget(SendArea, 2);

    bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(ClearSendButton, 0, Qt::AlignLeft);
    bottomLayout->addWidget(HexSend);
    bottomLayout->addWidget(NewLineBox, 0, Qt::AlignLeft);
    bottomLayout->addWidget(TimerBox, 0, Qt::AlignRight);
    bottomLayout->addWidget(TimerSpin, 1, Qt::AlignLeft);
    bottomLayout->addWidget(SendButton, 0, Qt::AlignRight);

    rightLayout = new QVBoxLayout();
    rightLayout->addWidget(ClearRecvButton, 0, Qt::AlignCenter);
    rightLayout->addWidget(HexRecv);
    rightLayout->addWidget(RTSBox);
    rightLayout->addWidget(DTRBox);

    centralLayout = new QGridLayout(this);

    centralLayout->addWidget(paramGroup, 0, 0, 2, 1);
    centralLayout->addLayout(bottomLayout, 1, 1);
    centralLayout->addLayout(cvlayout, 0, 1, 1, 1);
    centralLayout->addLayout(rightLayout, 0, 2, Qt::AlignTop);

    centralLayout->setColumnMinimumWidth(0, 150);
    centralLayout->setColumnMinimumWidth(2, 100);

    centralLayout->setColumnStretch(0, 0);
    centralLayout->setColumnStretch(1, 2);
    centralLayout->setColumnStretch(2, 0);

    setLayout(centralLayout);

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

    //connect开关串口控制信号 以及是否成功的返回信号
    connect(this, requestOpen, serialController, SerialController::openSerial);
    connect(this, requestClose, serialController, SerialController::closeSerial);
    connect(serialController, SerialController::openSuccess, this, serialOpened);
    connect(serialController, SerialController::openFailed, this, serialNotOpened);
    connect(serialController, SerialController::closeSuccess, this, serialClosed);
    //connect波特率等参数的控制信号
    connect(this, setBaudRate, serialController, SerialController::getBaudrate);
    connect(this, setStopBits, serialController, SerialController::getStopbits);
    connect(this, setDataBits, serialController, SerialController::getDatabits);
    connect(this, setParity, serialController, SerialController::getParity);
    connect(BaudrateBox, QComboBox::currentTextChanged, serialController, SerialController::getBaudrate);
    connect(StopbitsBox, QComboBox::currentTextChanged, serialController, SerialController::getStopbits);
    connect(DatabitsBox, QComboBox::currentTextChanged, serialController, SerialController::getDatabits);
    connect(ParityBox, QComboBox::currentTextChanged, serialController, SerialController::getParity);
    connect(this, changeRTS, serialController, SerialController::contrloRTS);
    connect(this, changeDTR, serialController, SerialController::controlDTR);
    connect(this, sendData, serialController, SerialController::writeData);
    connect(serialController, SerialController::recvData, this, getRecv);
}

void MainWidget::CheckSerials()
{
    //不断检查可用串口列表，并与当前列表进行比较，若发生变化则重新生成列表
    emit sendDateTime(QDateTime::currentDateTime().toString());//更新状态栏时间

    QList<QSerialPortInfo> SerialList = QSerialPortInfo::availablePorts();
    if (!SerialList.isEmpty())
    {
        QStringList TmpComList, TmpPortNameList, TmpDesList;
        for (QSerialPortInfo serial : SerialList)
        {
            TmpComList << serial.portName() + " " + serial.description();
            TmpPortNameList << serial.portName();
            TmpDesList << serial.description();
        }
        if (COMList != TmpComList)
        {
            //TODO: 可用串口发生改变却不为空时，保持串口打开状态，及其他细节的处理
            //只在串口发生变化时刷新ComboBox
            COMList = TmpComList;
            PortNameList = TmpPortNameList;
            DescList = TmpDesList;
            COMBox->setDisabled(false);
            COMBox->clear();
            COMBox->addItems(PortNameList);
            OpenButton->setDisabled(false);
            for (int i = 0; i < COMList.count(); i++)   //为串口列表增加ToolTip
            {
                COMBox->setItemData(i, DescList[i], Qt::ToolTipRole);
            }
        }
    }
    else    //可用串口为空时发送关闭串口信号
    {
        COMBox->clear();
        COMList.clear();
        COMBox->addItem(tr("(空)"));
        COMBox->setDisabled(true);
        OpenButton->setDisabled(true);
        emit CloseSerial();
        isOpened = false;
    }
}

MainWidget::~MainWidget()
{
    SerialThr.terminate();
    settings.setValue("Baud rate", BaudrateBox->currentIndex());
    settings.setValue("Stop bits", StopbitsBox->currentIndex());
    settings.setValue("Data bits", DatabitsBox->currentIndex());
    settings.setValue("Parity", ParityBox->currentIndex());
}

void MainWidget::serialOpened()
{
    isOpened = true;
    int portIndex = COMBox->currentIndex();
    emit sendStatus(QString(PortNameList[portIndex] + tr(" ") + DescList[portIndex]));
    //串口关闭时无法设置RTS,DTR信号（但此时checkbox仍然是可操作的），串口打开时发送stateChanged信号来应用改变
    emit RTSBox->stateChanged(RTSBox->checkState());
    emit DTRBox->stateChanged(DTRBox->checkState());
    //相应控件可用性做出改变(setDisabled)
    ACtionAttachToSerial(true);
}

void MainWidget::serialNotOpened()
{
    //设置状态栏 并发出警告音
    //TODO: 此处应有更容易察觉的提示
    emit sendStatus(tr("串口打开失败"));
    QApplication::beep();
}

void MainWidget::serialClosed()
{
    isOpened = false;
    //相应控件可用性做出改变(setDisabled)
    ACtionAttachToSerial(false);
    emit sendStatus(tr("串口已关闭"));
}

void MainWidget::getRecv(QByteArray recv)
{
    RecvArea->moveCursor(QTextCursor::End);
    //需要时将受到的数据进行16进制转换
    //TODO: 支持更多编码（可选择）
    if (!isRecvHex)
        RecvArea->textCursor().insertText(QString::fromLocal8Bit(recv));
    else
    {
        RecvArea->textCursor().insertText(QString(recv.toHex()) + tr(" "));
    }
}

void MainWidget::OpenSerial()
{
    QString portName = COMBox->currentText();
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

void MainWidget::ClearSend()
{
    SendArea->clear();
}

void MainWidget::detNewLine(int state)
{
    if (state == 2)
        isSendNewLine = true;
    else if (state == 0)
        isSendNewLine = false;
}

void MainWidget::ControlSendTimer(int state)
{
    if (state == 0)
        SendTimer->stop();
    else if (state == 2)
        SendTimer->start(TimerSpin->value());
}

void MainWidget::changeSendTimer()
{
    ControlSendTimer(TimerBox->checkState());
}

void MainWidget::detHex(int state)
{
    //TODO: 换行也可以作为16进制的分隔符
    if (state == 2)
    {
        isSendHex = true;
        SendArea->setText(SendArea->toPlainText().toLocal8Bit().toHex(' '));
    }
    else if (state == 0)
    {
        isSendHex = false;
        QString tmpstr = SendArea->toPlainText();
        SendArea->clear();
        SendArea->setText(HexStringToString(tmpstr));
    }
}

void MainWidget::detRecvHex(int state)
{
    if (state == 2)
    {
        isRecvHex = true;
        RecvArea->setText(RecvArea->toPlainText().toLocal8Bit().toHex(' '));
        RecvArea->moveCursor(QTextCursor::End);
        RecvArea->insertPlainText(tr(" "));
    }
    else if (state == 0)
    {
        isRecvHex = false;
        QString tmpstr = RecvArea->toPlainText();
        RecvArea->clear();
        RecvArea->setText(HexStringToString(tmpstr));
    }
}

void MainWidget::RTSControl(int state)
{
    if(isOpened)
    {
        if(state == 2)
            emit changeRTS(true);
        else if(state == 0)
            emit changeRTS(false);
    }
}

void MainWidget::DTRControl(int state)
{
    if(isOpened)
    {
        if(state == 2)
            emit changeDTR(true);
        else if(state == 0)
            emit changeDTR(false);
    }
}

QString MainWidget::HexStringToString(QString hexstr)
{
    //将内容为16进制字符串的string转换成16进制对应的内容
    QStringList list = hexstr.split(' ');
    QByteArray bit;
    foreach (QString str, list)
    {
        if (!str.isEmpty())
            bit.append(str.toInt(nullptr, 16));
    }
    return QString::fromLocal8Bit(bit);
}

void MainWidget::ACtionAttachToSerial(bool set)
{
    //根据串口开关状态决定一些控件的可用性
    if(set)
    {
        OpenButton->setText(tr("关闭串口"));
        COMBox->setDisabled(true);
        SendButton->setDisabled(false);
        disconnect(OpenButton, QPushButton::clicked, this, OpenSerial);
        connect(OpenButton, QPushButton::clicked, this, CloseSerial);
    }
    else
    {
        OpenButton->setText(tr("打开串口"));
        COMBox->setDisabled(false);
        SendButton->setDisabled(true);
        disconnect(OpenButton, QPushButton::clicked, this, CloseSerial);
        connect(OpenButton, QPushButton::clicked, this, OpenSerial);
    }
}

void MainWidget::SendContent()
{
    QString content = SendArea->toPlainText();
    if (isSendHex)
        content = HexStringToString(content);
    if (isSendNewLine)
        content += "\r\n";
    emit sendData(content);
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == SendArea && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *e = (QKeyEvent *)event;
        if (e->modifiers() == Qt::ControlModifier)
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
