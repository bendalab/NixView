#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mvw_is_set = false;
}

void MainWindow::connect_widgets()
{
    QObject::connect(this, SIGNAL(view_requested_raw_data(int)), mvw, SLOT(set_view(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// slots
void MainWindow::on_action_raw_data_triggered()
{
    emit view_requested_raw_data(0);
}

void MainWindow::on_action_another_tree_triggered()
{
    emit view_requested_raw_data(1);
}

void MainWindow::on_action_Open_File_triggered()
{
    if (mvw_is_set)
    {
        ui->main_ho_layout->removeWidget(mvw);
        mvw->deleteLater();
    }

    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setNameFilter(tr("NIX File (*.h5)"));
    fd.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fd.exec())
        fileNames = fd.selectedFiles();
    if (fileNames.size() == 0)
        return;
    std::string file_path = fileNames.front().toStdString();
    std::cout << file_path << std::endl;

    mvw = new MainViewWidget(file_path);
    ui->main_ho_layout->addWidget(mvw);
    mvw_is_set = true;

    connect_widgets();
}

void MainWindow::quit() {
    this->close();
}
