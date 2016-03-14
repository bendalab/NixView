#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "aboutdialog.h"
#include "plotdialog.h"
#include "views/RawTreeView.hpp"
#include "nix.hpp"
#include "common/Common.hpp"
#include "model/NixDataModel.hpp"
#include "model/NixModelItem.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    mvw_is_set = false;
    file_label = new QLabel(this);
    file_progress = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(new QLabel("File: ", this));
    ui->statusBar->addPermanentWidget(file_label);
    ui->statusBar->addPermanentWidget(new QLabel("File scanning: ", this));
    ui->statusBar->addPermanentWidget(file_progress, 1);
}

void MainWindow::connect_widgets() {
    QObject::connect(this, SIGNAL(view_requested_raw_data(int)), mvw, SLOT(set_view(int)));
    QObject::connect(mvw->get_rtv()->get_tree_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(activate_plot(QModelIndex, QModelIndex)));
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

void MainWindow::activate_plot(QModelIndex qml_new, QModelIndex) {
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

    selected_qml = qml_new;

    NixModelItem *item = mvw->get_current_model()->get_item_from_qml(qml_new);

    if((strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0) |
            (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0) |
            (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0)){
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
    d.set_entity(selected_qml);
    d.exec();
}


void MainWindow::file_scan_progress()
{
    file_progress->setValue(mvw->get_scan_progress());
    QCoreApplication::processEvents();
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
    file_label->setText(file_path.c_str());
    mvw = new MainViewWidget();
    connect(mvw, SIGNAL(scan_progress_update()), this, SLOT(file_scan_progress()));
    mvw->set_nix_file(file_path);
    ui->main_layout->addWidget(mvw);
    mvw_is_set = true;
    connect_widgets();
}
