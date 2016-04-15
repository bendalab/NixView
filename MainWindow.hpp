#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>
#include <nix.hpp>
#include "views/MainViewWidget.hpp"
#include "options/OptionsWidget.hpp"

class QVariant;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit MainWindow(QWidget *parent = 0);
    MainWindow(QWidget *parent, QApplication *app);
    ~MainWindow();

public slots:
    void on_actionTree_triggered();
    void on_actionColumn_triggered();
    void on_actionProperties_triggered();
    void nix_model_update(NixDataModel *model);
    void item_selected(QModelIndex qml);
    void open_file();
    void close_file();
    void show_about();
    void show_plot();
    void show_table();
    void file_scan_progress();
    void invalid_file_error();
    void recent_file_update(QStringList files);
    void open_recent_file(QAction* a);

signals:
    void emit_view_change(int);
    void emit_file_opened(QString filename);

private slots:
    void recent_file_selected(QListWidgetItem*);

private:
    Ui::MainWindow *ui;
    OptionsWidget *ow;
    void connect_widgets();
    QModelIndex selected_qml;
    QLabel* file_label;
    QProgressBar* file_progress;
    QStringList recent_files;

    void get_recent_files();
    void populate_recent_file_menu();
    void read_nix_file(QString filename);
};

#endif // MAINWINDOW_HPP
