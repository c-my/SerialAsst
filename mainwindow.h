#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QKeyEvent>
#include <mainwidget.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
    void setNewMsg(QString msg);
    void setDateTime(QString datetime);

private:
    MainWidget *widget;
    QStatusBar *statusbar;
    QLabel *timeLabel;
};

#endif // MAINWINDOW_H
