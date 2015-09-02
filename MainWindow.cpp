#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "MainViewWidget.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_File_triggered()
{
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

    //test
    MainViewWidget* mvw = new MainViewWidget(file_path);
    this->ui->main_ho_layout->addWidget(mvw);

    //TODO open NIX file
}
