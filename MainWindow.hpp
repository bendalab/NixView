#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <iostream>
#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>
#include <nix.hpp>
#include "views/MainViewWidget.hpp"

class QVariant;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent, QApplication *app);
    ~MainWindow();

    void read_nix_file(QString filename);

public slots:
    void on_actionTree_triggered();
    void on_actionColumn_triggered();
    void item_selected(QModelIndex qml);
    void open_file();
    void close_file();
    void show_about();
    void show_plot();
    void show_table();
    void show_options();
    void show_file_properties();
    void visible_columns_update(QString who, QString column, bool state);
    void file_scan_progress();
    void invalid_file_error();
    void recent_file_update(QStringList files);
    void open_recent_file(QAction* a);
    void find();
    void closeSearch();
    void clearSearch();
    void newSearchResults(std::vector<QVariant>);
    void searchResultSelected();

signals:
    void emit_view_change(int);

private slots:
    void recent_file_selected(QListWidgetItem*);

private:
    Ui::MainWindow *ui;
    void connect_widgets();
    QModelIndex selected_qml;
    QLabel* file_label;
    QProgressBar* file_progress;
    QStringList recent_files;
    int previous_page;
    void get_recent_files();
    void populate_recent_file_menu();

    void update_file_list(QString filename);
    void save_recent_files(QStringList &files);
};

#endif // MAINWINDOW_HPP
