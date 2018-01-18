#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    widget(new MainWidget(this))
{
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);
    setCentralWidget(widget);
    statusbar->showMessage(tr("准备就绪"));
    connect(widget, MainWidget::sendStatus, this, setNewMsg);
    resize(640, 500);
}

void MainWindow::setNewMsg(QString msg)
{
    statusBar()->showMessage(msg);
    qDebug()<<this->size();
}
