#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QKeyEvent>
#include <QMessageBox>
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
  void showAboutQt();

private:
  MainWidget *widget;
  QMenuBar *menuBar;
  QMenu *helpMenu;
  QAction *aboutQtAct;
  QStatusBar *statusbar;
  QLabel *timeLabel;

};

#endif // MAINWINDOW_H
