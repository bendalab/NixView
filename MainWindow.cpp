#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mvw_is_set = false;
}

MainWindow::~MainWindow()
{
    delete ui;
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
}
