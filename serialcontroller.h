#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

class SerialController : public QObject
{
    Q_OBJECT
public:
    explicit SerialController(QObject *parent = nullptr);

signals:
    void openSuccess();
    void openFailed();
    void closeSuccess();

public slots:
    void openSerial(QString name);
    void closeSerial();
    void getBaudrate(QString baudrate);
    void getStopbits(QString stopbits);
    void getDatabits(QString databits);
    void getParity(QString parity);

private:
    QSerialPort *serial;

};

#endif // SERIALCONTROLLER_H
