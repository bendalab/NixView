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
    void on_actionTree_triggered();
    void on_actionColumn_triggered();
    void activate_plot(QModelIndex qml);
    void open_file();
    void show_about();
    void show_plot();

signals:
    void emit_view_change(int);

private:
    Ui::MainWindow *ui;
    MainViewWidget* mvw;
    bool mvw_is_set;
    void connect_widgets();
    QModelIndex selected_qml;
};

#endif // MAINWINDOW_HPP
