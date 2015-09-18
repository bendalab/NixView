#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <nix.hpp>
#include "MainViewWidget.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Open_File_triggered();

private:
    Ui::MainWindow *ui;
    MainViewWidget* mvw;
};

#endif // MAINWINDOW_HPP
