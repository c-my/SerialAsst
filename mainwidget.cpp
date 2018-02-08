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
    COMLabel = new QLabel(tr("串口"));
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
    HexSend = new QCheckBox(tr("发送16进制"));
    HexRecv = new QCheckBox(tr("接收16进制"));
    HexSend->setToolTip(tr("以空格作为间隔符， \n非法字符及其后面的字符将被忽略"));

    connect(NewLineBox, QCheckBox::stateChanged, this, detNewLine);
    connect(TimerBox, QCheckBox::stateChanged, this, ControlSendTimer);
    connect(HexSend, QCheckBox::stateChanged, this, detHex);
    connect(HexRecv, QCheckBox::stateChanged, this, detRecvHex);

    //spinbox
    TimerSpin = new QSpinBox();
    TimerSpin->setSuffix(tr(" ms"));
    TimerSpin->setMaximum(1000000);
    TimerSpin->setValue(1000);

    connect(TimerSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, changeSendTimer);

    //初始化布局
    flayout = new QFormLayout();
    flayout->addRow(BaudrateLabel, BaudrateBox);
    flayout->addRow(StopbitsLabel, StopbitsBox);
    flayout->addRow(DatabitsLabel, DatabitsBox);
    flayout->addRow(ParityLabel, ParityBox);
    flayout->addRow(COMLabel, COMBox);
    flayout->addRow(OpenButton);
    flayout->setAlignment(OpenButton, Qt::AlignVCenter);
    //    flayout->setSpacing(50);
    //    flayout->setMargin(10);
    flayout->setVerticalSpacing(50);

    //    paramGroup = new QGroupBox();
    //    paramGroup->setLayout(flayout);
    //    paramGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    cvlayout = new QVBoxLayout();
    cvlayout->addWidget(RecvArea, 7);
    cvlayout->addWidget(SendArea, 2);
    //    RecvArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //    SendArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(HexSend);
    hlayout->addWidget(NewLineBox, 0, Qt::AlignLeft);
    hlayout->addWidget(TimerBox, 0, Qt::AlignRight);
    hlayout->addWidget(TimerSpin, 0, Qt::AlignLeft);
    hlayout->addWidget(SendButton, 0, Qt::AlignRight);

    rvlayout = new QVBoxLayout();
    rvlayout->addWidget(ClearButton, 0, Qt::AlignCenter);
    rvlayout->addWidget(HexRecv);

    layout = new QGridLayout(this);

    layout->addLayout(flayout, 0, 0, 2, 1);
    layout->addLayout(hlayout, 1, 1);
    layout->addLayout(cvlayout, 0, 1, 1, 1);
    layout->addLayout(rvlayout, 0, 2, Qt::AlignTop);

    layout->setColumnMinimumWidth(0, 150);
    layout->setColumnMinimumWidth(2, 100);

    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 0);

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
    emit sendDateTime(QDateTime::currentDateTime().toString());
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
            //只在串口发生变化时刷新ComboBox
            COMList = TmpComList;
            PortNameList = TmpPortNameList;
            DescList = TmpDesList;
            COMBox->setDisabled(false);
            COMBox->clear();
            COMBox->addItems(PortNameList);
            OpenButton->setDisabled(false);
            for (int i = 0; i < COMList.count(); i++)
                COMBox->setItemData(i, DescList[i], Qt::ToolTipRole);
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
    settings.setValue("Baud rate", BaudrateBox->currentIndex());
    settings.setValue("Stop bits", StopbitsBox->currentIndex());
    settings.setValue("Data bits", DatabitsBox->currentIndex());
    settings.setValue("Parity", ParityBox->currentIndex());
}

void MainWidget::serialOpened()
{
    OpenButton->setText(tr("关闭串口"));
    emit sendStatus(QString(/*tr("串口已打开") + */ COMBox->currentText()));
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

QString MainWidget::HexStringToString(QString hexstr)
{
    QStringList list = hexstr.split(' ');
    QByteArray bit;
    foreach (QString str, list)
    {
        if (!str.isEmpty())
            bit.append(str.toInt(nullptr, 16));
    }
    return QString::fromLocal8Bit(bit);
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
