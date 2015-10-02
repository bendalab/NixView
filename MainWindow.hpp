#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <nix.hpp>
#include "MainViewWidget.hpp"

#include <nix/Block.hpp>
Q_DECLARE_METATYPE(Block)
Q_DECLARE_METATYPE(nix::Section)
Q_DECLARE_METATYPE(nix::Property)
Q_DECLARE_METATYPE(nix::DataArray)
Q_DECLARE_METATYPE(nix::Tag)
Q_DECLARE_METATYPE(nix::MultiTag)

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
    void on_action_raw_data_triggered();
    void on_action_another_tree_triggered();
    void quit();

signals:
    void view_requested_raw_data(int);

private:
    Ui::MainWindow *ui;
    MainViewWidget* mvw;
    bool mvw_is_set;
    void connect_widgets();
};

#endif // MAINWINDOW_HPP
