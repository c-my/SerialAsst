#include "serialcontroller.h"

SerialController::SerialController(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
}

void SerialController::openSerial(QString name)
{
    serial->setPortName(name);
    if(serial->open(QIODevice::ReadWrite))
    {
        //串口打开成功
        emit openSuccess();
    }
    else
    {
        //串口打开失败
        emit openFailed();
    }
}

void SerialController::closeSerial()
{
    if(serial->isOpen())
    {
        serial->close();
        emit closeSuccess();
    }
}

void SerialController::getBaudrate(QString baudrate)
{
    serial->setBaudRate(baudrate.toInt());

}

void SerialController::getStopbits(QString stopbits)
{
    if(stopbits == "1")
        serial->setStopBits(QSerialPort::OneStop);
    else if(stopbits == "1.5")
        serial->setStopBits(QSerialPort::OneAndHalfStop);
    else if(stopbits == "2")
        serial->setStopBits(QSerialPort::TwoStop);
}

void SerialController::getDatabits(QString databits)
{
    serial->setDataBits(QSerialPort::DataBits(databits.toInt()));
}

void SerialController::getParity(QString parity)
{
    if(parity == tr("无"))
        serial->setParity(QSerialPort::NoParity);

    else if(parity == tr("奇校验"))
        serial->setParity(QSerialPort::OddParity);

    else if(parity == tr("偶校验"))
        serial->setParity(QSerialPort::EvenParity);
}
