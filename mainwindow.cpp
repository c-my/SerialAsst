#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    widget(new MainWidget(this))
{
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);
    setCentralWidget(widget);
    statusbar->showMessage(tr("准备就绪"));
    connect(widget, MainWidget::sendStatus, this, setNewMsg);
    timeLabel = new QLabel(statusbar);
    statusBar()->addPermanentWidget(timeLabel);
    timeLabel->setText(QDateTime::currentDateTime().toString());

    connect(widget, MainWidget::sendDateTime, this, setDateTime);
    resize(900, 600);
}

void MainWindow::setNewMsg(QString msg)
{
    statusBar()->showMessage(msg);
}

void MainWindow::setDateTime(QString datetime)
{
    timeLabel->setText(datetime);
}
