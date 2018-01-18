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
    SendArea->setFont(QFont(tr("Microsoft YaHei UI"), 13));
    SendArea->installEventFilter(this);

    //按钮
    OpenButton = new QPushButton(tr("打开串口"));
    SendButton = new QPushButton(tr("发送"));
    SendButton->setDisabled(true);
    SendButton->setToolTip(tr("Ctrl+Enter"));

    connect(OpenButton, QPushButton::clicked, this, OpenSerial);

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
    layout->addWidget(SendButton, 5, 2, Qt::AlignRight);
    layout->addWidget(RecvArea, 0, 2, 3, 1);
    layout->addWidget(SendArea, 3, 2, 2, 1);
    setLayout(layout);

    //计时器 初始化
    CheckTimer = new QTimer(this);
    connect(CheckTimer, QTimer::timeout, this, CheckSerials);
    CheckTimer->start(1000);

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
    connect(SendButton, QPushButton::clicked, this, SendContent);
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
    qDebug()<<"打开失败";
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
//    RecvArea->moveCursor(QTextCursor::End);
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

void MainWidget::SendContent()
{
    QString content = SendArea->toPlainText();
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
