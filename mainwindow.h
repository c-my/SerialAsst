﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

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
  void setNewMsg(QString msg);  //设置状态栏信息
  void setDateTime(QString datetime);   //设置状态栏时间
  void showAboutQt();
  void showAbout();

private:
  MainWidget *widget;
  QMenuBar *menuBar;
  QMenu *helpMenu;
  QAction *aboutQtAct, *aboutAct;
  QStatusBar *statusbar;
  QLabel *timeLabel;

  QString getCompilerVersion();

};

#endif // MAINWINDOW_H
