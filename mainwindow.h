#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <mainwidget.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
    void setNewMsg(QString msg);

private:
    MainWidget *widget;
    QStatusBar *statusbar;
};

#endif // MAINWINDOW_H
