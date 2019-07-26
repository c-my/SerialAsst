#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          widget(new MainWidget(this)),
                                          menuBar(new QMenuBar(this)),
                                          statusbar(new QStatusBar(this))
{
    setMenuBar(menuBar);
    setStatusBar(statusbar);

    aboutQtAct = new QAction(/*QIcon(":/pic/icon/qt-icon.png"), */tr("&关于Qt"), this);
    aboutAct = new QAction(/*QIcon(":/pic/icon/about.png"), */tr("&关于SerialAsst"), this);
    helpMenu = menuBar->addMenu(tr("帮助(&H)"));
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutAct);
    setCentralWidget(widget);
    statusbar->showMessage(tr("准备就绪"));
    connect(widget, &MainWidget::sendStatus, this, &MainWindow::setNewMsg);
    timeLabel = new QLabel(statusbar);
    statusBar()->addPermanentWidget(timeLabel);
    timeLabel->setText(QDateTime::currentDateTime().toString());

    connect(aboutQtAct, &QAction::triggered, this, &MainWindow::showAboutQt);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
    connect(widget, &MainWidget::sendDateTime, this, &MainWindow::setDateTime);
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

void MainWindow::showAboutQt()
{
      QMessageBox *aboutQtWindow = new QMessageBox(this);
      aboutQtWindow->aboutQt(this);
}

void MainWindow::showAbout()
{
    QMessageBox *aboutWindow = new QMessageBox(this);
    aboutWindow->setStandardButtons(QMessageBox::Ok);
    aboutWindow->setText(tr("<h1>SerialAsst</h1>"
                             "<p>Based on Qt 5.10.1 (") +
                            getCompilerVersion() +
                            tr(")</p>"
                             "Source Code: <a href=\"https://github.com/c-my/SerialAsst\">https://github.com/c-my/SerialAsst</a><br/>"
                            "Email: "
                            "<address><a href=\"mailto:cmy1113@outlook.com?subject=SerialAsst Feedback\">cmy1113@outlook.com</a>"
                            "</address>"));
    aboutWindow->show();
}

QString MainWindow::getCompilerVersion()
{
#ifdef __GNUC__
    return tr("GCC ") + QString::number(__GNUC__)+tr(".")+QString::number(__GNUC_MINOR__)+tr(".")+QString::number(__GNUC_PATCHLEVEL__);
#endif
#ifdef _MSC_VER
    return tr("MSVC ") + QString::number(_MSC_FULL_VER);
#endif
}
