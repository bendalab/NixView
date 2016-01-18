#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "aboutdialog.h"
#include "plotdialog.h"
#include "views/RawTreeView.hpp"
#include "nix.hpp"
#include "common/Common.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    mvw_is_set = false;
}

void MainWindow::connect_widgets() {
    QObject::connect(this, SIGNAL(view_requested_raw_data(int)), mvw, SLOT(set_view(int)));
    QObject::connect(mvw->get_rtv(), SIGNAL(item_found(QVariant)), this, SLOT(activate_plot(QVariant)));
}

MainWindow::~MainWindow() {
    delete ui;
}

// slots
void MainWindow::on_action_raw_data_triggered() {
    emit view_requested_raw_data(0);
}

void MainWindow::on_action_another_tree_triggered() {
    emit view_requested_raw_data(1);
}

void MainWindow::activate_plot(QVariant var) {
    QAction* plot_action;
    bool found_action = false;
    QList<QMenu*> list = this->menuBar()->findChildren<QMenu*>(QString("menuPlot"));
    if (list.size() == 1) {
        QMenu* plot_menu = list.at(0);
        QList<QAction*> actions = plot_menu->actions();
        QString name;
        foreach(QAction* a, actions) {
            name = a->objectName();
            if (name.compare("actionPlot") == 0) {
                plot_action = a;
                found_action = true;
            }
        }
    }
    selected_item = var;

    if(var.canConvert<nix::DataArray>() | var.canConvert<nix::MultiTag>() | var.canConvert<nix::Tag>()){
        if (found_action) {
            plot_action->setEnabled(true);
        }
    }
    else {
        if (found_action) {
            plot_action->setEnabled(false);
        }
    }
}


void MainWindow::show_about() {
    AboutDialog d(this);
    d.exec();
}


void MainWindow::show_plot() {
    PlotDialog d(this);
    d.set_entity(selected_item);

    d.exec();
}


void MainWindow::open_file() {
    QFileDialog fd(this);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setNameFilter(tr("NIX File (*.h5)"));
    fd.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fd.exec())
        fileNames = fd.selectedFiles();
    if (fileNames.size() == 0)
        return;

    if (mvw_is_set) {
        ui->main_layout->removeWidget(mvw);
        mvw->deleteLater();
        mvw_is_set = false;
    }

    std::string file_path = fileNames.front().toStdString();
    std::cout << file_path << std::endl;

    mvw = new MainViewWidget(file_path);
    ui->main_layout->addWidget(mvw);
    mvw_is_set = true;

    connect_widgets();
}
