#include <QApplication>
#include <QOperatingSystemVersion>
#include <QtGlobal>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows) {
        QApplication::setFont(QFont{"Segoe UI", 10});
    }
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QString(":/pic/icon/icon.png")));
    a.setStyle("fusion");
    MainWindow w;
    w.show();

    return a.exec();
}
