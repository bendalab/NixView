#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>
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
    void activate_plot(QModelIndex qml_new, QModelIndex);
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
    QModelIndex selected_qml;
    QLabel* file_label;
    QProgressBar* file_progress;
};

#endif // MAINWINDOW_HPP
