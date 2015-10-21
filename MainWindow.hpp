#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <nix.hpp>
#include "MainViewWidget.hpp"

class QVariant;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_action_raw_data_triggered();
    void on_action_another_tree_triggered();
    void activate_plot(QVariant var);
    void open_file();
    void show_about();
    void show_plot();

signals:
    void view_requested_raw_data(int);

private:
    Ui::MainWindow *ui;
    MainViewWidget* mvw;
    bool mvw_is_set;
    void connect_widgets();
    QVariant selected_item;
};

#endif // MAINWINDOW_HPP
